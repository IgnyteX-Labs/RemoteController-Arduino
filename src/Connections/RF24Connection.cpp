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
void RF24Connection::read(void *buffer, size_t length)
{
}

size_t RF24Connection::getPayloadSize()
{
}

bool RF24Connection::write(const void *buffer, size_t length)
{
}

size_t RF24Connection::getMaxPackageSize() {
	return 32;
}
