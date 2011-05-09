#include "contiki.h"
#include "net/rime.h"
#include "net/rime/mesh.h"
#include "net/mac/mac.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"

#include <stdio.h>

static struct mesh_conn mesh;

PROCESS(meshtestsink_process, "meshtestsink");
AUTOSTART_PROCESSES(&meshtestsink_process);

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
	printf("Data received from %d.%d (%d B), %d hops.\n",
		from->u8[0], from->u8[1], packetbuf_datalen(), hops);

	uint8_t *buffer = packetbuf_dataptr();
	printf("Room is %s\n", *buffer ? "occupied" : "unoccupied");
}

const static struct mesh_callbacks callbacks = {recv, sent, timedout};

PROCESS_THREAD(meshtestsink_process, ev, data)
{
	PROCESS_EXITHANDLER(mesh_close(&mesh);)
	PROCESS_BEGIN();

	mesh_open(&mesh, 132, &callbacks);

	SENSORS_ACTIVATE(button_sensor);

	for (;;)
	{
		PROCESS_WAIT_EVENT(); /* _UNTIL(ev == sensors_event && data == &button_sensor); */

/*		leds_toggle(LEDS_GREEN);
		printf("Click.\n"); */

/*		rimeaddr_t addr;

		packetbuf_copyfrom("Hej", 3);
		addr.u8[0] = 74;
		addr.u8[1] = 0;
		mesh_send(&mesh, &addr); */
	}

	PROCESS_END();
}

