//$Id$
//------------------------------------------------------------------------------
//                                 MissionTreeToolBar
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
// Created: 2011/08/24
//
/**
 * Shows MissionTree view control tool bar
 */
//------------------------------------------------------------------------------
#ifndef MissionTreeToolBar_hpp
#define MissionTreeToolBar_hpp

#include "gmatwxdefs.hpp"

class MissionTree;

class MissionTreeToolBar: public wxToolBar
{
public:

   MissionTreeToolBar(wxWindow *parent, long style = wxTB_VERTICAL | wxTB_FLAT,
                      wxWindowID id = -1,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      const wxString& name = wxPanelNameStr);
   ~MissionTreeToolBar();
   
   // operations
   //wxPanel*     CreateMissionTreePanel(wxWindow *parent);
   //wxPanel*     GetMissionTreePanel();
   MissionTree* GetMissionTree();
   //wxToolBar*   GetMissionToolBar();
   void         SetMissionTree(MissionTree *missionTree);
   //void         SetGmatNotebook(GmatNotebook *notebook);
   void			ClearFilters();
   void         ResetMissionTreeTools();
   void         SetMissionTreeExpandLevel(int level);
   
private:
   
   wxWindow     *mParent;
   //wxPanel      *mMissionTreePanel;
   MissionTree  *mMissionTree;
   //wxToolBar    *mMissionToolBar;
   
   int     mLastLevelClicked;
   int     mLastIncCategoryClicked;
   int     mLastExcCategoryClicked;
   
   void CreateMissionTreeToolBar();
   void ResetMissionTreeLevel();
   void ResetMissionTreeIncludeCategory();
   void ResetMissionTreeExcludeCategory();
   int  GetOnlyOneClicked(int category);
   
   DECLARE_EVENT_TABLE();
   
   void OnViewByLevel(wxCommandEvent& event);
   void OnViewByCategory(wxCommandEvent& event);
   void OnCustomView(wxCommandEvent& event);
   
   // IDs for the controls and the menu commands
   enum
   {
      ID_BUTTON = 14600,
      TOOL_LEVEL_ALL,
      TOOL_LEVEL_1,
      TOOL_LEVEL_2,
      TOOL_LEVEL_3,
	  TOOL_CLEAR_FILTERS,
      TOOL_INC_PHYSICS,
      TOOL_INC_SOLVER,
      TOOL_INC_SCRIPT,
      TOOL_INC_CONTROL,
      TOOL_EXC_REPORT,
      TOOL_EXC_EQUATION,
      TOOL_EXC_PLOT,
      TOOL_EXC_CALL,
      TOOL_CUSTOM_VIEW,
   };
};

#endif
