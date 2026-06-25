/*
 * File: core/bit_utils.h
 */

#ifndef CORE_BIT_UTILS_H
#define CORE_BIT_UTILS_H

/*
 * MCU-agnostic bit manipulation helpers.
 */
#define SET_BIT(reg, bit)     ((reg) |= (1UL << (bit)))
#define CLEAR_BIT(reg, bit)   ((reg) &= ~(1UL << (bit)))
#define TOGGLE_BIT(reg, bit)  ((reg) ^= (1UL << (bit)))
#define READ_BIT(reg, bit)    (((reg) >> (bit)) & 0x1UL)

#endif /* CORE_BIT_UTILS_H */
