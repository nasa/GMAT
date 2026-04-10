// GmatSocketServer.h
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
//      Author: Tuan Nguyen

#ifndef GmatSocketServer_hpp
#define GmatSocketServer_hpp

#include <wx/event.h>

#ifdef LINUX_MAC
	#include <unistd.h>
	#include <pthread.h>

	#include <sys/time.h>
	#include <sys/select.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>

	#define SOCKET_ERROR	-1
	#define INVALID_SOCKET	-1

#else
	#include <winsock2.h>
	#include <process.h>
#endif


#define IP_ADDRESS	"localhost"
//#define IP_ADDRESS	"128.183.221.98"
#define TCP_PORT	3000


class GmatSocketServer
{
public:
	GmatSocketServer(wxEvtHandler* handler);
	virtual ~GmatSocketServer();

	void Close();
	void SetEventHandler(wxEvtHandler* handler) {evthandler = handler;}

#ifdef LINUX_MAC
	bool RunRequest(int sock);
#else
	bool RunRequest(SOCKET sock);
#endif
	char* OnRequest(char* item);
	bool OnPoke(char* data);

	void RunServer();
#ifdef LINUX_MAC
	void OnAccept(int sock);
	static void* StaticOnAccept(void* objPtr)
	{
		GmatSocketServer* pThis = (GmatSocketServer*)objPtr;
		pThis->OnAccept(pThis->client_sock);
		return NULL;
	}
	static void* StaticRunServer(void* objPtr)
	{
		GmatSocketServer* pThis = (GmatSocketServer*)objPtr;
		pThis->RunServer();
      		return NULL;
	}
#else
	void OnAccept(SOCKET sock);
	static void StaticOnAccept(void* objPtr)
	{
		GmatSocketServer* pThis = (GmatSocketServer*)objPtr;
		pThis->OnAccept(pThis->client_sock);
	}
	static void StaticRunServer(void* objPtr)
	{
		GmatSocketServer* pThis = (GmatSocketServer*)objPtr;
		pThis->RunServer();
	}
#endif
private:
	int m_numClients;
	int error;
	bool shutdownserver;

	wxEvtHandler* evthandler;

#ifdef LINUX_MAC
        int Server;
	int client_sock;
#else
	SOCKET client_sock;
        SOCKET Server;
#endif
};

enum
{
	ID_SOCKET_POKE = 10000,
	ID_SOCKET_REQUEST,
};
#endif /* GmatSocketServer_hpp */
