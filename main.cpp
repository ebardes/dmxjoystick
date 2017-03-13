#include "mac.h"
#include <boost/thread.hpp>
#include <curses.h>
#include "timer.h"

config c;

void display_thread()
{
	char numbuf[500];

	initscr();

	endwin();
}

void netin_thread()
{
	eth e;
	e.openRead(c.input_universe);
	while (e.read())
	{
		std::cout <<
			c.pan.get(e) <<
			", " <<
			c.tilt.get(e) <<
			", " <<
			c.intensity.get(e) <<
			std::endl;
	}
}

int main(int argc, char **argv)
{
	// js j;
	// j.open("/dev/input/js0");
	

//	eth e;
//	e.openRead(1);
//	while (e.read())
//		e.dump();

//	timer t(50);

//	t.run();

	c.read("settings.xml");
//	c.save("settings.xml");

	boost::thread disp{display_thread};
	boost::thread net_in{netin_thread};

	disp.join();
	net_in.join();
	return 0;
}
