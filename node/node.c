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

#define SAMPLES 10
#define DELTA 1000


void std_packet(char *packet_buffer, int p_type, int seq_flag, int retries, int payload) {
	packet_buffer[0] = p_type & 0x3F | (seq_flag != 0) << 6 | (retries & 0x1) << 7;
	packet_buffer[1] = retries >> 1 | (payload != 0) <<7);
}

PROCESS_THREAD(node_process, ev, data)
{
	static struct etimer period;
	static struct etimer event;
	static struct etimer timeout;
	
	static int values[SAMPLES]; /* Buffer of SAMPLES latest values. */
	static int sample = 0;      /* Index of current sample in buffer. */
	
	static int i, v;
	static long avg;
	
	static uint8_t event = 0;
	
	//memset(values, 0, SAMPLES*sizeof(uint8_t));
	
	PROCESS_EXITHANDLER(mesh_close(&mesh));
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(phidgets);

	mesh_open(&mesh, 132, &callbacks);
	
	/* Set timers to reduce startup conditions */
	etimer_set(&period, CLOCK_SECOND * 15);
	etimer_set(&event, CLOCK_SECOND * 5);
	etimer_set(&timeout, CLOCK_SECOND * 300);
	
	/* Set sink node address */
	rimeaddr_t sink_addr = { { 70, 0 } };
	
	for (;;)
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et) || ev == sensors_event);
		
		if (ev == sensors_event) {
			printf("button pressed", );
		}
				
		/*
		 * The code below implements a Simple Moving Average filter.
		 */
		
		sample = (sample + 1) % SAMPLES; /* Wrap around in buffer. */
		
		v = phidgets.value(PHIDGET3V_1); /* Get new value from sensor. */
		values[sample] = v;
		
		/* Calculate average of the SAMPLES latest values. */
		avg = 0;
		for (i = 0; i < SAMPLES; i++)
			avg += values[i];
		avg /= SAMPLES;

		/* See if our newest value is off by more than DELTA from the average */
		if (abs(v - avg) > DELTA) {
			event = 1;
		}
		else {
			event = 0;
		}
		
		printf("v: %4d, avg: %4ld event:  %i\n", v, avg, event)</>;
		
		char packet_buffer[3];
		memset(packet_buffer, 0, 3*size_of(char));
				
		/* Periodic */
		
		if (etimer_expired(&period)) {
			etimer_set(&et, CLOCK_SECOND * 15);
			std_packet(packet_buffer, 0, 0, 0, event);
			packetbuf_copyfrom(&event, 2);
			mesh_send(&mesh, &sink_addr);
		}
				
		/* Event */	
		
		if (etimer_expired(&event)) {
			etimer_set(&event_threshold, CLOCK_SECOND * 5);
			std_packet(packet_buffer, 1, 0, 0, 1);
			packetbuf_copyfrom(&packet_buffer, 2);
			mesh_send(&mesh, &sink_addr);
			
			event = 0;
		}
		
		/* Timeout */
		
		if (etimer_expired(&timeout)) {
			// is the room empty, aka timeout?
			etimer_set(&event_threshold, CLOCK_SECOND * 300);
			std_packet(packet_buffer, 2, 0, 0, 1);
			packetbuf_copyfrom(&packet_buffer, 2);
			mesh_send(&mesh, &sink_addr);			
		} 
	}
	PROCESS_END();
}