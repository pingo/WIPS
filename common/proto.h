#ifndef _PROTO_H
#define _PROTO_H

#define P_TYPE_STATUS              0x01
#define P_TYPE_ACK                 0x02
#define P_TYPE_SET_DELTA           0x0A
#define P_TYPE_SET_SENSOR_INTERVAL 0x0B
#define P_TYPE_SET_SENSOR_TIMEOUT  0x0C
#define P_TYPE_SET_BEACON_INTERVAL 0x0D

#include <stdint.h>

int proto_p_type(char *buf);

/* Status packet. */
void proto_status_pack(char *buf, int seq_flag, int retries, int payload);
void proto_status_unpack(char *buf, int *seq_flag, int *retries, int *payload);

/* ACK packet */
void proto_ack_pack(char *buf, int seq_flag, int retries);
void proto_ack_unpack(char *buf, int *seq_flag, int *retries);

/* Set packet */
void proto_set_pack(char *buf, int p_type, uint16_t payload);
void proto_set_unpack(char *buf, uint16_t *payload);

#endif

