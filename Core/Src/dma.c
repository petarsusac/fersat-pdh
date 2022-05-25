/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.c
  * @brief   This file provides code for the configuration
  *          of all the requested memory to memory DMA transfers.
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
#include "dma.h"
#include "ads131m08.h"
/* USER CODE BEGIN 0 */
volatile uint8_t tc = 0;
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

/**
  * Enable DMA controller clock
  */
void MX_DMA_Init(void)
{

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

  /* DMA interrupt init */
  /* DMA2_Channel1_IRQn interrupt configuration */
  NVIC_SetPriority(DMA2_Channel1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(DMA2_Channel1_IRQn);
  /* DMA2_Channel2_IRQn interrupt configuration */
  NVIC_SetPriority(DMA2_Channel2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(DMA2_Channel2_IRQn);

}

/* USER CODE BEGIN 2 */
void DMA_Channel_Init(DMA_TypeDef *DMAx, uint32_t channel, uint32_t periph_addr, uint8_t *mem_addr) {
	LL_DMA_SetPeriphAddress(DMAx, channel, periph_addr);
	LL_DMA_SetMemoryAddress(DMAx, channel, (uint32_t) mem_addr);
	LL_DMA_EnableIT_TC(DMAx, channel);
}

void DMA_Set_Channel_Data_Length(DMA_TypeDef *DMAx, uint32_t channel, uint32_t length) {
	LL_DMA_SetDataLength(DMAx, channel, length);
}

void DMA_Reload_Memory_Address(DMA_TypeDef *DMAx, uint32_t channel, uint8_t *mem_addr) {
	LL_DMA_SetMemoryAddress(DMAx, channel, (uint32_t) mem_addr);
}

void DMA_Enable_CH1_CH2(DMA_TypeDef *DMAx) {
	LL_DMA_EnableChannel(DMAx, LL_DMA_CHANNEL_1);
	LL_DMA_EnableChannel(DMAx, LL_DMA_CHANNEL_2);
}

void DMA_Transfer_Complete_RX_interrupt_handler() {
	LL_DMA_ClearFlag_TC1(DMA2);
	LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_1);

	if (tc == 1) {
		DMA_Disable(SB_SPIx);
		tc = 0;
	} else {
		tc = 1;
	}
}

void DMA_Transfer_Complete_TX_interrupt_handler() {
	LL_DMA_ClearFlag_TC2(DMA2);
	LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_2);

	if (tc == 1) {
		DMA_Disable(SB_SPIx);
		tc = 0;
	} else {
		tc = 1;
	}
}


void DMA_Disable(SPI_TypeDef *SPIx) {
	SPI_Disable(SPIx);

	SPI_Disable_DMA_Requests(SPIx);

	LL_GPIO_SetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN);
	NVIC_EnableIRQ(EXTI4_IRQn);
}
/* USER CODE END 2 */

