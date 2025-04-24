#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "gapcom/gapcom.h"
#include "gapcom/gapcom_client.h"
#include "communicator.h"
#include "communicator_impl_client.h"
#include "gap.pb.h"

#include "log.h"
#include "options.h"



/***************************
 *** Private definitions ***
 ***************************/

#ifndef GAPCLI_RECVBUF_SIZE
#define GAPCLI_RECVBUF_SIZE 256
#endif

typedef enum gapcli_retcode_t
{
    GAPCLI_SUCCESS = 0,
    GAPCLI_SETUP_FAILED = 1,
    GAPCLI_CMDLINE_PARSING_FAILED = 2,
    GAPCLI_RECV_MSG_FAILED = 3,
    GAPCLI_SEND_MSG_FAILED = 4,
} gapcli_retcode_t;

typedef enum gapcli_cmd_type_t
{
    GAPCOM_MSG_PING = 0,
    GAPCOM_MSG_SET_LOG_VERBOSITY,
    GAPCOM_MSG_SET_GYROSCOPE,
    GAPCOM_MSG_SET_VERSION,
    GAPCOM_MSG_GET_VERSION,
    GAPCOM_MSG_SELFTEST,
    GAPCOM_MSG_POWER_SAVE_MODE,
} gapcli_cmd_type_t;

typedef struct gapcli_cmd_base_t
{
    gapcli_cmd_type_t type;
} gapcli_cmd_base_t;

typedef struct gapcom_cmd_ping_t
{
    gapcli_cmd_base_t base;
} gapcom_cmd_ping_t;

typedef struct gapcom_cmd_set_log_verbosity_t
{
    gapcli_cmd_base_t base;
    GAPLogVerbosity verbosity;
} gapcom_cmd_set_log_verbosity_t;

typedef struct gapcom_cmd_set_gyroscope_t
{
    gapcli_cmd_base_t base;
    bool set;
} gapcom_cmd_set_gyroscope_t;

typedef struct gapcom_cmd_set_version_t
{
    gapcli_cmd_base_t base;
    char version[GAPCOM_VERSION_MAX_LEN + 1]; /* + trailing \0 */
} gapcom_cmd_set_version_t;

typedef struct gapcom_cmd_get_version_t
{
    gapcli_cmd_base_t base;
} gapcom_cmd_get_version_t;

typedef struct gapcom_cmd_selftest_t
{
    gapcli_cmd_base_t base;
} gapcom_cmd_selftest_t;

typedef struct gapcom_cmd_power_save_mode_t
{
    gapcli_cmd_base_t base;
    bool save_power;
} gapcom_cmd_power_save_mode_t;

typedef struct gapcli_cmd_t
{
    options_t* options;
    gapcli_cmd_base_t* command;
} gapcli_cmd_t;

/*************************
 *** Private functions ***
 *************************/

static const char* gap_error_code_as_str[] = {
    "OK",                               /* GAPErrorCode_GAP_OK */
    "Feature not implemented",          /* GAPErrorCode_GAP_FEATURE_NOT_IMPLEMENTED */
    "Invalid log verbosity",            /* GAPErrorCode_GAP_INVALID_LOG_VERBOSITY */
    "Invalid version format",           /* GAPErrorCode_GAP_INVALID_VERSION_FORMAT */
};

