#define MINIAUDIO_IMPLEMENTATION
#define NOMINMAX
#include "tctaudio.hpp"
#include "miniaudio.h"
#include "tctassets.hpp"
#include "tctutils.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>

namespace tct {
namespace {

enum class FadeType : std::uint8_t { NONE, FADE_IN, FADE_OUT };

struct AudioInstance {
    std::uint8_t initStatus{};
    ma_decoder decoder{};
    ma_sound sound{};
    bool looping{};
    FadeType fType{};
    float fRate{};
    float cVol{};
    float tVol{};
    std::shared_ptr<Asset> asset;
    AudioInstance(ma_engine *audioEngine, const AudioRequest &aReq);
    AudioInstance(const AudioInstance &) = delete;
    AudioInstance &operator=(const AudioInstance &) = delete;
    AudioInstance(AudioInstance &&) = delete;
    AudioInstance &operator=(AudioInstance &&) = delete;
    ~AudioInstance();
};

/*
    Constructor is not responsible for interpreting the request.
    It only does enough to setup the instance itself.
*/

AudioInstance::AudioInstance(ma_engine *audioEngine, const AudioRequest &aReq) {
    check(audioEngine != nullptr, "Null ma_engine* passed to AudioInstance constructor.");
    check(aReq.asset.has_value(), "No resource given to initialize AudioInstance.");
    std::shared_ptr<Asset> reqAsset{aReq.asset->lock()};
    check(reqAsset != nullptr, "Resource released before acquisition.");
    ma_result decoderInit{ma_decoder_init_memory(
        static_cast<void *>(reqAsset->assetData.data()), reqAsset->assetData.size(), nullptr, &decoder
    )};
    check(decoderInit == MA_SUCCESS, "Decoder failed to initialize.");
    initStatus |= 1;
    ma_result soundInit{
        ma_sound_init_from_data_source(audioEngine, static_cast<ma_data_source *>(&decoder), 0, nullptr, &sound)
    };
    check(soundInit == MA_SUCCESS, "Sound failed to initialize.");
    initStatus |= 1 << 1;
    asset = reqAsset;
}

AudioInstance::~AudioInstance() {
    if (initStatus & 1 << 1) {
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
    }
    if (initStatus & 1) {
        ma_decoder_uninit(&decoder);
    }
}

using AudioInstancesArray = std::array<std::optional<AudioInstance>, concurrentAudio>;
void playF(const AudioRequest &aReq, ma_engine *audioEngine, AudioInstancesArray &instances) {
    check(aReq.asset.has_value(), "No resource given for play request.");
    std::shared_ptr<Asset> reqAsset = aReq.asset->lock();
    check(reqAsset != nullptr, "Resource released before acquisition.");
    const bool shouldFade = aReq.requestType == AudioRequestType::PLAY_FILE_FADE;
    for (std::optional<AudioInstance> &instance : instances) {
        if (instance.has_value()) {
            continue;
        }
        instance.emplace(audioEngine, aReq);
        instance->looping = aReq.looping.value();
        instance->cVol = aReq.volume.value();
        instance->tVol = aReq.volume.value();
        if (shouldFade) {
            instance->cVol = 0.0f;
            instance->fType = FadeType::FADE_IN;
            instance->fRate = (instance->tVol - instance->cVol) / std::max(0.1f, aReq.fadeDuration.value().count());
        }
        ma_sound_set_looping(&instance->sound, instance->looping);
        ma_sound_set_volume(&instance->sound, instance->cVol);
        ma_sound_start(&instance->sound);
        break;
    }
}

void startFadeOut(AudioInstance &instance, const float fDuration) {
    instance.cVol = ma_sound_get_volume(&instance.sound);
    instance.tVol = 0.0f;
    instance.fType = FadeType::FADE_OUT;
    instance.fRate = (instance.tVol - instance.cVol) / fDuration;
}

void stopF(const AudioRequest &aReq, AudioInstancesArray &instances) {
    check(aReq.asset.has_value(), "No resource given for stop request");
    std::shared_ptr<Asset> reqAsset = aReq.asset->lock();
    check(reqAsset != nullptr, "Resource released before acquisition.");
    AssetId reqId = reqAsset->assetId;
    const bool shouldFade = aReq.requestType == AudioRequestType::STOP_FILE_FADE;
    for (std::optional<AudioInstance> &instance : instances) {
        if (!instance.has_value()) {
            continue;
        }
        if (instance->asset->assetId != reqId) {
            continue;
        }
        if (shouldFade) {
            startFadeOut(*instance, aReq.fadeDuration.value().count());
            continue;
        }
        instance.reset();
    }
}

void stopT(const AudioRequest &aReq, AudioInstancesArray &instances) {
    const bool shouldFade = aReq.requestType == AudioRequestType::STOP_TYPE_FADE;
    for (std::optional<AudioInstance> &instance : instances) {
        if (!instance.has_value()) {
            continue;
        }
        AudioType aType = std::get<AudioMetadata>(instance->asset->assetMetadata).audioType;
        if (aType != aReq.aType.value()) {
            continue;
        }
        if (shouldFade) {
            startFadeOut(*instance, aReq.fadeDuration.value().count());
            continue;
        }
        instance.reset();
    }
}

void updateInstances(AudioInstancesArray &instances, std::chrono::duration<float> delta) {
    for (std::optional<AudioInstance> &instance : instances) {
        if (!instance.has_value()) {
            continue;
        }
        if (ma_sound_at_end(&instance->sound)) {
            instance.reset();
            continue;
        }
        if (instance->fType == FadeType::NONE) {
            continue;
        }
        const float iCVol{instance->cVol};
        const float iTVol{instance->tVol};
        const FadeType iFType{instance->fType};
        if (iCVol >= iTVol && iFType == FadeType::FADE_IN || iCVol <= iTVol && iFType == FadeType::FADE_OUT) {
            instance->fType = FadeType::NONE;
            instance->cVol = instance->tVol;
            instance->fRate = 0.0f;
            if (iFType == FadeType::FADE_OUT) {
                instance.reset();
            }
            continue;
        }
        instance->cVol += instance->fRate * delta.count();
        ma_sound_set_volume(&instance->sound, instance->cVol);
    }
}

} // namespace

Audio::Audio() {
    ma_result engineInit{ma_engine_init(nullptr, &audioEngine)};
    check(engineInit == MA_SUCCESS, "Miniaudio engine initialization failure.");
    audThread = std::thread([this](){ audThreadExec(); });
}

Audio::~Audio() {
    terminate.store(true);
    if (audThread.joinable()) {
        audThread.join();
    }
    ma_engine_uninit(&audioEngine);
}

void Audio::audThreadExec() {
    AudioInstancesArray instances{};
    std::array<AudioRequest, requestLimit> pendingRequests{};
    std::size_t pendingCount{};
    std::chrono::steady_clock::time_point cTime{std::chrono::steady_clock::now()};
    while (true) {
        {
            std::unique_lock<std::mutex> acquireLock{requestMutex};
            threadCv.wait_for(acquireLock, std::chrono::milliseconds{16}, [this]() {
                return requestTail != requestHead || terminate.load();
            });
            if (terminate.load()) {
                break;
            }
            while (requestTail != requestHead) {
                std::size_t nTail = (requestTail + 1) % requestLimit;
                pendingRequests[pendingCount] = requests[requestTail];
                requestTail = nTail;
                pendingCount++;
            }
        }
        std::chrono::duration<float> delta = std::chrono::steady_clock::now() - cTime;
        cTime = std::chrono::steady_clock::now();
        updateInstances(instances, delta);
        for (std::size_t i = 0; i < pendingCount; ++i) {
            using AReqType = AudioRequestType;
            switch (pendingRequests[i].requestType) {
            case AReqType::PLAY_FILE: [[fallthrough]];
            case AReqType::PLAY_FILE_FADE: playF(pendingRequests[i], &audioEngine, instances); break;
            case AReqType::STOP_FILE: [[fallthrough]];
            case AReqType::STOP_FILE_FADE: stopF(pendingRequests[i], instances); break;
            case AReqType::STOP_TYPE: [[fallthrough]];
            case AReqType::STOP_TYPE_FADE: stopT(pendingRequests[i], instances); break;
            default: break;
            }
        }
        pendingCount = 0;
        if (terminate.load()) {
            break;
        }
    }
}

void Audio::sendRequest(const AudioRequest request) {
    {
        std::unique_lock<std::mutex> acquireLock{requestMutex};
        const std::size_t nHead = (requestHead + 1) % requestLimit;
        if (nHead == requestTail) {
            return; // Drop request.
        }
        requests[requestHead] = request;
        requestHead = nHead;
    }
    threadCv.notify_one();
}

void Audio::playFile(const std::weak_ptr<Asset> audioAsset, const float volume, const bool looping) {
    AudioRequest request{};
    request.requestType = AudioRequestType::PLAY_FILE;
    request.looping = looping;
    request.volume = volume;
    request.asset = audioAsset;
    sendRequest(request);
}

void Audio::playFileFade(
    const std::weak_ptr<Asset> audioAsset, const float fadeDuration, const float volume, const bool looping
) {
    AudioRequest request{};
    request.requestType = AudioRequestType::PLAY_FILE_FADE;
    request.looping = looping;
    request.volume = volume;
    request.asset = audioAsset;
    request.fadeDuration = std::chrono::duration<float>(fadeDuration);
    sendRequest(request);
}

void Audio::stopFile(const std::weak_ptr<Asset> audioAsset) {
    AudioRequest request{};
    request.requestType = AudioRequestType::STOP_FILE;
    request.asset = audioAsset;
    sendRequest(request);
}

void Audio::stopFileFade(const std::weak_ptr<Asset> audioAsset, const float fadeDuration) {
    AudioRequest request{};
    request.requestType = AudioRequestType::STOP_FILE_FADE;
    request.asset = audioAsset;
    request.fadeDuration = std::chrono::duration<float>(fadeDuration);
    sendRequest(request);
}

void Audio::stopType(const AudioType aType) {
    AudioRequest request{};
    request.requestType = AudioRequestType::STOP_TYPE;
    request.aType = aType;
    sendRequest(request);
}

void Audio::stopTypeFade(const AudioType aType, const float fadeDuration) {
    AudioRequest request{};
    request.requestType = AudioRequestType::STOP_TYPE_FADE;
    request.aType = aType;
    request.fadeDuration = std::chrono::duration<float>(fadeDuration);
    sendRequest(request);
}

} // namespace tct