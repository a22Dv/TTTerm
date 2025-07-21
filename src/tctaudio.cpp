#define MINIAUDIO_IMPLEMENTATION

#include "tctaudio.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

tct::Audio::Audio() {
    ma_result maRet = ma_engine_init(NULL, &engine);
    tct::check(maRet == MA_SUCCESS, "AUDIO INITIALIZATION FAILURE.");
    audThread = std::thread([this]() { this->audThreadExec(); });
}

tct::Audio::~Audio() {
    terminate.store(true);
    cv.notify_one();
    audThread.join();
    ma_engine_uninit(&engine);
}

/*
    Helper functions to simplify API and handle request creation.
*/

void tct::Audio::playFile(const tct::AudioFileID fileId, const tct::AudioFileType fileType, const float volume, const bool looping) noexcept {
    if (fileId == tct::AudioFileID::NONE || fileId == tct::AudioFileID::NULL_FILE) {
        return;
    }
    tct::Audio::sendRequest(tct::AudioRequest{.reqType = tct::AudioRequestType::PLAY,
                                              .id = fileId,
                                              .type = fileType,
                                              .loop = looping,
                                              .volume = volume,
                                              .fadeDuration = std::chrono::duration<float>(0)});
}
void tct::Audio::playFileFadeIn(const tct::AudioFileID fileId,const tct::AudioFileType fileType, const float fadeSeconds,
                                const float volume, const bool looping) noexcept {
    if (fileId == tct::AudioFileID::NONE || fileId == tct::AudioFileID::NULL_FILE) {
        return;
    }
    tct::Audio::sendRequest(tct::AudioRequest{.reqType = tct::AudioRequestType::FADE_IN,
                                              .id = fileId,
                                              .type = fileType,
                                              .loop = looping,
                                              .volume = volume,
                                              .fadeDuration = std::chrono::duration<float>(fadeSeconds)});
}
void tct::Audio::setVolumeFile(const tct::AudioFileID fileId, const float volume) noexcept {
    if (fileId == tct::AudioFileID::NONE || fileId == tct::AudioFileID::NULL_FILE) {
        return;
    }
    tct::Audio::sendRequest(tct::AudioRequest{.reqType = tct::AudioRequestType::SET_VOLUME,
                                              .id = fileId,
                                              .type = tct::AudioFileType::NONE,
                                              .loop = false,
                                              .volume = volume,
                                              .fadeDuration = std::chrono::duration<float>(0)});
}
void tct::Audio::setVolumeType(const tct::AudioFileType fileType, const float volume) noexcept {
    if (fileType == tct::AudioFileType::NONE || fileType == tct::AudioFileType::NULL_TYPE) {
        return;
    }
    tct::Audio::sendRequest(tct::AudioRequest{.reqType = tct::AudioRequestType::SET_VOLUME,
                                              .id = tct::AudioFileID::NONE,
                                              .type = fileType,
                                              .loop = false,
                                              .volume = volume,
                                              .fadeDuration = std::chrono::duration<float>(0)});
}
void tct::Audio::stopFile(const tct::AudioFileID fileId) noexcept {
    if (fileId == tct::AudioFileID::NONE || fileId == tct::AudioFileID::NULL_FILE) {
        return;
    }
    tct::Audio::sendRequest(tct::AudioRequest{.reqType = tct::AudioRequestType::STOP,
                                              .id = fileId,
                                              .type = tct::AudioFileType::NONE,
                                              .loop = false,
                                              .volume = 0.0f,
                                              .fadeDuration = std::chrono::duration<float>(0)});
}
void tct::Audio::stopFileFadeOut(const tct::AudioFileID fileId, const float fadeSeconds) noexcept {
    if (fileId == tct::AudioFileID::NONE || fileId == tct::AudioFileID::NULL_FILE) {
        return;
    }
    tct::Audio::sendRequest(tct::AudioRequest{.reqType = tct::AudioRequestType::FADE_OUT,
                                              .id = fileId,
                                              .type = tct::AudioFileType::NONE,
                                              .loop = false,
                                              .volume = 0.0f,
                                              .fadeDuration = std::chrono::duration<float>(1)});
}
void tct::Audio::stopType(const tct::AudioFileType fileType) noexcept {
    if (fileType == tct::AudioFileType::NONE || fileType == tct::AudioFileType::NULL_TYPE) {
        return;
    }
    tct::Audio::sendRequest(tct::AudioRequest{.reqType = tct::AudioRequestType::STOP,
                                              .id = tct::AudioFileID::NONE,
                                              .type = fileType,
                                              .loop = false,
                                              .volume = 0.0f,
                                              .fadeDuration = std::chrono::duration<float>(1)});
}
void tct::Audio::stopTypeFadeOut(const tct::AudioFileType fileType, const float fadeSeconds) noexcept {
    if (fileType == tct::AudioFileType::NONE || fileType == tct::AudioFileType::NULL_TYPE) {
        return;
    }
    tct::Audio::sendRequest(tct::AudioRequest{.reqType = tct::AudioRequestType::FADE_OUT,
                                              .id = tct::AudioFileID::NONE,
                                              .type = fileType,
                                              .loop = false,
                                              .volume = 0.0f,
                                              .fadeDuration = std::chrono::duration<float>(1)});
}

