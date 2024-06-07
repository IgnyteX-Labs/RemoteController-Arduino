#ifndef REMOTECONTROLLERCONNECTION_H_
#define REMOTECONTROLLERCONNECTION_H_
#include <stddef.h>

/**
 * @brief This abstract class interfaces between the RemoteController and any way of transmitting the data for the RemoteController. E.g. WiFi and RF24.
 *
 */
class Connection
{
public:
	/**
	 * @brief Connects to and configures any modules needed by the Connection to transceive/receive
	 *
	 * @return true if the connection to and configuration of the required modules was successful
	 * @return false if failed to configure or connect
	 */
	virtual bool begin() = 0;
	/**
	 * @brief Closes the connection and frees all the memory occupied
	 * 
	 */
	virtual void end() = 0;
	
	/**
	 * @brief This function checks for incoming data and is called repeteadly!
	 * 
	 * @return true New data is available and needs to be handled
	 * @return false No data available can be skipped
	 */
	virtual bool available() = 0;

	/**
	 * @brief This function actually reads the available incoming data
	 * 
	 * @param buffer to store the incoming data to
	 * @param length length/size of the buffer
	 */
	virtual void read(void *buffer, size_t length) = 0;

	/**
	 * @brief Get the size of the payload that will be read with Connection::read()
	 * 
	 * @return size_t payload size in bytes, should return <1  if the payload is corrupt
	 */
	virtual size_t getPayloadSize() = 0;

	/**
	 * @brief Writes data to the other RemoteController
	 * 
	 * @param buffer where the data to transmit is stored
	 * @param length length/size of the payload/buffer
	 * @return true succesfull transmission (ack received)
	 * @return false failed to transmit
	 */
	virtual bool write(const void *buffer, size_t length) = 0;

	/**
	 * @brief The maximum size, in byte, that can be send in one package i.e. with one Connection::write() call.
	 * @warning If this is smaller than the size of one command (i.e. 2 bytes (identifer) + 5 bytes (command) = 7 bytes) you will encounter unexpected problems.
	 * 
	 */
	virtual size_t getMaxPackageSize() = 0;
};

#endif