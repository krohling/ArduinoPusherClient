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


#ifndef PUSHERCLIENT_H
#define PUSHERCLIENT_H_

#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "HashMap/HashMap.h"
#include "WebSocketClient.h"
#include "Logger.h"

//Define ENABLE_AUTH as 1 to subscribe to private channels and send client events.
//Client events must be enabled from the pusher web configuration page.
#define ENABLE_AUTH 1

typedef void (*EventDelegate)(const String& name, const String& data);

class PusherClient 
{
    public:
        PusherClient();
        
		bool connect();
        void monitor();
        
		bool connected();
        void disconnect();
        
		void bind(const String& eventName, EventDelegate delegate);
        void bindAll(EventDelegate delegate);
        
		void subscribe(const String& channel);
        void unsubscribe(const String& channel);
		
		void triggerEvent(const String& eventName, const String& eventData);

#if (ENABLE_AUTH == 1)
		void acquireSocketId(const String& eventData);
		void subscribePrivate(const String& channel);
        void triggerPrivateEvent(const String& channelName, const String& eventName, const String& eventData);
#endif
  
		void KeepConnectionAlive();
		
	private:		
        static void getStringTableItem(int index, String& text);
        static void getPusherInfoItem(int index, String& text);
		static void dataArrived(const String& data);
        static void parseMessageMember(const String& message, const String& name, String& value);

	private:	
        WebSocketClient _client;
		unsigned long lastPingTime;
		
#if (ENABLE_AUTH == 1)
		void getAuthString(const String& channel, String& auth);
		void computeSha256(const String& text, String& sha);
		String _socketid;
#endif

};

extern PusherClient Pusher;

#endif
