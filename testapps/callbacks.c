#include "contiki.h"
#include "dev/leds.h"
#include "net/rime.h"
#include "net/rime/mesh.h"

#include "callbacks.h"

extern uint16_t seqno[2];

static int alert = 0;

void cb_recv(struct mesh_conn *c, const rimeaddr_t *from, uint8_t hops)
{
	char *ptr = packetbuf_dataptr();

	printf("r %d.%d %2u %4d %3u\n",
		from->u8[0],  /* Originating node address. */
		from->u8[1],
		hops,         /* Hops the packet took, 1 for direct transmission. */
		packetbuf_attr(PACKETBUF_ATTR_RSSI),
		*ptr);        /* Data payload */
	
	if (seqno[0] >= *ptr) {
		seqno[0]++;
		seqno[1] = 0;
	}
	
	return 0; /* 0 = Do not resend. */
}

void cb_sent(struct mesh_conn *c)
{
	if (alert)
		leds_off(LEDS_RED);

	alert = 0;
}

void cb_timedout(struct mesh_conn *c)
{
	/*
	 * Display a red light indicating network issues.
	 */

	if (!alert)
		leds_on(LEDS_RED);

	alert = 1;
}

