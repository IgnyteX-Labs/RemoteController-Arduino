#ifndef REMOTECONTROLLER_H_
#define REMOTECONTROLLER_H_

#include <vector>
#include <functional>
#include <stdint.h>
#include <stddef.h>
#include "Connections/Connection.h"

/**
 * @brief
 *
 */
class RemoteController
{
public:
	/**
	 * @brief Priority with which the Command should be sent
	 *
	 */
	enum RemoteControllerPriority
	{
		Normal /** the command is sent as a batch when RemoteController::run() is called */,
		High /** the command is sent immediately with the RemoteController::sendCommand() function call */
	};

	RemoteController();
	RemoteController(Connection &connection);
	~RemoteController();

	/**
	 * @brief Starts the RemoteController and connects it to the other controller
	 *
	 * @param cmdClb this callback function is called with a vector of commands and a vector of throttles corresponding to the commands, when commands are received
	 * @return true succesffully started the RemoteController and connected to the other controller
	 * @return false failed to start or connect
	 */
	bool begin(std::function<void(const std::vector<uint8_t> &commands, const std::vector<uint8_t> &throttle)> cmdClb);

	/**
	 * @brief Starts the RemoteController and connects it to the other controller
	 *
	 * @param cmdClb this callback function is called with a vector of commands and a vector of throttles corresponding to the commands, when commands are received
	 * @param pldClb this callback function is called when a binary payload is received
	 * @return true succesffully started the RemoteController and connected to the other controller
	 * @return false failed to start or connect
	 */
	bool begin(std::function<void(const std::vector<uint8_t> &commands, const std::vector<uint8_t> &throttle)> cmdClb, std::function<void(const void *buffer, size_t length)> pldClb);
	/**
	 * @brief Closes the RemoteController connection and frees all occupied memory
	 *
	 */
	void end();

	/**
	 * @brief This function has to be called repeatedly in a loop! It handles the transmission of queued commands and receiving of commands/payloads.
	 *
	 */
	void run();

	/**
	 * @brief Sends a command to the other controller without throttle (throttle is set to 255 internally)
	 *
	 * @param command The command to be sent (should be implemented as enum on both RemoteControllers)
	 * @param priority The RemoteControllerPriority with which the command should be sent
	 */
	void sendCommand(uint8_t command, RemoteControllerPriority priority = RemoteControllerPriority::Normal);

	/**
	 * @brief Sends a command including a uint8_t throttle value.
	 *
	 * @param command The command to be sent (should be implemented as enum on both RemoteControllers)
	 * @param throttle a uint8_t (0-255) value to be sent alongside the command for throttle, etc. control
	 * @param priority The RemoteControllerPriority with which the command should be sent
	 */
	void sendCommand(uint8_t command, uint8_t throttle, RemoteControllerPriority priority = RemoteControllerPriority::Normal);

	/**
	 * @brief Sends a binary payload to the other RemoteController
	 *
	 * @param buffer the binary data to be sent
	 * @param length the length of the data buffer
	 */
	void sendPayload(const void *buffer, size_t length);

	/**
	 * @brief An implementation of standard commands. WARNING if additonal commands want to be used implement an enum conforming to uint8_t holding ALL nedded Commands. IGNORE the StandardCommands enum. Only one enum of Commands can be used!!
	 *
	 */
	enum StandardCommands : uint8_t
	{
		GoForward,
		GoBackward,
		GoLeft,
		GoRight
	};

	enum Error : uint8_t
	{
		NoError,
		CannotBeginConnection,
		FailedToTransmitCommands,
		
	};

private:
	Connection &connection;
	/**
	 * @brief Holds a list of 8bit integers in pairs of two: The first integer is always a Command and the second integer a corresponding throttle value.
	 *
	 */
	std::vector<uint8_t> commandQueue;
	std::function<void(const std::vector<uint8_t> &commands, const std::vector<uint8_t> &throttles)> commandCallbackFunction;
	std::function<void(const void *buffer, size_t length)> payloadCallbackFunction;

#ifndef REMOTECONTROLLER_INCOMING_BUFFER_SIZE
#define REMOTECONTROLLER_INCOMING_BUFFER_SIZE 32
#endif
#ifndef REMOTECONTROLLER_OUTGOING_BUFFER_SIZE
#define REMOTECONTROLLER_OUTGOING_BUFFER_SIZE 32
#endif

	uint8_t *incomingBuffer;
	uint8_t *outgoingBuffer;
	std::vector<uint8_t> incomingCommandsBuffer;
	std::vector<uint8_t> incomingThrottlesBuffer;
// RemoteController Identifier byte
#define REMOTECONTROLLER_IDENTIFIER_COMMAND 0xEEAF

	bool m_begin();
	bool transmitCommands(const std::vector<uint8_t> &commands);
};

#endif