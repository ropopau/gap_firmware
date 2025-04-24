#include <Windows.h>

/* Building with MinGW allows us to expect POSIX headers to be there. */
#include <stdio.h>

#include "communicator.h"
#include "communicator_impl_client.h"
#include "log.h"



/* Forward declarations */
typedef struct communicator_impl_uart_client_t communicator_impl_uart_client_t;
static int communicator_impl_uart_open_client(communicator_impl_uart_client_t* self,
    const char* com_port);
static int communicator_impl_uart_close_client(communicator_impl_uart_client_t* self);
static ssize_t communicator_impl_uart_send_client(communicator_impl_uart_client_t* self,
    const uint8_t* buf, size_t len);
static ssize_t communicator_impl_uart_recv_client(communicator_impl_uart_client_t* self,
    uint8_t* buf, size_t len);

typedef struct communicator_impl_uart_client_t
{
    /* communicator_t interface */
    int (*open)(communicator_impl_uart_client_t* self, const char* com_port);
    int (*close)(communicator_impl_uart_client_t* self);
    ssize_t (*send)(communicator_impl_uart_client_t* self, const uint8_t* buf, size_t len);
    ssize_t (*recv)(communicator_impl_uart_client_t* self, uint8_t* buf, size_t len);
    /* File handle for COM port */
    HANDLE h_com;
} communicator_impl_uart_client_t;

static communicator_impl_uart_client_t communicator_impl_uart_client = {
    .open = communicator_impl_uart_open_client,
    .close = communicator_impl_uart_close_client,
    .send = communicator_impl_uart_send_client,
    .recv = communicator_impl_uart_recv_client,
};

/*************************
 *** Private functions ***
 *************************/

/**
 * @brief Open a COM port and configure it as a GAP control port. The COM port
 * will be purged of any in-flight bytes before the end of initialization.
 * 
 * @param self Pointer to communicator interface implementation (self)
 * @param com_port Name of COM port, e.g. "COM3"
 */
int communicator_impl_uart_open_client(communicator_impl_uart_client_t* self, const char* com_port)
{
    /* Indicate failure by default */
    int ret = -1;
    HANDLE h_com;
    DCB dcb = { 0 };
    COMMTIMEOUTS timeouts = { 0 };

    h_com = CreateFile(
        (TCHAR *)com_port,
        GENERIC_READ | GENERIC_WRITE,
        0, /* no sharing */
        NULL, /* no security */
        OPEN_EXISTING,
        0, /* non-overlapped I/O */
        NULL /* always NULL for COM devices */);
    if (h_com == INVALID_HANDLE_VALUE)
    {
        log_cli("Failed to open COM port %s", com_port);
        goto end;
    }

    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(h_com, &dcb))
    {
        log_cli("Failed to retrieve parameters of COM port %s", com_port);
        goto close_handle;
    }
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = EVENPARITY;
    dcb.fParity = TRUE;
    if (!SetCommState(h_com, &dcb))
    {
        log_cli("Failed to set parameters of COM port %s", com_port);
        goto close_handle;
    }

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 50;
    if (!SetCommTimeouts(h_com, &timeouts))
    {
        log_cli("Failed to set timeouts of COM port %s", com_port);
        goto close_handle;
    }

    if (!SetCommMask(h_com, EV_RXCHAR))
    {
        log_cli("Failed to configure listening for received bytes on COM port %s", com_port);
        goto close_handle;
    }

    if (!PurgeComm(h_com, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR))
    {
        log_cli("Failed to purge TX + RX on COM port %s; communication may not work but continuing", com_port);
    }

    self->h_com = h_com;
    ret = 0;
    goto end;

close_handle:
    CloseHandle(h_com);
end:
    return ret;
}

int communicator_impl_uart_close_client(communicator_impl_uart_client_t* self)
{
    if (self->h_com) {
        CloseHandle(self->h_com);
    }
    return 0;
}

/**
 * @brief Send gapcom frame through COM port.
 *
 * @param self Pointer to communicator interface implementation (self)
 * @param buf Data buffer to send
 * @param len Number of bytes to send
 * @return ssize_t Number of bytes sent
 */
ssize_t communicator_impl_uart_send_client(communicator_impl_uart_client_t* self,
    const uint8_t* buf, size_t len)
{
    DWORD len_written = 0;

    if (self->h_com == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    if (!WriteFile(self->h_com, (LPCVOID)buf, (DWORD)len, &len_written, NULL /* no overlapping */))
    {
        return (ssize_t)-1;
    }
    return (ssize_t)len_written;
}

/**
 * @brief Receive gapcom frame through COM port. This function will not try
 * again if the expected number of bytes is not met immediately.
 * 
 * @param self Pointer to communicator interface implementation (self)
 * @param buf Data buffer to fill
 * @param len Number of bytes to read. This parameter is actually ignored as
 * the expected number of bytes can be computed ourselves
 */
ssize_t communicator_impl_uart_recv_client(
    communicator_impl_uart_client_t* self,
    uint8_t* buf,
    size_t len)
{
    (void)len; /* Unused */
    DWORD expected_bytes;
    DWORD data_len;
    DWORD bytes_read;
    DWORD total_bytes_read = 0;

    if (self->h_com == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    /* Consume (1 byte), ID (4 bytes), Length (2 bytes)*/
    expected_bytes = 7;
    if (!ReadFile(self->h_com, (LPVOID)buf, expected_bytes, &bytes_read, NULL))
    {
        log_cli("Failed to read SOF, ID and Length. Exiting (error=%lu)", GetLastError());
        goto end;
    }

    total_bytes_read += bytes_read;
    data_len = (buf[5] << 8) | buf[6];

    /* Consume type, header CRC, data, and footer CRC : 2 + 2 + length + (2 if length > 0) bytes
     *
     * This part may ask to read for quite a lot of data, so we collect bytes in a loop.
     */
    if (data_len > 0)
        expected_bytes = 2 + 2 + data_len + 2;
    else
        expected_bytes = 2 + 2;
    if (!ReadFile(self->h_com, (LPVOID)buf + total_bytes_read, expected_bytes, &bytes_read, NULL))
    {
        log_cli("Failed to read Type, Header CRC (, Data, Footer CRC). Exiting (error=%lu)", GetLastError());
        goto end;
    }

    total_bytes_read += bytes_read;

end:
    return (ssize_t)total_bytes_read;
}

/************************
 *** Public functions ***
 ************************/

communicator_t* communicator_impl_client_get()
{
    return (communicator_t*)(&communicator_impl_uart_client);
}
