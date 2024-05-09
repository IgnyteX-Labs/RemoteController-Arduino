#include <ArduinoFake.h>
#include <unity.h>

#include "RemoteController.h"

using namespace fakeit;

void test_begin()
{
	Mock<Connection> mockConnection;

	// Test if the begin failes when connection.begin fails...
	When(Method(mockConnection, begin)).Return(true); // Succesfully begin

	RemoteController rc(mockConnection.get());

	TEST_ASSERT_TRUE(rc.begin(nullptr));

	When(Method(mockConnection, begin)).Return(false); // Fail to begin

	TEST_ASSERT_FALSE(rc.begin(nullptr));
}


// Test sending commands...
void test_sendCommand_HighPriority()
{
	Mock<Connection> mockConnection;

	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, unsigned int length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, *(++pStart));
			}).Return(true);

	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, RemoteController::High);
}

void test_sendCommand_NormalPriority() {
	Mock<Connection> mockConnection;

	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, unsigned int length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, *(++pStart)); })
		.Return(true);
	When(Method(mockConnection, available)).Return(false);

	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, RemoteController::Normal);
	rc.run();
}

void test_sendCommandWithThrottle_HighPriority()
{
	Mock<Connection> mockConnection;

	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, unsigned int length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(53, *(++pStart)); })
		.Return(true);
	
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, 53, RemoteController::High);
}

void test_sendCommandWithThrottle_NormalPriority()
{
	Mock<Connection> mockConnection;

	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, unsigned int length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(53, *(++pStart)); })
		.Return(true);
	When(Method(mockConnection, available)).Return(false);

	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, 53, RemoteController::Normal);
	rc.run();
}

void test_sendCommandMultiple_NormalPriority()
{
	Mock<Connection> mockConnection;

	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, unsigned int length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(0x03, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(0xBF, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, *(++pStart)); })
		.Return(true);
	When(Method(mockConnection, available)).Return(false);


	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, RemoteController::Normal);
	rc.sendCommand(0x03, RemoteController::Normal);
	rc.sendCommand(0xBF, RemoteController::Normal);
	rc.run();
}

void test_sendCommandMultipleWithThrottle_NormalPriority()
{
	Mock<Connection> mockConnection;

	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, unsigned int length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(53, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(0x03, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(66, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(0xBF, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(255, *(++pStart)); })
		.Return(true);
	When(Method(mockConnection, available)).Return(false);

	RemoteController rc(mockConnection.get());	
	rc.begin(nullptr);
	rc.sendCommand(0x01, 53, RemoteController::Normal);
	rc.sendCommand(0x03, 66, RemoteController::Normal);
	rc.sendCommand(0xBF, 255, RemoteController::Normal);
	rc.run();
}

void setUp(void)
{
	// set stuff up here
	ArduinoFakeReset();
}

void tearDown(void)
{
	// clean stuff up here
}

int main(int argc, char **argv)
{
	UNITY_BEGIN();
	RUN_TEST(test_begin);
	RUN_TEST(test_sendCommand_HighPriority);
	RUN_TEST(test_sendCommand_NormalPriority);
	RUN_TEST(test_sendCommandWithThrottle_HighPriority);
	RUN_TEST(test_sendCommandWithThrottle_NormalPriority);
	RUN_TEST(test_sendCommandMultiple_NormalPriority);
	RUN_TEST(test_sendCommandMultipleWithThrottle_NormalPriority);

	UNITY_END();
}
