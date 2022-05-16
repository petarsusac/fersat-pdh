/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t current_sample_count;
volatile uint8_t rx_buffer[NUM_SAMPLES][BYTES_PER_SAMPLE];
volatile uint8_t start;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void ADC_first_read();
void wait_for_10_ms();
void ADC_SPI_DMA_init();
void ADC_DRDY_interrupt_handler();
void DMA_Transfer_Complete_interrupt_handler();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_DMA_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  NVIC_DisableIRQ(EXTI4_IRQn); // disable DRDY interrupts until setup is completed
  ADC_first_read();
  ADC_SPI_DMA_init();
  NVIC_EnableIRQ(EXTI4_IRQn);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (current_sample_count < NUM_SAMPLES) {
		  if (start == 1) {
			NVIC_DisableIRQ(EXTI4_IRQn);
			start = 0;

			LL_GPIO_ResetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN);
			LL_SPI_Enable(SB_SPIx);

			// send 30 dummy bytes to drive SPI clock, meanwhile DMA handles RX transfer
			for (int i = 0; i < 30; i++) {
				while ( !LL_SPI_IsActiveFlag_TXE(SB_SPIx) ); // wait for TXE
				LL_SPI_TransmitData8(SB_SPIx, 0x00); // transmit dummy byte
			}

			current_sample_count++;

			// communication end is handled in DMA Transfer Complete ISR
		  }
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_Init1msTick(16000000);

  LL_SetSystemCoreClock(16000000);
}

/* USER CODE BEGIN 4 */
void ADC_first_read() {
	uint8_t rx_buffer[30];

	LL_GPIO_ResetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN); // \CS low
	LL_SPI_Enable(SB_SPIx);

	LL_SPI_TransmitData8(SB_SPIx, 0x00);

	for (int i = 0; i <= 28; i++) {
	  while ( !LL_SPI_IsActiveFlag_TXE(SB_SPIx) ); // wait until TXE is set
	  LL_SPI_TransmitData8(SB_SPIx, 0x00);
	  while ( !LL_SPI_IsActiveFlag_RXNE(SB_SPIx) ); // wait until RXNE is set
	  rx_buffer[i] = LL_SPI_ReceiveData8(SB_SPIx); // read DR (clears RXNE)
	}

	while ( !LL_SPI_IsActiveFlag_RXNE(SB_SPIx) ); // wait until RXNE is set
	rx_buffer[29] = LL_SPI_ReceiveData8(SB_SPIx); // read DR (clears RXNE)

	LL_GPIO_SetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN); // \CS high

	for(int i = 0; i < 100; i++); // TODO: write a delay function to replace this

	LL_GPIO_ResetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN); // \CS low

	LL_SPI_TransmitData8(SB_SPIx, 0x00);

	for (int i = 0; i <= 28; i++) {
	  while ( !LL_SPI_IsActiveFlag_TXE(SB_SPIx) ); // wait until TXE is set
	  LL_SPI_TransmitData8(SB_SPIx, 0x00);
	  while ( !LL_SPI_IsActiveFlag_RXNE(SB_SPIx) ); // wait until RXNE is set
	  rx_buffer[i] = LL_SPI_ReceiveData8(SB_SPIx); // read DR (clears RXNE)
	}

	while ( !LL_SPI_IsActiveFlag_RXNE(SB_SPIx) ); // wait until RXNE is set
	rx_buffer[29] = LL_SPI_ReceiveData8(SB_SPIx); // read DR (clears RXNE)

	LL_GPIO_SetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN); // \CS high

	while (LL_SPI_GetTxFIFOLevel(SB_SPIx) != LL_SPI_TX_FIFO_EMPTY); // wait for TX FIFO to become empty
	while (LL_SPI_IsActiveFlag_BSY(SB_SPIx)); // wait for BSY flag to go low
	LL_SPI_Disable(SB_SPIx);
}

void wait_for_10_ms() {
	// 16 MHz APB1 clock before timer prescaler
	LL_TIM_SetPrescaler(TIM2, 16); // 1 MHz after prescaler
	LL_TIM_SetAutoReload(TIM2, 10000); // set auto-reload value
	LL_TIM_GenerateEvent_UPDATE(TIM2); // generate update event
	LL_TIM_ClearFlag_UPDATE(TIM2);
	LL_TIM_EnableCounter(TIM2); // enable counter
	while ( !LL_TIM_IsActiveFlag_UPDATE(TIM2) ); // wait for update flag
	LL_TIM_ClearFlag_UPDATE(TIM2); // clear update flag
}

void ADC_SPI_DMA_init() {
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_CHANNEL_1, LL_SPI_DMA_GetRegAddr(SB_SPIx));
	LL_DMA_EnableIT_TC(DMA2, LL_DMA_CHANNEL_1);
}

void ADC_DRDY_interrupt_handler() {
	LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_1, 30);
	LL_DMA_SetMemoryAddress(DMA2, LL_DMA_CHANNEL_1, (uint32_t) rx_buffer[current_sample_count]);

	LL_SPI_EnableDMAReq_RX(SB_SPIx);
	LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_1);
	LL_SPI_EnableIT_RXNE(SB_SPIx);

	start = 1;
}

void DMA_Transfer_Complete_interrupt_handler() {
	LL_DMA_ClearFlag_TC1(DMA2);

	LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_1);

	// SPI Disable procedure
	while (LL_SPI_GetTxFIFOLevel(SB_SPIx) != LL_SPI_TX_FIFO_EMPTY); // wait for TX FIFO to become empty
	while (LL_SPI_IsActiveFlag_BSY(SB_SPIx)); // wait for BSY flag to go low
	LL_SPI_Disable(SB_SPIx);

	LL_SPI_DisableDMAReq_RX(SB_SPIx);

	LL_GPIO_SetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN);

	NVIC_EnableIRQ(EXTI4_IRQn);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
