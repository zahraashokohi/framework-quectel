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

#include "Dss.h"

#define DEBUG_DSS
//DBG

void Dss::callback(qapi_DSS_Hndl_t hndl, void *user, qapi_DSS_Net_Evt_t evt, qapi_DSS_Evt_Payload_t *payload_ptr)
{
	if (!user)
		return;
	Dss *THIS = (Dss *)user;
	if (THIS->onEvent)
	{
		THIS->onEvent(hndl, user, evt, payload_ptr);
		//DEBUG_DSS("[DSS-CB] onEvent\n");
	}
	else
	{
		switch (evt)
		{
		case QAPI_DSS_EVT_NET_IS_CONN_E:
			//DEBUG_DSS("[DSS-CB] CONNECTED\n");
			if (THIS->onConnect)
				THIS->onConnect();
			THIS->_connected = true;
			THIS->state = DSS_ACTED;
			break;
		case QAPI_DSS_EVT_NET_NO_NET_E:
			//DEBUG_DSS("[DSS-CB] DISCONNECTED\n");
			THIS->_connected = false;
			THIS->_started = false;
			THIS->state = DSS_DEACTED;
			if (THIS->onDisconnect)
				THIS->onDisconnect();
			break;
		default:
			//DEBUG_DSS("[DSS-CB] evt: %d\n", evt);
			break;
		}
	}
	tx_event_flags_set(THIS->event, 1 << evt, TX_OR);
}

void Dss::ctor()
{
	static int once = 0;
	if (0 == once)
	{
		int r = txm_module_object_allocate(&event, sizeof(TX_EVENT_FLAGS_GROUP));
		if (r)
		{
			DEBUG_DSS("[ERROR] DSS EVENT txm_module_object_allocate: %d\n", r);
			abort();
		}
		r = tx_event_flags_create(event, "dss-event");
		if (r)
		{
			DEBUG_DSS("[ERROR] DSS tx_event_flags_create: %d\n", r);
			abort();
		}
		ctrl = qapi_DSS_Init(QAPI_DSS_MODE_GENERAL);
		//DEBUG_DSS("[DSS] qapi_DSS_Init: %d\n", ctrl);
		once = 1;
	}
}

Dss::Dss()
{
	onEvent = NULL;
	onConnect = NULL;
	onDisconnect = NULL;
	handle = NULL;
}

Dss::Dss(dss_full_event on_event)
{
	onEvent = on_event;
	onConnect = NULL;
	onDisconnect = NULL;
	handle = NULL;
}

Dss::Dss(dss_void_event on_connect, dss_void_event on_disconnect)
{
	onEvent = NULL;
	onConnect = on_connect;
	onDisconnect = on_disconnect;
	handle = NULL;
}

Dss::~Dss()
{
	close();
	if (0 == ctrl)
		qapi_DSS_Release(QAPI_DSS_MODE_GENERAL);
	tx_event_flags_delete(event);
	txm_module_object_deallocate(event);
}

bool Dss::set(qapi_DSS_Call_Param_Identifier_t param, qapi_DSS_Call_Param_Value_t *pInfo)
{
	if (!handle || !pInfo)
		return false;
	return qapi_DSS_Set_Data_Call_Param(handle, param, pInfo) == 0;
}

bool Dss::set(qapi_DSS_Call_Param_Identifier_t param, char value)
{
	if (!value)
		return false;
	qapi_DSS_Call_Param_Value_t info;
	info.buf_val = NULL;
	info.num_val = value;
	return set(param, &info);
}

