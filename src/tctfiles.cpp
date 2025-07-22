#include "tctfiles.hpp"

#include <cstddef>
#include <format>
#include <fstream>
#include <vector>

#include "tctutils.hpp"

std::vector<std::byte> tct::getDataFromId(const tct::AssetID assetId) {
    const std::size_t id = static_cast<std::size_t>(assetId);
    const std::size_t aCount = static_cast<std::size_t>(tct::audioCount);

    const char* targetPath = id < aCount
                                 ? tct::audioPaths[id]
                                 : tct::imagePaths[id - aCount];
    std::ifstream dataStream{targetPath, std::ios::binary | std::ios::ate};
    tct::check(!!dataStream, std::format("Failure to read asset data. Asset ID: {} at {}", id, targetPath));
    std::size_t dataSize = static_cast<std::size_t>(dataStream.tellg());
    std::vector<std::byte> data{dataSize};
    dataStream.seekg(0);
    dataStream.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(dataSize));
    return data;
}

tct::AssetMetadata tct::getMetaDataFromId(const tct::AssetID assetId) {
    tct::check(assetId != tct::AssetID::NONE, "INVALID ARGUMENT. No metadata for AssetID::NONE.");
    if (static_cast<std::size_t>(assetId) < tct::audioCount) {
        return tct::AssetMetadata{.type = tct::AssetType::AUDIO, .dims = tct::Dimensions{.x = 0, .y = 0}};
    } else {
        return tct::AssetMetadata{.type = tct::AssetType::IMAGE,
                             .dims = tct::imageDimensions[static_cast<std::size_t>(assetId) - tct::audioCount]};
    }
}