#include "mac.h"
#include "timer.h"
#include <curses.h>

char *n(char *scratch, int val, int width)
{
	sprintf(scratch, "%*d", width, val);
	return scratch;
}

int feature(int row, int col, dmxproperty &p)
{
	char scratch[100];
	attron(p.linked ? COLOR_PAIR(1) : COLOR_PAIR(2));
	mvaddstr(row, col, n(scratch, p.current, 6));
	attroff(p.linked ? COLOR_PAIR(1) : COLOR_PAIR(2));
	col += 7;

	mvaddstr(row, col, n(scratch, p.source, 6));
	col += 7;

	return col;
}

void display::run(void)
{
	timer t(50);

	initscr();
	start_color();

	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);

  mvaddstr(0, 24, "pan");
  mvaddstr(0, 38, "tilt");
  mvaddstr(0, 52, "intensity");

	int k = 21;
	mvaddstr(1, 0, "--Device -------");
	mvaddstr(1, k, "--Cur-"); k += 7;
	mvaddstr(1, k, "--Src-"); k += 7;
	mvaddstr(1, k, "--Cur-"); k += 7;
	mvaddstr(1, k, "--Src-"); k += 7;
	mvaddstr(1, k, "--Cur-"); k += 7;
	mvaddstr(1, k, "--Src-"); k += 7;

	while (t.tick())
	{
		for (int i = 0; i < instance_count; i++)
		{
			instance &inst = instances[i];

			k = 0;
			attron(inst.joystick.okay() ? COLOR_PAIR(1) : COLOR_PAIR(2));
			mvaddstr(i+2, k, inst.joystick_device.c_str()); k += 21;
			attroff(inst.joystick.okay() ? COLOR_PAIR(1) : COLOR_PAIR(2));
			k = feature(i+2, k, inst.fix.pan);
			k = feature(i+2, k, inst.fix.tilt);
			k = feature(i+2, k, inst.fix.intensity);
		}

		mvaddstr(0,0, "");
		refresh();
	}
	endwin();
}
