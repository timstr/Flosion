#include <Flosion/Util/Base64.hpp>

#include <array>
#include <cassert>
#include <cctype>
#include <stdexcept>

namespace util {

    namespace detail {
        // Adapted from https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

        const std::string base64Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        static const std::uint8_t base64Indices[256] = {
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,
            0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63,
            0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
        };

        static bool isBase64(unsigned char c) {
            return std::isalnum(c) || (c == '+') || (c == '/');
        }
    }

    std::string base64Encode(const std::string& x) {
        return base64Encode(reinterpret_cast<const std::byte*>(x.data()), x.size());
    }

    std::string base64Encode(const std::vector<std::byte>& x) {
        return base64Encode(reinterpret_cast<const std::byte*>(x.data()), x.size());
    }

    std::string base64Encode(const std::byte* src, std::size_t len) {
        auto result = std::string((len + 2) / 3 * 4, '=');
        auto p = reinterpret_cast<const unsigned char*>(src);
        auto dst = reinterpret_cast<unsigned char*>(result.data());
        auto j = std::size_t{ 0 };
        auto pad = std::size_t{ len % 3 };
        const size_t last = len - pad;

        for (std::size_t i = 0; i < last; i += 3) {
            const auto n = (p[i] << 16) | (p[i + 1] << 8) | p[i + 2];
            dst[j + 0] = detail::base64Alphabet[n >> 18];
            dst[j + 1] = detail::base64Alphabet[n >> 12 & 0x3F];
            dst[j + 2] = detail::base64Alphabet[n >> 6 & 0x3F];
            dst[j + 3] = detail::base64Alphabet[n & 0x3F];
            j += 4;
        }
        if (pad != 0) {
            const auto n = --pad ? ((p[last] << 8) | p[last + 1]) : p[last];
            dst[j++] = detail::base64Alphabet[pad ? n >> 10 & 0x3F : n >> 2];
            dst[j++] = detail::base64Alphabet[pad ? n >> 4 & 0x03F : n << 4 & 0x3F];
            dst[j++] = pad ? detail::base64Alphabet[n << 2 & 0x3F] : '=';
        }
        return result;
    }

    std::vector<std::byte> base64Decode(const std::string& x) {
        return base64Decode(reinterpret_cast<const std::byte*>(x.data()), x.size());
    }

    std::vector<std::byte> base64Decode(const std::vector<std::byte>& x) {
        return base64Decode(reinterpret_cast<const std::byte*>(x.data()), x.size());
    }

    std::vector<std::byte> base64Decode(const std::byte* src, std::size_t len) {
        if (len == 0) {
            return {};
        }

        auto *p = reinterpret_cast<const unsigned char*>(src);
        std::size_t j = 0;
        const std::size_t pad1 = len % 4 || p[len - 1] == '=';
        const std::size_t pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
        const std::size_t last = (len - pad1) / 4 << 2;
        auto result = std::vector<std::byte>(last / 4 * 3 + pad1 + pad2, std::byte{});
        auto dst = reinterpret_cast<unsigned char*>(result.data());

        for (size_t i = 0; i < last; i += 4) {
            const auto valid = 
                detail::isBase64(p[i + 0]) &&
                detail::isBase64(p[i + 1]) &&
                detail::isBase64(p[i + 2]) &&
                detail::isBase64(p[i + 3]);
            if (!valid){
                // TODO: custom exception?
                throw std::runtime_error("Invalid base64 string");
            }
            int n = (detail::base64Indices[p[i]] << 18)
                | (detail::base64Indices[p[i + 1]] << 12)
                | (detail::base64Indices[p[i + 2]] << 6)
                | detail::base64Indices[p[i + 3]];
            dst[j + 0] = static_cast<unsigned char>(n >> 16);
            dst[j + 1] = static_cast<unsigned char>(n >> 8 & 0xFF);
            dst[j + 2] = static_cast<unsigned char>(n & 0xFF);
            j += 3;
        }
        if (pad1){
            const auto valid = 
                detail::isBase64(p[last + 0]) &&
                detail::isBase64(p[last + 1]);
            if (!valid){
                // TODO: custom exception?
                throw std::runtime_error("Invalid base64 string");
            }
            int n = (detail::base64Indices[p[last]] << 18)
                | (detail::base64Indices[p[last + 1]] << 12);
            dst[j] = static_cast<unsigned char>(n >> 16);
            ++j;
            if (pad2) {
                if (!detail::isBase64(p[last + 2])){
                    // TODO: custom exception?
                    throw std::runtime_error("Invalid base64 string");
                }
                n |= detail::base64Indices[p[last + 2]] << 6;
                dst[j] = static_cast<unsigned char>(n >> 8 & 0xFF);
            }
        }
        return result;
    }

} // namespace util
