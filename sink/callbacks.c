#include "contiki.h"
#include "dev/leds.h"
#include "net/rime.h"
#include "net/rime/mesh.h"

#include "callbacks.h"
#include "proto.h"

#include <stdio.h>

extern struct mesh_conn mesh;

void cb_recv(struct mesh_conn *c, const rimeaddr_t *from, uint8_t hops)
{
	char *ptr = packetbuf_dataptr();
	int seq_flag, retries, payload;
	char buf[2];
	char type;

	switch (proto_p_type(ptr))
	{
		case P_TYPE_STATUS:
			proto_status_unpack(ptr, &seq_flag, &retries, &payload);

			printf("t,%d.%d,%u,%d,%d,%d,%d\n",
				from->u8[0], from->u8[1],
				hops,
				packetbuf_attr(PACKETBUF_ATTR_RSSI),
				seq_flag, retries, payload);

			type = 't';

			proto_ack_pack(buf, seq_flag, retries);

			packetbuf_copyfrom(buf, 2);
			mesh_send(&mesh, from);
			break;

		case P_TYPE_SET_DELTA:
			type = 'd';
			break;

		case P_TYPE_SET_SENSOR_INTERVAL:
			type = 'i';
			break;

		case P_TYPE_SET_SENSOR_TIMEOUT:
			type = 's';
			break;

		case P_TYPE_SET_BEACON_INTERVAL:
			type = 'b';
			break;

		default:
			return;
	}

	if (type != 't')
	{
		printf("%c,%d.%d\n",
			type,
			from->u8[0], from->u8[1]);
	}
}

void cb_sent(struct mesh_conn *c)
{
	/* Never reached. */
}

void cb_timedout(struct mesh_conn *c)
{
	/* Never reached. */
}
