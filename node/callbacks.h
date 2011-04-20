#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#include "contiki.h"
#include "net/rime.h"
#include "net/rime/mesh.h"

void cb_recv(struct mesh_conn *, const rimeaddr_t *, uint8_t);
void cb_sent(struct mesh_conn *);
void cb_timedout(struct mesh_conn *);

#endif

