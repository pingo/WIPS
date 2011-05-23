#ifndef _NODE_H
#define _NODE_H

#include <stdint.h>

/* Variables used for communication */
extern int p_seq_flag, p_retries;
extern struct mesh_conn mesh;
extern rimeaddr_t sink_addr;

/* Variables used for changing settings over-the-air */
extern uint16_t delta;
extern struct etimer empty_timer;
extern struct etimer interval_timer;
extern struct etimer period_timer;
extern clock_time_t sensor_interval;
extern clock_time_t sensor_timeout;
extern clock_time_t beacon_interval;

#endif
