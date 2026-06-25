/*
 * File: core/delay.h
 */

#ifndef CORE_DELAY_H
#define CORE_DELAY_H

#include "core/compiler.h"
#include "core/device.h"

void delay_us(uint16_t us);
void delay_ms(uint16_t ms);

/*
 * Precise blocking microsecond delay for timing-critical bit-banged protocols.
 *
 * Use this for protocols such as 1-Wire, DHT, IR, software SPI/UART,
 * where short pulses must be more stable than a generic application delay.
 */
void micro_delay_us(uint16_t us);

/*
 * Blocking delay measured in instruction-cycle steps.
 *
 * On PIC18, the instruction clock is Fosc / 4.
 * Example:
 *   Fosc = 8 MHz  -> 1 instruction cycle ~= 0.5 us
 *   Fosc = 20 MHz -> 1 instruction cycle ~= 0.2 us
 *
 * This is useful when a delay smaller than 1 us is needed.
 */
void micro_delay_cycles(uint16_t cycles);

#define DRV_DELAY_US(us) delay_us(us)
#define DRV_DELAY_MS(ms) delay_ms(ms)

/*
 * Separate macros for protocol-level precise delays.
 */
#define DRV_MICRO_DELAY_US(us) micro_delay_us(us)
#define DRV_MICRO_DELAY_CYCLES(cycles) micro_delay_cycles(cycles)

#endif /* CORE_DELAY_H */