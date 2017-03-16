#include "mac.h"
#include <boost/thread.hpp>
#include "timer.h"

instance instances[20];
int instance_count;
display disp;

void display_thread()
{
	disp.run();
	std::cout  << "Display Shutdown" << std::endl;
}

void instance::runReader()
{
	while (in.read())
	{
		fix.updateSource(in);
		if (config.xdebug) {
			std::cout << fix << std::endl;
		}
	}
}

void instance::runWriter()
{
	timer t(20);
	while (t.tick())
	{
		out.copyFrom(in);
		fix.updateValues();
		fix.putBuffer(out);

		out.write();
	}
}

void instance::runJoystick()
{
	joystick.open(joystick_device.c_str());
	while (joystick.okay())
	{
		joystick.read();
	}
}

void instance::init(void)
{
	in.openRead(input_universe);
	out.openWrite(output_universe);

	boost::thread reader(boost::bind(&instance::runReader, this));
	boost::thread writer(boost::bind(&instance::runWriter, this));
	boost::thread stick(boost::bind(&instance::runJoystick, this));
}

void fixture::putBuffer(eth &e)
{
	for (std::pair<std::string, dmxproperty*> p : properties)
		p.second->putBuffer(e);
}

void fixture::updateValues()
{
	for (std::pair<std::string, dmxproperty*> p : properties)
		p.second->updateValues();
}

void fixture::updateSource(eth &e)
{
	if (config.xdebug) {
		std::cout << "Packet" << std::endl;
	}
	for (std::pair<std::string, dmxproperty*> p : properties)
		p.second->updateSource(e);
}

std::ostream& operator << (std::ostream& os, const dmxproperty& p)
{
	os << p.name << ":{off:" << (p.offset+1) << ",size:" << p.size << ",cur:" << p.current << ",src:" << p.source << ",defined:" << p.defined << "}";
	return os;
}

std::ostream& operator << (std::ostream& os, const fixture& f)
{
	bool first = true;
	os << "{";
	for (std::pair<std::string, dmxproperty*> p : f.properties)
	{
		if (first)
			first = false;
		else
			os << ",";
		os << *p.second;
	}
	os << "}";
	return os;
}

int main(int argc, char **argv)
{
	int c;
	const char * configfile = "settings.xml";

	// memset(&config, 0, sizeof(config));
	while ((c = getopt(argc, argv, "xdc:")) != -1)
	{
		switch (c)
		{
		case 'c':
			configfile = optarg;
			break;

		case 'd':
			config.debug = true;
			break;

		case 'x':
			config.debug = config.xdebug = true;
			break;
		}
	}
	config.read(configfile);
//	c.save("settings.xml");

	instance_count = 1;
	instances[0].init();

	if (config.debug)
		std::cout << instances[0].fix << std::endl;

	if (config.xdebug) {
		sleep(300);
	} else {
		boost::thread disp{display_thread};
		disp.join();
	}
	return 0;
}
