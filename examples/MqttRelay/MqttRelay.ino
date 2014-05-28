/* An RF24Network base station, that relays sensor
   net messages in and out of an MQTT bus.

   -- 
   Peter Hardy <peter@hardy.dropbear.id.au>
*/
#include <SPI.h>
#include <Ethernet.h>

#include <PubSubClient.h>
#include <RF24.h>
#include <RF24Network.h>

#include <RF24SensorNet.h>

const int BASE_ADDR = 0;

// RF24 uses 9 and 10 by default, but the Ethernet shield is
// hard-wired to use pin 10 as CS, so we go with 8 and 9.
RF24 radio(8, 9);
RF24Network network(radio);
RF24SensorNet sensornet(network);

byte mac[] = { 0xDE, 0x17, 0x17, 0x17, 0x17, 0x17 }; // My MAC address

byte mqttBroker[] = { 10, 17, 17, 5 }; // IP address of the MQTT broker
char mqttClientId[] = "basestation";
char mqttUsername[] = "arduino";
char mqttPassword[] = "Ziu7im2i";

// We subscribe to MQTT topic "sensornet/in/+/+",
// and publish to "sensornet/out/+/+".
// The first variable is destination arduino node address,
// the second is the sensor type we're sending to on that node.
char mqttTopicPrefixIn[] = "sensornet/in";
char mqttTopicPrefixOut[] = "sensornet/out";
// This topic is used for general info about the relay.
char mqttAnnounceTopic[] = "sensornet/announce";

EthernetClient ethClient;
PubSubClient mqttClient(mqttBroker, 1883, mqtt_callback, ethClient);

char * constructMqttTopic(uint16_t fromAddr, char type) {
  static char mqttTopic[24];
  snprintf(mqttTopic, 24, "%s/%i/%i", mqttTopicPrefixOut, fromAddr, type);
  return mqttTopic;
}

void sendPowerMqtt(uint16_t fromAddr, bool battery, bool solar,
		   uint16_t vcc, uint16_t vs) {
  char mqttPayload[24];
  snprintf(mqttPayload, 24, "%i|%i|%i|%i", battery, solar, vcc, vs);
  mqttClient.publish(constructMqttTopic(fromAddr, PKT_POWER), mqttPayload);
}

void sendSwitchMqtt(uint16_t fromAddr, uint16_t id,
		    bool state, uint32_t timer) {
  char mqttPayload[12];
  snprintf(mqttPayload, 12, "%i|%i|%lu", id, state, timer);
  mqttClient.publish(constructMqttTopic(fromAddr, PKT_SWITCH),
		      mqttPayload);
}

void sendRgbMqtt(uint16_t fromAddr, uint16_t id,
		 char rgb[3], uint32_t timer) {
  char mqttPayload[24];
  snprintf(mqttPayload, 24, "%i|%i|%i|%i|%lu", id,
	   rgb[0], rgb[1], rgb[2], timer);
  mqttClient.publish(constructMqttTopic(fromAddr, PKT_RGB), mqttPayload);
}

void sendTempMqtt(uint16_t fromAddr, uint16_t id, int16_t temp) {
  // Arduino's snprintf doesn't do floats.
  // So I'm about to commit an atrocity to fake it.
  int integral = (int)(temp/10);
  int fractional = temp-(integral*10);

  char mqttPayload[24];
  snprintf(mqttPayload, 24, "%i|%i|%i.%i", fromAddr, id, integral, fractional);
  mqttClient.publish(constructMqttTopic(fromAddr, PKT_TEMP), mqttPayload);
}

void sendHumidMqtt(uint16_t fromAddr, uint16_t id, uint16_t humidity) {
  int integral = (int)(humidity/10);
  int fractional = humidity-(integral*10);

  char mqttPayload[24];
  snprintf(mqttPayload, 24, "%i|%i|%i.%i", fromAddr, id, integral, fractional);
  mqttClient.publish(constructMqttTopic(fromAddr, PKT_HUMID), mqttPayload);
}

bool send_power_rf24(uint16_t toaddr, unsigned char type, char * buffer) {
  (void)buffer;
  // Not possible to set power state, so assume it's a req.
  return sensornet.readPower(toaddr);
}

