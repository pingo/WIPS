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

	proto_status_unpack(ptr, &seq_flag, &retries, &payload);

	printf("STATUS from: %d.%d, hops: %d, seq_flag: %d, retries: %d, payload: %d\n",
		from->u8[0], from->u8[1], hops, seq_flag, retries, payload);

	proto_ack_pack(buf, seq_flag, retries);

	packetbuf_copyfrom(buf, 2);
	mesh_send(&mesh, from);
}

void cb_sent(struct mesh_conn *c)
{
	/* Never reached. */
}

void cb_timedout(struct mesh_conn *c)
{
	/* Never reached. */
}
