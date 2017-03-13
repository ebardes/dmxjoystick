#include "mac.h"
#include "timer.h"

#include <boost/thread/thread.hpp>

timer::timer(int hz)
{
	running = true;
}

void timer::run(void)
{
	while (running)
	{
		boost::this_thread::sleep_for(boost::chrono::milliseconds(200));

		printf("tick\n");
	}
}
