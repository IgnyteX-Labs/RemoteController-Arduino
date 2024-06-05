#include "RemoteController.h"
#include "Connections/Connection.h"

RemoteController::RemoteController(Connection &connection) : connection(connection)
{
}

RemoteController::~RemoteController()
{
}

#if defined(RC_ARCH_USE_FUNCTIONAL)
bool RemoteController::begin(std::function<void(const uint8_t commands[], const float throttles[], size_t length)> cmdClb)
{
	commandCallbackFunction = cmdClb;

	return m_begin();
}

bool RemoteController::begin(std::function<void(const uint8_t commands[], const float throttles[], size_t length)> cmdClb, std::function<void(const void *buffer, size_t length)> pldClb)
{
	// Assign callbacks
	commandCallbackFunction = cmdClb;
	payloadCallbackFunction = pldClb;

	return m_begin();
}
#else
bool RemoteController::begin(void (*cmdClb)(const uint8_t commands[], const float throttles[], size_t length))
{
	commandCallbackFunction = cmdClb;
	payloadCallbackFunction = NULL;

	return m_begin();
}

bool RemoteController::begin(void (*cmdClb)(const uint8_t commands[], const float throttles[], size_t length), void (*pldClb)(const void *buffer, size_t length))
{
	commandCallbackFunction = cmdClb;
	payloadCallbackFunction = pldClb;

	return m_begin();
}

#endif

bool RemoteController::m_begin()
{
	// Initialize connection to remote
	if (!connection.begin())
	{
		// Failed to start the connection
		error = CannotBeginConnection;
		return false;
	}

	error = NoError;
	return true;
}

void RemoteController::end()
{
	// At the moment no RemoteController specific dynamically allocated memory to free...
	connection.end();
}

uint8_t RemoteController::getErrorCode()
{
	return error;
}

_String RemoteController::getErrorDescription()
{
	switch (error)
	{
	case NoError:
		return "No Error, RC is running fine";
	case CannotBeginConnection:
		return "Remote Controller begin failed because it cannot begin its connection, probably because it failes to connect";
	case FailedToTransmitCommands:
		return "The RemoteController failed to transmit the commands because the Connection didn't succesfully transmit the data";
	case CommandQueueFull:
		return "The Command Queue is full. Too many commands where added and not transmitted. CAUSES UNDEFINED BEHAVIOR until a connection is established and commands are succesfully transmitted!";
	case FailedToTransmitCustomPayload:
		return "The Connection::write() failed to transmit the payload (No ack received)";
	case ReceivedCorruptPacket:
		return "The packet that was received and triggered Connection::available() is corrupt and cannot be read!";
	default:
		return "Unknown Error";
	}
}

