// GmatSocketServer.cpp
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
// ** Legal **
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
//
//  Created on: Mar 22, 2011
//     Author: Tuan Nguyen
//

#include "GmatMainFrame.hpp"
#include "GmatSocketServer.hpp"
#include "MessageInterface.hpp"

#define DEBUG_SOCKET


#ifndef MessageInterface_hpp
	#include <stdio.h>
	#include <ctype.h>
	#include <string>
#endif

using namespace std;

GmatSocketServer::GmatSocketServer(wxEvtHandler* handler)
{
	error = 0;
	m_numClients = 0;
	shutdownserver = false;

	evthandler = handler;

#ifdef LINUX_MAC
#else
	client_sock = 0;
#endif
}

GmatSocketServer::~GmatSocketServer()
{
}

#ifdef LINUX_MAC
bool GmatSocketServer::RunRequest(int sock)
#else
bool GmatSocketServer::RunRequest(SOCKET sock)
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
			#ifdef LINUX_MAC
			printf("Client %d: Read message:%s\n", sock, buf);
			#else
			MessageInterface::ShowMessage("Client %d: Read message:%s\n", sock, buf);
			#endif
		#else
			printf("Client %d: Read message:%s\n", sock, buf);
		#endif
	#endif


	// 2. Echo back the data
	send(sock, buf, len, 0);
	#ifdef DEBUG_SOCKET
		#ifdef MessageInterface_hpp
			#ifdef LINUX_MAC
			printf("Client %d: Echo back:%s\n", sock, buf);
			#else
			MessageInterface::ShowMessage("Client %d: Echo back:%s\n", sock, buf);
			#endif
		#else
			printf("Client %d: Echo back:%s\n", sock, buf);
		#endif
	#endif


	// 3. if the received message contains 'Request' string then run
	//    OnRequest() function
	if (strncmp(buf, "Request,", strlen("Request,")) == 0)
	{
		msg = &buf[strlen("Request,")];

	    // 3.1.1. save reuqest message to GmatMainFrame.socketrequest:
	    GmatMainFrame* mainframe = (GmatMainFrame*)evthandler;
	    strcpy(&mainframe->socketrequest[0], msg);
 
        // 3.1.2. Create and send wxEVT_SOCKET_REQUEST:
        wxCommandEvent* evt = new wxCommandEvent();
        evt->SetEventType(wxEVT_SOCKET_REQUEST);
        evt->SetId(ID_SOCKET_REQUEST);
        wxPostEvent(evthandler, *evt);

        // 3.1.3. Get result:
        do
	    {
           #ifdef LINUX_MAC
              usleep(1000);
           #else
              _sleep(1);
           #endif
		}while (((GmatMainFrame*)evthandler)->socketresult[0] == '\0');
        char* result = ((GmatMainFrame*)evthandler)->socketresult;
		// char* result = OnRequest(msg);

		// 3.2. Read 'Idle' state
//		#ifdef LINUX_MAC
//		while (select(0, &socks_set, NULL, NULL, &time) == 0)
//		{
//			usleep(1000);
//		}
//		#else
//		while (select(sock+1, &socks_set, NULL, NULL, &time) == 0)
//		{
//			_sleep(1);
//		}
//		#endif
		recv(sock, &len1c, 1, 0);
		len1 = (unsigned char)len1c;
		buf1 = new char[len1+1];
		recv(sock, buf1, len1, 0);
		buf1[len1] = '\0';

		// 3.3. Send results to client
		send(sock, result, strlen(result), 0);
		#ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				#ifdef LINUX_MAC
				printf("Client %d: Send result:%s\n", sock, result);
				#else
				MessageInterface::ShowMessage("Client %d: Send result:%s\n", sock, result);
				#endif
			#else
				printf("Client %d: Send result:%s\n", sock, result);
			#endif
		#endif

		((GmatMainFrame*)evthandler)->socketresult[0] = '\0';

		delete[] buf1;
   }
   else if (strncmp(buf, "script,", strlen("script,")) == 0)
   {
	    // 4.Run OnPoke function:
	    msg = &buf[strlen("script,")];

	    // 4.1. save poke message to GmatMainFrame.socketrequest:
	    GmatMainFrame* mainframe = (GmatMainFrame*)evthandler;
        while (mainframe->socketrequest[0]!= '\0')
        {
            #ifdef LINUX_MAC
               usleep(1000);
            #else
               _sleep(1);
            #endif
        }
        mainframe->socketrequest = new char[strlen(msg)+1];
	    strcpy(&mainframe->socketrequest[0], msg); 

	    // 4.2. Create and send wxEVT_SOCK_POKE:
	    wxCommandEvent* evt = new wxCommandEvent();
	    evt->SetEventType(wxEVT_SOCKET_POKE);
	    evt->SetId(ID_SOCKET_POKE);
	    wxPostEvent(evthandler, *evt);

	    // OnPoke(msg);

	    if (strcmp(msg, "Close;") == 0)
	    	return true;
   }

   delete[] buf;

   return false;
}



