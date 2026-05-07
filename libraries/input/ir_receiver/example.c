#include "core/compiler.h"
#include "core/debug.h"
#include "libraries/input/ir_receiver/ir_receiver.h"

void main(void)
{
    ir_receiver_t ir;
    ir_receiver_config_t cfg;
    ir_frame_t frame;
    ir_status_t st;

    cfg.input_port = &PORTB;
    cfg.input_tris = &TRISB;
    cfg.input_pin = 0u;
    cfg.protocol = IR_PROTOCOL_NEC;
    cfg.sample_period_us = 50u;
    cfg.frame_timeout_us = 20000u;

    (void)ir_receiver_init(&ir, &cfg);

    while (1)
    {
        /* Call periodically from loop/scheduler/fast tick. */
        ir_receiver_update(&ir);

        if (ir_receiver_available(&ir) != 0u)
        {
            st = ir_receiver_read(&ir, &frame);
            if ((st == IR_STATUS_OK) && (frame.valid != 0u))
            {
                DBG_PRINT("IR addr: ");
                DBG_PRINT_HEX((uint8_t)frame.address);
                DBG_PRINT(" cmd: ");
                DBG_PRINT_HEX((uint8_t)frame.command);
                DBG_PRINT(" rep: ");
                DBG_PRINT_INT((int32_t)frame.repeat);
                DBG_PRINTLN("");
            }
            else
            {
                DBG_PRINT("IR decode error: ");
                DBG_PRINT_INT((int32_t)st);
                DBG_PRINTLN("");
            }
        }
    }
}