enum class FadeType : uint8_t { NONE, FADE_IN, FADE_OUT };

struct SoundInstance {
    bool active{};
    float cVol{};
    float tVol{};
    float fRate{};
    std::chrono::duration<float> fadeDuration;
    FadeType fadeType{FadeType::NONE};
    tct::AudioFileID fileId{tct::AudioFileID::NULL_FILE};
    tct::AudioFileType fileType{tct::AudioFileType::NULL_TYPE};
    ma_sound sound;
};

/*
    For these functions while you could technically give a "valid indices" array to avoid
    looping for at most 10 times to look for a suitable spot, that operation is fast enough
    and really won't be noticed for just 10 loops.
*/

static void playMaSound(ma_engine& engine, std::array<SoundInstance, tct::concurrentAud>& sounds,
                        const tct::AudioRequest req) {
    for (SoundInstance& sound : sounds) {
        if (sound.active) {
            continue;
        }
        sound.active = true;
        sound.fileId = req.id;
        sound.fileType = req.type;
        sound.cVol = req.volume;
        ma_result mSnd =
            ma_sound_init_from_file(&engine, tct::getPathAudioId(sound.fileId), 0, NULL, NULL, &sound.sound);
        tct::check(mSnd == MA_SUCCESS, "Error initializing sound.");
        ma_sound_set_volume(&sound.sound, req.volume);
        ma_sound_set_looping(&sound.sound, req.loop);
        ma_result mSndStart = ma_sound_start(&sound.sound);
        tct::check(mSndStart == MA_SUCCESS, "Error playing sound.");
        break;
    }
}

static void stopMaSound(ma_engine& engine, std::array<SoundInstance, tct::concurrentAud>& sounds,
                        const tct::AudioRequest req) {
    for (SoundInstance& sound : sounds) {
        if (!sound.active) {
            continue;
        }
        // This is a coarse condition that stops all concurrent running sounds of the same type or file ID.
        if (req.id != sound.fileId && req.type != sound.fileType) {
            continue;
        }
        ma_result mSndStop = ma_sound_stop(&sound.sound);
        tct::check(mSndStop == MA_SUCCESS, "Error stopping sound.");
        ma_sound_uninit(&sound.sound);
        sound.active = false;
    }
}

static void setVolMaSound(ma_engine& engine, std::array<SoundInstance, tct::concurrentAud>& sounds,
                          const tct::AudioRequest req) {
    for (SoundInstance& sound : sounds) {
        if (!sound.active) {
            continue;
        }
        // This is a coarse condition that sets all concurrent running sounds of the same type or file ID.
        if (req.id != sound.fileId && req.type != sound.fileType) {
            continue;
        }
        ma_sound_set_volume(&sound.sound, req.volume);
    }
}

static void startFadeInMaSound(ma_engine& engine, std::array<SoundInstance, tct::concurrentAud>& sounds,
                               const tct::AudioRequest req) {
    for (SoundInstance& sound : sounds) {
        if (sound.active) {
            continue;
        }
        sound.active = true;
        sound.fileId = req.id;
        sound.fileType = req.type;
        sound.fadeType = FadeType::FADE_IN;
        sound.fadeDuration = req.fadeDuration;
        sound.cVol = 0.0;
        sound.tVol = req.volume;
        sound.fRate = (sound.tVol - sound.cVol) / sound.fadeDuration.count();

        ma_result mSnd =
            ma_sound_init_from_file(&engine, tct::getPathAudioId(sound.fileId), 0, NULL, NULL, &sound.sound);
        tct::check(mSnd == MA_SUCCESS, "Error initializing sound.");
        ma_sound_set_volume(&sound.sound, 0.0);
        ma_sound_set_looping(&sound.sound, req.loop);
        ma_result mSndStart = ma_sound_start(&sound.sound);
        tct::check(mSndStart == MA_SUCCESS, "Error playing sound.");
        break;
    }
}

static void startFadeOutMaSound(ma_engine& engine, std::array<SoundInstance, tct::concurrentAud>& sounds,
                                const tct::AudioRequest req) {
    for (SoundInstance& sound : sounds) {
        if (sound.fileId != req.id && sound.fileType != req.type) {
            continue;
        }
        sound.fadeType = FadeType::FADE_OUT;
        sound.cVol = ma_sound_get_volume(&sound.sound);
        sound.tVol = 0.0;
        sound.fadeDuration = req.fadeDuration;
        sound.fRate = (sound.tVol - sound.cVol) / sound.fadeDuration.count();
    }
}

