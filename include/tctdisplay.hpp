#pragma once
#include "tctassets.hpp"
#include <cstdint>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace tct {

constexpr const std::size_t brailleChWidth{2};
constexpr const std::size_t brailleChHeight{4};
constexpr const std::size_t brailleChs{brailleChWidth * brailleChHeight};
constexpr const std::size_t chFrameWidth{120};
constexpr const std::size_t chFrameHeight{30};
constexpr const std::size_t chFrameSize{chFrameWidth * chFrameHeight};
constexpr const std::size_t pxFrameWidth{chFrameWidth * brailleChWidth};
constexpr const std::size_t pxFrameHeight{chFrameHeight * brailleChHeight};
constexpr const std::size_t pxFrameSize{pxFrameHeight * pxFrameWidth};

struct Vector2 {
    std::uint16_t x{};
    std::uint16_t y{};
    Vector2() {};
    Vector2(std::uint16_t x, std::uint16_t y) : x{x}, y{y} {};
};

struct RenderRequest {
    std::weak_ptr<Asset> asset{};
    Vector2 position{}; // Aimed at top-left of sprite.
};

using Pixel = std::uint8_t;
/*
    TODO: Define Display API, finalize game scene handling logic.
    Finish Display class implementation. Finish defining
    and implementing Input API. Complete rewrite for Input
    required.
*/

class Display {
  private:
    HANDLE stdouth{GetStdHandle(STD_OUTPUT_HANDLE)};
    std::array<CHAR_INFO, chFrameSize> chFrameBuffer{};
    std::array<Pixel, pxFrameSize> pxFrameBuffer{};

  public:
    void render(const RenderRequest request);
    void present();

};

} // namespace tct