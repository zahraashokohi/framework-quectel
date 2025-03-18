#ifndef __QL_OE_H__
#define __QL_OE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <dirent.h>
#include <comdef.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <malloc.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "ql_error.h"
#include "ql_gpio.h"
#include "ql_eint.h"
#include "ql_i2c.h"
#include "ql_audio.h"
#include "ql_uart.h"
#include "ql_network.h"

//#include "gpioSysfs.h"

#ifndef POLLRDHUP
#define POLLRDHUP       0x2000
#endif


char * WIZ_SendAT(int smd_fd, const char * atCmd, const char * finalRsp, long timeout_ms);

#ifdef __cplusplus
}
#endif

#endif	//__QL_OE_H__
