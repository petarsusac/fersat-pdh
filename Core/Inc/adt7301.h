#ifndef INC_ADT7301_H_
#define INC_ADT7301_H_

#include "spi.h"
#include "gpio.h"

#define TEMP1_CS_GPIOx	GPIOA
#define TEMP1_CS_PIN	LL_GPIO_PIN_3
#define TEMP2_CS_GPIOx	GPIOA
#define TEMP2_CS_PIN	LL_GPIO_PIN_10
#define TEMP3_CS_GPIOx	GPIOA
#define TEMP3_CS_PIN	LL_GPIO_PIN_11

typedef struct {
	SPI_TypeDef *SPIx;
	float sample;
} ADT7301;

typedef enum {
	TEMP1,
	TEMP2,
	TEMP3
} temp_sensor;

void ADT7301_Init(ADT7301 *adt7301_struct, SPI_TypeDef *SPIx);
void ADT7301_Collect_Sample(temp_sensor ts);
void ADT7301_Shutdown(temp_sensor ts);
void ADT7301_Wakeup(temp_sensor ts);

#endif /* INC_ADT7301_H_ */
