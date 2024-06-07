#ifndef REMOTECONTROLLER_H_
#define REMOTECONTROLLER_H_

#include "ArchConfig.h" // Architecture specific implementation of String, Functional, etc.
#include "RemoteControllerConfig.h" // RemoteController preprocessor configuration (e.g. Buffer sizes, etc.). To use custom config define REMOTECONTROLLER_CUSTOM_CONFIG

#include "Connections/Connection.h"

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

	/**
	 * @brief Construct a new Remote Controller object
	 *
	 * @param connection a reference to a Connection object
	 */
	RemoteController(Connection &connection);
	~RemoteController();

#ifdef RC_ARCH_USE_FUNCTIONAL
	/**
	 * @brief Starts the RemoteController and connects it to the other controller. (Using std::functional)
	 *
	 * @param cmdClb this std::function callback is called when commands are received and passes the following arguments: (1) a c-array with the commands, (2) a c-array with the throttles, (3) the length of those arrays!
	 * @return true succesffully started the RemoteController and connected to the other controller
	 * @return false failed to start or connect
	 */
	bool begin(std::function<void(const uint8_t commands[], const float throttles[], size_t length)> cmdClb);

	/**
	 * @brief Starts the RemoteController and connects it to the other controller
	 *
	 * @param cmdClb this callback function is called when commands are received and passes the following arguments: (1) a c-array with the commands, (2) a c-array with the throttles, (3) the length of those arrays!
	 * @param pldClb this callback function is called when a binary payload is received
	 * @return true succesffully started the RemoteController and connected to the other controller
	 * @return false failed to start or connect
	 */
	bool begin(std::function<void(const uint8_t commands[], const float throttles[], size_t length)> cmdClb, std::function<void(const void *buffer, size_t length)> pldClb);
#else
	/**
	 * @brief Starts the RemoteController and connects it to the other controller. (Using C function pointers)
	 *
	 * @param cmdClb this c-function pointer callback is called when commands are received and passes the following arguments: (1) a c-array with the commands, (2) a c-array with the throttles, (3) the length of those arrays!
	 * @return true succesffully started the RemoteController and connected to the other controller
	 * @return false failed to start or connect
	 */
	bool begin(void (*cmdClb)(const uint8_t commands[], const float throttles[], size_t length));

	/**
	 * @brief Starts the RemoteController and connects it to the other controller
	 *
	 * @param cmdClb this callback function is called when commands are received and passes the following arguments: (1) a c-array with the commands, (2) a c-array with the throttles, (3) the length of those arrays!
	 * @param pldClb this callback function is called when a binary payload is received
	 * @return true succesffully started the RemoteController and connected to the other controller
	 * @return false failed to start or connect
	 */
	bool begin(void (*cmdClb)(const uint8_t commands[], const float throttles[], size_t length), void (*pldClb)(const void *buffer, size_t length));
#endif


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
	 * @brief Sends a command to the other controller without throttle
	 * @note The throttle is set internally (to 0) to comply with RemoteController-Protocol encoding requirements.
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
	void sendCommand(uint8_t command, float throttle, Priority priority = Priority::Normal);

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
		FailedToTransmitCustomPayload /** The Connection::write() failed to transmit the payload (No ack received)*/,
		ReceivedCorruptPacket /** The packet that was received and triggered Connection::available() is corrupt and cannot be read! */
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
	 * @return String Human readable error description
	 */
	_String getErrorDescription();

#ifndef UNIT_TEST
private:
#endif
	Connection &connection;
	/**
	 * @brief Holds a list of Commands per RemoteController-Protocol v1.0.0: 8-bit instruction 32-bit float throttle.
	 *
	 */
	uint8_t commandQueue[REMOTECONTROLLER_COMMAND_QUEUE_SIZE];
	size_t commandQueueIndex = 0; // The currently free index in the command queue to write to (needs to be checked for out-of-bounds before writing)
	
	uint8_t incomingBuffer[REMOTECONTROLLER_INCOMING_BUFFER_SIZE];
	uint8_t outgoingBuffer[REMOTECONTROLLER_OUTGOING_BUFFER_SIZE];
	
	uint8_t incomingCommandsBuffer[REMOTECONTROLLER_INCOMING_CALLBACK_ARRAY_LENGTH];
	float incomingThrottlesBuffer[REMOTECONTROLLER_INCOMING_CALLBACK_ARRAY_LENGTH];

#if defined(RC_ARCH_USE_FUNCTIONAL)
	std::function<void(const uint8_t commands[], const float throttles[], size_t length)> commandCallbackFunction;
	std::function<void(const void *buffer, size_t length)> payloadCallbackFunction;
#else
	void (*commandCallbackFunction)(const uint8_t commands[], const float throttles[], size_t length);
	void (*payloadCallbackFunction)(const void *buffer, size_t length);

#endif

	Error error = NoError;
	bool m_begin();
	void addToCommandQueue(uint8_t command, float throttle);
	bool transmitCommands(const uint8_t commands[], size_t length);
	void encodeCommand(uint8_t command, float throttle, uint8_t *buffer);
};

#endif