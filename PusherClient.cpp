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

#include "PusherClient.h"

#if (ENABLE_AUTH == 1)
#include "sha256.h"
const byte HASH_SIZE = 10;
static EventDelegate _bindAllDelegate;
static HashMap<String, EventDelegate, HASH_SIZE> _bindMap = HashMap<String, EventDelegate, HASH_SIZE>();
void connectionEstablished(const String& eventName, const String& eventData);
#endif

prog_char pusherAppId[] PROGMEM = ""; //INSERT YOU API ID HERE
prog_char pusherKey[] PROGMEM = ""; //INSERT YOU API KEY HERE
prog_char pusherSecret[] PROGMEM = ""; //INSERT YOU API SECRET HERE
PROGMEM const char* pusherInfos[] =
{   
    pusherAppId,      //0
    pusherKey,	      //1
    pusherSecret,     //2            
};

prog_char stringVar0[] PROGMEM = "#1";
prog_char stringVar1[] PROGMEM = "#2";
prog_char stringVar2[] PROGMEM = "#3";
prog_char pusherPath[] PROGMEM = "/app/#1?client=js&version=1.9.0";
prog_char pusherHostname[] PROGMEM = "ws.pusherapp.com";
prog_char subscribeEventName[] PROGMEM = "pusher:subscribe";
prog_char subscribeMessage1[] PROGMEM = "{\"channel\":\"#1\"}";
prog_char subscribeMessage2[] PROGMEM = "{\"channel\":\"#1\",\"auth\":\"#2\"}";
prog_char subscribeMessage3[] PROGMEM = "{\"channel\":\"#1\",\"auth\":\"#2\",\"channel_data\":{\"user_id\":#3}}";
prog_char unsubscribeMessage[] PROGMEM = "{\"channel\":\"#1\"}";
prog_char triggerEventMessage[] PROGMEM = "{\"event\":\"#1\",\"data\":#2}";
prog_char triggerEventChannelMessage[] PROGMEM = "{\"event\":\"#1\",\"channel\":\"#3\",\"data\":#2}";
prog_char eventNameStart[] PROGMEM = "\"event\"";
prog_char dataNameStart[] PROGMEM = "\"data\"";
prog_char unsubscribeEventName[] PROGMEM = "pusher:unsubscribe";
prog_char pingEvent[] PROGMEM = "{\"event\":\"pusher:ping\",\"data\":\"\"}";
prog_char emptyEventData[] PROGMEM = "\"\"";
prog_char connectionEstablishedEventName[] PROGMEM = "pusher:connection_established";
prog_char socketIdName[] PROGMEM = "\"socket_id\"";
prog_char hexCharacters[] PROGMEM = "0123456789abcdef";

#define STRING_BUFFER_SIZE 				85
#define DEFINE_STRING_BUFFER(name) 		char name[STRING_BUFFER_SIZE];

prog_char logConnectionFailedPC[] PROGMEM = 	"Connection to Pusher failed.";
prog_char logConnectionSuccessPC[] PROGMEM = 	"Connection to Pusher completed.";
prog_char logBindEventPC[] PROGMEM = 			"Bound to event ";
prog_char logSubscribeChannelPC[] PROGMEM = 	"Subscribed to channel ";
prog_char logAnswerToPing[] PROGMEM = 			"Sending pong event";
prog_char logTriggeringEvent[] PROGMEM = 		"Triggering event ";

PROGMEM const char* stringTablePusherClient[] =
{   
    stringVar0, 				//0
    stringVar1,					//1
    stringVar2,					//2
    pusherPath,					//3
    pusherHostname,				//4
    subscribeEventName,			//5
    subscribeMessage1,			//6
    subscribeMessage2,			//7
    subscribeMessage3,			//8
    unsubscribeMessage,			//9
    triggerEventMessage,		//10
    eventNameStart,				//11
    unsubscribeEventName,		//12
    dataNameStart,				//13
    triggerEventChannelMessage,	//14
    pingEvent,				//15
    emptyEventData,				//16
	connectionEstablishedEventName,	//17
	socketIdName,				//18
	hexCharacters,				//19
};