bool Dss::apn(char *name, char *user, char *pass)
{
	if (!handle)
		return false;
	qapi_DSS_Call_Param_Value_t info;
	if (name)
	{
		info.buf_val = name;
		info.num_val = strlen(name);
		if (qapi_DSS_Set_Data_Call_Param(handle, QAPI_DSS_CALL_INFO_APN_NAME_E, &info))
		{
			DEBUG_DSS("[ERROR] qapi_DSS_Set_Data_Call_Param 1\n");
			return false;
		}
	}
	if (user)
	{
		info.buf_val = user;
		info.num_val = strlen(user);
		if (qapi_DSS_Set_Data_Call_Param(handle, QAPI_DSS_CALL_INFO_USERNAME_E, &info))
		{
			DEBUG_DSS("[ERROR] qapi_DSS_Set_Data_Call_Param 2\n");
			return false;
		}
	}
	if (pass)
	{
		info.buf_val = pass;
		info.num_val = strlen(pass);
		if (qapi_DSS_Set_Data_Call_Param(handle, QAPI_DSS_CALL_INFO_PASSWORD_E, &info))
		{
			DEBUG_DSS("[ERROR] qapi_DSS_Set_Data_Call_Param 3\n");
			return false;
		}
	}
	return true;
}

bool Dss::open()
{
	if (handle) // allready
		return true;
	do
	{
		if (qapi_DSS_Get_Data_Srvc_Hndl(callback, this, &handle))
			qapi_Timer_Sleep(100, QAPI_TIMER_UNIT_MSEC, 1);
	} while (NULL == handle);
	//DEBUG_DSS("[DSS] OPEN: %X\n", handle);
	return true;
}

bool Dss::open(dss_radio_e radio)
{
	bool res = false;
	if (open())
	{
		set(QAPI_DSS_CALL_INFO_IP_VERSION_E, QAPI_DSS_IP_VERSION_4);
		set(QAPI_DSS_CALL_INFO_UMTS_PROFILE_IDX_E, 1);
		res = set(QAPI_DSS_CALL_INFO_TECH_PREF_E, radio);
	}
	return res;
}

void Dss::close()
{
	if (handle)
		qapi_DSS_Rel_Data_Srvc_Hndl(handle);
	handle = NULL;
	_connected = false;
	_started = false;
}

bool Dss::act()
{
	if (!handle || _started)
		return false;
	tx_event_flags_set(event, 0, TX_AND);
	_started = qapi_DSS_Start_Data_Call(handle) == 0;
	return _started;
}

bool Dss::act(bool blocked)
{
AGAIN:
	bool res = act();
	if (res && blocked)
	{
		ULONG sig = 0;
		ULONG mask = 1 << QAPI_DSS_EVT_NET_IS_CONN_E;
		tx_event_flags_get(event, -1, TX_OR, &sig, TX_WAIT_FOREVER);
		if (0 == (sig & mask))
		{
			qapi_Timer_Sleep(1000, QAPI_TIMER_UNIT_MSEC, 1);
			goto AGAIN;
		}
	}
	return res;
}

bool Dss::deact(void)
{
	if (!handle || !_started)
		return false;
	tx_event_flags_set(event, 0, TX_AND);
	return qapi_DSS_Stop_Data_Call(handle) == 0; // started flag wiil cleared in cb
}

bool Dss::deact(bool blocked)
{
AGAIN:
	bool res = deact();
	if (res && blocked)
	{
		ULONG sig = 0;
		ULONG mask = 1 << QAPI_DSS_EVT_NET_NO_NET_E;
		tx_event_flags_get(event, -1, TX_OR, &sig, TX_WAIT_FOREVER);
		//DEBUG_DSS("[DSS] STOP SIG: %X", sig);
		if (0 == (sig & mask))
		{
			qapi_Timer_Sleep(1000, QAPI_TIMER_UNIT_MSEC, 1);
			goto AGAIN;
		}
	}
	return res;
}

bool Dss::connected()
{
	return _connected;
}

bool Dss::statistic(qapi_DSS_Data_Pkt_Stats_s *s, bool reset)
{
	if (!handle || !s)
		return false;
	memset(&s, 0, sizeof(s));
	if (reset)
		return qapi_DSS_Reset_Pkt_Stats(handle) == 0;
	return qapi_DSS_Get_Pkt_Stats(handle, s) == 0;
}

bool Dss::getIfaceName(void *iface)
{
	if (!handle || !iface)
		return false;
	return qapi_DSS_Get_Device_Name(handle, (char *)iface, 15) == 0;
}

uint32_t Dss::getIPcount()
{
	uint32_t count = 0;
	if (handle)
		if (qapi_DSS_Get_IP_Addr_Count(handle, &count))
			return 0;
	return count;
}

