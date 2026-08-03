/*
 * File: core/config/project_config_template.h
 */

#ifndef CORE_CONFIG_PROJECT_CONFIG_TEMPLATE_H
#define CORE_CONFIG_PROJECT_CONFIG_TEMPLATE_H

/*
 * Project-level configuration template for PIC18F452 applications.
 *
 * Copy this file into the application project as project_config.h and adjust
 * the values to match the real hardware setup.
 *
 * Keep project-local helper flags here.
 * Project-wide clock/timing values belong in compiler macros / define-macros.
 * Emit the real CONFIG bits only in config_bits.c with literal values.
 */

/*
 * Legacy projects may opt into the default 8 MHz clock explicitly by defining
 * PIC_PLATFORM_ALLOW_DEFAULT_CLOCK in their own project configuration.
 *
 * New projects should define PIC_PLATFORM_CLOCK_HZ in project metadata
 * (compiler -D / MPLAB define-macros). Do not rely on an implicit default clock.
 */

/*
 * Simple helper flags for build-time checks and documentation.
 * Update these in the project if the final CONFIG state changes.
 */
#ifndef PIC_PLATFORM_WDT_ENABLED
#define PIC_PLATFORM_WDT_ENABLED 0
#endif

#ifndef PIC_PLATFORM_LVP_ENABLED
#define PIC_PLATFORM_LVP_ENABLED 0
#endif

#ifndef PIC_PLATFORM_BOR_ENABLED
#define PIC_PLATFORM_BOR_ENABLED 1
#endif

#ifndef PIC_PLATFORM_CCP2MUX_ENABLED
#define PIC_PLATFORM_CCP2MUX_ENABLED 1
#endif

#endif /* CORE_CONFIG_PROJECT_CONFIG_TEMPLATE_H */
