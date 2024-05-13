#pragma once

#include <ArduinoFake.h>
#include <unity.h>
#include <stddef.h>
#include <stdint.h>

#include "RemoteController.h"

using namespace fakeit;

// RemoteController::sendPayload()

void test_sendPayload()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).Return(4).Return(2);
	When(Method(mockConnection, write))
		.Do([](const void *buffer, size_t length) -> bool
			{ 
			uint8_t *pStart = reinterpret_cast<unsigned char *>(const_cast<void *>(buffer));
			TEST_ASSERT_EQUAL_UINT8(0x20, *(pStart));
			TEST_ASSERT_EQUAL_UINT8(0xFF, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(0xFF, *(++pStart));
			TEST_ASSERT_EQUAL_UINT8(0x68, *(++pStart));
			return true; });
	When(Method(mockConnection, available)).Return(false);
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	uint8_t buf[4] = {0x20, 0xFF, 0xFF, 0x68};
	TEST_ASSERT_TRUE(rc.sendPayload(buf, 4));
	TEST_ASSERT_FALSE(rc.sendPayload(buf, 4));
	TEST_ASSERT_TRUE(Verify(Method(mockConnection, write)).Once()); // Write should only be called once as the second time should fail
	rc.end();
}