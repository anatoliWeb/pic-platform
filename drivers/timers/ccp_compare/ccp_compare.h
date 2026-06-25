/*
 * File: drivers/timers/ccp_compare/ccp_compare.h
 */

#ifndef DRIVERS_CCP_COMPARE_CCP_COMPARE_H
#define DRIVERS_CCP_COMPARE_CCP_COMPARE_H

#include "core/compiler.h"
#include "core/types.h"

typedef void (*ccp_compare_callback_t)(void);

void ccp_compare_init(void);
void ccp_compare_enable(void);
void ccp_compare_disable(void);
void ccp_compare_set_value(uint16_t value);
void ccp_compare_set_callback(ccp_compare_callback_t cb);
void ccp_compare_irq_handler(void);

#endif /* DRIVERS_CCP_COMPARE_CCP_COMPARE_H */
