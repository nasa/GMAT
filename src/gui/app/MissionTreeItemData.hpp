//$Id$
//------------------------------------------------------------------------------
//                             MissionTreeItemData
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
// Author: Linda Jun
// Created: 2004/02/06
/**
 * This class provides a data type for tree nodes.
 */
//------------------------------------------------------------------------------
#ifndef MissionTreeItemData_hpp
#define MissionTreeItemData_hpp

#include "gmatwxdefs.hpp"
#include "GmatTreeItemData.hpp"

#include "GmatCommand.hpp"

class MissionTreeItemData : public GmatTreeItemData
{
public:
   MissionTreeItemData(const wxString &name, GmatTree::ItemType type,
                       const wxString &title = "", GmatCommand *cmd = NULL);
   
   virtual GmatCommand* GetCommand();
   virtual void SetCommand(GmatCommand *cmd);
   virtual wxTreeItemId GetNodeId();
   virtual void SetNodeId(const wxTreeItemId &nodeId);
   
protected:
   GmatCommand *theCommand;
   wxTreeItemId theNodeId;
};
#endif // MissionTreeItemData_hpp
