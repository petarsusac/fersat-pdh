#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"

#include "spi.h"
#include "gpio.h"


void ADC_first_read(SPI_TypeDef *SPIx);
void ADC_DRDY_interrupt_handler();
void ADC_set_RX_buffer(volatile uint8_t *rx_buffer);
void ADC_set_num_samples(uint16_t num_samples);

#endif /* INC_ADC_H_ */
