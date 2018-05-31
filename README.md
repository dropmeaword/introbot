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

When the bot gets a light stimulus while she's happy it will go into *stressed mode*, in stressed mode it will move away from the light by recoiling away from the light. If the bot doesn't receive any light stimuli for more than 2 seconds it will go back to *happy* mode. When the bot goes to *stressed mode* for a third time it will get into *paranoid mode* which will trigger a series of somewhat random on-the-spot movements, by rotating randomly. The robot stays in *paranoid mode* for as long as it doesn't receive any light stimulus for at least 10 seconds.

## Setting up the play area

The room must be reasonably darkened. There is no need for perfect darkness, just enough darkness to give good light differentials with the torches that you will use in the game is enough.

Switch on the bots while you are inside the darkened play area and let them do their thing for the first 40 seconds or so.

Your play area should be ready now!

# A little more about the code

### I want to change the time it spends calibrating or demoing

In the file `introbot_may.ino` on lines 237 and 238 you can find the timed transitions, adjust these to your needs. We find that the calibration step should be a little more than 10 seconds for optimal results.

### Threshold seems too high, the bot never reaches it

The function `on_calibrating_loop` is what makes the threshold adaptive across time.

In the file `introbot_may.ino` on lines 46 you can find this formula: `threshold = threshold + (threshold * 1.25)`, this means that the threshold is set to 125% of the last reading resulting from the sum of both eye inputs.

### What libraries are used?

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

