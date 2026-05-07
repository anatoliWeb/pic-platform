#include "core/compiler.h"
#include "core/debug.h"
#include "drivers/communication/i2c/i2c.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/sensors/bmp/bmp.h"

void main(void)
{
    bmp_t sensor;
    bmp_config_t cfg;
    bmp_status_t st;
    uint32_t last_ms;

    i2c_init(100000u);
    tick_init();

    cfg.sensor_type = BMP_SENSOR_BMP280;
    cfg.i2c_address = 0x76u;
    cfg.oversampling = 1u;

    if (bmp_init(&sensor, &cfg) == 0u)
    {
        DBG_PRINTLN("BMP init failed");
        while (1)
        {
        }
    }

    last_ms = 0u;
    while (1)
    {
        if ((tick_get() - last_ms) >= 1000u)
        {
            last_ms = tick_get();
            st = bmp_read(&sensor);

            if (st == BMP_STATUS_OK)
            {
                DBG_PRINT("T[x10]: ");
                DBG_PRINT_INT(bmp_get_temperature(&sensor));
                DBG_PRINT(" P[Pa]: ");
                DBG_PRINT_INT((int32_t)bmp_get_pressure(&sensor));
                DBG_PRINT(" Alt[m]: ");
                DBG_PRINT_INT(bmp_get_altitude(&sensor, 101325u));
                DBG_PRINTLN("");
            }
            else
            {
                DBG_PRINT("BMP read error: ");
                DBG_PRINT_INT((int32_t)st);
                DBG_PRINTLN("");
            }
        }
    }
}
