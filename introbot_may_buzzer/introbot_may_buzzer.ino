#include <SoftwareSerial.h>

#include <Metro.h>

#include "statemachine.h"
#include "config.h"

#include "shaker.h"

#define LED_COUNT 3
int pinLEDS[LED_COUNT] = {10, 5, 6};

#define EVENT_GOT_HAPPY    1000
#define EVENT_GOT_STRESSED 1100
#define EVENT_GOT_PARANOID 1200

#define SHAKE_TIME 180

Shaker shaker(PIN_SHAKER);

Metro metro(3000);

// ///////////////////////////////////////////////////////////////////////////////////
// ON DEMO
// ///////////////////////////////////////////////////////////////////////////////////
void on_demo_enter() {
  Serial.println("on_demo_enter");
  shaker.on(SHAKE_TIME);
}

void on_demo_leave() {
  Serial.println("on_demo_leave");
  //kinetics.stop();
  shaker.off();
}

void on_demo_loop() {
//  kinetics.demo_loop();
  shaker.update();

  if(metro.check()) {
    shaker.on(SHAKE_TIME);
  }
}

State demo("demo", &on_demo_enter, &on_demo_loop, &on_demo_leave);
Fsm fsm(&demo);

// ///////////////////////////////////////////////////////////////////////////////////
// ON HAPPY
// ///////////////////////////////////////////////////////////////////////////////////
void on_happy_enter() {
  Serial.println("on_happy_enter");
}

void on_happy_leave() {
  Serial.println("on_happy_leave");
}

void on_happy_loop() {
}

State happy("happy", &on_happy_enter, &on_happy_loop, &on_happy_leave);

// ///////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.println("setup()");
 
  // state transition configuration
  //fsm.add_timed_transition(&calibrating, &happy, 5000, NULL);
  fsm.add_timed_transition(&demo, &happy, 25000, NULL);

  Serial.println("fsm up");
}


void bot_loop() {
}

void loop() {
  bot_loop();
  fsm.run_machine();
}

