/*
 * Ril.cpp
 *
 *  Created on: 04.06.2018
 *      Author: Georgi Angelov
 */

#include "ril.h"

#define DEBUG_RIL
//DBG

extern "C" char *strdup(const char *s);

static const char *s_finalResponsesError[] = {"ERROR",
											  "+CMS ERROR:",
											  "+CME ERROR:",
											  "NO CARRIER",
											  "NO ANSWER",
											  "NO DIALTONE"};

static const char *s_finalResponsesSuccess[] = {"OK", "CONNECT"};

static const char *s_smsUnsoliciteds[] = {"+CMT:", "+CDS:", "+CBM:"};

#define N_ELEMS(x) (sizeof(x) / sizeof((x)[0]))

static int isFinalResponseError(const char *line)
{
	size_t i;
	for (i = 0; i < N_ELEMS(s_finalResponsesError); i++)
	{
		if (strStartsWith(line, s_finalResponsesError[i]))
			return 1;
	}
	return 0;
}

static int isFinalResponseSuccess(const char *line)
{
	size_t i;
	for (i = 0; i < N_ELEMS(s_finalResponsesSuccess); i++)
	{
		if (strStartsWith(line, s_finalResponsesSuccess[i]))
			return 1;
	}
	return 0;
}

int isFinalResponse(const char *line)
{
	return isFinalResponseSuccess(line) || isFinalResponseError(line);
}

int isSMSUnsolicited(const char *line)
{
	size_t i;
	for (i = 0; i < N_ELEMS(s_smsUnsoliciteds); i++)
	{
		if (strStartsWith(line, s_smsUnsoliciteds[i]))
			return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Ril::Ril(qapi_at_port_t id)
{
	onURC = NULL;
	task = NULL;
	event = NULL;
	w_mutex = NULL;
	l_mutex = NULL;
	c_mutex = NULL;
	stack = NULL;
	stream = -1;
	port_id = id;
}

void Ril::ctor()
{
	static int once = 1;
	if (once)
	{
		int r;
		if ((r = txm_module_object_allocate(&w_mutex, sizeof(TX_MUTEX))))
		{
			DEBUG_RIL("[RIL] MUTEX W txm_module_object_allocate: %d\n", r);
			goto error;
		}
		if ((r = txm_module_object_allocate(&l_mutex, sizeof(TX_MUTEX))))
		{
			DEBUG_RIL("[RIL] MUTEX L txm_module_object_allocate: %d\n", r);
			goto error;
		}
		if ((r = txm_module_object_allocate(&c_mutex, sizeof(TX_MUTEX))))
		{
			DEBUG_RIL("[RIL] MUTEX C txm_module_object_allocate: %d\n", r);
			goto error;
		}
		if ((r = txm_module_object_allocate(&event, sizeof(TX_EVENT_FLAGS_GROUP))))
		{
			DEBUG_RIL("[RIL] EVENT txm_module_object_allocate: %d\n", r);
			goto error;
		}
		if ((r = txm_module_object_allocate(&task, sizeof(TX_THREAD))))
		{
			DEBUG_RIL("[RIL] TASK txm_module_object_allocate = %d\n", r);
			goto error;
		}
		if ((r = tx_mutex_create(w_mutex, "ril-w-mutex", TX_INHERIT)))
		{
			DEBUG_RIL("[RIL] MUTEX W tx_mutex_create = %d\n", r);
			goto error;
		}
		if ((r = tx_mutex_create(l_mutex, "ril-l-mutex", TX_INHERIT)))
		{
			DEBUG_RIL("[RIL] MUTEX L tx_mutex_create = %d\n", r);
			goto error;
		}
		if ((r = tx_mutex_create(c_mutex, "ril-c-mutex", TX_INHERIT)))
		{
			DEBUG_RIL("[RIL] MUTEX C tx_mutex_create = %d\n", r);
			goto error;
		}
		if ((r = tx_event_flags_create(event, "ril-event")))
		{
			DEBUG_RIL("[RIL] EVENT tx_event_flags_create = %d\n", r);
			goto error;
		}
		stack = (UCHAR *)malloc(RIL_STACK_SIZE);
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_TICK, 1);
		if ((r = tx_thread_create(task, "ril-thread", entry, this, stack, RIL_STACK_SIZE, PRIO, PRIO, TX_NO_TIME_SLICE, TX_AUTO_START)))
		{
			DEBUG_RIL("[RIL] TASK tx_thread_create = %d\n", r);
			goto error;
		}
		//DEBUG_RIL("[RIL] CTOR: %d\n", port_id);
		once = 0;		
	}
	return;
error:
	abort();
	
}

void Ril::entry(void *p)
{
	Ril *THIS = (Ril *)p;
	const char *line;
	tx_event_flags_set(THIS->event, 0, TX_AND); // CLEAR ALL
	//DEBUG_RIL("[RIL] BEGIN\n");
	while (1)
	{
		ULONG sig;
		tx_event_flags_get(THIS->event, RIL_RECEIVE_EVENT_MASK, TX_OR_CLEAR, &sig, TX_WAIT_FOREVER);
		while ((line = THIS->readLine()))
		{
			//DEBUG_RIL("[R] %s\n", line);
			THIS->processLine(line);
		}
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_TICK, 1); // YEILD
	}
}

