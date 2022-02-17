/*
 WebsocketClient, a websocket client for Arduino
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

#include "WebSocketClient.h"

prog_char stringVar[] PROGMEM = "$";
prog_char clientHandshakeLine1[] PROGMEM = "GET $ HTTP/1.1";
prog_char clientHandshakeLine2[] PROGMEM = "Upgrade: WebSocket";
prog_char clientHandshakeLine3[] PROGMEM = "Connection: Upgrade";
prog_char clientHandshakeLine4[] PROGMEM = "Host: $";
prog_char clientHandshakeLine5[] PROGMEM = "Origin: ArduinoWebSocketClient";
prog_char serverHandshake[] PROGMEM = "HTTP/1.1 101";

prog_char logConnectingWS[] PROGMEM = 			"Connecting to host.";
prog_char logConnectionFailedWS[] PROGMEM = 	"Connection to host failed.";
prog_char logHandShakingWS[] PROGMEM = 			"Handshaking.";
prog_char logHandShakingFailedWS[] PROGMEM = 	"Handshaking failed.";
prog_char logSendingData[] PROGMEM = 			"Sending data: ";
prog_char logReceivedData[] PROGMEM = 			"Received data: ";

PROGMEM const char* stringTableWebSocket[] =
{   
    stringVar, 				//0
    clientHandshakeLine1,	//1
    clientHandshakeLine2,	//2
    clientHandshakeLine3,	//3
    clientHandshakeLine4,	//4
    clientHandshakeLine5,	//5
    serverHandshake			//6
};

PROGMEM const char* logMessageTableWebSocket[] =
{   
    logConnectingWS, 			//0
    logConnectionFailedWS, 		//1
	logHandShakingWS,			//2
	logHandShakingFailedWS,		//3
	logSendingData,				//4
	logReceivedData,			//5
};

void WebSocketClient::getStringTableItem(int index, String& text)
{
 	char* ptr = (char*)pgm_read_word(&(stringTableWebSocket[index]));
	int len = strlen_P(ptr);
	char buffer[len+1];
	strcpy_P(buffer, ptr);
	text = buffer;
}

bool WebSocketClient::connect(const String& hostname, const String& path, int port)
{
	LogPrintLn(logMessageTableWebSocket, 0);
	char hostNameArray[32];
	hostname.toCharArray(hostNameArray, 32);
	if (!_client.connect(hostNameArray, port))
	{
		LogPrintLn(logMessageTableWebSocket, 1);
		return false;
	}	
	
	LogPrintLn(logMessageTableWebSocket, 2);
	if (!Handshake(hostname, path))
	{
		LogPrintLn(logMessageTableWebSocket, 3);
		return false;
	}
	
	return true;
}

bool WebSocketClient::connected() 
{
    return _client.connected();
}

void WebSocketClient::disconnect() 
{
    _client.stop();
}

void WebSocketClient::setDataArrivedDelegate(DataArrivedDelegate dataArrivedDelegate)
{
	_dataArrivedDelegate = dataArrivedDelegate;
}

bool WebSocketClient::Handshake(const String& hostname, const String& path)
{
    if (!SendHandshake(hostname, path))
		return false;
    
	if (!WaitHandshake(hostname, path))
		return false;

	return true;
}

bool WebSocketClient::WaitHandshake(const String& hostname, const String& path)
{
	bool foundHandShake = false;
    String line;  
    String param;  

	//LogPrintLn("Wait to receive handshake");
	
	int maxAttempts = 50, attempts;
    for(attempts = 0; attempts < maxAttempts; attempts++)
	{ 
		if (_client.available())
			break;
		
		delay(100);
    }
	if (attempts == maxAttempts)
	{
		//LogPrintLn("Max attempts reached");
		return false;
	}
		
	getStringTableItem(6, param);
	while(readLine(line), line.length()!=0) 
	{
		if (line.indexOf( param ) != -1)
			foundHandShake = true;

    }
    if (!foundHandShake)
		return false;

	//LogPrintLn("Handshaking completed");
	return true;
}

bool WebSocketClient::SendHandshake(const String& hostname, const String& path)
{
    String line;  
    String param;  
    
	//LogPrintLn("Sending first handshake");
	
	getStringTableItem(0, param);

	//line 1
	getStringTableItem(1, line);
    line.replace(param, path);
    _client.println(line);
	
	//line 2
	getStringTableItem(2, line);
    _client.println(line);
	
	//line 3
	getStringTableItem(3, line);
    _client.println(line);
	
	//line 4
	getStringTableItem(4, line);
    line.replace(param, hostname);
    _client.println(line);
	
	//line 5
	getStringTableItem(5, line);
    _client.println(line);
	
    //end of line
	_client.println();
	_client.flush();
		
	return true;
}

void WebSocketClient::readLine(String& line)
{
    char character;
    
	line = "";
    
    while(_client.available() > 0 && (character = _client.read()) != '\n')
	{
        if (character != '\r' && character != -1)
		{
            line += character;
        }
    }
}

void WebSocketClient::send(const String& data)
{
    _client.print((char)0);
	_client.print(data);
    _client.print((char)255);
	_client.flush();
	
	LogPrint(logMessageTableWebSocket, 4);
	LogPrintLn(data);
}

void WebSocketClient::send(const char data[])
{
    _client.print((char)0);
	_client.print(data);
    _client.print((char)255);
	_client.flush();
	
	LogPrint(logMessageTableWebSocket, 4);
	LogPrintLn(data);
}

void WebSocketClient::monitor()
{
	if (_client.available() > 0)
	{
		if (_client.read() == 0)
		{
			String message;
			char character;			
			
			message.reserve(128);
			
			while(character = _client.read(), character != -1)
			{
				message += character;
			}
			
			LogPrint(logMessageTableWebSocket, 5);
			LogPrintLn(message);
			
			if (_dataArrivedDelegate != NULL)
			{
				_dataArrivedDelegate(message);
			}
		}
	}
}

