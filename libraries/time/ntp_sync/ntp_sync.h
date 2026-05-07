#ifndef LIBRARIES_TIME_NTP_SYNC_H
#define LIBRARIES_TIME_NTP_SYNC_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/time/rtc_framework/rtc_framework.h"
#include "libraries/time/timezone/timezone.h"

typedef uint8_t (*ntp_send_callback_t)(const uint8_t* data, uint16_t length, void* user_ctx);
typedef uint16_t (*ntp_receive_callback_t)(uint8_t* data, uint16_t max_length, void* user_ctx);

typedef enum
{
    NTP_SYNC_STATUS_OK = 0,
    NTP_SYNC_STATUS_ERROR,
    NTP_SYNC_STATUS_SEND_FAILED,
    NTP_SYNC_STATUS_NO_RESPONSE,
    NTP_SYNC_STATUS_INVALID_SIZE,
    NTP_SYNC_STATUS_INVALID_PACKET,
    NTP_SYNC_STATUS_INVALID_MODE_VERSION,
    NTP_SYNC_STATUS_INVALID_TIMESTAMP,
    NTP_SYNC_STATUS_RTC_UPDATE_FAILED
} ntp_sync_status_t;

typedef struct
{
    ntp_send_callback_t send_cb;
    ntp_receive_callback_t receive_cb;
    void* transport_ctx;

    uint8_t ntp_version;
} ntp_sync_config_t;

typedef struct
{
    ntp_sync_config_t config;
    uint32_t unix_utc;
    uint8_t valid;
    uint8_t initialized;
} ntp_sync_t;

uint8_t ntp_sync_init(ntp_sync_t* ntp, const ntp_sync_config_t* config);

ntp_sync_status_t ntp_sync_request(ntp_sync_t* ntp);
ntp_sync_status_t ntp_sync_process_response(ntp_sync_t* ntp);
ntp_sync_status_t ntp_sync_apply_to_rtc(ntp_sync_t* ntp, rtc_t* rtc, const timezone_t* tz);

uint8_t ntp_sync_is_valid(const ntp_sync_t* ntp);
uint8_t ntp_sync_get_unix(const ntp_sync_t* ntp, uint32_t* unix_time);

#endif /* LIBRARIES_TIME_NTP_SYNC_H */
