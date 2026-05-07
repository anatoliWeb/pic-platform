#include "drivers/utils/ring_buffer/ring_buffer.h"

static uint16_t rb_next_index(uint16_t index, uint16_t size)
{
    index++;
    if (index >= size)
    {
        index = 0u;
    }
    return index;
}

void rb_init(ring_buffer_t* rb, uint8_t* buf, uint16_t size)
{
    if ((rb == (ring_buffer_t*)0) || (buf == (uint8_t*)0) || (size < 2u))
    {
        return;
    }

    rb->buffer = buf;
    rb->size = size;
    rb->head = 0u;
    rb->tail = 0u;
}

uint8_t rb_push(ring_buffer_t* rb, uint8_t data)
{
    uint16_t next;

    if ((rb == (ring_buffer_t*)0) || (rb->buffer == (uint8_t*)0) || (rb->size < 2u))
    {
        return 0u;
    }

    next = rb_next_index(rb->head, rb->size);

    if (next == rb->tail)
    {
#if RB_OVERWRITE
        rb->tail = rb_next_index(rb->tail, rb->size);
#else
        return 0u;
#endif
    }

    rb->buffer[rb->head] = data;
    rb->head = next;

    return 1u;
}

uint8_t rb_pop(ring_buffer_t* rb, uint8_t* data)
{
    if ((rb == (ring_buffer_t*)0) || (data == (uint8_t*)0) || (rb->buffer == (uint8_t*)0) || (rb->size < 2u))
    {
        return 0u;
    }

    if (rb->head == rb->tail)
    {
        return 0u;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = rb_next_index(rb->tail, rb->size);

    return 1u;
}

uint8_t rb_is_empty(ring_buffer_t* rb)
{
    if (rb == (ring_buffer_t*)0)
    {
        return 1u;
    }

    return (uint8_t)(rb->head == rb->tail);
}

uint8_t rb_is_full(ring_buffer_t* rb)
{
    uint16_t next;

    if ((rb == (ring_buffer_t*)0) || (rb->size < 2u))
    {
        return 0u;
    }

    next = rb_next_index(rb->head, rb->size);
    return (uint8_t)(next == rb->tail);
}

uint16_t rb_available(ring_buffer_t* rb)
{
    if ((rb == (ring_buffer_t*)0) || (rb->size < 2u))
    {
        return 0u;
    }

    if (rb->head >= rb->tail)
    {
        return (uint16_t)(rb->head - rb->tail);
    }

    return (uint16_t)(rb->size - (rb->tail - rb->head));
}
