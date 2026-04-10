//$Id$
//------------------------------------------------------------------------------
//                              ArraySetupDialog
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
// Created: 2004/12/30
//
/**
 * Declares ArraySetupDialog class. This panel enables user to see and modify
 * array values.
 */
//------------------------------------------------------------------------------

#ifndef ArraySetupDialog_hpp
#define ArraySetupDialog_hpp

#include "GmatDialog.hpp"
#include "Parameter.hpp"
#include "Rmatrix.hpp"

class ArraySetupDialog: public GmatDialog
{
public:
   // constructors
   ArraySetupDialog(wxWindow *parent, const wxString &name); 
   
private:
   
   Rmatrix mRmat;  // for saving array values
   Parameter *mParam;
   bool mIsArrValChanged;
   
   int mNumRows;
   int mNumCols;
   
   wxString mVarName;
   
   wxTextCtrl *mArrNameTextCtrl;
   wxTextCtrl *mArrRowTextCtrl;
   wxTextCtrl *mArrColTextCtrl;
   wxTextCtrl *mArrValTextCtrl;
   
   wxButton *mUpdateButton;
   
   wxComboBox *mRowComboBox;
   wxComboBox *mColComboBox;
   
   wxGrid *mArrGrid;
   
   wxBoxSizer *mPageBoxSizer;
   wxStaticBoxSizer *mArrStaticBoxSizer;
   wxBoxSizer *mArrValBoxSizer;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnTextEnter(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnButtonClick(wxCommandEvent& event);
   void OnGridCellChange(wxGridEvent& event);
   void OnGridTabbing(wxGridEvent& event);
   
   void UpdateCellValue();
   bool CheckCellValue(Real &rval, int row, int col, const char *str);
   bool CheckCellValue(Real &rval, int row, int col, const std::string &str);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL,
      ID_GRID
   };
};

#endif
