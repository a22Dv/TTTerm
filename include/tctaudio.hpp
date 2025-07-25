#pragma once

#include "miniaudio.h"
#include <array>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <optional>
#include <chrono>

namespace tct {

struct Asset;
enum class AudioType : uint8_t;
constexpr const std::size_t requestLimit = 5;
constexpr const std::size_t concurrentAudio = 10;

enum class AudioRequestType : uint8_t {
    PLAY_FILE,
    PLAY_FILE_FADE,
    STOP_FILE,
    STOP_FILE_FADE,
    STOP_TYPE,
    STOP_TYPE_FADE
};

struct AudioRequest {
    AudioRequestType requestType;
    std::optional<bool> looping{};
    std::optional<float> volume{};
    std::optional<std::chrono::duration<float>> fadeDuration{};
    std::optional<std::weak_ptr<Asset>> asset{};
    std::optional<AudioType> aType{};
};

class Audio {
  private:
    std::atomic<bool> terminate{};
    bool engineInitialized{};
    ma_engine audioEngine{};
    std::thread audThread{};
    std::mutex requestMutex{};
    std::condition_variable threadCv{};
    std::array<AudioRequest, requestLimit> requests{};
    std::size_t requestHead{};
    std::size_t requestTail{};
    void audThreadExec();
    void sendRequest(const AudioRequest request);
  public:
    Audio();
    ~Audio();
    Audio(const Audio &) = delete;
    Audio &operator=(const Audio &) = delete;
    Audio(Audio &&) = delete;
    Audio &operator=(Audio &&) noexcept = delete;
    
    void playFile(const std::weak_ptr<Asset> audioAsset, const float volume = 1.0f, const bool looping = false);
    void playFileFade(const std::weak_ptr<Asset> audioAsset, const float fadeDuration = 1.0f, const float volume = 1.0f, const bool looping = false);
    void stopFile(const std::weak_ptr<Asset> audioAsset);
    void stopFileFade(const std::weak_ptr<Asset> audioAsset, const float fadeDuration = 1.0f);
    void stopType(const AudioType aType);
    void stopTypeFade(const AudioType aType, const float fadeDuration = 1.0f);
};

} // namespace tct