#include "adt7301.h"

ADT7301 *adt7301;

void ADT7301_Init(ADT7301 *adt7301_struct, SPI_TypeDef *SPIx) {
	adt7301_struct->SPIx = SPIx;
	adt7301 = adt7301_struct;
}

void ADT7301_Collect_Sample(temp_sensor ts) {
	uint8_t sample_bytes[2];
	uint8_t dummy_bytes[2] = {0};

	if (ts == TEMP1) {
		LL_GPIO_ResetOutputPin(TEMP1_CS_GPIOx, TEMP1_CS_PIN);
		SPI_TransmitReceive(adt7301->SPIx, 2, dummy_bytes, sample_bytes);
		LL_GPIO_SetOutputPin(TEMP1_CS_GPIOx, TEMP1_CS_PIN);
	}

	int16_t tmp = (sample_bytes[0] << 8) | sample_bytes[1];

	if ((tmp & (1 << 13)) != 0) {
		tmp -= 16384;
	}

	adt7301->sample = (float)tmp;
	adt7301->sample /= 32.;
}

void ADT7301_Shutdown(temp_sensor ts) {
	uint8_t dummy_buffer[2];
	uint8_t shutdown_cmd[2] = {0x20, 0x00};

	if (ts == TEMP1) {
		LL_GPIO_ResetOutputPin(TEMP1_CS_GPIOx, TEMP1_CS_PIN);
		SPI_TransmitReceive(adt7301->SPIx, 2, shutdown_cmd, dummy_buffer);
		LL_GPIO_SetOutputPin(TEMP1_CS_GPIOx, TEMP1_CS_PIN);
	}
}

void ADT7301_Wakeup(temp_sensor ts) {
	uint8_t dummy_buffer[2];
	uint8_t wakeup_cmd[2] = {0};

	if (ts == TEMP1) {
		LL_GPIO_ResetOutputPin(TEMP1_CS_GPIOx, TEMP1_CS_PIN);
		SPI_TransmitReceive(adt7301->SPIx, 2, wakeup_cmd, dummy_buffer);
		LL_GPIO_SetOutputPin(TEMP1_CS_GPIOx, TEMP1_CS_PIN);
	}
}



