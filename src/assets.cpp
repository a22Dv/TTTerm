#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <vector>

#include "tt_assets.hpp"
#include "tt_utils.hpp"

namespace fs = std::filesystem;
const std::vector<std::byte>& ttt::Asset::read_data() const {
    if (_data.empty()) {
        const char* path = get_path(_type, _uid);
        check(fs::exists(path), std::format("ERROR: File does not exist at {}", path));
        std::ifstream file{path, std::ios::binary | std::ios::ate};
        check(!file, std::format("ERROR: File cannot be opened at {}", path));
        std::streamsize size = file.tellg();
        check(size == -1, std::format("ERROR: Cannot retrieve file size at {}", path));
        file.seekg(0, std::ios::beg);
        _data.resize(size);
        file.read(reinterpret_cast<char*>(_data.data()), size);
        check(!file, std::format("ERROR: Cannot read file at {}", path));
    }
    return _data;
}