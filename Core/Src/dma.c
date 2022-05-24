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
void ADC_SPI_DMA_init() {
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_CHANNEL_1, LL_SPI_DMA_GetRegAddr(SB_SPIx));
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_CHANNEL_2, LL_SPI_DMA_GetRegAddr(SB_SPIx));
	LL_DMA_SetMemoryAddress(DMA2, LL_DMA_CHANNEL_2, (uint32_t) dummy_bytes);
	LL_DMA_EnableIT_TC(DMA2, LL_DMA_CHANNEL_1);
	LL_DMA_EnableIT_TC(DMA2, LL_DMA_CHANNEL_2);
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
	// SPI Disable procedure
	SPI_Disable(SPIx);

	LL_SPI_DisableDMAReq_RX(SPIx);
	LL_SPI_DisableDMAReq_TX(SPIx);

	LL_GPIO_SetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN);
	NVIC_EnableIRQ(EXTI4_IRQn);
}
/* USER CODE END 2 */

