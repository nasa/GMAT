//$Header: /GMAT/dev/cvs/matlab/gmat_mex/src/MatlabConnection.cpp,v 1.2 2006/08/28 17:42:06 wshoan Exp $
//------------------------------------------------------------------------------
//                            MatlabConnection
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author:  Joey Gurganus
// Created: 2005/11/30
//
/**
 * Implements MatlabConnnection to provide server/client.
 */
//------------------------------------------------------------------------------

#include "MatlabConnection.hpp"

//#define DEBUG_CONNECT 1

#ifdef DEBUG_CONNECT
#include <iostream>
#include "MessageInterface.hpp"
#endif

// ----------------------------------------------------------------------------
// MatlabConnection
// ----------------------------------------------------------------------------
bool MatlabConnection::OnAdvise(const wxString& topic, const wxString& item, 
                                wxChar *data, int size, wxIPCFormat format)
{
#ifdef DEBUG_CONNECT
    //std::cout << "\nMatlabConnection::OnAdvise() enters...\n";
    MessageInterface::ShowMessage("\nMatlabConnection::OnAdvise() enters...\n");
#endif

    return true;
}

bool MatlabConnection::OnDisconnect()
{
#ifdef DEBUG_CONNECT
    //std::cout << "\nMatlabConnection::OnDisconnect() enters...\n";
    MessageInterface::ShowMessage("\nMatlabConnection::OnDisconnect() enters...\n");
#endif
    return true;
}

bool MatlabConnection::Execute(const wxChar *data, int size, wxIPCFormat format)
{
#ifdef DEBUG_CONNECT
    //std::cout << "\nMatlabConnection::Execute() enters...\n";
    MessageInterface::ShowMessage("\nMatlabConnection::Execute() enters...\n");
#endif

    bool retval = wxConnection::Execute(data, size, format);

#ifdef DEBUG_CONNECT
    if (!retval)
       //std::cout << "\nExecute failed! and MatlabConnect::Execute exits\n";
       MessageInterface::ShowMessage("\nExecute failed! and MatlabConnect::Execute exits\n");
#endif

    return retval;
}

wxChar *MatlabConnection::Request(const wxString& item, int *size, 
                                  wxIPCFormat format)
{
#ifdef DEBUG_CONNECT
    //std::cout << "\nMatlabConnection::Execute() enters...\n";
    MessageInterface::ShowMessage("\nMatlabConnection::Request() enters...\n");
    MessageInterface::ShowMessage(" ... and item = %s\n", item.c_str());
#endif
    wxChar *data =  (wxChar*)wxConnection::Request(item, (size_t*)size, format);
    return data;
}

bool MatlabConnection::Poke(const wxString& item, wxChar *data, int size, 
                            wxIPCFormat format)
{
   return wxConnection::Poke(item, data, size, format);
}
