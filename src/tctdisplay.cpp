#include "tctdisplay.hpp"

#define NOMINMAX
#include <Windows.h>

#include <algorithm>
#include <span>

// This entire file looks like crap. Refactor this stuff damn. Wipe it out when you have the time. 

void tct::Display::bayer8x8(const RenderDimensions rDims) {
    constexpr const std::array<std::uint8_t, 64> bayerMatrix = {
        1,   129, 32,  161, 8,   137, 40,  169, 193, 64,  225, 96,  201, 72,  233, 104, 48,  177, 16,  145, 56,  185,
        24,  153, 241, 112, 209, 80,  249, 120, 217, 88,  12,  141, 60,  189, 4,   133, 52,  181, 205, 76,  253, 125,
        197, 68,  221, 92,  28,  157, 20,  149, 44,  173, 36,  165, 237, 108, 213, 84,  245, 116, 229, 100};
    const std::size_t renderSize = rDims.renderHeight * rDims.renderWidth;
    const std::size_t renderWidth = rDims.renderWidth;
    const std::size_t renderHeight = rDims.renderHeight;
    for (std::size_t idx = 0; idx < renderSize; ++idx) {
        // & 7 == % 8, << 3 == * 8
        const std::uint8_t threshold = bayerMatrix[(((idx / renderWidth) & 7) << 3) + ((idx % renderWidth) & 7)];
        pxRenderFrameBuffer[idx] = pxRenderFrameBuffer[idx] < threshold ? 0 : UINT8_MAX;
    }
}

tct::RenderDimensions tct::Display::calcRenderDims() {
    const std::size_t cHeight = consoleBufferInfo.srWindow.Bottom - consoleBufferInfo.srWindow.Top + 1;
    const std::size_t cWidth = consoleBufferInfo.srWindow.Right - consoleBufferInfo.srWindow.Left + 1;

    const std::size_t cLHeight = cHeight * tct::chSubPixelHeight;
    const std::size_t cLWidth = cWidth * tct::chSubPixelWidth;

    const std::size_t scaleFactorH = cLHeight / pxFBufHeight;
    const std::size_t scaleFactorW = cLWidth / pxFBufWidth;
    const std::size_t scaleFactor =
        std::min(scaleFactorH != 0 ? scaleFactorH : 1, scaleFactorW != 0 ? scaleFactorW : 1);
    const std::size_t renderHeight = pxFBufHeight * scaleFactor;
    const std::size_t renderWidth = pxFBufWidth * scaleFactor;

    Vector2D startRenderAt{.x = scaleFactorW != 0 ? (cLWidth - renderWidth) / 2 : 0,
                           .y = scaleFactorH != 0 ? (cLHeight - renderHeight) / 2 : 0};

    return tct::RenderDimensions{.consoleHeight = cHeight,
                                 .consoleWidth = cWidth,
                                 .renderHeight = renderHeight,
                                 .renderWidth = renderWidth,
                                 .consoleStartRender = startRenderAt};
}

WCHAR tct::Display::convertToBraille(const std::array<tct::Pixel, tct::chSubPixels> bitmap) {
    static constexpr std::array<std::uint8_t, tct::chSubPixels> offsets{0, 3, 1, 4, 2, 5, 7, 6};
    uint8_t bit{};
    WCHAR ch{0x2800};
    for (std::size_t i = 0; i < tct::chSubPixels; ++i) {
        bit = bitmap[i] >> 7;
        ch |= bit << offsets[i];
    }
    return ch;
}

void tct::Display::nearestNeighbors(const uint8_t factor) {
    const std::size_t square = factor * factor;
    const std::size_t nSize = square * pxFBufHeight * pxFBufWidth;
    const std::size_t nWidth = factor * pxFBufWidth;
    const std::size_t nHeight = factor * pxFBufHeight;

    if (pxRenderFrameBuffer.size() != nSize) {
        pxRenderFrameBuffer.resize(nSize);
    }
    for (std::size_t i = 0; i < nSize; ++i) {
        const std::size_t oIdx = (i % nWidth) / factor + ((i / nWidth) / factor) * pxFBufWidth;
        pxRenderFrameBuffer[i] = pxFrameBuffer[oIdx];
    }
}

