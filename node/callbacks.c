#include "contiki.h"
#include "dev/leds.h"
#include "net/rime.h"
#include "net/rime/mesh.h"

#include "callbacks.h"
#include "proto.h"
#include "node.h"

#include <stdio.h>

static int alert = 0;

void cb_recv(struct mesh_conn *c, const rimeaddr_t *from, uint8_t hops)
{
	/* Packet attributes. */
	char *ptr = packetbuf_dataptr();
	int seq_flag, retries;
	uint16_t payload;

	switch (proto_p_type(ptr))
	{
		case P_TYPE_ACK:
			proto_ack_unpack(ptr, &seq_flag, &retries);
			printf("ACK from: %d.%d, hops: %d, seq_flag: %d, retries: %d\n",
				from->u8[0], from->u8[1], hops, seq_flag, retries);
			/* Invert ACKed seq_flag and reset retries */
			if (p_seq_flag == seq_flag) {
				p_seq_flag = !p_seq_flag;
				p_retries = 0;
			}
			break;

		case P_TYPE_SET_DELTA:
			proto_set_unpack(ptr, &payload);
			printf("SET DELTA from: %d.%d, hops: %d, payload: %d\n",
				from->u8[0], from->u8[1], hops, payload);
			delta = payload;
			mesh_send(&mesh, &sink_addr);
			break;

		case P_TYPE_SET_SENSOR_INTERVAL:
			proto_set_unpack(ptr, &payload);
			printf("SET SENSOR INTERVAL from: %d.%d, hops: %d, payload: %d\n",
				from->u8[0], from->u8[1], hops, payload);
			sensor_interval = CLOCK_SECOND * payload;
			mesh_send(&mesh, &sink_addr);			
			break;

		case P_TYPE_SET_SENSOR_TIMEOUT:
			proto_set_unpack(ptr, &payload);
			printf("SET SENSOR TIMEOUT from: %d.%d, hops: %d, payload: %d\n",
				from->u8[0], from->u8[1], hops, payload);
			sensor_timeout = CLOCK_SECOND * payload;
			mesh_send(&mesh, &sink_addr);			
			break;

		case P_TYPE_SET_BEACON_INTERVAL:
			proto_set_unpack(ptr, &payload);
			printf("SET BEACON INTERVAL from: %d.%d, hops: %d, payload: %d\n",
				from->u8[0], from->u8[1], hops, payload);
			beacon_interval = CLOCK_SECOND * payload;
			mesh_send(&mesh, &sink_addr);
			break;

		default:
			printf("UNKNOWN PACKET TYPE\n");
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
	/* Display a red light indicating network issues. */
	if (!alert)
		leds_on(LEDS_RED);

	alert = 1;
}

