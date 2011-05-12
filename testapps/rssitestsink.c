#include "contiki.h"
#include "net/rime.h"
#include "net/rime/mesh.h"
#include "net/mac/mac.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "net/rime/route.h"

#include <stdio.h>

list_t route_table_get(void); /* FIXME: Put in header. */

static struct mesh_conn mesh;

PROCESS(meshtest_process, "meshtest");
AUTOSTART_PROCESSES(&meshtest_process);

static void sent(struct mesh_conn *c)
{
	/* printf("Packet sent.\n"); */
}

static void timedout(struct mesh_conn *c)
{
	/* printf("Packet timedout.\n"); */
}

static void recv(struct mesh_conn *c, const rimeaddr_t *from, uint8_t hops)
{
	uint16_t *buf = packetbuf_dataptr();

	if (packetbuf_datalen() != sizeof(uint16_t) * 2)
		return;

	printf("%d.%d %2u %4d %3u %3u\n",
		from->u8[0], from->u8[1],
		hops,
		packetbuf_attr(PACKETBUF_ATTR_RSSI),
		buf[0], buf[1]);

	packetbuf_copyfrom(buf, sizeof(uint16_t));
	mesh_send(&mesh, from);
}

const static struct mesh_callbacks callbacks = {recv, sent, timedout};

PROCESS_THREAD(meshtest_process, ev, data)
{
	PROCESS_EXITHANDLER(mesh_close(&mesh);)
	PROCESS_BEGIN();

	mesh_open(&mesh, 132, &callbacks);

	SENSORS_ACTIVATE(button_sensor);

	for (;;)
	{
		static struct route_entry *e;

		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

		printf("BEGIN ROUTING TABLE\n");
		printf("dest\tnext\tcost\ttime\tdecay\tlastdecay\n");

		for (e = list_head(route_table_get()); e != NULL; e = list_item_next(e))
		{
			printf("%d.%d\t"
			       "%d.%d\t"
			       "%hhu\t"
			       "%hhu\t"
			       "%hhu\t"
			       "%hhu\n",
			       e->dest.u8[0], e->dest.u8[1],
			       e->nexthop.u8[0], e->nexthop.u8[1],
			       e->cost,
			       e->time,
			       e->decay,
			       e->time_last_decay);
		}

		printf("END ROUTING TABLE\n\n");

		leds_toggle(LEDS_BLUE);
	}

	PROCESS_END();
}