void tct::Display::render(const std::vector<tct::Renderable>& renderables) {
    static tct::RenderDimensions cacheRDims{};

    std::fill(pxFrameBuffer.begin(), pxFrameBuffer.end(), 0);
    GetConsoleScreenBufferInfo(stdouth, &consoleBufferInfo);

    for (const tct::Renderable& renderable : renderables) {
        std::shared_ptr<Asset> lockAsset{renderable.asset.lock()};
        tct::check(lockAsset != nullptr, "Resource deleted before acquisition.\n");
        if (lockAsset->metadata.type != tct::AssetType::IMAGE) {
            continue;
        }
        Dimensions imageDims{lockAsset->metadata.dims};
        std::span<const uint8_t> imageData{reinterpret_cast<const uint8_t*>(lockAsset->data.data()),
                                           lockAsset->data.size()};
        Vector2D startIdx{renderable.position};
        for (std::size_t y = startIdx.y; (y - startIdx.y) < imageDims.y; ++y) {
            for (std::size_t x = startIdx.x; (x - startIdx.x) < imageDims.x; ++x) {
                if (y >= pxFBufHeight || x >= pxFBufWidth) {
                    continue;
                }
                pxFrameBuffer[y * pxFBufWidth + x] = imageData[(y - startIdx.y) * imageDims.x + (x - startIdx.x)];
            }
        }
    }

    tct::RenderDimensions rDims{calcRenderDims()};
    if (rDims.consoleHeight != cacheRDims.consoleHeight || rDims.consoleWidth != cacheRDims.consoleWidth) {
        DWORD chWritten = 0;
        FillConsoleOutputCharacterW(stdouth, L' ', static_cast<DWORD>(rDims.consoleHeight * rDims.consoleWidth),  COORD{.X{0}, .Y{0}}, &chWritten);
        cacheRDims = rDims;
    }

    nearestNeighbors(rDims.renderWidth / pxFBufWidth);
    bayer8x8(rDims);

    const std::size_t hChars = rDims.renderHeight / chSubPixelHeight;
    const std::size_t wChars = rDims.renderWidth / chSubPixelWidth;
    const std::size_t tChars = wChars * hChars;

    chFrameBuffer.clear();
    chFrameBuffer.reserve(tChars);

    std::array<Pixel, chSubPixels> bitmap{};
    std::array<std::size_t, chSubPixels> alignment{0,
                                                   1,
                                                   rDims.renderWidth,
                                                   rDims.renderWidth + 1,
                                                   rDims.renderWidth * 2,
                                                   rDims.renderWidth * 2 + 1,
                                                   rDims.renderWidth * 3,
                                                   rDims.renderWidth * 3 + 1};

    for (std::size_t i = 0; i < tChars; ++i) {
        for (std::size_t j = 0; j < chSubPixels; ++j) {
            bitmap[j] =
                pxRenderFrameBuffer[((i / wChars) * chSubPixelHeight * rDims.renderWidth + (i % wChars) * chSubPixelWidth) + alignment[j]];
        }
        CHAR_INFO chInfo;
        chInfo.Char.UnicodeChar = convertToBraille(bitmap);
        chInfo.Attributes = 0 | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
        chFrameBuffer.push_back(chInfo);
    }

    SMALL_RECT smRect{
        .Left{static_cast<SHORT>(cacheRDims.consoleStartRender.x / tct::chSubPixelWidth)},
        .Top{static_cast<SHORT>(cacheRDims.consoleStartRender.y / tct::chSubPixelHeight)},
        .Right{static_cast<SHORT>(wChars > 0 ? cacheRDims.consoleStartRender.x / tct::chSubPixelWidth + wChars - 1 : 0)},
        .Bottom{static_cast<SHORT>(hChars > 0 ? cacheRDims.consoleStartRender.y / tct::chSubPixelHeight + hChars - 1 : 0)},
    };
    WriteConsoleOutputW(stdouth, chFrameBuffer.data(),
                        COORD{.X{static_cast<SHORT>(wChars)}, .Y{static_cast<SHORT>(hChars)}}, COORD{.X{0}, .Y{0}},
                        &smRect);
}
