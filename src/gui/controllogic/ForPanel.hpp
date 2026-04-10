//$Id$
//------------------------------------------------------------------------------
//                              ForPanel
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
// Author: Allison Greene
// Created: 2004/05/17
//
/**
 * Declares the For loop setup window.
 */
//------------------------------------------------------------------------------
#ifndef ForPanel_hpp
#define ForPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "Parameter.hpp"
#include "MessageInterface.hpp"

#include "gmatdefs.hpp"
#include "GmatCommand.hpp"
#include "For.hpp"

class ForPanel : public GmatPanel
{
public:
   // constructors/destructors
   ForPanel(wxWindow *parent, GmatCommand *cmd);
   ~ForPanel();  

private:

   enum GridColumn
   {
      INDEX_SEL_COL = 0,
      INDEX_COL,
      START_SEL_COL,
      START_COL,
      INCR_SEL_COL,
      INCR_COL,
      END_SEL_COL,
      END_COL,
      MAX_COL,
   };
   
   wxString mIndexString;
   wxString mStartString;
   wxString mEndString;
   wxString mIncrString;
   wxGrid *conditionGrid;
   
   bool mIndexIsSet;
   bool mIndexIsParam;
   bool mStartIsParam;
   bool mEndIsParam;
   bool mIncrIsParam;
   
   Real mStartValue;
   Real mIncrValue;
   Real mEndValue;  
   
   wxArrayString mObjectTypeList;
   For *theForCommand;
   Parameter* mIndexParam;  
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // data handling methods
   void GetNewValue(Integer row, Integer col);
   
   // event handling methods
   void OnCellLeftClick(wxGridEvent& event);
   void OnCellRightClick(wxGridEvent& event);
   void OnCellValueChange(wxGridEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_GRID = 99000
   };
};

#endif // ForPanel_hpp
