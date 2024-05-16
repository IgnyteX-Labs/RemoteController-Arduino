#include <Arduino.h> // Remove include when using Arduino IDE

#include <SPI.h>
#include "RemoteController.h"
#include "Connections/Connection.h"

class CustomConnection: public Connection {
public:
	/**
	 * @brief Connects to and configures any modules needed by the Connection to transceive/receive
	 *
	 * @return true if the connection to and configuration of the required modules was successful
	 * @return false if failed to configure or connect
	 */
	bool begin() {
		// Any configuration
		return true;
	}

	/**
	 * @brief Closes the connection and frees all the memory occupied
	 *
	 */
	void end() {

	}

	/**
	 * @brief This function checks for incoming data and is called repeteadly!
	 *
	 * @return true New data is available and needs to be handled
	 * @return false No data available can be skipped
	 */
	bool available() {
		// Check for incoming data
		return false;
	}

	/**
	 * @brief This function actually reads the available incoming data
	 *
	 * @param buffer to store the incoming data to
	 * @param length length/size of the buffer
	 */
	void read(void *buffer, size_t length) {
		// Write the incoming data into the buffer
	}

	/**
	 * @brief Get the size of the payload that will be read with Connection::read()
	 *
	 * @return size_t payload size in bytes, should return <1  if the payload is corrupt
	 */
	size_t getPayloadSize() {
		return 8; // 8 bytes
	}

	/**
	 * @brief Writes data to the other RemoteController
	 *
	 * @param buffer where the data to transmit is stored
	 * @param length length/size of the payload/buffer
	 * @return true succesfull transmission (ack received)
	 * @return false failed to transmit
	 */
	bool write(const void *buffer, size_t length) {
		// Write the data to the other controller (should be received with the same Connection implementation on another device)
		return true; // Transmission succeeded (ack received)
	}

	/**
	 * @brief The maximum size, in byte, that can be send in one package i.e. with one Connection::write() call. HAS TO BE AN EVEN NUMBER!!
	 *
	 */
	size_t getMaxPackageSize() {
		// return the max payload size of this connection implementation
		return 32; // 32 bytes
	}
};

CustomConnection customConnection;

// Remote Controller configuration
RemoteController rc(customConnection);

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

void loop()
{
	// Execute the rc.run() function repeatedly
	if (!rc.run())
	{
		// An error occured
		Serial.println(rc.getErrorDescription().c_str());
	}

	// Send example commands every 2 sec (non blocking) (do not use delay(2000))
	if (millis() - previousMillis > 2000 || previousMillis == 0)
	{
		rc.sendCommand(GoForward);
		rc.sendCommand(ToggleLED);
	}
}