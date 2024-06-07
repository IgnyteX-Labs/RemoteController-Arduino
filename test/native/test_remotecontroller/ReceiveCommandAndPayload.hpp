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
		*(float *)(++pStart) = (float)200;
		pStart += 4;
		*pStart = 0x55;
		*(float *)(++pStart) = (float)0; });
	When(Method(mockConnection, getPayloadSize)).AlwaysReturn(12);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);

	RemoteController rc(mockConnection.get());
	int clb = 0;
	rc.begin([&clb](const uint8_t commands[], const float throttle[], size_t length) -> void
			 { 
		clb += 1;
		TEST_ASSERT_EQUAL_size_t(2, length);
		TEST_ASSERT_EQUAL_UINT8(0x32, commands[0]);
		TEST_ASSERT_EQUAL_FLOAT(200, throttle[0]);
		TEST_ASSERT_EQUAL_UINT8(0x55, commands[1]);
		TEST_ASSERT_EQUAL_FLOAT(0, throttle[1]); });
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
	When(Method(mockConnection, getPayloadSize)).AlwaysReturn(16);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);

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