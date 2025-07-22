#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <thread>
#include <memory>

#include "tctfiles.hpp"
#include "tctutils.hpp"

#ifdef __cplusplus
extern "C" {
#endif
#include "miniaudio.h"
#ifdef __cplusplus
}
#endif

namespace tct {

/// @brief  Audio file type.
enum class AudioFileType { NONE, MUSIC, SFX, DIALOGUE };
enum class AudioRequestType { STOP, PLAY, SET_VOLUME, FADE_IN, FADE_OUT, COUNT };

constexpr const std::size_t audReqBufSz = 5;
constexpr const std::size_t concurrentAud = 10;

struct AudioRequest {
    std::weak_ptr<Asset> asset;
    AudioRequestType reqType;
    AudioFileType type;
    bool loop;
    float volume;
    std::chrono::duration<float> fadeDuration;
};

class Audio {
   private:
    ma_engine engine;
    std::condition_variable cv{};
    std::atomic<bool> terminate{false};
    std::mutex reqMutex{};
    std::size_t reqIdx{};
    std::size_t audProcIdx{};
    std::array<AudioRequest, audReqBufSz> reqRBuf;
    std::thread audThread{};
    void audThreadExec();
    void sendRequest(const AudioRequest request) noexcept;

   public:
    Audio();

    // You don't wanna move or copy this thing at all.
    Audio(Audio&) = delete;
    Audio(Audio&&) = delete;
    Audio& operator=(Audio&) = delete;
    Audio& operator=(Audio&&) = delete;

    void playFile(std::weak_ptr<Asset> asset, const AudioFileType fileType, const float volume = 1.0f,
                  const bool looping = false) noexcept;
    void playFileFadeIn(std::weak_ptr<Asset> asset, const AudioFileType fileType, const float fadeSeconds = 1.0f,
                        const float volume = 1.0f, const bool looping = false) noexcept;
    void setVolumeFile(std::weak_ptr<Asset> asset, const float volume = 1.0f) noexcept;
    void setVolumeType(const AudioFileType fileType, const float volume = 1.0f) noexcept;
    void stopFile(std::weak_ptr<Asset> asset) noexcept;
    void stopFileFadeOut(std::weak_ptr<Asset> asset, const float fadeSeconds = 1.0f) noexcept;
    void stopType(const AudioFileType fileType) noexcept;
    void stopTypeFadeOut(const AudioFileType fileType, const float fadeSeconds = 1.0f) noexcept;
    ~Audio();
};

} // namespace tct