#ifndef REMOTECONTROLLERCONNECTION_H_
#define REMOTECONTROLLERCONNECTION_H_

/**
 * @brief This abstract class interfaces between the RemoteController and any way of transmitting the data for the RemoteController. E.g. WiFi and RF24.
 *
 */
class Connection
{
public:
	/**
	 * @brief Initiates and tests the Connection to the other RemoteController (the other device)
	 *
	 * @return true The connection was initiated and tested successfully
	 * @return false Could not connect to the other RemoteController
	 */
	virtual bool begin() = 0;
	/**
	 * @brief Closes the connection to the other RemoteController and frees all the memory occupied
	 * 
	 */
	virtual void end() = 0;
	
	/**
	 * @brief This function checks for incoming data and needs to be called repeteadly!
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
	virtual void read(void *buffer, unsigned int length) = 0;

	/**
	 * @brief Get the size of the payload that will be read with Connection::read()
	 * 
	 * @return int
	 */
	virtual int getPayloadSize() = 0;

	/**
	 * @brief Writes data to the other RemoteController
	 * 
	 * @param buffer where the data to transmit is stored
	 * @param length length/size of the payload
	 * @return true succesfully transmission
	 * @return false failed to transmit
	 */
	virtual bool write(const void *buffer, unsigned int length) = 0;

	/**
	 * @brief The maximum size, in byte, that can be send in one package i.e. with one Connection::write() call. HAS TO BE AN EVEN NUMBER!!
	 * 
	 */
	const int maxPackageSize = 0;
};

#endif