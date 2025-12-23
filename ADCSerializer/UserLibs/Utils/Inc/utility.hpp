#pragma once
#include <cstdint>
#include <cstddef>
#include <array>

//Gets uint64_t timestamp using tim2 and tim5 modules
std::uint64_t getTimestamp();

void buttonControl(void);

template <size_t N>
constexpr auto string_counter(const char (&p)[N]) -> size_t {
    return N ;
}

void memcpy_be(void *dest, const void *src, size_t size);