#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/z1-phidgets.h"
#include "net/rime.h"
#include "net/rime/mesh.h"
#include "lib/print-stats.h"
#include "net/rime/route.h"

list_t route_table_get(void);

#include "callbacks.h"

#include <stdio.h>
#include <math.h>

PROCESS(node_process, "node");
AUTOSTART_PROCESSES(&node_process);

uint16_t seqno = 0;

static struct mesh_conn mesh;
const static struct mesh_callbacks callbacks =
	{
		.recv    = &cb_recv,
		.sent    = &cb_sent,
		.timedout = &cb_timedout,
	};

PROCESS_THREAD(node_process, ev, data)
{
	static struct etimer et;
	
	PROCESS_EXITHANDLER(mesh_close(&mesh));
	PROCESS_BEGIN();
	
	SENSORS_ACTIVATE(button_sensor);

	mesh_open(&mesh, 132, &callbacks);
	
	etimer_set(&et, CLOCK_SECOND * 15);

	for (;;)
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et) || ev == sensors_event);
				
		if (ev == sensors_event)
		{
			static struct route_entry *e;

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
		}
		else
		{
			etimer_set(&et, CLOCK_SECOND * 15);
			
			rimeaddr_t addr = { { 70, 0 } };

			printf("sending: %d.%d  %i\n", rimeaddr_node_addr.u8[0], 														   rimeaddr_node_addr.u8[1], 
										   seqno);
			
			packetbuf_copyfrom(&seqno, sizeof(uint16_t));
			mesh_send(&mesh, &addr);
		}
	}
	PROCESS_END();
	
}
