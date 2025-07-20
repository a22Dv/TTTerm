#include "tt_utils.hpp"
#include <stdexcept>
#include <string>

inline void ttt::check(bool condition, const std::string& message) {
    if (condition) {
        throw std::runtime_error(message);
    }
}
inline void ttt::check(bool condition, const char* message) {
    if (condition) {
        throw std::runtime_error(message);
    }
}
