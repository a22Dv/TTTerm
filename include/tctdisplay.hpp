#pragma once
#include <Windows.h>

#include <vector>

#include "tctscene.hpp"

namespace tct {
using Pixel = uint8_t;
class Display {
   private:
    CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo;
    std::vector<WCHAR> wcharFrameBuffer;
    std::vector<Pixel> pxFrameBuffer;
    void applyDither();
    void floydSteinberg(std::vector<Pixel>& pxFrameBuffer);
    void bayer16x16(std::vector<Pixel>& pxFrameBuffer);

   public:
    void render(const Scene& targetScene);
};

} // namespace tct