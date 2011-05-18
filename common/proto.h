#ifndef _PROTO_H
#define _PROTO_H

#define P_TYPE_STATUS 0x01

int proto_p_type(char *buf);

/* Status packet. */
void proto_status_pack(char *buf, int seq_flag, int retries, int payload);
void proto_status_unpack(char *buf, int *seq_flag, int *retries, int *payload);

/* ACK packet */
void proto_ack_pack(char *buf, int seq_flag, int retries);
void proto_ack_unpack(char *buf, int *seq_flag, int *retries);

#endif

