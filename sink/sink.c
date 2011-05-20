#include "contiki.h"
#include "net/rime.h"
#include "net/rime/mesh.h"
#include "dev/leds.h"
#include "dev/serial-line.h"

#include "callbacks.h"
#include "proto.h"

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

PROCESS(sink_process, "Sink");
PROCESS(command_process, "Command");
AUTOSTART_PROCESSES(&sink_process);

struct mesh_conn mesh;
const static struct mesh_callbacks callbacks =
	{
		.recv     = &cb_recv,
		.sent     = &cb_sent,
		.timedout = &cb_timedout,
	};

static char *getchar_ptr = NULL;

static int getchar(void)
{
	static char c;

	c = *getchar_ptr;

	if (c != '\0')
	{
		getchar_ptr++;
		return c;
	}
	else
		return EOF;
}

static uint8_t get_u8(void)
{
	static int c;
	static uint8_t v;

	v = 0;

	while (v < 256 && isdigit(c = getchar()))
	{
		v *= 10;
		v += c - '0';
	}

	getchar_ptr--;

	return v;
}

static uint16_t get_u16(void)
{
	static int c;
	static uint16_t v;

	v = 0;

	while (v < 65536 && isdigit(c = getchar()))
	{
		v *= 10;
		v += c - '0';
	}

	getchar_ptr--;

	return v;
}

PROCESS_THREAD(command_process, ev, data)
{
	static uint8_t major, minor;
	static int p_type;
	static int c;
	static char buf[3];
	static uint16_t payload;

	PROCESS_BEGIN();

	for (;;)
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == serial_line_event_message);
		getchar_ptr = data;

		major = get_u8();
		if (getchar() != '.')
			continue;

		minor = get_u8();
		if (getchar() != ':')
			continue;

		switch (c = getchar())
		{
			case 'd':
				p_type = P_TYPE_SET_DELTA;
				break;

			case 'i':
				p_type = P_TYPE_SET_SENSOR_INTERVAL;
				break;

			case 's':
				p_type = P_TYPE_SET_SENSOR_TIMEOUT;
				break;

			case 'b':
				p_type = P_TYPE_SET_BEACON_INTERVAL;
				break;

			default:
				continue;
		}

		rimeaddr_t addr = { { major, minor } };

		payload = get_u16();

		proto_set_pack(buf, p_type, payload);
		packetbuf_copyfrom(buf, 3);

		mesh_send(&mesh, &addr);
	}

	PROCESS_END();
}

PROCESS_THREAD(sink_process, ev, data)
{
	PROCESS_EXITHANDLER(mesh_close(&mesh);)
	PROCESS_BEGIN();

	mesh_open(&mesh, 132, &callbacks);

	process_start(&command_process, NULL);

	for (;;)
	{
		PROCESS_WAIT_EVENT();
	}

	PROCESS_END();
}

