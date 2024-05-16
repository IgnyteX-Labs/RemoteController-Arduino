#pragma once
#include <ArduinoFake.h>
#include <unity.h>
#include <stddef.h>
#include <stdint.h>

#include "RemoteController.h"

using namespace fakeit;

// RemoteController::sendCommand()

void test_sendCommand_HighPriority()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, size_t length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, *(++pStart));
			return true; });
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, RemoteController::High);
	TEST_ASSERT_TRUE(Verify(Method(mockConnection, write)).Once());
	rc.end();
}

void test_sendCommand_NormalPriority()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, size_t length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, *(++pStart));
			return true; });
	When(Method(mockConnection, available)).Return(false);
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, RemoteController::Normal);
	rc.run();
	TEST_ASSERT_TRUE(Verify(Method(mockConnection, write)).Once());
	rc.end();
}

void test_sendCommandWithThrottle_HighPriority()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, size_t length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(53, *(++pStart));
			return true; });
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, 53, RemoteController::High);
	TEST_ASSERT_TRUE(Verify(Method(mockConnection, write)).Once());
}

void test_sendCommandWithThrottle_NormalPriority()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, size_t length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(53, *(++pStart));
			return true; });
	When(Method(mockConnection, available)).Return(false);
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, 53, RemoteController::Normal);
	rc.run();
	TEST_ASSERT_TRUE(Verify(Method(mockConnection, write)).Once());
	rc.end();
}

void test_sendCommandMultiple_NormalPriority()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, size_t length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(0x03, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(0xBF, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, *(++pStart)); 
			return true; });
	When(Method(mockConnection, available)).Return(false);
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, RemoteController::Normal);
	rc.sendCommand(0x03, RemoteController::Normal);
	rc.sendCommand(0xBF, RemoteController::Normal);
	rc.run();
	TEST_ASSERT_TRUE(Verify(Method(mockConnection, write)).Once());
	rc.end();
}

void test_sendCommandMultipleWithThrottle_NormalPriority()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, size_t length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(53, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(0x03, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(66, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(0xBF, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(255, *(++pStart)); 
			return true; });
	When(Method(mockConnection, available)).Return(false);
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, 53, RemoteController::Normal);
	rc.sendCommand(0x03, 66, RemoteController::Normal);
	rc.sendCommand(0xBF, 255, RemoteController::Normal);
	rc.run();
	TEST_ASSERT_TRUE(Verify(Method(mockConnection, write)).Once());
	rc.end();
}

void test_sendCommandStreaming()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(4);
	int writeCalled = 0;
	When(Method(mockConnection, write))
		.AlwaysDo([&writeCalled](const void *buffer, size_t length) -> bool
				  { 
			writeCalled++;
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_TRUE((*pStart * 256 + *(pStart+1)) == REMOTECONTROLLER_IDENTIFIER_COMMAND); // Check identifier command
			pStart++;
			if(writeCalled == 1) {
				TEST_ASSERT_EQUAL_UINT8(0x01, *(++pStart));
				TEST_ASSERT_EQUAL_UINT8(53, *(++pStart));
			}else {
				TEST_ASSERT_EQUAL_size_t(4, length);
				TEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(2, writeCalled, "The rc should only call Connection::write twice");
				TEST_ASSERT_EQUAL_UINT8(0x03, *(++pStart));
				TEST_ASSERT_EQUAL_UINT8(66, *(++pStart));
			}
			return true; });
	When(Method(mockConnection, available)).Return(false);
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, 53, RemoteController::Normal);
	rc.sendCommand(0x03, 66, RemoteController::Normal);
	rc.run();
	TEST_ASSERT_TRUE(Verify(Method(mockConnection, write)).Exactly(2));
	rc.end();
}

void test_sendCommandPackageSize()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(30);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, size_t length) -> bool
			{ 
			TEST_ASSERT_EQUAL_size_t(6, length);
			return true; });
	When(Method(mockConnection, available)).Return(false);
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x01, 53, RemoteController::Normal); // 2 byte
	rc.sendCommand(0x03, 66, RemoteController::Normal); // 2 byte + 2 byte identifer = 6 byte
	rc.run();
	TEST_ASSERT_TRUE(Verify(Method(mockConnection, write)).Once());
	rc.end();
}