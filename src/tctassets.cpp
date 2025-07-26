#include <memory>
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "tctassets.hpp"
#include "tctutils.hpp"
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>

namespace tct {
namespace {

// Forces the image to only have a Grayscale channel.
constexpr const int stbGray = 1;

void loadAudio(std::shared_ptr<Asset> assetPtr, const std::size_t audioIdx) {
    const std::filesystem::path audioPath = std::filesystem::path{audioPaths[audioIdx]};
    std::ifstream audioData{audioPath, std::ios::binary | std::ios::ate};
    const std::streamsize dataSize{audioData.tellg()};
    audioData.seekg(0);
    assetPtr->assetData.resize(dataSize);
    audioData.read(reinterpret_cast<char *>(assetPtr->assetData.data()), dataSize);
    AudioMetadata mtdta{};
    mtdta.audioType = audioTypes[audioIdx];
    assetPtr->assetMetadata = mtdta;
}

void loadImage(std::shared_ptr<Asset> assetPtr, const std::size_t imgIdx) {
    int width{};
    int height{};
    const char *imagePath{imagePaths[imgIdx]};
    std::uint8_t *imgData{static_cast<std::uint8_t *>(stbi_load(imagePath, &width, &height, nullptr, stbGray))};
    check(imgData != nullptr, std::format("An error has occured when loading {}.", imagePath));
    ImageMetadata mtdta{};
    mtdta.height = static_cast<std::size_t>(height);
    mtdta.width = static_cast<std::size_t>(width);
    mtdta.channels = static_cast<std::size_t>(stbGray);
    const std::size_t totalBytes{static_cast<std::size_t>(width * height * mtdta.channels)};
    assetPtr->assetMetadata = mtdta;
    assetPtr->assetData.resize(totalBytes);
    memcpy(static_cast<void *>(assetPtr->assetData.data()), static_cast<void *>(imgData), totalBytes);
    stbi_image_free(static_cast<void *>(imgData));
}

using LoadDispatchArray = std::array<std::function<void(std::shared_ptr<Asset>, const std::size_t)>, assetTypes>;

} // namespace

std::weak_ptr<Asset> AssetRegistry::getAsset(const AssetId assetId) const {
    const std::size_t assetIdx{static_cast<std::size_t>(assetId)};
    if (assets[assetIdx].get() == nullptr) {
        loadAsset(assetId);
    }
    return static_cast<std::weak_ptr<Asset>>(assets[assetIdx]);
}

void AssetRegistry::loadAsset(const AssetId assetId) const {
    const std::size_t assetIdx{static_cast<std::size_t>(assetId)};
     std::shared_ptr<Asset> assetPtr = assets[assetIdx];
    if (assetPtr != nullptr) {
        return; // Asset is already loaded.
    }
    static const LoadDispatchArray dispatchTable{loadAudio, loadImage};
    constexpr const std::array<std::size_t, assetTypes + 1> offsets{audioIdOffset, imageIdOffset, assetCount};
    assets[assetIdx] = std::make_shared<Asset>(Asset{});
    std::shared_ptr<Asset> setAssetPtr{assets[assetIdx]};
    setAssetPtr->assetId = assetId;
    for (std::size_t i = 0; i < assetTypes; ++i) {
        if (assetIdx < offsets[i] || assetIdx >= offsets[i + 1]) {
            continue;
        }
        dispatchTable[i](setAssetPtr, assetIdx - offsets[i]);
        break;
    }
}

} // namespace tct
