#include "mac.h"
#include <linux/joystick.h>

js::js()
{
	memset(this, 0, sizeof(*this));
	joy_fd = -1;
}

js::~js()
{
	if (joy_fd >= 0)
	{
		::close(joy_fd);
	}
}

void js::close()
{
	if (joy_fd >= 0)
	{
		::close(joy_fd);
		joy_fd = -1;
	}
}

bool js::open(const char *JOY_DEV)
{

	if((joy_fd = ::open(JOY_DEV, O_RDONLY)) < 0)
	{
		return false;
	}

	::ioctl(joy_fd, JSIOCGAXES, &num_of_axis);				//	GET THE NUMBER OF AXIS ON JS
	::ioctl(joy_fd, JSIOCGBUTTONS, &num_of_buttons);		//	GET THE NUMBER OF BUTTONS ON THE JS
	::ioctl(joy_fd, JSIOCGNAME(80), &name_of_joystick);	//	GET THE NAME OF THE JS

	// ::fcntl(joy_fd, F_SETFL, O_NONBLOCK);

	return true;
}

bool js::read()
{
	js_event js;

	::read(joy_fd, &js, sizeof(struct js_event));

	switch (js.type & ~JS_EVENT_INIT)
	{
		case JS_EVENT_AXIS:
			axis   [ js.number ] = js.value;
			break;

		case JS_EVENT_BUTTON:
			button [ js.number ] = js.value;
			break;
	}

	return true;
}

bool js::okay()
{
	return joy_fd >= 0;
}
