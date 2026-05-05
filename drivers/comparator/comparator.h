#ifndef DRIVERS_COMPARATOR_COMPARATOR_H
#define DRIVERS_COMPARATOR_COMPARATOR_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    CMP_MODE_OFF = 0,
    CMP_MODE_1,
    CMP_MODE_2,
    CMP_MODE_3
} comparator_mode_t;

void comparator_init(comparator_mode_t mode);
void comparator_enable(void);
void comparator_disable(void);
uint8_t comparator_get_output(void);

#endif /* DRIVERS_COMPARATOR_COMPARATOR_H */
