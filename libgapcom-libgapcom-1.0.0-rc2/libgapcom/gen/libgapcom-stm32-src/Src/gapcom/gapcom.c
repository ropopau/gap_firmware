#include "gapcom/gapcom.h"

#include "TinyFrame.h"
#include "gapcom_core.h"
#include "log.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/************************
 *** Public functions ***
 ************************/

gapcom_handle_t* gapcom_create()
{
    gapcom_handle_t* handle;
    bool status;

    handle = calloc(1, sizeof(*handle));
    if (!handle)
        goto end;

    status = TF_InitStatic(&handle->tf_handle, TF_MASTER);
    if (!status)
        goto free_handle;

    /* All libgapcom instances use the same generic listener */
    TF_AddGenericListener(&handle->tf_handle, gapcom_tf_id_listener);

    goto end;

free_handle:
    free(handle);
    handle = NULL;
end:
    return handle;
}

void gapcom_destroy(gapcom_handle_t* handle)
{
    if (handle)
    {
        /* Don't call TF_DeInit() because it is meant for TF handles allocated
         * with an individual call to malloc().
         */
        free(handle);
    }
}

void gapcom_set_sender_impl(gapcom_handle_t* handle, gapcom_sender_t* sender_impl)
{
    handle->sender_impl = sender_impl;
}

void gapcom_install_callback(
    gapcom_handle_t* handle,
    gapcom_callback_t callback,
    gapcom_msg_t msg_type)
{
    if (msg_type < GAPCOM_MSG_MIN || msg_type > GAPCOM_MSG_MAX)
    {
        log_com("Unexpected msg_type for callback: %u", msg_type);
        return;
    }
    else
    {
        handle->user_callback[msg_type] = callback;
    }
}

void gapcom_uninstall_callback(
    gapcom_handle_t* handle,
    gapcom_msg_t msg_type)
{
    handle->user_callback[msg_type] = NULL;
}

void gapcom_accept(gapcom_handle_t* handle, const uint8_t* buf, size_t len)
{
    log_com("Accepting message");
    /* CAUTION: TinyFrame offers no way of checking if this succeeded ! */
    TF_Accept(&handle->tf_handle, buf, len);
}
