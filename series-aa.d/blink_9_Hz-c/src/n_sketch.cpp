// old_filename: blink_9_Hz-a.ino
#define DATE_STAMP "11 Feb 2022 Friday 23:14:42 UTC"

// target: Adafruit Circuit Playground Express (CPX) ATSAMD21G18A

// Sun  5 Dec 11:50:37 UTC 2021

// purpose: exercise accelerometer
// old_purpose: blink LED at 9 Hertz.

// #include "Arduino.h" // isrt quoted here
#include <Arduino.h> // isrt quoted here
#undef  HOLD_FOREVER
#define HOLD_FOREVER // circumvent main loop so mouse control is regained
#undef  HOLD_FOREVER

#include <Adafruit_CircuitPlayground.h>
#include <Mouse.h>
#include <Wire.h>
#include <SPI.h>

#define i9_Hz 55 // milliseconds

#define LED 13 // change for your target board

#define XACCEL_MIN 0.1
#define XACCEL_MAX 8.0
#define XMOUSE_RANGE 25.0
#define XMOUSE_SCALE 1
#define YACCEL_MIN XACCEL_MIN
#define YACCEL_MAX XACCEL_MAX
#define YMOUSE_RANGE XMOUSE_RANGE
#define YMOUSE_SCALE 1
#define FLIP_AXES true

void pulse_once(int interval) {
    digitalWrite(LED,HIGH);
    delay(interval);
    digitalWrite(LED,LOW);
    delay(interval);
}

void pulse_at_9_Hz(void) {
    while(-1) {
        pulse_once(i9_Hz);
    }
}

void pulse(void) { // alias
    // pulse_at_9_Hz();
    pulse_once(i9_Hz);
}

void setup_gpio(void) {
    pinMode(LED, 1);
    digitalWrite(LED, 0); // turn it off
}

void blink_waiting(void) {
    pulse_once(i9_Hz);
    delay(7200); // no harm nothing happening now
}

void setup_serial(void) {
    Serial.begin(9600);
    while(!Serial) { // hold for serial connection
        blink_waiting();
    }
    delay(100); // superstition
    Serial.println("Program is online now.");
}

void bring_stuff_up(void) {
    setup_gpio();
    setup_serial();
}

void mousing(void) {
    Serial.println("Program may begin to move the mouse pointer (depending on how it was compiled).");
}

void blinking(void) {
    Serial.println("Program may begin to blink (depending on how it was compiled).");
    // pulse();
}

void in_all_cases(void) {
    #warning serial port connection usually required
    Serial.println("In all cases - you are likely required to connect to the serial port.");
    /*
        If the program does little or nothing, try connecting to
        the serial port (USB).
        It ordinarily waits for this connection before continuing.
        It will not time-out - it will instead wait forever. ;)

        See, in particular:

            setup_serial();

        That's a good place to disable the behavior.
    */
}

void announces(void) {
    Serial.println(DATE_STAMP);
    blinking();
    mousing();
    in_all_cases();
}

void setup(void) {
    delay(700); // recovery chance interval
    // new:
    CircuitPlayground.begin(); Mouse.begin();
    bring_stuff_up();
    announces();
}


float lerp(float x, float x0, float x1, float y0, float y1) {
  if (x <= x0) {
    return y0;
  }
  else if (x >= x1) {
    return y1;
  }
  return y0 + (y1-y0)*((x-x0)/(x1-x0));
}


void loop(void) {
#ifdef HOLD_FOREVER
   while(-1); // hold forever
#endif
/*
    while(-1); // endless
    Serial.println("This never prints.");
*/

  float x = CircuitPlayground.motionX();
  float y = CircuitPlayground.motionY();
  float x_mag = abs(x);
  float x_mouse = lerp(x_mag, XACCEL_MIN, XACCEL_MAX, 0.0, XMOUSE_RANGE);
  float y_mag = abs(y);
  float y_mouse = lerp(y_mag, YACCEL_MIN, YACCEL_MAX, 0.0, YMOUSE_RANGE);

  if (x < 0) {
    x_mouse *= -1.0;
  }
  if (y < 0) {
    y_mouse *= -1.0;
  }
  x_mouse = floor(x_mouse*XMOUSE_SCALE);
  y_mouse = floor(y_mouse*YMOUSE_SCALE);

  if (!FLIP_AXES) {
    Mouse.move((int)x_mouse, (int)y_mouse, 0);
  }
  else {
    Mouse.move((int)y_mouse, (int)x_mouse, 0);
  }
  delay(10);
  pulse();
}

// ENiD,

#if 0
// Circuit Playground Accelerometer Mouse Example
// Tilt Circuit Playground left/right and up/down to move your mouse, and
// press the left and right push buttons to click the mouse buttons!  Make sure
// the slide switch is in the on (+) position to enable the mouse, or slide into
// the off (-) position to disable it.  By default the sketch assumes you hold
// Circuit Playground with the USB cable coming out the top.
// Author: Tony DiCola
// License: MIT License (https://opensource.org/licenses/MIT)
#include <Adafruit_CircuitPlayground.h>
#include <Mouse.h>
#include <Wire.h>
#include <SPI.h>


