#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PusherClient.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 10 };
PusherClient client("your-api-key-here");
Servo leftServo; 
Servo rightServo; 

void setup() {
  pinMode(2,OUTPUT);
  leftServo.attach(2);
  
  pinMode(3, OUTPUT);
  rightServo.attach(3);

  leftServo.write(95);
  rightServo.write(95);
  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  
  if(client.connect()) {
    client.bind("forward", moveForward);
    client.bind("backward", moveBackward);
    client.bind("turn_left", turnLeft);
    client.bind("turn_right", turnRight);
    client.bind("stop", stopMoving);
    client.subscribe("robot_channel");
  }
  else {
    while(1) {}
  }
}

void loop() {
  if (client.connected()) {
    client.monitor();
  }
  else {
    leftServo.write(95);
    rightServo.write(95);
  }
}

void moveForward(String data) {
  leftServo.write(0);
  rightServo.write(180);
}

void moveBackward(String data) {
  leftServo.write(180);
  rightServo.write(0);
}

void turnLeft(String data) {
  leftServo.write(0);
  rightServo.write(0);
}

void turnRight(String data) {
  leftServo.write(180);
  rightServo.write(180);
}

void stopMoving(String data) {
  leftServo.write(95);
  rightServo.write(95);
}