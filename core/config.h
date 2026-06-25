/*
 * File: core/config.h
 */

#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

/*
 * Global compile-time feature flags.
 * Projects can override these definitions before including this file.
 */
#ifndef DRV_DEBUG_ENABLE
    #define DRV_DEBUG_ENABLE 1
#endif

#ifndef DRV_USE_UART
    #define DRV_USE_UART 1
#endif

#endif /* CORE_CONFIG_H */
