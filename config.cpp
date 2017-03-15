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

	BOOST_FOREACH(pt::ptree::value_type &v, j) {
		if (v.first == "map")
			i.joystick.map(i.fix, v.second);
	}

	i.joystick_device = j.get<std::string>("<xmlattr>.device");
}

/*
void config::save(const char *name)
{
	pt::xml_writer_settings<char> settings(' ', 4);
	pt::write_xml(name, c, std::locale(), settings);
}
*/

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

void js::map(fixture &fix, pt::ptree &node)
{
	pt::ptree &attr = node.get_child("<xmlattr>");
	std::string link = attr.get<std::string>("link");
	std::string type = attr.get<std::string>("type");
	int number = attr.get<int>("number");

	if (type == "button")
	{
		fix[link].button = & buttons[number];
	}
	if (type == "analog")
	{
		analog &a = analogs[number];

		a.min = attr.get<int>("min");
		a.max = attr.get<int>("max");
		a.deadmin = attr.get<int>("deadmin");
		a.deadmax = attr.get<int>("deadmax");
		a.scale = attr.get<int>("scale");

		fix[link].analog = &a;
	}
}
