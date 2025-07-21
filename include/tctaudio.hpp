#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <filesystem>
#include <mutex>
#include <thread>

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
namespace fs = std::filesystem;

/// @brief  Audio file type.
/// @note DO NOT USE NULL_TYPE. Use NONE to ignore choices.
enum class AudioFileType { NONE, MUSIC, SFX, DIALOGUE, NULL_TYPE };

/// @brief Audio file ID.
/// @note DO NOT USE NULL_TYPE. Use NONE to ignore choices.
enum class AudioFileID {
#define X(id, pth) id,
    AUDIO_FILES COUNT,
    NONE,
    NULL_FILE,
#undef X
};
enum class AudioRequestType { STOP, PLAY, SET_VOLUME, FADE_IN, FADE_OUT, COUNT };

constexpr const size_t audReqBufSz = 5;
constexpr const size_t concurrentAud = 10;
constexpr const char* getPathAudioId(const AudioFileID fileId) {
    constexpr const char* paths[] = {
#define X(id, pth) pth,
        AUDIO_FILES
#undef X
    };
    return paths[static_cast<size_t>(fileId)];
}

struct AudioRequest {
    AudioRequestType reqType;
    AudioFileID id;
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
    size_t reqIdx{};
    size_t audProcIdx{};
    std::array<AudioRequest, audReqBufSz> reqRBuf{};
    std::thread audThread{};
    void audThreadExec();
    void sendRequest(const AudioRequest request) noexcept;

   public:
    Audio();
    void playFile(const AudioFileID fileId, const AudioFileType fileType, const float volume = 1.0f,
                  const bool looping = false) noexcept;
    void playFileFadeIn(const AudioFileID fileId, const AudioFileType fileType, const float fadeSeconds = 1.0f,
                        const float volume = 1.0f, const bool looping = false) noexcept;
    void setVolumeFile(const AudioFileID fileId, const float volume = 1.0f) noexcept;
    void setVolumeType(const AudioFileType fileType, const float volume = 1.0f) noexcept;
    void stopFile(const AudioFileID fileId) noexcept;
    void stopFileFadeOut(const AudioFileID fileId, const float fadeSeconds = 1.0f) noexcept;
    void stopType(const AudioFileType fileType) noexcept;
    void stopTypeFadeOut(const AudioFileType fileType, const float fadeSeconds = 1.0f) noexcept;
    ~Audio();
};

} // namespace tct