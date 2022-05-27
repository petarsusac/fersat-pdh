#include "sensor_board.h"

Sensor_Board *sensor_board;
uint16_t aligned_samples[NUM_SAMPLES * 8];
float temperature;

void SB_Init(Sensor_Board *sb) {
	static ADS131M08 adc;
	sb->adc = &adc;
	static ADT7301 ts;
	sb->tmp_sensor = &ts;
	sensor_board = sb;
	ADT7301_Init(&ts, SB_SPIx);
	ADC_Init(&adc, SB_SPIx, SB_DMAx);
	ADC_Start_Sampling();
}

void SB_Align_Samples() {
	while (!sensor_board->adc->sampling_complete_flag);

	uint32_t aligned_index = 0;
	for (uint16_t i = 0; i < NUM_SAMPLES; i++) {
		uint32_t base_index = i * BYTES_PER_SAMPLE;

		// A sample is made of 10 24-bit words. The first
		// and last word should be ignored, and others
		// truncated to 16 bits.

		for(uint8_t j = 3; j < 27; j += 3) {
			uint8_t first_byte = sensor_board->adc->samples[base_index + j];
			uint8_t second_byte = sensor_board->adc->samples[base_index + j + 1];
			// third byte is ignored, we only want 16 bits

			aligned_samples[aligned_index++] = ((uint16_t) first_byte << 8) | ((uint16_t) second_byte);
		}
	}
}

void SB_Get_Temperature_Reading() {
	ADT7301_Wakeup(TEMP1);
	ADT7301_Collect_Sample(TEMP1);
	ADT7301_Shutdown(TEMP1);

	temperature = sensor_board->tmp_sensor->sample;
}