static const char* gap_selftest_code_as_str[] = {
    "OK",                           /* GAPSelftestCode_GAP_SELFTEST_OK */
    "Flash failed to read",         /* GAPSelftestCode_GAP_SELFTEST_FLASH_FAILED_TO_READ */
    "Flash failed to write",        /* GAPSelftestCode_GAP_SELFTEST_FLASH_FAILED_TO_WRITE */
    "Flash readback is different",  /* GAPSelftestCode_GAP_SELFTEST_FLASH_READ_BACK_IS_DIFFERENT */
    "UART debug level NOK",         /* GAPSelftestCode_UART_DEBUG_NOK */
    "UART info level NOK",          /* GAPSelftestCode_UART_INFO_NOK */
    "UART warning level NOK",       /* GAPSelftestCode_UART_WARNING_NOK */
    "UART error level NOK",         /* GAPSelftestCode_ERROR_NOK */
    "Gyroscope X axis NOK",         /* GAPSelftestCode_GYROSCOPE_X_NOK */
    "Gyroscope Y axis NOK",         /* GAPSelftestCode_GYROSCOPE_Y_NOK */
    "Gyroscope Z axis NOK",         /* GAPSelftestCode_GYROSCOPE_Z_NOK */
    "Green LED NOK",                /* GAPSelftestCode_LED_GREEN_NOK */
    "Red LED NOK",                  /* GAPSelftestCode_LED_RED_NOK */
};

static void ping_callback(gapcom_handle_t* handle, const void* msg)
{
    (void)handle; /* Ignored */
    GAPPingResp *resp = (GAPPingResp *)msg;

    log_cli("Got Ping response");
    log_cli("\tError code : %s", gap_error_code_as_str[resp->error_code]);
}

static void set_log_verbosity_callback(gapcom_handle_t* handle, const void* msg)
{
    (void)handle; /* Ignored */
    GAPSetLogVerbosityResp *resp = (GAPSetLogVerbosityResp *)msg;

    log_cli("Got Set Log Verbosity response");
    log_cli("\tError code : %s", gap_error_code_as_str[resp->error_code]);
}

static void set_gyroscope_callback(gapcom_handle_t* handle, const void* msg)
{
    (void)handle; /* Ignored */
    GAPSetGyroscopeResp *resp = (GAPSetGyroscopeResp *)msg;

    log_cli("Got Set Gyroscope response");
    log_cli("\tError code : %s", gap_error_code_as_str[resp->error_code]);
}

static void set_version_callback(gapcom_handle_t* handle, const void* msg)
{
    (void)handle; /* Ignored */
    GAPSetVersionResp *resp = (GAPSetVersionResp *)msg;

    log_cli("Got Set Version response");
    log_cli("\tError code : %s", gap_error_code_as_str[resp->error_code]);
}

static void get_version_callback(gapcom_handle_t* handle, const void* msg)
{
    (void)handle; /* Ignored */
    GAPGetVersionResp *resp = (GAPGetVersionResp *)msg;

    log_cli("Got Get Version response");
    log_cli("\tError code : %s", gap_error_code_as_str[resp->error_code]);
    log_cli("\tVersion : %s", resp->version);
}

static void selftest_callback(gapcom_handle_t* handle, const void* msg)
{
    (void)handle; /* Ignored */
    GAPSelftestResp *resp = (GAPSelftestResp *)msg;

    log_cli("Got Selftest response");
    log_cli("\tError code : %s", gap_error_code_as_str[resp->error_code]);
    log_cli("\tFlash selftest code : %s", gap_selftest_code_as_str[resp->flash_test_code]);
    log_cli("\tDebug UART selftest code : %s", gap_selftest_code_as_str[resp->debuguart_test_code]);
    log_cli("\tGyroscope selftest code : %s", gap_selftest_code_as_str[resp->gyroscope_test_code]);
    log_cli("\tLED selftest code : %s", gap_selftest_code_as_str[resp->led_test_code]);
}

static void power_save_mode_callback(gapcom_handle_t* handle, const void* msg)
{
    (void)handle; /* Ignored */
    GAPPowerSaveModeResp *resp = (GAPPowerSaveModeResp *)msg;

    log_cli("Got Power Save Mode response");
    log_cli("\tError code : %s", gap_error_code_as_str[resp->error_code]);
}

