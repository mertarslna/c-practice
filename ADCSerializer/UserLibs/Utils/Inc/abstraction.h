#pragma once
#ifndef ABSTRACTION_H
#define ABSTRACTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma2;

extern bool log_adc_data_flag;
extern bool adcConvComp_flag;

void logAndTransmitAdcData(void);

void Error_Handler(void);

void SystemClock_Config(void);
void GPIO_Init(void);
void TIM1_Init(void);
void TIM2_Init(void);
void TIM5_Init(void);
void UART_Init(void);

void initialization(void);

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif /* ABSTRACTION_H */
