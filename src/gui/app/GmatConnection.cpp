//$Id$
//------------------------------------------------------------------------------
//                               GmatConnection
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/08/27
//
/**
 * Implements GmatConnection which provides service to client.
 */
//------------------------------------------------------------------------------

#include "GmatConnection.hpp"
#include "GmatInterface.hpp"
#include "gmatwxdefs.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_CONNECTION
//#define DEBUG_CONNECTION_EXECUTE
//#define DEBUG_CONNECTION_POKE
//#define DEBUG_CONNECTION_REQUEST
//#define DEBUG_CONNECTION_ADVISE


//------------------------------------------------------------------------------
// GmatConnection()
//------------------------------------------------------------------------------
GmatConnection::GmatConnection()
   : wxConnection()
{   
   #ifdef DEBUG_CONNECTION
   MessageInterface::ShowMessage
      ("GmatConnection() constructor entered, this=%p\n", this);
   #endif
}


//------------------------------------------------------------------------------
// ~GmatConnection()
//------------------------------------------------------------------------------
GmatConnection::~GmatConnection()
{
   #ifdef DEBUG_CONNECTION
   MessageInterface::ShowMessage
      ("~GmatConnection() destructor entered, this=%p\n", this);
   #endif   
}

//===========================================
// If using wxWidgets 3.0 or higher
#if wxCHECK_VERSION(3, 0, 0)
//===========================================

//------------------------------------------------------------------------------
// bool OnExec(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool GmatConnection::OnExec(const wxString &WXUNUSED(topic),const wxString &data)
{
   #ifdef DEBUG_CONNECTION_EXECUTE
   MessageInterface::ShowMessage
      ("GmatConnection::OnExecute() command: %s\n", data.WX_TO_C_STRING);
   #endif
   
   return TRUE;
}


