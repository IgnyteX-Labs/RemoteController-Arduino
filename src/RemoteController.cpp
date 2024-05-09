#include "RemoteController.h"
#include "Connections/Connection.h"
#include "Connections/RF24Connection.h"

RemoteController::RemoteController() : connection(*(new RF24Connection()))
{
}

RemoteController::RemoteController(Connection &connection) : connection(connection)
{
}

RemoteController::~RemoteController()
{

}

bool RemoteController::begin(std::function<void(const std::vector<std::uint8_t> &commands, const std::vector<std::uint8_t> &throttle)> cmdClb)
{
	commandCallbackFunction = cmdClb;

	return m_begin();
}

bool RemoteController::begin(std::function<void(const std::vector<std::uint8_t> &commands, const std::vector<std::uint8_t> &throttle)> cmdClb, std::function<void(const void *buffer, std::size_t length)> pldClb)
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
		/// TODO: Implement error handling
		return false;
	}

	incomingBuffer = new std::uint8_t[REMOTECONTROLLER_INCOMING_BUFFER_SIZE];
	outgoingBuffer = new std::uint8_t[REMOTECONTROLLER_OUTGOING_BUFFER_SIZE];
	incomingCommandsBuffer.reserve(10);
	incomingThrottlesBuffer.reserve(10);
	commandQueue.reserve(20);

	return true;
}

void RemoteController::end()
{
	connection.end();
	commandQueue.clear();

	delete[] incomingBuffer;
	delete[] outgoingBuffer;
}

void RemoteController::run()
{
	// Transmit the queued commands to the receiver
	if (transmitCommands(commandQueue))
	{
	commandQueue.clear(); // Clear the command queue as those commands where succesfully transmitted
	}else {
		/// TODO: Implement error handling
		///-> Log error message and keep command queue to hopefully be transmitted with the next run() call
	}
	// Check and process incomming commands and payloads
	if (connection.available())
	{
		connection.read(incomingBuffer, sizeof(REMOTECONTROLLER_INCOMING_BUFFER_SIZE));
		size_t payloadSize = std::min(connection.getPayloadSize(), REMOTECONTROLLER_INCOMING_BUFFER_SIZE);
		std::uint8_t *pStart = incomingBuffer;
		std::uint8_t *pEnd = incomingBuffer + payloadSize;

		// Check the first two bytes of the buffer for RemoteController Command identifier
		if ((*pStart * 256 + *(++pStart)) == REMOTECONTROLLER_IDENTIFIER_COMMAND)
		{
			while (pStart != pEnd)
			{
				incomingCommandsBuffer.push_back(*(++pStart));
				incomingThrottlesBuffer.push_back(*(++pStart));
			}
			// Commands successfully parsed
			commandCallbackFunction(incomingCommandsBuffer, incomingThrottlesBuffer);
			return;
		}
		else
		{
			// Non Command type payload received...
			payloadCallbackFunction(incomingBuffer, payloadSize);
			return;
		}
	}
}

void RemoteController::sendCommand(std::uint8_t command, RemoteControllerPriority priority)
{
	sendCommand(command, UINT8_MAX, priority);
}

void RemoteController::sendCommand(std::uint8_t command, std::uint8_t throttle, RemoteControllerPriority priority)
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
			/// TODO: Implement error handling
			/// - Failed to transmit log error message and add to commandqueue to transmit the command later
		}
		return;
	}
}

void RemoteController::sendPayload(const void *buffer, size_t length) {

}

bool RemoteController::transmitCommands(const std::vector<std::uint8_t> &commands)
{
	// Stream the command data if neccessary -> The first two bytes of each package are the IDENTIFIER COMMAND
	int commandsSent = 0;

	while (commandsSent < commands.size())
	{
		int commandsInPacket = 0;
		std::uint8_t *pStart = outgoingBuffer;
		
		// Encode the REMOTECONTROLLER_IDENTIFIER_COMMAND into the binary payload
		*pStart = (std::uint8_t)(REMOTECONTROLLER_IDENTIFIER_COMMAND >> 8);
		*(++pStart) = (std::uint8_t)REMOTECONTROLLER_IDENTIFIER_COMMAND;

		// Encode the commands
		while ((commandsInPacket + 2) <= std::min(REMOTECONTROLLER_OUTGOING_BUFFER_SIZE, connection.maxPackageSize) && commandsSent + commandsInPacket <= commands.size())
		{
			*(++pStart) = commands[commandsSent + commandsInPacket];
			commandsInPacket++;
		}

		// Try to transmit the payload
		if(!connection.write(outgoingBuffer, commandsInPacket+2)) {
			return false;
		}
		commandsSent += commandsInPacket;
	}
	return true;
}
