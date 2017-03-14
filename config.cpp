#include "mac.h"

#include "boost/property_tree/xml_parser.hpp"

void config::read(const char *name)
{
	boost::property_tree::read_xml(name, c, boost::property_tree::xml_parser::trim_whitespace);

	boost::property_tree::ptree &s = c.get_child("settings");

	instance &i = instances[0];

	i.input_universe = s.get<int>("input_universe");
	i.output_universe = s.get<int>("output_universe");

	i.joystick_device = s.get<std::string>("joystick.device");

	i.fix.pan.define(
		s.get<int>("pan.<xmlattr>.offset"),
		s.get<int>("pan.<xmlattr>.size"),
		s.get<int>("pan.<xmlattr>.order")
	);
	i.fix.tilt.define(
		s.get<int>("tilt.<xmlattr>.offset"),
		s.get<int>("tilt.<xmlattr>.size"),
		s.get<int>("tilt.<xmlattr>.order")
	);
	i.fix.intensity.define(
		s.get<int>("intensity.<xmlattr>.offset"),
		s.get<int>("intensity.<xmlattr>.size"),
		s.get<int>("intensity.<xmlattr>.order")
	);
}

void config::save(const char *name)
{
	boost::property_tree::xml_writer_settings<char> settings(' ', 4);
	boost::property_tree::write_xml(name, c, std::locale(), settings);
}
