#ifndef SOCKET_H_
#define SOCKET_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>

    typedef unsigned char u8_t;
    typedef signed char s8_t;
    typedef unsigned short u16_t;
    typedef signed short s16_t;
    typedef unsigned long u32_t;
    typedef signed long s32_t;

#if !defined(socklen_t)
    typedef uint32_t socklen_t; /* 32 */
#endif

#if !defined(sa_family_t) && !defined(SA_FAMILY_T_DEFINED)
    typedef u8_t sa_family_t;
#endif

#if !defined(in_port_t) && !defined(IN_PORT_T_DEFINED)
    typedef u16_t in_port_t;
#endif

#if !defined(in_addr_t) && !defined(IN_ADDR_T_DEFINED)
    typedef u32_t in_addr_t;
#endif

    struct in_addr
    {
        in_addr_t s_addr;
    };

    struct sockaddr_in
    {
        u8_t sin_len;
        sa_family_t sin_family;
        in_port_t sin_port;
        struct in_addr sin_addr;
#define SIN_ZERO_LEN 8
        char sin_zero[SIN_ZERO_LEN];
    };

    struct sockaddr
    {
        u8_t sa_len;
        sa_family_t sa_family;
        char sa_data[14];
    };

    typedef struct
    {
        union {
            u32_t u32_addr[4];
            u8_t u8_addr[16];
        } un;
    } in6_addr;

    struct sockaddr_in6
    {
        u8_t sin6_len;
        sa_family_t sin6_family;
        in_port_t sin6_port;
        u32_t sin6_flowinfo;
        in6_addr sin6_addr;
        u32_t sin6_scope_id;
    };

    struct sockaddr_storage
    {
        u8_t s2_len;
        sa_family_t ss_family;
        char s2_data1[2];
        u32_t s2_data2[3];
        u32_t s2_data3[3];
    };    
    
    struct hostent
    {
        char *h_name;         /* Official name of the host. */
        char **h_aliases;     /* A pointer to an array of pointers to alternative host names, terminated by a null pointer. */
        int h_addrtype;       /* Address type. */
        int h_length;         /* The length, in bytes, of the address. */
        char **h_addr_list;   /* A pointer to an array of pointers to network addresses (in network byte order) for the host, terminated by a null pointer. */
#define h_addr h_addr_list[0] /* for backward compatibility */
    };

    struct addrinfo
    {
        int ai_flags;             /* Input flags. */
        int ai_family;            /* Address family of socket. */
        int ai_socktype;          /* Socket type. */
        int ai_protocol;          /* Protocol of socket. */
        socklen_t ai_addrlen;     /* Length of socket address. */
        struct sockaddr *ai_addr; /* Socket address of socket. */
        char *ai_canonname;       /* Canonical name of service location. */
        struct addrinfo *ai_next; /* Pointer to next in list. */
    };

    struct ip4_addr
    {
        u32_t addr;
    };
    typedef struct ip4_addr ip4_addr_t;

    struct ip6_addr
    {
        u32_t addr[4];
    };
    typedef struct ip6_addr ip6_addr_t;

    typedef struct _ip_addr
    {
        union {
            ip6_addr_t ip6;
            ip4_addr_t ip4;
        } u_addr;
        u8_t type;
    } ip_addr_t __attribute__((aligned(4)));

    /***************************************************************************************************/

#define ERR_OK 0          /* No error, everything OK. */
#define ERR_MEM -1        /* Out of memory error.     */
#define ERR_BUF -2        /* Buffer error.            */
#define ERR_TIMEOUT -3    /* Timeout.                 */
#define ERR_RTE -4        /* Routing problem.         */
#define ERR_INPROGRESS -5 /* Operation in progress    */
#define ERR_VAL -6        /* Illegal value.           */
#define ERR_WOULDBLOCK -7 /* Operation would block.   */
#define ERR_USE -8        /* Address in use.          */
#define ERR_ALREADY -9    /* Already connecting.      */
#define ERR_ISCONN -10    /* Conn already established.*/
#define ERR_CONN -11      /* Not connected.           */
#define ERR_ABRT -12      /* Connection aborted.      */
#define ERR_RST -13       /* Connection reset.        */
#define ERR_CLSD -14      /* Connection closed.       */
#define ERR_ARG -15       /* Illegal argument.        */
#define ERR_IF -16        /* Low-level netif error    */
#define ERR_IS_FATAL(e) ((e) < ERR_CONN)

    /***************************************************************************************************/

#define IPADDR_ANY ((u32_t)0x00000000UL)
#define INADDR_ANY IPADDR_ANY
#define IPADDR_BROADCAST ((u32_t)0xffffffffUL)
#define IPADDR_NONE ((u32_t)0xffffffffUL)

#define AF_INET 2

#define IPPROTO_IP 0
#define IPPROTO_ICMP 1
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17

#define SOL_SOCKET 0xfff /* options for socket level */

/* Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3

/*  Option flags per-socket. These must match the SOF_ flags in ip.h (checked in init.c) */
#define SO_REUSEADDR 0x0004 /* Allow local address reuse */
#define SO_KEEPALIVE 0x0008 /* keep connections alive */
#define SO_BROADCAST 0x0020 /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */

/* Additional options, not kept in so_options. */
#define SO_DEBUG 0x0001       /* Unimplemented: turn on debugging info recording */
#define SO_ACCEPTCONN 0x0002  /* socket has had listen() */
#define SO_DONTROUTE 0x0010   /* Unimplemented: just use interface addresses */
#define SO_USELOOPBACK 0x0040 /* Unimplemented: bypass hardware when possible */
#define SO_LINGER 0x0080      /* linger on close if data present */
#define SO_DONTLINGER ((int)(~SO_LINGER))
#define SO_OOBINLINE 0x0100    /* Unimplemented: leave received OOB data in line */
#define SO_REUSEPORT 0x0200    /* Unimplemented: allow local address & port reuse */
#define SO_SNDBUF 0x1001       /* Unimplemented: send buffer size */
#define SO_RCVBUF 0x1002       /* receive buffer size */
#define SO_SNDLOWAT 0x1003     /* Unimplemented: send low-water mark */
#define SO_RCVLOWAT 0x1004     /* Unimplemented: receive low-water mark */
#define SO_SNDTIMEO 0x1005     /* send timeout */
#define SO_RCVTIMEO 0x1006     /* receive timeout */
#define SO_ERROR 0x1007        /* get error status and clear */
#define SO_TYPE 0x1008         /* get socket type */
#define SO_CONTIMEO 0x1009     /* Unimplemented: connect timeout */
#define SO_NO_CHECK 0x100a     /* don't create UDP checksum */
#define SO_BINDTODEVICE 0x100b /* bind to device */

/* Options for level IPPROTO_TCP */
#define TCP_NODELAY 0x01   /* don't delay send to coalesce packets */
#define TCP_KEEPALIVE 0x02 /* send KEEPALIVE probes when idle for pcb->keep_idle milliseconds */
#define TCP_KEEPIDLE 0x03  /* set pcb->keep_idle  - Same as TCP_KEEPALIVE, but use seconds for get/setsockopt */
#define TCP_KEEPINTVL 0x04 /* set pcb->keep_intvl - Use seconds for get/setsockopt */
#define TCP_KEEPCNT 0x05   /* set pcb->keep_cnt   - Use number of probes sent for get/setsockopt */

/* commands for fnctl */
#ifndef F_GETFL
#define F_GETFL 3
#endif
#ifndef F_SETFL
#define F_SETFL 4
#endif

/* File status flags and file access modes for fnctl, these are bits in an int. */
#ifndef O_NONBLOCK
#define O_NONBLOCK 1 /* nonblocking I/O */
#endif

#ifndef O_NDELAY
#define O_NDELAY 1 /* same as O_NONBLOCK, for compatibility */
#endif

#ifndef SHUT_RD
#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2
#endif

/* Flags we can use with send and recv. */
#define MSG_PEEK 0x01     /* Peeks at an incoming message */
#define MSG_WAITALL 0x02  /* Unimplemented: Requests that the function block until the full amount of data requested can be returned */
#define MSG_OOB 0x04      /* Unimplemented: Requests out-of-band data. The significance and semantics of out-of-band data are protocol-specific */
#define MSG_DONTWAIT 0x08 /* Nonblocking i/o for this operation only */
#define MSG_MORE 0x10     /* Sender will send more */
#define MSG_TRUNC 0x20    /* Message truncate */

#define HTONS(x) ((((x)&0xff) << 8) | (((x)&0xff00) >> 8))

#define NTOHS(x) HTONS(x)

#define HTONL(x) ((((x)&0xff) << 24) |      \
                  (((x)&0xff00) << 8) |     \
                  (((x)&0xff0000UL) >> 8) | \
                  (((x)&0xff000000UL) >> 24))

#define NTOHL(x) HTONL(x)

#define htons(x) HTONS(x)

#define htonl(x) HTONL(x)

#undef fcntl

    int socket(int domain, int type, int protocol);
    int connect(int s, const struct sockaddr *name, socklen_t namelen);
    int closesocket(int s);
    int shutdown(int s, int how);
    int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
    int bind(int s, const struct sockaddr *name, socklen_t namelen);
    int listen(int s, int backlog);

    int recv(int s, void *mem, size_t len, int flags);
    int recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
    int send(int s, const void *dataptr, size_t size, int flags);
    int sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
    int fcntl(int, int, ...);
    void freeaddrinfo(struct addrinfo *ai);
    int getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res);

    int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
    int getsockopt(int, int, int, void *, socklen_t *); // getsockopt NOT EXIST

    int ip4addr_aton(const char *cp, ip4_addr_t *addr);
    u32_t ipaddr_addr(const char *cp);

#ifdef __cplusplus
}
#endif
#endif /* SOCKET_H_ */
