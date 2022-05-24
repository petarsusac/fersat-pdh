#include "adc.h"

volatile uint16_t adc_num_samples;
volatile uint8_t *adc_rx_buffer;
volatile uint16_t current_sample_count = 0;

void ADC_first_read(SPI_TypeDef *SPIx) {
	uint8_t rx_buffer[30];

	LL_GPIO_ResetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN); // \CS low
	SPI_TransmitReceive(SPIx, 30, dummy_bytes, rx_buffer);
	LL_GPIO_SetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN); // \CS high

	for(int i = 0; i < 100; i++); // TODO: write a delay function to replace this

	LL_GPIO_ResetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN); // \CS low
	SPI_TransmitReceive(SPIx, 30, dummy_bytes, rx_buffer);
	LL_GPIO_SetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN); // \CS high

	SPI_Disable(SPIx);
}

void ADC_DRDY_interrupt_handler() {
	if (current_sample_count < adc_num_samples) {
		LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_1, BYTES_PER_SAMPLE);
		LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_2, BYTES_PER_SAMPLE);
		LL_DMA_SetMemoryAddress(DMA2, LL_DMA_CHANNEL_1, (uint32_t) &adc_rx_buffer[current_sample_count++ * BYTES_PER_SAMPLE]);

		LL_SPI_EnableDMAReq_RX(SB_SPIx);

		LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_1);
		LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_2);

		LL_SPI_EnableDMAReq_TX(SB_SPIx);

		LL_SPI_EnableIT_RXNE(SB_SPIx);
		LL_SPI_EnableIT_TXE(SB_SPIx);

		NVIC_DisableIRQ(EXTI4_IRQn);

		LL_GPIO_ResetOutputPin(ADC_CS_GPIOx, ADC_CS_PIN);
		LL_SPI_Enable(SB_SPIx);
	}
}

void ADC_set_RX_buffer(volatile uint8_t *rx_buffer) {
	adc_rx_buffer = rx_buffer;
}

void ADC_set_num_samples(uint16_t num_samples) {
	adc_num_samples = num_samples;
}



