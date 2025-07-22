#include "tctdisplay.hpp"

void tct::Display::bayer8x8(std::vector<tct::Pixel>& pxFrameBuffer) {
    constexpr const std::array<uint8_t, 64> bayerMatrix = {
        0,   129, 32,  161, 8,   137, 40,  169, 193, 64,  225, 96,  201, 72,  233, 104, 48,  177, 16,  145, 56,  185,
        24,  153, 241, 112, 209, 80,  249, 120, 217, 88,  12,  141, 60,  189, 4,   133, 52,  181, 205, 76,  253, 125,
        197, 68,  221, 92,  28,  157, 20,  149, 44,  173, 36,  165, 237, 108, 213, 84,  245, 116, 229, 100};
    for (std::size_t idx = 0; idx < pxFBufSize; ++idx) {
        const uint8_t threshold = bayerMatrix[(((idx / pxFBufWidth) % 8) * 8) + ((idx % pxFBufWidth) % 8)]; 
        pxFrameBuffer[idx] = pxFrameBuffer[idx] < threshold ? 0 : UINT8_MAX;  
    }
}

void tct::Display::render(const std::vector<tct::Renderable>& renderables) {
    for (const tct::Renderable& renderable : renderables) {
        
    }
}

// class Display {
//    private:
//     CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo;
//     std::array<WCHAR, charFBufSize>  wcharFrameBuffer;
//     std::array<Pixel, pxFBufSize> pxFrameBuffer;
//     void applyDither();
//     void floydSteinberg(std::vector<Pixel>& pxFrameBuffer);
//     void bayer8x8(std::vector<Pixel>& pxFrameBuffer);
//    public:
//     void render(const std::vector<Renderable>& renderables);
// };