bool Dss::getAddressInfo(qapi_DSS_Addr_Info_t *info, uint32_t count)
{
	if (!handle || !info || !count)
		return false;
	uint32_t c = getIPcount();
	if (0 == c)
		return false;
	memset(info, 0, sizeof(qapi_DSS_Addr_Info_t));
	if (count > c) // MIN
		count = c;
	return qapi_DSS_Get_IP_Addr(handle, info, count) == 0;
}

bool Dss::getAddress(uint32_t count)
{
	return getAddressInfo(&addr_info, count);
}

uint32_t Dss::getMTU()
{
	uint32_t mtu = 0;
	if (handle)
		qapi_DSS_Get_Link_Mtu(handle, &mtu);
	return mtu;
}

bool Dss::send(uint8_t *data, uint32_t data_len, uint8_t ex_data)
{
	if (!handle || !data || !data_len)
		return -1;
	return qapi_DSS_Nipd_Send(handle, data, data_len, ex_data) == 0;
}

int Dss::getBearer()
{
	if (!handle)
		return -1;
	qapi_DSS_Data_Bearer_Tech_t bearer;
	int err = qapi_DSS_Get_Current_Data_Bearer_Tech(handle, &bearer);
	return err ? err : bearer;
}

bool Dss::getIFace(String &s)
{
	if (!handle || !s)
		return false;
	char iface[16];
	bool res = getIfaceName(&iface);
	s = res ? iface : "";
	return res;
}

void Dss::get(IPAddress &primary, IPAddress &secondary)
{
	getAddress(1);
	primary = addr_info.dnsp_addr_s.addr.v4;
	secondary = addr_info.dnss_addr_s.addr.v4;
}

void Dss::get(IPAddress &ip, IPAddress &gateway, IPAddress &primary, IPAddress &secondary)
{
	getAddress(1);
	ip = addr_info.iface_addr_s.addr.v4;
	gateway = addr_info.gtwy_addr_s.addr.v4;
	primary = addr_info.dnsp_addr_s.addr.v4;
	secondary = addr_info.dnss_addr_s.addr.v4;
}

void Dss::begin()
{
	ctor();
	static int once = 0;
	if (0 == once)
	{
		stack = (UCHAR *)malloc(DEFAUT_STACK_SIZE);
		int r = txm_module_object_allocate(&task, sizeof(TX_THREAD));
		if (r)
		{
			DEBUG_DSS("[ERROR] DSS THREAD txm_module_object_allocate = %d\n", r);
			abort();
		}
		r = tx_thread_create(task, "dss-thread", entry, this, stack, DEFAUT_STACK_SIZE, PRIO, PRIO, TX_NO_TIME_SLICE, TX_AUTO_START);
		if (r)
		{
			DEBUG_DSS("[ERROR] DSS tx_thread_create = %d\n", r);
			abort();
		}
		once = 1;
	}
}

void Dss::entry(void *p)
{
	if (NULL == p)
		return;
	int ret;
	Dss *THIS = (Dss *)p;
	THIS->state = DSS_ACT;
	DEBUG_DSS("[DSS] begin auto reconnect\n");
	while (1)
	{
		switch (THIS->state)
		{
		case DSS_ACT:
			DEBUG_DSS("[DSS]-ACT\n");
			THIS->act();
			THIS->state = DSS_WAIT;
			break;
		case DSS_ACTED:
			DEBUG_DSS("[DSS]-ACTED\n");
			THIS->state = DSS_WAIT;
			break;
		case DSS_DEACT:
			DEBUG_DSS("[DSS]-DEACT\n");
			THIS->deact();
			THIS->state = DSS_WAIT;
			break;
		case DSS_DEACTED:
			DEBUG_DSS("[DSS]-DEACTED\n");
			qapi_Timer_Sleep(1000, QAPI_TIMER_UNIT_MSEC, 1);
			THIS->state = DSS_ACT;
			break;
		default:
			break;
		}
		qapi_Timer_Sleep(100, QAPI_TIMER_UNIT_MSEC, 1);
	}
}
