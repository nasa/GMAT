//------------------------------------------------------------------------------
// SocketServer.cpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under purchase
// order NNG16LD52P
//
//
//  Created on: Mar 4, 2011
//      Author: Tuan Nguyen
//

#define DEBUG_SOCKET_SERVICE_REQUEST
#define DEBUG_SOCKET_SERVICE_POKE
#define DEBUG_SOCKET

#include "SocketServer.hpp"
#include "GmatInterface.hpp"
//#include "MessageInterface.hpp"


#ifndef MessageInterface_hpp
	#include <stdio.h>
	#include <ctype.h>
	#include <string>
#endif

using namespace std;

SocketServer::SocketServer()
{
	error = 0;
	m_numClients = 0;
	shutdownserver = false;
#ifdef LINUX_MAC
#else
	client_sock = 0;
#endif
}

SocketServer::~SocketServer()
{
}

#ifdef LINUX_MAC
bool SocketServer::RunRequest(int sock)
#else
bool SocketServer::RunRequest(SOCKET sock)
#endif
{
	char lenc, len1c;
	unsigned char len, len1;
	char *buf, *buf1, *msg;

	// 1. Read data
	struct timeval time;
	time.tv_sec = 0;
	time.tv_usec = 2000000;
#ifdef LINUX_MAC
	fd_set socks_set;
	FD_ZERO(&socks_set);
	FD_SET(sock, &socks_set);
	int count = select(sock+1, &socks_set, NULL, NULL, &time);
#else
	struct fd_set socks_set;
	socks_set.fd_count = 1;
	socks_set.fd_array[0] = sock;
	int count = select(0, &socks_set, NULL, NULL, &time);
#endif

	if (count == 0)
		return false;

	int numBytes = recv(sock, &lenc, 1, 0);
	if (numBytes == 0)
		return false;

	len = (unsigned char)lenc;
	buf = new char[len+1];
	while (recv(sock, buf, len, 0) == 0)
	{
#ifdef LINUX_MAC
		usleep(1000);
#else
		_sleep(1);
#endif
	}
	buf[len] = '\0';

	#ifdef DEBUG_SOCKET
		#ifdef MessageInterface_hpp
			MessageInterface::ShowMessage("Client %d: Read message:%s\n", sock, buf);
		#else
			printf("Client %d: Read message:%s\n", sock, buf);
		#endif
	#endif


	// 2. Echo back the data
	send(sock, buf, len, 0);
	#ifdef DEBUG_SOCKET
		#ifdef MessageInterface_hpp
			MessageInterface::ShowMessage("Client %d: Echo back:%s\n", sock, buf);
		#else
			printf("Client %d: Echo back:%s\n", sock, buf);
		#endif
	#endif


	// 3. if the received message contains 'Request' string then run
	//    OnRequest() function
	if (strncmp(buf, "Request,", strlen("Request,")) == 0)
	{
		msg = &buf[strlen("Request,")];

		// 3.1. Run OnRequest function:
		char* result = OnRequest(msg);

		// 3.2. Read 'Idle' state
		#ifdef LINUX_MAC
		while (select(0, &socks_set, NULL, NULL, &time) == 0)
		{
			usleep(1000);
		}
		#else
		while (select(sock+1, &socks_set, NULL, NULL, &time) == 0)
		{
			_sleep(1);
		}
		#endif
		recv(sock, &len1c, 1, 0);
		len1 = (unsigned char)len1c;
		buf1 = new char[len1+1];
		recv(sock, buf1, len1, 0);
		buf1[len1] = '\0';

		// 3.3. Send results to client
		send(sock, result, strlen(result), 0);
		#ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("Client %d: Send result:%s\n", sock, result);
			#else
				printf("Client %d: Send result:%s\n", sock, result);
			#endif
		#endif

		delete[] buf1;
   }
   else if (strncmp(buf, "script,", strlen("script,")) == 0)
   {
	    // 4.Run OnPoke function:
	    msg = &buf[strlen("script,")];
	    OnPoke(msg);
	    if (strcmp(msg, "Close;") == 0)
	    	return true;
   }

   delete[] buf;

   return false;
}