void Ril::begin(qapi_at_resp_func_cb_t receive_callback)
{
	ctor();
	static bool started = 0;
	if (started)
		return;
	int r;
	rx_ring_tail = 0;
	rx_ring_head = 0;
	clearLine();
	r = tx_event_flags_set(event, 0, TX_AND); //CLEAR ALL
	qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_TICK, 1);
	r = qapi_QT_Apps_AT_Port_Open(port_id, &stream, receive_callback, /*SDK4*/ &pipe_data);
	if (r != QAPI_QT_ERR_OK)
	{
		DEBUG_RIL("[ERROR] RIL qapi_QT_Apps_AT_Port_Open = %d, %d, %d, %X\n", r, port_id, stream, receive_callback);
		abort();
	}
	qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_TICK, 1);
	started = 1;
}

void Ril::end()
{
	if (stream > -1)
	{
		qapi_QT_Apps_AT_Port_Close(port_id);
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_TICK, 1);
		stream = -1;
	}
	//TODO
}

Ril::~Ril()
{
	end();
	tx_mutex_delete(w_mutex);
	tx_mutex_delete(l_mutex);
	tx_mutex_delete(c_mutex);
	tx_event_flags_delete(event);
}

size_t Ril::save(char c)
{
	ENTER_CRITICAL();
	uint32_t i = (uint32_t)(rx_ring_head + 1) % sizeof(rx_ring_buff);
	size_t saved = 0;
	if (i != rx_ring_tail)
	{
		rx_ring_buff[rx_ring_head] = c;
		rx_ring_head = i;
		saved = 1; // saved one char
	}
	EXIT_CRITICAL();
	return saved; // full
}

size_t Ril::save(char *buffer, size_t size)
{
	if (!size || !buffer)
		return 0;
	size_t saved = 0;
	for (size_t i = 0; i < size; i++, saved++)
		if (0 == save(buffer[i]))
			break;
	tx_event_flags_set(event, RIL_RECEIVE_EVENT_MASK, TX_OR);
	return saved;
}

size_t Ril::write(const char *s)
{
	if (stream < 0 || !s)
		return 0;
	tx_mutex_get(w_mutex, TX_WAIT_FOREVER);
	int rc = qapi_QT_Apps_Send_AT(stream, s) == QAPI_QT_ERR_OK ? strlen(s) : 0;
	qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_TICK, 1); // FUCK QUECTEL
	tx_mutex_put(w_mutex);
	return rc;
}

size_t Ril::available(void)
{
	return (uint32_t)(sizeof(rx_ring_buff) + rx_ring_head - rx_ring_tail) % sizeof(rx_ring_buff);
}

int Ril::read(void)
{
	if (rx_ring_head == rx_ring_tail)
		return -1;
	ENTER_CRITICAL();
	uint8_t rc = rx_ring_buff[rx_ring_tail];
	rx_ring_tail = (uint32_t)(rx_ring_tail + 1) % sizeof(rx_ring_buff);
	EXIT_CRITICAL();
	return rc;
}

int Ril::writeCtrlZ(const char *s)
{
	if (!s)
		return AT_ERROR_GENERIC;
	/* the main string */
	if (0 == write(s))
		return AT_ERROR_GENERIC;
	/* the ^Z  */
	if (0 == write("\032"))
		return AT_ERROR_GENERIC;
	return AT_NO_ERROR;
}

void Ril::clearLine(void)
{
	memset(lineBuffer, 0, sizeof(lineBuffer));
	lineBufferCur = lineBuffer;
}

