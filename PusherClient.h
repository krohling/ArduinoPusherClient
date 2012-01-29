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
#include <WString.h>
#include "Arduino.h"
#include <WebSocketClient.h>

//Uncomment this to use WIFLY Client
//#define WIFLY true

class PusherClient {

    public:
        PusherClient();
        typedef void (*EventDelegate)(String data);
        bool connect(String appId);
        bool connected();
        void disconnect();
        void monitor();
        void bindAll(EventDelegate delegate);
        void bind(String eventName, EventDelegate delegate);
        void subscribe(String channel);
        void subscribe(String channel, String auth);
        void subscribe(String channel, String auth, String userId);
        void triggerEvent(String eventName, String eventData);
        void unsubscribe(String channel);
    private:
        String _appId;
        WebSocketClient _client;
        static String getStringTableItem(int index);
        static void dataArrived(WebSocketClient client, String data);
        static String parseMessageMember(String memberName, String data);
};


#endif
