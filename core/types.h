#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include "core/compiler.h"

/*
 * Common driver status codes used across the library.
 */
typedef enum
{
    DRV_STATUS_OK = 0,
    DRV_STATUS_ERROR = 1,
    DRV_STATUS_TIMEOUT = 2
} drv_status_t;

/*
 * Optional boolean wrapper for consistent driver APIs.
 */
typedef enum
{
    DRV_FALSE = 0,
    DRV_TRUE = 1
} drv_bool_t;

#endif /* CORE_TYPES_H */
