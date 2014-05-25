/*
  RF24SensorNet
  A library for sending standard sensor and actuator
  messages between nodes connected using RF24Network.

  Handling for switch packets - on/off switches attached to nodes.

  -- 
  Peter Hardy <peter@hardy.dropbear.id.au>
*/
#ifndef __RF24_SENSOR_NET_SWITCH_H__
#define __RF24_SENSOR_NET_SWITCH_H__

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include "RF24Network.h"
#include "RF24SensorNet.h"
#include "RF24SensorNet_types.h"

void RF24SensorNet::addSwitchReadHandler(switchReadHandler handler)
{
  _switchReadHandler = handler;
}

void RF24SensorNet::addSwitchWriteHandler(switchWriteHandler handler)
{
  _switchWriteHandler = handler;
}

void RF24SensorNet::addSwitchRcvHandler(switchRcvHandler handler)
{
  _switchRcvHandler = handler;
}

bool RF24SensorNet::writeSwitch(uint16_t toAddr, uint16_t id,
				bool state, uint32_t timer)
{
  pkt_switch_t payload;
  payload.id = id;
  payload.state = state;
  payload.timer = timer;

  bool ok = _write(toAddr, PKT_SWITCH+64, &payload);
  return ok;
}

bool RF24SensorNet::readSwitch(uint16_t toAddr, uint16_t id)
{
  pkt_switch_t payload;
  payload.id = id;
  payload.state = 0;
  payload.timer = 0;

  bool ok = _write(toAddr, PKT_SWITCH+32, &payload);
  return ok;
}

bool RF24SensorNet::sendSwitch(uint16_t toAddr, uint16_t id,
			       bool state, uint32_t timer)
{
  pkt_switch_t payload;
  payload.id = id;
  payload.state = state;
  payload.timer = timer;

  bool ok = _write(toAddr, PKT_SWITCH, &payload);
  return ok;
}
#endif
