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
#include <HashMap/HashMap.h>
#include <WString.h>
#include <string.h>
#include <stdlib.h>

const byte HASH_SIZE = 10;
typedef void (*EventDelegate)(String data);
static EventDelegate _bindAllDelegate;
static HashMap<String, EventDelegate, HASH_SIZE> _bindMap = HashMap<String, EventDelegate, HASH_SIZE>();

prog_char stringVar0[] PROGMEM = "{0}";
prog_char stringVar1[] PROGMEM = "{1}";
prog_char stringVar2[] PROGMEM = "{2}";
prog_char pusherPath[] PROGMEM = "/app/{0}?client=js&version=2.1&protocol=7";
prog_char pusherHostname[] PROGMEM = "ws.pusherapp.com";
prog_char subscribeEventName[] PROGMEM = "pusher:subscribe";
prog_char subscribeMessage1[] PROGMEM = "{\"channel\": \"{0}\" }";
prog_char subscribeMessage2[] PROGMEM = "{\"channel\": \"{0}\", \"auth\": \"{1}\" }";
prog_char subscribeMessage3[] PROGMEM = "{\"channel\": \"{0}\", \"auth\": \"{1}\", \"channel_data\": { \"user_id\": {2} } }";
prog_char unsubscribeMessage[] PROGMEM = "{\"channel\": \"{0}\" }";
prog_char triggerEventMessage[] PROGMEM = "{\"event\": \"{0}\", \"data\": {1} }";
prog_char eventNameStart[] PROGMEM = "event";
prog_char unsubscribeEventName[] PROGMEM = "pusher:unsubscribe";


PROGMEM const char *stringTable[] =
{   
    stringVar0,
    stringVar1,
    stringVar2,
    pusherPath,
    pusherHostname,
    subscribeEventName,
    subscribeMessage1,
    subscribeMessage2,
    subscribeMessage3,
    unsubscribeMessage,
    triggerEventMessage,
    eventNameStart,
    unsubscribeEventName
};

String PusherClient::getStringTableItem(int index) {
    char buffer[85];
    strcpy_P(buffer, (char*)pgm_read_word(&(stringTable[index])));
    return String(buffer);
}

PusherClient::PusherClient()
{ 
    _client.setDataArrivedDelegate(dataArrived);
}

bool PusherClient::connect(String appId) {
    String stringVar0 = getStringTableItem(0);
    String path = getStringTableItem(3);
    path.replace(stringVar0, appId);

    char pathData[path.length() + 1];
    path.toCharArray(pathData, path.length() + 1);
    
    return _client.connect("ws.pusherapp.com", pathData, 80);
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
    _bindMap[eventName] = delegate;
}

void PusherClient::subscribe(String channel) {
    String subscribeEventName = getStringTableItem(5);
    String stringVar0 = getStringTableItem(0);
    String message = getStringTableItem(6);
    message.replace(stringVar0, channel);
    triggerEvent(subscribeEventName, message);
}

void PusherClient::subscribe(String channel, String auth) {
    String subscribeEventName = getStringTableItem(5);
    String stringVar0 = getStringTableItem(0);
    String stringVar1 = getStringTableItem(1);
    String message = getStringTableItem(7);
    message.replace(stringVar0, channel);
    message.replace(stringVar1, auth);
    triggerEvent(subscribeEventName, message);
}

void PusherClient::subscribe(String channel, String auth, String userId) {
    String subscribeEventName = getStringTableItem(5);
    String stringVar0 = getStringTableItem(0);
    String stringVar1 = getStringTableItem(1);
    String stringVar2 = getStringTableItem(2);
    String message = getStringTableItem(8);
    message.replace(stringVar0, channel);
    message.replace(stringVar1, auth);
    message.replace(stringVar2, userId);
    triggerEvent(subscribeEventName, message);
}

void PusherClient::unsubscribe(String channel) {
    String unsubscribeEventName = getStringTableItem(12);
    String stringVar0 = getStringTableItem(0);
    String message = getStringTableItem(9);
    message.replace(stringVar0, channel);
    triggerEvent(unsubscribeEventName, message);
}

void PusherClient::triggerEvent(String eventName, String eventData) {
    String stringVar0 = getStringTableItem(0);
    String stringVar1 = getStringTableItem(1);
    String message = getStringTableItem(10);
    
    message.replace(stringVar0, eventName);
    message.replace(stringVar1, eventData);
    
    _client.send(message);
}


void PusherClient::dataArrived(WebSocketClient client, String data) {
    String eventNameStart = getStringTableItem(11);
    String eventName = parseMessageMember(eventNameStart, data);
    
    if (_bindAllDelegate != NULL) {
        _bindAllDelegate(data);
    }
    
    EventDelegate delegate = _bindMap[eventName];
    if (delegate != NULL) {
        delegate(data);
    }
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
    
    String result = data.substring(memberDataStart, memberDataEnd);
    result.replace("\\\"", "\"");
    return result;
}
