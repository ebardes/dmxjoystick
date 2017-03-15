#include "mac.h"
#include "timer.h"
#include <curses.h>

void dumpBuffer(int row, const uint8_t*buffer, int length);

char *n(char *scratch, int val, int width)
{
	sprintf(scratch, "%*d ", width, val);
	return scratch;
}

int feature(int row, int col, dmxproperty &p)
{
	if (!p.defined)
		return col + 14;

	char scratch[100];
	attron(p.linked ? COLOR_PAIR(1) : COLOR_PAIR(3));
	mvaddstr(row, col, n(scratch, p.current, 6));
	attroff(p.linked ? COLOR_PAIR(1) : COLOR_PAIR(3));
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
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);

  mvaddstr(0, 24, "pan");
  mvaddstr(0, 38, "tilt");
  mvaddstr(0, 52, "intensity");
  mvaddstr(0, 66, "iris");

	int k = 21;
	mvaddstr(1, 0, "--Device -------");
	mvaddstr(1, k, "--Cur-"); k += 7;
	mvaddstr(1, k, "--Src-"); k += 7;
	mvaddstr(1, k, "--Cur-"); k += 7;
	mvaddstr(1, k, "--Src-"); k += 7;
	mvaddstr(1, k, "--Cur-"); k += 7;
	mvaddstr(1, k, "--Src-"); k += 7;
	mvaddstr(1, k, "--Cur-"); k += 7;
	mvaddstr(1, k, "--Src-"); k += 7;

	while (t.tick())
	{
		int i;

		for (i = 0; i < instance_count; i++)
		{
			instance &inst = instances[i];

			k = 0;
			attron(inst.joystick.okay() ? COLOR_PAIR(1) : COLOR_PAIR(2));
			mvaddstr(i+2, k, (char*) inst.joystick_device.c_str()); k += 21;
			attroff(inst.joystick.okay() ? COLOR_PAIR(1) : COLOR_PAIR(2));

			k = feature(i+2, k, inst.fix["pan"]);
			k = feature(i+2, k, inst.fix["tilt"]);
			k = feature(i+2, k, inst.fix["intensity"]);
			k = feature(i+2, k, inst.fix["iris"]);
			clrtoeol();
		}

		// clean up old messages
		while (messages.size() > 40)
		{
			messages.pop_back();
		}

		i += 3;
		for (std::string m : messages)
		{
			mvaddstr(i, 0, (char*) m.c_str());
			clrtoeol();
			i++;

			if (i >= LINES)
				break;
		}

		if (config.debug) dumpBuffer(i + 3, instances[0].out.getBuffer(), 512);

		mvaddstr(0,0, "");
		refresh();
	}
	endwin();
}

#define WIDTH 16
#define SPLIT 4

void dumpBuffer(int row, const uint8_t*buffer, int length)
{
	int p = 0;
	int i;
	uint8_t c;
	uint64_t offset;
	char buff1[64];
	char buff2[64];

	buff1[0] = '\0';
	offset = 0;
	for (i = 0; p < length; i++)
	{
		c = buffer[p++];

		buff2[i % WIDTH] = (c >= ' ' && c < 127) ? c : '.';
		sprintf(buff1 + strlen(buff1), "%02X ", c);
		if ((i % SPLIT) == SPLIT-1)
			strcat(buff1, " ");
		if ((i % WIDTH) == WIDTH-1)
		{
			buff2[WIDTH] = '\0';
			mvprintw(row++, 0, "%012llX: %-52s|  %s\n", offset, buff1, buff2);
			buff1[0] = '\0';
			offset += WIDTH;
		}
	}
	buff2[i % WIDTH] = '\0';
	mvprintw(row, 0, "%012llX: %-52s|  %s\n", offset, buff1, buff2);
}

