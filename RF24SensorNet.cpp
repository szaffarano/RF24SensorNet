/*
  RF24SensorNet
  A library for sending standard sensor and actuator
  messages between nodes connected using RF24Network.
  -- 
  Peter Hardy <peter@hardy.dropbear.id.au>
*/
#include "RF24Network.h"
#include "RF24SensorNet.h"

// Neat hack for reading Vcc compared to internal reference voltage.
// http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

RF24SensorNet::RF24SensorNet(RF24Network& network): _network(network)
{
  _infoReadHandler = NULL;
  _infoWriteHandler = NULL;
  _infoRcvHandler = NULL;
  _powerReadHandler = NULL;
  _powerRcvHandler = NULL;
  _switchReadHandler = NULL;
  _switchWriteHandler = NULL;
  _switchRcvHandler = NULL;
  _rgbReadHandler = NULL;
  _rgbWriteHandler = NULL;
  _rgbRcvHandler = NULL;
}

void RF24SensorNet::begin()
{

}

void RF24SensorNet::update(void)
{
  _network.update();

  while (_network.available()) {
    RF24NetworkHeader header;
    _network.peek(header);
    switch (header.type % 64) {
    case PKT_INFO:
      _infoHandler(header);
      break;
    case PKT_POWER:
      _powerHandler(header);
      break;
    case PKT_SWITCH:
      _switchHandler(header);
      break;
    case PKT_RGB:
      _rgbHandler(header);
      break;
    }
  }
}

bool RF24SensorNet::_write(uint16_t toaddr, uint16_t type, const void* message)
{
  uint8_t size = 0;
  switch(type % 64) {
  case PKT_INFO:
    break;
  case PKT_POWER:
    size = sizeof(pkt_power_t);
    break;
  case PKT_SWITCH:
    size = sizeof(pkt_switch_t);
    break;
  case PKT_RGB:
    size = sizeof(pkt_rgb_t);
    break;
  default:
    // No such type :-(
    return false;
  }
  RF24NetworkHeader header(toaddr, type);
  return _network.write(header, message, size);
}

void RF24SensorNet::_infoHandler(RF24NetworkHeader header)
{
}

void RF24SensorNet::_powerHandler(RF24NetworkHeader header)
{
  if (_powerRcvHandler != NULL) {
    pkt_power_t payload;
    _network.read(header, &payload, sizeof(payload));
    _powerRcvHandler(header.from_node, payload.battery, payload.solar,
		     payload.vcc, payload.vs);
  }
}

void RF24SensorNet::_switchHandler(RF24NetworkHeader header)
{
  pkt_switch_t payload;
  _network.read(header, &payload, sizeof(payload));

  if (header.type > 64) {
    if (_switchWriteHandler != NULL) {
      _switchWriteHandler(header.from_node, payload.id,
			  payload.state, payload.timer);
    }
  } else if (header.type > 32) {
    if (_switchReadHandler != NULL) {
      _switchReadHandler(header.from_node, payload.id);
    }
  } else {
    if (_switchRcvHandler != NULL) {
      _switchRcvHandler(header.from_node, payload.id,
			payload.state, payload.timer);
    }
  }
}

void RF24SensorNet::_rgbHandler(RF24NetworkHeader header)
{
  pkt_rgb_t payload;
  _network.read(header, &payload, sizeof(payload));

  if (header.type > 64) {
    if (_rgbWriteHandler != NULL) {
      _rgbWriteHandler(header.from_node, payload.id,
		       payload.rgb, payload.timer);
    }
  } else if (header.type > 32) {
    if (_rgbReadHandler != NULL) {
      _rgbReadHandler(header.from_node, payload.id);
    }
  } else {
    if (_rgbRcvHandler != NULL) {
      _rgbRcvHandler(header.from_node, payload.id,
		     payload.rgb, payload.timer);
    }
  }
}
