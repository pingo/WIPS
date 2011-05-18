#include "contiki.h"
#include "net/rime.h"
#include "net/rime/mesh.h"
#include "dev/leds.h"

#include "callbacks.h"
#include "proto.h"

#include <stdio.h>

PROCESS(sink_process, "Sink");
AUTOSTART_PROCESSES(&sink_process);

struct mesh_conn mesh;
const static struct mesh_callbacks callbacks = {
    .recv     = &cb_recv,
    .sent     = &cb_sent,
    .timedout = &cb_timedout,
  };

PROCESS_THREAD(sink_process, ev, data)
{
  PROCESS_EXITHANDLER(mesh_close(&mesh);)
  PROCESS_BEGIN();

  mesh_open(&mesh, 132, &callbacks);

  for(;;) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}

