#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <array>
#include <cstddef>
#include <filesystem>
#include <iterator>
#include <memory>
#include <variant>
#include <vector>

#include "tctutils.hpp"

#define AUDIO_ASSETS                                                                                                   \
    X(SELECT_MENU, SFX, "audio/select_menu.wav")                                                                       \
    X(MENU_BGM, MUSIC, "audio/xDeviruchi - Title Theme (Loop).wav")                                                    \
    X(ENTER_MENU, MUSIC, "audio/menu_enter.wav")                                                                       \
    X(ERROR_SFX, SFX, "audio/err.wav")                                                                                 \
    X(VICTORY_SFX, SFX, "audio/victory.wav")                                                                           \
    X(DEFEAT_SFX, SFX, "audio/defeat.wav")                                                                             \
    X(TIE_SFX, SFX, "audio/tie.wav")                                                                                   \
    X(EASY_BGM, MUSIC, "audio/xDeviruchi - Mysterious Dungeon.wav")                                                    \
    X(NORMAL_BGM, MUSIC, "audio/xDeviruchi - Exploring the Unknown.wav")                                               \
    X(HARD_BGM, MUSIC, "audio/xDeviruchi - Prepare for Battle!.wav")                                                   \
    X(UNBEATABLE_BGM, MUSIC, "audio/xDeviruchi - Decisive Battle.wav")

#define IMAGE_ASSETS                                                                                                   \
    X(TEST_IMAGE, "images/test.png")                                                                                   \
    X(AI_MENU_EASY, "images/ai_menu_easy.png")                                                                         \
    X(AI_MENU_NORMAL, "images/ai_menu_normal.png")                                                                     \
    X(AI_MENU_HARD, "images/ai_menu_hard.png")                                                                         \
    X(AI_MENU_UNBEATABLE, "images/ai_menu_unbeatable.png")                                                             \
    X(SELECTOR_MENU, "images/selector_menu.png")                                                                       \
    X(TITLE, "images/title.png")                                                                                       \
    X(BORDER, "images/BORDER.png")                                                                                     \
    X(EASY, "images/EASY.png")                                                                                         \
    X(NORMAL, "images/NORMAL.png")                                                                                     \
    X(HARD, "images/HARD.png")                                                                                         \
    X(UNBEATABLE, "images/UNBEATABLE.png")                                                                             \
    X(VERSION, "images/VERSION.png")                                                                                   \
    X(X_CHAR, "images/X.png")                                                                                          \
    X(O_CHAR, "images/O.png")                                                                                          \
    X(BOARD, "images/BOARD.png")                                                                                       \
    X(SLCT_D1, "images/SLCT.png")                                                                                      \
    X(SLCT_D2, "images/SLCT2.png")                                                                                     \
    X(SLCT_D3, "images/SLCT3.png")                                                                                     \
    X(TURN_AI, "images/TURN_AI.png")                                                                                   \
    X(TURN_PL, "images/TURN_PL.png")                                                                                   \
    X(VICTORY, "images/victory.png")                                                                                   \
    X(DEFEAT, "images/defeat.png")                                                                                     \
    X(TIE, "images/tie.png")                                                                                           \
    X(END_BANNER, "images/end.png")

constexpr const char *audioPaths[]{
#define X(enum, type, pth) pth,
    AUDIO_ASSETS
#undef X
};

constexpr const char *imagePaths[]{
#define X(enum, pth) pth,
    IMAGE_ASSETS
#undef X
};

namespace tct {

constexpr const std::size_t imageCount = std::size(imagePaths);
constexpr const std::size_t audioCount = std::size(audioPaths);
constexpr const std::size_t assetCount = imageCount + audioCount;
constexpr const std::size_t assetTypes = 2;
constexpr const std::size_t audioIdOffset = 0;
constexpr const std::size_t imageIdOffset = audioCount;

enum class AssetId : std::size_t {
#define X(enum, type, pth) enum,
    AUDIO_ASSETS
#undef X
#define X(enum, pth) enum,
        IMAGE_ASSETS
#undef X
};

enum class AudioType : std::uint8_t { MUSIC, SFX };

constexpr const AudioType audioTypes[]{
#define X(enum, type, pth) AudioType::type,
    AUDIO_ASSETS
#undef X
};

struct AssetMetadata {
    virtual ~AssetMetadata() = default;
};

struct ImageMetadata : public AssetMetadata {
    std::size_t height{};
    std::size_t width{};
    std::uint8_t channels{};
    ~ImageMetadata() = default;
};

struct AudioMetadata : public AssetMetadata {
    AudioType audioType;
    ~AudioMetadata() = default;
};

using AssetMetadataVariant = std::variant<ImageMetadata, AudioMetadata>;
struct Asset {
    AssetMetadataVariant assetMetadata;
    AssetId assetId;
    std::vector<std::byte> assetData;
};

class AssetRegistry {
  private:
    // Cached hence mutable.
    mutable std::array<std::shared_ptr<Asset>, assetCount> assets{};
    const std::filesystem::path rootAssetPath{[] {
        std::array<WCHAR, 512> buffer{};
        DWORD ret{GetModuleFileNameW(NULL, buffer.data(), buffer.size())};
        check(ret != 0 && ret < buffer.size(), "Cannot retrieve path to executable.");
        std::filesystem::path execPath{buffer.data()};
        std::filesystem::path root{execPath.parent_path()};
        std::filesystem::path assetPath{root / "assets/"};
        return assetPath;
    }()};
    void loadImage(std::shared_ptr<Asset> assetPtr, const std::size_t imgIdx);
    void loadAudio(std::shared_ptr<Asset> assetPtr, const std::size_t audioIdx);
    void loadAsset(const AssetId assetId);

  public:
    std::weak_ptr<Asset> getAsset(const AssetId assetId);
};

} // namespace tct