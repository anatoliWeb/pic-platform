#include "libraries/input/ir_receiver/ir_receiver.h"

#include "drivers/gpio/gpio.h"

#define IR_DEC_STATE_IDLE 0u
#define IR_DEC_STATE_NEC_DATA 1u

/* NEC timing in microseconds (falling-edge period model). */
#define NEC_START_PERIOD_US  14062u
#define NEC_REPEAT_PERIOD_US 11812u
#define NEC_BIT0_PERIOD_US    1125u
#define NEC_BIT1_PERIOD_US    2250u

#define NEC_TOL_START_US      2500u
#define NEC_TOL_REPEAT_US     2000u
#define NEC_TOL_BIT_US         450u

static uint8_t ir_in_range(uint32_t value, uint32_t ref, uint32_t tol)
{
    if (value < (ref - tol))
    {
        return 0u;
    }
    if (value > (ref + tol))
    {
        return 0u;
    }
    return 1u;
}

static void ir_reset_decoder(ir_receiver_t* ir)
{
    ir->state = IR_DEC_STATE_IDLE;
    ir->bit_index = 0u;
    ir->work_raw = 0u;
}

static void ir_publish_repeat(ir_receiver_t* ir)
{
    ir->frame.repeat = 1u;
    ir->frame.valid = 1u;
    ir->frame.protocol = (uint8_t)IR_PROTOCOL_NEC;
    ir->available = 1u;
    ir->last_status = IR_STATUS_OK;
}

static void ir_publish_raw_nec(ir_receiver_t* ir)
{
    uint8_t addr;
    uint8_t addr_inv;
    uint8_t cmd;
    uint8_t cmd_inv;

    addr = (uint8_t)(ir->work_raw & 0xFFu);
    addr_inv = (uint8_t)((ir->work_raw >> 8u) & 0xFFu);
    cmd = (uint8_t)((ir->work_raw >> 16u) & 0xFFu);
    cmd_inv = (uint8_t)((ir->work_raw >> 24u) & 0xFFu);

    if ((uint8_t)(addr ^ addr_inv) != 0xFFu)
    {
        /* Could be 16-bit address extension; keep as 16-bit address. */
        ir->frame.address = (uint16_t)(ir->work_raw & 0xFFFFu);
    }
    else
    {
        ir->frame.address = (uint16_t)addr;
    }

    if ((uint8_t)(cmd ^ cmd_inv) != 0xFFu)
    {
        ir->last_status = IR_STATUS_CHECKSUM;
        ir->frame.valid = 0u;
        return;
    }

    ir->frame.raw = ir->work_raw;
    ir->frame.command = (uint16_t)cmd;
    ir->frame.protocol = (uint8_t)IR_PROTOCOL_NEC;
    ir->frame.repeat = 0u;
    ir->frame.valid = 1u;
    ir->available = 1u;
    ir->last_status = IR_STATUS_OK;
}

static void ir_decode_nec_falling_period(ir_receiver_t* ir, uint32_t period_us)
{
    if (ir->state == IR_DEC_STATE_IDLE)
    {
        if (ir_in_range(period_us, NEC_START_PERIOD_US, NEC_TOL_START_US) != 0u)
        {
            ir->state = IR_DEC_STATE_NEC_DATA;
            ir->bit_index = 0u;
            ir->work_raw = 0u;
            return;
        }

        if (ir_in_range(period_us, NEC_REPEAT_PERIOD_US, NEC_TOL_REPEAT_US) != 0u)
        {
            ir_publish_repeat(ir);
            return;
        }

        return;
    }

    if (ir->state == IR_DEC_STATE_NEC_DATA)
    {
        if (ir_in_range(period_us, NEC_BIT0_PERIOD_US, NEC_TOL_BIT_US) != 0u)
        {
            /* Bit 0, nothing to set. */
        }
        else if (ir_in_range(period_us, NEC_BIT1_PERIOD_US, NEC_TOL_BIT_US) != 0u)
        {
            ir->work_raw |= (uint32_t)(1uL << ir->bit_index);
        }
        else
        {
            ir->last_status = IR_STATUS_INVALID_TIMING;
            ir_reset_decoder(ir);
            return;
        }

        ir->bit_index++;
        if (ir->bit_index >= 32u)
        {
            ir_publish_raw_nec(ir);
            ir_reset_decoder(ir);
        }
    }
}

