//$Header: /GMAT/dev/cvs/matlab/gmat_mex/src/MatlabClient.cpp,v 1.2 2006/08/28 17:42:06 wshoan Exp $ 
//------------------------------------------------------------------------------
//                               MatlabClient
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
 * Implements MatlabClient which creates new connection to GMAT server.
 */
//------------------------------------------------------------------------------

#include "MatlabClient.hpp"
#include "MatlabConnection.hpp"
#include "ipcsetup.h"

//#define DEBUG_CLIENT 1

#ifdef DEBUG_CLIENT
#include <iostream>
#include "MessageInterface.hpp"
#endif

// ----------------------------------------------------------------------------
// MatlabClient
// ----------------------------------------------------------------------------
MatlabClient::MatlabClient() : wxClient()
{
    m_connection = NULL;
}

bool MatlabClient::Connect()
{
    return Connect(IPC_HOST,IPC_SERVICE,IPC_TOPIC);
}

bool MatlabClient::Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic)
{

#ifdef DEBUG_CLIENT
    //std::cout << "\nMatlab::Connect(wxString,x,y) enters...\n";
    MessageInterface::ShowMessage("\nMatlab::Connect(wxString,x,y) enters...\n");
#endif

    m_connection = (MatlabConnection *)MakeConnection(sHost, sService, sTopic);

#ifdef DEBUG_CLIENT
    //std::cout << "\nMatlab::Connect(wxString,x,y) is about exiting...\n";
    MessageInterface::ShowMessage("\nMatlab::Connect(wxString,x,y) is about exiting...\n");
#endif

    return m_connection    != NULL;
}

wxConnectionBase *MatlabClient::OnMakeConnection()
{
#ifdef DEBUG_CLIENT
    //std::cout << "\nMatlabClient::MakeConnection() enters...\n";
    MessageInterface::ShowMessage("\nMatlabClient::MakeConnection() enters...\n");
#endif

    return new MatlabConnection;
}


bool MatlabClient::Disconnect()
{
    if (m_connection)
    {
        m_connection->Disconnect();
        delete m_connection;
        m_connection = NULL;

        return true;
    }
    else
        return false;
}

MatlabClient::~MatlabClient()
{
    Disconnect();
}
