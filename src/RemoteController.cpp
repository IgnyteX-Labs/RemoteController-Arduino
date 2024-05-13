#include "RemoteController.h"
#include "Connections/Connection.h"

RemoteController::RemoteController(Connection &connection) : connection(connection)
{
}

RemoteController::~RemoteController()
{
}

bool RemoteController::begin(std::function<void(const std::vector<uint8_t> &commands, const std::vector<uint8_t> &throttle)> cmdClb)
{
	commandCallbackFunction = cmdClb;

	return m_begin();
}

bool RemoteController::begin(std::function<void(const std::vector<uint8_t> &commands, const std::vector<uint8_t> &throttle)> cmdClb, std::function<void(const void *buffer, size_t length)> pldClb)
{
	// Assign callbacks
	commandCallbackFunction = cmdClb;
	payloadCallbackFunction = pldClb;

	return m_begin();
}

bool RemoteController::m_begin()
{
	// Initialize connection to remote
	if (!connection.begin())
	{
		// Failed to start the connection
		error = CannotBeginConnection;
		return false;
	}

	incomingBuffer = new uint8_t[REMOTECONTROLLER_INCOMING_BUFFER_SIZE];
	outgoingBuffer = new uint8_t[REMOTECONTROLLER_OUTGOING_BUFFER_SIZE];
	incomingCommandsBuffer.reserve(10);
	incomingThrottlesBuffer.reserve(10);
	commandQueue.reserve(20);

	error = NoError;
	return true;
}

void RemoteController::end()
{
	connection.end();
	commandQueue.clear();
	commandQueue.shrink_to_fit();

	delete[] incomingBuffer;
	delete[] outgoingBuffer;
	incomingBuffer = nullptr;
	outgoingBuffer = nullptr;
}

uint8_t RemoteController::getErrorCode()
{
	return error;
}

std::string RemoteController::getErrorDescription()
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
	default:
		return "Unknown Error";
	}
}

bool RemoteController::run()
{
	// Transmit the queued commands to the receiver
	if (commandQueue.size() > REMOTECONTROLLER_MAX_COMMAND_QUEUE_SIZE)
	{
		commandQueue.clear();
		error = CommandQueueFull;
		return false;
	}
	if (!commandQueue.empty())
	{
		if (transmitCommands(commandQueue))
		{
			commandQueue.clear(); // Clear the command queue as those commands where succesfully transmitted
		}
		else
		{
			return false; // Return error message and keep command queue to hopefully be transmitted with the next run() call
		}
	}
	// Check and process incomming commands and payloads
	if (connection.available())
	{
		connection.read(incomingBuffer, REMOTECONTROLLER_INCOMING_BUFFER_SIZE);
		size_t payloadSize = std::min((int)connection.getPayloadSize(), REMOTECONTROLLER_INCOMING_BUFFER_SIZE);
		uint8_t *pStart = incomingBuffer;

		// Check the first two bytes of the buffer for RemoteController Command identifier
		if ((*pStart * 256 + *(pStart + 1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND)
		{
			pStart++;
			int len = (payloadSize-2)/2; // The amount of incoming command&throttle pairs (-2 bytes because of the identifier and /2 as one pair is 2 bytes big)
			while (len--)
			{
				incomingCommandsBuffer.push_back(*(++pStart));
				incomingThrottlesBuffer.push_back(*(++pStart));
			}
			// Commands successfully parsed

			if (commandCallbackFunction)
			{
				commandCallbackFunction(incomingCommandsBuffer, incomingThrottlesBuffer);
			}
			
			// Clear the incoming vector buffers after calling the callback
			incomingCommandsBuffer.clear();
			incomingThrottlesBuffer.clear();
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
	sendCommand(command, UINT8_MAX, priority);
}

void RemoteController::sendCommand(uint8_t command, uint8_t throttle, Priority priority)
{
	if (priority == Normal)
	{
		// Check if the command queue is full...
		commandQueue.push_back(command);
		commandQueue.push_back(throttle);
		return;
	}
	else if (priority == High)
	{
		if (!transmitCommands({command, throttle}))
		{
			/// - Failed to transmit log error message and add to commandqueue to transmit the command later
			commandQueue.push_back(command);
			commandQueue.push_back(throttle);
			error = FailedToTransmitCommands;
		}
		return;
	}
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

bool RemoteController::transmitCommands(const std::vector<uint8_t> &commands)
{
	// Stream the command data if neccessary -> The first two bytes of each package are the IDENTIFIER COMMAND
	int commandsSent = 0;

	while (commandsSent < commands.size())
	{
		int commandsInPacket = 0;
		uint8_t *pStart = outgoingBuffer;

		// Encode the REMOTECONTROLLER_IDENTIFIER_COMMAND into the binary payload
		*pStart = (uint8_t)(REMOTECONTROLLER_IDENTIFIER_COMMAND >> 8);
		*(++pStart) = (uint8_t)REMOTECONTROLLER_IDENTIFIER_COMMAND;

		// Encode the commands
		while ((commandsInPacket + 2) < std::min(REMOTECONTROLLER_OUTGOING_BUFFER_SIZE, (int)connection.getMaxPackageSize()) && commandsSent + commandsInPacket < commands.size())
		{
			commandsInPacket++;
			*(++pStart) = commands[commandsSent + (commandsInPacket - 1)];
		}

		// Try to transmit the payload
		if (!connection.write(outgoingBuffer, commandsInPacket + 2))
		{
			error = FailedToTransmitCommands;
			return false;
		}
		commandsSent += commandsInPacket;
	}
	return true;
}
