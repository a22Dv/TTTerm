#pragma once

#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <variant>
#include <vector>

#define AUDIO_ASSETS                                                                                                   \
    X(TEST_MUSIC, MUSIC, "C:/repositories/ttterm/assets/audio/KATSEYE - Gabriel [GYyzaa9_ERg].wav")                    \
    X(SELECT_MENU, SFX, "C:/repositories/ttterm/assets/audio/select_menu.wav")                                         \
    X(MENU_BGM, MUSIC, "C:/repositories/ttterm/assets/audio/xDeviruchi - Title Theme (Loop).wav")                      \
    X(ENTER_MENU, MUSIC, "C:/repositories/ttterm/assets/audio/menu_enter.wav")                                         \
    X(ERROR_SFX, SFX, "C:/repositories/ttterm/assets/audio/err.wav")                                                   \
    X(VICTORY_SFX, SFX, "C:/repositories/ttterm/assets/audio/victory.wav")                                             \
    X(DEFEAT_SFX, SFX, "C:/repositories/ttterm/assets/audio/defeat.wav")                                               \
    X(TIE_SFX, SFX, "C:/repositories/ttterm/assets/audio/tie.wav")                                                     \
    X(EASY_BGM, MUSIC, "C:/repositories/ttterm/assets/audio/xDeviruchi - Mysterious Dungeon.wav")                      \
    X(NORMAL_BGM, MUSIC, "C:/repositories/ttterm/assets/audio/xDeviruchi - Exploring the Unknown.wav")                 \
    X(HARD_BGM, MUSIC, "C:/repositories/ttterm/assets/audio/xDeviruchi - Prepare for Battle!.wav")                     \
    X(UNBEATABLE_BGM, MUSIC, "C:/repositories/ttterm/assets/audio/xDeviruchi - Decisive Battle.wav")

#define IMAGE_ASSETS                                                                                                   \
    X(TEST_IMAGE, "C:/repositories/ttterm/assets/images/test.png")                                                     \
    X(AI_MENU_EASY, "C:/repositories/ttterm/assets/images/ai_menu_easy.png")                                           \
    X(AI_MENU_NORMAL, "C:/repositories/ttterm/assets/images/ai_menu_normal.png")                                       \
    X(AI_MENU_HARD, "C:/repositories/ttterm/assets/images/ai_menu_hard.png")                                           \
    X(AI_MENU_UNBEATABLE, "C:/repositories/ttterm/assets/images/ai_menu_unbeatable.png")                               \
    X(SELECTOR_MENU, "C:/repositories/ttterm/assets/images/selector_menu.png")                                         \
    X(TITLE, "C:/repositories/ttterm/assets/images/title.png")                                                         \
    X(BORDER, "C:/repositories/ttterm/assets/images/BORDER.png")                                                       \
    X(EASY, "C:/repositories/ttterm/assets/images/EASY.png")                                                           \
    X(NORMAL, "C:/repositories/ttterm/assets/images/NORMAL.png")                                                       \
    X(HARD, "C:/repositories/ttterm/assets/images/HARD.png")                                                           \
    X(UNBEATABLE, "C:/repositories/ttterm/assets/images/UNBEATABLE.png")                                               \
    X(VERSION, "C:/repositories/ttterm/assets/images/VERSION.png")                                                     \
    X(X_CHAR, "C:/repositories/ttterm/assets/images/X.png")                                                            \
    X(O_CHAR, "C:/repositories/ttterm/assets/images/O.png")                                                            \
    X(BOARD, "C:/repositories/ttterm/assets/images/BOARD.png")                                                         \
    X(SLCT_D1, "C:/repositories/ttterm/assets/images/SLCT.png")                                                        \
    X(SLCT_D2, "C:/repositories/ttterm/assets/images/SLCT2.png")                                                       \
    X(SLCT_D3, "C:/repositories/ttterm/assets/images/SLCT3.png")                                                       \
    X(TURN_AI, "C:/repositories/ttterm/assets/images/TURN_AI.png")                                                     \
    X(TURN_PL, "C:/repositories/ttterm/assets/images/TURN_PL.png")                                                     \
    X(VICTORY, "C:/repositories/ttterm/assets/images/victory.png")                                                     \
    X(DEFEAT, "C:/repositories/ttterm/assets/images/defeat.png")                                                       \
    X(TIE, "C:/repositories/ttterm/assets/images/tie.png")                                                             \
    X(END_BANNER, "C:/repositories/ttterm/assets/images/end.png")

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
    void loadAsset(const AssetId assetId) const;

  public:
    std::weak_ptr<Asset> getAsset(const AssetId assetId) const;
};

} // namespace tct