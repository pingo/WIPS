#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/z1-phidgets.h"
#include "net/rime.h"
#include "net/rime/mesh.h"
#include "lib/print-stats.h"

#include "callbacks.h"
#include "proto.h"

#include <stdio.h>
#include <stdlib.h>

static process_event_t empty_event;
static process_event_t occupied_event;

PROCESS(node_process, "node");
PROCESS(sensor_process, "sensor");
AUTOSTART_PROCESSES(&node_process);

#define SAMPLES 10

extern uint16_t delta;
extern struct etimer empty_timer;
extern struct etimer interval_timer;
extern struct etimer period_timer;
extern clock_time_t sensor_interval;
extern clock_time_t sensor_timeout;
extern clock_time_t beacon_interval;

struct mesh_conn mesh;
rimeaddr_t sink_addr = { { 70, 0 } };
int p_seq_flag = 0;
int p_retries = 0;

const static struct mesh_callbacks callbacks =
	{
		.recv     = &cb_recv,
		.sent     = &cb_sent,
		.timedout = &cb_timedout,
	};


static void send_status_packet(int payload)
{
	/* Create a buffer for packet */
	char packet_buffer[2];
				
	/* Send packet to sink */
	proto_status_pack(packet_buffer, p_seq_flag, p_retries, payload);
	packetbuf_copyfrom(packet_buffer, 2);
	mesh_send(&mesh, &sink_addr);

	printf("DATA  to: %d.%d, seq_flag: %i, retries: %i, payload: %i\n", sink_addr.u8[0], sink_addr.u8[1], p_seq_flag, p_retries, payload);

	++p_retries;
}

PROCESS_THREAD(sensor_process, ev, data)
{
	static struct etimer interval_timer, /* Sampling interval timer. */
	                     empty_timer;    /* Timer for determening room inactivity. */

	static int values[SAMPLES]; /* Buffer of SAMPLES latest values. */
	static int sample = 0;      /* Index of current sample in buffer. */

	static int occupied = 0;
	static int i, v;
	static long avg;

	PROCESS_BEGIN();
	SENSORS_ACTIVATE(phidgets);

	empty_event = process_alloc_event();
	occupied_event = process_alloc_event();

	etimer_set(&empty_timer, sensor_timeout);

	for (;;)
	{
		etimer_set(&interval_timer, sensor_interval);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&interval_timer));

		/*
		 * In the code below we implement a Simple Moving Average filter.
		 */

		sample = (sample + 1) % SAMPLES; /* Wrap around in buffer. */
		v = phidgets.value(PHIDGET3V_1); /* Get value from sensor. */
		values[sample] = v;

		/* Calculate average of the SAMPLES latest values. */
		avg = 0;
		for (i = 0; i < SAMPLES; i++)
			avg += values[i];
		avg /= SAMPLES;

		/* See if our newest value is off by more than DELTA from the average */
		if (abs(v - avg) > delta) {
			if (!occupied) {
				process_post(&node_process, occupied_event, NULL);
				occupied = 1;
			}

			etimer_set(&empty_timer, sensor_timeout);
		}
		else if (occupied && etimer_expired(&empty_timer)) {
			process_post(&node_process, empty_event, NULL);
			occupied = 0;
		}
	}

	PROCESS_END();
}

PROCESS_THREAD(node_process, ev, data)
{
	static struct etimer period_timer;	
	static int sensor_status = 0; /* 0 if not active (no one in room) */

	PROCESS_EXITHANDLER(mesh_close(&mesh));
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(button_sensor);

	mesh_open(&mesh, 132, &callbacks);

	/* Start the sensor process. */
	process_start(&sensor_process, NULL);

	/* Set timers to reduce startup conditions */
	etimer_set(&period_timer, beacon_interval);

	for (;;)
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&period_timer) ||
		                         ev == sensors_event ||
		                         ev == occupied_event ||
		                         ev == empty_event);

		if (ev == sensors_event) {
			printf("button pressed\n");
		}
		else if (ev == occupied_event) {
			printf("OCCUPIED EVENT\n");

			sensor_status = 1;
			send_status_packet(sensor_status);
		}
		else if (ev == empty_event) {
			sensor_status = 0;
			send_status_packet(sensor_status);
		}

		/* Send periodic status packet */
		if (etimer_expired(&period_timer)) {
			/* Reset periodic timer */
			etimer_set(&period_timer, beacon_interval);
			/* Send packet */
			send_status_packet(sensor_status);
		}
	}

	PROCESS_END();
}
