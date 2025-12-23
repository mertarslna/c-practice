#include "utility.hpp"
#include "abstraction.h"
void buttonControl(void) {
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) != GPIO_PIN_RESET);
    // (HAL_GPIO_ReadPin()) Bu fonksiyon, belirli bir GPIO pininin fiziksel durumunu okur.
}

std::uint64_t getTimestamp() {
    std::uint32_t time_stamp_high_1=__HAL_TIM_GET_COUNTER(&htim5);
    std::uint32_t time_stamp_low=__HAL_TIM_GET_COUNTER(&htim2);
    std::uint32_t time_stamp_high_2=__HAL_TIM_GET_COUNTER(&htim5);
    // fonksiyonu içindeki __HAL_TIM_GET_COUNTER(&htim5) ve __HAL_TIM_GET_COUNTER(&htim2) gibi makrolar:
    // Bu makrolar, belirli bir zamanlayıcının donanım sayacının mevcut değerini doğrudan okur.
    // Bu, zamanlayıcı donanımının kayıtçılarından veri alındığı için çok yüksek derecede donanım bağımlıdır.

    if (time_stamp_high_1 != time_stamp_high_2) {
        // Overflow occurred while reading low
        time_stamp_low = __HAL_TIM_GET_COUNTER(&htim2);
        time_stamp_high_1 = time_stamp_high_2;
    }
    return (static_cast<std::uint64_t>(time_stamp_high_1) << 32) | time_stamp_low;
}

void memcpy_be(void *dest, const void *src, size_t size) {
    const uint8_t *s = (const uint8_t *)src;
    std::uint8_t *d = (uint8_t *)dest;

    for (size_t i = 0; i < size; ++i) {
        d[i] = s[size - 1 - i];  // Reverse byte order
    }
}


