/*
  RF24SensorNet
  A library for sending standard sensor and actuator
  messages between nodes connected using RF24Network.

  Handling for info packets - basic node information.

  -- 
  Peter Hardy <peter@hardy.dropbear.id.au>
*/
#ifndef __RF24_SENSOR_NET_INFO_H__
#define __RF24_SENSOR_NET_INFO_H__

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include "RF24Network.h"
#include "RF24SensorNet.h"
#include "RF24SensorNet_types.h"

void RF24SensorNet::addInfoReadHandler(infoReadHandler handler)
{
  _infoReadHandler = handler;
}

void RF24SensorNet::addInfoWriteHandler(infoWriteHandler handler)
{
  _infoWriteHandler = handler;
}

void RF24SensorNet::addInfoRcvHandler(infoRcvHandler handler)
{
  _infoRcvHandler = handler;
}

bool RF24SensorNet::readInfo(uint16_t toAddr)
{
  return true;
}

bool RF24SensorNet::writeInfo(uint16_t toAddr, uint16_t newAddr)
{
  return true;
}

bool RF24SensorNet::sendInfo(uint16_t toAddr)
{
  return true;
}
#endif
