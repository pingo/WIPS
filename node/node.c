#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/z1-phidgets.h"
#include "net/rime.h"
#include "net/rime/netflood.h"

#include <stdio.h>
#include <math.h>

PROCESS(node_process, "node");
AUTOSTART_PROCESSES(&node_process);

static int nf_recv(struct netflood_conn *c, const rimeaddr_t *from, const rimeaddr_t *originator, uint8_t seqno, uint8_t hops)
{
	char *ptr = packetbuf_dataptr();

	printf("packet from %d.%d originating from %d.%d (seqno %d, %d hops): %s\r\n",
		from->u8[0],
		from->u8[1],
		originator->u8[0],
		originator->u8[1],
		seqno,
		hops,
		*ptr ? "true" : "false");

	return 1;
}

static void nf_sent(struct netflood_conn *c)
{
	printf("packet sent\r\n");
}

static void nf_dropped(struct netflood_conn *c)
{
	printf("packet dropped\r\n");
}

static struct netflood_conn connection;
static struct netflood_callbacks callbacks = 
	{
		.recv    = &nf_recv,
		.sent    = &nf_sent,
		.dropped = &nf_dropped,
	};


#define SAMPLES 10
#define DELTA 1000

PROCESS_THREAD(node_process, ev, data)
{
	static struct etimer et;
	static struct timer timeout_timer;
	static struct timer event_timer;

	static int values[SAMPLES]; /* Buffer of SAMPLES latest values. */
	static int sample = 0;      /* Index of current sample in buffer. */

	static int i, v;
	static long avg;

	//memset(values, 0, SAMPLES*sizeof(int));

	PROCESS_EXITHANDLER(netflood_close(&connection);)
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(phidgets);
	
	netflood_open(&connection, CLOCK_SECOND * 10, 42, &callbacks);

	timer_set(&event_timer, CLOCK_SECOND * 3);
	timer_set(&timeout_timer, CLOCK_SECOND * 60);
			

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
		
		printf("v: %4d, avg: %4ld event: %i\n", v, avg, event);
				
		if (event && timer_expired(&event_timer)) {
			timer_set(&event_timer, CLOCK_SECOND * 3);
			timer_set(&timeout_timer, CLOCK_SECOND * 60);
			printf("room occupied event sent\n");
			packetbuf_copyfrom(&event, 1);
			netflood_send(&connection, seqno++);
			leds_toggle(LEDS_GREEN);
			leds_off(LEDS_BLUE);
		}
		
		if (timer_expired(&timeout_timer)) {
			timer_set(&timeout_timer, CLOCK_SECOND * 60);
			printf("timeout -> room is : %i\n", event);
			packetbuf_copyfrom(&event, 1);
			netflood_send(&connection, seqno++);
			leds_toggle(LEDS_BLUE);
		}
	}

	PROCESS_END();
}

