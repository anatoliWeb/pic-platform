#ifndef CORE_CONFIG_PLATFORM_CONFIG_CHECK_H
#define CORE_CONFIG_PLATFORM_CONFIG_CHECK_H

/*
 * Lightweight project configuration checker.
 *
 * Libraries may define requirement hints such as:
 *   PIC_PLATFORM_REQUIRES_CLOCK_HZ
 *   PIC_PLATFORM_REQUIRES_WDT_CONFIG
 *   PIC_PLATFORM_REQUIRES_CCP2MUX_CONFIG
 *
 * Recommendations are documented, but the project keeps final control.
 * The checks stay simple so older C18 preprocessing stays safe.
 */

#ifndef PIC_PLATFORM_CLOCK_HZ
#error "PIC_PLATFORM_CLOCK_HZ is required by project_config.h"
#endif

#ifndef DRV_XTAL_FREQ
#error "DRV_XTAL_FREQ is required by project_config.h"
#endif

#ifndef _XTAL_FREQ
#error "_XTAL_FREQ is required by project_config.h"
#endif

#ifndef PIC_PLATFORM_CONFIG_OSC
#error "PIC_PLATFORM_CONFIG_OSC is required by project_config.h"
#endif

#ifndef PIC_PLATFORM_CONFIG_WDT
#error "PIC_PLATFORM_CONFIG_WDT is required by project_config.h"
#endif

#ifndef PIC_PLATFORM_CONFIG_LVP
#error "PIC_PLATFORM_CONFIG_LVP is required by project_config.h"
#endif

#ifndef PIC_PLATFORM_CONFIG_BOR
#error "PIC_PLATFORM_CONFIG_BOR is required by project_config.h"
#endif

#ifndef PIC_PLATFORM_CONFIG_STVR
#error "PIC_PLATFORM_CONFIG_STVR is required by project_config.h"
#endif

#ifndef PIC_PLATFORM_CONFIG_CCP2MUX
#error "PIC_PLATFORM_CONFIG_CCP2MUX is required by project_config.h"
#endif

#ifdef PIC_PLATFORM_REQUIRES_CLOCK_HZ
#ifndef PIC_PLATFORM_CLOCK_HZ
#error "PIC_PLATFORM_CLOCK_HZ is required by the active library"
#endif
#endif

#ifdef PIC_PLATFORM_REQUIRES_WDT_CONFIG
#ifndef PIC_PLATFORM_CONFIG_WDT
#error "PIC_PLATFORM_CONFIG_WDT is required by the active library"
#endif
#endif

#ifdef PIC_PLATFORM_REQUIRES_CCP2MUX_CONFIG
#ifndef PIC_PLATFORM_CONFIG_CCP2MUX
#error "PIC_PLATFORM_CONFIG_CCP2MUX is required by the active library"
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
