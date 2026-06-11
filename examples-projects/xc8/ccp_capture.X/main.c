#include "project_config.h"
#include "core/compiler.h"
#include "drivers/timers/ccp_capture/ccp_capture.h"

static volatile uint16_t last_capture = 0u;

static void capture_cb(uint16_t value)
{
    last_capture = value;
}

void main(void)
{
    ccp_capture_init(CCP_CAPTURE_RISING);
    ccp_capture_set_callback(capture_cb);
    ccp_capture_enable();

    while (1)
    {
        DRV_UNUSED(last_capture);
    }
}

/* XC8 ISR hook example:
void __interrupt() isr(void)
{
    ccp_capture_irq_handler();
}
*/

