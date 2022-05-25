/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.h
  * @brief   This file contains all the function prototypes for
  *          the dma.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* DMA memory to memory transfer handles -------------------------------------*/

/* USER CODE BEGIN Includes */
#include "spi.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_DMA_Init(void);

/* USER CODE BEGIN Prototypes */
void DMA_Channel_Init(DMA_TypeDef *DMAx, uint32_t channel, uint32_t periph_addr, uint8_t *mem_addr);
void DMA_Set_Channel_Data_Length(DMA_TypeDef *DMAx, uint32_t channel, uint32_t length);
void DMA_Reload_Memory_Address(DMA_TypeDef *DMAx, uint32_t channel, uint8_t *mem_addr);
void DMA_Transfer_Complete_RX_interrupt_handler();
void DMA_Transfer_Complete_TX_interrupt_handler();
void DMA_Enable_CH1_CH2(DMA_TypeDef *DMAx);
void DMA_Disable(SPI_TypeDef *SPIx);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */

