#include "gapcom/gapcom_client.h"

#include "TinyFrame.h"
#include "gapcom/gapcom.h"
#include "gapcom_core.h"
#include "log.h"

#include <gap.pb.h>
#include <pb_encode.h>
#include <stdio.h>

/************************
 *** Public functions ***
 ************************/

int gapcom_request_ping(gapcom_handle_t* handle)
{
    int ret = 0;
    GAPPingReq req = GAPPingReq_init_zero;

    if (!(handle->user_callback[GAPCOM_MSG_PING_RESP]))
    {
        log_com("No callback installed for Ping response; not sending request");
        ret = -1;
        goto end;
    }

    ret += gapcom_encapsulate_protobuf(handle, GAPPingReq_fields, req);
    if (ret == -1)
    {
        log_com("Failed to encode Ping protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_PING_REQ, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Send(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_request_set_log_verbosity(
    gapcom_handle_t* handle,
    GAPLogVerbosity verbosity)
{
    int ret;
    GAPSetLogVerbosityReq req = GAPSetLogVerbosityReq_init_zero;

    ret = 0;
    if (!(handle->user_callback[GAPCOM_MSG_SET_LOG_VERBOSITY_RESP]))
    {
        log_com("No callback installed for Set Log Verbosity response; not sending request");
        ret = -1;
        goto end;
    }

    req.verbosity = verbosity;

    ret += gapcom_encapsulate_protobuf(handle, GAPSetLogVerbosityReq_fields, req);
    if (ret == -1)
    {
        log_com("Failed to encode Set Log Verbosity protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_SET_LOG_VERBOSITY_REQ, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Send(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_request_set_gyroscope(
    gapcom_handle_t* handle,
    bool set)
{
    int ret;
    GAPSetGyroscopeReq req = GAPSetGyroscopeReq_init_zero;

    ret = 0;
    if (!(handle->user_callback[GAPCOM_MSG_SET_GYROSCOPE_RESP]))
    {
        log_com("No callback installed for Set Gyroscope response; not sending request");
        ret = -1;
        goto end;
    }

    req.set = set;

    ret += gapcom_encapsulate_protobuf(handle, GAPSetGyroscopeReq_fields, req);
    if (ret == -1)
    {
        log_com("Failed to encode Set Gyroscope protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_SET_GYROSCOPE_REQ, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Send(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_request_set_version(
    gapcom_handle_t* handle,
    const char *version)
{
    int ret;
    GAPSetVersionReq req = GAPSetVersionReq_init_zero;

    ret = 0;
    if (!(handle->user_callback[GAPCOM_MSG_SET_VERSION_RESP]))
    {
        log_com("No callback installed for Set Version response; not sending request");
        ret = -1;
        goto end;
    }

    if (strlen(version) > GAPCOM_VERSION_MAX_LEN)
    {
        log_com(
            "Given version is too long (> %u); not sending frame",
            GAPCOM_VERSION_MAX_LEN);
        ret = -1;
        goto end;
    }
    strcpy(req.version, version);

    ret += gapcom_encapsulate_protobuf(handle, GAPSetVersionReq_fields, req);
    if (ret == -1)
    {
        log_com("Failed to encode Set Version protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_SET_VERSION_REQ, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Send(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_request_get_version(gapcom_handle_t* handle)
{
    int ret;
    GAPGetVersionReq req = GAPGetVersionReq_init_zero;

    ret = 0;
    if (!(handle->user_callback[GAPCOM_MSG_GET_VERSION_RESP]))
    {
        log_com("No callback installed for Get Version response; not sending request");
        ret = -1;
        goto end;
    }

    ret += gapcom_encapsulate_protobuf(handle, GAPGetVersionReq_fields, req);
    if (ret == -1)
    {
        log_com("Failed to encode Get Version ding frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_GET_VERSION_REQ, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Send(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_request_selftest(gapcom_handle_t* handle)
{
    int ret;
    GAPSelftestReq req = GAPSelftestReq_init_zero;

    ret = 0;
    if (!(handle->user_callback[GAPCOM_MSG_SELFTEST_RESP]))
    {
        log_com("No callback installed for Selftest response; not sending request");
        ret = -1;
        goto end;
    }

    ret += gapcom_encapsulate_protobuf(handle, GAPSelftestReq_fields, req);
    if (ret == -1)
    {
        log_com("Failed to encode Selftest protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_SELFTEST_REQ, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Send(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}

int gapcom_request_power_save_mode(
    gapcom_handle_t* handle,
    bool save_power)
{
    int ret;
    GAPPowerSaveModeReq req = GAPPowerSaveModeReq_init_zero;

    ret = 0;
    if (!(handle->user_callback[GAPCOM_MSG_POWER_SAVE_MODE_RESP]))
    {
        log_com("No callback installed for Power Save Mode response; not sending request");
        ret = -1;
        goto end;
    }

    req.save_power = save_power;

    ret += gapcom_encapsulate_protobuf(handle, GAPPowerSaveModeReq_fields, req);
    if (ret == -1)
    {
        log_com("Failed to encode Power Save Mode protobuf; not sending frame");
        goto end;
    }

    ret += gapcom_encapsulate_tf(handle, GAPCOM_MSG_POWER_SAVE_MODE_REQ, ret);

    /* Send the frame without any callback. This function always succeed. */
    TF_Send(&handle->tf_handle, &handle->tf_msg);

end:
    return ret;
}
