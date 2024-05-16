#pragma once
#include <ArduinoFake.h>
#include <unity.h>
#include <stddef.h>
#include <stdint.h>

#include "RemoteController.h"

using namespace fakeit;

// RemoteController command/payload callback

void test_receiveCommands()
{
	Mock<Connection> mockConnection;
	When(Method(mockConnection, begin)).Return(true);
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, available)).AlwaysReturn(true);
	When(Method(mockConnection, read)).AlwaysDo([](void *buffer, size_t length) -> void
										  {
		uint8_t *pStart = reinterpret_cast<uint8_t *>(buffer);
		*(pStart) = (uint8_t)(REMOTECONTROLLER_IDENTIFIER_COMMAND >> 8);
		*(++pStart) = (uint8_t)REMOTECONTROLLER_IDENTIFIER_COMMAND;
		*(++pStart) = 0x32;
		*(++pStart) = 200;
		*(++pStart) = 0x55;
		*(++pStart) = UINT8_MAX; });
	When(Method(mockConnection, getPayloadSize)).AlwaysReturn(6);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);

	RemoteController rc(mockConnection.get());
	int clb = 0;
	rc.begin([&clb](const std::vector<uint8_t> &commands, const std::vector<uint8_t> &throttle) -> void
			 { 
		clb += 1;
		TEST_ASSERT_EQUAL_size_t(2, commands.size());
		TEST_ASSERT_EQUAL_size_t(2, throttle.size());
		TEST_ASSERT_EQUAL_UINT8(0x32, commands.at(0));
		TEST_ASSERT_EQUAL_UINT8(200, throttle.at(0));
		TEST_ASSERT_EQUAL_UINT8(0x55, commands.at(1));
		TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, throttle.at(1)); });
	// Test receive first batch of commands
	rc.run();
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, clb, "The Callback function has to be called exactly once!");
	// Test a second batch of commands
	rc.run();
	TEST_ASSERT_EQUAL_INT_MESSAGE(2, clb, "The Callback function should have been called exactly twice!");
	rc.end();
}

void test_receivePayload()
{
	Mock<Connection> mockConnection;
	When(Method(mockConnection, begin)).Return(true);
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, available)).Return(true);
	When(Method(mockConnection, read)).Do([](void *buffer, size_t length) -> void
										  {
		uint8_t *pStart = reinterpret_cast<uint8_t *>(buffer);
		int len = 16;
		while(len--) {
			*(pStart++) = 0x55;
		} });
	When(Method(mockConnection, getPayloadSize)).Return(16);
	When(Method(mockConnection, getMaxPackageSize)).Return(32);

	RemoteController rc(mockConnection.get());
	int clb = 0;
	rc.begin(nullptr, [&clb](const void *buffer, size_t length) -> void
			 { 
				const uint8_t *pStart = (const uint8_t*) buffer;
				int len = 16; 
			 while(len--) {
				 TEST_ASSERT_EQUAL_UINT8(0x55, *(pStart++));
				 clb++;
			 } });
	rc.run();
	TEST_ASSERT_EQUAL_INT_MESSAGE(16, clb, "Exactly 16 bytes of data have to be received");
	rc.end();
}