#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/z1-phidgets.h"

#include <stdio.h>
#include <math.h>

PROCESS(test_phidgets_process, "Test Phidgets");
AUTOSTART_PROCESSES(&test_phidgets_process);

#define SAMPLES 10
#define DELTA 1000

PROCESS_THREAD(test_phidgets_process, ev, data)
{
	static struct etimer et;

	static int values[SAMPLES]; /* Buffer of SAMPLES latest values. */
	static int sample = 0;      /* Index of current sample in buffer. */

	static int i, v;
	static long avg;

	PROCESS_BEGIN();
	SENSORS_ACTIVATE(phidgets);

	for (;;)
	{
		etimer_set(&et, CLOCK_SECOND / 2);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

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
		if (abs(v - avg) > DELTA)
			leds_on(LEDS_RED);
		else
			leds_off(LEDS_RED);

		printf("v: %4d, avg: %4ld\n", v, avg);
	}

	PROCESS_END();
}

