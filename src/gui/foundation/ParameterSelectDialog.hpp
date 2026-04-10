//$Id$
//------------------------------------------------------------------------------
//                              ParameterSelectDialog
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
 * Declares ParameterSelectDialog class. This class shows dialog window where a
 * user parameter can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef ParameterSelectDialog_hpp
#define ParameterSelectDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include "GuiItemManager.hpp"
#include <wx/config.h>

class ParameterSelectDialog : public GmatDialog
{
public:
   
   ParameterSelectDialog(wxWindow *parent,
                         const wxArrayString &objectTypeList,
                         int  showOption = GuiItemManager::SHOW_PLOTTABLE,
                         int  showWholeObjOption = 0,
                         bool allowMultiSelect = false,
                         bool allowString = false,
                         bool allowSysParam = true,
                         bool allowVariable = true,
                         bool allowArray = true,
                         bool allowArrayElement = true,
                         const wxString &objectType = "Spacecraft",
                         bool createParam = true,
                         bool showSettableOnly = false,
                         bool skipDependency = false,
                         bool forStopCondition = false);
   ~ParameterSelectDialog();

   bool HasSelectionChanged();
   bool IsParamSelected();
   wxString GetParamName();
   void SetObjectType(const wxString &objType);
   wxArrayString& GetParamNameArray();
   void SetParamNameArray(const wxArrayString &paramNames);
   
protected:
   
   wxString mParamName;
   wxArrayString mObjectTypeList;
   wxString mObjectType;
   wxString mLastCoordSysName;
   // the last non-blank CS name selected
   wxString mPreviousCoordSysName;
   wxArrayString mParamNameArray;
   
   bool mHasSelectionChanged;
   bool mIsParamSelected;
   bool mIsAddingMode;
   bool mCanClose;
   bool mUseUserParam;
   bool mAllowSysParam;
   bool mAllowVariable;
   bool mAllowArray;
   bool mAllowArrayElement;
   bool mAllowString;
   bool mAllowMultiSelect;
   bool mCreateParam;
   bool mShowSettableOnly;
   bool mSkipDependency;
   bool mForStopCondition;
   bool mAllowWholeObject;
//   bool mLastCSHasFixedOnly;
//   bool mLastCSHasCBOriginOnly;
   
   int  mShowOption;
   int  mShowObjectOption;
   int  mNumRow;
   int  mNumCol;
   int  mLastHardwareSelection;
   
   wxArrayInt mLastObjectSelections;
   wxArrayInt mLastPropertySelections;
   
   wxStaticText *mHardwareStaticText;
   wxStaticText *mRowStaticText;
   wxStaticText *mColStaticText;
   
   wxTextCtrl *mRowTextCtrl;
   wxTextCtrl *mColTextCtrl;
   
   wxStaticText *mCoordSysLabel;
   
   wxButton *mUpButton;
   wxButton *mDownButton;
   wxButton *mAddButton;
   wxButton *mRemoveButton;
   wxButton *mAddAllButton;
   wxButton *mRemoveAllButton;
   
   wxComboBox *mObjectTypeComboBox;
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mCentralBodyComboBox;
   wxComboBox *mODEModelComboBox;
   
   wxListBox *mUserParamListBox;
   wxListBox *mObjectListBox;
   wxListBox *mHardwareListBox;
   wxListBox *mPropertyListBox;
   wxListBox *mSelectedListBox;
   
   wxCheckBox *mEntireObjectCheckBox;
   
   wxBoxSizer *mVarBoxSizer;
   wxBoxSizer *mCoordSysSizer;
   wxSizer *mParameterSizer;
   
   wxConfigBase *mConfig;
   
   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // virtual methods from GmatDialog
   virtual void OnCancel(wxCommandEvent &event);
   
   // event handling
   void OnButtonClick(wxCommandEvent& event);
   void OnListBoxSelect(wxCommandEvent& event);
   void OnListBoxDoubleClick(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {
      TEXT_ID = 9300,
      TEXTCTRL_ID,
      COMBOBOX_ID,
      BUTTON_ID,
      LISTBOX_ID,
      CHECKBOX_ID,
   };
   
private:
   
   bool AddWholeObject();
   bool AddParameterSelection();
   void AddParameter(const wxString &param);
   bool AddMultipleSelections();
   void AddAll();
   void RemoveParameter();
   void ShowSpacecraft();
   void ShowSpacePoints();
   void ShowImpulsiveBurns();
   void ShowFiniteBurns();
   void ShowArrays();
   void ShowStrings();
   void ShowVariables();
   void ShowArrayIndex(bool show);
   void ShowAttachedHardware(bool show);
   void ShowObjectProperties();
   void BuildAttachedHardware(const wxString &scName);
   void ShowHardwareProperties(const wxString &scName, const wxString &hwName);
   void ShowCoordSystem(bool showBlank = false);
   void ClearProperties();
   void DeselectAllObjects();
   void DeselectObjects(wxArrayInt &newSelects, wxArrayInt &oldSelects);
   int  GetLastPropertySelection();
   
   wxString GetObjectSelection();
   wxString GetPropertySelection();
   wxString FormArrayElement();
   wxString FormParameterName();
   
   Parameter* GetParameter(const wxString &name);
   
   void DisplayWarning(const wxString &arg1, const wxString &ar2 = "");
};

#endif
