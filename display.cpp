#include "mac.h"
#include "timer.h"
#include <curses.h>

void dumpBuffer(int row, const uint8_t*buffer, int length);

WINDOW *w;

char *n(char *scratch, int val, int width)
{
	sprintf(scratch, "%*d ", width, val);
	return scratch;
}

int feature(int row, int col, dmxproperty *p)
{
	if (p == NULL || !p->defined)
		return col + 15;

	char scratch[100];
	attron(p->linked ? COLOR_PAIR(1) : COLOR_PAIR(3));
	mvaddstr(row, col, n(scratch, p->current, 6));
	attroff(p->linked ? COLOR_PAIR(1) : COLOR_PAIR(3));
	col += 7;

	mvaddstr(row, col, n(scratch, p->source, 6));
	col += 8;

	return col;
}

void display::runKeyboard()
{
	int ch = getch();
	switch (ch)
	{
	case 'q':
		quit = true;
		break;

	case 'r':
		message("release");
		instances[0].release();
		break;
	}
}


void display::run(void)
{
	quit = false;
	timer t(100);

	w = initscr();
	cbreak();
	nodelay(w, TRUE);
	start_color();

	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);

	mvaddstr(1, 0, "--Device -------");

	std::vector<std::string> names;
	for (int i = 0; i < instance_count; i++)
	{
		instance &inst = instances[i];
		for (std::pair<std::string, dmxproperty*> p : inst.fix.properties)
		{
			names.push_back(p.first);
		}
	}

	int k = 21;
	for (std::string name : names)
	{
		mvaddstr(0, k + 3, name.c_str());

		mvaddstr(1, k, "--Cur-"); k += 7;
		mvaddstr(1, k, "--Src-"); k += 8;
	}

	while (!quit && t.tick())
	{
		int i = 0;
		while (::ioctl(0, FIONREAD, &i) >= 0 && i > 0)
		{
			runKeyboard();
		}

		for (i = 0; i < instance_count; i++)
		{
			instance &inst = instances[i];

			k = 0;
			attron(inst.joystick.okay() ? COLOR_PAIR(1) : COLOR_PAIR(2));
			mvaddstr(i+2, k, (char*) inst.joystick_device.c_str()); k += 21;
			attroff(inst.joystick.okay() ? COLOR_PAIR(1) : COLOR_PAIR(2));

			for (std::string name : names)
			{
				k = feature(i+2, k, inst.fix[name]);
			}
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

