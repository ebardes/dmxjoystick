#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/timeb.h>
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
	bool read();
	bool okay();
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

	inline uint8_t* getBuffer() { return frame.dmx_data; }
	inline void copyFrom(eth &e) { memcpy(frame.dmx_data, e.frame.dmx_data, sizeof(frame.dmx_data)); }
};

class dmxproperty
{
	int offset;
	int size;
	int order;
public:
	bool linked;
	bool defined;
	dmxproperty() : linked(true), defined(false) {}
	int current;
	int source;

	void define(int offset, int size, int order)
	{
		this->offset = offset - 1;
		this->size = size;
		this->order = order;
		this->defined = true;
	};

	int get(eth &eth);
	void put(eth &eth, int value);
	inline int updateSource(eth &eth) { if (defined) { source = get(eth); if (linked) current = source; }}
};

class display
{
public:
	void run(void);
};

class fixture
{
public:
	dmxproperty pan;
	dmxproperty tilt;
	dmxproperty iris;
	dmxproperty intensity;
};

class instance
{
	void netreader();
public:
	int input_universe;
	int output_universe;

	std::string joystick_device;

	js joystick;

	eth in;
	eth out;
	fixture fix;

	void init(void);

	void runReader(void);
	void runWriter(void);
	void runJoystick(void);
};

class config
{
	boost::property_tree::ptree c;

public:

	void read(const char *name);
	void save(const char *name);
};

extern instance instances[20];
extern int instance_count;
