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
// Author: Linda Jun
// Created: 2004/02/06
/**
 * Implements MissionTreeItemData class.
 */
//------------------------------------------------------------------------------
#include "MissionTreeItemData.hpp"

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// MissionTreeItemData(const wxString &name, GmatTree::ItemType type,
//                     const wxString &title, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Constructs MissionTreeItemData object.
 *
 * @param <name> command name
 * @param <type> input int for type of data.
 * @param <title> input string for description of node.
 * @param <cmd> command pointer
 *
 * @note Creates the MissionTreeItemData object.
 */
//------------------------------------------------------------------------------
MissionTreeItemData::MissionTreeItemData(const wxString &name, GmatTree::ItemType type,
                                         const wxString &title, GmatCommand *cmd)
   : GmatTreeItemData(name, type, title, false)
{
   // Set title as name for mission tree since they are the same
   mItemTitle = name;
   theCommand = cmd;
}


//------------------------------------------------------------------------------
// virtual GmatCommand* GetCommand()
//------------------------------------------------------------------------------
/**
 * @return Command pointer
 */
//------------------------------------------------------------------------------
GmatCommand* MissionTreeItemData::GetCommand()
{
   return theCommand;
}

//------------------------------------------------------------------------------
// virtual void SetCommand(GmatCommand* cmd)
//------------------------------------------------------------------------------
/**
 * Sets command
 */
//------------------------------------------------------------------------------
void MissionTreeItemData::SetCommand(GmatCommand* cmd)
{
   theCommand = cmd;
}

//------------------------------------------------------------------------------
// virtual wxTreeItemId GetNodeId()
//------------------------------------------------------------------------------
wxTreeItemId MissionTreeItemData::GetNodeId()
{
   return theNodeId;
}

//------------------------------------------------------------------------------
// virtual void SetNodeId(const wxTreeItemId &nodeId)
//------------------------------------------------------------------------------
void MissionTreeItemData::SetNodeId(const wxTreeItemId &nodeId)
{
   theNodeId = nodeId;
}
