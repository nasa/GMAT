//$Id$
//------------------------------------------------------------------------------
//                              PropagatePanel
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
// Author: Waka Waktola
// Created: 2003/09/08
// Modified:
//    2004/05/06 Allison Greene: inherit from GmatPanel
//    2004/10/20 Linda Jun: rename from PropagateCommandPanel
/**
 * This class contains the Propagate command setup window.
 */
//------------------------------------------------------------------------------

#ifndef PropagatePanel_hpp
#define PropagatePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatCommand.hpp"
#include "Propagate.hpp"
#include "StopCondition.hpp"
#include "PropSetup.hpp"
#include "GmatColorPanel.hpp"
#include <wx/treebase.h>

class PropagatePanel : public GmatPanel
{
public:
   // constructors
   PropagatePanel(wxWindow *parent, GmatCommand *cmd, wxTreeItemId nodeId);
   virtual ~PropagatePanel();
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(UnsignedInt type,
                                  const wxString &oldName,
                                  const wxString &newName);
   
private:
   
   static const Integer MAX_PROP_ROW           = 15;
   static const Integer MAX_PROP_COL           = 4;
   static const Integer MAX_STOPCOND_ROW       = 10;
   static const Integer MAX_STOPCOND_COL       = 5;
   static const Integer PROP_NAME_SEL_COL      = 0;
   static const Integer PROP_NAME_COL          = 1;
   static const Integer PROP_SOS_SEL_COL       = 2;
   static const Integer PROP_SOS_COL           = 3;
   static const Integer STOPCOND_LEFT_SEL_COL  = 0;
   static const Integer STOPCOND_LEFT_COL      = 1;
   static const Integer STOPCOND_RELOPER_COL   = 2;
   static const Integer STOPCOND_RIGHT_SEL_COL = 3;
   static const Integer STOPCOND_RIGHT_COL     = 4;
   
   struct PropType
   {
      bool isChanged;
      Integer  soCount;
      wxString propName;
      wxString soNames;
      wxArrayString soNameList;
   };
   
   struct StopCondType
   {
      bool           isChanged;
      wxString       name;
      wxString       desc;
      wxString       varName;
      wxString       relOpStr;
      wxString       goalStr;
      StopCondition *stopCondPtr;
   };
   
   wxGrid *propGrid;
   wxGrid *stopCondGrid;

   wxCheckBox *backPropCheckBox;
   wxCheckBox *stmPropCheckBox;
   wxCheckBox *aMatrixCalcCheckBox;

   wxTextCtrl *mStopTolTextCtrl;
   wxComboBox *mPropModeComboBox;
   wxComboBox *equalityComboBox;
   
   GmatColorPanel *mColorPanel;
   
   bool mPropModeChanged;
   bool mPropDirChanged;
   bool mPropStmChanged;
   bool mCalcAmatrixChanged;
   bool mPropSatChanged;
   bool mStopCondChanged;
   bool mStopTolChanged;
   bool mPropGridSelected;
   bool mStopCondGridSelected;
   bool isPropGridDisabled;
   
   Integer mPropModeCount;
   Integer mPropCount;
   Integer mSpaceObjectCount;
   Integer mStopCondCount;
   PropType mTempProp[MAX_PROP_ROW];
   StopCondType mTempStopCond[MAX_STOPCOND_ROW];
   
   Propagate *thePropCmd;
   Parameter *theParameter;
   wxTreeItemId theNodeId;
   
   wxArrayString mObjectTypeList;
   std::vector<StopCondition*> mRemovedStopCondList;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Layout & data handling methods
   void InitializeData();
   void DisplayPropagator();
   void DisplayStopCondition();
   void UpdateStopCondition(Integer stopRow);
   void GetNewPropagatorName(Integer row, Integer col);
   void GetNewSpaceObjectList(Integer row, Integer col);
   void GetNewStopCondLeftValue(Integer row, Integer col);
   void GetNewStopCondRightValue(Integer row, Integer col);
   
   wxString FormatStopCondDesc(const wxString &varName,
                               const wxString &relOpStr,
                               const wxString &goalStr);
   
   // event handling method
   void OnTextChange(wxCommandEvent& event);
   void OnCellLeftClick(wxGridEvent &event);
   void OnCellRightClick(wxGridEvent &event);
   void OnCellValueChange(wxGridEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent &event);
   void OnGridTabbing(wxGridEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 44000,
      ID_TEXTCTRL,
      ID_COMBOBOX,
      ID_CHECKBOX,
      ID_GRID,
   };
};

#endif // PropagatePanel_hpp

