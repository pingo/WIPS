#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#include "contiki.h"
#include "net/rime.h"
#include "net/rime/netflood.h"

int nf_recv(struct netflood_conn *, const rimeaddr_t *, const rimeaddr_t *, uint8_t, uint8_t);
void nf_sent(struct netflood_conn *);
void nf_dropped(struct netflood_conn *);

#endif

