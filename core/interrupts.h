#ifndef CORE_INTERRUPTS_H
#define CORE_INTERRUPTS_H

#include "core/compiler.h"

/*
 * Global interrupt control helpers.
 * No ISR definitions are allowed in this header.
 */
#define DRV_INT_ENABLE()   do { INTCONbits.GIE = 1; } while (0)
#define DRV_INT_DISABLE()  do { INTCONbits.GIE = 0; } while (0)

#endif /* CORE_INTERRUPTS_H */
