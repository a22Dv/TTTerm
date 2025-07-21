#pragma once

#include <stdexcept>
#include <string>

namespace tct {

inline void check(bool check_condition, const std::string& msg) {
    if (!check_condition) {
        throw std::runtime_error(msg);
    }
}
inline void check(bool check_condition, const char* msg) {
    if (!check_condition) {
        throw std::runtime_error(msg);
    }
}

} // namespace tct