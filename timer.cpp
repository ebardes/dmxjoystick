#include "mac.h"
#include "timer.h"

#include <boost/thread/thread.hpp>

timer::timer(int hz)
{
	running = true;
	sleep = hz;
}

bool timer::tick(void)
{
	if (!running)
		return false;

	boost::this_thread::sleep_for(boost::chrono::milliseconds(sleep));
	return true;
}
