#include "Connections/RF24Connection.h"

RF24Connection::RF24Connection()
{
}

bool RF24Connection::begin()
{
}

void RF24Connection::end()
{
}
bool RF24Connection::available()
{
}
void RF24Connection::read(void *buffer, unsigned int length)
{
}

int RF24Connection::getPayloadSize()
{
}

bool RF24Connection::write(const void *buffer, unsigned int length)
{
}

int RF24Connection::getMaxPackageSize() {
	return 32;
}
