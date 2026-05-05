#include <xc.h>
#include "core/compiler.h"
#include "drivers/_template/template.h"

void main(void)
{
    /* Minimal XC8 startup example */
    template_init();

    while (1)
    {
        /* Driver call flow goes here */
    }
}
