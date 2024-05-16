#ifndef RF24CONNECTION_H_
#define RF24CONNECTION_H_

#include "Connection.h"
#include <RF24.h>

#define REMOTECONTROLLER_RF24CONNECTION_DEFAULT_ADDRESS "RF000"

class RF24Connection : public Connection
{
public:
	/**
	 * @name Implementations of Connection Class Functions
	 *
	 * RF24 implementation of the required methods to conform to @ref Connection
	 *
	 */
	/**@{*/

	bool begin();
	void end();
	bool available();
	void read(void *buffer, size_t length);
	size_t getPayloadSize();
	bool write(const void *buffer, size_t length);
	size_t getMaxPackageSize();
	
	/**@}*/
	/**
	 * @name RF24Connection Specific Functions
	 *
	 * Specific Constructors and Methods for settings, etc.
	 */
	/**@{*/

	/**
	 *
	 * @brief Construct a new RF24Connection object
	 *
	 * @param cepin The pin attached to Chip Enable on the RF module
	 * @param cspin The pin attached to Chip Select (often labeled CSN) on the radio module.
	 * @param address (Optional) The 5 byte address of the RemoteController. Needs to be the same for paired RemoteControllers!
	 * @note When using a c-string as address account for the null-terminator (a 6th byte), thus use const uint8_t[6]
	 *
	 */
	RF24Connection(int cepin, int cspin, const uint8_t *address = REMOTECONTROLLER_RF24CONNECTION_DEFAULT_ADDRESS);

	/**
	 * @brief Construct a new RF24Connection object
	 *
	 * @param rf24 An already constructed and configured RF24 object
	 * @param address (Optional) The 5 byte address of the RemoteController. Needs to be the same for paired RemoteControllers!
	 * 
	 * @note When using a c-string as address account for the null-terminator (a 6th byte), thus use const uint8_t[6]
	 * 
	 */
	RF24Connection(RF24 &rf24, const uint8_t *address = REMOTECONTROLLER_RF24CONNECTION_DEFAULT_ADDRESS);

	/**
	 * @brief Allows to optionally specify a non-default SPI bus to use.
	 * 
	 * @warning This function must be called before RF24Connection::begin()
	 * @warning Cannot be used together with RF24Connection::RF24Connection(RF24 &rf24)
	 * 
	 * @param spiBus A pointer or reference to an instantiated SPI bus object. The _SPI datatype is declared in the RF24 library to allow different SPI implementations.
	 */
	void useSpecificSPIBus(_SPI *spiBus);

	/**
	 * @brief The NRF24L01 module supports packages of size 4 bytes to 32 bytes.
	 * 
	 */
	const size_t maxPackageSize = 32;

	/**@}*/
private:
	RF24 rf24;
	_SPI *nonDefaultSPI = nullptr;
	bool isRF24Initialized = false; // To not call rf24.begin if true
	uint8_t rf24_address[5];
};

#endif