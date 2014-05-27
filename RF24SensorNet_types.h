/*
  RF24SensorNet
  A library for sending standard sensor and actuator
  messages between nodes connected using RF24Network.

  Custom types.

  -- 
  Peter Hardy <peter@hardy.dropbear.id.au>
*/
#ifndef __RF24_SENSOR_NET_TYPES_H__
#define __RF24_SENSOR_NET_TYPES_H__

#include <stdint.h>
#include <stdbool.h>

/* Defined packet types */
enum pkt_type {
  PKT_INFO   = 0, /* General info for this node */
  PKT_POWER  = 1, /* Power status for this node */
  PKT_SWITCH = 2, /* Digital (on/off) switches */
  PKT_RGB    = 3, /* RGB (dimmer) switches */
  PKT_TEMP   = 4, /* Temperature sensors */
  PKT_HUMID  = 5  /* Humidity sensors */
};

/* Packet containing info about this node's power supply. */
struct pkt_power_t {
  bool battery; /* Is this node battery powered? */
  bool solar; /* Does this node have a solar panel? */
  uint16_t vcc; /* Supply voltage */
  uint16_t vs; /* Voltage supplied by solar panel. 0 if not panel. */
};

/* Packet for an on-off switch attached to this node. */
struct pkt_switch_t {
  uint16_t id; /* ID for the switch, unique to this node. */
  bool state; /* Is the switch on? */
  uint32_t timer; /* Seconds remaining until timer deactivates switch.
		     0 for no timer. */
};

/* Packet for a dimmer or RGB light attached to this node. */
struct pkt_rgb_t {
  uint16_t id; /* ID for the switch, unique to this node. */
  char rgb[3]; /* 0-255 for RGB. Off is [0, 0, 0] */
  uint32_t timer; /* Seconds remaining until timer deactivates switch.
		     0 for no timer. */
};

/* Packet for readings from a temperature sensor. */
struct pkt_temp_t {
  uint16_t id; /* ID for the sensor, unique to this node. */
  int16_t temp; /* Temperature reading from the sensor, in 0.1 degree C. */
};

/* Packet for readings from a humidity sensor. */
struct pkt_humid_t {
  uint16_t id; /* ID for the sensor, unique to this node. */
  uint16_t humidity; /* Relative humidity from the sensor, in 0.1%. */
};

typedef void (*infoReadHandler)(uint16_t fromAddr);
typedef void (*infoWriteHandler)(uint16_t fromAddr, uint16_t newAddr);
typedef void (*infoRcvHandler)(uint16_t fromAddr);

typedef void (*powerReadHandler)(uint16_t fromAddr);
typedef void (*powerRcvHandler)(uint16_t fromAddr, bool battery, bool solar,
			       uint16_t vcc, uint16_t vs);

typedef void (*switchReadHandler)(uint16_t fromAddr, uint16_t id);
typedef void (*switchWriteHandler)(uint16_t fromAddr, uint16_t id,
				  bool state, uint32_t timer);
typedef void (*switchRcvHandler)(uint16_t fromAddr, uint16_t id,
				 bool state, uint32_t timer);

typedef void (*rgbReadHandler)(uint16_t fromAddr, uint16_t id);
typedef void (*rgbWriteHandler)(uint16_t fromAddr, uint16_t id,
				char rgb[3], uint32_t timer);
typedef void (*rgbRcvHandler)(uint16_t fromAddr, uint16_t id,
			      char rgb[3], uint32_t timer);

typedef void (*tempReadHandler)(uint16_t fromAddr, uint16_t id);
typedef void (*tempRcvHandler)(uint16_t fromAddr, uint16_t id, int16_t temp);

typedef void (*humidReadHandler)(uint16_t fromAddr, uint16_t id);
typedef void (*humidRcvHandler)(uint16_t fromAddr, uint16_t id,
				uint16_t humidity);
#endif
