#include "mac.h"

#include <boost/detail/endian.hpp>

const uint8_t raw_acn_packet[sizeof(struct E131_2009)] = {
#include "acnraw.h"
};

int eth_fd = -1;

static uint32_t addr = (239 << 24) | (255 << 16); // ACN Ethernet address 239.255.x.x

eth::eth()
{
	sequence = 0;
	memcpy((void*)&frame, (const void*)raw_acn_packet, sizeof(frame));
}

void eth::ethCommon(void)
{
	if (eth_fd < 0)
	{
		if ((eth_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			perror("cannot create socket");
			return;
		}

		memset((char *)&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
		sin.sin_port = htons(ACN_PORT);

		if (bind(eth_fd, (struct sockaddr *) &sin, sizeof(sin)) < 0)
		{
			perror("bind error");
			return;
		}
	}
}

bool eth::openRead(int universe)
{
	ethCommon();
	if (eth_fd < 0)
			return false;

	struct ip_mreq mreq;
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = htonl(addr | universe);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(eth_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		perror("setsockopt");
		return false;
	}

	this->universe = universe;
	return true;
}

bool eth::openWrite(int universe)
{
	ethCommon();
	if (eth_fd < 0)
			return false;

	this->universe = universe;
	memset(&sin, 0, sizeof(sin));
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
	if (nbytes < 0)
	{
		std::string m("Packet send error: fd=");
		m += std::to_string(eth_fd);
		m += ", errno=";
		m += std::to_string(errno);
		m += " ";
		m += strerror(errno);

		disp.message(m);
	}

	return nbytes > 0;
}

bool eth::write(void)
{
	if (eth_fd >= 0)
	{
		frame.universe[0] = (universe >> 8);
		frame.universe[1] = (universe & 0xff);
		frame.seq_num[0]++;
		frame.priority[0] = (uint8_t) 100;

		int nbytes = sendto(eth_fd, &frame, sizeof(frame), 0, (const sockaddr*)&sin, sizeof(sin));
		if (nbytes < 0)
		{
			std::string m("Packet send error: fd=");
			m += std::to_string(eth_fd);
			m += ", errno=";
			m += std::to_string(errno);
			m += " ";
			m += strerror(errno);

			disp.message(m);
		}
		return nbytes > 0;
	}
	return false;
}

/*
 ******************************************************************************
 ******************************************************************************
 */

/*
 ******************************************************************************
 * DMX Property methods
 ******************************************************************************
 */
void dmxproperty::updateSource(eth &eth)
{
	if (defined)
	{
		int x = get(eth);
		if (linked)
		{
			current = x;
		}
		else if (fader.running())
		{
			if (source != x) // the console changed the target value
				release(); // recalculate the fade
		}
		source = x;
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

void dmxproperty::updateValues()
{
	if (fader.running())
	{
		current = fader.tick();
		if (current == source)
		{
			linked = true;
			fader.stop();
		}
	}

	if (analog)
	{
		int n = analog->tick();
		if (n)
		{
			fader.stop();
			linked = false;
			current += n;
			if (current < min)
				current = min;
			if (current > max)
				current = max;
		}
	}
}

void dmxproperty::release(void)
{
	if (!linked && current != source)
		fader.init(current, fadetime, source);
}
