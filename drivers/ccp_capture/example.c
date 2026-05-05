#include "core/compiler.h"
#include "drivers/ccp_capture/ccp_capture.h"

static volatile uint16_t t_rise = 0u;
static volatile uint16_t t_fall = 0u;
static volatile uint16_t pulse_width = 0u;
static volatile uint8_t waiting_falling = 0u;

static void ccp_capture_cb(uint16_t value)
{
    if (waiting_falling == 0u)
    {
        t_rise = value;
        waiting_falling = 1u;
        ccp_capture_init(CCP_CAPTURE_FALLING);
    }
    else
    {
        t_fall = value;
        pulse_width = (uint16_t)(t_fall - t_rise);
        waiting_falling = 0u;
        ccp_capture_init(CCP_CAPTURE_RISING);
    }
}

void main(void)
{
    ccp_capture_init(CCP_CAPTURE_RISING);
    ccp_capture_set_callback(ccp_capture_cb);
    ccp_capture_enable();

    while (1)
    {
        DRV_UNUSED(pulse_width);
    }
}
