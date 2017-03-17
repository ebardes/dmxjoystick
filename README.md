# dmxjoystick

This project is designed to use a common game controller to control an automated moving head light as a spotlight.
The basic principle of operation is it listens to a console on one sACN universe and emits a modified DMX frames on another universe.
It's up to the user how to convert those modified frames to the fixture.

## Building

This project makes the following assumptions about the working environment.

* A Linux environment. No effort has gone into making this work on Mac or Windows.
* A reasonable modern curses implementation.
* [Boost Libraries](http://www.boost.org/) are available. Boost is uses for threading and configuration information.
* A gamepad controller. The developer only has access to a Microsoft XBox 360 S controller. Others MAY work.
* A lighting console capable of generating sACN (a.k.a. E1.31) DMX packets.

On Debian (and probably Ubuntu)

```
$ sudo apt-get install libncurses5-dev libboost-thread-dev build-essential joystick
```

This is a coarse set of installation terms. You might be able to select finer grained installation targets to scale that back a bit.

### xpad drivers

I use Pavel Rojtberg's driver for gamepads. It uses the Dynamic Kernel Modules Support framework for installation.

```
$ sudo apt-get install dkms
```

Follow the installation directions at [https://github.com/paroj/xpad](https://github.com/paroj/xpad).

## Running

It's a simple app. By default, it looks for a `settings.xml` file in the current directory.
```
$ ./js [-c config-file.xml]
```

## Configuration File

This is a sample XML file. For my testing, I used a High End Systems Studio Spot 250 addressed at 301. There are no fixture libraries. You need to know the addresses of all the parameters you want to control.

The config file contains a `settings` element with `input_universe`, `output_universe`, `joystick`, and `fixtures` sub-elements. The order of these is not important.
```xml

<?xml version="1.0" encoding="utf-8"?>
<settings>
  <!--
	 Notes:
			 Byte Order: 1 = MSB, 0 = LSB
			 DMX Offsets start at 1
			 Universe Offsets start at 1
		-->
  <input_universe>1</input_universe>
  <output_universe>2</output_universe>
  <joystick device="/dev/input/js0">
    <map type="button" link="go" number="1"/>
    <map type="analog" link="pan" number="3" min="-32767" max="32767" deadmin="-2048" deadmax="2048" scale="16"/>
    <map type="analog" link="tilt" number="4" min="-32767" max="32767" deadmin="-2048" deadmax="2048" scale="-24"/>
    <map type="analog" link="intensity" number="0" deadmin="-3000" deadmax="3000" scale="-2000" />
  </joystick>
  <fixture address="301">
    <parameter name="intensity" offset="15" size="1" order="1"/>
    <parameter name="pan" offset="1" size="2" order="1"/>
    <parameter name="tilt" offset="3" size="2" order="1"/>
    <parameter name="iris" offset="13" size="1" order="1" min="0" max="132"/>
  </fixture>
</settings>
```

### Fixture Definition

The fixture defines what you want to control or monitor.  The fixture `address` attributes defines base addressing of the fixture. If omitted, the default is 1.
The fixture contains multiple parameter elements defining the parameters you want to control.
The `name` attribute is a arbitrary. You can call them anything. The `offset` attribute is the address of that parameter.
The `size` attribute declares whether that parameter is 1 byte or 2 bytes. The `order`, which is only relevant for multi-byte parameters says whether the bytes are ordered [1] MSB (big-endian; coarse to fine) or [0] LSB (little-endian; fine to coarse).
The `min` and `max` attributes define the range that controller will allow the parameter to go. This is useful to fence in the boundaries of a performance space. This can also be used to prevent some parameters from going into random effect values.

### Gamepad Mapping

The joystick map elements link gamepad controls to fixture attributes. The links are defined by the `link` attribute. The link name has to match a fixture attribute name. Analog joysticks are typically inaccurate.
The center is rarely 0. The `deadmin` and `deadmax` define the range of what is presumed to be zero. Any time that control is within that range, it presumes it's idle. The `scale` factor affects how fast to move the parameter.
Larger values adjust the parameter more slowly. *NOTE:* Negative values invert the direction.

## Contact
