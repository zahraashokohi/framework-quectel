/*
 * Dns.h
 *
 *  Created on: 28.06.2018
 *      Author: georgi.angelov
 */

#ifndef DNS_H_
#define DNS_H_

#include <interface.h>
#include <IPAddress.h>
#include <WString.h>

#define DEBUG_DNS /*DBG*/

#define IFACE_DEFAULT_NAME "rmnet_data0"

typedef enum
{
    DNS_ANY_SERVER = QAPI_NET_DNS_ANY_SERVER_ID,
    DNS_V4_PRIMARY = QAPI_NET_DNS_V4_PRIMARY_SERVER_ID,
    DNS_V4_SECONDARY = QAPI_NET_DNS_V4_SECONDARY_SERVER_ID,
    DNS_V6_PRIMARY = QAPI_NET_DNS_V6_PRIMARY_SERVER_ID,
    DNS_V6_SECONDARY = QAPI_NET_DNS_V6_SECONDARY_SERVER_ID,
} dns_server_e;

class Dns
{
public:
    static void start()
    {
        if (false == qapi_Net_DNSc_Is_Started())
        {
            int r = qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);
            DEBUG_DNS("[DNS] start = %d\n", r);
        }
    }

    static void stop()
    {
        if (true == qapi_Net_DNSc_Is_Started())
        {
            int r = qapi_Net_DNSc_Command(QAPI_NET_DNS_STOP_E);
            DEBUG_DNS("[DNS] stop = %d\n", r);
        }
    }

    static bool query(const char *host, struct ip46addr *addr, const char *iface = IFACE_DEFAULT_NAME)
    {
        if (NULL == host || NULL == addr || NULL == iface)
            return false;
        memset(addr, 0, sizeof(ip46addr));
        addr->type = AF_INET;
        uint32_t ip = (uint32_t)inet_addr(host);
        if (0 == ip)
        {
            DEBUG_DNS("[DNS] query ( %s ) '%s'\n", iface, host);
            int r = qapi_Net_DNSc_Reshost_on_iface(host, addr, iface);
            if (r)
            {
                DEBUG_DNS("[ERROR] qapi_Net_DNSc_Reshost_on_iface() %d\n", r);
            }
        }
        else
        {
            addr->a.addr4 = ip;
        }
        DEBUG_DNS("[DNS] IP = %d.%d.%d.%d\n",
                  addr->a.addr4 & 0xFF,
                  (addr->a.addr4 >> 8) & 0xFF,
                  (addr->a.addr4 >> 16) & 0xFF,
                  (addr->a.addr4 >> 24) & 0xFF);
        return true;
    }

    static bool query(String host, IPAddress &ip, const char *iface = IFACE_DEFAULT_NAME)
    {
        struct ip46addr addr;
        if (query(host.c_str(), &addr, iface))
            ip = (uint32_t)addr.a.addr4;
        else
            ip = (uint32_t)0;
        return ip > 0;
    }

    static bool remove(dns_server_e who, const char *iface = IFACE_DEFAULT_NAME)
    {
        if (!iface)
            return false;
        int r = qapi_Net_DNSc_Del_Server_on_iface(who, iface);
        DEBUG_DNS("[DNS] remove %d { %s } : %d\n", who, iface, r);
        return r == 0;
    }

    static bool add(dns_server_e who, const char *addr, const char *iface = IFACE_DEFAULT_NAME)
    {
        if (!addr || !iface)
            return false;
        if (0 == inet_addr(addr)) // test for ip x.x.x.x
            return false;
        int r = qapi_Net_DNSc_Add_Server_on_iface(addr, who, iface);
        DEBUG_DNS("[DNS] add %d { %s @ %s } : %d\n", who, addr, iface, r);
        return r == 0;
    }

    static bool set(const char *primary, const char *secondary, const char *iface = IFACE_DEFAULT_NAME)
    {
        if (primary)
        {
            remove(DNS_V4_PRIMARY, iface);
            if (false == add(DNS_V4_PRIMARY, primary, iface))
                return false;
        }
        if (secondary)
        {
            remove(DNS_V4_SECONDARY, iface);
            return add(DNS_V4_SECONDARY, secondary, iface);
        }
        return false;
    }
};

#endif /* DNS_H_ */
