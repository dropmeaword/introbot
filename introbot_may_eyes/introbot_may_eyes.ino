#include <SoftwareSerial.h>

#include "statemachine.h"
#include "config.h"

#include "eyes.h"

#define EVENT_GOT_HAPPY    1000
#define EVENT_GOT_STRESSED 1100
#define EVENT_GOT_PARANOID 1200


Eyes eyes(LDR_LEFT, LDR_RIGHT);

int threshold = 8;

// ///////////////////////////////////////////////////////////////////////////////////
// ON DEMO
// ///////////////////////////////////////////////////////////////////////////////////
void on_demo_enter() {
//  Serial.println("on_demo_enter");
}

void on_demo_leave() {
//  Serial.println("on_demo_leave");
}

void on_demo_loop() {
}

State demo("demo", &on_demo_enter, &on_demo_loop, &on_demo_leave);
Fsm fsm(&demo);

// ///////////////////////////////////////////////////////////////////////////////////
// ON HAPPY
// ///////////////////////////////////////////////////////////////////////////////////
void on_happy_enter() {
//  Serial.println("on_happy_enter");
}

void on_happy_leave() {
//  Serial.println("on_happy_leave");
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

//  Serial.println("setup()");

  // state transition configuration
  //fsm.add_timed_transition(&calibrating, &happy, 5000, NULL);
  fsm.add_timed_transition(&demo, &happy, 2000, NULL);

//  Serial.println("fsm up");
}


void bot_loop() {
  eyes.read();
  eyes.debug();

  Serial.print(", ");
  Serial.println(threshold);
}

void loop() {
  bot_loop();
  fsm.run_machine();

  delay(20);
}

