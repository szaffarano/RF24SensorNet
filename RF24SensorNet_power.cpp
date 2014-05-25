/*
  RF24SensorNet
  A library for sending standard sensor and actuator
  messages between nodes connected using RF24Network.

  Handling for power packets - node power supply and voltages.

  -- 
  Peter Hardy <peter@hardy.dropbear.id.au>
*/
#ifndef __RF24_SENSOR_NET_POWER_H__
#define __RF24_SENSOR_NET_POWER_H__

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include "RF24Network.h"
#include "RF24SensorNet.h"
#include "RF24SensorNet_types.h"

void RF24SensorNet::addPowerReadHandler(powerReadHandler handler)
{
  _powerReadHandler = handler;
}

void RF24SensorNet::addPowerRcvHandler(powerRcvHandler handler)
{
  _powerRcvHandler = handler;
}

bool RF24SensorNet::readPower(uint16_t toAddr)
{
  bool ok = _write(toAddr, PKT_POWER+32, NULL);
  return ok;
}

bool RF24SensorNet::sendPower(uint16_t toAddr, bool battery, bool solar,
			       uint16_t vcc, uint16_t vs)
{
  pkt_power_t payload;
  payload.battery = battery;
  payload.solar = solar;
  payload.vcc = vcc;
  payload.vs = vs;

  bool ok = _write(toAddr, PKT_POWER, &payload);
  return ok;
}
#endif
