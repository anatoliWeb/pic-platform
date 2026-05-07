#ifndef DRIVERS_ADC_ADC_H
#define DRIVERS_ADC_ADC_H

#include "core/compiler.h"
#include "core/types.h"

void adc_init(void);

uint16_t adc_read(uint8_t channel);
uint16_t adc_read_avg(uint8_t channel, uint8_t samples);
void adc_read_multiple(uint8_t* channels, uint16_t* results, uint8_t count);

uint16_t adc_to_millivolts(uint16_t adc_value, uint16_t vref_mv);
uint16_t adc_read_voltage(uint8_t channel);
int16_t adc_read_thermistor(uint8_t channel);
uint8_t adc_read_button(uint8_t channel);

#endif /* DRIVERS_ADC_ADC_H */
