#include "tctdisplay.hpp"
#include "tctutils.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <vector>

namespace tct {
void Display::render(const RenderRequest request) {
    const Vector2 topLeft{request.position};
    const std::shared_ptr<Asset> acquiredAsset{request.asset.lock()};
    check(acquiredAsset != nullptr, "Resource released before acquisition.");
    const std::vector<std::byte> &assetData{acquiredAsset->assetData};
    const ImageMetadata imgMtdta{std::get<ImageMetadata>(acquiredAsset->assetMetadata)};
    if (topLeft.x >= pxFrameWidth || topLeft.y >= pxFrameHeight) {
        return; // Target is not in frame at all.
    }
    const Vector2 renderStart = topLeft;
    const std::size_t renderHeight = std::min(imgMtdta.height, pxFrameHeight - renderStart.y);
    const std::size_t renderWidth = std::min(imgMtdta.width, pxFrameWidth - renderStart.x);
    for (std::size_t y = 0; y < renderHeight; ++y) {
        for (std::size_t x = 0; x < renderWidth; ++x) {
            const std::size_t newFIdx = ((renderStart.y + y) * pxFrameWidth) + (renderStart.x + x);
            const std::size_t imgIdx = y * imgMtdta.width + x;
            pxFrameBuffer[newFIdx] = static_cast<std::uint8_t>(assetData[imgIdx]);
        }
    }
}

void Display::present() {
    constexpr const WORD attr = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
    constexpr const COORD cZero{COORD{.X = 0, .Y = 0}};
    SMALL_RECT writeRegion{SMALL_RECT{.Left = 0, .Top = 0, .Right = chFrameWidth, .Bottom = chFrameHeight}};
    constexpr const COORD bufferSize{COORD{.X = chFrameWidth, .Y = chFrameHeight}};
    constexpr const std::array<std::uint8_t, brailleChs> alignment{0, 3, 1, 4, 2, 5, 6, 7};
    for (std::size_t chY = 0; chY < chFrameHeight; ++chY) {
        for (std::size_t chX = 0; chX < chFrameWidth; ++chX) {
            const std::size_t pxFIdx = (chY * brailleChHeight * pxFrameWidth) + (chX * brailleChWidth);
            CHAR_INFO &dspChar{chFrameBuffer[chY * chFrameWidth + chX]};
            WCHAR &ch{dspChar.Char.UnicodeChar};
            ch |= 0x2800;
            dspChar.Attributes = attr;
            for (std::size_t pxY = 0; pxY < brailleChHeight; ++pxY) {
                for (std::size_t pxX = 0; pxX < brailleChWidth; ++pxX) {
                    const std::size_t pxIdx = pxFIdx + (pxY * pxFrameWidth) + pxX;
                    const bool pxV = pxFrameBuffer[pxIdx] >= 128;
                    ch |= (pxV << alignment[pxY * brailleChWidth + pxX]);
                }
            }
        }
    }
    WriteConsoleOutputW(stdouth, chFrameBuffer.data(), bufferSize, cZero, &writeRegion);
}

} // namespace tct