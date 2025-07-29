#include "tctinput.hpp"
#include "tctutils.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <conio.h>

namespace tct {

Input::Input() {
    stdinh = GetStdHandle(STD_INPUT_HANDLE);
    check(stdinh != nullptr && stdinh != INVALID_HANDLE_VALUE, "Cannot retrieve STDIN handle.");
    check(GetConsoleMode(stdinh, &consolePrevMode), "Failed to retrieve STDIN mode.");

    consoleCurrMode = consolePrevMode;
    consoleCurrMode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    check(SetConsoleMode(stdinh, consoleCurrMode), "Failed to set STDIN mode.");
}

Input::~Input() { check(SetConsoleMode(stdinh, consolePrevMode), "Failed to reset STDIN mode."); }

InputKey Input::pollInput() {
    if (!_kbhit()) {
        return InputKey::NONE;
    }
    int ch{_getch()};
    if (ch != extKey) {
        switch (ch) {
        case '\r': return InputKey::ENTER;
        default: return InputKey::NONE;
        }
    }
    ch = _getch();
    switch (ch) {
    case leftArrowKey: return InputKey::ARROW_LEFT;
    case rightArrowKey: return InputKey::ARROW_RIGHT;
    case upArrowKey: return InputKey::ARROW_UP;
    case downArrowKey: return InputKey::ARROW_DOWN;
    default: return InputKey::NONE;
    }
};

} // namespace tct
