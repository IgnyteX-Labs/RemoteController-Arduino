#include <Arduino.h> // Remove include when using Arduino IDE

#include <SPI.h>
#include "RemoteController.h"
#include "Connections/RF24Connection.h" // Uses the nrf24/RF24 library...

// Remote Controller configuration
#define CE_PIN 10
#define CS_PIN 11

RF24Connection connection(CE_PIN, CS_PIN);
RemoteController rc(connection);

enum Commands : uint8_t
{
	GoForward,
	GoBackward,
	GoLeft,
	GoRight,
	ToggleLED // Add any commands you wish
};

void commandReceivedCallback(const uint8_t commands[], const float throttles[], size_t length)
{
	// Do something with the received commands
	for (size_t i = 0; i < length; i++)
	{
		if (commands[i] == GoForward)
		{
			// Go Forward
			Serial.print("GoForward with throttle: ");
			Serial.println(throttles[i]);
		}
		else if (commands[i] == GoBackward)
		{
			// Go Forward
			Serial.print("GoBackward with throttle: ");
			Serial.println(throttles[i]);
		}
	}
}

/* This second callback function is optional to receive binary payloads send via RemoteController::sendPayload(). */
void payloadReceivedCallback(const void *buffer, size_t length)
{
	// Process binary data
}

// Example programm
unsigned long previousMillis = 0;

void setup()
{
	Serial.begin(115200);
	if (!rc.begin(commandReceivedCallback, payloadReceivedCallback))
	{
		// Failed to begin
		Serial.println(rc.getErrorDescription());
	}
}

void loop() {
	// Execute the rc.run() function repeatedly
	if (!rc.run())
	{
		// An error occured
		Serial.println(rc.getErrorDescription());
	}

	// Send example commands every 2 sec (non blocking) (do not use delay(2000))
	if(millis()-previousMillis > 2000 || previousMillis == 0) {
		rc.sendCommand(GoForward);
		rc.sendCommand(ToggleLED);
	}
}