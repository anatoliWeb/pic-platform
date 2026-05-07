#include "core/compiler.h"
#include "core/debug.h"
#include "libraries/time/rtc_framework/rtc_framework.h"
#include "libraries/time/software_rtc/software_rtc.h"
#include "libraries/time/timezone/timezone.h"
#include "libraries/time/ntp_sync/ntp_sync.h"

#define EXAMPLE_NTP_PACKET_SIZE 48u
#define EXAMPLE_NTP_UNIX_EPOCH_DELTA 2208988800UL

typedef struct
{
    uint8_t response[EXAMPLE_NTP_PACKET_SIZE];
    uint16_t response_len;
    uint8_t last_request_first_byte;
} mock_udp_ctx_t;

static uint8_t mock_send(const uint8_t* data, uint16_t length, void* user_ctx)
{
    mock_udp_ctx_t* ctx;
    if ((data == (const uint8_t*)0) || (user_ctx == (void*)0) || (length < 1u))
    {
        return 0u;
    }

    ctx = (mock_udp_ctx_t*)user_ctx;
    ctx->last_request_first_byte = data[0];
    return 1u;
}

static uint16_t mock_receive(uint8_t* data, uint16_t max_length, void* user_ctx)
{
    mock_udp_ctx_t* ctx;
    uint16_t i;

    if ((data == (uint8_t*)0) || (user_ctx == (void*)0))
    {
        return 0u;
    }

    ctx = (mock_udp_ctx_t*)user_ctx;
    if ((ctx->response_len == 0u) || (max_length < ctx->response_len))
    {
        return 0u;
    }

    for (i = 0u; i < ctx->response_len; i++)
    {
        data[i] = ctx->response[i];
    }
    return ctx->response_len;
}

void main(void)
{
    software_rtc_t sw;
    rtc_t rtc;
    timezone_t tz;
    ntp_sync_t ntp;
    ntp_sync_config_t cfg;
    mock_udp_ctx_t udp;
    rtc_datetime_t dt;
    uint32_t unix_time;
    ntp_sync_status_t st;
    uint8_t i;

    /* Prepare software RTC as target. */
    (void)software_rtc_init(&sw, (const rtc_datetime_t*)0);
    (void)rtc_init(&rtc, software_rtc_get_driver(), &sw);
    (void)timezone_init(&tz);
    timezone_set_offset_minutes(&tz, 120); /* UTC+2 */
    timezone_set_dst(&tz, 0u);

    for (i = 0u; i < EXAMPLE_NTP_PACKET_SIZE; i++)
    {
        udp.response[i] = 0u;
    }
    /* LI=0, VN=4, Mode=4 (server). */
    udp.response[0] = 0x24u;
    /* Fake transmit timestamp seconds = UNIX(2026-01-01 00:00:00) + delta. */
    {
        uint32_t fake_unix = 1767225600UL;
        uint32_t ntp_secs = fake_unix + EXAMPLE_NTP_UNIX_EPOCH_DELTA;
        udp.response[40] = (uint8_t)(ntp_secs >> 24u);
        udp.response[41] = (uint8_t)(ntp_secs >> 16u);
        udp.response[42] = (uint8_t)(ntp_secs >> 8u);
        udp.response[43] = (uint8_t)(ntp_secs);
    }
    udp.response_len = EXAMPLE_NTP_PACKET_SIZE;
    udp.last_request_first_byte = 0u;

    cfg.send_cb = mock_send;
    cfg.receive_cb = mock_receive;
    cfg.transport_ctx = &udp;
    cfg.ntp_version = 4u;
    (void)ntp_sync_init(&ntp, &cfg);

    st = ntp_sync_request(&ntp);
    if (st == NTP_SYNC_STATUS_OK)
    {
        st = ntp_sync_process_response(&ntp);
    }

    if (st == NTP_SYNC_STATUS_OK)
    {
        st = ntp_sync_apply_to_rtc(&ntp, &rtc, &tz);
    }

    if ((st == NTP_SYNC_STATUS_OK) && (rtc_get_datetime(&rtc, &dt) != 0u))
    {
        DBG_PRINT("Synced local datetime: ");
        DBG_PRINT_INT((int32_t)dt.year);
        DBG_PRINT("-");
        DBG_PRINT_INT((int32_t)dt.month);
        DBG_PRINT("-");
        DBG_PRINT_INT((int32_t)dt.day);
        DBG_PRINT(" ");
        DBG_PRINT_INT((int32_t)dt.hour);
        DBG_PRINT(":");
        DBG_PRINT_INT((int32_t)dt.minute);
        DBG_PRINT(":");
        DBG_PRINT_INT((int32_t)dt.second);
        DBG_PRINTLN("");
    }

    if (ntp_sync_get_unix(&ntp, &unix_time) != 0u)
    {
        DBG_PRINT("NTP UNIX UTC: ");
        DBG_PRINT_INT((int32_t)unix_time);
        DBG_PRINTLN("");
    }

    while (1)
    {
    }
}
