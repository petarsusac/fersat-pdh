#include "sensor_board.h"

void SB_Init(Sensor_Board sb) {
	ADS131M08 adc;
	ADC_Init(&adc, SB_SPIx, SB_DMAx);
}



