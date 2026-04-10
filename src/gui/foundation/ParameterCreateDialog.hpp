//$Id$
//------------------------------------------------------------------------------
//                              ParameterCreateDialog
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
// Created: 2004/02/25
//
/**
 * Declares ParameterCreateDialog class. This class shows dialog window where a
 * user parameter can be created.
 * 
 */
//------------------------------------------------------------------------------
#ifndef ParameterCreateDialog_hpp
#define ParameterCreateDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include <wx/string.h> // for wxArrayString

#include "Parameter.hpp"

class ParameterCreateDialog : public GmatDialog
{
public:
   
   // parameter type to create 
   enum ParameterType
   {
      VARIABLE,
      ARRAY,
      STRING
   };
   
   ParameterCreateDialog(wxWindow *parent, ParameterType paramType);
   ParameterCreateDialog(wxWindow *parent, const wxString paramName);
   ~ParameterCreateDialog();
   
   wxArrayString& GetParamNames()
      { return mParamNames; }
   bool IsParamCreated()
      { return mIsParamCreated; }
   virtual void SetParameterType( ParameterType paramType );
   virtual void OnHelp(wxCommandEvent &event);
   
   // OK button works like Close so reimplement here
   virtual void OnOK(wxCommandEvent &event);
   
protected:

   Parameter *mCurrParam;
   
   wxArrayString mParamNames;
   wxArrayString mExcludedScList;
   wxArrayString mSelectVarStrings;
   
   ParameterType mParamType;
   bool mIsParamCreated;
   bool mPageChangedByUser;
   bool mArrayChanged;
   bool mVariableChanged;
   bool mStringChanged;
   
   wxTextCtrl *mVarNameTextCtrl;
   wxTextCtrl *mVarValueTextCtrl;
   wxTextCtrl *mStringNameTextCtrl;
   wxTextCtrl *mStringValueTextCtrl;
   wxTextCtrl *mArrNameTextCtrl;
   wxTextCtrl *mArrRowTextCtrl;
   wxTextCtrl *mArrColTextCtrl;
   wxTextCtrl *mArrValueTextCtrl;

   wxButton *mCreateVariableButton;
   wxButton *mSelectButton;
   wxButton *mCreateStringButton;
   wxButton *mCreateArrayButton;
   wxButton *mEditArrayButton;

   wxBitmapButton *mVarClearButton;
   wxBitmapButton *mArrClearButton;
   wxBitmapButton *mStrClearButton;

   wxListBox *mUserVarListBox;
   wxListBox *mUserStringListBox; 
   wxListBox *mUserArrayListBox;
   
   wxNotebook *notebook;
   wxBoxSizer *mDetailsBoxSizer;

   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   virtual void ResetControls();
   
   // event handling
   void OnVarTextUpdate(wxCommandEvent& event);
   void OnStrTextUpdate(wxCommandEvent& event);
   void OnAryTextUpdate(wxCommandEvent& event);
   void OnCreateButton(wxCommandEvent& event);
   void OnSelectButtonClick(wxCommandEvent& event);
   void OnClearButtonClick(wxCommandEvent& event);
   void OnEditArrayButtonClick(wxCommandEvent& event);
   void OnPageChanged(wxNotebookEvent& event);
   void OnPageChanging(wxNotebookEvent& event);
   void OnListboxClick(wxCommandEvent& event);
   void OnTextEnter(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9200,
      ID_SELECT_BUTTON,
      ID_EDITARRAY_BUTTON,
      ID_CREATE_BUTTON,
      ID_CLEAR_VAR_BUTTON,
      ID_CLEAR_ARR_BUTTON,
      ID_CLEAR_STR_BUTTON,
      ID_VARTEXTCTRL,
      ID_ARYTEXTCTRL,
      ID_NOTEBOOK,
      ID_LISTBOX,
      ID_STRTEXTCTRL
   };

private:
   Parameter* CreateParameter(const wxString &name);
   void CreateVariable();
   void CreateString();
   void CreateArray();
   void SetVariableToAnotherObject(const std::string &varName,
                                   const std::string &varExpr);
};

#endif
