////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef SELECT_H_
#define SELECT_H_
#ifdef __cplusplus
extern "C"
{
#endif

#define _SYS_SELECT_H

#include <sys/_timeval.h>

#ifndef FD_SET

#undef FD_SETSIZE

#define MEMP_NUM_NETCONN 32
#define FD_SETSIZE MEMP_NUM_NETCONN
#define SOCKET_OFFSET 0

#define FDSETSAFESET_VAL(n, p, code)                                                   \
    do                                                                                 \
    {                                                                                  \
        if (((n)-SOCKET_OFFSET < MEMP_NUM_NETCONN) && (((int)(n)-SOCKET_OFFSET) >= 0)) \
        {                                                                              \
            code;                                                                      \
        }                                                                              \
    } while (0)

#define FDSETSAFESET(n, p, code)          \
    do                                    \
    {                                     \
        if ((p) != NULL)                  \
        {                                 \
            FDSETSAFESET_VAL(n, p, code); \
        }                                 \
    } while (0)

#define FDSETSAFEGET(n, p, code) (((p) != NULL) && ((n)-SOCKET_OFFSET < MEMP_NUM_NETCONN) && (((int)(n)-SOCKET_OFFSET) >= 0) ? (code) : 0)
#define FD_SET(n, p) FDSETSAFESET(n, p, (p)->fd_bits[((n)-SOCKET_OFFSET) / 8] |= (1 << (((n)-SOCKET_OFFSET) & 7)))
#define FD_SET_VAL(n, p) FDSETSAFESET_VAL(n, &(p), (p).fd_bits[((n)-SOCKET_OFFSET) / 8] |= (1 << (((n)-SOCKET_OFFSET) & 7)))
#define FD_CLR(n, p) FDSETSAFESET(n, p, (p)->fd_bits[((n)-SOCKET_OFFSET) / 8] &= ~(1 << (((n)-SOCKET_OFFSET) & 7)))
#define FD_ISSET(n, p) FDSETSAFEGET(n, p, (p)->fd_bits[((n)-SOCKET_OFFSET) / 8] & (1 << (((n)-SOCKET_OFFSET) & 7)))
#define FD_ZERO(p) memset((void *)(p), 0, sizeof(*(p)))

#endif

#undef fd_set

    typedef struct fd_set
    {
        unsigned char fd_bits[(FD_SETSIZE + 7) / 8];
    } fd_set;

    int select(int mn, fd_set *rd, fd_set *wr, fd_set *er, struct timeval *timeout);

#ifdef __cplusplus
}
#endif
#endif /* SELECT_H_ */