#ifdef LINUX_MAC
void GmatSocketServer::OnAccept(int sk)
#else
void GmatSocketServer::OnAccept(SOCKET sk)
#endif
{
	++m_numClients;

	#ifdef DEBUG_SOCKET
		#ifdef MessageInterface_hpp
			#ifdef LINUX_MAC
			printf("number of clients = %d\n", m_numClients);
			#else
			MessageInterface::ShowMessage("number of clients = %d\n", m_numClients);
			#endif
		#else
			printf("number of clients = %d\n", m_numClients);
		#endif
	#endif

	bool stop = false;
	while((!stop)&&(!shutdownserver))
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
			#ifdef LINUX_MAC
			printf("number of clients = %d\n", m_numClients);
			#else
			MessageInterface::ShowMessage("number of clients = %d\n", m_numClients);
			#endif
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



void GmatSocketServer::RunServer()
{
	 struct sockaddr_in serv_addr,cli_addr;
#ifdef LINUX_MAC
//	 int Server;
	 socklen_t clilen=sizeof(cli_addr);
#else
//	 SOCKET Server;
	 int clilen=sizeof(cli_addr);
#endif

	 // 0. Set event handler:

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
				#ifdef LINUX_MAC
				printf("Socket creation failed!STOP!!!\n");
				#else
				MessageInterface::ShowMessage("Socket creation failed!STOP!!!\n");
				#endif
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
				#ifdef LINUX_MAC
				printf("Socket is created successfully...\n");
				#else
				MessageInterface::ShowMessage("Socket is created successfully...\n");
				#endif
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
				#ifdef LINUX_MAC
				printf("Attempt to bind failed!STOP!!!\n");
				#else
				MessageInterface::ShowMessage("Attempt to bind failed!STOP!!!\n");
				#endif
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
				#ifdef LINUX_MAC
				printf("Bind process is created successfully...\n");
				#else
				MessageInterface::ShowMessage("Bind process is created successfully...\n");
				#endif
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
				#ifdef LINUX_MAC
				printf("Error in listening the socket!STOP!!!\n");
				#else
				MessageInterface::ShowMessage("Error in listening the socket!STOP!!!\n");
				#endif
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
				#ifdef LINUX_MAC
				printf("Listening process is successfully...\n");
				#else
				MessageInterface::ShowMessage("Listening process is successfully...\n");
				#endif
			#else
				printf("Listening process is successfully...\n");
			#endif
		 #endif
     }


     while(shutdownserver == false)
     {
		 #ifdef DEBUG_SOCKET
			#ifdef MessageInterface_hpp
				#ifdef LINUX_MAC					
				printf("Server is waiting for a connection ...\n");
				#else
				MessageInterface::ShowMessage("Server is waiting for a connection ...\n");
				#endif
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

     error = -1;
     return;	
}

void GmatSocketServer::Close()
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

