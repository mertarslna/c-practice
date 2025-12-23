#pragma once
#include<ULogSerializer.hpp>

template<size_t N_str>
static constexpr size_t get_formatSize(const char (&p)[N_str],size_t N_pins) {
    return N_str + sizeof(':') + sizeof("uint64_t timestamp;") + sizeof("uint16_t PAX;")*N_pins;
}

/**
 * @brief Specialization of ULogSerializer for ADC-based data sources.
 *
 * This class generates and initializes ULog message structures for ADC data,
 * including the setup of the message header, format description, and subscription block.
 * It uses compile-time ADC pin types to generate field names and formats at runtime.
 *
 * Inherits from:
 * - ULogSerializer: the base class that handles raw buffer formatting .
 */
template< size_t N,const char (&msg_name)[N],std::uint64_t header_timestamp , typename... Pins>
class ULogSerializer_ADC:public ULogSerializer<1,1,get_formatSize(msg_name,sizeof...(Pins)),N,2*sizeof...(Pins)+sizeof(uint64_t)>{
public:

    explicit ULogSerializer_ADC() {
        this->init_header(header_timestamp);
        process_format();
        process_subscription();
    }

private:
    /**
     * @brief Constructs the format string for the ULog message.
     *
     * The format string includes:
     * - A `timestamp` field of type `uint64_t`
     * - One `uint16_t` field per ADC pin, with names taken from `adcPinMaps<Pins>::dataFieldName`
     *
     * @tparam Pins Variadic ADC pin marker types
     * @param msg_name Name of the ULog message used as a prefix
     */
    void process_format() {
        std::string format = msg_name;
        format.append(":uint64_t timestamp;");
        ((format.append("uint16_t "), format.append((adcPinMaps<Pins>::dataFieldName)), format.append(";")), ...);
        this->init_format(format.c_str());
    }

    /**
     * @brief Registers the ULog message subscription.
     *
     * Associates the message name with a message ID and optional multi-instance ID.
     * A single subscription is required for this class specialization.
     *
     * @param msg_name Name of the message type being subscribed toz
     */
    void process_subscription() {
        std::uint8_t multi_id = 0;
        std::uint16_t msg_id = 0;
        this->init_subscription(multi_id, msg_id, msg_name);
    }

protected:
};

