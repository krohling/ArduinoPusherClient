#include <SPI.h>
#include <Ethernet.h>

#include <PusherClient.h>

void setup() 
{
  Serial.begin(9600);

  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  if (Ethernet.begin(mac) == 0)
  {
    for(;;);
  }
  
  if (!Pusher.connect())
  {
    for(;;);
  } 
  
  Pusher.subscribePrivate("private-ChannelName");
  Pusher.triggerPrivateEvent("private-ChannelName", "client-eventName", "\"\"");
}

unsigned long lasttime;
bool toggle;

void loop() 
{   
  Pusher.monitor();  

  unsigned long time = millis();  
  if (time > lasttime + 1000)
  {
    signalActualState();
    lasttime = time;
  }
}

void signalActualState()
{
    if (toggle)
      Pusher.triggerPrivateEvent("private-ChannelName", "client-eventName", "\"1\"");
    else
      Pusher.triggerPrivateEvent("private-ChannelName", "client-eventName", "\"0\"");
}
