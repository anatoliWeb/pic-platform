#ifndef DRIVERS_DS18B20_DS18B20_H
#define DRIVERS_DS18B20_DS18B20_H

#include "core/compiler.h"
#include "core/types.h"

uint8_t ds18b20_start_conversion(uint8_t* rom);
uint8_t ds18b20_read_scratchpad(uint8_t* rom, uint8_t* data);
int16_t ds18b20_get_temperature_raw(uint8_t* rom);
int16_t ds18b20_get_temperature_celsius_x10(uint8_t* rom);

#endif /* DRIVERS_DS18B20_DS18B20_H */
