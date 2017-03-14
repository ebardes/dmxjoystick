#include "mac.h"

#include <boost/detail/endian.hpp>

const uint8_t raw_acn_packet[sizeof(struct E131_2009)] = {
#include "acnraw.h"
};

static uint32_t addr = (239 << 24) | (255 << 16); // ACN Ethernet address 239.255.x.x

bool eth::openRead(int universe)
{
	if ((eth_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("cannot create socket");
		return false;
	}

	memset((char *)&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(ACN_PORT);

	if (bind(eth_fd, (struct sockaddr *) &sin, sizeof(sin)) < 0)
	{
		::close(eth_fd);
		eth_fd = -1;
		perror("bind error");
		return false;
	}

	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = htonl(addr | universe);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(eth_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		::close(eth_fd);
		eth_fd = -1;
		perror("setsockopt");
		return false;
	}

	return true;
}

bool eth::openWrite(int universe)
{
	if ((eth_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("cannot create socket");
		return false;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(addr | universe);
	sin.sin_port = htons(ACN_PORT);

	return true;
}

bool eth::read(void)
{
	int nbytes;

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	nbytes = recvfrom(eth_fd, &frame, sizeof(frame), 0, (struct sockaddr *) &addr, &addrlen);
}

bool eth::write(void)
{
	if (eth_fd >= 0)
	{
		frame.seq_num[0]++;
		sendto(eth_fd, &frame, sizeof(frame), 0, (const sockaddr*)&sin, sizeof(sin));
	}
}

/*
 ******************************************************************************
 ******************************************************************************
 */
dmxproperty::dmxproperty(const dmxproperty &x) :
	name(x.name),

	offset(x.offset),
	size(x.size),
	order(x.order),

	min(x.min),
	max(x.max),
	speed(x.speed),

	defined(x.defined),
	linked(x.linked),
	current(x.current),
	source(x.source)
{
}

dmxproperty &fixture::operator[](const char*key)
{
	std::string name(key);

	return *properties[name];
}

/*
 ******************************************************************************
 * DMX Property methods
 ******************************************************************************
 */
int dmxproperty::updateSource(eth &eth)
{
	if (defined)
	{
		source = get(eth);
		if (linked)
			current = source;
	}
}

void dmxproperty::putBuffer(eth &eth)
{
	if (defined)
	{
		put(eth, current);
	}
}

int dmxproperty::get(eth &eth)
{
	int value = 0;
	switch (size)
	{
	case 2:
		value = *(int16_t*) (eth.getBuffer() + offset);
		if (order == 1)
			value = ntohs(value);
		value -= 32768;
		break;

	case 1:
		value = (eth.getBuffer() [offset]);
		break;
	}
	return value;
}

void dmxproperty::put(eth &eth, int val)
{
	int value = val;
	switch(size)
	{
	case 2:
		value += 32768;
		if (order == 1)
			value = htons(value);
		*(int16_t*) (eth.getBuffer() + offset) = value;
		break;

	case 1:
		(eth.getBuffer() [offset]) = (uint8_t) value;
		break;
	}
}
