#pragma once
#include "main.h"

struct PA0_{}; struct PA1_{};
struct PA2_{}; struct PA3_{};
struct PA4_{}; struct PA5_{};
struct PA6_{}; struct PA7_{};

/**
 * @brief ADC pin mapping traits.
 *
 * Specializations of this template provide compile-time mapping between
 * GPIO pin types and their corresponding ADC hardware configuration.
 *
 * These are:
 *  - ADC channel index (e.g., ADC_CHANNEL_0)
 *  - GPIO port letter (e.g., 'A')
 *  - GPIO pin mask (e.g., GPIO_PIN_0)
 *  - A string field name used for logging
 *
 * @tparam Pin The pin marker type (e.g., `PA0_`, `PA1_`, etc.)
 */
template<typename Pin>
struct adcPinMaps;

template<> struct adcPinMaps<PA0_> {
    static constexpr uint32_t channel = ADC_CHANNEL_0;
    static constexpr char port = 'A';
    static constexpr uint32_t pin = GPIO_PIN_0;
    static constexpr const char* dataFieldName = "PA0";
};

template<> struct adcPinMaps<PA1_> {
    static constexpr uint32_t channel = ADC_CHANNEL_1;
    static constexpr char port = 'A';
    static constexpr uint32_t pin = GPIO_PIN_1;
    static constexpr const char* dataFieldName = "PA1";
};

template<> struct adcPinMaps<PA2_> {
    static constexpr uint32_t channel = ADC_CHANNEL_2;
    static constexpr char port = 'A';
    static constexpr uint32_t pin = GPIO_PIN_2;
    static constexpr  const char* dataFieldName = "PA2";
};

template<> struct adcPinMaps<PA3_> {
    static constexpr uint32_t channel = ADC_CHANNEL_3;
    static constexpr char port = 'A';
    static constexpr uint32_t pin = GPIO_PIN_3;
    static constexpr const char* dataFieldName = "PA3";
};

template<> struct adcPinMaps<PA4_> {
    static constexpr uint32_t channel = ADC_CHANNEL_4;
    static constexpr char port = 'A';
    static constexpr uint32_t pin = GPIO_PIN_4;
    static constexpr const char* dataFieldName = "PA4";
};

template<> struct adcPinMaps<PA5_> {
    static constexpr uint32_t channel = ADC_CHANNEL_5;
    static constexpr char port = 'A';
    static constexpr uint32_t pin = GPIO_PIN_5;
    static constexpr const char* dataFieldName = "PA5";
};

template<> struct adcPinMaps<PA6_> {
    static constexpr uint32_t channel = ADC_CHANNEL_6;
    static constexpr char port = 'A';
    static constexpr uint32_t pin = GPIO_PIN_6;
    static constexpr const char* dataFieldName = "PA6";
};

template<> struct adcPinMaps<PA7_> {
    static constexpr uint32_t channel = ADC_CHANNEL_7;
    static constexpr char port = 'A';
    static constexpr uint32_t pin = GPIO_PIN_7;
    static constexpr const char* dataFieldName = "PA7";
};

/**
 * @brief Static namespace for ADC pin marker types.
 *
 * This struct provides constexpr instances of marker types representing
 * available ADC-capable pins. These are used to select channels at compile time.
 *
 */
struct ADC_Pins {
    static constexpr PA0_ PA0 = {}; static constexpr PA1_ PA1 = {}; static constexpr PA2_ PA2 = {}; static constexpr PA3_ PA3 = {};
    static constexpr PA4_ PA4 = {}; static constexpr PA5_ PA5 = {}; static constexpr PA6_ PA6 = {}; static constexpr PA7_ PA7 = {};
};