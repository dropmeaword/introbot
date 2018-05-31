# Introbots

#### Favara edition

## Getting your Arduino ready

You will need a few libraries in your Arduino environment. I have packed them up as a zipfile that you can find here called `libraries-arduino.zip`, that you can [download directly by clicking here](https://github.com/dropmeaword/introbot/raw/master/libraries-arduino.zip).

To install them simply uncompress that zipfile onto your `~/Documents/Arduino` folder. You have to do that before you start the Arduino programming environment.

## What code should we load on the bots?

When you download this repo you will be getting a bunch of stuff, but the primary sketch for the *introbots* is named `introbot_may`.

## Description of the bot's behaviour

When the new code is loaded on the bot, it will behave in the following way:
- 10 second demo, goes forward, then backward, then rotates left, then rotates right
- 15 seconds of calibration, the bot will adapt to light levels during this time and will set the thresholds for interaction adaptively
- when the calibration terminates the bot will buzz for a brief instant
- after the buzzing the bot goes into *happy* mode and is ready for interaction

## What libraries are these?

You will need the Chrono, Metro and corestats libraries in your Arduino environment.

https://github.com/SofaPirate/Chrono.git

https://github.com/thomasfredericks/Metro-Arduino-Wiring.git

https://github.com/dropmeaword/corestats.git

#### introbot_april_playtest_version

This is the version that was used in the user-experiment session that took place in April.

#### introbot_asssembly_test

Tests all the inputs/outputs of the bot to test the assembly work

#### introbot_fsm_may

A Finite State Machine based version of the code that entirely removes the need for delays

#### introbot_testbed_eyes

Isolated LDR eye code

#### introbot_testbed_led

Isolated LED smooth animations

