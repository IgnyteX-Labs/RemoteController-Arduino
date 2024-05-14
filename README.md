# RemoteController

Library for the Arduino Framework to implement any type of remote control to Arduino, ESP32, ESP8266. (Connection between two Arduino boards!)

## Standard Connection Types

The library supports the following connection protocols for the Remote Controller out of the box.

- __RF24__ (Using the [RF24 Library](https://github.com/nRF24/RF24))
- *__Bluetooth__ (Implementation planned)*
- *__Wifi__ (Implementation planned)*

Custom or more sophisticated connection protocols can be added by creating a class conforming to the `Connection` class. For the implementation requirements please refer to the docs.

## Supported Platforms and Boards

The library currently supports the following platforms/boards, with the Arduino Framework:

- __espressif32__ (The following boards where tested)
  - esp32dev

More will be added in the future!

## Quick start guide

Currently the library can only be downloaded by cloning the Github Repository into the library folder. In the future the library will be added to the PlatformIO and Arduino Library Managers.

__1.__ At the top of your Project include the RemoteController header and create a RemoteController object. The Connection type has to be specified by passing an object conforming to `Connection`.

```[c++]
#include <RemoteController.h>

RF24Connection connection; /* Using the RF24 Connection but any other can also be used. */
RemoteController rc(connection);
```

The RemoteController library uses so called `Commands` to process instructions send form one remote to the other. Implement an enum holding all possible instructions. Additionally every instruction can be sent together with a throttle value (0-255).

```[c++]
enum Commands:uint8_t {
    GoForward, GoBackward, GoLeft, GoRight, ToggleLED // Add any commands you wish
};
```

__2.__ Create the callback functions, that will be called when a `Command` or a custom payload is received.

```[c++]
void commandReceivedCallback(const std::vector<uint8_t> &commands, const std::vector<uint8_t> &throttle) {
    // Do something with the received commands
    for(auto command:commands) {
        if(command == GoForward) {
            // Go Forward
        }else if(command == GoBackward) {
            // Go backward
        }
    }
}

/* This second callback function is optional to receive binary payloads send via RemoteController::sendPayload(). */

void payloadReceivedCallback(const void *buffer, size_t length) {
    // Process binary data
}
```

__3.__ In the `void setup()` function initialize the RemoteController. Pass one or two callback functions to receive commands and custom payloads. This will connect the RemoteController.

```[c++]
if(!rc.begin(commandReceivedCallback, payloadReceivedCallback)) {
    // Failed to begin
    Serial.println(rc.getErrorDescription());
}
```

__4.__ In the `void loop()` function repeatedly call the RemoteController::run() function to handle queued commands and incoming data.

```[c++]
if(!rc.run()) {
    // An error occured
    Serial.prinln(rc.getErrorDescription());
}
```

__5.__ Now to send commands use the RemoteController::sendCommand() functions anywhere in your code:

```[c++]
rc.sendCommand(GoForward); // Full speed
rc.sendCommand(GoForward, 60); // only 60/255 speed

/* All the above commands are sent with Priority::Normal meaning they are sent as a batch with the rc.run() call

For high priority commands that should be sent immediately specifiy Priority::High like so: */

rc.sendCommand(GoForward, 60, RemoteController::High);
```
