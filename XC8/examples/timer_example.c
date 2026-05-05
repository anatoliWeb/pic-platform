#include <xc.h>
#include "core/compiler.h"
#include "drivers/timer0/timer0.h"
#include "drivers/timer1/timer1.h"
#include "drivers/timer2/timer2.h"
#include "drivers/timer3/timer3.h"
#include "drivers/tick/tick.h"

void main(void)
{
    timer0_init(256u);
    timer1_init(8u);
    timer2_init(16u);
    timer3_init(8u);

    tick_init();

    while (1)
    {
        if ((tick_get() % 1000u) == 0u)
        {
        }
    }
}