static void gapcli_install_callbacks(gapcom_handle_t* handle)
{
    gapcom_install_callback(handle, ping_callback, GAPCOM_MSG_PING_RESP);
    gapcom_install_callback(handle, set_log_verbosity_callback, GAPCOM_MSG_SET_LOG_VERBOSITY_RESP);
    gapcom_install_callback(handle, set_gyroscope_callback, GAPCOM_MSG_SET_GYROSCOPE_RESP);
    gapcom_install_callback(handle, set_version_callback, GAPCOM_MSG_SET_VERSION_RESP);
    gapcom_install_callback(handle, get_version_callback, GAPCOM_MSG_GET_VERSION_RESP);
    gapcom_install_callback(handle, selftest_callback, GAPCOM_MSG_SELFTEST_RESP);
    gapcom_install_callback(handle, power_save_mode_callback, GAPCOM_MSG_POWER_SAVE_MODE_RESP);
}

void gapcli_print_usage()
{
    log_cli("Usage: gapcli [options...] <cmd> [args...]");
    log_cli("List of available options:");
    log_cli("\t-com <path> : serial communication port (examples : /dev/ttyACM0 on Linux, COM3 on Windows)");
    log_cli("List of available commands:");
    log_cli("\tping : Test communication, nothing else");
    log_cli("\tset-log-verbosity debug|info|warning|error : Modify log verbosity level on debug UART");
    log_cli("\tset-gyroscope on|off: Enable or disable reading the gyroscope");
    log_cli("\tset-version <version> : Set the version string of the Server, e.g. 1.2.0");
    log_cli("\tget-version : Get the version string of the Server");
    log_cli("\tselftest : Ask the Server to perform a hardware selftest, and return the results");
    log_cli("\tpower-save-mode enter|exit : Make Server enter or exit power saving mode");
}

gapcli_cmd_base_t* gapcom_parse_cmdline_ping(int argc, const char* argv[])
{
    (void)argv; /* Ignored */
    gapcom_cmd_ping_t* cmd;

    if (argc != 0)
        return NULL;
    cmd = calloc(1, sizeof(*cmd));
    if (!cmd)
        return NULL;

    cmd->base.type = GAPCOM_MSG_PING;
    return (gapcli_cmd_base_t*)cmd;
}

gapcli_cmd_base_t* gapcom_parse_cmdline_set_log_verbosity(int argc, const char* argv[])
{
    gapcom_cmd_set_log_verbosity_t* cmd;
    const char *verbosity_arg;

    if (argc != 1)
        return NULL;
    verbosity_arg = argv[0];
    cmd = calloc(1, sizeof(*cmd));
    if (!cmd)
        return NULL;

    cmd->base.type = GAPCOM_MSG_SET_LOG_VERBOSITY;
    if (strcmp(verbosity_arg, "debug") == 0) {
        cmd->verbosity = GAPLogVerbosity_GAP_LOG_DEBUG;
    } else if (strcmp(verbosity_arg, "info") == 0) {
        cmd->verbosity = GAPLogVerbosity_GAP_LOG_INFO;
    } else if (strcmp(verbosity_arg, "warning") == 0) {
        cmd->verbosity = GAPLogVerbosity_GAP_LOG_WARNING;
    } else if (strcmp(verbosity_arg, "error") == 0) {
        cmd->verbosity = GAPLogVerbosity_GAP_LOG_ERROR;
    } else {
        free(cmd);
        cmd = NULL;
    }
    return (gapcli_cmd_base_t*)cmd;
}

gapcli_cmd_base_t* gapcom_parse_cmdline_set_gyroscope(int argc, const char* argv[])
{
    gapcom_cmd_set_gyroscope_t* cmd;
    const char *set_arg;

    if (argc != 1)
        return NULL;
    set_arg = argv[0];
    cmd = calloc(1, sizeof(*cmd));
    if (!cmd)
        return NULL;

    cmd->base.type = GAPCOM_MSG_SET_GYROSCOPE;
    if (strcmp(set_arg, "on") == 0) {
        cmd->set = true;
    } else if (strcmp(set_arg, "off") == 0) {
        cmd->set = false;
    } else {
        free(cmd);
        cmd = NULL;
    }
    return (gapcli_cmd_base_t*)cmd;
}

