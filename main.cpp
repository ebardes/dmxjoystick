#include "mac.h"
#include <boost/thread.hpp>
#include "timer.h"

config c;

instance instances[20];
int instance_count;

void display_thread()
{
	display d;

	d.run();
	std::cout  << "Display Shutdown" << std::endl;
}

void instance::runReader()
{
	while (in.read())
	{
		fix.pan.updateSource(in);
		fix.tilt.updateSource(in);
		fix.intensity.updateSource(in);
	}
}

void instance::runWriter()
{
	timer t(200);
	while (t.tick())
	{
	}
}

void instance::runJoystick()
{
	joystick.open(joystick_device.c_str());
	while (joystick.read())
	{
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


int main(int argc, char **argv)
{
	c.read("settings.xml");
//	c.save("settings.xml");

	instance_count = 1;
	instances[0].init();

	boost::thread disp{display_thread};

	disp.join();
	return 0;
}
