#include "core/compiler.h"
#include "drivers/_template/template.h"

static void app_init(void)
{
    template_init();
}

void main(void)
{
    app_init();

    while (1)
    {
        template_write(template_read());
    }
}
