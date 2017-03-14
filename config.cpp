#include "mac.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

namespace pt = boost::property_tree;
class config config;

void config::read(const char *name)
{
	pt::read_xml(name, c, pt::xml_parser::trim_whitespace);

	pt::ptree &s = c.get_child("settings");

	instance &i = instances[0];

	i.input_universe = s.get<int>("input_universe");
	i.output_universe = s.get<int>("output_universe");

	// if(s.count("pan") > 0) i.fix.pan.define(s.get_child("pan"));
	// if(s.count("tilt") > 0) i.fix.tilt.define(s.get_child("tilt"));
	// if(s.count("iris") > 0) i.fix.iris.define(s.get_child("iris"));
	// if(s.count("intensity") > 0) i.fix.intensity.define(s.get_child("intensity"));

	/*
	 * Iteration over the dmx parameters
	 */
	pt::ptree &f = s.get_child("fixture");
	BOOST_FOREACH(pt::ptree::value_type &v, f) {
		std::string name = v.second.get<std::string>("<xmlattr>.name");

		i.fix.addDefinition(name, v.second);
	}

	/*
	 * Joystick stuff
	 */
	pt::ptree &j = s.get_child("joystick");
	i.joystick_device = j.get<std::string>("device");

	BOOST_FOREACH(pt::ptree::value_type &v, j.get_child("map")) {
	//	std::cout << v.second.data() << std::endl;
	}
}

void config::save(const char *name)
{
	pt::xml_writer_settings<char> settings(' ', 4);
	pt::write_xml(name, c, std::locale(), settings);
}

void dmxproperty::define(pt::ptree &node)
{
	size = 1;
	order = 1;

	pt::ptree &attr = node.get_child("<xmlattr>");
	name = attr.get<std::string>("name");

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

void fixture::addDefinition(std::string& name, pt::ptree &p)
{
	dmxproperty *x = new dmxproperty();
	x->define(p);

	properties.insert(std::pair<std::string, dmxproperty*>(name, x));
}