char* SocketServer::OnRequest(char* item)
{
	#ifdef DEBUG_SOCKET_SERVICE_REQUEST
		#ifdef MessageInterface_hpp
			MessageInterface::ShowMessage("GmatSocketService::OnRequest() %s\n", item);
		#else
			printf("GmatSocketService::OnRequest() %s\n", item);
		#endif
   #endif

   // Check for user interrupt first (loj: 2007.05.11 Added)
   GmatInterface::Instance()->CheckUserInterrupt();

   // How can I tell whether item is an object or a parameter?
   // For now GetGMATObject.m appends '.' for object name.

   char *data;
   if (item[strlen(item)-1] == '.')
   {
	    item[strlen(item)-1] = '\0';
	    data = GmatInterface::Instance()->GetGmatObject(std::string(item));
   }
   else if (strcmp(item, "RunState") == 0)
   {
	    data = GmatInterface::Instance()->GetRunState();

		#ifdef DEBUG_SOCKET_SERVICE_REQUEST
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("GmatSocketService::OnRequest() data=%s\n", data);
			#else
				printf("GmatSocketService::OnRequest() data=%s\n", data);
			#endif
		#endif
   }
   else if (strcmp(item, "CallbackStatus") == 0)
   {
	    data = GmatInterface::Instance()->GetCallbackStatus();

		#ifdef DEBUG_SOCKET_SERVICE_REQUEST
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("GmatSocketService::OnRequest() data=%s\n", data);
			#else
				printf("GmatSocketService::OnRequest() data=%s\n", data);
			#endif
		#endif
   }
   else if (strcmp(item, "CallbackResults") == 0)
   {
	    data = GmatInterface::Instance()->GetCallbackResults();

		#ifdef DEBUG_SOCKET_SERVICE_REQUEST
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("GmatSocketService::OnRequest() data=%s\n", data);
			#else
				printf("GmatSocketService::OnRequest() data=%s\n", data);
			#endif
		#endif
   }
   else
   {
	   data = GmatInterface::Instance()->GetParameter(std::string(item));
   }


   return data;
}

/*
//------------------------------------------------------------------------------
// bool OnExecute(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool OnExecute(const wxString& WXUNUSED(topic),
                               wxChar *data,
                               int WXUNUSED(size),
                               wxIPCFormat WXUNUSED(format))
{
   #ifdef DEBUG_SOCKET_SERVICE_EXECUTE
   MessageInterface::ShowMessage
      ("GmatConnection::OnExecute() command: %s\n", data);
   #endif

   return TRUE;
}
*/


bool SocketServer::OnPoke(char* data)
{
	#ifdef DEBUG_SOCKET_SERVICE_POKE
		#ifdef MessageInterface_hpp
			MessageInterface::ShowMessage("GmatSocketService::OnPoke() data = %s\n", data);
		#else
			printf("GmatSocketService::OnPoke() data = %s\n", data);
		#endif
	#endif

	//------------------------------
    // save data to string stream
    //------------------------------

    if (strcmp(data, "Open;") == 0)
    {
    	GmatInterface::Instance()->OpenScript();
    }
    else if (strcmp(data, "Clear;") == 0)
    {
    	GmatInterface::Instance()->ClearScript();
    }
    else if (strcmp(data, "Build;") == 0)
    {
    	GmatInterface::Instance()->BuildObject();
    }
    else if (strcmp(data, "Update;") == 0)
    {
    	GmatInterface::Instance()->UpdateObject();
    }
    else if (strcmp(data, "Build+Run;") == 0)
    {
    	GmatInterface::Instance()->BuildObject();
    	GmatInterface::Instance()->RunScript();
    }
    else if (strcmp(data, "Run;") == 0)
    {
    	GmatInterface::Instance()->RunScript();
    }
    else if (strcmp(data, "Callback;") == 0)
    {
    	GmatInterface::Instance()->ExecuteCallback();
    }
    else if (strncmp(data, "CallbackData", strlen("CallbackData")) == 0)
    {
    	std::string callbackData(&data[strlen("CallbackData")]);
		#ifdef DEBUG_SOCKET_SERVICE_POKE
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("GmatSocketService::callbackData = %s\n", callbackData);
			#else
				printf("GmatSocketService::callbackData = %s\n", callbackData.c_str());
			#endif
		#endif

		GmatInterface::Instance()->PutCallbackData(callbackData);
    }
    else
    {
	   char* s = new char[strlen(data)+1];
	   strcpy(s, data);
       GmatInterface::Instance()->PutScript(s);

       delete s;
    }

   return true;
}

