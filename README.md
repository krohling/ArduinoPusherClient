# Arduino Pusher Client, a Pusher client developed for use on Arduino devices
Blog: [World Domination Using Arduinos And Websockets](http://kevinrohling.wordpress.com/2011/09/14/world-domination-using-arduinos-and-websockets)

[Pusher] (http://www.pusherapp.com) is a Push Notification service that uses Websockets for relaying messages back and forth between clients.  This allows real time messaging between a diverse range of applications running on Web browsers, mobile devices and now Arduinos.  It is my hope that allowing devices to easily send information about themselves as well as respond to messages received from applications and other devices will result in some interesting applications.

This fork adds support for private channel subscription (the auth string is computed internally), client event triggering, ping-pong events (to keep connection alive). 

Since Arduino has very little RAM, remember to reduce memory allocations and fragmentation as much as possible. Each variable of type String should be passed by reference if possible. So function "void function(const String& i_param)" is ok, while "void function(String i_param)" is not (since it will cause Arduino to allocate a new String).

## Installation instructions

Once you've cloned this repo locally, copy the ArduinoPusherClient directory into your Arduino Sketchbook directory under Libraries then restart the Arduino IDE so that it notices the new library.  Now, under File\Examples you should see ArduinoPusherClient.  To use the library in your app, select Sketch\Import Library\ArduinoPusherClient.

Remember to add your api id, key and secret inside pusherAppId, pusherKey and pusherSecret in PusherClient.cpp.

## Examples

Included with this library are 2 example:
1) RobotExample, that uses Pusher events to drive two Servos.  This example connects to a channel named "robot_channel" and binds to 5 events: forward, backward, turn_left, turn_right, and stop.  When the events are received the appropriate method gets called and adjusts the angle of the servo motors, driving the robot.
2) private_channels, that register the client to a private channel, triggering client events into it.

## How To Use This Library

### Connecting to Pusher

Include the main header file:
#include <PusherClient.h>

Initialize Serial (for logging) and the ethernet lib:
Serial.begin(9600);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
if (!Ethernet.begin(mac)) {
  //error
}


Initialize the pusher lib:
if(!Pusher.connect()) {
  //error
}

Add the monitor function inside the loop function.
void loop() {
  Pusher.monitor();   //Must have a call to monitor() inside loop()
}
```


### Channels

```
//Subscribing to a Public Channel
client.subscribe("my-channel");  

//Subscribing to a Private Channel
client.subscribePrivate("private-my-channel");  

//Unsubscribing to a Channel
client.unsubscribe("my-channel");  

```

### Triggering Events

```
client.triggerEvent("my-event", "some data about my-event");
client.triggerPrivateEvent("private-my-channel", "client-my-event", "some data about my-event");
```

### Binding to Events

```
client.bind("my-event", handleMyEvent);

void handleMyEvent(const String& eventName, const String& eventData) {
   //Do stuff here
}
```


### Binding to all Events

```
client.bindAll(handleAllEvents);

void handleAllEvents(const String& eventName, const String& eventData) {
   //Do stuff here
}
```

## Credits

Arduino Pusher Client uses the [HashMap](http://www.arduino.cc/playground/Code/HashMap) library developed by Alexander Brevig.
