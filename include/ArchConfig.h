#ifndef REMOTE_CONTROLLER_ARCH_CONFIG_H_
#define REMOTE_CONTROLLER_ARCH_CONFIG_H_

// Project wide std includes
#include <stdint.h>
#include <stddef.h>

// String implementation
#if defined(ARDUINO_ARCH_NATIVE)
#include <string> // Use std::string on native environment as Arduino String is not available
#define _String std::string
#else
#include <WString.h> // Use Arduino String class on all other architectures
#define _String String
#endif

// Min & Max method implementation
#define rcmin(a, b)((a) < (b) ? (a) : (b))
#define rcmax(a, b) ((a) > (b) ? (a) : (b))

// Functional implementation
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_NATIVE) // ESP32 and native will use the same config (ARDUINO_ARCH_NATIVE is defined in the platformio.ini build flags!!)
// ESP32 & Native use std::functional for a more modern approach
#include <functional>
#define RC_ARCH_USE_FUNCTIONAL
#elif defined(ARDUINO_ARCH_AVR)
// Arduino AVR boards such as Uno, Nano, Mega, etc. will use function pointers and
#endif

#endif