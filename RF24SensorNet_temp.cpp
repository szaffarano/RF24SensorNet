/*
  RF24SensorNet
  A library for sending standard sensor and actuator
  messages between nodes connected using RF24Network.

  Handling for temperature sensors.

  -- 
  Peter Hardy <peter@hardy.dropbear.id.au>
*/
#ifndef __RF24_SENSOR_NET_TEMP_H__
#define __RF24_SENSOR_NET_TEMP_H__

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include "RF24Network.h"
#include "RF24SensorNet.h"
#include "RF24SensorNet_types.h"

void RF24SensorNet::addTempReadHandler(tempReadHandler handler)
{
  _tempReadHandler = handler;
}

void RF24SensorNet::addTempRcvHandler(tempRcvHandler handler)
{
  _tempRcvHandler = handler;
}

bool RF24SensorNet::readTemp(uint16_t toAddr, uint16_t id)
{
  pkt_temp_t payload;
  payload.id = id;
  payload.temp = 0;

  bool ok = _write(toAddr, PKT_TEMP+32, &payload);
  return ok;
}

bool RF24SensorNet::sendTemp(uint16_t toAddr, uint16_t id, int16_t temp)
{
  pkt_temp_t payload;
  payload.id = id;
  payload.temp = temp;

  bool ok = _write(toAddr, PKT_TEMP, &payload);
  return ok;
}
#endif
