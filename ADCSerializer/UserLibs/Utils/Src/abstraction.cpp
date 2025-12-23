#include "abstraction.h"
#include "msgPackSerializer.hpp"
#include <utility>
#include "AdcDmaController.hpp"
#include "ULogSerializer_ADC.hpp"
#include "msgPackSerializer_ADC.h"

ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma2;

bool log_adc_data_flag = false;
bool adcConvComp_flag = false;

#define REAL_TIME_COM 0 // ulog için 0, msgpack için 1

template<size_t N, const char (&msg_name)[N],typename ...Pins>
static auto adc_controller_serializer_init(auto instance_tuple, Pins...pins) {
#if REAL_TIME_COM
  static msgPackSerializer_ADC<N,msg_name,Pins...>adc_ser;
#else
  constexpr std::uint64_t header_timestamp = 1000;
  static ULogSerializer_ADC<N,msg_name,header_timestamp,Pins...> adc_ser;
#endif
  static AdcDmaController adc_cnt(instance_tuple,pins...);
  return std::tie(adc_ser,adc_cnt);
}

void logAndTransmitAdcData(void) {

  const auto instance_tuple = std::make_tuple(ADC1,&hadc1,DMA2_Stream0,&hdma2);
  static constexpr char adc_name[] = "ADC1";
  constexpr size_t name_size = string_counter(adc_name);
  auto&& [adc1_ser,adc1_cnt] = adc_controller_serializer_init<name_size,adc_name>(instance_tuple,ADC_Pins::PA0,ADC_Pins::PA1,ADC_Pins::PA4,ADC_Pins::PA5);

#if !REAL_TIME_COM
  auto&& setupTxBuff = adc1_ser.get_ulogSetupMsg();
  if (HAL_UART_Transmit(&huart2,setupTxBuff.data(),setupTxBuff.size(),1000) != 0x00U)
    // (HAL_UART_Transmit()) Bu fonksiyon, UART çevre birimi aracılığıyla veri gönderir.
    Error_Handler();
#endif
  //begin adc sampling
  if (HAL_TIM_Base_Start_IT(&htim1) != 0x00U)
    Error_Handler();

  if (HAL_TIM_Base_Start(&htim2) != 0x00U)
    Error_Handler();

  if (HAL_TIM_Base_Start(&htim5) != 0x00U)
    Error_Handler();

  while (1)
  {
    if (log_adc_data_flag) {
      adc1_cnt.get_adc_data();
      while (adcConvComp_flag == false)
        __WFI(); // sleep until interrupt to save power
      // Bu, "Wait For Interrupt" (Kesme beklet) komutudur.
      // İşlemciyi düşük güç moduna sokar ve bir kesme (interrupt) gelene kadar bekler.
      // Bu, işlemcinin komut setine özgü, düşük seviyeli bir donanım komutudur

#if !REAL_TIME_COM
      adc1_ser.set_ulogDataMsg(0,getTimestamp(),adc1_cnt.adcDataBuffer);
      auto&& adcDataTxBuffer = adc1_ser.get_ulogDataMsg();
#else
      adc1_ser.set_msgPackDataMsg(getTimestamp(),adc1_cnt.adcDataBuffer);
      auto&& adcDataTxBuffer = adc1_ser.get_msgPackDataMsg();

      uint32_t size = adcDataTxBuffer.size();
      std::array<uint8_t, 4> big_endian_bytes;
      memcpy_be(big_endian_bytes.data(),&size,sizeof(size));

      if (HAL_UART_Transmit(&huart2,big_endian_bytes.data(),big_endian_bytes.size(),1000) != HAL_OK)
        Error_Handler();
#endif
      if (HAL_UART_Transmit(&huart2,adcDataTxBuffer.data(),adcDataTxBuffer.size(),1000) != HAL_OK)
        Error_Handler();
      log_adc_data_flag=false;
      adcConvComp_flag=false;
     }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void GPIO_Init(void) {
  // Bu fonksiyon, GPIO (General Purpose Input/Output) pinlerinin nasıl davranacağını
  // (örneğin, giriş veya çıkış,pull-up/pull-down dirençleri, hız vb.) belirler.
  // Bu, fiziksel pinlerin işlevlerini tanımladığı için donanıma doğrudan bağımlıdır.
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
void TIM1_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 10000;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 840/2-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
void TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 84-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
void TIM5_Init(void)
{
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 84;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  if (HAL_TIM_SlaveConfigSynchro(&htim5, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

void initialization(void) {
  HAL_Init();
  // Bu fonksiyon, STM32'nin HAL (Hardware Abstraction Layer) kütüphanesini başlatır.
  // HAL, donanımı soyutlayan bir katman olsa da, kendisi belirli bir mikrodenetleyici ailesi için yazılmıştır
  // ve donanıma sıkı sıkıya bağlıdır.

  SystemClock_Config();

  GPIO_Init();
  TIM1_Init();
  TIM2_Init();
  TIM5_Init();
  UART_Init();
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}


extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // (HAL_ADC_ConvCpltCallback()) Bu fonksiyonlar, donanım kesmeleri (interrupts) tarafından çağrılan geri çağırım (callback),
  // fonksiyonlarıdır. Bir zamanlayıcı periyodu dolduğunda veya ADC dönüşümü tamamlandığında otomatik olarak çalıştırılırlar.
  // Bu davranış, mikrodenetleyicinin kesme denetleyicisi (interrupt controller) donanımına özgüdür.

  if (htim->Instance == TIM1) {
    log_adc_data_flag=true;
  }
}

extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  // (HAL_ADC_ConvCpltCallback()) Bu fonksiyonlar, donanım kesmeleri (interrupts) tarafından çağrılan geri çağırım (callback),
  // fonksiyonlarıdır. Bir zamanlayıcı periyodu dolduğunda veya ADC dönüşümü tamamlandığında otomatik olarak çalıştırılırlar.
  // Bu davranış, mikrodenetleyicinin kesme denetleyicisi (interrupt controller) donanımına özgüdür.

  adcConvComp_flag = true;
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */