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

	if(s.count("pan") > 0) i.fix.pan.define(s.get_child("pan"));
	if(s.count("tilt") > 0) i.fix.tilt.define(s.get_child("tilt"));
	if(s.count("iris") > 0) i.fix.iris.define(s.get_child("iris"));
	if(s.count("intensity") > 0) i.fix.intensity.define(s.get_child("intensity"));
}

void config::save(const char *name)
{
	boost::property_tree::xml_writer_settings<char> settings(' ', 4);
	boost::property_tree::write_xml(name, c, std::locale(), settings);
}

void dmxproperty::define(boost::property_tree::ptree &node)
{
	size = 1;
	order = 1;

	boost::property_tree::ptree &attr = node.get_child("<xmlattr>");

	offset = attr.get<int>("offset") - 1;

	if (attr.count("size") > 0)
		size = attr.get<int>("size");

	if (attr.count("order") > 0)
		order = attr.get<int>("order");

	if (attr.count("min") > 0)
		min = attr.get<int>("min");
	else
		min = (size == 1) ? 0 : -32767;

	if (attr.count("max") > 0)
		max = attr.get<int>("max");
	else
		max = (size == 1) ? 255 : 32767;

	defined = true;
}
