#pragma once

#include <cstddef>
#include <vector>

namespace util {

    std::string base64Encode(const std::string&);

    std::string base64Encode(const std::vector<std::byte>&);

    std::string base64Encode(const std::byte* src, std::size_t len);

    std::vector<std::byte> base64Decode(const std::string&);

    std::vector<std::byte> base64Decode(const std::vector<std::byte>&);

    std::vector<std::byte> base64Decode(const std::byte* src, std::size_t len);

} // namespace util