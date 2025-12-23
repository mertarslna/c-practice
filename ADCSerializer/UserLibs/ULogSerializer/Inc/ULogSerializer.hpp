#pragma once

#include <array>
#include <span>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include "ADCPinMap.hpp"
#include "utility.hpp"

/**
 * @brief General Purpose Basic Ulog Serializer
 * Can be inherited for specialization or implemented in application.
 * Includes capacity to serialize header, format , subscription and log data frames.
 * Maximum sizes are set by the user manually.
 */
template<size_t formatMsg_quantity, size_t subMsg_quantity, size_t msgFormat_size, size_t msgName_size, size_t dataMsg_size>
class ULogSerializer {
public:
    /**
      * @brief Provides access to the ULog setup message buffer.
      *
      * This function returns a span view over the internal buffer containing the
      * ULog setup message.
      *
      * @return std::span<std::uint8_t> View over the setup message buffer.
      */
    std::span<std::uint8_t> get_ulogSetupMsg() {
        return {setupMsg_buffer.data(), cursor};
    }

    /**
      * @brief Provides access to the current ULog data message buffer.
      * Returns a span view over the most recent ULog data message.
      * @ref set_ulogDataMsg() must be called at least once.
      * @return std::span<std::uint8_t> View over the current data message buffer.
      */
    std::span<std::uint8_t> get_ulogDataMsg() {
        std::uint8_t temp_cursor = dataBuffer_cursor;
        dataBuffer_cursor = 0;
        return {dataMsg_buffer.data(), temp_cursor};
    }

    /**
     * @brief Fills the ULog data message buffer with a new payload.
     *
     * This method formats and stores a complete data message, consisting of:
     * - message ID
     * - timestamp
     * - payload
     *
     * The payload is provided as a span-compatible type
     * and is internally cast to a byte span for serialization.
     *
     * @tparam DataSpan A span-like container .
     * @param msg_id    Message identifier .
     * @param timestamp Timestamp in microseconds .
     * @param data      Payload span containing raw data.
     */
    template<typename DataSpan>
    void set_ulogDataMsg(std::uint16_t msg_id, std::uint64_t timestamp, DataSpan data) {
        if (data.size_bytes() > data_msg_size_s::sc_max_data_size) {
            error_handler();
        }
        std::span<const uint8_t> data_bytes{
            reinterpret_cast<const uint8_t *>(data.data()),
            data.size_bytes()
        };
        std::uint16_t data_msg_size = sizeof(msg_id) + sizeof(timestamp) + data_bytes.size();
        std::memcpy(&dataMsg_buffer[dataBuffer_cursor], &data_msg_size, msg_header_size::sc_msg_size_size);
        dataBuffer_cursor += msg_header_size::sc_msg_size_size;

        dataMsg_buffer[dataBuffer_cursor] = 'D';
        dataBuffer_cursor += msg_header_size::sc_msg_type_size;

        std::memcpy(&dataMsg_buffer[dataBuffer_cursor], &msg_id, data_msg_size_s::sc_msg_id_size);
        dataBuffer_cursor += data_msg_size_s::sc_msg_id_size;

        std::memcpy(&dataMsg_buffer[dataBuffer_cursor], &timestamp, sizeof(timestamp));
        dataBuffer_cursor += sizeof(timestamp);

        std::ranges::copy(data_bytes, &dataMsg_buffer[dataBuffer_cursor]);
        dataBuffer_cursor += data_bytes.size();
    }

    /**
      * @brief Initializes the ULog message header.
      *
      * This function sets up the ULog header structure,
      *
      * @param timestamp  64-bit timestamp indicating when logging was started.
      */
    void init_header(uint64_t timestamp) {
        setupMsg_buffer[cursor]=0x55; setupMsg_buffer[cursor+1]=0x4c;
        setupMsg_buffer[cursor+2]=0x6f; setupMsg_buffer[cursor+3]=0x67;
        setupMsg_buffer[cursor+4]=0x01; setupMsg_buffer[cursor+5]=0x12;
        setupMsg_buffer[cursor+6]=0x35;
        cursor += header_size::sc_file_magic_size;

        setupMsg_buffer[cursor] = 0x01; //version
        cursor += header_size::sc_version_size;

        std::memcpy(&setupMsg_buffer[cursor],&timestamp,header_size::sc_headerTimestamp_size);
        cursor += header_size::sc_headerTimestamp_size;
    }


    /**
     * @brief Initializes the ULog format message.
     *
     * This function encodes the format string for the message.
     * Format messages describe the structure of data fields in format described
     * by the ulog file format documentation.
     *
     * @param format A format string  describing the name and layout of the payload data.
     */
    void init_format(const char* format) {
        size_t format_msg_size = std::strlen(format);

        std::memcpy(&setupMsg_buffer[cursor],&format_msg_size,msg_header_size::sc_msg_size_size);
        cursor += msg_header_size::sc_msg_size_size;

        setupMsg_buffer[cursor] = 'F';
        cursor += msg_header_size::sc_msg_type_size;

        std::memcpy(&setupMsg_buffer[cursor],format,format_msg_size);
        cursor += format_msg_size;
        current_format_number++;
        if (current_format_number>sc_max_data_formatMsg_number) {
            error_handler();
        }
    }

