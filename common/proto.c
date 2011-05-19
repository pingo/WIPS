#include "proto.h"

#include <stdint.h>

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
  *seq_flag =  (buf[0] & (1 << 6)) ? 1 : 0;
  *retries  = ((buf[0] & (1 << 7)) >> 7) | ((buf[1] << 1) & 0xFE);
}

void proto_status_pack(char *buf, int seq_flag, int retries, int payload)
{
  buf[0] = (P_TYPE_STATUS & 0x3F) | ((seq_flag != 0) << 6) | (retries & 0x1) << 7;
  buf[1] = (retries >> 1) | ((payload != 0) << 7);
}

void proto_status_unpack(char *buf, int *seq_flag, int *retries, int *payload)
{
  *seq_flag =  (buf[0] & (1 << 6)) ? 1 : 0;
  *retries  = ((buf[0] & (1 << 7)) >> 7) | ((buf[1] << 1) & 0xFE);
  *payload  =  (buf[1] & (1 << 7)) ? 1 : 0;
}

void proto_set_pack(char *buf, int p_type, uint16_t payload)
{
  buf[0] = (p_type & 0x3F);
  buf[1] = (payload >> 8) & 0xFF;
  buf[2] = payload & 0xFF;
}

void proto_set_unpack(char *buf, uint16_t *payload)
{
  *payload = (buf[2] & 0xFF) | (buf[1] & 0xFF) << 8;
}

