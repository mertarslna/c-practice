#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Pin definitions */
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

#ifdef __cplusplus
}

#include <cstring>
#include <array>

/* ADC pin enum */
enum class ADC_Pins : uint32_t {
    PA0 = 0,
    PA1 = 1,
    PA4 = 4,
    PA5 = 5
};

/* Utility function for string length calculation */
template<size_t N>
constexpr size_t string_counter(const char (&)[N]) {
    return N - 1; // Exclude null terminator
}

/* Big endian memory copy function */
inline void memcpy_be(uint8_t* dest, const void* src, size_t size) {
    const uint8_t* src_bytes = static_cast<const uint8_t*>(src);
    for (size_t i = 0; i < size; ++i) {
        dest[i] = src_bytes[size - 1 - i];
    }
}

#endif

#endif /* __MAIN_H */