gapcli_cmd_base_t* gapcom_parse_cmdline_set_version(int argc, const char* argv[])
{
    gapcom_cmd_set_version_t* cmd;
    const char *version_arg;

    if (argc != 1)
        return NULL;
    version_arg = argv[0];
    cmd = calloc(1, sizeof(*cmd));
    if (!cmd)
        return NULL;

    cmd->base.type = GAPCOM_MSG_SET_VERSION;
    if (strlen(version_arg) <= GAPCOM_VERSION_MAX_LEN) {
        strncpy(cmd->version, version_arg, strlen(version_arg));
    } else {
        free(cmd);
        cmd = NULL;
    }
    return (gapcli_cmd_base_t*)cmd;
}

gapcli_cmd_base_t* gapcom_parse_cmdline_get_version(int argc, const char* argv[])
{
    (void)argv; /* Ignored */
    gapcom_cmd_get_version_t* cmd;

    if (argc != 0)
        return NULL;
    cmd = calloc(1, sizeof(*cmd));
    if (!cmd)
        return NULL;

    cmd->base.type = GAPCOM_MSG_GET_VERSION;
    return (gapcli_cmd_base_t*)cmd;
}

gapcli_cmd_base_t* gapcom_parse_cmdline_selftest(int argc, const char* argv[])
{
    (void)argv; /* Ignored */
    gapcom_cmd_selftest_t* cmd;

    if (argc != 0)
        return NULL;
    cmd = calloc(1, sizeof(*cmd));
    if (!cmd)
        return NULL;

    cmd->base.type = GAPCOM_MSG_SELFTEST;
    return (gapcli_cmd_base_t*)cmd;
}

gapcli_cmd_base_t* gapcom_parse_cmdline_power_save_mode(int argc, const char* argv[])
{
    gapcom_cmd_power_save_mode_t* cmd;
    const char *save_power_arg;

    if (argc != 1)
        return NULL;
    save_power_arg = argv[0];
    cmd = calloc(1, sizeof(*cmd));
    if (!cmd)
        return NULL;

    cmd->base.type = GAPCOM_MSG_POWER_SAVE_MODE;
    if (strcmp(save_power_arg, "enter") == 0) {
        cmd->save_power = true;
    } else if (strcmp(save_power_arg, "exit") == 0) {
        cmd->save_power = false;
    } else {
        free(cmd);
        cmd = NULL;
    }
    return (gapcli_cmd_base_t*)cmd;
}

static gapcli_cmd_base_t* gapcom_parse_cmdline_command(int argc, const char* argv[])
{
    gapcli_cmd_base_t* cmd;
    const char* cmd_type_str;

    if (argc < 1)
    {
        return NULL;
    }

    cmd = NULL;
    cmd_type_str = argv[0];
    if (strcmp(cmd_type_str, "ping") == 0)
        cmd = gapcom_parse_cmdline_ping(argc - 1, argv + 1);
    else if (strcmp(cmd_type_str, "set-log-verbosity") == 0)
        cmd = gapcom_parse_cmdline_set_log_verbosity(argc - 1, argv + 1);
    else if (strcmp(cmd_type_str, "set-gyroscope") == 0)
        cmd = gapcom_parse_cmdline_set_gyroscope(argc - 1, argv + 1);
    else if (strcmp(cmd_type_str, "set-version") == 0)
        cmd = gapcom_parse_cmdline_set_version(argc - 1, argv + 1);
    else if (strcmp(cmd_type_str, "get-version") == 0)
        cmd = gapcom_parse_cmdline_get_version(argc - 1, argv + 1);
    else if (strcmp(cmd_type_str, "selftest") == 0)
        cmd = gapcom_parse_cmdline_selftest(argc - 1, argv + 1);
    else if (strcmp(cmd_type_str, "power-save-mode") == 0)
        cmd = gapcom_parse_cmdline_power_save_mode(argc - 1, argv + 1);
    else
        log_cli("Unknown command %s", cmd_type_str);

    return cmd;
}

