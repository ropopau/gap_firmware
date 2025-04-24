#include "gapcom_core.h"

#include "TinyFrame.h"
#include "log.h"

#include <gap.pb.h>
#include <pb_decode.h>
#include <stdint.h>
#include <unistd.h>

void TF_WriteImpl(TinyFrame* tf_handle, const uint8_t* buf, uint32_t len)
{
    ssize_t bytes_sent;
    gapcom_handle_t* handle;
    gapcom_sender_t* sender;

    handle = container_of(tf_handle, gapcom_handle_t, tf_handle);
    sender = handle->sender_impl;
    bytes_sent = sender->send(sender, buf, len);
    if (bytes_sent < 0)
    {
        log_com("Failed to send message, returned %lld", bytes_sent);
    }
}

#define GAPCOM_TF_ID_LISTENER_CASE(Success, Handle, Callback, StreamPtr, MsgFieldsType, MsgPtr) \
    do { \
        (Success) = pb_decode((StreamPtr), (MsgFieldsType), (MsgPtr)); \
        if (Success) \
        { \
            Callback((Handle), (const void *)(MsgPtr)); \
        } \
        else \
        { \
            log_com("Failed to decode protobuf for message type %s", #MsgFieldsType); \
        } \
    } while (0)

TF_Result gapcom_tf_id_listener(TinyFrame* tf_handle, TF_Msg* tf_msg)
{
    gapcom_handle_t* handle;
    gapcom_callback_t callback;
    pb_istream_t stream;
    bool success;

    /* Check if there is any user-defined callback to be called */
    handle = container_of(tf_handle, gapcom_handle_t, tf_handle);
    callback = handle->user_callback[tf_msg->type];
    if (!callback)
        goto end;

    /* Decode Protocol Buffer payload */
    stream = pb_istream_from_buffer(tf_handle->data, tf_handle->len);
    if (tf_msg->type == GAPCOM_MSG_PING_REQ)
    {
        GAPPingReq req = GAPPingReq_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPPingReq_fields, &req);
    }
    else if (tf_msg->type == GAPCOM_MSG_PING_RESP)
    {
        GAPPingResp resp = GAPPingResp_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPPingResp_fields, &resp);
    }
    else if (tf_msg->type == GAPCOM_MSG_SET_LOG_VERBOSITY_REQ)
    {
        GAPSetLogVerbosityReq req = GAPSetLogVerbosityReq_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPSetLogVerbosityReq_fields, &req);
    }
    else if (tf_msg->type == GAPCOM_MSG_SET_LOG_VERBOSITY_RESP)
    {
        GAPSetLogVerbosityResp resp = GAPSetLogVerbosityResp_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPSetLogVerbosityResp_fields, &resp);
    }
    else if (tf_msg->type == GAPCOM_MSG_SET_GYROSCOPE_REQ)
    {
        GAPSetGyroscopeReq req = GAPSetGyroscopeReq_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPSetGyroscopeReq_fields, &req);
    }
    else if (tf_msg->type == GAPCOM_MSG_SET_GYROSCOPE_RESP)
    {
        GAPSetGyroscopeResp resp = GAPSetGyroscopeResp_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPSetGyroscopeResp_fields, &resp);
    }
    else if (tf_msg->type == GAPCOM_MSG_SET_VERSION_REQ)
    {
        GAPSetVersionReq req = GAPSetVersionReq_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPSetVersionReq_fields, &req);
    }
    else if (tf_msg->type == GAPCOM_MSG_SET_VERSION_RESP)
    {
        GAPSetVersionResp resp = GAPSetVersionResp_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPSetVersionResp_fields, &resp);
    }
    else if (tf_msg->type == GAPCOM_MSG_GET_VERSION_REQ)
    {
        GAPGetVersionReq req = GAPGetVersionReq_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPGetVersionReq_fields, &req);
    }
    else if (tf_msg->type == GAPCOM_MSG_GET_VERSION_RESP)
    {
        GAPGetVersionResp resp = GAPGetVersionResp_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPGetVersionResp_fields, &resp);
    }
    else if (tf_msg->type == GAPCOM_MSG_SELFTEST_REQ)
    {
        GAPSelftestReq req = GAPSelftestReq_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPSelftestReq_fields, &req);
    }
    else if (tf_msg->type == GAPCOM_MSG_SELFTEST_RESP)
    {
        GAPSelftestResp resp = GAPSelftestResp_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPSelftestResp_fields, &resp);
    }
    else if (tf_msg->type == GAPCOM_MSG_POWER_SAVE_MODE_REQ)
    {
        GAPPowerSaveModeReq req = GAPPowerSaveModeReq_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPPowerSaveModeReq_fields, &req);
    }
    else if (tf_msg->type == GAPCOM_MSG_POWER_SAVE_MODE_RESP)
    {
        GAPPowerSaveModeResp resp = GAPPowerSaveModeResp_init_zero;
        GAPCOM_TF_ID_LISTENER_CASE(success, handle, callback, &stream, GAPPowerSaveModeResp_fields, &resp);
    }
    else
    {
        log_com("Unsupported protobuf message type %u; ignoring", tf_msg->type);
    }

end:
    return TF_STAY;
}

int gapcom_encapsulate_tf(
    gapcom_handle_t* handle,
    gapcom_msg_t msg_type,
    int msg_len)
{
    /* Build TinyFrame frame pointing to payload */
    TF_ClearMsg(&handle->tf_msg);
    handle->tf_msg.type = msg_type;
    handle->tf_msg.data = (const uint8_t*)(&handle->sendbuf);
    handle->tf_msg.len = (TF_LEN)msg_len;
    /* When the payload is empty (which can happen with protobuf, for example
     * with a single numeric field with value 0 : it will get compressed and
     * interpreted as 0 upon decoding) the payload CRC is not appended by
     * TinyFrame. Otherwise, we do have a payload CRC as a footer.
     */
    if (msg_len > 0)
        return GAPCOM_TF_HEADER_SIZE_BYTES + GAPCOM_TF_FOOTER_SIZE_BYTES;
    else
        return GAPCOM_TF_HEADER_SIZE_BYTES;
}
