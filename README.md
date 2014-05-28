# RF24SensorNet
This library defines a collection of sensor and actuator types, and
provides a convenient way to send data and commands to/from them using
RF24Network as a transport.

This library relies on the RF24Network library. I've tested against
[manicbug's original code](http://maniacbug.github.io/RF24Network/) and
regularly run it with the [TMRh20 fork](http://tmrh20.github.io/RF24Network/).

## Overview
The network consists of several nodes, each consisting of an
Arduino-compatible MCU and an nRF24L01 radio module. Each node can have
zero or more devices of the below types connected, each with a unique ID.

There are three basic message types:

* A request for data from a device. This request will contain the
  destination node address, the device type, and the device's ID.
* A request to send data to a device (for example, turn a switch on).
  The request will contain the destination node address, device type, and
  other information depending on the device.
* A device data packet. It will contain the destination node address,
  device type, and other information depending on the device. It can be
  sent in response to either of the above request messages, or on
  any type of schedule defined by the sending node.

All messages sent across the network are one-one.

## Status
The info device type is currently unimplemented. I'm intending to use
it to retrieve the status of all devices attached to a node, and use it
to set the node's RF24Network address.

The library is currently a fair bit heavier than it really needs to be -
it'll run on ATtiny processors, but some juggling might be required to get
a sketch in to their 8KB of flash. Optimizing code size and rearranging to
make it easier to add new device types is next on the list of things to do.

## Supported device types
* **info**. Used to retrieve information about a node, including the status
  of all attached devices.
* **power**. Used to retrieve information about a node's power supply.
  Can be used to show battery status, or how much power is being supplied
  by an attached solar panel.
* **switch**. A basic digital on/off switch. This can be used to switch
  relays attached to a node on and off, or a node can use it to report
  status of an attached switch.
* **rgb**.  Sends three values for red, green and blue channels. Used to
  control RGB lights, or fading lights.
* **temp**. A temperature sensor. Currently only supports reading/sending
  temperatures, so it's not (yet) suitable for controlling thermostats.
* **humid**. A humidity sensor. Sends relative humidity.

## Usage
### Sending messages
Each device type has three functions, to send each type of message.

* **read\*** sends a message requesting data from a device.
* **write\*** sends a message containing new data for devices that support
  being modified.
* **send\*** sends a message containing device data.

### Receiving messages
A node interested in receiving messages pertaining to a given device
type must implement one or more callback functions, and register them
with these functions:

* **add\*ReadHandler** registers a callback that will receive requests
  for device data.
* **add\*WriteHandler** registers a callback that will receive requests
  to change a device (not all devices support this).
* **add\*RcvHandler** registers a callback that will receive messages
  containing data from a device.

## Examples
* rxTest: A basic node with a single switch device controlling an LED.
  Load this on an arduino with nRF24L01 module, and attach an LED to pin 5.
* txTest: A basic node with no attached devices, which blinks a remote
  switch device on and off. Loat this on a second arduino with nRF24L01
  module, and it will switch the rxTest module on and off.

### MqttRelay
The MqttRelay sketch runs on an Ethernet-equipped Arduino. It registers
to an MQTT bus. It is designed to run with the Arduino's standard Ethernet
library, and uses [pubsubclient](https://github.com/knolleary/pubsubclient)
to talk to MQTT.

Before attempting to use it, you'll need to update mqttBroker,
mqttClientID, mqttUsername and mqttPassword to match your network. They're
towards the top of the sketch and commented.
