#include "Connections/RF24Connection.h"

RF24Connection::RF24Connection(int cepin, int cspin) : rf24(RF24(cepin, cspin)), rf24_address("RF000")
{
}

RF24Connection::RF24Connection(RF24 &rf24) : rf24(rf24), rf24_address("RF000")
{
	isRF24Initialized = true;
}

RF24Connection::RF24Connection() : rf24(RF24()), rf24_address("RF000")
{
}

void RF24Connection::useSpecificSPIBus(_SPI *spiBus)
{
	nonDefaultSPI = spiBus;
}

bool RF24Connection::begin()
{
	if (!isRF24Initialized)
	{
		// Connect to the NRF24L01 module
		if (nonDefaultSPI)
		{
			// If a non-default SPI bus was specified use this one
			if (!rf24.begin(nonDefaultSPI))
				return false;
		}
		else
		{
			// Use default SPI bus
			if (!rf24.begin())
				return false;
		}
		// NRF24L01 configuration
		rf24.setPALevel(RF24_PA_HIGH);

		isRF24Initialized = true;
	}
	// Required NRF24L01 settings for RF24 to work
	rf24.enableDynamicPayloads();
	rf24.setAutoAck(true);
	rf24.openReadingPipe(1, rf24_address);
	rf24.startListening();
	return true;
}

void RF24Connection::end()
{
	rf24.closeReadingPipe(1);
	rf24.powerDown();
	isRF24Initialized = false;
}

bool RF24Connection::available()
{
	return rf24.available();
}

void RF24Connection::read(void *buffer, size_t length)
{
	rf24.read(buffer, length);
}

size_t RF24Connection::getPayloadSize()
{
	return rf24.getDynamicPayloadSize();
}

bool RF24Connection::write(const void *buffer, size_t length)
{
	rf24.stopListening();
	rf24.closeReadingPipe(1);
	rf24.openWritingPipe(rf24_address);
	bool success = rf24.write(buffer, length);
	rf24.openReadingPipe(1, rf24_address);
	rf24.startListening();
	return success;
}

size_t RF24Connection::getMaxPackageSize()
{
	return maxPackageSize;
}
