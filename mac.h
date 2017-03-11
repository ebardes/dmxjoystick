#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/timeb.h>
// #include <curses.h>
#include <fcntl.h>

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
	bool open(const char *);
	void tick();
};