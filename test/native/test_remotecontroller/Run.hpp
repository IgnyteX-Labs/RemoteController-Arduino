#pragma once
#include <ArduinoFake.h>
#include <unity.h>
#include <stddef.h>
#include <stdint.h>

#include "RemoteController.h"

using namespace fakeit;

// RemoteController::run()

void test_run()
{
	Mock<Connection> mockConnection;
	Fake(Method(mockConnection, end));
	When(Method(mockConnection, begin)).Return(true);
	When(Method(mockConnection, getMaxPackageSize)).AlwaysReturn(32);
	When(Method(mockConnection, write)).Return(false);
	When(Method(mockConnection, available)).Return(false);
	RemoteController rc(mockConnection.get());
	rc.begin(nullptr);
	rc.sendCommand(0x00);
	TEST_ASSERT_FALSE(rc.run());
	TEST_ASSERT_EQUAL_UINT8(RemoteController::Error::FailedToTransmitCommands, rc.getErrorCode());
	rc.end();
}