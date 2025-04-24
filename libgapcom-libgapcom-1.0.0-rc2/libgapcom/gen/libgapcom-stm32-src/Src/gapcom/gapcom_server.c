#include "gapcom/gapcom_server.h"

#include "TinyFrame.h"
#include "gapcom/gapcom.h"
#include "gapcom_core.h"
#include "log.h"

#include <gap.pb.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include <stdio.h>

/************************
 *** Public functions ***
 ************************/

int gapcom_respond_ping(
    gapcom_handle_t* handle,
    GAPErrorCode error_code)
{
    int ret = 0;
    GAPPingResp resp = GAPPingResp_init_zero;

    resp.error_code = error_code;

    ret += gapcom_encapsulate_protobuf(handle, GAPPingResp_fields, resp);
    if (ret == -1)
    {
        log_com("Failed to encode Ping protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_PING_RESP, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Respond(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_respond_set_log_verbosity(
    gapcom_handle_t* handle,
    GAPErrorCode error_code)
{
    int ret = 0;
    GAPSetLogVerbosityResp resp = GAPSetLogVerbosityResp_init_zero;

    resp.error_code = error_code;

    ret += gapcom_encapsulate_protobuf(handle, GAPSetLogVerbosityResp_fields, resp);
    if (ret == -1)
    {
        log_com("Failed to encode Set Log Verbosity protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_SET_LOG_VERBOSITY_RESP, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Respond(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_respond_set_gyroscope(
    gapcom_handle_t* handle,
    GAPErrorCode error_code)
{
    int ret = 0;
    GAPSetGyroscopeResp resp = GAPSetGyroscopeResp_init_zero;

    resp.error_code = error_code;

    ret += gapcom_encapsulate_protobuf(handle, GAPSetGyroscopeResp_fields, resp);
    if (ret == -1)
    {
        log_com("Failed to encode Set Gyroscope protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_SET_GYROSCOPE_RESP, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Respond(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_respond_set_version(
    gapcom_handle_t* handle,
    GAPErrorCode error_code)
{
    int ret = 0;
    GAPSetVersionResp resp = GAPSetVersionResp_init_zero;

    resp.error_code = error_code;

    ret += gapcom_encapsulate_protobuf(handle, GAPSetVersionResp_fields, resp);
    if (ret == -1)
    {
        log_com("Failed to encode Set Version protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_SET_VERSION_RESP, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Respond(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_respond_get_version(
    gapcom_handle_t* handle,
    GAPErrorCode error_code,
    const char *version)
{
    int ret = 0;
    GAPGetVersionResp resp = GAPGetVersionResp_init_zero;

    resp.error_code = error_code;
    if (strlen(version) > GAPCOM_VERSION_MAX_LEN)
    {
        log_com(
            "Given version is too long (> %u); not sending frame",
            GAPCOM_VERSION_MAX_LEN);
        ret = -1;
        goto end;
    }
    strcpy(resp.version, version);

    ret += gapcom_encapsulate_protobuf(handle, GAPGetVersionResp_fields, resp);
    if (ret == -1)
    {
        log_com("Failed to encode Get Version protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_GET_VERSION_RESP, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Respond(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_respond_selftest(
    gapcom_handle_t* handle,
    GAPErrorCode error_code,
    GAPSelftestCode flash_test_code,
    GAPSelftestCode debuguart_test_code,
    GAPSelftestCode gyroscope_test_code,
    GAPSelftestCode led_test_code)
{
    int ret = 0;
    GAPSelftestResp resp = GAPSelftestResp_init_zero;

    resp.error_code = error_code;
    resp.flash_test_code = flash_test_code;
    resp.debuguart_test_code = debuguart_test_code;
    resp.gyroscope_test_code = gyroscope_test_code;
    resp.led_test_code = led_test_code;

    ret += gapcom_encapsulate_protobuf(handle, GAPSelftestResp_fields, resp);
    if (ret == -1)
    {
        log_com("Failed to encode Selftest protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_SELFTEST_RESP, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Respond(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_respond_power_save_mode(
    gapcom_handle_t* handle,
    GAPErrorCode error_code)
{
    int ret = 0;
    GAPPowerSaveModeResp resp = GAPPowerSaveModeResp_init_zero;

    resp.error_code = error_code;

    ret += gapcom_encapsulate_protobuf(handle, GAPPowerSaveModeResp_fields, resp);
    if (ret == -1)
    {
        log_com("Failed to encode Power Save Mode protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_POWER_SAVE_MODE_RESP, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Respond(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}
