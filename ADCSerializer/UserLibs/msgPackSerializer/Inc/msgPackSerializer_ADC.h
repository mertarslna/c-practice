#pragma once
#include "msgPackSerializer.hpp"


template<size_t N,const char (&msg_name)[N],typename ...Adc_Pin>
class msgPackSerializer_ADC:public msgPackSerializer<1+1+N+1+8*sizeof...(Adc_Pin)+
                                                    1+sizeof("timestamp")+1+sizeof(uint64_t)> {
public:

    explicit msgPackSerializer_ADC(){
        this->append_map(2);
        this->append_data(msg_name);
        this->append_map(sizeof...(Adc_Pin));
        uint16_t defaultData_16 = 0xFF;
        uint64_t defaultData_64 = 0;
        uint16_t adc_data_cursor = 0;
        ((this->append_data(adcPinMaps<Adc_Pin>::dataFieldName),
            adc_data_addrs[adc_data_cursor++]=this->cursor+1,
            this->append_data(defaultData_16)), ...);
        this->append_data("timestamp");
        adc_data_addrs[adc_data_cursor] = this->cursor+1;
        this->append_data(defaultData_64);
    }

    template<typename DataSpan>
    void set_msgPackDataMsg(std::uint64_t timestamp, DataSpan data) {
        std::span<const uint8_t> data_bytes{
            reinterpret_cast<const uint8_t *>(data.data()),
            data.size_bytes()
        };
        std::uint64_t timestamp_us = timestamp;
        uint8_t indx = 0;
        for (; indx < sizeof...(Adc_Pin); indx++) {
            memcpy_be(&this->dataMsg_buffer[adc_data_addrs[indx]],&data_bytes[indx*2],sizeof(uint16_t));
        }
        memcpy_be(&this->dataMsg_buffer[adc_data_addrs[indx]],&timestamp_us,sizeof(timestamp_us));
    }

private:
    std::array<uint8_t,sizeof...(Adc_Pin)+1> adc_data_addrs;



};