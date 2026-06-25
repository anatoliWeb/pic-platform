/*
 * File: libraries/sensors/ds18b20/ds18b20.h
 */

#ifndef DRIVERS_DS18B20_DS18B20_H
#define DRIVERS_DS18B20_DS18B20_H

#include "core/compiler.h"
#include "core/types.h"

uint8_t ds18b20_search(uint8_t (*roms)[8], uint8_t max_devices);
uint8_t ds18b20_find_first(uint8_t* rom);

uint8_t ds18b20_is_valid_family(const uint8_t* rom);
uint8_t ds18b20_is_valid_rom(const uint8_t* rom);

uint8_t ds18b20_start_conversion(uint8_t* rom);
uint8_t ds18b20_start_conversion_skip_rom(void);

uint8_t ds18b20_read_scratchpad(uint8_t* rom, uint8_t* data);
uint8_t ds18b20_read_scratchpad_skip_rom(uint8_t* data);

uint8_t ds18b20_read_temperature_raw(uint8_t* rom, int16_t* out_raw);
uint8_t ds18b20_read_temperature_raw_skip_rom(int16_t* out_raw);

uint8_t ds18b20_read_temperature_celsius(uint8_t* rom, int16_t* out_temp_x10);
uint8_t ds18b20_read_temperature_celsius_skip_rom(int16_t* out_temp_x10);

int16_t ds18b20_get_temperature_raw(uint8_t* rom);
int16_t ds18b20_get_temperature_celsius_x10(uint8_t* rom);

#endif /* DRIVERS_DS18B20_DS18B20_H */
