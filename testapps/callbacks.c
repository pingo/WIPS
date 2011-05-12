#include "contiki.h"
#include "dev/leds.h"
#include "net/rime.h"
#include "net/rime/mesh.h"

#include "callbacks.h"

extern uint16_t seqno;

static int alert = 0;

void cb_recv(struct mesh_conn *c, const rimeaddr_t *from, uint8_t hops)
{
	char *ptr = packetbuf_dataptr();

	printf("%d.%d %d %d\r\n",
		from->u8[0],  /* Originating node address. */
		from->u8[1],
		*ptr,         /* Data payload */
		hops);        /* Hops the packet took, 1 for direct transmission. */
	
	
	if (seqno >= *ptr) {
		seqno++;
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

