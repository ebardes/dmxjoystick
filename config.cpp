#include "mac.h"

#include "boost/property_tree/xml_parser.hpp"

void config::read(const char *name)
{
	boost::property_tree::read_xml(name, c, boost::property_tree::xml_parser::trim_whitespace);

	input_universe = getInt("settings.input_universe");
	output_universe = getInt("settings.output_universe");

	joystick_device = c.get<std::string>("settings.joystick.device");

	pan.define(
		getInt("settings.pan.<xmlattr>.offset"),
		getInt("settings.pan.<xmlattr>.size"),
		getInt("settings.pan.<xmlattr>.order")
	);
	tilt.define(
		getInt("settings.tilt.<xmlattr>.offset"),
		getInt("settings.tilt.<xmlattr>.size"),
		getInt("settings.tilt.<xmlattr>.order")
	);
	intensity.define(
		getInt("settings.intensity.<xmlattr>.offset"),
		getInt("settings.intensity.<xmlattr>.size"),
		getInt("settings.intensity.<xmlattr>.order")
	);
}

void config::save(const char *name)
{
	boost::property_tree::xml_writer_settings<char> settings(' ', 4);
	boost::property_tree::write_xml(name, c, std::locale(), settings);
}
