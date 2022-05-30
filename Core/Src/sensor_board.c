#include "sensor_board.h"

Sensor_Board *sensor_board;
uint16_t aligned_samples[NUM_SAMPLES * 8 * 2];
float *temperature;

// Initializes the required memory structures for sensor board operations.
void SB_Init(Sensor_Board *sb) {
	static ADS131M08 adc;
	sb->adc = &adc;
	static ADT7301 ts;
	sb->tmp_sensor = &ts;
	sensor_board = sb;
}

// Starts ADC sampling procedure. This function is non-blocking, since
// sampling is handled by DMA.
void SB_Start_ADC_Sampling() {
	ADC_Init(sensor_board->adc, SB_SPIx, SB_DMAx);
	ADC_Start_Sampling();
}

// Fetches 24-bit ADC samples from memory buffer and stores them into
// an array of complex numbers intended to be used as the source buffer
// for CMSIS FFT implementation.
// IMPORTANT: This function should output floating point numbers, currently
// outputs 16-bit unsigned integers for testing purposes.
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

			// TODO: insert conversion to float here
			aligned_samples[aligned_index] = ((uint16_t) first_byte << 8) | ((uint16_t) second_byte);
			aligned_index += 2;
		}
	}
}

// Collects samples from sensor board temperature sensors.
void SB_Get_Temperature_Readings() {
	ADT7301_Init(sensor_board->tmp_sensor, SB_SPIx);

	ADT7301_Wakeup(TEMP1);
	ADT7301_Collect_Sample(TEMP1);
	ADT7301_Shutdown(TEMP1);

	ADT7301_Wakeup(TEMP2);
	ADT7301_Collect_Sample(TEMP2);
	ADT7301_Shutdown(TEMP2);

//	ADT7301_Wakeup(TEMP3);
//	ADT7301_Collect_Sample(TEMP3);
//	ADT7301_Shutdown(TEMP3);

	temperature = sensor_board->tmp_sensor->samples;
}



