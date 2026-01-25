#ifndef ADC_DMA_CONTROLLER_HPP
#define ADC_DMA_CONTROLLER_HPP

#include "main.h"
#include <tuple>
#include <array>

class AdcDmaController {
public:
    std::array<uint16_t, 4> adcDataBuffer;
    
    template<typename... Pins>
    AdcDmaController(auto instance_tuple, Pins... pins) {
        // TODO: Initialize ADC DMA controller with given pins
        adcDataBuffer.fill(0);
    }
    
    void get_adc_data() {
        // TODO: Trigger ADC DMA conversion
        // This would normally start the ADC conversion process
    }
};

#endif // ADC_DMA_CONTROLLER_HPP