static gapcli_cmd_t* gapcom_parse_cmdline(int argc, const char* argv[])
{
    gapcli_cmd_t* cmd;

    if (argc < 1)
    {
        log_cli("Error: no argument nor option given");
        gapcli_print_usage();
        return NULL;
    }
    cmd = calloc(1, sizeof(*cmd));
    if (!cmd)
        return NULL;

    /* The options parser will modify argc and argv to "consume" parts of
     * the command-line that were used. No need to tell this to the caller.
     */
    cmd->options = options_parse_cmdline(&argc, &argv);
    if (!(cmd->options))
    {
        log_cli("Failed to parse option(s); aborting");
        gapcli_print_usage();
        free(cmd);
        return NULL;
    }
    if (argc == 0)
    {
        log_cli("No command given; aborting");
        gapcli_print_usage();
        free(cmd);
        return NULL;
    }
    cmd->command = gapcom_parse_cmdline_command(argc, argv);
    if (!(cmd->command))
    {
        log_cli("Failed to parse command; aborting");
        gapcli_print_usage();
        options_destroy_cmdline(cmd->options);
        free(cmd);
        return NULL;
    }
    return cmd;
}

static void gapcli_destroy_cmdline(gapcli_cmd_t* cmd)
{
    if (!cmd)
        return;

    options_destroy_cmdline(cmd->options);
    free(cmd->command);
    free(cmd);
}

/*************************
 *** Public functions ***
 *************************/

