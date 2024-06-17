#ifndef REMOTECONTROLLER_SPICONNECTION_H_
#define REMOTECONTROLLER_SPICONNECTION_H_

#include "Connection.h"
#include <SPI.h>

/**
 * @brief A Connection Implementation using SPI for a wired connection between Remote and Receiver
 *
 */
class SPIConnection : Connection
{
	/**
	 * @name Implementations of Connection Class Functions
	 *
	 * SPIConnection implementation of the required methods to conform to @ref Connection
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
	 * @name SPIConnection Specific Functions
	 *
	 * Specific Constructors and Methods for settings, etc.
	 */
	/**@{*/

	/**@}*/
};

#endif