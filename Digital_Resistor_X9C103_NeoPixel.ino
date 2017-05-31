#include "Arduino.h"

// This is the library we need
#include <FastLED.h>

// How many LEDs are connected?
#define NUM_LEDS 16

// Define the Pins
#define DATA_PIN 6

// Define the array of LEDs
CRGB leds[NUM_LEDS];

// Digital resistor X9C103
#define upDownPin A1
#define movePin A2
#define resistancePin A5

// X9C103 pins
#define CS	10
#define INC	9
#define UD	8
#define PWR 7

// Current mode is INC or DEC volume?
bool isINC = true;

// ---------------------------------------------------------------
// SETUP     SETUP     SETUP     SETUP     SETUP     SETUP
// ---------------------------------------------------------------
void setup() {
	Serial.begin(115200);

	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

	// CS LOW is active
	pinMode(CS, OUTPUT);
	digitalWrite(CS, LOW); // ON

	// INC - LOW is active
	pinMode(INC, OUTPUT);
	digitalWrite(INC, HIGH); // OFF

	// UP is high, DOWN is low
	pinMode(UD, OUTPUT);

	// Power up device
	Serial.println("Powering up X9C103");
	pinMode(PWR, OUTPUT);
	digitalWrite(PWR, HIGH);
	delay(10);

	// Retrieve stored volume setting
	Serial.print("Restoring volume to ");
	Serial.println(readVolume());

	// Demo switches, direction and push button
	pinMode(upDownPin, INPUT);
	pinMode(movePin, INPUT_PULLUP);

	// Initial lighting up
	lightLEDs();
}

// ---------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// ---------------------------------------------------------------
void loop() {

	// Any change to resistor value?
	if (changeVolume()) {

		// Light them up
		lightLEDs();
	}
}

void lightLEDs() {
	// Map the pot values to 0 - Number of LEDs (restricted to 1000)
	// to avoid flicker at the extreme end of the pot.
	int numLedsToLight = readVolume();
	Serial.print("LED count: ");
	Serial.println(numLedsToLight);

	// Clear the existing led values
	FastLED.clear();

	// Change led colours for each 'block' of LEDs
	// If you have more / less LEDs than 12 you will have to change this
	for (int led = 0; led < numLedsToLight; led++) {
		// First 4 LEDs
		if (led < 4) leds[led] = CRGB::Green;

		// Second 4 LEDs
		if ((led >= 4) & (led < 8)) leds[led] = CRGB::DeepSkyBlue;

		// Next 3 LEDs
		if ((led >= 8) & (led < 11)) leds[led] = CRGB::Red;

		// Final LEDs
		if (led >= 11) leds[led] = CRGB::AliceBlue;
	}

	// Brightness ranges from 0 (off) to 100 (watch current consumption!)
	FastLED.setBrightness(10);

	// Update the display
	FastLED.show();
}

long readVolume() {
	// Get the new resistance
	return map(analogRead(resistancePin), 0, 1020, 0, NUM_LEDS);
}

void storeVolume() {
	// Ensure INC is HIGH then move CS HIGH and back to LOW
	digitalWrite(INC, HIGH);
	delayMicroseconds(1);
	digitalWrite(CS, HIGH);
	delay(20);
	digitalWrite(CS, LOW);
	Serial.println("Volume stored.");
}

bool changeVolume() {
	// Are we INC or DEC the value?
	isINC = digitalRead(upDownPin) == HIGH;

	// Did we change the volume flag
	bool volChange = false;

	// If the UD pin is pressed initiate a change to resistance
	while (digitalRead(movePin) == LOW) {

		// Set the direction, up/down
		digitalWrite(UD, isINC ? HIGH : LOW);

		// Increment or Decrement as per the UD value
		digitalWrite(INC, LOW);
		delay(10);

		// Done!
		digitalWrite(INC, HIGH);

		// Get the new resistance
		Serial.println(readVolume());

		// Set the flag that shows we have changed the volume
		volChange = true;

		delay(100);
	}

	if (volChange) {
		storeVolume();
		return true;
	}
	else
		return false;
}
