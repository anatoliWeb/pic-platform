/*
 * File: drivers/timers/ccp_capture/ccp_capture.h
 */

#ifndef DRIVERS_CCP_CAPTURE_CCP_CAPTURE_H
#define DRIVERS_CCP_CAPTURE_CCP_CAPTURE_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    CCP_CAPTURE_RISING = 0,
    CCP_CAPTURE_FALLING,
    CCP_CAPTURE_EVERY_4,
    CCP_CAPTURE_EVERY_16
} ccp_capture_mode_t;

typedef void (*ccp_capture_callback_t)(uint16_t value);

void ccp_capture_init(ccp_capture_mode_t mode);
void ccp_capture_enable(void);
void ccp_capture_disable(void);
uint16_t ccp_capture_get_value(void);
void ccp_capture_set_callback(ccp_capture_callback_t cb);
void ccp_capture_irq_handler(void);

#endif /* DRIVERS_CCP_CAPTURE_CCP_CAPTURE_H */
