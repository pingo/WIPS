#include "contiki.h"
#include "net/rime.h"
#include "net/rime/mesh.h"
#include "net/mac/mac.h"
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
	printf("Data received from %d.%d (%d B), %d hops.\n",
		from->u8[0], from->u8[1], packetbuf_datalen(), hops);
}

const static struct mesh_callbacks callbacks = {recv, sent, timedout};

PROCESS_THREAD(meshtest_process, ev, data)
{
	PROCESS_EXITHANDLER(mesh_close(&mesh);)
	PROCESS_BEGIN();

	mesh_open(&mesh, 132, &callbacks);

	static uint8_t occupied = 0;
    static struct etimer et;

	SENSORS_ACTIVATE(button_sensor);

	for (;;)
	{
		etimer_set(&et, CLOCK_SECOND * 5);

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et) || (ev == sensors_event && data == &button_sensor));

		if (ev == sensors_event)
			occupied = !occupied;

		printf("Room now %s\n", occupied ? "occupied" : "unoccupied");

		if (occupied) leds_on(LEDS_GREEN);
		else          leds_off(LEDS_GREEN);

		leds_toggle(LEDS_BLUE);

		rimeaddr_t addr = { { 70, 0 } };
	
		packetbuf_copyfrom(&occupied, 1);
		mesh_send(&mesh, &addr);
	}

	PROCESS_END();
}

