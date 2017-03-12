#include "mac.h"
#include <sys/socket.h>
#include <netinet/in.h>

const uint8_t raw_acn_packet[sizeof(struct E131_2009)] = {
#include "acnraw.h"
};

static uint32_t addr = (239 << 24) | (255 << 16); // ACN Ethernet address 239.255.x.x

#define WIDTH 16
#define SPLIT 4

void dumpBuffer(const unsigned char *buffer, int length)
{
	int p = 0;
	int i;
	uint8_t c;
	uint64_t offset;
	char buff1[64];
	char buff2[64];

	buff1[0] = '\0';
	offset = 0;
	for (i = 0; p < length; i++)
	{
		c = buffer[p++];

		buff2[i % WIDTH] = (c >= ' ' && c < 127) ? c : '.';
		sprintf(buff1 + strlen(buff1), "%02X ", c);
		if ((i % SPLIT) == SPLIT-1)
			strcat(buff1, " ");
		if ((i % WIDTH) == WIDTH-1)
		{
			buff2[WIDTH] = '\0';
			printf("%012llX: %-52s|  %s\n", offset, buff1, buff2);
			buff1[0] = '\0';
			offset += WIDTH;
		}
	}
	buff2[i % WIDTH] = '\0';
	printf("%012llX: %-52s|  %s\n", offset, buff1, buff2);

}

bool eth::openRead(int universe)
{
	if ((eth_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("cannot create socket");
		return false;
	}

	struct sockaddr_in sin;

	memset((char *)&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(ACN_PORT);

	if (bind(eth_fd, (struct sockaddr *) &sin, sizeof(sin)) < 0)
	{
		perror("bind error");
		return false;
	}

	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = htonl(addr | universe);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(eth_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		perror("setsockopt");
		return false;
	}

	return true;
}

bool eth::read()
{
	int nbytes;

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	nbytes = recvfrom(eth_fd, &frame, sizeof(frame), 0, (struct sockaddr *) &addr, &addrlen);
}

void eth::dump()
{
	dumpBuffer((const unsigned char*) &frame.dmx_data, sizeof(frame.dmx_data));
}