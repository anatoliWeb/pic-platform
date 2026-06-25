/*
 * File: drivers/communication/i2c/i2c.h
 */

#ifndef DRIVERS_I2C_I2C_H
#define DRIVERS_I2C_I2C_H

#include "core/compiler.h"
#include "core/types.h"

void i2c_init(uint32_t clock_hz);

void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);

uint8_t i2c_write_byte(uint8_t data);
uint8_t i2c_read_byte(uint8_t ack);

uint8_t i2c_write_register(uint8_t device_addr, uint8_t reg_addr, uint8_t data);
uint8_t i2c_read_register(uint8_t device_addr, uint8_t reg_addr, uint8_t* data);

uint8_t i2c_device_ready(uint8_t device_addr);
uint8_t i2c_scan(uint8_t* found_addresses, uint8_t max_devices);

#endif /* DRIVERS_I2C_I2C_H */
