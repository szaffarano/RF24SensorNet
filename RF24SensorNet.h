/*
  RF24SensorNet
  A library for sending standard sensor and actuator
  messages between nodes connected using RF24Network.
  -- 
  Peter Hardy <peter@hardy.dropbear.id.au>
*/
#ifndef __RF24_SENSOR_NET_H__
#define __RF24_SENSOR_NET_H__

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <stdbool.h>

#include "RF24SensorNet_types.h"

class RF24Network;

/* Maximum RF24 payload size. RF24Network truncates anything longer. */
const char MAX_PAYLOAD=24;

long readVcc();

class RF24SensorNet
{
 public:
  RF24SensorNet(RF24Network& network);

  void addInfoReadHandler(infoReadHandler handler);
  void addInfoWriteHandler(infoWriteHandler handler);
  void addInfoRcvHandler(infoRcvHandler handler);
  bool readInfo(uint16_t toAddr);
  bool writeInfo(uint16_t toAddr, uint16_t newAddr);
  bool sendInfo(uint16_t toAddr);

  void addPowerReadHandler(powerReadHandler handler);
  void addPowerRcvHandler(powerRcvHandler handler);
  bool readPower(uint16_t toAddr);
  bool sendPower(uint16_t toAddr, bool battery, bool solar,
		 uint16_t vcc, uint16_t vs);

  void addSwitchReadHandler(switchReadHandler handler);
  void addSwitchWriteHandler(switchWriteHandler handler);
  void addSwitchRcvHandler(switchRcvHandler handler);
  bool readSwitch(uint16_t toAddr, uint16_t id);
  bool writeSwitch(uint16_t toAddr, uint16_t id,
		   bool state, uint32_t timer);
  bool sendSwitch(uint16_t toAddr, uint16_t id,
		  bool state, uint32_t timer);

  void addRgbReadHandler(rgbReadHandler handler);
  void addRgbWriteHandler(rgbWriteHandler handler);
  void addRgbRcvHandler(rgbRcvHandler handler);
  bool readRgb(uint16_t toAddr, uint16_t id);
  bool writeRgb(uint16_t toAddr, uint16_t id,
		char rgb[3], uint32_t timer);
  bool sendRgb(uint16_t toAddr, uint16_t id,
	       char rgb[3], uint32_t timer);

  void begin();
  void update(void);

 private:
  RF24Network& _network;
  infoReadHandler _infoReadHandler;
  infoWriteHandler _infoWriteHandler;
  infoRcvHandler _infoRcvHandler;
  powerReadHandler _powerReadHandler;
  powerRcvHandler _powerRcvHandler;
  switchReadHandler _switchReadHandler;
  switchWriteHandler _switchWriteHandler;
  switchRcvHandler _switchRcvHandler;
  rgbReadHandler _rgbReadHandler;
  rgbWriteHandler _rgbWriteHandler;
  rgbRcvHandler _rgbRcvHandler;

  void _infoHandler(RF24NetworkHeader header);
  void _powerHandler(RF24NetworkHeader header);
  void _switchHandler(RF24NetworkHeader header);
  void _rgbHandler(RF24NetworkHeader header);

  bool _write(uint16_t toAddr, uint16_t type, const void* message);
};
#endif