/*
//------------------------------------------------------------------------------
// bool OnStartAdvise(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool OnStartAdvise(const wxString& WXUNUSED(topic),
                                   const wxString& item)
{
   #ifdef DEBUG_SOCKET_SERVICE_ADVISE
   MessageInterface::ShowMessage
      ("GmatSocketService::OnStartAdvise() %s\n", item.c_str());
   #endif

   //#ifdef DEBUG_SOCKET_SERVICE_ADVISE
   //char* data = GmatInterface::Instance()->GetRunState();
   //MessageInterface::ShowMessage
   //   ("GmatSocketService::OnStartAdvise() data=%s\n", data);
   //#endif

   return TRUE;
}


//------------------------------------------------------------------------------
// bool OnDisconnect()
//------------------------------------------------------------------------------
bool OnDisconnect()
{
   #ifdef DEBUG_SOCKET_SERVICE
   MessageInterface::ShowMessage
      ("GmatSocketService::OnDisconnect() entered, this=%p\n", this);
   #endif
   return true;
}
*/


#ifdef LINUX_MAC
void SocketServer::OnAccept(int sk)
#else
void SocketServer::OnAccept(SOCKET sk)
#endif
{
	++m_numClients;

	#ifdef DEBUG_SOCKET
		#ifdef MessageInterface_hpp
			MessageInterface::ShowMessage("number of clients = %d\n", m_numClients);
		#else
			printf("number of clients = %d\n", m_numClients);
		#endif
	#endif

	bool stop = false;
	while ((!stop)&&(!shutdownserver))
    {
    	// repeat service until the client tells "it closes the connection"
    	stop = RunRequest(sk);

		#ifdef LINUX_MAC
			usleep(1000);
		#else
			_sleep(1);
		#endif
	}

	#ifdef LINUX_MAC
		close(sk);		// close client socket before ending of service
	#else
		closesocket(sk);	// close client socket before ending of service
	#endif

    	--m_numClients;

	#ifdef DEBUG_SOCKET
		#ifdef MessageInterface_hpp
			MessageInterface::ShowMessage("number of clients = %d\n", m_numClients);
		#else
			printf("number of clients = %d\n", m_numClients);
		#endif
	#endif

	#ifdef LINUX_MAC
		pthread_exit(NULL);			// end of client service thread
	#else
		_endthread();				// end of client service thread
	#endif
}



