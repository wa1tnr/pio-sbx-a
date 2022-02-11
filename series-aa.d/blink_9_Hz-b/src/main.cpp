// blink_9_Hz-a.ino

// Sun  5 Dec 11:50:37 UTC 2021

// purpose: blink LED at 9 Hertz.

#define i9_Hz 55 // milliseconds

// target: almost any board will do, including the Uno R3

#define LED 13 // change for your target board

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
    pulse_at_9_Hz();
}

void setup_gpio(void) {
    pinMode(LED, 1);
    digitalWrite(LED, 0); // turn it off
}

void setup_serial(void) {
    Serial.begin(9600);
    while(!Serial); // hold for serial connection
    delay(100); // superstition
    Serial.println("Program is online now.");
}

void bring_stuff_up(void) {
    setup_gpio();
    setup_serial();
}

void blinking(void) {
    Serial.println("Program begins to blink.");
    pulse();
}

void setup(void) {
    delay(700); // recovery chance interval
    bring_stuff_up();
    blinking();
}

void loop(void) {
    while(-1); // endless
    Serial.println("This never prints.");
}

// END.
