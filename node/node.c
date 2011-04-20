#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/z1-phidgets.h"
#include "net/rime.h"
#include "net/rime/netflood.h"
#include "lib/print-stats.h"

#include "callbacks.h"

#include <stdio.h>
#include <math.h>

PROCESS(node_process, "Node");
AUTOSTART_PROCESSES(&node_process);

#define SAMPLES 10
#define DELTA 1000

PROCESS_THREAD(node_process, ev, data)
{
	static struct etimer et;
	static struct timer event_timer;

	static int values[SAMPLES]; /* Buffer of SAMPLES latest values. */
	static int sample = 0;      /* Index of current sample in buffer. */

	static int i, v;
	static long avg;

	//memset(values, 0, SAMPLES*sizeof(int));

	PROCESS_EXITHANDLER(mesh_close(&mesh);)
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(phidgets);
	
	mesh_open(&mesh, 132, &callbacks);

	timer_set(&event_timer, CLOCK_SECOND * 5);

	for (;;)
	{
		etimer_set(&et, CLOCK_SECOND / 2);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		static uint8_t event = 0, seqno = 0;

		/*
		 * In the code below implements a Simple Moving Average filter.
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
			leds_on(LEDS_RED);
			event = 1;
		}
		else {
			leds_off(LEDS_RED);
			event = 0;
		}
		
		printf("v: %4d, avg: %4ld event:  %i\n", v, avg, event);
		
		/* Send event if presence is sensed, 
		   toggle the green led and print some stats */
		
		if (event && timer_expired(&event_timer)) {
			timer_set(&event_timer, CLOCK_SECOND * 5);
			packetbuf_copyfrom(&event, 1);
			netflood_send(&connection, seqno++);
			leds_toggle(LEDS_GREEN);
			print_stats();
		}
	}
	
	PROCESS_END();

}
