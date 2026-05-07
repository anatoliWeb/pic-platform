#ifndef LIBRARIES_INPUT_IR_RECEIVER_H
#define LIBRARIES_INPUT_IR_RECEIVER_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    IR_PROTOCOL_UNKNOWN = 0,
    IR_PROTOCOL_NEC
} ir_protocol_t;

typedef enum
{
    IR_STATUS_OK = 0,
    IR_STATUS_ERROR,
    IR_STATUS_INVALID_TIMING,
    IR_STATUS_INCOMPLETE,
    IR_STATUS_TIMEOUT,
    IR_STATUS_CHECKSUM
} ir_status_t;

typedef struct
{
    volatile uint8_t* input_port;
    volatile uint8_t* input_tris;
    uint8_t input_pin;

    ir_protocol_t protocol;

    /* Decoder update period in microseconds. */
    uint16_t sample_period_us;
    uint32_t frame_timeout_us;
} ir_receiver_config_t;

typedef struct
{
    uint32_t raw;
    uint16_t address;
    uint16_t command;
    uint8_t protocol;
    uint8_t repeat;
    uint8_t valid;
} ir_frame_t;

typedef struct
{
    ir_receiver_config_t config;
    ir_frame_t frame;
    ir_status_t last_status;

    uint8_t initialized;
    uint8_t available;

    uint8_t last_level;
    uint32_t elapsed_us;
    uint32_t last_edge_us;
    uint32_t last_falling_us;
    uint8_t has_falling;

    uint8_t state;
    uint8_t bit_index;
    uint32_t work_raw;
} ir_receiver_t;

uint8_t ir_receiver_init(ir_receiver_t* ir, const ir_receiver_config_t* config);

void ir_receiver_update(ir_receiver_t* ir);
uint8_t ir_receiver_available(const ir_receiver_t* ir);
ir_status_t ir_receiver_read(ir_receiver_t* ir, ir_frame_t* out_frame);
void ir_receiver_clear(ir_receiver_t* ir);

#endif /* LIBRARIES_INPUT_IR_RECEIVER_H */
