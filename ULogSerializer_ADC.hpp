#ifndef ULOG_SERIALIZER_ADC_HPP
#define ULOG_SERIALIZER_ADC_HPP

#include <vector>
#include <array>
#include <cstdint>

template<size_t N, const char (&msg_name)[N], uint64_t header_timestamp, typename... Pins>
class ULogSerializer_ADC {
public:
    std::vector<uint8_t> get_ulogSetupMsg() {
        // TODO: Return ULog setup message
        return std::vector<uint8_t>{0x55, 0x4C, 0x4F, 0x47}; // "ULOG" header
    }
    
    void set_ulogDataMsg(uint32_t msg_id, uint64_t timestamp, const std::array<uint16_t, 4>& data) {
        // TODO: Set ULog data message with given parameters
    }
    
    std::vector<uint8_t> get_ulogDataMsg() {
        // TODO: Return ULog data message
        return std::vector<uint8_t>{0x01, 0x02, 0x03, 0x04}; // Dummy data
    }
};

#endif // ULOG_SERIALIZER_ADC_HPP