// Configuration values to adjust the sensitivity and speed of the mouse.
// X axis (left/right) configuration:
#define XACCEL_MIN 0.1      // Minimum range of X axis acceleration, values below
                            // this won't move the mouse at all.
#define XACCEL_MAX 8.0      // Maximum range of X axis acceleration, values above
                            // this will move the mouse as fast as possible.
#define XMOUSE_RANGE 25.0   // Range of velocity for mouse movements.  The higher
                            // this value the faster the mouse will move.
#define XMOUSE_SCALE 1      // Scaling value to apply to mouse movement, this is
                            // useful to set to -1 to flip the X axis movement.

// Y axis (up/down) configuration:
// Note that the meaning of these values is exactly the same as the X axis above,
// just applied to the Y axis and up/down mouse movement.  You probably want to
// keep these values the same as for the X axis (which is the default, they just
// read the X axis values but you can override with custom values).
#define YACCEL_MIN XACCEL_MIN
#define YACCEL_MAX XACCEL_MAX
#define YMOUSE_RANGE XMOUSE_RANGE
#define YMOUSE_SCALE 1

// Set this true to flip the mouse X/Y axis with the board X/Y axis (what you want
// if holding with USB cable facing up).
#define FLIP_AXES true


// Floating point linear interpolation function that takes a value inside one
// range and maps it to a new value inside another range.  This is used to transform
// each axis of acceleration to mouse velocity/speed. See this page for details
// on the equation: https://en.wikipedia.org/wiki/Linear_interpolation


float lerp(float x, float x0, float x1, float y0, float y1) {
  // Check if the input value (x) is outside its desired range and clamp to
  // those min/max y values.
  if (x <= x0) {
    return y0;
  }
  else if (x >= x1) {
    return y1;
  }
  // Otherwise compute the value y based on x's position within its range and
  // the desired y min & max.
  return y0 + (y1-y0)*((x-x0)/(x1-x0));
}


void setup() {
  // Initialize Circuit Playground library.
  CircuitPlayground.begin();
  // Initialize Arduino mouse library.
  Mouse.begin();
}

void loop() {
  // Check if the slide switch is enabled (on +) and if not then just exit out
  // and run the loop again.  This lets you turn on/off the mouse movement with
  // the slide switch.
  if (!CircuitPlayground.slideSwitch()) {
    return;
  }

  // Grab initial left & right button states to later check if they are pressed
  // or released.  Do this early in the loop so other processing can take some
  // time and the button state change can be detected.
  boolean left_first = CircuitPlayground.leftButton();
  boolean right_first = CircuitPlayground.rightButton();

  // Grab x, y acceleration values (in m/s^2).
  float x = CircuitPlayground.motionX();
  float y = CircuitPlayground.motionY();
  // Use the magnitude of acceleration to interpolate the mouse velocity.
  float x_mag = abs(x);
  float x_mouse = lerp(x_mag, XACCEL_MIN, XACCEL_MAX, 0.0, XMOUSE_RANGE);
  float y_mag = abs(y);
  float y_mouse = lerp(y_mag, YACCEL_MIN, YACCEL_MAX, 0.0, YMOUSE_RANGE);
  // Change the mouse direction based on the direction of the acceleration.
  if (x < 0) {
    x_mouse *= -1.0;
  }
  if (y < 0) {
    y_mouse *= -1.0;
  }
  // Apply any global scaling to the axis (to flip it for example) and truncate
  // to an integer value.
  x_mouse = floor(x_mouse*XMOUSE_SCALE);
  y_mouse = floor(y_mouse*YMOUSE_SCALE);

  // Move mouse.
  if (!FLIP_AXES) {
    // Non-flipped axes, just map board X/Y to mouse X/Y.
    Mouse.move((int)x_mouse, (int)y_mouse, 0);
  }
  else {
    // Flipped axes, swap them around.
    Mouse.move((int)y_mouse, (int)x_mouse, 0);
  }

  // Small delay to wait for button state changes and slow down processing a bit.
  delay(10);

  // Grab a second button state reading to check if the buttons were pressed or
  // released.
  boolean left_second = CircuitPlayground.leftButton();
  boolean right_second = CircuitPlayground.rightButton();

  // Check for left button pressed / released.
  if (!left_first && left_second) {
    // Low then high, button was pressed!
    Mouse.press(MOUSE_LEFT);
  }
  else if (left_first && !left_second) {
    // High then low, button was released!
    Mouse.release(MOUSE_LEFT);
  }
  // Check for right button pressed / released.
  if (!right_first && right_second) {
    // Low then high, button was pressed!
    Mouse.press(MOUSE_RIGHT);
  }
  else if (right_first && !right_second) {
    // High then low, button was released!
    Mouse.release(MOUSE_RIGHT);
  }
}
#endif
