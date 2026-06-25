/*
 * File: core/crc/crc.h
 */

#ifndef DRIVERS_CRC_CRC_H
#define DRIVERS_CRC_CRC_H

#include "core/compiler.h"
#include "core/types.h"

uint8_t crc8_dallas(const uint8_t* data, uint16_t len);
uint16_t crc16_modbus(const uint8_t* data, uint16_t len);
uint8_t checksum8(const uint8_t* data, uint16_t len);

#endif /* DRIVERS_CRC_CRC_H */
