#pragma once

#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <vector>
#include <variant>

#define AUDIO_ASSETS                                                                               \
    X(TEST_MUSIC, MUSIC, "C:/repositories/ttterm/assets/audio/KATSEYE - Gabriel [GYyzaa9_ERg].wav")

#define IMAGE_ASSETS X(TEST_IMAGE, "C:/repositories/ttterm/assets/images/ttt_board.png")

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