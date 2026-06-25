/*
 * File: core/config/platform_config_check.h
 */

#ifndef CORE_CONFIG_PLATFORM_CONFIG_CHECK_H
#define CORE_CONFIG_PLATFORM_CONFIG_CHECK_H

/*
 * Lightweight project configuration checker.
 *
 * Libraries may declare requirement hints such as:
 *   PIC_PLATFORM_REQUIRES_CLOCK_HZ
 *   PIC_PLATFORM_REQUIRES_WDT_ENABLED
 *   PIC_PLATFORM_REQUIRES_CCP2MUX_ENABLED
 *
 * The project keeps final control over the selected CONFIG bits.
 * Real CONFIG values must stay in config_bits.c as literal pragmas.
 */

/*
 * Projects should prefer PIC_PLATFORM_CLOCK_HZ as the single source of truth.
 * Legacy projects may still rely on the explicit allow flag defined in the
 * project itself, which keeps the historical 8 MHz fallback available.
 */
#if !defined(PIC_PLATFORM_CLOCK_HZ) && !defined(PIC_PLATFORM_ALLOW_DEFAULT_CLOCK)
#error "PIC_PLATFORM_CLOCK_HZ is required by project_config.h"
#endif

#ifndef PIC_PLATFORM_WDT_ENABLED
#error "PIC_PLATFORM_WDT_ENABLED is required by project_config.h"
#endif

#ifndef PIC_PLATFORM_LVP_ENABLED
#error "PIC_PLATFORM_LVP_ENABLED is required by project_config.h"
#endif

#ifndef PIC_PLATFORM_BOR_ENABLED
#error "PIC_PLATFORM_BOR_ENABLED is required by project_config.h"
#endif

#ifndef PIC_PLATFORM_CCP2MUX_ENABLED
#error "PIC_PLATFORM_CCP2MUX_ENABLED is required by project_config.h"
#endif

#ifdef PIC_PLATFORM_REQUIRES_CLOCK_HZ
#ifndef PIC_PLATFORM_CLOCK_HZ
#error "PIC_PLATFORM_CLOCK_HZ is required by the active library"
#endif
#endif

#ifdef PIC_PLATFORM_REQUIRES_WDT_ENABLED
#ifndef PIC_PLATFORM_WDT_ENABLED
#error "PIC_PLATFORM_WDT_ENABLED is required by the active library"
#endif
#endif

#ifdef PIC_PLATFORM_REQUIRES_CCP2MUX_ENABLED
#ifndef PIC_PLATFORM_CCP2MUX_ENABLED
#error "PIC_PLATFORM_CCP2MUX_ENABLED is required by the active library"
#endif
#endif

/*
 * Recommendation markers are informational only. They do not force a build
 * failure in this minimal checker.
 */
#ifdef PIC_PLATFORM_RECOMMENDS_WDT_ON
/* Recommendation: WDT should be enabled for this feature set if practical. */
#endif

#endif /* CORE_CONFIG_PLATFORM_CONFIG_CHECK_H */
