#include "contiki.h"
#include "dev/leds.h"
#include "net/rime.h"
#include "net/rime/mesh.h"

#include "callbacks.h"
#include "proto.h"

#include <stdio.h>

extern int p_seq_flag, p_retries;

static int alert = 0;

void cb_recv(struct mesh_conn *c, const rimeaddr_t *from, uint8_t hops)
{
	char *ptr = packetbuf_dataptr();
	int seq_flag, retries;

	switch (proto_p_type(ptr))
	{
		case P_TYPE_ACK:
			proto_ack_unpack(ptr, &seq_flag, &retries);
			printf("ACK from: %d.%d, hops: %d, seq_flag: %d, retries: %d\n",
				from->u8[0], from->u8[1], hops, seq_flag, retries);
			/* Invert ACKed seqno and reset retries */
			if (p_seq_flag == seq_flag) {
				p_seq_flag = !p_seq_flag;
				p_retries = 0;
			}
			break;

		default:
			printf("unkown packet type\n");
			break;
	}
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