//------------------------------------------------------------------------------
// bool OnPoke(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool GmatConnection::OnPoke(const wxString& WXUNUSED(topic),
                            const wxString& item,
                            const void *data,
                            size_t WXUNUSED(size),
                            wxIPCFormat WXUNUSED(format))
{
   #ifdef DEBUG_CONNECTION_POKE
   MessageInterface::ShowMessage
      ("\nGmatConnection::OnPoke() wx%d.%d entered\n   item = '%s'\n   data = '%s'\n",
       wxMAJOR_VERSION, wxMINOR_VERSION, item.WX_TO_C_STRING, data);
   #endif
   
   const char *itemData = (char*)data;
   
   bool retval = RunPoke(itemData);
   
   #ifdef DEBUG_CONNECTION_POKE
   MessageInterface::ShowMessage
      ("GmatConnection::OnPoke() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// const void* OnRequest(const wxString& WXUNUSED(topic), const wxString& item,
//                   size_t * WXUNUSED(size), wxIPCFormat WXUNUSED(format))
//------------------------------------------------------------------------------
/*
 * This method responds to the client application to request data from the server.
 *
 * @param <topic>  Unused
 * @param <item>   Object or parameter name to retrive value from
 * @param <size>   Unused
 * @param <format> Unused
 *
 * @return Object or parameter value string.
 */
//------------------------------------------------------------------------------
const void* GmatConnection::OnRequest(const wxString& WXUNUSED(topic),
                                      const wxString& item,
                                      size_t * WXUNUSED(size),
                                      wxIPCFormat WXUNUSED(format))
{
   #ifdef DEBUG_CONNECTION_REQUEST
   MessageInterface::ShowMessage
      ("\nGmatConnection::OnRequest() wx%d.%d entered\n   item = '%s'\n",
       wxMAJOR_VERSION, wxMINOR_VERSION, item.WX_TO_C_STRING);
   #endif
   
   char *data = RunRequest(item);
   
   #ifdef DEBUG_CONNECTION_REQUEST
   MessageInterface::ShowMessage
      ("GmatConnection::OnRequest() returning '%s'\n", data);
   #endif
   
   return data;
}

//===========================================
#else
//===========================================

//------------------------------------------------------------------------------
// bool OnExecute(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool GmatConnection::OnExecute(const wxString& WXUNUSED(topic),
                               wxChar *data,
                               int WXUNUSED(size),
                               wxIPCFormat WXUNUSED(format))
{
   #ifdef DEBUG_CONNECTION_EXECUTE
   MessageInterface::ShowMessage
      ("GmatConnection::OnExecute() command: %s\n", data);
   #endif
   
   return TRUE;
}


//------------------------------------------------------------------------------
// bool OnPoke(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool GmatConnection::OnPoke(const wxString& WXUNUSED(topic),
                            const wxString& item,
                            wxChar *data,
                            int WXUNUSED(size),
                            wxIPCFormat WXUNUSED(format))
{
   #ifdef DEBUG_CONNECTION_POKE
   MessageInterface::ShowMessage
      ("\nGmatConnection::OnPoke() wx%d.%d entered\n   item = '%s'\n   data = '%s'\n",
       wxMAJOR_VERSION, wxMINOR_VERSION, item.WX_TO_C_STRING, data);
   #endif
   
   const char* itemData = (char*)data;
   
   bool retval = RunPoke(itemData);
   
   #ifdef DEBUG_CONNECTION_POKE
   MessageInterface::ShowMessage
      ("GmatConnection::OnPoke() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// wxChar* OnRequest(const wxString& WXUNUSED(topic), const wxString& item,
//                   int * WXUNUSED(size), wxIPCFormat WXUNUSED(format))
//------------------------------------------------------------------------------
/*
 * This method responds to the client application to request data from the server.
 *
 * @param <topic>  Unused
 * @param <item>   Object or parameter name to retrive value from
 * @param <size>   Unused
 * @param <format> Unused
 *
 * @return Object or parameter value string.
 */
//------------------------------------------------------------------------------
wxChar* GmatConnection::OnRequest(const wxString& WXUNUSED(topic),
                                  const wxString& item,
                                  int * WXUNUSED(size),
                                  wxIPCFormat WXUNUSED(format))
{
   #ifdef DEBUG_CONNECTION_REQUEST
   MessageInterface::ShowMessage
      ("\nGmatConnection::OnRequest() wx%d.%d entered\n   item = '%s'\n",
       wxMAJOR_VERSION, wxMINOR_VERSION, item.c_stre());
   #endif
   
   char *data = RunRequest(item);
   
   #ifdef DEBUG_CONNECTION_REQUEST
   MessageInterface::ShowMessage
      ("GmatConnection::OnRequest() returning '%s'\n", data);
   #endif
   
   return _T(data);
}

//===========================================
#endif
//===========================================


//------------------------------------------------------------------------------
// bool OnStartAdvise(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool GmatConnection::OnStartAdvise(const wxString& WXUNUSED(topic),
                                   const wxString& item)
{
   #ifdef DEBUG_CONNECTION_ADVISE
   MessageInterface::ShowMessage
      ("GmatConnection::OnStartAdvise() %s\n", item.c_str());
   #endif
   
   //#ifdef DEBUG_CONNECTION_ADVISE
   //char* data = GmatInterface::Instance()->GetRunState();
   //MessageInterface::ShowMessage
   //   ("GmatConnection::OnStartAdvise() data=%s\n", data);
   //#endif
   
   return TRUE;
}


//------------------------------------------------------------------------------
// bool OnDisconnect()
//------------------------------------------------------------------------------
bool GmatConnection::OnDisconnect()
{
   #ifdef DEBUG_CONNECTION
   MessageInterface::ShowMessage
      ("GmatConnection::OnDisconnect() entered, this=%p\n", this);
   #endif
   return true;
}


//------------------------------------------------------------------------------
// char* RunRequest(const wxString &item)
//------------------------------------------------------------------------------
char* GmatConnection::RunRequest(const wxString &item)
{
   #ifdef DEBUG_CONNECTION_REQUEST
   MessageInterface::ShowMessage
      ("GmatConnection::RunRequest() entered\n   item = '%s'\n", item.WX_TO_C_STRING);
   #endif
   
   // Check for user interrupt first (loj: 2007.05.11 Added)
   GmatInterface::Instance()->CheckUserInterrupt();
   
   // How can I tell whether item is an object or a parameter?
   // For now GetGMATObject.m appends '.' for object name.
   
   char *data;
   if (item.Last() == '.')
   {
      wxString tempItem = item;
      tempItem.RemoveLast();
      data = GmatInterface::Instance()->GetGmatObject(tempItem.c_str());
   }
   else if (item == "RunState")
   {
      data = GmatInterface::Instance()->GetRunState();
   }
   else if (item == "CallbackStatus")
   {
      data = GmatInterface::Instance()->GetCallbackStatus();
   }
   else if (item == "CallbackResults")
   {
      data = GmatInterface::Instance()->GetCallbackResults();
   }
   else
   {
      data = GmatInterface::Instance()->GetParameter(std::string(item.c_str()));
   }
   
   #ifdef DEBUG_CONNECTION_REQUEST
   MessageInterface::ShowMessage
      ("GmatConnection::RunRequest() returning '%s'\n", data);
   #endif
   
   return data;
}


//------------------------------------------------------------------------------
// bool RunPoke(const char *itemData)
//------------------------------------------------------------------------------
bool GmatConnection::RunPoke(const char *itemData)
{
   const char *data = itemData;
   
   #ifdef DEBUG_CONNECTION_POKE
   MessageInterface::ShowMessage
      ("GmatConnection::RunPoke() entered\n   item = '%s'\n", data);
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
   else if (strncmp(data, "CallbackData", 12) == 0)
   {
      std::string theString(data);
      std::string callbackData = theString.substr(13,1024);
      #ifdef DEBUG_CONNECTION_POKE
         MessageInterface::ShowMessage
            ("   Calling PutCallbackData() with\n"
             "   callbackData = '%s'\n", callbackData.c_str());
      #endif
      GmatInterface::Instance()->PutCallbackData(callbackData);
   }
   else
   {
      GmatInterface::Instance()->PutScript((char*)data);
   }
   
   #ifdef DEBUG_CONNECTION_POKE
   MessageInterface::ShowMessage("GmatConnection::RunPoke() returning true\n");
   #endif
   
   return true;
}