PROGMEM const char* logMessageTablePusherClient[] =
{   
    logConnectionFailedPC, 		//0
    logConnectionSuccessPC,		//1
	logBindEventPC, 			//2
    logSubscribeChannelPC, 		//3
	logAnswerToPing,			//4
	logTriggeringEvent,			//5
};

void PusherClient::getStringTableItem(int index, String& text)
{
	char* ptr = (char*)pgm_read_word(&(stringTablePusherClient[index]));
	int len = strlen_P(ptr);
	char buffer[len+1];
	strcpy_P(buffer, ptr);
	text = buffer;
}

void PusherClient::getPusherInfoItem(int index, String& text)
{
	char* ptr = (char*)pgm_read_word(&(pusherInfos[index]));
	int len = strlen_P(ptr);
	char buffer[len+1];
	strcpy_P(buffer, ptr);
	text = buffer;
}

PusherClient::PusherClient()
{ 
    String eventName; 
	getStringTableItem(17, eventName);
	bind(eventName, connectionEstablished);
	
	_client.setDataArrivedDelegate(dataArrived);
	lastPingTime = millis();
}

bool PusherClient::connect()
{   
	String stringVar0; 
	String path; 
	String host; 
	String key;
	
	getStringTableItem(0, stringVar0);
    getStringTableItem(3, path);	
    getStringTableItem(4, host);	
    getPusherInfoItem(1, key);	
    path.replace(stringVar0, key);

    if (!_client.connect(host, path, 80))
	{
		LogPrintLn(logMessageTablePusherClient, 0);
		return false;
	}	
	
#if (ENABLE_AUTH == 1)
	while(_socketid.length() == 0)
	{
		delay(100);
		monitor();
	}
#endif
	
	LogPrintLn(logMessageTablePusherClient, 1);
	return true;
}

bool PusherClient::connected() 
{
    return _client.connected();
}

void PusherClient::disconnect() 
{
    _client.disconnect();
}

void PusherClient::monitor()
{
    _client.monitor();
	
	if (_client.connected())
	{
		const unsigned long timeout = 60000;

		unsigned long time = millis();
		
		if (time > lastPingTime + timeout)
		{    
			KeepConnectionAlive();
			lastPingTime = time;
		}
	}
}

void PusherClient::bindAll(EventDelegate delegate) 
{
    _bindAllDelegate = delegate;
}

void PusherClient::bind(const String& eventName, EventDelegate delegate) 
{
    _bindMap[eventName] = delegate;
}

void PusherClient::subscribe(const String& channel) 
{
    if (!_client.connected())
		return;
		
    String subscribeEventName; 
    String stringVar0; 
    String message;
	
	getStringTableItem(5, subscribeEventName);
	getStringTableItem(0, stringVar0);	
	getStringTableItem(6, message);
	
    message.replace(stringVar0, channel);
    
	triggerEvent(subscribeEventName, message);

	LogPrint(logMessageTablePusherClient, 3);
	LogPrintLn(channel);
}

void PusherClient::unsubscribe(const String& channel) 
{
    String unsubscribeEventName; 
    String stringVar0; 
    String message; 
		
	getStringTableItem(12, unsubscribeEventName);
	getStringTableItem(0, stringVar0);
	getStringTableItem(9, message);
	
    message.replace(stringVar0, channel);
	
    triggerEvent(unsubscribeEventName, message);
}

void PusherClient::KeepConnectionAlive()
{
    String pingEvent; 
    getStringTableItem(15, pingEvent);	
	_client.send(pingEvent);
}

void PusherClient::dataArrived(const String& message)
{
	String paramName;
	String eventName;
	String eventData;
	
	getStringTableItem(11, paramName);
	parseMessageMember(message, paramName, eventName);
	
	getStringTableItem(13, paramName);
	parseMessageMember(message, paramName, eventData);
    
    if (_bindAllDelegate != NULL) {
        _bindAllDelegate(eventName, eventData);
    }
    
    EventDelegate delegate = _bindMap[eventName];
    if (delegate != NULL) {
        delegate(eventName, eventData);
    }
}

