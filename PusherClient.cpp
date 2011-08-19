/*
 PusherClient, a Pusher (http://pusherapp.com) client for Arduino
 Copyright 2011 Kevin Rohling
 http://kevinrohling.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include <PusherClient.h>
#include <HashMap.h>
#include <WString.h>
#include <string.h>
#include <stdlib.h>
#include "WProgram.h"

int _currentMapIndex = 0;
const byte HASH_SIZE = 10;
typedef void (*EventDelegate)(String data);
static EventDelegate _bindAllDelegate;
static HashType<String,EventDelegate> _bindArray[HASH_SIZE];
static HashMap<String,EventDelegate> _bindMap = HashMap<String,EventDelegate>( _bindArray, HASH_SIZE );

#ifdef WIFLY
PusherClient::PusherClient(String appId) :
	_client("ws.pusherapp.com", "/app/" + appId + "?client=js&version=1.9.0", 80)
{ 
    _client.setDataArrivedDelegate(dataArrived);
}
#else
byte server[] = { 174, 129, 22, 121 }; //ws.pusherapp.com
PusherClient::PusherClient(String appId) :
_client(server, "/app/" + appId + "?client=js&version=1.9.0", 80)
{ 
    _client.setDataArrivedDelegate(dataArrived);
}
#endif

bool PusherClient::connect() {
    return _client.connect();
}

bool PusherClient::connected() {
    return _client.connected();
}

void PusherClient::disconnect() {
    _client.disconnect();
}

void PusherClient::monitor () {
    _client.monitor();
}

void PusherClient::bindAll(EventDelegate delegate) {
    _bindAllDelegate = delegate;
}

void PusherClient::bind(String eventName, EventDelegate delegate) {
    _bindMap[_currentMapIndex](eventName, delegate);
    _currentMapIndex++;
}

void PusherClient::subscribe(String channel) {
    triggerEvent("pusher:subscribe", "{\"channel\": \"" + channel + "\" }");
}

void PusherClient::subscribe(String channel, String auth) {
    triggerEvent("pusher:subscribe", "{\"channel\": \"" + channel + "\", \"auth\": \"" + auth + "\" }");
}

void PusherClient::subscribe(String channel, String auth, int userId) {
    triggerEvent("pusher:subscribe", "{\"channel\": \"" + channel + "\", \"auth\": \"" + auth + "\", \"channel_data\": { \"user_id\": " + userId + " } }");
}

void PusherClient::unsubscribe(String channel) {
    triggerEvent("pusher:unsubscribe", "{\"channel\": \"" + channel + "\" }");
}

void PusherClient::triggerEvent(String eventName, String eventData) {
    Serial.println("Triggering Event");
    Serial.println("{\"event\": \"" + eventName + "\", \"data\": " + eventData + " }");
    _client.send("{\"event\": \"" + eventName + "\", \"data\": " + eventData + " }");
}


void PusherClient::dataArrived(WebSocketClient client, String data) {
    String eventNameStart = "event";
    String eventName = parseMessageMember("event", data);
    Serial.println(eventName);
    
    if (_bindAllDelegate != NULL) {
        _bindAllDelegate(data);
    }
    
    EventDelegate delegate = _bindMap.getValueOf(eventName);
    if (delegate != NULL) {
        delegate(data);
    }
    
    /*
    if (eventName == "pusher:connection_established") {
        String eventData = parseMessageMember("data", data);
        String socketId = parseMessageMember("socket_id", eventData);
        
    }
    else if(eventName == "pusher:connection_failed") {
        
    }
    else if(eventName == "pusher:error") {
        String message = parseMessageMember("message", data);
        String code = parseMessageMember("code", data);
        
    }
    else if(eventName == "pusher:heartbeat") {
        String time = parseMessageMember("time", data);
        
    }
    else {
        String eventData = parseMessageMember("data", data);
        String channel = parseMessageMember("channel", data);
        
    }
    */
}

String PusherClient::parseMessageMember(String memberName, String data) {
    memberName = "\"" + memberName + "\"";
    int memberDataStart = data.indexOf(memberName) + memberName.length();
    
    char currentCharacter;
    do {
        memberDataStart++;
        currentCharacter = data.charAt(memberDataStart);
    } while (currentCharacter == ' ' || currentCharacter == ':' || currentCharacter == '\"');
    
    int memberDataEnd = memberDataStart;
    bool isString = data.charAt(memberDataStart-1) == '\"';
    if (!isString) {
        do {
            memberDataEnd++;
            currentCharacter = data.charAt(memberDataEnd);
        } while (currentCharacter != ' ' && currentCharacter != ',');
    }
    else {
        char previousCharacter;
        currentCharacter = ' ';
        do {
            memberDataEnd++;
            previousCharacter = currentCharacter;
            currentCharacter = data.charAt(memberDataEnd);
        } while (currentCharacter != '"' || previousCharacter == '\\');
    }
    
    return data.substring(memberDataStart, memberDataEnd).replace("\\\"", "\"");
}