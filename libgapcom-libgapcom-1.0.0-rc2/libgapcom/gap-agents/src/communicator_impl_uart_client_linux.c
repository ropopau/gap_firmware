#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "communicator.h"
#include "communicator_impl_client.h"
#include "log.h"



/***************************
 *** Private definitions ***
 ***************************/

#define UART_READ_TRIES (5)

#define UART_RETRY_NAP_DURATION_USEC (1000 * 1000)

/* Forward declarations */
typedef struct communicator_impl_uart_client_t communicator_impl_uart_client_t;
static int communicator_impl_uart_open_client(communicator_impl_uart_client_t* self,
    const char* path);
static int communicator_impl_uart_close_client(communicator_impl_uart_client_t* self);
static ssize_t communicator_impl_uart_send_client(communicator_impl_uart_client_t* self,
    const uint8_t* buf, size_t len);
static ssize_t communicator_impl_uart_recv_client(communicator_impl_uart_client_t* self,
    uint8_t* buf, size_t len);

typedef struct communicator_impl_uart_client_t
{
    /* communicator_t interface */
    int (*open)(communicator_impl_uart_client_t* self, const char* path);
    int (*close)(communicator_impl_uart_client_t* self);
    ssize_t (*send)(communicator_impl_uart_client_t* self, const uint8_t* buf, size_t len);
    ssize_t (*recv)(communicator_impl_uart_client_t* self, uint8_t* buf, size_t len);
    /* File descriptor for node in /dev */
    int fd;
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

static ssize_t read_nonblock_with_tries(int fd, uint8_t* buf, size_t count, size_t tries, int32_t delay)
{
    int ret;
    size_t initial_count = count;
    ssize_t bytes_read = 0;

    do
    {
        ret = read(fd, buf + bytes_read, count);
        if (ret == -1)
        {
            if (errno == EAGAIN)
            {
                /* No bytes readable right now, but as termios is not configured
                * in canonical mode this does not mean there are really no bytes
                * left: read() returns -1 and EAGAIN instead of returning 0.
                */
                log_cli("No bytes read on UART; sleeping and trying again");
                --tries;
            }
            else
            {
                /* Unhandled error, abort and let caller consult errno */
                bytes_read = -1;
                break;
            }
        }
        else /* ret >= 0 */
        {
            bytes_read += (ssize_t)ret;
            count -= (size_t)ret;
            --tries;
        }

        if ((size_t)bytes_read < initial_count)
            usleep(delay);
    } while ((size_t)bytes_read < initial_count && tries > 0);

    if ((size_t)bytes_read < initial_count && tries == 0)
        log_cli("Still expected %zu bytes but no tries left; stopping", count - bytes_read);

    return bytes_read;
}

/**
 * @brief Open a a pseudo-TTY device and configure it as a GAP control port.
 * The pseudo-TTY (actually an UART) will be purged of any in-flight bytes
 * before the end of initialization.
 * 
 * @param self Pointer to communicator interface implementation (self)
 * @param path Path to pseudo-TTY node in devfs, e.g. /dev/ttyACM0
*/
int communicator_impl_uart_open_client(communicator_impl_uart_client_t* self,
    const char* path)
{
    int ret;
    int fd;
    struct termios termios;
    const speed_t speed = B9600; /* Bauds */

    ret = 0;

    fd = open(path, O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        log_cli("Failed to open com path %s", path);
        ret = -1;
        goto end;
    }
    if (!isatty(fd))
    {
        log_cli("Com path %s is not a serial device", path);
        ret = -1;
        goto close_fd;
    }
    /* Set exclusive access to serial port */
    if (ioctl(fd, TIOCEXCL, NULL) < 0)
        log_cli("Failed to set exclusive access to serial device; ignoring");
    self->fd = fd;

    /* Get serial port characteristics */
    ret = tcgetattr(fd, &termios);
    if (ret == -1)
    {
        log_cli("Failed to get serial port characteristics (errno=%s)", strerror(errno));
        goto close_fd;
    }

    termios.c_iflag=INPCK;
    termios.c_oflag=0;
    termios.c_cflag=CS8|CREAD|CLOCAL|PARENB;
    termios.c_lflag=0;
    termios.c_cc[VMIN]=200;
    termios.c_cc[VTIME]=200;

    /* Set serial port characteristics */
    ret = cfsetospeed(&termios, speed);
    if (ret == -1)
    {
        log_cli("Failed to configure baudrate (output) (errno=%s)", strerror(errno));
        goto close_fd;
    }
    ret = cfsetispeed(&termios, speed);
    if (ret == -1)
    {
        log_cli("Failed to configure baudrate (input) (errno=%s)", strerror(errno));
        goto close_fd;
    }
    ret = tcsetattr(fd, TCSANOW, &termios);
    if (ret == -1)
    {
        log_cli("Failed to set serial port characteristics (errno=%s)", strerror(errno));
        goto close_fd;
    }
    /* Wait for all in-flight I/O operations (as seen by Linux) to complete */
    ret = tcdrain(fd);
    if (ret == -1)
    {
        log_cli("Failed to drain buffers (errno=%s)", strerror(errno));
        goto close_fd;
    }
    /* Drop any remaining bytes in the USB-serial buffers. This poses no
     * problem as we want to start anew. Otherwise, the first bytes will
     * be compared to an expected Start of Frame and possibly corrupt the
     * state machine.
     */
    ret = tcflush(fd, TCIOFLUSH);
    if (ret == -1)
    {
        log_cli("Failed to flush input buffer (received but not read yet) and output buffer (written but not send yet) (errno=%s); carrying on",
            strerror(errno));
        ret = 0;
    }

    goto end;

close_fd:
    close(fd);
end:
    return ret;
}

int communicator_impl_uart_close_client(communicator_impl_uart_client_t* self)
{
    if (self->fd > 0)
    {
        close(self->fd);
    }
    return 0;
}

/**
 * @brief Send gapcom frame through UART (serial port).
 *
 * @param self Pointer to communicator interface implementation (self)
 * @param buf Data buffer to send
 * @param len Number of bytes to send
 * @return ssize_t Number of bytes sent
 */
ssize_t communicator_impl_uart_send_client(communicator_impl_uart_client_t* self,
    const uint8_t* buf, size_t len)
{
    ssize_t sent_bytes;

    if (self->fd <= 0)
        return 0;

    sent_bytes = (ssize_t)write(self->fd, buf, len);
    if (sent_bytes == -1)
        return sent_bytes;
    tcdrain(self->fd);
    return sent_bytes;
}

/**
 * @brief Receive gapcom frame through UART (serial port). This function
 * will try again if not enough bytes are met.
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
    (void)len; /* Ignored */
    ssize_t recv_bytes = 0;
    ssize_t retsz;
    size_t data_len;
    size_t expected_bytes;

    if (self->fd <= 0)
        return 0;

    /* Read and check SoF (1 byte), ID (4 bytes), Length (2 bytes)*/
    expected_bytes = 7;
    retsz = read_nonblock_with_tries(self->fd, buf, expected_bytes, UART_READ_TRIES, 0);
    if (retsz == -1)
    {
        log_cli("Failed to read SOF, ID and Length. Exiting.(errno=%s)",
        strerror(errno));
        goto end;
    }
    else if ((size_t)retsz < expected_bytes)
    {
        log_cli("Failed to read SOF, ID and Length. (missing bytes after multiple tries)");
        goto end;
    }

    recv_bytes += expected_bytes;
    data_len = (buf[recv_bytes - 2] << 8) | buf[recv_bytes - 1];

    /* Skip type, header CRC, data, and footer CRC : 2 + 2 + length + (2 if length > 0) bytes
     *
     * This part may ask to read for quite a lot of data, so we collect bytes in a loop.
     */
    if (data_len > 0)
        expected_bytes = 2 + 2 + data_len + 2;
    else
        expected_bytes = 2 + 2;
    retsz = read_nonblock_with_tries(self->fd, buf + recv_bytes, expected_bytes, UART_READ_TRIES, UART_RETRY_NAP_DURATION_USEC);
    if (retsz == -1)
    {
        log_cli("Failed to read type, header CRC, data and footer CRC on serial port (errno=%s)",
            strerror(errno));
        goto end;
    }
    else if ((size_t)retsz < expected_bytes)
    {
        log_cli("Failed to read type, header CRC, data and footer CRC on serial port (missing bytes after multiple tries)");
        goto end;
    }
    recv_bytes += retsz;

end:
    return recv_bytes;
}

/************************
 *** Public functions ***
 ************************/

communicator_t* communicator_impl_client_get()
{
    return (communicator_t*)(&communicator_impl_uart_client);
}