ATResponse *Ril::response_new(void)
{
	return (ATResponse *)calloc(1, sizeof(ATResponse));
}

void Ril::response_free(ATResponse *p_response)
{
	ATLine *p_line;
	if (p_response == NULL)
		return;
	p_line = p_response->p_intermediates;
	while (p_line != NULL)
	{
		ATLine *p_toFree;
		p_toFree = p_line;
		p_line = p_line->p_next;
		free(p_toFree->line);
		free(p_toFree);
	}
	free(p_response->finalResponse);
	free(p_response);
}

void Ril::reverseIntermediates(ATResponse *p_response)
{
	ATLine *pcur, *pnext;
	pcur = p_response->p_intermediates;
	p_response->p_intermediates = NULL;
	while (pcur != NULL)
	{
		pnext = pcur->p_next;
		pcur->p_next = p_response->p_intermediates;
		p_response->p_intermediates = pcur;
		pcur = pnext;
	}
}

void Ril::addIntermediate(const char *line)
{
	ATLine *p_new;
	p_new = (ATLine *)malloc(sizeof(ATLine));
	p_new->line = strdup(line);
	p_new->p_next = sp_response->p_intermediates;
	sp_response->p_intermediates = p_new;
}

const char *Ril::readLine(void)
{
	int rc;
	while ((rc = read()) > -1)
	{
		if (0 == sizeof(lineBuffer) - (lineBufferCur - lineBuffer)) /* LIMIT */
		{
			clearLine();
			return NULL;
		}
		*lineBufferCur++ = (char)rc;
		*lineBufferCur = '\0';
		if ('\n' == (char)rc)
		{
			lineBufferCur = lineBuffer;
			return lineBuffer;
		}
	}
	return NULL;
}

void Ril::processLine(const char *line)
{
	tx_mutex_get(l_mutex, TX_WAIT_FOREVER);
	if (sp_response == NULL)
	{
		/* no command pending */
		handleUnsolicited(line);
	}
	else if (isFinalResponseSuccess(line))
	{
		sp_response->success = 1;
		handleFinalResponse(line);
	}
	else if (isFinalResponseError(line))
	{
		sp_response->success = 0;
		handleFinalResponse(line);
	}
	else if (s_smsPDU != NULL && 0 == strcmp(line, "> "))
	{
		//Commands like AT+CMGS have a "> " prompt
		writeCtrlZ(s_smsPDU);
		s_smsPDU = NULL;
	}
	else
		switch (s_type)
		{
		case NO_RESULT:
			handleUnsolicited(line);
			break;
		case NUMERIC:
			if (sp_response->p_intermediates == NULL && isdigit(line[0]))
			{
				addIntermediate(line);
			}
			else
			{
				handleUnsolicited(line);
			}
			break;
		case SINGLELINE:
			if (sp_response->p_intermediates == NULL && strStartsWith(line, s_responsePrefix))
			{
				addIntermediate(line);
			}
			else
			{
				handleUnsolicited(line);
			}
			break;
		case MULTILINE:
			if (strStartsWith(line, s_responsePrefix))
			{
				addIntermediate(line);
			}
			else
			{
				handleUnsolicited(line);
			}
			break;
		default: /* this should never be reached */
			handleUnsolicited(line);
			break;
		}
	clearLine();
	tx_mutex_put(l_mutex);
	qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_TICK, true); // IMPORTANT YIELD !!!
}

void Ril::handleFinalResponse(const char *line)
{
	if (!line)
		return;
	sp_response->finalResponse = strdup(line);
	//DEBUG_RIL("[FINAL] %s", sp_response->finalResponse);
	tx_event_flags_set(event, RIL_RESPONSE_EVENT_MASK, TX_OR);
}

void Ril::handleUnsolicited(const char *line)
{
	if (!line)
		return;
	//DEBUG_RIL("[UNSOL] %s", line);
	if (onURC)
		onURC(line);
}

void Ril::clearPendingCommand(void)
{
	if (sp_response != NULL)
		response_free(sp_response);
	sp_response = NULL;
	s_responsePrefix = NULL;
	s_smsPDU = NULL;
}

