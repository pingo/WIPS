#include "contiki.h"
#include "net/rime.h"
#include "net/rime/netflood.h"

#include "callbacks.h"

#include <stdio.h>

int nf_recv(struct netflood_conn *c, const rimeaddr_t *from, const rimeaddr_t *originator, uint8_t seqno, uint8_t hops)
{
	char *ptr = packetbuf_dataptr();

	printf("%d.%d %d %d.%d %d %d\r\n",
		originator->u8[0],
		originator->u8[1],
		*ptr ? 1 : 0,
		from->u8[0],
		from->u8[1],
		seqno,
		hops);

	return 0;
}

void nf_sent(struct netflood_conn *c)
{
	printf("packet sent\r\n");
}

void nf_dropped(struct netflood_conn *c)
{
	printf("packet dropped\r\n");
}

