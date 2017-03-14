#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <map>

#include <boost/property_tree/ptree.hpp>

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

/**
 * The sACN class
 */
class eth
{
	int eth_fd;
	int sequence;

	struct sockaddr_in sin;

	struct E131_2009 frame;
public:
	eth() : eth_fd(-1), sequence(0) {}
	bool openRead(int universe);
	bool openWrite(int universe);
	bool read(void);
	bool write(void);

	inline uint8_t* getBuffer() { return frame.dmx_data; }
	inline void copyFrom(eth &e) { memcpy(frame.dmx_data, e.frame.dmx_data, sizeof(frame.dmx_data)); }
};

/**
 * 
 */
class dmxproperty
{
	std::string name;
	int offset;
	int size;
	int order;

	int min;
	int max;
	float speed;
public:
	bool linked;
	bool defined;
	int current;
	int source;

	dmxproperty(const dmxproperty &);
	dmxproperty() : linked(true), defined(false) {}
	void define(boost::property_tree::ptree &node);

	int get(eth &eth);
	void put(eth &eth, int value);
	int updateSource(eth &eth);
	void putBuffer(eth &eth);

	friend std::ostream& operator << (std::ostream&, const dmxproperty &);
};

/**
 * Display manager
 */
class display
{
public:
	void run(void);
};

/**
 * The fixture class
 */
class fixture
{
	std::map<std::string, dmxproperty*> properties;
public:
	void addDefinition(std::string &name, boost::property_tree::ptree &node);

	dmxproperty &operator[](const char *);

	void updateSource(eth &eth);
	void putBuffer(eth &eth);

	friend std::ostream& operator << (std::ostream&, const fixture &);
};

/**
 * A control instance.
 * Contains a DMX input, a DMX output, a fixture and a joystick.
 */
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

/**
 * Configuration management
 */
class config
{
	boost::property_tree::ptree c;

public:
	bool debug;
	bool xdebug;

	void read(const char *name);
	void save(const char *name);
};

extern instance instances[20];
extern int instance_count;
extern config config;
