#include "proto.h"

int proto_p_type(char *buf)
{
  return *buf & 0x3F;
}

void proto_ack_pack(char *buf, int seq_flag, int retries)
{
  buf[0] = (P_TYPE_ACK & 0x3F) | ((seq_flag != 0) << 6) | (retries & 0x1) << 7;
  buf[1] = (retries >> 1);
}

void proto_ack_unpack(char *buf, int *seq_flag, int *retries)
{
  *seq_flag =  (buf[0] & (1 << 6));
  *retries  = ((buf[0] & (1 << 7)) << 7) | (buf[1] & 0x7F);
}

void proto_status_pack(char *buf, int seq_flag, int retries, int payload)
{
  buf[0] = (P_TYPE_STATUS & 0x3F) | ((seq_flag != 0) << 6) | (retries & 0x1) << 7;
  buf[1] = (retries >> 1) | ((payload != 0) << 7);
}

void proto_status_unpack(char *buf, int *seq_flag, int *retries, int *payload)
{
  *seq_flag =  (buf[0] & (1 << 6));
  *retries  = ((buf[0] & (1 << 7)) << 7) | (buf[1] & 0x7F);
  *payload  =  (buf[1] & (1 << 7));
}

