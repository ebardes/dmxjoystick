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

#ifdef __linux__
#include <linux/joystick.h>
#endif

#include <map>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include "acn.h"

#define USE_OLA 1
#define USE_ACN 0

class instance;
class fixture;

/**
 * The sACN class
 */
#if USE_ACN
class eth
{
	int universe;
	int sequence;

	struct sockaddr_in sin;
	struct E131_2009 frame;

	void ethCommon(void);
public:
	eth();
	bool openRead(int universe);
	bool openWrite(int universe);
	bool read(void);
	bool write(void);

	inline uint8_t* getBuffer() { return frame.dmx_data; }
	inline void copyFrom(eth &e) { memcpy(frame.dmx_data, e.frame.dmx_data, sizeof(frame.dmx_data)); }
};
#endif
#ifdef USE_OLA

#include <ola/DmxBuffer.h>
class eth
{
	ola::DmxBuffer buffer;
public:
	eth();
	bool openRead(int universe);
	bool openWrite(int universe);
	bool read(void);
	bool write(void);

	inline const uint8_t* getBuffer() { return buffer.GetRaw(); }
	inline void copyFrom(eth &e) { buffer.Set(e.buffer); }
};
#endif

class action
{
};

#define MAX_AXIS 20
class analog
{
	int current;
public:
	int min, max, deadmin, deadmax;
	int scale;
	void map(int);
	int tick();
};

class button
{
	int current;
public:
	void map(int);
	bool pushed() { if (current > 0) { current = 0; return true;  } else { return false; }}
};

class js
{
	int joy_fd;
	int num_of_axis;
	int num_of_joysticks;
	int num_of_buttons;
	char name_of_joystick[120];

	analog analogs[MAX_AXIS];
	button buttons[MAX_AXIS];

public:
	js();
	~js();
	bool open(const char *);
	void close();
	bool read();
	bool okay();

	void map(instance &inst, fixture &fix, boost::property_tree::ptree &node);
};

class fade
{
	int *points;
	int npoints;
	int p;
	bool fading;
public:
	int target;
	int start;
	fade() : points(NULL), npoints(0), fading(false), target(0) {};
	~fade();
	void init(int y1, int time, int y2);
	int tick(void);
	bool running() { return fading; }
	void stop(void) { fading = false; p = start = target = 0; }
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
	int fadetime;
	float speed;
	fade fader;
public:
	bool linked;
	bool defined;
	int current;
	int source;

	class analog *analog;
	class button *button;

	dmxproperty() : linked(true), defined(false), current(0), source(0), analog(NULL), button(NULL) {}
	void define(boost::property_tree::ptree &node, int baseAddress = 0);

	int get(eth &eth);
	void put(eth &eth, int value);
	void updateSource(eth &eth);
	void putBuffer(eth &eth);
	void updateValues(void);
	void fadeTo(int value, int time);
	void release();

	friend std::ostream& operator << (std::ostream&, const dmxproperty &);
};

/**
 * Display manager
 */
class display
{
	std::vector<std::string> messages;
	bool quit;
public:
	void run(void);

	void message(std::string msg) { messages.insert(messages.begin(), msg); }
	void message(const char *msg) { message(std::string(msg)); }

	void runKeyboard(void);
};

/**
 * The fixture class
 */
class fixture
{
public:
	std::map<std::string, dmxproperty*> properties;

	fixture() { release(); }
	~fixture();
	void addDefinition(std::string &name, boost::property_tree::ptree &node, int baseAddress);

	inline dmxproperty *operator[](std::string name) { return properties[name]; }
	inline dmxproperty *operator[](const char *n) { return properties[std::string(n)]; }

	inline bool has(std::string name) { return properties.count(name) > 0; }

	void updateSource(eth &eth);
	void putBuffer(eth &eth);
	void updateValues(void);
	void release(void);

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

	button *releaseButton;

	void init(void);

	void runReader(void);
	void runWriter(void);
	void runJoystick(void);

	void release(void) { fix.release(); }
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

extern display disp;
extern instance instances[20];
extern int instance_count;
extern config config;
