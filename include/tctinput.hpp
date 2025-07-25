#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>

namespace tct {
struct GameState;

constexpr const std::uint8_t extKey = 0xE0;
constexpr const std::uint8_t upArrowKey = 72;
constexpr const std::uint8_t downArrowKey = 80;
constexpr const std::uint8_t leftArrowKey = 75;
constexpr const std::uint8_t rightArrowKey = 77;

enum class InputKey {
    NONE,
    ENTER,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
};
enum class InputAction {
    NONE,
    INTERACT,
    SWITCH_FOCUS_LEFT,
    SWITCH_FOCUS_RIGHT,
    SWITCH_FOCUS_UP,
    SWITCH_FOCUS_DOWN,
};
class Input {
  private:
    HANDLE stdinh{};
    DWORD consolePrevMode{};
    DWORD consoleCurrMode{};
    InputKey pollInput();
    InputAction mapKey(InputKey key);

  public:
    void processInput(GameState& state);
    // Input();
    // Input(const Input &) = delete;
    // Input &operator=(const Input &) = delete;
    // Input(Input &&) = delete;
    // Input &operator=(Input &&) = delete;
    // ~Input();
};

} // namespace tct