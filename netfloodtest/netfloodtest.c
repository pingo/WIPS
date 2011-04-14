#include "contiki.h"
#include "net/rime.h"
#include "net/rime/netflood.h"
#include "dev/leds.h"

#include <stdio.h>

PROCESS(netfloodtest_process, "netfloodtest");
AUTOSTART_PROCESSES(&netfloodtest_process);

static int nf_recv(struct netflood_conn *c, const rimeaddr_t *from, const rimeaddr_t *originator, uint8_t seqno, uint8_t hops)
{
	char *ptr = packetbuf_dataptr();

	printf("packet from %d.%d originating from %d.%d (seqno %d, %d hops): %s\r\n",
		from->u8[0],
		from->u8[1],
		originator->u8[0],
		originator->u8[1],
		seqno,
		hops,
		*ptr ? "true" : "false");

	return 0;
}

static void nf_sent(struct netflood_conn *c)
{
	printf("packet sent\r\n");
}

static void nf_dropped(struct netflood_conn *c)
{
	printf("packet dropped\r\n");
}

static struct etimer et;
static struct netflood_conn connection;
static struct netflood_callbacks callbacks = 
	{
		.recv    = &nf_recv,
		.sent    = &nf_sent,
		.dropped = &nf_dropped,
	};

PROCESS_THREAD(netfloodtest_process, ev, data)
{
	PROCESS_EXITHANDLER(netflood_close(&connection);)
	PROCESS_BEGIN();

	static uint8_t occupied = 0, seqno = 0;

	netflood_open(&connection, CLOCK_SECOND * 5, 42, &callbacks);

	for (;;)
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		occupied = !occupied;

		packetbuf_copyfrom(&occupied, 1);
		netflood_send(&connection, seqno++);
	}

	netflood_close(&connection);

	PROCESS_END();
}
