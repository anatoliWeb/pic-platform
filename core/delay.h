#ifndef CORE_DELAY_H
#define CORE_DELAY_H

#include "core/compiler.h"
#include "core/device.h"

void delay_us(uint16_t us);
void delay_ms(uint16_t ms);

#define DRV_DELAY_US(us) delay_us(us)
#define DRV_DELAY_MS(ms) delay_ms(ms)

#endif /* CORE_DELAY_H */