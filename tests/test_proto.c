#include <stdio.h>
#include <assert.h>

#include "proto.h"

int main(void)
{
	char buf[3];
	int st;
	int pt;
	int i, it;
	uint16_t int_16;
	uint16_t pt_16;

	/*
	 * Test ack_pack:
	 */

	for (i = 0; i < 255; i++)
	{
		proto_ack_pack(buf, 0, i);
		proto_ack_unpack(buf, &st, &it);
		assert(i == it);
		assert(0 == st);

		proto_ack_pack(buf, 1, i);
		proto_ack_unpack(buf, &st, &it);
		assert(i == it);
		assert(1 == st);
	}

	/*
	 * Test status_pack:
	 */

	for (i = 0; i < 255; i++)
	{
		proto_status_pack(buf, 0, i, 0);
		proto_status_unpack(buf, &st, &it, &pt);
		assert(i == it);
		assert(0 == st);
		assert(0 == pt);

		proto_status_pack(buf, 0, i, 1);
		proto_status_unpack(buf, &st, &it, &pt);
		assert(i == it);
		assert(0 == st);
		assert(1 == pt);

		proto_status_pack(buf, 1, i, 0);
		proto_status_unpack(buf, &st, &it, &pt);
		assert(i == it);
		assert(1 == st);
		assert(0 == pt);

		proto_status_pack(buf, 1, i, 1);
		proto_status_unpack(buf, &st, &it, &pt);
		assert(i == it);
		assert(1 == st);
		assert(1 == pt);
	}

	/*
	 * Test set_pack:
	 */

	for (int_16 = 0; int_16 < 65535; int_16++)
	{
		proto_set_pack(buf, 0, int_16);
		proto_set_unpack(buf, &pt_16);
		assert(int_16 == pt_16);
	}

	puts("All OK");

	return 0;
}

