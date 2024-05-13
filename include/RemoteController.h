#ifndef REMOTECONTROLLER_H_
#define REMOTECONTROLLER_H_

#include <vector>
#include <functional>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include "Connections/Connection.h"

#ifndef REMOTECONTROLLER_INCOMING_BUFFER_SIZE
#define REMOTECONTROLLER_INCOMING_BUFFER_SIZE 32
#endif
#ifndef REMOTECONTROLLER_OUTGOING_BUFFER_SIZE
#define REMOTECONTROLLER_OUTGOING_BUFFER_SIZE 32
#endif
#ifndef REMOTECONTROLLER_MAX_COMMAND_QUEUE_SIZE
#define REMOTECONTROLLER_MAX_COMMAND_QUEUE_SIZE 128
#endif
#ifndef REMOTECONTROLLER_IDENTIFIER_COMMAND
#define REMOTECONTROLLER_IDENTIFIER_COMMAND 0xEEAF // RemoteController Identifier 2 bytes
#endif

/**
 * @brief The RemoteController Class provides a simple and easy to use API for RemoteControllers in Embedded Projects.
 * 
 */
class RemoteController
{
public:
	/**
	 * @brief Priority with which the Command should be sent
	 *
	 */
	enum Priority
	{
		Normal /** the command is sent as a batch when RemoteController::run() is called */,
		High /** the command is sent immediately with the RemoteController::sendCommand() function call */
	};

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
	 * @return true RemoteController tasks where handled successfully
	 * @return false An Error occured, get error using RemoteController::getErrorCode() or RemoteController::getErrorDescription()
	 */
	bool run();

	/**
	 * @brief Sends a command to the other controller without throttle (throttle is set to 255 internally)
	 *
	 * @param command The command to be sent (should be implemented as enum on both RemoteControllers)
	 * @param priority The Priority with which the command should be sent
	 */
	void sendCommand(uint8_t command, Priority priority = Priority::Normal);

	/**
	 * @brief Sends a command including a uint8_t throttle value.
	 *
	 * @param command The command to be sent (should be implemented as enum on both RemoteControllers)
	 * @param throttle a uint8_t (0-255) value to be sent alongside the command for throttle, etc. control
	 * @param priority The Priority with which the command should be sent
	 */
	void sendCommand(uint8_t command, uint8_t throttle, Priority priority = Priority::Normal);

	/**
	 * @brief Sends a binary payload to the other RemoteController. Basically a wrapper for Connection::write()
	 *
	 * @param buffer the binary data to be sent
	 * @param length the length of the data buffer
	 * @return true the payload was transmitted succesfully
	 * @return false failed to transmit the payload, use RemoteController::getErrorCode() or RemoteController::getErrorDescription() for info!
	 */
	bool sendPayload(const void *buffer, size_t length);

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
	/**
	 * @brief Errors that can occur in the RemoteController, these are handled to the best ability by the RemoteControllerClass itself
	 * 
	 */
	enum Error : uint8_t
	{
		NoError /** No Error, RC is running fine*/,
		CannotBeginConnection /** Remote Controller begin failed because it cannot begin its connection, probably because it failes to connect*/,
		FailedToTransmitCommands /** The RemoteController failed to transmit the commands because the Connection didn't succesfully transmit the data*/,
		CommandQueueFull /** The Command Queue is full. Too many commands where added and not transmitted. CAUSES UNDEFINED BEHAVIOR until a connection is established and commands are succesfully transmitted!*/,
		CustomPayloadTooBig /** Buffer overflow prevented: The payload that was tried to be send with RemoteController::sendPayload() was too big for the Connection package buffer*/,
		FailedToTransmitCustomPayload /** The Connection::write() failed to transmit the payload (No ack received)*/
	};
	
	/**
	 * @brief Get the current Error Code 
	 * 
	 * @return uint8_t 8bit error code, defined in RemoteController::Error
	 */
	uint8_t getErrorCode();

	/**
	 * @brief Get the Description of the current Error (Code)
	 * 
	 * @return String Humand readable error description
	 */
	std::string getErrorDescription();
	
#ifndef UNIT_TEST
private:
#endif
	Connection &connection;
	/**
	 * @brief Holds a list of 8bit integers in pairs of two: The first integer is always a Command and the second integer a corresponding throttle value.
	 *
	 */
	std::vector<uint8_t> commandQueue;
	std::function<void(const std::vector<uint8_t> &commands, const std::vector<uint8_t> &throttles)> commandCallbackFunction;
	std::function<void(const void *buffer, size_t length)> payloadCallbackFunction;
	Error error = NoError;

	uint8_t *incomingBuffer = nullptr;
	uint8_t *outgoingBuffer = nullptr;
	std::vector<uint8_t> incomingCommandsBuffer;
	std::vector<uint8_t> incomingThrottlesBuffer;

	bool m_begin();
	bool transmitCommands(const std::vector<uint8_t> &commands);
};

#endif