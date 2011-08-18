#ifndef PUSHERCLIENT_H
#define PUSHERCLIENT_H_

#include <string.h>
#include <stdlib.h>
#include <WString.h>
#include <Client.h>
#include "WProgram.h"
#include <WebSocketClient.h>

//Uncomment this to use WIFLY Client
//#define WIFLY true

class PusherClient {
    
	public:
        PusherClient(String appId);
        typedef void (*EventDelegate)(String data);
		bool connect();
        bool connected();
        void disconnect();
		void monitor();
        void bindAll(EventDelegate delegate);
        void bind(String eventName, EventDelegate delegate);
        void subscribe(String channel);
        void subscribe(String channel, String auth);
        void subscribe(String channel, String auth, int userId);
        void triggerEvent(String eventName, String eventData);
        void unsubscribe(String channel);
        static String parseMessageMember(String memberName, String data);
	private:
        String _appId;
		WebSocketClient _client;
        static void dataArrived(WebSocketClient client, String data);
};


#endif
