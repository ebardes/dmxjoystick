#include "mac.h"

#if USE_OLA

#include <ola/Logging.h>
#include <ola/Callback.h>

using std::cout;
using std::endl;

bool eth::read(instance *in)
{
	this->in = in;
	wrapper.GetSelectServer()->Run();
	return true;
}

bool eth::write(void)
{
	wrapper.GetClient()->SendDMX(universe, buffer, ola::client::SendDMXArgs());
	return true;
}

void RegisterComplete(const ola::client::Result& result) {
	if (!result.Success()) {
		OLA_WARN << "Failed to register universe: " << result.Error();
	}
}
void eth::onframe(const ola::client::DMXMetadata &metadata, const ola::DmxBuffer &data)
{
	buffer.Set(data);
	in->newData(*this);
}

bool eth::openRead(int universe)
{
	if (!wrapper.Setup())
	{
		std::cerr << "Setup failed" << endl;
		return false;
	}

	this->universe = universe;
	
	ola::client::OlaClient *client = wrapper.GetClient();
	client->SetDMXCallback(ola::NewCallback(this, &eth::onframe));
	client->RegisterUniverse(universe, ola::client::REGISTER, ola::NewSingleCallback(&RegisterComplete));
	  
	return true;
}

bool eth::openWrite(int universe)
{
	if (!wrapper.Setup())
	{
		std::cerr << "Setup failed" << endl;
		return false;
	}

	this->universe = universe;
	return true;
}

eth::eth(void)
{
	ola::InitLogging(ola::OLA_LOG_INFO, ola::OLA_LOG_STDERR);
	buffer.Blackout();
}

#endif
