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

PROCESS(node_process, "node");
AUTOSTART_PROCESSES(&node_process);

int p_seq_flag = 0;
int p_retries = 0;

static struct mesh_conn mesh;
static rimeaddr_t sink_addr = { { 70, 0 } };
	
const static struct mesh_callbacks callbacks =
	{
		.recv    = &cb_recv,
		.sent    = &cb_sent,
		.timedout = &cb_timedout,
	};

#define SAMPLES 10
#define DELTA 1000

static void send_status_packet(int payload)
{
	/* Create a buffer for packet */
	char packet_buffer[2];
				
	/* Send packet to sink */
	proto_status_pack(packet_buffer, p_seq_flag, p_retries, payload);
	packetbuf_copyfrom(packet_buffer, 2);
	mesh_send(&mesh, &sink_addr);
}

PROCESS_THREAD(node_process, ev, data)
{
	static struct etimer period;
	static struct etimer presence_timeout;
	
	static int values[SAMPLES]; /* Buffer of SAMPLES latest values. */
	static int sample = 0;      /* Index of current sample in buffer. */
	
	static int i, v;
	static long avg;
	
	static int sensor_status = 0; /* 0 if not active (no one in room) */
	
	//memset(values, 0, SAMPLES*sizeof(uint8_t));
	
	PROCESS_EXITHANDLER(mesh_close(&mesh));
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(phidgets);

	mesh_open(&mesh, 132, &callbacks);
	
	/* Set timers to reduce startup conditions */
	etimer_set(&period, CLOCK_SECOND * 15);
	etimer_set(&presence_timeout, CLOCK_SECOND * 300);
	
	for (;;)
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&period) || etimer_expired(&presence_timeout) || ev == sensors_event);
		
		if (ev == sensors_event) {
			printf("button pressed");
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
			/* Reset the presence timer */
			etimer_set(&presence_timeout, CLOCK_SECOND * 300);
			/* Check if we should send packet (only if nobody was
			 * present earlier) */
			if (!sensor_status) {
				/* Send status packet */
				send_status_packet(1);
				/* Print debug */
				printf("v: %4d, avg: %4ld sensor_activated:  %i\n", v, avg, sensor_status);
			}
			sensor_status = 1;
		}
		
		/* Send periodic status packet */
		if (etimer_expired(&period)) {
			/* Reset periodic timer */
			etimer_set(&period, CLOCK_SECOND * 15);
			/* Send packet */
			send_status_packet(sensor_status);
		}
				
		/* Ok, nothing has happened for a long time. It seems like the
		 * room is unoccupied. */
		if (etimer_expired(&presence_timeout)) {
			/* Tell the sink that the room is unoccupied if it was
			 * occupied before */
			if (sensor_status) {
				send_status_packet(0);
			}
			sensor_status = 0;
		} 
	}
	PROCESS_END();
}
