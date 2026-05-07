#include "core/compiler.h"
#include "core/debug.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/sensors/ultrasonic/ultrasonic.h"

void main(void)
{
    ultrasonic_t sensor;
    ultrasonic_config_t cfg;
    ultrasonic_status_t st;
    uint32_t last_ms;

    tick_init();

    cfg.trig_port = &PORTB;
    cfg.trig_tris = &TRISB;
    cfg.trig_pin = 1u;
    cfg.echo_port = &PORTB;
    cfg.echo_tris = &TRISB;
    cfg.echo_pin = 2u;
    cfg.timeout_us = 30000u;
    cfg.min_pulse_us = 100u;
    cfg.max_pulse_us = 25000u;

    (void)ultrasonic_init(&sensor, &cfg);

    last_ms = 0u;
    while (1)
    {
        if ((tick_get() - last_ms) >= 100u)
        {
            last_ms = tick_get();
            st = ultrasonic_read(&sensor);

            if (st == ULTRASONIC_STATUS_OK)
            {
                DBG_PRINT("Distance mm: ");
                DBG_PRINT_INT((int32_t)ultrasonic_get_distance_mm(&sensor));
                DBG_PRINT(" cm: ");
                DBG_PRINT_INT((int32_t)ultrasonic_get_distance_cm(&sensor));
                DBG_PRINTLN("");
            }
            else if ((st == ULTRASONIC_STATUS_TIMEOUT_WAIT_HIGH) || (st == ULTRASONIC_STATUS_TIMEOUT_WAIT_LOW))
            {
                DBG_PRINTLN("Ultrasonic: out of range or disconnected");
            }
            else
            {
                DBG_PRINT("Ultrasonic error: ");
                DBG_PRINT_INT((int32_t)st);
                DBG_PRINTLN("");
            }
        }
    }
}
