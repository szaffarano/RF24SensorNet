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

Temperature devices are currently read-only.

Note that temperature and humidity readings are passed around on the
RF24Network as integers, multiplied by 10. 15.6 degrees will be
represented in the packet as `156`, and 43.5% humidity as `435`.

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

Before attempting to use it, you'll need to update `mqttBroker`,
`mqttClientID`, `mqttUsername` and `mqttPassword` to match your network.
They're towards the top of the sketch and commented.

The sketch registers as RF24Network device ID 0 - the root node. Any node
on the RF24 network may address a packet to 0, and if it's a valid
RF24SensorNet packet it will be relayed to MQTT.

#### MQTT packet format
Messages relayed from the RF24 network on to MQTT will be sent with a
topic `sensornet/out/*nodeAddress*/*packetType*`. For a message to be
relayed from MQTT in to the RF24 network, it should be sent with a topic
`sensornet/in/*nodeAddress*/*packetType*`.

`nodeAddress` is the RF24Network address of the source/destination node.
`packetType` is the sum of the device type and a command type. Device types
describe what class of device is sending or recieving this packet, and are
listed in the `pkt_type` enum in `RF24SensorNet_types.h`:

Type       | ID
-----------|---
PKT_INFO   | 0
PKT_POWER  | 1
PKT_SWITCH | 2
PKT_RGB    | 3
PKT_TEMP   | 4
PKT_HUMID  | 5

The command type specifies if this packet contains device data, or a
read or write request. These are:

Type                 | ID
---------------------|---
Device data          | 0
Device read request  | 32
Device write request | 64

For example, when the node at RF24 network address 012 sends a packet
about its current battery reading, it will be broadcast on MQTT with a
topic of `sensornet/out/12/1`. If an MQTT client wants to get a
temperature reading from the node at RF24 network address 03, it
should broadcast an MQTT message with a topic of `sensornet/in/3/36`.
And when an MQTT client would like to change the colour of a light
attached to a node at RF24 network address 021, it should use a topic of
`sensornet/in/21/67`.

For both inbound and outbound traffic, the MQTT payload should be the
contents of the appropriate packet type, in order, delineated with pipe
(`|`) characters. Again, these are described in `RF24SensorNet_types.h`.
Most types are numeric. For boolean types, use `1` for true and `0` for
false.

An MQTT command requesting the switch with device ID 3 be turned on will
have a payload of `3|1|0`. An MQTT packet with a temperature reading
of 27.3 degrees from device ID 0 will have a payload of `0|27.3`.