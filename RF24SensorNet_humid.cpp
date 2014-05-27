/*
  RF24SensorNet
  A library for sending standard sensor and actuator
  messages between nodes connected using RF24Network.

  Handling for temperature sensors.

  -- 
  Peter Hardy <peter@hardy.dropbear.id.au>
*/
#ifndef __RF24_SENSOR_NET_HUMID_H__
#define __RF24_SENSOR_NET_HUMID_H__

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include "RF24Network.h"
#include "RF24SensorNet.h"
#include "RF24SensorNet_types.h"

void RF24SensorNet::addHumidReadHandler(humidReadHandler handler)
{
  _humidReadHandler = handler;
}

void RF24SensorNet::addHumidRcvHandler(humidRcvHandler handler)
{
  _humidRcvHandler = handler;
}

bool RF24SensorNet::readHumid(uint16_t toAddr, uint16_t id)
{
  pkt_humid_t payload;
  payload.id = id;
  payload.humidity = 0;

  bool ok = _write(toAddr, PKT_TEMP+32, &payload);
  return ok;
}

bool RF24SensorNet::sendHumid(uint16_t toAddr, uint16_t id, uint16_t humidity)
{
  pkt_humid_t payload;
  payload.id = id;
  payload.humidity = humidity;
  bool ok = _write(toAddr, PKT_HUMID, &payload);
  return ok;
}
#endif
