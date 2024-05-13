#pragma once
#include <ArduinoFake.h>
#include <unity.h>
#include <stddef.h>
#include <stdint.h>

#include "RemoteController.h"

using namespace fakeit;

void test_begin()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));

	// Test if the begin failes when connection.begin fails...

	RemoteController rc(mockConnection.get());

	When(Method(mockConnection, begin)).Return(true); // Succesfully begin
	TEST_ASSERT_TRUE(rc.begin(nullptr));
	rc.end();

	When(Method(mockConnection, begin)).Return(false); // Fail to begin
	TEST_ASSERT_FALSE(rc.begin(nullptr, nullptr));

	When(Method(mockConnection, begin)).Return(true); // Succesfully begin
	TEST_ASSERT_TRUE(rc.begin(nullptr, nullptr));
	rc.end();

	When(Method(mockConnection, begin)).Return(false); // Fail to begin
	TEST_ASSERT_FALSE(rc.begin(nullptr));
}

void test_buffers()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);

	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	// Check the outoging buffer
	int len = REMOTECONTROLLER_OUTGOING_BUFFER_SIZE;
	uint8_t *pStart = rc.outgoingBuffer;
	while (len--)
	{
		*(pStart++) = (uint8_t)0x02;
	}
	len = REMOTECONTROLLER_OUTGOING_BUFFER_SIZE;
	pStart = rc.outgoingBuffer;
	while (len--)
	{
		TEST_ASSERT_EQUAL_UINT8(0x02, *(pStart++));
	}
	// incoming buffer
	len = REMOTECONTROLLER_INCOMING_BUFFER_SIZE;
	pStart = rc.incomingBuffer;
	while (len--)
	{
		*(pStart++) = (uint8_t)0x02;
	}
	len = REMOTECONTROLLER_INCOMING_BUFFER_SIZE;
	pStart = rc.incomingBuffer;
	while (len--)
	{
		TEST_ASSERT_EQUAL_UINT8(0x02, *(pStart++));
	}
	rc.end();
}

void test_end()
{
	Mock<Connection> mockConnection;
	When(Method(mockConnection, begin)).Return(true); // Succesfully begin
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	Fake(Method(mockConnection, end));
	rc.end();

	TEST_ASSERT_TRUE(Verify(Method(mockConnection, end)).Once());
}