static void updateFades(std::array<SoundInstance, tct::concurrentAud>& sounds, std::chrono::duration<float> delta) {
    static const std::chrono::duration<float> end = std::chrono::duration<float>(0.0);
    for (SoundInstance& sound : sounds) {
        if (!sound.active) {
            continue;
        }
        if (sound.fadeType == FadeType::NONE) {
            continue;
        }
        bool hasEnded{};
        if (sound.fadeType == FadeType::FADE_IN && sound.tVol <= sound.cVol ||
            sound.fadeType == FadeType::FADE_OUT && sound.tVol >= sound.cVol) {
            if (sound.fadeType == FadeType::FADE_OUT) {
                hasEnded = true;
            }
            sound.cVol = sound.tVol;
            sound.fadeType = FadeType::NONE;
        } else {
            sound.cVol += sound.fRate * delta.count();
        }
        ma_sound_set_volume(&sound.sound, sound.cVol);
        if (!hasEnded) {
            continue;
        }
        ma_result mSndStop = ma_sound_stop(&sound.sound);
        tct::check(mSndStop == MA_SUCCESS, "Error stopping sound.");
        ma_sound_uninit(&sound.sound);
        sound.active = false;
        sound.fileId = tct::AudioFileID::NULL_FILE;
        sound.fileType = tct::AudioFileType::NULL_TYPE;
    }
}

static void cleanupSoundInstances(std::array<SoundInstance, tct::concurrentAud>& sounds, bool unconditional) {
    for (SoundInstance& sound : sounds) {
        if (!sound.active) {
            continue;
        }
        if (!(ma_sound_at_end(&sound.sound) || unconditional)) {
            continue;
        }
        ma_sound_stop(&sound.sound);
        ma_sound_uninit(&sound.sound);
        sound.active = false;
    }
}

using DispatchAudioArray = std::array<
    std::function<void(ma_engine& engine, std::array<SoundInstance, tct::concurrentAud>&, const tct::AudioRequest)>,
    static_cast<std::size_t>(tct::AudioRequestType::COUNT)>;

void tct::Audio::audThreadExec() {
    std::array<SoundInstance, tct::concurrentAud> sounds{};
    std::array<tct::AudioRequest, tct::audReqBufSz> requests{};
    std::size_t cRequests = 0;
    DispatchAudioArray dispatch{};

    dispatch[static_cast<std::size_t>(tct::AudioRequestType::PLAY)] = playMaSound;
    dispatch[static_cast<std::size_t>(tct::AudioRequestType::SET_VOLUME)] = setVolMaSound;
    dispatch[static_cast<std::size_t>(tct::AudioRequestType::STOP)] = stopMaSound;
    dispatch[static_cast<std::size_t>(tct::AudioRequestType::FADE_IN)] = startFadeInMaSound;
    dispatch[static_cast<std::size_t>(tct::AudioRequestType::FADE_OUT)] = startFadeOutMaSound;
    std::chrono::steady_clock::time_point cTime{std::chrono::steady_clock::now()};
    while (true) {
        {
            std::unique_lock<std::mutex> lock(reqMutex);
            cv.wait_for(lock, std::chrono::milliseconds(16),
                        [this] { return terminate.load() || reqIdx != audProcIdx; });
            if (terminate.load() && reqIdx == audProcIdx) break;
            while (audProcIdx != reqIdx) {
                requests[cRequests] = reqRBuf[audProcIdx];
                cRequests++;
                audProcIdx = (audProcIdx + 1) % audReqBufSz;
            }
        }
        const std::chrono::steady_clock::time_point nTime = std::chrono::steady_clock::now();
        const std::chrono::duration<float> delta = nTime - cTime;
        cTime = nTime;
        updateFades(sounds, delta);
        cleanupSoundInstances(sounds, false);
        for (std::size_t req = 0; req < cRequests; ++req) {
            dispatch[static_cast<std::size_t>(requests[req].reqType)](engine, sounds, requests[req]);
        }
        cRequests = 0;
    }
    cleanupSoundInstances(sounds, true);
}

void tct::Audio::sendRequest(const tct::AudioRequest request) noexcept {
    // Invalid requests.
    if (request.id == tct::AudioFileID::NULL_FILE || request.type == tct::AudioFileType::NULL_TYPE ||
        request.id == tct::AudioFileID::NONE && request.type == tct::AudioFileType::NONE ||
        request.reqType == tct::AudioRequestType::PLAY && request.id == tct::AudioFileID::NONE ||
        request.reqType == tct::AudioRequestType::FADE_IN && request.id == tct::AudioFileID::NONE) {
        return;
    }
    {
        std::unique_lock<std::mutex> lock(reqMutex);

        // We just drop these requests. It's for a mini game engine
        // anyways. Doubt we'd need more.
        if ((reqIdx + 1) % audReqBufSz == audProcIdx) {
            return;
        }
        reqRBuf[reqIdx] = request;
        reqIdx = (reqIdx + 1) % audReqBufSz;
    }
    cv.notify_one();
}
