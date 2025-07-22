#pragma once
#include <Windows.h>

#include <array>
#include <vector>

#include "tcttypes.hpp"
#include "tctfiles.hpp"

namespace tct {
    
constexpr std::size_t pxFBufWidth = 320;
constexpr std::size_t pxFBufHeight = 180;
constexpr std::size_t pxFBufSize = pxFBufWidth * pxFBufHeight;
constexpr std::size_t charFBufWidth = pxFBufWidth / 2;
constexpr std::size_t charFBufHeight = pxFBufHeight / 4;
constexpr std::size_t charFBufSize = charFBufWidth * charFBufHeight;

struct Renderable {
    Vector2D position;
    AssetID id;
};

using Pixel = uint8_t;

class Display {
   private:
    CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo;
    std::array<WCHAR, charFBufSize>  wcharFrameBuffer;
    std::array<Pixel, pxFBufSize> pxFrameBuffer;
    void bayer8x8(std::vector<Pixel>& pxFrameBuffer);
   public:
    void render(const std::vector<Renderable>& renderables);
};

} // namespace tct