    /**
     * @brief Initializes the ULog subscription message.
     *
     * Subscription messages associate a message name and format with a given
     * message ID and optional multi-instance ID. This allows the logger or
     * reader to route and interpret data messages correctly.
     *
     * @param multi_id      Optional sub-instance ID .
     * @param msg_id        Unique message ID used in data transmissions.
     * @param message_name  Name of the message type.
     */
    void init_subscription(std::uint8_t multi_id, std::uint16_t msg_id, const char *message_name ) {
        std::uint16_t msg_name_size = std::strlen(message_name);
        std::uint16_t subscription_msg_size = msg_name_size+sizeof(multi_id)+sizeof(msg_id);
        std::memcpy(&setupMsg_buffer[cursor],&subscription_msg_size,msg_header_size::sc_msg_size_size);
        cursor += msg_header_size::sc_msg_size_size;

        setupMsg_buffer[cursor] = 'A';
        cursor += msg_header_size::sc_msg_type_size;

        setupMsg_buffer[cursor] = multi_id;
        cursor += sub_msg_size::sc_msg_multi_id_size;

        std::memcpy(&setupMsg_buffer[cursor],&msg_id,sub_msg_size::sc_msg_id_size);
        cursor += sub_msg_size::sc_msg_id_size;;

        std::memcpy(&setupMsg_buffer[cursor],message_name,msg_name_size);
        cursor += msg_name_size;
        current_submessage_number++;
        if (current_submessage_number>sc_max_subMsg_number) {
            error_handler();
        }
    }

private:
    /**private variables**/
    //header
    struct header_size {
        static constexpr size_t sc_file_magic_size = sizeof(std::array<std::uint8_t, 7>);
        static constexpr size_t sc_version_size = sizeof(std::uint8_t);
        static constexpr size_t sc_headerTimestamp_size = sizeof(std::uint64_t);
        static constexpr size_t sc_header_size = sc_file_magic_size + sc_version_size + sc_headerTimestamp_size;
    };

    struct msg_header_size {
        static constexpr size_t sc_msg_size_size = sizeof(std::uint16_t);
        static constexpr size_t sc_msg_type_size = sizeof(std::uint8_t);
        static constexpr size_t sc_msgHeader_size = sc_msg_size_size + sc_msg_type_size;
    };

    struct format_msg_size {
        static constexpr size_t sc_msgHeader_size = msg_header_size::sc_msgHeader_size;
        static constexpr size_t sc_format_max_size = msgFormat_size;
        static constexpr size_t sc_formatMsg_max_size = sc_msgHeader_size + sc_format_max_size;
    };

    struct sub_msg_size {
        static constexpr size_t sc_msgHeader_size = msg_header_size::sc_msgHeader_size;
        static constexpr size_t sc_msg_multi_id_size = sizeof(std::uint8_t);
        static constexpr size_t sc_msg_id_size = sizeof(std::uint16_t);
        static constexpr size_t sc_msg_name_max_size = msgName_size;
        static constexpr size_t sc_addLoggedMsg_max_size = sc_msgHeader_size + sc_msg_multi_id_size + sc_msg_id_size + sc_msg_name_max_size;
    };

    //Scaling constats
    static constexpr size_t sc_max_data_formatMsg_number = formatMsg_quantity;
    static constexpr size_t sc_max_subMsg_number = subMsg_quantity;
    static constexpr size_t sc_max_dataMsg_number = sc_max_subMsg_number;

    //total setup message size
    static constexpr size_t sc_setupMsg_max_size = header_size::sc_header_size
                                                          + format_msg_size::sc_formatMsg_max_size *
                                                          sc_max_data_formatMsg_number
                                                          + sub_msg_size::sc_addLoggedMsg_max_size *
                                                          sc_max_subMsg_number;

    std::array<std::uint8_t, sc_setupMsg_max_size> setupMsg_buffer = {};
    std::uint16_t cursor = 0;

    //message data
    struct data_msg_size_s {
        static constexpr size_t sc_msgHeader_size = msg_header_size::sc_msgHeader_size;
        static constexpr size_t sc_msg_id_size = sub_msg_size::sc_msg_id_size;
        static constexpr size_t sc_max_data_size = dataMsg_size;
        static constexpr size_t sc_dataMsg_max_size = sc_msgHeader_size + sc_msg_id_size + sc_max_data_size;
    };

    std::array<std::uint8_t, data_msg_size_s::sc_dataMsg_max_size * sc_max_dataMsg_number> dataMsg_buffer = {};
    std::uint8_t dataBuffer_cursor = 0;

    std::uint8_t current_format_number = 0;
    std::uint8_t current_submessage_number = 0;
    std::uint8_t current_dataMsg_number = 0;

    /**private functions**/


protected:
    //Class specific error handler
    void error_handler() {
        while (true) {}
    }
};


