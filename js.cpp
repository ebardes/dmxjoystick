#include "mac.h"

#ifdef __linux__
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
	::ioctl(joy_fd, JSIOCGNAME(sizeof(name_of_joystick)), &name_of_joystick);	//	GET THE NAME OF THE JS

	disp.message(name_of_joystick);

	return true;
}

bool js::read()
{
	js_event js;

	int x = ::read(joy_fd, &js, sizeof(struct js_event));

	if (js.type & JS_EVENT_INIT)
		return true;

	/*
	std::string m = "Event: ";
	m += std::to_string(js.value);
	m += " ";
	m += std::to_string(js.number);
	disp.message(m);
	*/

	switch (js.type & ~JS_EVENT_INIT)
	{
		case JS_EVENT_AXIS:
			analogs[js.number].map(js.value);
			break;

		case JS_EVENT_BUTTON:
			buttons[js.number].map(js.value);
			break;
	}

	return true;
}

bool js::okay()
{
	return joy_fd >= 0;
}

void analog::map(int value)
{
	if (!scale)
		return;

	if (value > deadmin && value < deadmax)
	{
		value = 0;
	}
	else if (value <= deadmin)
	{
		value = (value - deadmin) / scale;
	}
	else
	{
		value = (value - deadmax) / scale;
	}

	current = value;
}

int analog::tick()
{
	return current;
}

void button::map(int value)
{
	current = value;
}

#endif
#ifdef __sun
js::js() {}
js::~js() {}
bool js::open(const char *def) { return true; }
bool js::read() { return false; }
bool js::okay() { return false; }
int analog::tick() { return false; }
#endif

