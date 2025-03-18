/*
    Created on: 29.06.2018
    Author: Georgi Angelov
        http://www.wizio.eu/
        https://github.com/Wiz-IO    

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA   
 */

#ifndef DSS_H_
#define DSS_H_

#include <interface.h>
#include <IPAddress.h>
#include <WString.h>
#include <Device.h>

typedef void (*dss_full_event)(qapi_DSS_Hndl_t hndl, void *user, qapi_DSS_Net_Evt_t evt, qapi_DSS_Evt_Payload_t *payload_ptr);
typedef void (*dss_void_event)(void);

typedef enum
{
	RADIO_UNKNOWN = QAPI_DSS_RADIO_TECH_UNKNOWN, /**< Technology Unknown. */
	RADIO_MIN = QAPI_DSS_RADIO_TECH_MIN,		 /**< Start. */
	RADIO_UMTS = QAPI_DSS_RADIO_TECH_UMTS,		 /**< UMTS. */
	RADIO_CDMA = QAPI_DSS_RADIO_TECH_CDMA,		 /**< CDMA. */
	RADIO_1X = QAPI_DSS_RADIO_TECH_1X,			 /**< 1X. */
	RADIO_DO = QAPI_DSS_RADIO_TECH_DO,			 /**< DO. */
	RADIO_LTE = QAPI_DSS_RADIO_TECH_LTE,		 /**< LTE. */
	RADIO_DSCDMA = QAPI_DSS_RADIO_TECH_TDSCDMA,  /**< TDSCDMA. */
} dss_radio_e;

typedef enum
{
	DSS_WAIT,
	DSS_ACT,
	DSS_ACTED,
	DSS_DEACT,
	DSS_DEACTED,
} DSS_STATE;

class Dss
{
  private:
	int ctrl;
	int state;
	qapi_DSS_Hndl_t handle;
	TX_EVENT_FLAGS_GROUP *event;
	TX_THREAD *task;
	UCHAR *stack;

	bool _connected;
	bool _started;
	dss_void_event onConnect;
	dss_void_event onDisconnect;
	dss_full_event onEvent;
	qapi_DSS_Addr_Info_t addr_info;
	static void callback(qapi_DSS_Hndl_t hndl, void *user, qapi_DSS_Net_Evt_t evt, qapi_DSS_Evt_Payload_t *payload_ptr);
	void ctor();

	static void entry(void *p);

  public:
	Dss();
	Dss(dss_full_event on_event);
	Dss(dss_void_event on_connect, dss_void_event on_disconnect = NULL);
	~Dss();
	operator bool() { return connected(); }
	bool connected();
	bool set(qapi_DSS_Call_Param_Identifier_t param, qapi_DSS_Call_Param_Value_t *pInfo);
	bool set(qapi_DSS_Call_Param_Identifier_t param, char value);
	bool apn(char *name, char *user = NULL, char *pass = NULL);
	bool open();
	bool open(dss_radio_e radio);
	void close();
	bool act();
	bool act(bool blocked);
	bool deact(void);
	bool deact(bool blocked);
	bool send(uint8_t *data, uint32_t data_len, uint8_t ex_data);
	bool statistic(qapi_DSS_Data_Pkt_Stats_s *s, bool reset = false);

	uint32_t getMTU();
	bool getIfaceName(void *iface);

	uint32_t getIPcount();
	bool getAddress(uint32_t buffer_count);
	bool getAddressInfo(qapi_DSS_Addr_Info_t *p_addr_info, uint32_t buffer_count = 1);

	int getBearer();									// bearer
	bool getIFace(String &s);							// iface
	void get(IPAddress &primary, IPAddress &secondary); // dns
	void get(IPAddress &local, IPAddress &gateway, IPAddress &primary, IPAddress &secondary);

	void begin();
};

#endif /* DSS_H_ */
