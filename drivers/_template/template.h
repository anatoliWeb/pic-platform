#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "core/compiler.h"

/*
 * Generic driver template API.
 * Rename "template" to your driver name before implementation.
 */

void template_init(void);
uint8_t template_read(void);
void template_write(uint8_t value);

#endif /* TEMPLATE_H */
