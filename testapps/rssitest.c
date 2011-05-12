#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/z1-phidgets.h"
#include "net/rime.h"
#include "net/rime/mesh.h"
#include "lib/print-stats.h"

#include "callbacks.h"

#include <stdio.h>
#include <math.h>

PROCESS(node_process, "node");
AUTOSTART_PROCESSES(&node_process);

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
	static struct timer period_timer; /* Periodic timer for network protocol */
	
	static uint16_t seqno = 0;
	
	PROCESS_EXITHANDLER(mesh_close(&mesh));
	PROCESS_BEGIN();
	
	mesh_open(&mesh, 132, &callbacks);
	
	timer_set(&period_timer, CLOCK_SECOND * 15);
	
	for (;;)
	{
		etimer_set(&et, CLOCK_SECOND / 2);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
				
		rimeaddr_t addr = { { 70, 0 } };
		
		if (timer_expired(&period_timer)) {
			timer_set(&period_timer, CLOCK_SECOND * 15);

			printf("%d.%d  %i\n", rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1], seqno);
			
			packetbuf_copyfrom(&seqno, sizeof(uint16_t));
			mesh_send(&mesh, &addr);

			seqno++;
		}
		
	}
	PROCESS_END();
	
}
