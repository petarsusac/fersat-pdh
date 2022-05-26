#ifndef INC_SENSOR_BOARD_H_
#define INC_SENSOR_BOARD_H_

#include "ads131m08.h"

#define SB_SPIx SPI3
#define SB_DMAx DMA2

#define TEMP1_CS_GPIOx	GPIOA
#define TEMP1_CS_PIN	LL_GPIO_PIN_3
#define TEMP2_CS_GPIOx	GPIOA
#define TEMP2_CS_PIN	LL_GPIO_PIN_10
#define TEMP3_CS_GPIOx	GPIOA
#define TEMP3_CS_PIN	LL_GPIO_PIN_11

typedef struct {
	ADS131M08 adc;
} Sensor_Board;

void SB_Init(Sensor_Board sb);

#endif /* INC_SENSOR_BOARD_H_ */
