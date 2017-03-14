
class timer
{
	bool running;
	int sleep;
public:
	timer(int hz);
	bool tick();
};
