#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include "utility.hpp"

template<std::size_t N>
class msgPackSerializer {

public:

    void append_map(std::uint8_t map_size){
        dataMsg_buffer[cursor] = FIXMAP_MASK + map_size;
        cursor++;

    }

    template<typename Type>
    void append_data(Type& data) {
        using Decayed = std::decay_t<Type>;

        std::uint8_t prefix;
        std::uint16_t size;

        if constexpr (std::is_same_v<Decayed, uint8_t>) {
            prefix = UINT8_PREFIX;
            size = sizeof(uint8_t);
            dataMsg_buffer[cursor++] = prefix;
            memcpy_be(&dataMsg_buffer[cursor], &data, size);
        } else if constexpr (std::is_same_v<Decayed, uint16_t>) {
            prefix = UINT16_PREFIX;
            size = sizeof(uint16_t);
            dataMsg_buffer[cursor++] = prefix;
            memcpy_be(&dataMsg_buffer[cursor], &data, size);
        } else if constexpr (std::is_same_v<Decayed, uint32_t>) {
            prefix = UINT32_PREFIX;
            size = sizeof(uint32_t);
            dataMsg_buffer[cursor++] = prefix;
            memcpy_be(&dataMsg_buffer[cursor], &data, size);
        } else if constexpr (std::is_same_v<Decayed, uint64_t>) {
            prefix = UINT64_PREFIX;
            size = sizeof(uint64_t);
            dataMsg_buffer[cursor++] = prefix;
            memcpy_be(&dataMsg_buffer[cursor], &data, size);
        } else if constexpr (std::is_same_v<Decayed, std::string>) {
            prefix = FIXSTR_MASK + data.size();
            size = data.length();
            dataMsg_buffer[cursor++] = prefix;
            std::memcpy(&dataMsg_buffer[cursor], data.data(), size);
        } else if constexpr (std::is_same_v<Decayed, const char*>) {
            prefix = FIXSTR_MASK + strlen(data);
            size = strlen(data);
            dataMsg_buffer[cursor++] = prefix;
            std::memcpy(&dataMsg_buffer[cursor], data, size);
        } else {
            static_assert("Unsupported type");
        }
        cursor += size;
    }

    std::span<std::uint8_t> get_msgPackDataMsg() {

        return {dataMsg_buffer.data(), cursor};
    }

private:

    static constexpr uint8_t UINT8_PREFIX      = 0xcc;
    static constexpr uint8_t UINT16_PREFIX     = 0xcd;
    static constexpr uint8_t UINT32_PREFIX     = 0xce;
    static constexpr uint8_t UINT64_PREFIX     = 0xcf;
    static constexpr uint8_t FIXMAP_MASK       = 0x80; // 0x80 - 0x8f (0–15 key-value pairs)
    static constexpr uint8_t FIXARRAY_MASK     = 0x90; // 0x90 - 0x9f (0–15 elements)
    static constexpr uint8_t FIXSTR_MASK       = 0xa0; // 0xa0 - 0xbf (0–31 bytes)
protected:
    std::uint16_t cursor = 0;
    std::array<std::uint8_t, N> dataMsg_buffer;
    //std::span<uint8_t> dataMsg_buffer = std::span<uint8_t> (allocated_space);

protected:
};
