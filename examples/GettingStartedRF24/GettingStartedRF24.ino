#include <Arduino.h> // Remove include when using Arduino IDE

#include <SPI.h>
#include "RemoteController.h"
#include "Connections/RF24Connection.h"

// Remote Controller configuration
RF24Connection connection;
RemoteController rc(connection);

enum Commands : uint8_t
{
	GoForward,
	GoBackward,
	GoLeft,
	GoRight,
	ToggleLED // Add any commands you wish
};

void commandReceivedCallback(const std::vector<uint8_t> &commands, const std::vector<uint8_t> &throttle)
{
	// Do something with the received commands
	for (auto command : commands)
	{
		if (command == GoForward)
		{
			// Go Forward
			Serial.println("GoForward");
		}
		else if (command == GoBackward)
		{
			// Go backward
			Serial.println("GoBackward");
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
		Serial.println(rc.getErrorDescription().c_str());
	}
}

void loop() {
	// Execute the rc.run() function repeatedly
	if (!rc.run())
	{
		// An error occured
		Serial.println(rc.getErrorDescription().c_str());
	}

	// Send example commands every 2 sec (non blocking) (do not use delay(2000))
	if(millis()-previousMillis > 2000 || previousMillis == 0) {
		rc.sendCommand(GoForward);
		rc.sendCommand(ToggleLED);
	}
}