int main(int argc, const char* argv[])
{
    int ret;
    uint8_t recvbuf[GAPCLI_RECVBUF_SIZE];
    int sent_bytes;
    ssize_t recv_bytes;
    gapcom_handle_t* handle;
    communicator_t* communicator_impl;
    gapcli_cmd_t* cmd;

    /* Start by obtaining a session handle for libgapcom. Each user of the library
     * should obtain its own handle, which maintains local state.
     */
    ret = GAPCLI_SUCCESS;
    handle = gapcom_create();
    if (!handle)
    {
        ret = GAPCLI_SETUP_FAILED;
        goto end;
    }
    /* After obtaining the handle, we can install our callbacks. Here a single function
     * does this, but take a look at what it does to understand.
     *
     * As we are a client, we send *requests* but install callbacks on *responses*.
     * Our callbacks will be called automatically after invoking gapcom_accept() on
     * the bytes we received from the server; see below for details.
     */
    gapcli_install_callbacks(handle);

    /* Parse the given commmand line. Skip program name (argv[0]).
     *
     * In our case we need the path/name of the UART communication backend
     * (something like /dev/ttyACM0 for Linux, or COM3 for Windows) and the 
     * user-friendly name of the request to send to the server.
     */
    --argc;
    ++argv;
    cmd = gapcom_parse_cmdline(argc, argv);
    if (!cmd)
    {
        ret = GAPCLI_CMDLINE_PARSING_FAILED;
        goto destroy_handle;
    }

    /* Before sending any request to the server, we need to "explain" libgapcom how to
     * actually send bytes. Indeed, libgapcom handles encoding/decoding frames but does
     * not impose a communication medium : that choice is left to the user
     * using the option : -com <serial-port>
     *
     * This separation of responsibility allows reusing libgapcom without the need to
     * rebuild it if the communication backend changes. It is also useful to keep libgapcom
     * small; let's not forget the same library code will be used on an STM32 MCU which
     * has limited resources.
     *
     * The important part here is the call to gapcom_set_sender_impl(...). This sample
     * code has a little overhead abstracting away the communication backend, but that
     * has nothing to do with the GAP protocol itself.
     *
     *
     * The "sender" object is an "interface" in programming terms. While C does not offer
     * interfaces like e.g. Java does, we mimic them by implementing a struct only containing
     * pointers to functions. Please look at gapcom_sender_t declaration in gapcom.h.
     */
    communicator_impl = communicator_impl_client_get();
    if (communicator_impl->open(communicator_impl, (const char*)(cmd->options->com_node_path)) == -1)
    {
        log_cli("Failed to open client; aborting");
        ret = GAPCLI_SETUP_FAILED;
        goto destroy_cmdline;
    }
    gapcom_set_sender_impl(handle, (gapcom_sender_t*)communicator_impl);

    /* Depending on the given command, send a request to the server. All gapcom_request_*()
     * functions check arguments, encode the message, and send it using the "sender" object
     * we just gave in the previous section.
     *
     * If you have not installed a callback for the associated response, sending the request
     * will fail with an error message. Sending requests without caring about the response
     * response makes no sense with the current design, and this small verification can help
     * find a logic error in the program.
     */
    switch (cmd->command->type)
    {
        case GAPCOM_MSG_PING:
            sent_bytes = gapcom_request_ping(handle);
            break;
        case GAPCOM_MSG_SET_LOG_VERBOSITY:
            sent_bytes = gapcom_request_set_log_verbosity(
                handle,
                ((gapcom_cmd_set_log_verbosity_t *)(cmd->command))->verbosity
            );
            break;
        case GAPCOM_MSG_SET_GYROSCOPE:
            sent_bytes = gapcom_request_set_gyroscope(
                handle,
                ((gapcom_cmd_set_gyroscope_t *)(cmd->command))->set
            );
            break;
        case GAPCOM_MSG_SET_VERSION:
            sent_bytes = gapcom_request_set_version(
                handle,
                ((gapcom_cmd_set_version_t *)(cmd->command))->version
            );
            break;
        case GAPCOM_MSG_GET_VERSION:
            sent_bytes = gapcom_request_get_version(handle);
            break;
        case GAPCOM_MSG_SELFTEST:
            sent_bytes = gapcom_request_selftest(handle);
            break;
        case GAPCOM_MSG_POWER_SAVE_MODE:
            sent_bytes = gapcom_request_power_save_mode(
                handle,
                ((gapcom_cmd_power_save_mode_t *)(cmd->command))->save_power
            );
            break;
        default:
            log_cli("Failed to parse command line");
            ret = GAPCLI_CMDLINE_PARSING_FAILED;
            goto close_communicator;
    }
    if (sent_bytes <= 0)
    {
        ret = GAPCLI_SEND_MSG_FAILED;
        log_cli("Failed to query, returned %d", sent_bytes);
        goto close_communicator;
    }
    else
    {
        log_cli("Queried successfully, sent %d bytes", sent_bytes);
    }

    /* Immediately wait for response. This is a blocking operation.
     *
     * Please note there is no libgapcom function for this. The libgapcom state
     * machine needs to know how to send bytes, but on the receive side we need
     * to fetch data ourselves before starting the state machine on it to decode
     * the message.
     * 
     * The core of the state machine, processing each byte in order, lies in
     * TinyFrame library. Our code on the receive side is actually only
     * TinyFrame callbacks.
     *
     * WARNING: a message not matching the binary format of libgapcom messages
     * will be silently ignored ! This is an unfortunate TinyFrame limitation.
     */
    recv_bytes = communicator_impl->recv(communicator_impl, (uint8_t*)recvbuf, sizeof(recvbuf));
    if (recv_bytes == -1)
    {
        log_cli("Failed to receive response; cleaning up");
        ret = GAPCLI_RECV_MSG_FAILED;
        goto close_communicator;
    }
    /* Parse the given response and trigger the callback, if any. */
    gapcom_accept(handle, (const uint8_t*)recvbuf, (size_t)recv_bytes);

close_communicator:
    communicator_impl->close(communicator_impl);
destroy_cmdline:
    gapcli_destroy_cmdline(cmd);
destroy_handle:
    gapcom_destroy(handle);
end:
    return ret;
}
