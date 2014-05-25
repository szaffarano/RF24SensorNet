/*
  RF24SensorNet
  A library for sending standard sensor and actuator
  messages between nodes connected using RF24Network.

  Handling for RGB packets - RGB lights and light faders.

  -- 
  Peter Hardy <peter@hardy.dropbear.id.au>
*/
#ifndef __RF24_SENSOR_NET_RGB_H__
#define __RF24_SENSOR_NET_RGB_H__

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include "RF24Network.h"
#include "RF24SensorNet.h"
#include "RF24SensorNet_types.h"

void RF24SensorNet::addRgbReadHandler(rgbReadHandler handler)
{
  _rgbReadHandler = handler;
}

void RF24SensorNet::addRgbWriteHandler(rgbWriteHandler handler)
{
  _rgbWriteHandler = handler;
}

void RF24SensorNet::addRgbRcvHandler(rgbRcvHandler handler)
{
  _rgbRcvHandler = handler;
}

bool RF24SensorNet::writeRgb(uint16_t toAddr, uint16_t id,
			     char rgb[3], uint32_t timer)
{
  pkt_rgb_t payload;
  payload.id = id;
  for (int i=0; i<3; i++) payload.rgb[i] = rgb[i];
  payload.timer = timer;

  bool ok = _write(toAddr, PKT_RGB+64, &payload);
  return ok;
}

bool RF24SensorNet::readRgb(uint16_t toAddr, uint16_t id)
{
  pkt_rgb_t payload;
  payload.id = id;
  for (int i=0; i<3; i++) payload.rgb[i] = 0;
  payload.timer = 0;

  bool ok = _write(toAddr, PKT_RGB+32, &payload);
  return ok;
}

bool RF24SensorNet::sendRgb(uint16_t toAddr, uint16_t id,
			     char rgb[3], uint32_t timer)
{
  pkt_rgb_t payload;
  payload.id = id;
  for (int i=0; i<3; i++) payload.rgb[i] = rgb[i];
  payload.timer = timer;

  bool ok = _write(toAddr, PKT_RGB, &payload);
  return ok;
}
#endif
