/*
 * Ril.h
 *
 *  Created on: 04.06.2018
 *      Author: 1124
 */

#ifndef INTERNALSERIAL_H_
#define INTERNALSERIAL_H_

#include <interface.h>
#include <qapi_quectel.h>
#include <ctype.h>

#define AT_ERROR_GENERIC (-1)
#define AT_ERROR_COMMAND_PENDING (-2)
#define AT_ERROR_CHANNEL_CLOSED (-3)
#define AT_ERROR_TIMEOUT (-4)
#define AT_ERROR_INVALID_THREAD (-5)   /* AT commands may not be issued from reader thread (or unsolicited response callback */
#define AT_ERROR_INVALID_RESPONSE (-6) /* eg an at_send_command_singleline that did not get back an intermediate response */
#define AT_NO_ERROR 0

typedef enum
{
	NO_RESULT,  /* no intermediate response expected */
	NUMERIC,	/* a single intermediate response starting with a 0-9 */
	SINGLELINE, /* a single intermediate response starting with a prefix */
	MULTILINE   /* multiple line intermediate response starting with a prefix */
} ATCommandType;

/** a singly-lined list of intermediate responses */
typedef struct ATLine
{
	struct ATLine *p_next;
	char *line;
} ATLine;

/** Free this with at_response_free() */
typedef struct
{
	int success;			 /* true if final response indicates success (eg "OK") */
	char *finalResponse;	 /* eg OK, ERROR */
	ATLine *p_intermediates; /* any intermediate responses */
} ATResponse;

#define RIL_STACK_SIZE (8 * 1024)
#define RIL_RECEIVE_EVENT_MASK (0x00000001)
#define RIL_RESPONSE_EVENT_MASK (0x10000000)
#define INTERNAL_RECEIVE_SIZE (4 * 1024)

class Ril
{
protected:
	TX_THREAD *task;
	TX_EVENT_FLAGS_GROUP *event;
	TX_MUTEX *w_mutex;
	TX_MUTEX *l_mutex;
	TX_MUTEX *c_mutex;
	UCHAR *stack;
	char lineBuffer[INTERNAL_RECEIVE_SIZE + 1];
	char *lineBufferCur = 0;
	ATCommandType s_type = NO_RESULT;
	const char *s_responsePrefix = NULL;
	const char *s_smsPDU = NULL;
	ATResponse *sp_response = NULL;
	static void entry(void *THIS);

private:
	qapi_at_port_t port_id;
	qapi_at_stream_id_t stream;
	uint32_t rx_ring_tail = 0;
	uint32_t rx_ring_head = 0;
	UCHAR rx_ring_buff[INTERNAL_RECEIVE_SIZE];
	void ctor();
	size_t save(char c);
	int writeCtrlZ(const char *s);
	void clearLine(void);
	const char *readLine(void);
	void processLine(const char *line);
	void handleUnsolicited(const char *line);
	void handleFinalResponse(const char *line);
	void clearPendingCommand(void);
	void addIntermediate(const char *line);
	void reverseIntermediates(ATResponse *p_response);
	int send_nolock(const char *command, ATCommandType type, const char *responsePrefix, const char *smspdu, uint32_t timeoutMsec, ATResponse **pp_outResponse);
	int send_full(const char *command, ATCommandType type, const char *responsePrefix, const char *smspdu, uint32_t timeoutMsec, ATResponse **pp_outResponse);

	qapi_at_pipe_data_t pipe_data;

public:
	Ril(qapi_at_port_t port_id);
	virtual ~Ril();
	void begin(qapi_at_resp_func_cb_t cb = NULL);
	void end(void);
	size_t available(void);
	int read(void);
	size_t write(const char *str);
	size_t save(char *buffer, size_t size);
	int send(const char *command, ATResponse **pp_outResponse);
	int send_singleline(const char *command, const char *responsePrefix, ATResponse **pp_outResponse);
	int send_numeric(const char *command, ATResponse **pp_outResponse);
	int send_sms(const char *command, const char *pdu, const char *responsePrefix, ATResponse **pp_outResponse);
	int send_multiline(const char *command, const char *responsePrefix, ATResponse **pp_outResponse);

	ATResponse *response_new(void);
	void response_free(ATResponse *p_response);
	void (*onURC)(const char *line);

	bool getIMEI(char *imei, size_t size);
	bool sleep(bool enable);
};

#endif /* INTERNALSERIAL_H_ */
