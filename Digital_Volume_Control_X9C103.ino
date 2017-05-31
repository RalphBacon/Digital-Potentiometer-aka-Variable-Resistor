#include "Arduino.h"

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

void setup() {
	Serial.begin(9600);
	delay(1000);

	Serial.println("Send any value to continue");
	while (!Serial.available()){}

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

	Serial.println("Setup completed");
}

void loop() {
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

		// Stop demo runaway!
		delay(100);

		// Set the flag that shows we have changed the volume
		volChange = true;
	}

	if (volChange) storeVolume();
	delay(100);
}

long readVolume() {
	// Get the new resistance
	return map(analogRead(resistancePin), 0, 1023, 0, 100);
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
