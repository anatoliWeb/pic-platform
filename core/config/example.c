/*
 * File: core/config/example.c
 */

/*
 * Example project configuration flow.
 *
 * This file is a reference only. Copy the pattern into an application project
 * and keep the final CONFIG values in the project layer.
 */

#if 0

#include "project_config.h"
#include "platform_config_check.h"

#include "core/compiler.h"
#include "core/delay.h"

int main(void)
{
    while (1)
    {
        delay_ms(500);
    }

    return 0;
}

#endif
