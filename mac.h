#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/timeb.h>
// #include <curses.h>
#include <fcntl.h>

#include "boost/property_tree/ptree.hpp"

#include "acn.h"

class js
{
	int joy_fd;
	int num_of_axis;
	int num_of_joysticks;
	int num_of_buttons;
	char name_of_joystick[80];

	int axis[50];
	int button[50];

public:
	js();
	~js();
	bool open(const char *);
	void close();
	void tick();
};

class eth
{
	int eth_fd;
	int sequence;

	struct E131_2009 frame;
public:
	eth() : eth_fd(-1), sequence(0) {}
	bool openRead(int universe);
	bool openWrite(int universe);
	bool read(void);
	void dump(void);

	inline const uint8_t* getBuffer() { return frame.dmx_data; }
};

class dmxproperty
{
	int offset;
	int size;
	int order;
public:
	void define(int offset, int size, int order)
	{
		this->offset = offset - 1;
		this->size = size;
		this->order = order;
	};

	int get(eth &eth);
};

class config
{
	boost::property_tree::ptree c;

	int getInt(const char *key) { return c.get<int>(key); }

public:
	int input_universe;
	int output_universe;

	std::string joystick_device;

	dmxproperty pan;
	dmxproperty tilt;
	dmxproperty intensity;

	void read(const char *name);
	void save(const char *name);
};
