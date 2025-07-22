#pragma once

#define NOMINMAX
#include <Windows.h>

#include <array>
#include <memory>
#include <vector>

#include "tctfiles.hpp"
#include "tcttypes.hpp"

namespace tct {

constexpr const std::size_t pxFBufWidth = 320;
constexpr const std::size_t pxFBufHeight = 180;
constexpr const std::size_t pxFBufSize = pxFBufWidth * pxFBufHeight;
constexpr const std::size_t chSubPixelHeight = 4;
constexpr const std::size_t chSubPixelWidth = 2;
constexpr const std::size_t chSubPixels = chSubPixelHeight * chSubPixelWidth;
struct Renderable {
    Vector2D position;
    std::weak_ptr<Asset> asset;
};

struct RenderDimensions {
    std::size_t consoleHeight;
    std::size_t consoleWidth;
    std::size_t renderHeight;
    std::size_t renderWidth;
    Vector2D consoleStartRender; // Character position to place the first braille character to preserve aspect ratio.
};

using Pixel = std::uint8_t;

class Display {
   private:
    CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo{};
    HANDLE stdouth{GetStdHandle(STD_OUTPUT_HANDLE)};
    std::array<Pixel, pxFBufSize> pxFrameBuffer{};
    std::vector<Pixel> pxRenderFrameBuffer{};
    std::vector<CHAR_INFO> chFrameBuffer{};
    void bayer8x8(const RenderDimensions rDims);
    void nearestNeighbors(const uint8_t factor);
    RenderDimensions calcRenderDims();
    WCHAR convertToBraille(const std::array<Pixel, chSubPixels> bitmap);
   public:
    void render(const std::vector<Renderable>& renderables);
};

} // namespace tct