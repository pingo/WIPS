#include "contiki.h"
#include "dev/leds.h"
#include "net/rime.h"
#include "net/rime/mesh.h"

#include "callbacks.h"

#include <stdio.h>

void cb_recv(struct mesh_conn *c, const rimeaddr_t *from, uint8_t hops)
{
	char *ptr = packetbuf_dataptr();

	printf("%d.%d %d %d\r\n",
		from->u8[0],  /* Originating node address. */
		from->u8[1],
		*ptr ? 1 : 0, /* 1 = Occupied, 0 = Unoccupied. */
		hops);        /* Hops the packet took, 1 for direct transmission. */
}

void cb_sent(struct mesh_conn *c)
{
	/* Never reached. */
}

void cb_timedout(struct mesh_conn *c)
{
	/* Never reached. */
}
