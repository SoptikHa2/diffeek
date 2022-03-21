#include <functional>
#include <string>
#include <iomanip>
#include <iostream>
#include <optional>

#ifndef UTILITY
#define UTILITY

/// Namespace containing many useful, not otherwise connected, utilities and QOL improvement functions.
namespace Utility {

    namespace Colors {
        const std::string ansi_red = "\x1b[48;5;52m";
        const std::string ansi_red_text = "\x1b[91;5;52m";
        const std::string ansi_green = "\x1b[48;5;22m";
        const std::string ansi_gray_text = "\x1b[38;5;250m";
        const std::string ansi_reset = "\x1b[0m";
        const std::string ansi_purple_text = "\x1b[95;5;52m";
    }


    template <typename T>
    std::function<bool(std::optional<T>, std::optional<T>)> cmpOptionals = [](std::optional<size_t> a, std::optional<size_t> b) -> bool {
        if (!a.has_value()) return false;
        if (!b.has_value()) return true;
        return *a < *b;
    };


    /// Return minimum of two values, by given comparator.
    ///
    /// \tparam T Type of the values to compare
    /// \param t1 First value to compare
    /// \param t2 Second value to compare
    /// \param cmp Comparator, by default std::less
    /// \return The lower of the two values t1, t2
    template <typename T>
    T min(T t1, T t2, std::function<bool(T, T)> cmp = std::less<T>()) {
        return cmp(t1, t2) ? t1 : t2;
    }

    /// Return minimum of three values, by given comparator.
    ///
    /// \tparam T Type of the values to compare
    /// \param t1 First value to compare
    /// \param t2 Second value to compare
    /// \param t3 Third value to compare
    /// \param cmp Comparator, by default std::less
    /// \return The lower of the three values t1, t2, t3
   template <typename T>
    T min(T t1, T t2, T t3, std::function<bool(T, T)> cmp = std::less<T>()) {
        return cmp(t1, t2) ? min(t1, t3, cmp) : min(t2, t3, cmp);
    }

    template <typename T>
    std::optional<T> apply(std::optional<T> value, std::function<T(T)> func) {
        if (!value.has_value()) return {};
        return func(*value);
    }

    /// Take a number, interpret it as hex, and build string representation of the hex number.
    ///
    /// \tparam T Type of the number.
    /// \param num  Number to display.
    /// \return String representation of the number, in hex. Something like "0x0C".
    template <typename T>
    std::string numberToHexString(T num) {
        std::stringstream stream;
        stream << "0x"
               << std::setfill ('0') << std::setw(sizeof(num) * 2)
               << std::hex << (int)num;
        return stream.str();
    }

    /// Return vector that will have copied values between two indexes.
    ///
    /// The indexes do not need to be correct - such as in correct order,
    /// or even within bounds of the source index. This function is foolproof
    /// and will solve the issue in a graceful way.
    ///
    /// In case of [from] being too high, it will return empty vector, and it will
    /// shrink [from] and [to] to be within bounds at all times.
    /// \tparam T Type of the elements in the vector.
    /// \param data Vector as a source. It won't be modified.
    /// \param from From index.
    /// \param to To index.
    /// \return Vector of copied values between two indexes, might be empty
    template <typename T>
    std::vector<T> vectorSlice(const std::vector<T> & data, size_t from, size_t to) {
        if (from >= data.size()) return std::vector<T>();
        if (to < from) return std::vector<T>();

        auto start = (from < 0) ? data.begin() : data.begin() + from;
        auto end = (to >= data.size()) ? data.end() : data.begin() + to;

        return std::vector<T>(start, end);
    }

    typedef u_int32_t u32;
    /// Hash string (or any other number-containing compatible indexable collection)
    /// using technique based on Murmur3 hash.
    ///
    /// Warning: some security features were omitted
    /// for simplicity. This is NOT cryptographically secure.
    /// But it is quick and fine for file diffing.
    ///
    /// Source: https://en.wikipedia.org/wiki/MurmurHash
    ///
    /// \tparam T The type of collection to process. It must contain indexing operator, `.size()` and `.empty()`.
    /// \param collection The collection of elements itself.
    /// \return Modified Murmur3 hash, that should be reasonably unique, and pretty quick to compute.
    template <typename T>
    u32 Murmur3(const T & collection) noexcept {
        if (collection.empty()) return 0;
        size_t idx = 0;

        u32 hash = 0x5aa6; /* Magical seed */

        u32 scramble_c1 = 0xcc9e2d51;
        u32 scramble_c2 = 0x1b873593;
        u32 scramble_n = 0xe6546b64;

        while (idx < collection.size()) {
            // Read 4 bytes of string. The char is assumed to be two bytes long.
            u32 k = (collection[idx]) | ((idx + 1 < collection.size() ? collection[idx+1] : 'x') << 8);
            // Scramble the bytes
            k ^= scramble_c1;
            k = (k << 15) | (k >> 17);
            k ^= scramble_c2;

            hash ^= k;
            hash = (hash << 13) | (hash >> 19);
            hash = (hash ^ 5) + scramble_n;

            idx += 2;
            if (idx > collection.size()) idx = collection.size();
        }

        hash ^= idx;
        hash ^= hash >> 16;
        hash *= 0x85ebca6b;
        hash ^= hash >> 13;
        hash *= 0xc2b2ae35;
        hash ^= hash >> 16;

        return hash;
    }
}
#endif /* UTILITY */