int Ril::send_nolock(const char *command, ATCommandType type, const char *responsePrefix, const char *smspdu, uint32_t timeoutMsec, ATResponse **pp_outResponse)
{
	int err = 0;
	ULONG sig = 0;
	if (sp_response != NULL)
	{
		err = AT_ERROR_COMMAND_PENDING;
		goto error;
	}
	if (0 == write(command))
	{
		err = AT_ERROR_GENERIC;
		goto error;
	}
	s_type = type;
	s_responsePrefix = responsePrefix;
	s_smsPDU = smspdu;
	sp_response = response_new();
	if (timeoutMsec <= TICKS_PER_MSEC)
		timeoutMsec = TX_WAIT_FOREVER;
	else
		timeoutMsec /= TICKS_PER_MSEC;
	while (sp_response->finalResponse == NULL)
	{
		tx_event_flags_get(event, RIL_RESPONSE_EVENT_MASK, TX_OR_CLEAR, &sig, timeoutMsec);
		if (0 == (sig & RIL_RESPONSE_EVENT_MASK))
		{
			err = AT_ERROR_TIMEOUT;
			goto error;
		}
	}
	if (pp_outResponse == NULL)
	{
		response_free(sp_response);
	}
	else
	{
		reverseIntermediates(sp_response);
		*pp_outResponse = sp_response;
	}
	sp_response = NULL;
	err = AT_NO_ERROR;
error:
	clearPendingCommand();
	return err;
}

int Ril::send_full(const char *command, ATCommandType type, const char *responsePrefix, const char *smspdu, uint32_t timeoutMsec, ATResponse **pp_outResponse)
{
	int err;
	tx_mutex_get(c_mutex, TX_WAIT_FOREVER);
	err = send_nolock(command, type, responsePrefix, smspdu, timeoutMsec, pp_outResponse);
	tx_mutex_put(c_mutex);
	return err;
}

int Ril::send(const char *command, ATResponse **pp_outResponse)
{
	return send_full(command, NO_RESULT, NULL, NULL, 0, pp_outResponse);
}

int Ril::send_singleline(const char *command, const char *responsePrefix, ATResponse **pp_outResponse)
{
	int err;
	err = send_full(command, SINGLELINE, responsePrefix, NULL, 0, pp_outResponse);
	if (err == 0 && pp_outResponse != NULL && (*pp_outResponse)->success > 0 && (*pp_outResponse)->p_intermediates == NULL)
	{
		response_free(*pp_outResponse);
		*pp_outResponse = NULL;
		return AT_ERROR_INVALID_RESPONSE;
	}
	return err;
}

int Ril::send_numeric(const char *command, ATResponse **pp_outResponse)
{
	int err;
	err = send_full(command, NUMERIC, NULL, NULL, 0, pp_outResponse);
	if (err == 0 && pp_outResponse != NULL && (*pp_outResponse)->success > 0 && (*pp_outResponse)->p_intermediates == NULL)
	{
		/* successful command must have an intermediate response */
		response_free(*pp_outResponse);
		*pp_outResponse = NULL;
		return AT_ERROR_INVALID_RESPONSE;
	}
	return err;
}

int Ril::send_sms(const char *command, const char *pdu, const char *responsePrefix, ATResponse **pp_outResponse)
{
	int err;
	err = send_full(command, SINGLELINE, responsePrefix, pdu, 0, pp_outResponse);
	if (err == 0 && pp_outResponse != NULL && (*pp_outResponse)->success > 0 && (*pp_outResponse)->p_intermediates == NULL)
	{
		/* successful command must have an intermediate response */
		response_free(*pp_outResponse);
		*pp_outResponse = NULL;
		return AT_ERROR_INVALID_RESPONSE;
	}
	return err;
}

int Ril::send_multiline(const char *command, const char *responsePrefix, ATResponse **pp_outResponse)
{
	int err;
	err = send_full(command, MULTILINE, responsePrefix, NULL, 0, pp_outResponse);
	return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Ril::getIMEI(char *imei, size_t size)
{
	if (NULL == imei || size < 16)
		return false;
	int r;
	memset(imei, 0, size);
	ATResponse *response = NULL;
	if (AT_NO_ERROR == (r = send_numeric("AT+GSN\r\n", &response)))
	{
		if (response->p_intermediates->line)
			strcpy(imei, response->p_intermediates->line);
		response_free(response);
	}
	return r == AT_NO_ERROR;
}

bool Ril::sleep(bool enable)
{
	if (enable)
		return send("AT+QSCLK=1\r\n", NULL);
	else
		return send("AT+QSCLK=0\r\n", NULL);
}