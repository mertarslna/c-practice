#pragma once
#include <string>
#include <tuple>
#include <array>
#include <span>
#include "main.h"
#include "ADCPinMap.hpp"

template<typename... Channel_Pins>
class AdcDmaController {
public:
    template<typename Instance_Tuple>
    explicit AdcDmaController(Instance_Tuple& instance_tuple, Channel_Pins&... channel_pins) {
        adcDataBuffer = std::span<uint16_t>(adc_data_stack_mem);
        std::apply([this](auto&&... args) { initialize_instances(args...); }, instance_tuple);
        (initialize_channel(channel_pins), ...);
    }

    /// Start ADC conversion using DMA
    void get_adc_data() {
        HAL_ADC_Start_DMA(m_hadc, reinterpret_cast<std::uint32_t*>(adcDataBuffer.data()), adcDataBuffer.size());
    }

    /// Span view of the ADC DMA buffer
    std::span<uint16_t> adcDataBuffer;

private:
    ADC_HandleTypeDef* m_hadc;

    static constexpr uint8_t channel_count = sizeof...(Channel_Pins);
    uint16_t adc_data_stack_mem[channel_count];
    uint8_t current_rank = 0;

    template<typename Pin>
    void initialize_channel(Pin pin) {
        using Traits = adcPinMaps<Pin>;

        init_gpio(Traits::port, Traits::pin);

        ADC_ChannelConfTypeDef config{};
        config.Channel = Traits::channel;
        config.Rank = ++current_rank;
        config.SamplingTime = ADC_SAMPLETIME_3CYCLES; // default

        HAL_ADC_ConfigChannel(m_hadc, &config);
    }

    void configure_adc(ADC_HandleTypeDef* hadc)
    {
        hadc->Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV8;
        hadc->Init.Resolution            = ADC_RESOLUTION_12B;
        hadc->Init.ScanConvMode          = ENABLE;
        hadc->Init.ContinuousConvMode    = DISABLE;
        hadc->Init.DiscontinuousConvMode = DISABLE;

        hadc->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
        hadc->Init.ExternalTrigConv      = ADC_SOFTWARE_START;
        hadc->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
        hadc->Init.NbrOfConversion       = channel_count;
        hadc->Init.DMAContinuousRequests = DISABLE;
        hadc->Init.EOCSelection          = ADC_EOC_SEQ_CONV;
    }

    void configure_dma(DMA_HandleTypeDef* hdma) {
        hdma->Init = {
            .Channel = DMA_CHANNEL_0,
            .Direction = DMA_PERIPH_TO_MEMORY,
            .PeriphInc = DMA_PINC_DISABLE,
            .MemInc = DMA_MINC_ENABLE,
            .PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD,
            .MemDataAlignment = DMA_MDATAALIGN_HALFWORD,
            .Mode = DMA_NORMAL,
            .Priority = DMA_PRIORITY_LOW,
            .FIFOMode = DMA_FIFOMODE_DISABLE
        };
    }

    void initialize_instances(ADC_TypeDef* adc_inst, ADC_HandleTypeDef* hadc,
                              DMA_Stream_TypeDef* dma_inst, DMA_HandleTypeDef* hdma) {
        hadc->Instance = adc_inst;
        hdma->Instance = dma_inst;

        configure_adc(hadc);
        configure_dma(hdma);

        enable_adc_clock(adc_inst);
        HAL_ADC_Init(hadc);

        enable_dma_clock(dma_inst);
        HAL_DMA_Init(hdma);

        link_dma(hadc, hdma);
        m_hadc = hadc;
    }

    void enable_adc_clock(ADC_TypeDef* instance) {
        if (instance == ADC1) __HAL_RCC_ADC1_CLK_ENABLE();
        // Add other ADC instances if needed
    }

    void enable_dma_clock(DMA_Stream_TypeDef* stream) {
        __HAL_RCC_DMA2_CLK_ENABLE();

        if (stream == DMA2_Stream0) {
            HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
        } else if (stream == DMA2_Stream4) {
            HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
        }
    }

    void link_dma(ADC_HandleTypeDef* hadc, DMA_HandleTypeDef* hdma) {
        hadc->DMA_Handle = hdma;
        hdma->Parent = hadc;
    }

    void init_gpio(char portChar, std::uint32_t pin) {
        GPIO_TypeDef* port = nullptr;

        switch (portChar) {
            case 'A': __HAL_RCC_GPIOA_CLK_ENABLE(); port = GPIOA; break;
            case 'B': __HAL_RCC_GPIOB_CLK_ENABLE(); port = GPIOB; break;
            case 'C': __HAL_RCC_GPIOC_CLK_ENABLE(); port = GPIOC; break;
            // Add more ports as needed
        }

        GPIO_InitTypeDef gpio = {
            .Pin = pin,
            .Mode = GPIO_MODE_ANALOG,
            .Pull = GPIO_NOPULL
        };
        HAL_GPIO_Init(port, &gpio);
    }
};

/// CTAD guide (C++17 class template argument deduction)
template <typename Instance_Tuple, typename... Channel_Pins>
AdcDmaController(Instance_Tuple&&, Channel_Pins&&...) -> AdcDmaController<std::decay_t<Channel_Pins>...>;