void PusherClient::parseMessageMember(const String& message, const String& name, String& value)
{
	//name must be in the "name" format
	
	int memberNameStart = message.indexOf(name);
    if (memberNameStart == -1)
	{
		value = "";
		return;
	}		
		
	int memberDataStart = memberNameStart + name.length();
    
    char currentCharacter;
    do {
        memberDataStart++;
        currentCharacter = message.charAt(memberDataStart);
    } while (currentCharacter == ' ' || currentCharacter == ':' || currentCharacter == '\"');
    	
	int memberDataEnd = memberDataStart;
    bool isString = message.charAt(memberDataStart-1) == '\"';
    if (!isString) {
        do {
            memberDataEnd++;
            currentCharacter = message.charAt(memberDataEnd);
        } while (currentCharacter != ' ' && currentCharacter != ',');
    }
    else {
        char previousCharacter;
        currentCharacter = ' ';
        do {
            memberDataEnd++;
            previousCharacter = currentCharacter;
            currentCharacter = message.charAt(memberDataEnd);
        } while (currentCharacter != '"' || previousCharacter == '\\');
    }
    	
	value = message.substring(memberDataStart, memberDataEnd);
    value.replace("\\\"", "\"");
}

#if (ENABLE_AUTH == 1)
void connectionEstablished(const String& eventName, const String& eventData)
{
	Pusher.acquireSocketId(eventData);
}

void PusherClient::acquireSocketId(const String& eventData)
{
    String socketIdName;	
	getStringTableItem(18, socketIdName);
	parseMessageMember(eventData, socketIdName, _socketid);
}

void PusherClient::triggerEvent(const String& eventName, const String& eventData)
{
    String stringVar; 
    String message; 
    
	getStringTableItem(10, message);
	
	getStringTableItem(0, stringVar);
	message.replace(stringVar, eventName);
    
	getStringTableItem(1, stringVar);
	message.replace(stringVar, eventData);
	
    _client.send(message);
	
	LogPrint(logMessageTablePusherClient, 5);
	LogPrintLn(eventName);
}

void PusherClient::getAuthString(const String& channel, String& auth)
{
	Sha256Class sha256;
	
	//Sha init with secret key
	{
		String secret;
		getPusherInfoItem(2, secret);
		sha256.initHmac((uint8_t*)&secret[0], secret.length());
	}
	
	//Set the data to encrypt
	{
		String text;
		text.reserve(_socketid.length() + 1 + channel.length());
		text = _socketid;
		text += ':';
		text += channel;
		sha256.print(text);
	}
	
	uint8_t* result = sha256.resultHmac();
	
	String hexChars;
	getStringTableItem(19, hexChars);
	
	auth.reserve(21 /*key*/ + 1 + 64 /*sha256*/);	
	getPusherInfoItem(1, auth); //key
	auth += ':';	
	for (int i=0; i<32; i++)
	{
		auth += hexChars.charAt(result[i]>>4);
		auth += hexChars.charAt(result[i]&0xf);
	}
}

void PusherClient::subscribePrivate(const String& channel) 
{
    String message;
    getStringTableItem(7, message);
	
	{
		String stringVar;
		
		{
			String auth;
			getStringTableItem(1, stringVar);
			getAuthString(channel, auth);	
			message.replace(stringVar, auth);
			LogPrintLn(message);
		}
		
		getStringTableItem(0, stringVar);
		message.replace(stringVar, channel);
		LogPrintLn(message);	
	}
	
	String subscribeEventName;
	getStringTableItem(5, subscribeEventName);
    triggerEvent(subscribeEventName, message);
}

void PusherClient::triggerPrivateEvent(const String& channelName, const String& eventName, const String& eventData)
{
    String stringVar0; 
    String stringVar1; 
	String stringVar2; 
    String message; 
    
	getStringTableItem(0, stringVar0);
	getStringTableItem(1, stringVar1);
	getStringTableItem(2, stringVar2);
	getStringTableItem(14, message);
	
    message.replace(stringVar0, eventName);
    message.replace(stringVar1, eventData);
    message.replace(stringVar2, channelName);
	
    _client.send(message);
	
	LogPrint(logMessageTablePusherClient, 5);
	LogPrintLn(eventName);
}

#endif

PusherClient Pusher;
