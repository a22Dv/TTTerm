#pragma once
#include <stdexcept>
#include <string>

namespace tct {

inline void check(const bool successCondition, const std::string exceptionMessage) {
    if (!successCondition) {
        throw std::runtime_error(exceptionMessage);
    }
}
inline void check(const bool successCondition, const char *exceptionMessage) {
    if (!successCondition) {
        throw std::runtime_error(exceptionMessage);
    }
}

} // namespace tct