uint8_t ir_receiver_init(ir_receiver_t* ir, const ir_receiver_config_t* config)
{
    if ((ir == (ir_receiver_t*)0) || (config == (const ir_receiver_config_t*)0))
    {
        return 0u;
    }

    if ((config->input_port == (volatile uint8_t*)0) || (config->input_tris == (volatile uint8_t*)0))
    {
        return 0u;
    }

    ir->config = *config;
    if (ir->config.sample_period_us == 0u)
    {
        ir->config.sample_period_us = 50u;
    }
    if (ir->config.frame_timeout_us == 0u)
    {
        ir->config.frame_timeout_us = 20000u;
    }

    gpio_set_input(ir->config.input_tris, ir->config.input_pin);

    ir->frame.raw = 0u;
    ir->frame.address = 0u;
    ir->frame.command = 0u;
    ir->frame.protocol = (uint8_t)IR_PROTOCOL_UNKNOWN;
    ir->frame.repeat = 0u;
    ir->frame.valid = 0u;
    ir->last_status = IR_STATUS_OK;
    ir->available = 0u;
    ir->elapsed_us = 0u;
    ir->last_edge_us = 0u;
    ir->last_falling_us = 0u;
    ir->has_falling = 0u;
    ir->last_level = gpio_read(ir->config.input_port, ir->config.input_pin);
    ir_reset_decoder(ir);
    ir->initialized = 1u;

    return 1u;
}

void ir_receiver_update(ir_receiver_t* ir)
{
    uint8_t level;
    uint32_t edge_period_us;
    uint32_t fall_period_us;

    if ((ir == (ir_receiver_t*)0) || (ir->initialized == 0u))
    {
        return;
    }

    ir->elapsed_us += (uint32_t)ir->config.sample_period_us;

    level = gpio_read(ir->config.input_port, ir->config.input_pin);
    if (level != ir->last_level)
    {
        edge_period_us = (uint32_t)(ir->elapsed_us - ir->last_edge_us);
        DRV_UNUSED(edge_period_us);
        ir->last_edge_us = ir->elapsed_us;

        /*
         * IR demod output is active low during marks.
         * Falling edge marks start of new NEC mark.
         */
        if ((ir->last_level != 0u) && (level == 0u))
        {
            if (ir->has_falling != 0u)
            {
                fall_period_us = (uint32_t)(ir->elapsed_us - ir->last_falling_us);
                if (ir->config.protocol == IR_PROTOCOL_NEC)
                {
                    ir_decode_nec_falling_period(ir, fall_period_us);
                }
            }
            ir->last_falling_us = ir->elapsed_us;
            ir->has_falling = 1u;
        }

        ir->last_level = level;
    }

    if ((ir->state != IR_DEC_STATE_IDLE) &&
        ((uint32_t)(ir->elapsed_us - ir->last_edge_us) > ir->config.frame_timeout_us))
    {
        ir->last_status = IR_STATUS_TIMEOUT;
        ir_reset_decoder(ir);
    }
}

uint8_t ir_receiver_available(const ir_receiver_t* ir)
{
    if (ir == (const ir_receiver_t*)0)
    {
        return 0u;
    }
    return ir->available;
}

ir_status_t ir_receiver_read(ir_receiver_t* ir, ir_frame_t* out_frame)
{
    if ((ir == (ir_receiver_t*)0) || (out_frame == (ir_frame_t*)0))
    {
        return IR_STATUS_ERROR;
    }

    if (ir->available == 0u)
    {
        return IR_STATUS_INCOMPLETE;
    }

    *out_frame = ir->frame;
    ir->available = 0u;
    return ir->last_status;
}

void ir_receiver_clear(ir_receiver_t* ir)
{
    if (ir == (ir_receiver_t*)0)
    {
        return;
    }

    ir->available = 0u;
    ir->frame.valid = 0u;
    ir->frame.repeat = 0u;
    ir->frame.raw = 0u;
    ir->frame.address = 0u;
    ir->frame.command = 0u;
    ir->frame.protocol = (uint8_t)IR_PROTOCOL_UNKNOWN;
    ir->last_status = IR_STATUS_OK;
    ir_reset_decoder(ir);
}
