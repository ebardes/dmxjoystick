#include "mac.h"


int main(int argc, char **argv)
{
	// js j;

	// j.open("/dev/input/js0");
	

	eth e;

	e.openRead(1);
	while (e.read())
		e.dump();
	return 0;
}