#include "libraries/time/ntp_sync/ntp_sync.h"

#define NTP_PACKET_SIZE 48u
#define NTP_UNIX_EPOCH_DELTA 2208988800UL

static uint32_t ntp_read_u32_be(const uint8_t* p)
{
    return ((uint32_t)p[0] << 24u) |
           ((uint32_t)p[1] << 16u) |
           ((uint32_t)p[2] << 8u) |
           (uint32_t)p[3];
}

uint8_t ntp_sync_init(ntp_sync_t* ntp, const ntp_sync_config_t* config)
{
    if ((ntp == (ntp_sync_t*)0) || (config == (const ntp_sync_config_t*)0))
    {
        return 0u;
    }

    if ((config->send_cb == (ntp_send_callback_t)0) || (config->receive_cb == (ntp_receive_callback_t)0))
    {
        return 0u;
    }

    ntp->config = *config;
    if (ntp->config.ntp_version == 0u)
    {
        ntp->config.ntp_version = 4u;
    }

    ntp->unix_utc = 0u;
    ntp->valid = 0u;
    ntp->initialized = 1u;
    return 1u;
}

ntp_sync_status_t ntp_sync_request(ntp_sync_t* ntp)
{
    uint8_t pkt[NTP_PACKET_SIZE];
    uint8_t i;
    uint8_t ok;

    if ((ntp == (ntp_sync_t*)0) || (ntp->initialized == 0u))
    {
        return NTP_SYNC_STATUS_ERROR;
    }

    for (i = 0u; i < NTP_PACKET_SIZE; i++)
    {
        pkt[i] = 0u;
    }

    /* LI=0, VN=configured, Mode=3 (client). */
    pkt[0] = (uint8_t)((ntp->config.ntp_version << 3u) | 0x03u);

    ok = ntp->config.send_cb(pkt, NTP_PACKET_SIZE, ntp->config.transport_ctx);
    if (ok == 0u)
    {
        return NTP_SYNC_STATUS_SEND_FAILED;
    }

    return NTP_SYNC_STATUS_OK;
}

ntp_sync_status_t ntp_sync_process_response(ntp_sync_t* ntp)
{
    uint8_t pkt[NTP_PACKET_SIZE];
    uint16_t len;
    uint8_t li_vn_mode;
    uint8_t version;
    uint8_t mode;
    uint32_t ntp_secs;

    if ((ntp == (ntp_sync_t*)0) || (ntp->initialized == 0u))
    {
        return NTP_SYNC_STATUS_ERROR;
    }

    len = ntp->config.receive_cb(pkt, NTP_PACKET_SIZE, ntp->config.transport_ctx);
    if (len == 0u)
    {
        return NTP_SYNC_STATUS_NO_RESPONSE;
    }

    if (len < NTP_PACKET_SIZE)
    {
        return NTP_SYNC_STATUS_INVALID_SIZE;
    }

    li_vn_mode = pkt[0];
    version = (uint8_t)((li_vn_mode >> 3u) & 0x07u);
    mode = (uint8_t)(li_vn_mode & 0x07u);

    if ((mode != 4u) && (mode != 5u))
    {
        return NTP_SYNC_STATUS_INVALID_MODE_VERSION;
    }

    if ((version == 0u) || (version > 4u))
    {
        return NTP_SYNC_STATUS_INVALID_MODE_VERSION;
    }

    /* Transmit Timestamp seconds field at bytes 40..43. */
    ntp_secs = ntp_read_u32_be(&pkt[40]);
    if (ntp_secs < NTP_UNIX_EPOCH_DELTA)
    {
        return NTP_SYNC_STATUS_INVALID_TIMESTAMP;
    }

    ntp->unix_utc = (uint32_t)(ntp_secs - NTP_UNIX_EPOCH_DELTA);
    ntp->valid = 1u;

    return NTP_SYNC_STATUS_OK;
}

ntp_sync_status_t ntp_sync_apply_to_rtc(ntp_sync_t* ntp, rtc_t* rtc, const timezone_t* tz)
{
    rtc_datetime_t utc_dt;
    rtc_datetime_t local_dt;

    if ((ntp == (ntp_sync_t*)0) || (rtc == (rtc_t*)0) || (ntp->initialized == 0u))
    {
        return NTP_SYNC_STATUS_ERROR;
    }

    if (ntp->valid == 0u)
    {
        return NTP_SYNC_STATUS_INVALID_TIMESTAMP;
    }

    if (rtc_set_unix(rtc, ntp->unix_utc) == 0u)
    {
        return NTP_SYNC_STATUS_RTC_UPDATE_FAILED;
    }

    if (tz != (const timezone_t*)0)
    {
        if (rtc_get_datetime(rtc, &utc_dt) == 0u)
        {
            return NTP_SYNC_STATUS_RTC_UPDATE_FAILED;
        }

        if (timezone_utc_to_local(tz, &utc_dt, &local_dt) == 0u)
        {
            return NTP_SYNC_STATUS_ERROR;
        }

        if (rtc_set_datetime(rtc, &local_dt) == 0u)
        {
            return NTP_SYNC_STATUS_RTC_UPDATE_FAILED;
        }
    }

    return NTP_SYNC_STATUS_OK;
}

uint8_t ntp_sync_is_valid(const ntp_sync_t* ntp)
{
    if ((ntp == (const ntp_sync_t*)0) || (ntp->initialized == 0u))
    {
        return 0u;
    }
    return ntp->valid;
}

uint8_t ntp_sync_get_unix(const ntp_sync_t* ntp, uint32_t* unix_time)
{
    if ((ntp == (const ntp_sync_t*)0) || (unix_time == (uint32_t*)0) || (ntp->initialized == 0u))
    {
        return 0u;
    }

    if (ntp->valid == 0u)
    {
        return 0u;
    }

    *unix_time = ntp->unix_utc;
    return 1u;
}