bool send_switch_rf24(uint16_t toaddr, unsigned char type, char * buffer) {
  char * token;
  uint32_t id;
  bool state;
  uint32_t timer;

  token = strtok(buffer, "|");
  id = strtol(token, NULL, 10);
  token = strtok(NULL,  "|");
  state = strtol(token, NULL, 10);
  token = strtok(NULL, "|");
  timer = strtol(token, NULL, 10);

  if (type > 64) {
    return sensornet.writeSwitch(toaddr, id, state, timer);
  } else {
    return sensornet.readSwitch(toaddr, id);
  }
}

bool send_rgb_rf24(uint16_t toaddr, unsigned char type, char * buffer) {
  char * token;
  uint32_t id;
  char rgb[3];
  uint32_t timer;

  token = strtok(buffer, "|");
  id = strtol(token, NULL, 10);
  token = strtok(NULL, "|");
  rgb[0] = strtol(token, NULL, 10);
  token = strtok(NULL, "|");
  rgb[1] = strtol(token, NULL, 10);
  token = strtok(NULL, "|");
  rgb[2] = strtol(token, NULL, 10);
  token = strtok(NULL, "|");
  timer = strtol(token, NULL, 10);

  if (type > 64) {
    return sensornet.writeRgb(toaddr, id, rgb, timer);
  } else {
    return sensornet.readRgb(toaddr, id);
  }
}

bool send_temp_rf24(uint16_t toaddr, unsigned char type, char * buffer) {
  // We currently assume it's not possible to write a temp, so the
  // only requests we'd see come in over MQTT would be reads.
  // That may change if somebody ever wants to control a thermostat.
  char * token;
  uint32_t id;
  // int16_t temp;

  token = strtok(buffer, "|");
  id = strtol(token, NULL, 10);
  // token = strtok(NULL, "|");
  // temp = strtol(token, NULL, 10);

  return sensornet.readTemp(toaddr, id);
}

bool send_humid_rf24(uint16_t toaddr, unsigned char type, char * buffer) {
  // It's even less likely that anybody would want to write a humidty
  // ready. Right? Right?
  char * token;
  uint32_t id;

  token = strtok(buffer, "|");
  id = strtol(token, NULL, 10);

  return sensornet.readHumid(toaddr, id);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  char buffer[length+1];
  strncpy(buffer, (char*)payload, length);
  buffer[length] = '\0';

  char * topictoken;
  topictoken = strtok(topic, "/");
  // We assume the prefixes contain two tokens, so discard them here.
  topictoken = strtok(NULL, "/");
  topictoken = strtok(NULL, "/");
  uint16_t toaddr = strtol(topictoken, NULL, 10);
  topictoken = strtok(NULL, "/");
  unsigned char type = strtol(topictoken, NULL, 10);

  switch(type % 32) {
  case PKT_POWER:
    send_power_rf24(toaddr, type, buffer);
    break;
  case PKT_SWITCH:
    send_switch_rf24(toaddr, type, buffer);
    break;
  case PKT_RGB:
    send_rgb_rf24(toaddr, type, buffer);
    break;
  case PKT_TEMP:
    send_temp_rf24(toaddr, type, buffer);
    break;
  case PKT_HUMID:
    send_humid_rf24(toaddr, type, buffer);
    break;
  }
}

void setup() {
  SPI.begin();

  Ethernet.begin(mac);
  if (mqttClient.connect(mqttClientId, mqttUsername, mqttPassword)) {
    char onlineMessage[] = "wireless basestation online";
    mqttClient.publish(mqttAnnounceTopic, onlineMessage);
    int topicSize = sizeof(mqttTopicPrefixIn) + 5;
    char subTopic[topicSize];
    snprintf(subTopic, topicSize, "%s/+/+", mqttTopicPrefixIn);
    mqttClient.subscribe(subTopic);
  }

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  network.begin( /*channel*/ 90, /*node address*/ BASE_ADDR);
  sensornet.begin();

  sensornet.addPowerRcvHandler(sendPowerMqtt);
  sensornet.addSwitchRcvHandler(sendSwitchMqtt);
  sensornet.addRgbRcvHandler(sendRgbMqtt);
  sensornet.addTempRcvHandler(sendTempMqtt);
  sensornet.addHumidRcvHandler(sendHumidMqtt);
}

void loop() {
  mqttClient.loop();
  sensornet.update();
}