bool RemoteController::run()
{
	// Transmit the queued commands to the receiver
	if (commandQueueIndex != 0)
	{
		if (transmitCommands(commandQueue, commandQueueIndex))
		{
			commandQueueIndex = 0; // Clear the command queue as those commands where succesfully transmitted
								   // Note: the actual data is not cleared as it will be overwritten as needed by new data
			// Check if the command queue was overfilled...
			if (error == CommandQueueFull)
			{
				return false;
			}
		}
		else
		{
			return false; // Return error message and keep command queue to hopefully be transmitted with the next run() call
		}
	}
	// Check and process incomming commands and payloads
	if (connection.available())
	{
		size_t payloadSize = rcmin((int)connection.getPayloadSize(), REMOTECONTROLLER_INCOMING_BUFFER_SIZE);
		// Check if the packet is corrupt
		if (payloadSize < 1)
		{
			error = ReceivedCorruptPacket;
			return false;
		}
		// Read the valid packet into the buffer
		connection.read(incomingBuffer, payloadSize);
		uint8_t *pStart = incomingBuffer;

		// Check the first two bytes of the buffer for RemoteController Command identifier
		if ((*pStart * 256 + *(pStart + 1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND)
		{
			pStart += 2;
			int len = (payloadSize - 2) / 5; // The amount of incoming command&throttle pairs (-2 bytes because of the identifier and /5 as one pair is 5 bytes big)
			int bufferIndex = 0;
			while (len--)
			{
				incomingCommandsBuffer[bufferIndex] = *(pStart++);
				incomingThrottlesBuffer[bufferIndex] = *((float *)pStart);
				pStart += 4;
				bufferIndex++;
			}
			// Commands successfully parsed

			if (commandCallbackFunction)
			{
				commandCallbackFunction(incomingCommandsBuffer, incomingThrottlesBuffer, bufferIndex);
			}
		}
		else
		{
			// Non Command type payload received...
			if (payloadCallbackFunction)
			{
				payloadCallbackFunction(incomingBuffer, payloadSize);
			}
		}
	}

	error = NoError;
	return true;
}

void RemoteController::sendCommand(uint8_t command, Priority priority)
{
	sendCommand(command, 0, priority);
}

void RemoteController::sendCommand(uint8_t command, float throttle, Priority priority)
{
	if (priority == Normal)
	{
		addToCommandQueue(command, throttle);
	}
	else if (priority == High)
	{
		uint8_t encodedCommand[5];
		encodeCommand(command, throttle, encodedCommand);
		if (!transmitCommands(encodedCommand, 5))
		{
			/// - Failed to transmit log error message and add to commandqueue to transmit the command later
			addToCommandQueue(command, throttle);
			error = FailedToTransmitCommands;
		}
	}
}

void RemoteController::addToCommandQueue(uint8_t command, float throttle)
{
	// Check if the command queue is full...
	if (commandQueueIndex >= REMOTECONTROLLER_COMMAND_QUEUE_SIZE)
	{
		error = CommandQueueFull;
		return;
	}

	encodeCommand(command, throttle, commandQueue + commandQueueIndex);
	commandQueueIndex += (sizeof command + sizeof throttle);
}

void RemoteController::encodeCommand(uint8_t command, float throttle, uint8_t *buffer)
{
	*buffer++ = command;
	*(float *)buffer = throttle;
}

bool RemoteController::sendPayload(const void *buffer, size_t length)
{
	if (length > connection.getMaxPackageSize())
	{
		error = CustomPayloadTooBig;
		return false;
	}
	if (!connection.write(buffer, length))
	{
		error = FailedToTransmitCustomPayload;
		return false;
	}
	return true;
}

bool RemoteController::transmitCommands(const uint8_t commands[], size_t length)
{
	// Stream the command data if neccessary -> The first two bytes of each package are the IDENTIFIER COMMAND
	int bytesSent = 0;
	const int maxPackageSize = rcmin(REMOTECONTROLLER_OUTGOING_BUFFER_SIZE, (int)connection.getMaxPackageSize()); // Actual maximum size in byte that can be sent in one packet
	int maxPackedSize; // The maxPackedSize is set to the maximum size the exactly fits 2bytes identifer and x many commands into the actual maximum package size that is possible
	maxPackedSize = (((int)((maxPackageSize - 2) / REMOTECONTROLLER_ENCODED_COMMAND_SIZE)) * REMOTECONTROLLER_ENCODED_COMMAND_SIZE) + 2;
	while (bytesSent < length)
	{
		int bytesInPacket = 0;
		uint8_t *pStart = outgoingBuffer;

		// Encode the REMOTECONTROLLER_IDENTIFIER_COMMAND into the binary payload
		*pStart = (uint8_t)(REMOTECONTROLLER_IDENTIFIER_COMMAND >> 8);
		*(++pStart) = (uint8_t)REMOTECONTROLLER_IDENTIFIER_COMMAND;

		// Encode the commands
		while ((bytesInPacket + 2) < maxPackedSize && bytesSent + bytesInPacket < length)
		{
			bytesInPacket++;
			*(++pStart) = commands[bytesSent + (bytesInPacket - 1)];
		}

		// Try to transmit the payload
		if (!connection.write(outgoingBuffer, bytesInPacket + 2))
		{
			error = FailedToTransmitCommands;
			return false;
		}
		bytesSent += bytesInPacket;
	}
	return true;
}
