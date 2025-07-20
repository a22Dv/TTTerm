#include <stdexcept>
#include <string>

namespace ttt {
inline void check(bool condition, const std::string& message);
inline void check(bool condition, const char* message);
}