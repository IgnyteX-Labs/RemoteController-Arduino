#include <ArduinoFake.h>
#include <unity.h>
#include <stddef.h>
#include <stdint.h>

// Class to test
#include "RemoteController.h"

// Tests for class member functions
#include "BeginAndEnd.hpp"
#include "ReceiveCommandAndPayload.hpp"
#include "Run.hpp"
#include "SendCommand.hpp"
#include "SendPayload.hpp"

void setUp(void)
{
	// set stuff up here
	ArduinoFakeReset();
}

void tearDown(void)
{
	// clean stuff up here
	ArduinoFakeReset();
}

int main(int argc, char **argv)
{
	UNITY_BEGIN();
	
	RUN_TEST(test_begin);
	RUN_TEST(test_end);
	RUN_TEST(test_buffers);
	RUN_TEST(test_sendCommand_HighPriority);
	RUN_TEST(test_sendCommand_NormalPriority);
	RUN_TEST(test_sendCommandWithThrottle_HighPriority);
	RUN_TEST(test_sendCommandWithThrottle_NormalPriority);
	RUN_TEST(test_sendCommandMultiple_NormalPriority);
	RUN_TEST(test_sendCommandMultipleWithThrottle_NormalPriority);
	RUN_TEST(test_sendCommandStreaming);
	RUN_TEST(test_sendCommandPackageSize);
	RUN_TEST(test_sendPayload);
	RUN_TEST(test_run);
	RUN_TEST(test_receiveCommands);
	RUN_TEST(test_receivePayload);

	UNITY_END();
}
