#include <SoftwareSerial.h>
#include <Metro.h>

#include "statemachine.h"
#include "config.h"

#include "eyes.h"

#define EVENT_GOT_HAPPY    1000
#define EVENT_GOT_STRESSED 1100
#define EVENT_GOT_PARANOID 1200

Metro lookout(20); // sampling frequency of LDR sensing

//Eyes eyes(LDR_LEFT, LDR_RIGHT);

int threshold = 10.0;

// normalizing the smoothed signal
// ///////////////////////////////
// converges to around 1.30
// takes about 1700 samples to get there

// normalizing the raw signal
// //////////////////////////
// converges between -10, 1
// takes about 1000 samples to get there

// ///////////////////////////////////////////////////////////////////////////////////
// ON CALIBRATING
// ///////////////////////////////////////////////////////////////////////////////////
void on_calibrating_enter() {
  Serial.println("on_calibrating_enter");
}

void on_calibrating_leave() {
  Serial.println("on_calibrating_leave");
  threshold = threshold + (threshold * 1.25);
}

void on_calibrating_loop() {
  threshold = ceil(eyes_abssum()); //max(leye.vmax, reye.vmax); //ceil(eyes_absdiff());
}

State calibrating("calibrating", &on_calibrating_enter, &on_calibrating_loop, &on_calibrating_leave);

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
Fsm fsm(&calibrating);
// ///////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  while(!Serial);

  eyes_init();
//  Serial.println("setup()");

  // state transition configuration
  fsm.add_timed_transition(&calibrating, &happy, 15000, NULL);
  //fsm.add_timed_transition(&demo, &happy, 2000, NULL);
  fsm.add_transition(&happy, &demo, EVENT_GOT_STRESSED, NULL);

//  Serial.println("fsm up");
}


void bot_loop() {
  if( lookout.check() ) {
    eyes_read();
    eyes_debug();
    Serial.print(", ");
    Serial.println(threshold);
  }
}

void loop() {
  bot_loop();
  fsm.run_machine();
}

