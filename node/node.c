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

PROCESS_THREAD(node_process, ev, data)
{
	static struct etimer et;
	static struct timer event_timer; /* Periodic threshhold for event sensing. */
	
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
	
	/* Set timer to reduce startup conditions */
	timer_set(&event_timer, CLOCK_SECOND * 5);
	
	for (;;)
	{
		etimer_set(&et, CLOCK_SECOND / 2);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
				
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
		
		printf("v: %4d, avg: %4ld event:  %i\n", v, avg, event);
		
		/* Send event if presence is sensed, 
		   toggle the green led and print some stats */
		
		rimeaddr_t addr = { { 70, 0 } };
		
		if (event && timer_expired(&event_timer)) {
			timer_set(&event_timer, CLOCK_SECOND * 5);
			packetbuf_copyfrom(&event, 1);
			mesh_send(&mesh, &addr);
			leds_toggle(LEDS_GREEN);
			print_stats();
		}
	}
	PROCESS_END();
	
}