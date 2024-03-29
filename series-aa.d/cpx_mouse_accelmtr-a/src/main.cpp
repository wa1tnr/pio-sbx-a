// target: Adafruit Circuit Playground Express (CPX) ATSAMD21G18A

// Thu 11 Aug 23:14:35 UTC 2022

// purpose: simulate a mouse pointer no buttons - was exercise accelerometer

#include "Arduino.h"

#undef NO_SERIAL_USED
#define NO_SERIAL_USED

#undef  HOLD_FOREVER
#define HOLD_FOREVER // circumvent main loop so mouse control is regained
#undef  HOLD_FOREVER

#include <Adafruit_CircuitPlayground.h>
#include <Mouse.h>
#include <Wire.h>
#include <SPI.h>

#define i9_Hz 55 // milliseconds

#define LED 13 // change for your target board

// NICE PATCH: XACCEL_THRESHOLD: 3.0, XACCEL_MAX: 2.0, and XMOUSE_SPEED: 2.0
// The effect: wide dead spot at centers.  Requires considerable tilt to start in any direction.
// That was the effect intented. ;)

#define XACCEL_THRESHOLD 0.7 // 0.1 prior to 11 aug


// sensitivity to threshold where high speed kicks in: XACCEL_MAX.
// 'does it trip too easily when not tilted a lot?'


// trial 23:33z 11 Aug: increase _MAX from 2.0 to 4.0 .. want it to trip much further from flat.
#define XACCEL_MAX 4.0 // 8.0 // tripwire: if larger than _MAX, then max it out (to the value of _SPEED).



// trial 23:33z 11 Aug: increase _SPEED from 4.0 to 6.0 .. when it does trip, want it to go faster.
#define XMOUSE_SPEED 6.0 // 1.5 unbalanced + or - but 1.0 is balanced.  Why.

/*

    lerp returns XMOUSE_SPEED when it goes past XACCEL_MAX

    if it's > MAX then just return SPEED itself

    float x_mouse = lerp(x_mag, XACCEL_THRESHOLD, XACCEL_MAX, 0.0, XMOUSE_SPEED);

*/

#define XMOUSE_SCALE 1
#define YACCEL_THRESHOLD XACCEL_THRESHOLD
#define YACCEL_MAX XACCEL_MAX
#define YMOUSE_SPEED XMOUSE_SPEED
#define YMOUSE_SCALE 1
// #define FLIP_AXES true // factory code
#define FLIP_AXES false // local mod tnr
#define FLIP_X_AXIS true
#define FLIP_Y_AXIS true

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
    // OLD IGNORE 11 Aug 2022: pulse_at_9_Hz();
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
#ifdef NO_SERIAL_USED
    #warning NO_SERIAL_USED flag set
#endif

#ifndef NO_SERIAL_USED
    #warning NO_SERIAL_USED flag unset
    setup_serial();
#endif
}

void mousing(void) {
#ifndef NO_SERIAL_USED
    Serial.println("Program may begin to move the mouse pointer (depending on how it was compiled).");
#endif
}

void blinking(void) {
#ifndef NO_SERIAL_USED
    Serial.println("Program may begin to blink (depending on how it was compiled).");
    Serial.println("git clone git@github.com:wa1tnr/pio-sbx-a.git in tinagra");
    Serial.println("");
#endif
    // pulse();
}

void in_all_cases(void) {

#ifdef NO_SERIAL_USED
    #warning NO_SERIAL_USED flag is set
#endif

#ifndef NO_SERIAL_USED
    #warning serial port connection usually required
    Serial.println("In all cases - you are likely required to connect to the serial port.");
#endif
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

  // float y = CircuitPlayground.motionX(); // preferred axes
  // float x = CircuitPlayground.motionY(); // preferred axes

  float x = CircuitPlayground.motionX(); // canonical axes
  float y = CircuitPlayground.motionY(); // canonical axes

  float x_mag = abs(x);
  float x_mouse = lerp(x_mag, XACCEL_THRESHOLD, XACCEL_MAX, 0.0, XMOUSE_SPEED);
  float y_mag = abs(y);
  float y_mouse = lerp(y_mag, YACCEL_THRESHOLD, YACCEL_MAX, 0.0, YMOUSE_SPEED);

#ifdef FLIP_X_AXIS
  x_mouse *= -1.0; // swap left with right, only
#endif

  if (x < 0) {
    x_mouse *= -1.0;
  }

#ifdef FLIP_Y_AXIS
  y_mouse *= -1.0;
#endif

  if (y < 0) {
    y_mouse *= -1.0;
  }
  x_mouse = floor(x_mouse*XMOUSE_SCALE);
  y_mouse = floor(y_mouse*YMOUSE_SCALE);










  if (!FLIP_AXES) {
    // the not-FLIP state is programmed, since it's used here.
    Mouse.move((int)x_mouse, (int)y_mouse, 0); // when not flipped

#ifndef NO_SERIAL_USED
    Serial.print("  x_mouse: ");
    Serial.print(x_mouse);

    Serial.print("  y_mouse: "); // flipped by local cruftiness so y is x and x is y. ;)
    Serial.print(y_mouse);

    Serial.println("  ");

    if ( y_mouse >  0.9 ) { Serial.write("py"); }
    if ( x_mouse >  0.9 ) { Serial.write("px"); }

    if ( y_mouse < 0.0 ) { Serial.write("ny"); }
    if ( x_mouse < 0.0 ) { Serial.write("nx"); }
#endif


#if 0




    if (
        ( x_mouse >  0.9 ) || // test for signed char 7 bits + sign maybe
        ( y_mouse >  0.9 )
    ) {
        Mouse.move((int)x_mouse, (int)y_mouse, 0); // when not flipped
        Serial.write('+');
    }


    if (
        ( x_mouse < 0.0 ) ||
        ( y_mouse < 0.0 )
    ) {
        Mouse.move((int)x_mouse, (int)y_mouse, 0);
        Serial.write('-');
    }





#endif // #if 0


  }
  else {

  // the FLIP state can also be programmed; it's not used here.

        Mouse.move((int)y_mouse, (int)x_mouse, 0); // when flipped



  }
  delay(10);
  // disable 11 Aug 2022 too much blikie action: pulse();
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
#define XACCEL_THRESHOLD 0.1      // Minimum range of X axis acceleration, values below
                            // this won't move the mouse at all.
#define XACCEL_MAX 8.0      // Maximum range of X axis acceleration, values above
                            // this will move the mouse as fast as possible.
#define XMOUSE_SPEED 25.0   // Range of velocity for mouse movements.  The higher
                            // this value the faster the mouse will move.
#define XMOUSE_SCALE 1      // Scaling value to apply to mouse movement, this is
                            // useful to set to -1 to flip the X axis movement.

// Y axis (up/down) configuration:
// Note that the meaning of these values is exactly the same as the X axis above,
// just applied to the Y axis and up/down mouse movement.  You probably want to
// keep these values the same as for the X axis (which is the default, they just
// read the X axis values but you can override with custom values).
#define YACCEL_THRESHOLD XACCEL_THRESHOLD
#define YACCEL_MAX XACCEL_MAX
#define YMOUSE_SPEED XMOUSE_SPEED
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
  float x_mouse = lerp(x_mag, XACCEL_THRESHOLD, XACCEL_MAX, 0.0, XMOUSE_SPEED);
  float y_mag = abs(y);
  float y_mouse = lerp(y_mag, YACCEL_THRESHOLD, YACCEL_MAX, 0.0, YMOUSE_SPEED);
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
