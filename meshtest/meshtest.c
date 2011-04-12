#include "contiki.h"
#include "net/rime.h"
#include "net/rime/mesh.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"

#include <stdio.h>

static struct mesh_conn mesh;

PROCESS(meshtest_process, "meshtest");
AUTOSTART_PROCESSES(&meshtest_process);

static void sent(struct mesh_conn *c)
{
	printf("Packet sent.\n");
}

static void timedout(struct mesh_conn *c)
{
	printf("Packet timedout.\n");
}

static void recv(struct mesh_conn *c, const rimeaddr_t *from, uint8_t hops)
{
	printf("Data received from %d.%d: %.*s (%d).\n",
		from->u8[0], from->u8[1],
		packetbuf_datalen(), (char *)packetbuf_dataptr(), packetbuf_datalen());

	packetbuf_copyfrom("Hopp", 4);
	mesh_send(&mesh, from);
}

const static struct mesh_callbacks callbacks = {recv, sent, timedout};

PROCESS_THREAD(meshtest_process, ev, data)
{
	PROCESS_EXITHANDLER(mesh_close(&mesh);)
	PROCESS_BEGIN();

	mesh_open(&mesh, 132, &callbacks);

	SENSORS_ACTIVATE(button_sensor);

/*	printf("Started.\n"); */

	for (;;)
	{
		rimeaddr_t addr;

		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

		leds_toggle(LEDS_GREEN);
		/* printf("Click.\n"); */

		packetbuf_copyfrom("Hej", 3);
		addr.u8[0] = 1;
		addr.u8[1] = 0;
		mesh_send(&mesh, &addr);
	}

	PROCESS_END();
}

