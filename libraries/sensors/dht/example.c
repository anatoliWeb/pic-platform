#include "core/compiler.h"
#include "core/debug.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/sensors/dht/dht.h"

void main(void)
{
    dht_t sensor;
    dht_config_t cfg;
    uint32_t last_read_ms;
    dht_status_t st;

    tick_init();

    cfg.port = &PORTB;
    cfg.tris = &TRISB;
    cfg.pin = 0u;
    cfg.sensor_type = DHT_SENSOR_DHT22;
    cfg.start_low_ms = 0u;
    cfg.min_interval_ms = 0u;
    cfg.bit_timeout_us = 0u;

    (void)dht_init(&sensor, &cfg);

    last_read_ms = 0u;
    while (1)
    {
        if ((tick_get() - last_read_ms) >= 2000u)
        {
            last_read_ms = tick_get();

            st = dht_read(&sensor);
            if (st == DHT_STATUS_OK)
            {
                DBG_PRINT("DHT T[x10]: ");
                DBG_PRINT_INT(dht_get_temperature(&sensor));
                DBG_PRINT(" H[x10]: ");
                DBG_PRINT_INT(dht_get_humidity(&sensor));
                DBG_PRINTLN("");
            }
            else if (st == DHT_STATUS_CHECKSUM)
            {
                DBG_PRINTLN("DHT checksum error");
            }
            else if (st == DHT_STATUS_TIMEOUT)
            {
                DBG_PRINTLN("DHT timeout");
            }
            else
            {
                DBG_PRINTLN("DHT read error");
            }
        }
    }
}
