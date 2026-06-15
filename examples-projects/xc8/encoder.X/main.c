#include <xc.h>

#include "project_config.h"
#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "libraries/input/encoder/encoder.h"

static encoder_t g_encoder;

static void app_init_io(void)
{
    /*
     * PIC18F452 encoder demo wiring:
     * A  -> RB0
     * B  -> RB1
     * SW -> RB2
     *
     * Use external pull-ups or enable PORTB pull-ups in Proteus.
     */
    ADCON1 = 0x0Fu;
    INTCON2bits.RBPU = 0u;
}

static void app_print_position_step(int8_t step)
{
    if (step > 0)
    {
        DBG_PRINT("RIGHT POS=");
    }
    else
    {
        DBG_PRINT("LEFT POS=");
    }

    DBG_PRINT_INT((int)encoder_get_position(&g_encoder));
    DBG_PRINTLN("");
}

void main(void)
{
    uint32_t last_poll;
    int8_t step;
    uint8_t clicks;

    app_init_io();

    uart_init(9600u);
    tick_init();

    encoder_init(&g_encoder,
                 &PORTB, &TRISB,
                 0u, 1u,
                 &PORTB, &TRISB, 2u);

    DBG_PRINTLN("PIC18F452 encoder example");
    DBG_PRINTLN("A=RB0, B=RB1, SW=RB2");

    last_poll = tick_get();

    while (1)
    {
        if ((tick_get() - last_poll) >= 2u)
        {
            last_poll = tick_get();
            encoder_tick(&g_encoder);

            while ((step = encoder_get_delta(&g_encoder)) != 0)
            {
                app_print_position_step(step);
            }

            if (button_pressed(&g_encoder.button) != 0u)
            {
                DBG_PRINTLN("PRESS");
            }

            if (button_released(&g_encoder.button) != 0u)
            {
                DBG_PRINTLN("RELEASE");
            }

            if (encoder_clicked(&g_encoder) != 0u)
            {
                DBG_PRINTLN("CLICK");
            }

            if (encoder_double_clicked(&g_encoder) != 0u)
            {
                DBG_PRINTLN("DOUBLE_CLICK");
            }

            if (encoder_held(&g_encoder) != 0u)
            {
                DBG_PRINTLN("HOLD");
            }

            if (button_hold_repeated(&g_encoder.button) != 0u)
            {
                DBG_PRINTLN("HOLD_REPEAT");
            }

            clicks = encoder_get_click_count(&g_encoder);
            if (clicks != 0u)
            {
                DBG_PRINT("CLICKS=");
                DBG_PRINT_INT((int)clicks);
                DBG_PRINTLN("");
            }
        }
    }
}
