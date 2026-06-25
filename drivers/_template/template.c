/*
 * File: drivers/_template/template.c
 */

#include "core/compiler.h"
#include "drivers/_template/template.h"

void template_init(void)
{
    /* TODO: configure peripheral registers and initial state */
}

uint8_t template_read(void)
{
    /* TODO: read data from peripheral */
    return 0u;
}

void template_write(uint8_t value)
{
    DRV_UNUSED(value);
    /* TODO: write data to peripheral */
}
