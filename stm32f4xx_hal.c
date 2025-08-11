#include "stm32f4xx_hal.h"
#include <stdio.h>

// Dummy peripheral instances
static GPIO_TypeDef gpioa_instance = {0};
static GPIO_TypeDef gpioc_instance = {0};
static ADC_TypeDef adc1_instance = {0};
static TIM_TypeDef tim1_instance = {0};
static TIM_TypeDef tim2_instance = {0};
static TIM_TypeDef tim5_instance = {0};
static USART_TypeDef usart2_instance = {0};
static DMA_Stream_TypeDef dma2_stream0_instance = {0};

// Global counter for timers (for simulation)
static uint32_t global_counter = 0;

HAL_StatusTypeDef HAL_Init(void) {
    printf("HAL_Init() called\n");
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef* RCC_OscInitStruct) {
    printf("HAL_RCC_OscConfig() called\n");
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef* RCC_ClkInitStruct, uint32_t FLatency) {
    printf("HAL_RCC_ClockConfig() called\n");
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef* htim) {
    printf("HAL_TIM_Base_Init() called for timer instance %p\n", (void*)htim->Instance);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_ConfigClockSource(TIM_HandleTypeDef* htim, TIM_ClockConfigTypeDef* sClockSourceConfig) {
    printf("HAL_TIM_ConfigClockSource() called\n");
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef* htim, TIM_MasterConfigTypeDef* sMasterConfig) {
    printf("HAL_TIMEx_MasterConfigSynchronization() called\n");
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro(TIM_HandleTypeDef* htim, TIM_SlaveConfigTypeDef* sSlaveConfig) {
    printf("HAL_TIM_SlaveConfigSynchro() called\n");
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef* htim) {
    printf("HAL_TIM_Base_Start_IT() called for timer instance %p\n", (void*)htim->Instance);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef* htim) {
    printf("HAL_TIM_Base_Start() called for timer instance %p\n", (void*)htim->Instance);
    // Simulate timer counter
    htim->Instance->CNT = global_counter++;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef* huart) {
    printf("HAL_UART_Init() called for UART instance %p with baud rate %lu\n", 
           (void*)huart->Instance, (unsigned long)huart->Init.BaudRate);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef* huart, uint8_t* pData, uint16_t Size, uint32_t Timeout) {
    printf("HAL_UART_Transmit() called - sending %d bytes: ", Size);
    for (uint16_t i = 0; i < Size && i < 16; i++) { // Limit output for readability
        printf("0x%02X ", pData[i]);
    }
    if (Size > 16) printf("...");
    printf("\n");
    return HAL_OK;
}

void HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_Init) {
    printf("HAL_GPIO_Init() called for GPIO port %p, pin 0x%lX\n", 
           (void*)GPIOx, (unsigned long)GPIO_Init->Pin);
}

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    printf("HAL_GPIO_WritePin() called - Port %p, Pin 0x%X, State %d\n", 
           (void*)GPIOx, GPIO_Pin, PinState);
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    printf("HAL_GPIO_ReadPin() called - Port %p, Pin 0x%X\n", (void*)GPIOx, GPIO_Pin);
    // Simulate button press (return SET to continue program execution)
    return GPIO_PIN_SET;
}