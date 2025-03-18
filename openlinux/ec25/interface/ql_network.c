#include <sys/wait.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

#include "ql_oe.h"
#include "ql_network.h"


static NW_Status_CB cb_nw_sts_ind = NULL;
static boolean bThrdContinued = TRUE;
static int nCounter = 0;

static void callback_onTimer(int param);
static void* thrd_nw_detect(void* arg);

int Ql_NW_EnableMobileAP(int timeout, NW_Status_CB nw_status_cb)
{
    pid_t status;
    pthread_t thrd_nw;
    //void* thrdRet;

    nCounter = timeout;
    cb_nw_sts_ind = nw_status_cb;
    
    status = system("start-stop-daemon -S -b -a Quectel_QCMAP_CLI &");
    if (-1 == status)
    {
        printf("system error!");
    } else {
        printf("exit status value = [0x%x]\n", status);

        if (WIFEXITED(status))
        {
            if (0 == WEXITSTATUS(status))
            {
                printf("Starting MobileAP, please waiting...\n");

                // Start a thread to detect linux network status
                if (pthread_create(&thrd_nw, NULL, thrd_nw_detect, NULL) != 0)
                {
                    printf("Fail to create thread!\n");
                    return -1;
                }
/*
                // Wait for the end of thread, and put the return value in "tret"
                if (pthread_join(thrd_nw, &thrdRet) != 0)
                {
                    printf("Join thrd_nw error!\n");
                    return -3;
                }
                printf("thrd_nw exit code: %d\n", *((int *)thrdRet));
*/                
            } else {
                printf("Fail to start Quectel_QCMAP_CLI, error code: %d\n", WEXITSTATUS(status));
                return -2;
            }
        } else {
            printf("exit status = [%d]\n", WEXITSTATUS(status));
        }
    }
    return 0;
}

static void callback_onTimer(int param)
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    char host[NI_MAXHOST];
    boolean bIpFound = 0;

    nCounter--;
    
    if (getifaddrs(&ifaddr) == -1)
    {
       perror("getifaddrs");
       return;
    }

    /* Walk through linked list, maintaining head pointer so we can free list later */
    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++)
    {
        if (ifa->ifa_addr == NULL)
           continue;

        family = ifa->ifa_addr->sa_family;

        /* Display interface name and family (including symbolic
        * form of the latter for the common families)
        */
        printf("%-8s %s (%d)\n",
              ifa->ifa_name,
              (family == AF_PACKET) ? "AF_PACKET" :
              (family == AF_INET) ? "AF_INET" :
              (family == AF_INET6) ? "AF_INET6" : "???",
              family);

        /* Find the "rmnet_data0" network adapter */
        if (strncmp(ifa->ifa_name, "rmnet_data0", 11) == 0)
        {
            bIpFound = 1;
            break;
        }
    }

    if (bIpFound)
    {
        s = getnameinfo(ifa->ifa_addr,
               (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                     sizeof(struct sockaddr_in6),
               host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (s != 0)
        {
           printf("getnameinfo() failed: %s\n", gai_strerror(s));
        }
        printf("< Retrived IP, address:%s >\n", host);
        if (cb_nw_sts_ind)
        {
            cb_nw_sts_ind(1);
            bThrdContinued = FALSE;
        }
    } else {
        printf("< Not find IP, nCounter:%d >\n", nCounter);
        if (nCounter > 0)
        {
            alarm(1);
        } else {
            printf("< timeout >\n");
            cb_nw_sts_ind(0);
            bThrdContinued = FALSE;
        }
    }
   freeifaddrs(ifaddr);
      
#if 0
    pid_t status = system("ifconfig | grep rmnet_data0 &");
    nCounter--;
    
    if (-1 == status)
    {
        printf("system error!");
    } else {
        printf("exit status value = [0x%x]\n", status);

        if (WIFEXITED(status))
        {
            if (0 == WEXITSTATUS(status))
            {
                printf("< Retrived IP, cb_nw_sts_ind=%X >\n", (uint32_t)cb_nw_sts_ind);
                if (cb_nw_sts_ind)
                {
                    cb_nw_sts_ind(1);

                    bThrdContinued = FALSE;
                }
            } else {
                printf("Not find IP, shell error code: %d\n", WEXITSTATUS(status));
                if (nCounter > 0)
                {
                    alarm(1);
                } else {
                    printf("< timeout >\n");
                    cb_nw_sts_ind(0);
                    bThrdContinued = FALSE;
                }
            }
        } else {
            printf("exit status = [%d]\n", WEXITSTATUS(status));
        }
    }
#endif
}

static void* thrd_nw_detect(void* arg)
{
    signal(SIGALRM, callback_onTimer);
    alarm(1);

    while (bThrdContinued)
    {
    }

    pthread_exit((void *)1);  // Explicitly exit the thread, and return (void *)1
}
