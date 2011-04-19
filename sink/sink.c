#include "contiki.h"
#include "net/rime.h"
#include "net/rime/netflood.h"
#include "dev/leds.h"

#include "callbacks.h"

#include <stdio.h>

PROCESS(sink_process, "Sink");
AUTOSTART_PROCESSES(&sink_process);

static struct netflood_conn connection;
static struct netflood_callbacks callbacks = 
	{
		.recv    = &nf_recv,
		.sent    = &nf_sent,
		.dropped = &nf_dropped,
	};

PROCESS_THREAD(sink_process, ev, data)
{
	PROCESS_EXITHANDLER(netflood_close(&connection);)
	PROCESS_BEGIN();

	netflood_open(&connection, CLOCK_SECOND * 10, 42, &callbacks);

	for (;;)
	{
		PROCESS_WAIT_EVENT();
	}

	PROCESS_END();
}

