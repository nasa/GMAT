//$Header: /GMAT/dev/cvs/matlab/gmat_mex/src/MatlabClient.hpp,v 1.1 2005/12/22 19:07:19 jgurgan Exp $
//------------------------------------------------------------------------------
//                               MatlabClient
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
 * Declares MatlabClient which creates new connection to GMAT server.
 */
//------------------------------------------------------------------------------

#ifndef MatlabClient_hpp
#define MatlabClient_hpp

#include "gmatwxdefs.hpp"
#include "wx/ipc.h"
#include "MatlabConnection.hpp"

class MatlabClient: public wxClient
{
public:
    MatlabClient();
    ~MatlabClient();
    bool Connect();
    bool Connect(const wxString& sHost, const wxString& sService, 
                 const wxString& sTopic);
    bool Disconnect();
    wxConnectionBase *OnMakeConnection();
    bool IsConnected() { return m_connection != NULL; };
    MatlabConnection *GetConnection() { return m_connection; };

protected:
    MatlabConnection     *m_connection;
};

#endif // MatlabClient_hpp
