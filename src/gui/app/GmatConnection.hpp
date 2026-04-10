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
// ** Legal **
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
#ifndef GmatConnection_hpp
#define GmatConnection_hpp

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/ipc.h"

class GmatConnection : public wxConnection
{
public:
   GmatConnection();
   ~GmatConnection();
   
   // If using wxWidgets 3.0 or higher
   #if wxCHECK_VERSION(3, 0, 0)
   
   virtual bool         OnExec(const wxString &topic, const wxString &data);
   virtual bool         OnPoke(const wxString &topic, const wxString &item,
                               const void *data, size_t size, wxIPCFormat format);
   virtual const void*  OnRequest(const wxString &topic, const wxString &item,
                                  size_t *size, wxIPCFormat format);
   #else
   
   virtual bool         OnExecute(const wxString &topic, wxChar *data, int size,
                                  wxIPCFormat format);
   virtual bool         OnPoke(const wxString &topic, const wxString &item,
                               wxChar *data, int size, wxIPCFormat format);
   virtual wxChar*      OnRequest(const wxString &topic, const wxString &item,
                                  int *size, wxIPCFormat format);
   
   #endif
   
   virtual bool         OnStartAdvise(const wxString &topic, const wxString &item);
   virtual bool         OnDisconnect();

protected:
   char* RunRequest(const wxString &item);
   bool  RunPoke(const char *itemData);
};

#endif // GmatConnection_hpp
