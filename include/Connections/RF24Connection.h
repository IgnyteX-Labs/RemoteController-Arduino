#ifndef RF24CONNECTION_H_
#define RF24CONNECTION_H_

#include "Connection.h"

class RF24Connection: public Connection {
public:
	RF24Connection();
	bool begin();
	void end();
	bool available();
	void read(void *buffer, unsigned int length);
	int getPayloadSize();
	bool write(const void *buffer, unsigned int length);
	const int maxPackageSize = 32;
};

#endif