void SocketServer::RunServer()
{
	 struct sockaddr_in serv_addr,cli_addr;
#ifdef LINUX_MAC
	 socklen_t clilen=sizeof(cli_addr);
#else
	 int clilen=sizeof(cli_addr);
#endif


	 // 1. Set number of clients = 0
	 m_numClients = 0;


	 // 2. Socket initialization
#ifdef LINUX_MAC
#else
	 WSADATA WsaDat;
     if (WSAStartup(MAKEWORD(1,1),&WsaDat) != 0)
	 {
		#ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("WSA Initialization failed! STOP!!!\n");
			#else
				printf("WSA Initialization failed! STOP!!!\n");
			#endif
		#endif

		error = 1;
		return;
	 }
     else
     {
		#ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("WSA Initialization is successful...\n");
			#else
				printf("WSA Initialization is successful...\n");
			#endif
		#endif
     }
#endif


     // 3. Socket creation
     Server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     if (Server == INVALID_SOCKET)
     {
		#ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("Socket creation failed!STOP!!!\n");
			#else
				printf("Socket creation failed!STOP!!!\n");
			#endif
		#endif
		error = 2;
		return;
     }
     else
     {
		#ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("Socket is created successfully...\n");
			#else
				printf("Socket is created successfully...\n");
			#endif
		#endif
     }



     // 4. Set server IP address:
     serv_addr.sin_family=AF_INET;
     serv_addr.sin_port = htons(TCP_PORT);

     struct hostent* addr_info;
     char* computername = new char[strlen(IP_ADDRESS)+1];
     strcpy(computername, IP_ADDRESS);

     if (isalpha(computername[0]))
     {
    	 addr_info = gethostbyname(IP_ADDRESS);
#ifdef LINUX_MAC
	 serv_addr.sin_addr.s_addr = *(u_long *)addr_info->h_addr_list[0];
#else
    	 serv_addr.sin_addr.S_un.S_addr = *(u_long *)addr_info->h_addr_list[0];
#endif
     }
     else
     {
#ifdef LINUX_MAC
	 inet_aton(computername, &serv_addr.sin_addr);
#else
    	 serv_addr.sin_addr.S_un.S_addr = inet_addr(computername);
#endif
     }
     printf("Port = %x  IP address = %x\n", serv_addr.sin_port, serv_addr.sin_addr.s_addr);

     // 5. Bind address to the socket
     if (bind(Server,(struct sockaddr *)(&serv_addr),sizeof(serv_addr)) == SOCKET_ERROR)
     {
		 #ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("Attempt to bind failed!STOP!!!\n");
			#else
				printf("Attempt to bind failed!STOP!!!\n");
			#endif
		 #endif
		 error = 3;
		 return;
     }
     else
     {
		 #ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("Bind process is created successfully...\n");
			#else
				printf("Bind process is created successfully...\n");
			#endif
		 #endif
     }


     // 6. Listen for a connection request
     if(listen(Server,5)==SOCKET_ERROR)
     {
		 #ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("Error in listening the socket!STOP!!!\n");
			#else
				printf("Error in listening the socket!STOP!!!\n");
			#endif
		 #endif
		 error = 4;
		 return;
     }
     else
     {
		 #ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("Listening process is successfully...\n");
			#else
				printf("Listening process is successfully...\n");
			#endif
		 #endif
     }


     while(shutdownserver == false)
     {
		 #ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("Server is waiting for a connection ...\n");
			#else
				printf("Server is waiting for a connection ...\n");
			#endif
		 #endif

		 this->client_sock = accept(Server, (struct sockaddr*)&cli_addr, &clilen);

		 #ifdef LINUX_MAC
			if (this->client_sock == -1)
			{
				shutdownserver = true;
				break;
			}
			pthread_t threadID;
			pthread_create(&threadID, NULL, StaticOnAccept,(void *)this);
			usleep(10000);

		 #else
		    if (this->client_sock == INVALID_SOCKET)
			{
				shutdownserver = true;
				break;
			}
			_beginthread(StaticOnAccept,0,(void *)this);
			_sleep(10);
		 #endif
     }

     #ifdef LINUX_MAC
	 pthread_exit(NULL);	// end of socket-server thread
     #else
	 _endthread();		// end of socket-server thread
     #endif

}


void SocketServer::Close()
{
    shutdownserver = true;

#ifdef LINUX_MAC
     close(Server);
#else
     closesocket(Server);
     if (WSACleanup() != 0)
     {
		#ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("WSACleanup failed! STOP!!!\n");
			#else
				printf("WSACleanup failed! STOP!!!\n");
			#endif
		#endif

		error = 1;
		return;
     }
     else
     {
		#ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				MessageInterface::ShowMessage("WSACleanup is successful...\n");
			#else
				printf("WSACleanup is successful...\n");
			#endif
		#endif
     }
#endif

}


