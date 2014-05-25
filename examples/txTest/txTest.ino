/* RF24SensorNet txTest
   An update to the BlinkWithoutDelay example sketch.
   Instead of blinking an attached LED, send orders to
   blink a remote LED over an RF24SensorNet network.

   -- 
   Peter Hardy <peter@hardy.dropbear.id.au>
*/
#include <SPI.h>

#include <RF24.h>
#include <RF24Network.h>
#include <RF24SensorNet.h>

static uint16_t myAddr = 00; // RF24Network address of this node.
static uint16_t remoteAddr = 01; // RF24Network address of destination node.
static uint16_t remoteLedId = 0; // The ID of the switch we want to control.

int ledState = LOW; // The currrent state of the remote LED
unsigned long previousMillis = 0; // The last time the remote LED was updated
static long interval = 1000; // How often to update the LED

RF24 radio(9, 10);
RF24Network network(radio);
RF24SensorNet sensornet(network);

// Callback when a switch status packet is received.
void getStatus(uint16_t fromAddr, uint16_t id, bool state, uint32_t timer) {
  if (fromAddr == remoteAddr && id == remoteLedId) {
    if (state) {
      Serial.println("Switch is now on.");
      ledState = HIGH;
    } else {
      Serial.println("Switch is now off.");
      ledState = LOW;
    }
  }
}

void setup() {
  // Initialise the SPI bus.
  SPI.begin();
  // Initialise the nrf24L01 radio.
  radio.begin();
  // Initialise the RF24Network.
  network.begin( /*channel*/ 90, /*node address*/ myAddr);
  // Initialise the RF24SensorNet
  sensornet.begin();

  // Attach the switch get handler callback to listen for responses.
  sensornet.addSwitchRcvHandler(getStatus);

  // Initialise Serial debugging
  Serial.begin(115200);
}

void loop() {
  // Pump the RF24SensorNet
  sensornet.update();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    if (ledState == LOW) {
      Serial.println("Sending switch on command.");
      sensornet.writeSwitch(remoteAddr, remoteLedId, true, 0);
    }
    else {
      Serial.println("Sending switch off command.");
      sensornet.writeSwitch(remoteAddr, remoteLedId, false, 0);
    }
  }
}
