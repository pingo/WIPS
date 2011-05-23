# WIPS - Wireless Indoor Presence Sensing

WIPS means "Wireless Indoor Presence Sensing" and is a small project
performed within the course "Wireless Communication and Networked
embedded systems" at Uppsala University, Sweden during spring 2011.

The general idea is to create a simple system that reacts to movement
(or presence) and transmits presence information to a central sink
node. The sink communicates with a regular PC which in turn hosts a
web server that publish presence information. The central server can
also control various parts of the environment such as turning lights
on and off using a Tellstick and wireless relays.

## Hardware

The hardware platform consists of five [Zolertia
Z1](http://www.zolertia.com/products/Z1)s. One acts as a central sink
connected to a PC via USB, the other four nodes acts as sensor nodes
using either a light sensor (the
[1127](http://www.phidgets.com/products.php?category=1&product_id=1127))
or a motion sensor (the
[1111](http://www.phidgets.com/products.php?category=1&product_id=1111)).
Communication is performed using the
[CC2420](http://focus.ti.com/docs/prod/folders/print/cc2420.html) IEEE
802.15.4 radio present on the Z1.

For light control, a Tellstick from [Telldus](http://www.telldus.se/)
is used in conjunction with a
[LYCR-300](http://www.nexa.se/LYCR300WDimmerextra.htm) wireless relay.

## Software components

WIPS uses the [Contiki](http://www.sics.se/contiki/) operating system
as the underlying operating system and its communication stack
Rime. Apart from Contiki, the following software components is used in
the WIPS project and are available in this repository:

`backend` contains a simple Python 2 program that reads data on a
serial port from the sink node and enters events into a Sqlite
database.

`common` contains the packet protocol, modified rime libraries and netstack configuration.

`frontend` contains a PHP frontend for the database. An example
lighttpd config is provided.

`node` contains the actual node software that polls sensors and handle
radio communication.

`sink` contains the actual sink software that communicates with the
sensor nodes and the base station PC.

`testapps` contains test programs for sampling data from phidgets and
testing different types of network architectures.

`tests` conatins unit tests.

`tools` contains some helper programs that aid development and
debugging.
