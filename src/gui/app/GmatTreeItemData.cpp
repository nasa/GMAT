//$Id$
//------------------------------------------------------------------------------
//                             GmatTreeItemData
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
// Author: Allison Greene
// Created: 2003/10/08
/**
 * This class provides a data type for tree nodes.
 */
//------------------------------------------------------------------------------
#include "GmatTreeItemData.hpp"

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatTreeItemData(const wxString &name, GmatTree::ItemType type,
//                  const wxString &title, bool isClonable)
//------------------------------------------------------------------------------
/**
 * Constructs GmatTreeItemData object.
 *
 * @param <name>  input string for name of node which is object name
 * @param <type>  input int for type of object.
 * @param <title> input string for title of node.
 * @param <isClonable> input flag indicating that this name and type is clonable
 *
 * @note Creates the GmatTreeItemData object.
 */
//------------------------------------------------------------------------------
GmatTreeItemData::GmatTreeItemData(const wxString &name, GmatTree::ItemType type,
                                   const wxString &title, bool isClonable,
                                   bool pluginGui)
{
   mItemName = name;
   mItemType = type;
   mItemTitle = title;
   mIsClonable = isClonable;
   mHasPluginGui = pluginGui;
}


//------------------------------------------------------------------------------
// wxString GetName()
//------------------------------------------------------------------------------
wxString GmatTreeItemData::GetName()
{
   return mItemName;
}


//------------------------------------------------------------------------------
// wxString GetTitle()
//------------------------------------------------------------------------------
wxString GmatTreeItemData::GetTitle()
{
   return mItemTitle;
}


//------------------------------------------------------------------------------
// GmatTree::ItemType GetItemType()
//------------------------------------------------------------------------------
GmatTree::ItemType GmatTreeItemData::GetItemType()
{
   return mItemType;
}


//------------------------------------------------------------------------------
// bool IsClonable()
//------------------------------------------------------------------------------
bool GmatTreeItemData::IsClonable()
{
   return mIsClonable;
}


//------------------------------------------------------------------------------
// void SetName(const wxString &name)
//------------------------------------------------------------------------------
void GmatTreeItemData::SetName(const wxString &name)
{
   mItemName = name;
}


//------------------------------------------------------------------------------
// void SetTitle(const wxString &title)
//------------------------------------------------------------------------------
void GmatTreeItemData::SetTitle(const wxString &title)
{
   mItemTitle = title;
}


//------------------------------------------------------------------------------
// void SetItemType(GmatTree::ItemType type)
//------------------------------------------------------------------------------
void GmatTreeItemData::SetItemType(GmatTree::ItemType type)
{
   mItemType = type;
}


//------------------------------------------------------------------------------
// void SetClonable(bool clonable)
//------------------------------------------------------------------------------
void GmatTreeItemData::SetClonable(bool clonable)
{
   mIsClonable = clonable;
}


//------------------------------------------------------------------------------
// void SetPluginGui(bool pluginGui)
//------------------------------------------------------------------------------
/**
 * Sets the flag indicating is there is an associated PluginGui panel
 *
 * @param pluginGui The new setting
 */
//------------------------------------------------------------------------------
void GmatTreeItemData::SetPluginGui(bool pluginGui)
{
   mHasPluginGui = pluginGui;
}


//------------------------------------------------------------------------------
// firtual Command* GetCommand()
//------------------------------------------------------------------------------
/**
 * @return Command pointer
 */
//------------------------------------------------------------------------------
GmatCommand* GmatTreeItemData::GetCommand()
{
   return NULL;
}


//------------------------------------------------------------------------------
// virtual wxTreeItemId GetNodeId()
//------------------------------------------------------------------------------
wxTreeItemId GmatTreeItemData::GetNodeId()
{
   return wxTreeItemId();
}

//------------------------------------------------------------------------------
// bool HasPluginGui()
//------------------------------------------------------------------------------
/**
 * Method used to check for plugin GUI code
 *
 * @return true if there is plugin GUI code, false if not
 */
//------------------------------------------------------------------------------
bool GmatTreeItemData::HasPluginGui()
{
   return mHasPluginGui;
}
