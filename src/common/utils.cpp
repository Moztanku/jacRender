#include "common/utils.hpp"

#include <fstream>

namespace common {

[[nodiscard]]
auto read_file(const std::filesystem::path& path) -> std::vector<char>
{
    std::ifstream file(path, std::ios::in);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    file.seekg(0, std::ios::end);
    const auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    file.read(buffer.data(), size);

    if (!file) {
        throw std::runtime_error("Failed to read file: " + path.string());
    }

    return buffer;
}

} // namespace common
