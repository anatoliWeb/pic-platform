/*
 * File: core/ring_buffer/ring_buffer.h
 */

#ifndef DRIVERS_RING_BUFFER_RING_BUFFER_H
#define DRIVERS_RING_BUFFER_RING_BUFFER_H

#include "core/compiler.h"
#include "core/types.h"

#ifndef RB_OVERWRITE
#define RB_OVERWRITE 0
#endif

typedef struct
{
    uint8_t* buffer;
    uint16_t size;

    volatile uint16_t head;
    volatile uint16_t tail;

} ring_buffer_t;

void rb_init(ring_buffer_t* rb, uint8_t* buf, uint16_t size);

uint8_t rb_push(ring_buffer_t* rb, uint8_t data);
uint8_t rb_pop(ring_buffer_t* rb, uint8_t* data);

uint8_t rb_is_empty(ring_buffer_t* rb);
uint8_t rb_is_full(ring_buffer_t* rb);

uint16_t rb_available(ring_buffer_t* rb);

#endif /* DRIVERS_RING_BUFFER_RING_BUFFER_H */
