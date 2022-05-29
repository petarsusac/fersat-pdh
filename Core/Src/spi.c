/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* SPI3 init function */
void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  /**SPI3 GPIO Configuration
  PC10   ------> SPI3_SCK
  PC11   ------> SPI3_MISO
  PC12   ------> SPI3_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* SPI3 DMA Init */

  /* SPI3_RX Init */
  LL_DMA_SetPeriphRequest(DMA2, LL_DMA_CHANNEL_1, LL_DMA_REQUEST_3);

  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA2, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA2, LL_DMA_CHANNEL_1, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_BYTE);

  /* SPI3_TX Init */
  LL_DMA_SetPeriphRequest(DMA2, LL_DMA_CHANNEL_2, LL_DMA_REQUEST_3);

  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA2, LL_DMA_CHANNEL_2, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA2, LL_DMA_CHANNEL_2, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_CHANNEL_2, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_CHANNEL_2, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_CHANNEL_2, LL_DMA_MDATAALIGN_BYTE);

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI3, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI3, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_DisableNSSPulseMgt(SPI3);
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/* USER CODE BEGIN 1 */

void SPI_Enable_DMA_Rx_Request(SPI_TypeDef *SPIx) {
	LL_SPI_EnableDMAReq_RX(SPIx);
}

void SPI_Enable_DMA_Tx_Request(SPI_TypeDef *SPIx) {
	LL_SPI_EnableDMAReq_TX(SPIx);
}

void SPI_Disable_DMA_Requests(SPI_TypeDef *SPIx) {
	LL_SPI_DisableDMAReq_RX(SPIx);
	LL_SPI_DisableDMAReq_TX(SPIx);
}

void SPI_Enable_RXNE_TXE_Interrupts(SPI_TypeDef *SPIx) {
	LL_SPI_EnableIT_RXNE(SPIx);
	LL_SPI_EnableIT_TXE(SPIx);
}

void SPI_Start_Transfer(SPI_TypeDef *SPIx) {
	LL_GPIO_ResetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN);
	LL_SPI_Enable(SPIx);
}


void SPI_TransmitReceive(SPI_TypeDef *SPIx, uint8_t len, uint8_t *tx_buffer, uint8_t *rx_buffer) {
	LL_SPI_Enable(SPIx);

	LL_SPI_TransmitData8(SPIx, tx_buffer[0]);

	for (int i = 0; i < len - 1; i++) {
	  while ( !LL_SPI_IsActiveFlag_TXE(SPIx) ); // wait until TXE is set
	  LL_SPI_TransmitData8(SPIx, tx_buffer[i + 1]);
	  while ( !LL_SPI_IsActiveFlag_RXNE(SPIx) ); // wait until RXNE is set
	  rx_buffer[i] = LL_SPI_ReceiveData8(SPIx); // read DR (clears RXNE)
	}

	while ( !LL_SPI_IsActiveFlag_RXNE(SPIx) ); // wait until RXNE is set
	rx_buffer[len - 1] = LL_SPI_ReceiveData8(SPIx); // read DR (clears RXNE)
}

void SPI_Disable(SPI_TypeDef *SPIx) {
	while (LL_SPI_GetTxFIFOLevel(SPIx) != LL_SPI_TX_FIFO_EMPTY); // wait for TX FIFO to become empty
	while (LL_SPI_IsActiveFlag_BSY(SPIx)); // wait for BSY flag to go low
	LL_SPI_Disable(SPIx);
}

void SPI_Set_Mode(SPI_mode mode, SPI_TypeDef *SPIx) {
	if (mode == CPOL1_CPHA1) {
		LL_SPI_SetClockPolarity(SPIx, LL_SPI_POLARITY_HIGH);
		LL_SPI_SetClockPhase(SPIx, LL_SPI_PHASE_2EDGE);
	} else if (mode == CPOL0_CPHA1) {
		LL_SPI_SetClockPolarity(SPIx, LL_SPI_POLARITY_LOW);
		LL_SPI_SetClockPhase(SPIx, LL_SPI_PHASE_2EDGE);
	}
}
/* USER CODE END 1 */
