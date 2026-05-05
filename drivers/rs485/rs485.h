#ifndef DRIVERS_RS485_RS485_H
#define DRIVERS_RS485_RS485_H

#include "core/compiler.h"
#include "core/types.h"

void rs485_init(
    volatile uint8_t* dir_port,
    volatile uint8_t* dir_tris,
    uint8_t dir_pin
);

void rs485_set_tx(void);
void rs485_set_rx(void);

void rs485_send_byte(uint8_t data);
uint8_t rs485_read_byte(void);

uint8_t rs485_send_frame(uint8_t* data, uint8_t len);
uint8_t rs485_receive_frame(uint8_t* buffer, uint8_t max_len);

uint8_t rs485_crc8(uint8_t* data, uint8_t len);

#endif /* DRIVERS_RS485_RS485_H */
