/*
 * File: core/types.h
 */

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include "core/compiler.h"

typedef enum
{
    DRV_STATUS_OK = 0,
    DRV_STATUS_ERROR,
    DRV_STATUS_TIMEOUT
} drv_status_t;

typedef enum
{
    DRV_FALSE = 0,
    DRV_TRUE = 1
} drv_bool_t;

#endif /* CORE_TYPES_H */