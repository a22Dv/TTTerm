#pragma once

#include "tcttypes.hpp"
#include "tctutils.hpp"

#include <cstddef>
#include <iterator>
#include <vector>

#define AUDIO_FILES                                                                              \
    X(TEST_AUDIO, "C:\\repositories\\tic_tac_term\\assets\\KATSEYE - Gabriel [GYyzaa9_ERg].wav") \
    X(MAIN_MENU, "C:\\repositories\\tic_tac_term\\assets\\Audio\\Title Theme\\xDeviruchi - Title Theme (Loop).wav")

#define IMAGE_FILES X(TEST_IMAGE, "C:\\repositories\\tic_tac_term\\assets\\Images\\donut.raw", 64, 64)

namespace tct {

enum class AssetID {
#define X(enum, path) enum,
    AUDIO_FILES
#undef X
#define X(enum, path, X, Y) enum,
        IMAGE_FILES
        NONE
#undef X
};

constexpr const char* audioPaths[]{
#define X(enum, path) path,
    AUDIO_FILES
#undef X
};

constexpr const char* imagePaths[]{
#define X(enum, path, X, Y) path,
    IMAGE_FILES
#undef X
};

constexpr std::size_t audioCount = std::size(audioPaths);
constexpr std::size_t imageCount = std::size(imagePaths);

constexpr const Dimensions imageDimensions[]{
#define X(enum, path, X, Y) Dimensions{.x = X, .y = Y},
    IMAGE_FILES
#undef X
};

enum class AssetType { NONE, IMAGE, AUDIO };

struct AssetMetadata {
    AssetType type{AssetType::NONE};
    Dimensions dims{};
};

std::vector<std::byte> getDataFromId(const AssetID assetId);
AssetMetadata getMetaDataFromId(const AssetID assetId);

struct Asset {
    AssetID assetId;
    AssetMetadata metadata;
    std::vector<std::byte> data{};
    Asset(AssetID assetId) : assetId{assetId}, data{getDataFromId(assetId)}, metadata{getMetaDataFromId(assetId)} {};
};

} // namespace tct