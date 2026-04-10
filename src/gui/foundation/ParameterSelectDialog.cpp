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
 * Implements ParameterSelectDialog class. This class shows dialog window where a
 * user parameter can be selected.
 */
//------------------------------------------------------------------------------

#include "ParameterSelectDialog.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "Array.hpp"                    // for GetRowCount()
#include "MessageInterface.hpp"

//#define DEBUG_LOAD
//#define DEBUG_SAVE
//#define DEBUG_BUTTON
//#define DEBUG_OBJECT
//#define DEBUG_PROPERTY
//#define DEBUG_CHECKBOX
//#define DEBUG_COMBOBOX_CHANGE
//#define DEBUG_LISTBOX_SELECT
//#define DEBUG_LISTBOX_DOUBLE_CLICK
//#define DEBUG_MULTI_SELECT
//#define DEBUG_WHOLE_OBJECT
//#define DEBUG_PARAMETER
//#define DEBUG_CS
//#define DEBUG_ADD_ALL
//#define DEBUG_SHOW_ARRAY_INDEX
//#define DEBUG_PSD

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterSelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_CANCEL, ParameterSelectDialog::OnCancel)
   EVT_COMBOBOX(COMBOBOX_ID, ParameterSelectDialog::OnComboBoxChange)
   EVT_BUTTON(BUTTON_ID, ParameterSelectDialog::OnButtonClick)
   EVT_LISTBOX(LISTBOX_ID, ParameterSelectDialog::OnListBoxSelect)
   EVT_LISTBOX_DCLICK(LISTBOX_ID, ParameterSelectDialog::OnListBoxDoubleClick)
   EVT_CHECKBOX(CHECKBOX_ID, ParameterSelectDialog::OnCheckBoxChange)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// ParameterSelectDialog(wxWindow *parent, ...)
//------------------------------------------------------------------------------
/* Shows parameter selection dialog.
 *
 * @param *parent           parent window pointer
 * @param &objectTypeList   list of object types to show in the type ComboBox
 * @param showOption        one of GuiItemManager::ShowParamOption for object
 *                            properties (SHOW_PLOTTABLE)
 * @param showObjectOption  0 = do not allow any whole object
 *                          1 = allow any whole object
 *                          2 = allow only whole array and array element
 * @param allowMultiSelect  true if multiple selection is allowed [false]
 * @param allowString       true if selection of String is allowed [false]
 * @param allowSysParam     true if selection of system parameter is allowed [true]
 * @param allowVariable     true if selection of Varialbe is allowed [true]
 * @param allowArray        true if selection of whole Array is allowed [true]
 * @param allowArrayElement true if selection of Array Element is allowed [true]
 * @param &objectType       default object type to show ("Spacecraft")
 * @param createParam       true if to create non-existant system parameter [true]
 * @param skipDependency    true if skipping dependency object when creating Parameter name [false]
 * @param forStopCondition  true if this dialog is for stopping condition for propagation [false]
 */
//------------------------------------------------------------------------------
ParameterSelectDialog::ParameterSelectDialog
     (wxWindow *parent, const wxArrayString &objectTypeList, int showOption,
      int showWholeObjOption, bool allowMultiSelect, bool allowString,
      bool allowSysParam, bool allowVariable, bool allowArray, bool allowArrayElement,
      const wxString &objectType, bool createParam, bool showSettableOnly,
      bool skipDependency, bool forStopCondition)
   : GmatDialog(parent, -1, wxString(_T("ParameterSelectDialog")))
{
   mHasSelectionChanged = false;
   mIsParamSelected = false;
   mIsAddingMode = false;
   mCanClose = true;
   mUseUserParam = false;
   mObjectTypeList = objectTypeList;
   mShowOption = showOption;
   mShowObjectOption = showWholeObjOption;
   mAllowMultiSelect = allowMultiSelect;
   mAllowString = allowString;
   mAllowVariable = allowVariable;
   mAllowArray = allowArray;
   mAllowArrayElement = allowArrayElement;
   mAllowSysParam = allowSysParam;
   mCreateParam = createParam;
   mShowSettableOnly = showSettableOnly;
   mSkipDependency = skipDependency;
   mForStopCondition = forStopCondition;
   mObjectType = objectType;
   
   mLastCoordSysName     = "EarthMJ2000Eq";
   mPreviousCoordSysName = "EarthMJ2000Eq";

   // Set initial flag for allowing selecting whole object
   mAllowWholeObject = false;
//   // Set initial flag indicating last CoordinateSystem ComboBox has Fixed CS only
//   mLastCSHasFixedOnly = false;
//   // Set initial flag indicating last CoordinateSystem ComboBox has CS with CB Origin only
//   mLastCSHasCBOriginOnly = false;
   
   mNumRow = -1;
   mNumCol = -1;
   mLastHardwareSelection = -1;
   
   mParamNameArray.Clear();
   
   #ifdef DEBUG_PSD
   MessageInterface::ShowMessage
      ("ParameterSelectDialog() mShowOption=%d, mShowOjectOption=%d, "
       "mAllowMultiSelect=%d, mAllowString=%d, mAllowSysParam=%d, mAllowVariable=%d\n"
       "   mAllowArray=%d, mAllowArrayElement=%d, mObjectType=%s, mCreateParam=%d, "
       "mShowSettableOnly=%d, mSkipDependency=%d, mForStopCondition=%d\n", mShowOption,
       mShowObjectOption, mAllowMultiSelect, mAllowString, mAllowSysParam, mAllowVariable,
       mAllowArray, mAllowArrayElement, mObjectType.c_str(), mCreateParam, mShowSettableOnly,
       mSkipDependency, mForStopCondition);
   #endif
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~ParameterSelectDialog()
//------------------------------------------------------------------------------
ParameterSelectDialog::~ParameterSelectDialog()
{
   #ifdef DEBUG_PSD
   MessageInterface::ShowMessage
      ("ParameterSelectDialog::~ParameterSelectDialog() Unregister "
       "mObjectListBox:%p\n", mObjectListBox);
   #endif
   
   theGuiManager->UnregisterListBox(mObjectType, mObjectListBox);
   theGuiManager->UnregisterListBox("Hardware", mHardwareListBox);
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
   theGuiManager->UnregisterComboBox("ODEModel", mODEModelComboBox);
   theGuiManager->UnregisterComboBox("CelestialBody", mCentralBodyComboBox);
}


//------------------------------------------------------------------------------
// bool HasSelectionChanged()
//------------------------------------------------------------------------------
bool ParameterSelectDialog::HasSelectionChanged()
{
   return ParameterSelectDialog::mHasSelectionChanged;
}


//------------------------------------------------------------------------------
// bool IsParamSelected()
//------------------------------------------------------------------------------
bool ParameterSelectDialog::IsParamSelected()
{
   return ParameterSelectDialog::mIsParamSelected;
}


//------------------------------------------------------------------------------
// wxString GetParamName()
//------------------------------------------------------------------------------
wxString ParameterSelectDialog::GetParamName()
{
   return mParamName;
}


//------------------------------------------------------------------------------
// void SetObjectType(const wxString &objType)
//------------------------------------------------------------------------------
void ParameterSelectDialog::SetObjectType(const wxString &objType)
{
   mObjectType = objType;
   mObjectTypeComboBox->SetValue(mObjectType);
}


//------------------------------------------------------------------------------
// wxArrayString& GetParamNameArray()
//------------------------------------------------------------------------------
wxArrayString& ParameterSelectDialog::GetParamNameArray()
{
   return mParamNameArray;
}


//------------------------------------------------------------------------------
//void SetParamNameArray(const wxArrayString &paramNames)
//------------------------------------------------------------------------------
void ParameterSelectDialog::SetParamNameArray(const wxArrayString &paramNames)
{
   mParamNameArray = paramNames;
   
   #ifdef DEBUG_PSD
   MessageInterface::ShowMessage
      ("SetParamNameArray() param count=%d\n", mParamNameArray.GetCount());
   #endif
   
   // show selected parameter
   for (unsigned int i=0; i<mParamNameArray.GetCount(); i++)
   {
      mSelectedListBox->Append(mParamNameArray[i]);
      #ifdef DEBUG
      MessageInterface::ShowMessage("param=%s\n", mParamNameArray[i].c_str());
      #endif
   }
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterSelectDialog::Create()
{
   #ifdef DEBUG_PSD
   MessageInterface::ShowMessage("ParameterSelectDialog::Create() entered.\n");
   #endif
   
   // get the config object
   mConfig = wxConfigBase::Get();
   mConfig->SetPath(wxT("/Parameter Select"));
   
   //------------------------------------------------------
   // create parameter sizer
   //------------------------------------------------------
   
   mParameterSizer = theGuiManager->
      CreateParameterSizer(this, &mEntireObjectCheckBox, CHECKBOX_ID,
                           &mObjectTypeComboBox, COMBOBOX_ID,
                           &mObjectListBox, LISTBOX_ID,
                           &mHardwareStaticText, TEXT_ID,
                           &mHardwareListBox, LISTBOX_ID,
                           &mRowStaticText, TEXT_ID,
                           &mColStaticText, TEXT_ID,
                           &mRowTextCtrl, TEXTCTRL_ID,
                           &mColTextCtrl, TEXTCTRL_ID,
                           &mPropertyListBox, LISTBOX_ID,
                           &mCoordSysComboBox, COMBOBOX_ID,
                           &mCentralBodyComboBox, COMBOBOX_ID,
                           &mODEModelComboBox, COMBOBOX_ID,
                           &mCoordSysLabel, &mCoordSysSizer,
                           &mUpButton, BUTTON_ID,
                           &mDownButton, BUTTON_ID,
                           &mAddButton, BUTTON_ID,
                           &mRemoveButton, BUTTON_ID,
                           &mAddAllButton, BUTTON_ID,
                           &mRemoveAllButton, BUTTON_ID,
                           &mSelectedListBox, LISTBOX_ID,
                           mObjectTypeList, mShowOption,
                           mShowObjectOption, mShowSettableOnly,
                           mAllowMultiSelect, mAllowString,
                           mAllowSysParam, mAllowVariable,
                           mAllowArray, mAllowArrayElement,
                           mForStopCondition, mObjectType,
                           "Parameter Select");
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(mParameterSizer, 0, wxALIGN_CENTRE|wxALL, 5);

}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterSelectDialog::LoadData()
{   
   #ifdef DEBUG_LOAD
   MessageInterface::ShowMessage
      ("\nParameterSelectDialog::LoadData() entered, mShowOption=%d, mAllowSysParam=%d, "
       "mAllowMultiSelect=%d\n", mShowOption, mAllowSysParam, mAllowMultiSelect);
   #endif
   
   if (mShowOption == GuiItemManager::SHOW_WHOLE_OBJECT_ONLY)
      mAllowWholeObject = true;
   
   if (mShowOption != GuiItemManager::SHOW_WHOLE_OBJECT_ONLY)
   {
      if (mAllowSysParam)
      {
         mPropertyListBox->SetSelection(0);
         
         //if (mObjectType == "ImpulsiveBurn" || mAllowMultiSelect)
         if (mAllowMultiSelect ||
             mObjectType == "ImpulsiveBurn" || mObjectType == "FiniteBurn")
         {
            mCoordSysLabel->Hide();
            mCoordSysComboBox->SetValue("");
            mCentralBodyComboBox->SetValue("");
            mODEModelComboBox->SetValue("");
            mCoordSysComboBox->Hide();
            mCentralBodyComboBox->Hide();
            mODEModelComboBox->Hide();
         }
         else
         {
            mLastCoordSysName     = mCoordSysComboBox->GetString(0);
            mPreviousCoordSysName = mCoordSysComboBox->GetString(0);
            
            // Show coordinate system or central body
            ShowCoordSystem(false);
         }
      }
      else
		  ShowCoordSystem(false);
   }
   
   // Hide array element
   ShowArrayIndex(false);
   
   // Hide hardware and reset last hardware selection
   ShowAttachedHardware(false);
   mLastHardwareSelection = -1;
   
   if (mObjectType == "Spacecraft")
      ShowAttachedHardware(true);
   
   // Fire ListBoxSelect event to show object properties
   mObjectListBox->SetSelection(0);
   wxCommandEvent tempEvent;
   tempEvent.SetEventObject(mObjectListBox);
   OnListBoxSelect(tempEvent);
   
   #ifdef DEBUG_LOAD
   MessageInterface::ShowMessage
      ("ParameterSelectDialog::LoadData() exiting. mIsParamSelected=%d\n",
       mIsParamSelected);
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterSelectDialog::SaveData()
{
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage("ParameterSelectDialog::SaveData() entered.\n");
   #endif
   
   mCanClose = true;
   mParamNameArray.Clear();
   mIsParamSelected = false;
   
   if (mSelectedListBox->GetCount() > 0)
   {
      mIsParamSelected = true;
      mParamName = mSelectedListBox->GetString(0);
      
      for(unsigned int i=0; i<mSelectedListBox->GetCount(); i++)
      {
         #ifdef DEBUG_SAVE
         MessageInterface::ShowMessage
            ("   adding %s\n", mSelectedListBox->GetString(i).c_str());
         #endif
         
         mParamNameArray.Add(mSelectedListBox->GetString(i));
      }
   }
   
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage
      ("ParameterSelectDialog::SaveData() exiting. mIsParamSelected=%d\n",
       mIsParamSelected);
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ResetData()
{
   mIsParamSelected = false;
}


//------------------------------------------------------------------------------
// void OnCancel()
//------------------------------------------------------------------------------
/**
 * Resets selection changed flag to false.
 */
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnCancel(wxCommandEvent &event)
{
   mHasSelectionChanged = false;
   GmatDialog::OnCancel(event);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnButtonClick(wxCommandEvent& event)
{
   #ifdef DEBUG_BUTTON
   MessageInterface::ShowMessage("\nOnButtonClick() entered\n");
   #endif
   
   if (event.GetEventObject() == mUpButton)
   {
      int sel = mSelectedListBox->GetSelection();
      
      if (sel-1 >= 0)
      {
         wxString frontString = mSelectedListBox->GetString(sel-1);
         mSelectedListBox->SetString(sel-1, mSelectedListBox->GetStringSelection());
         mSelectedListBox->SetString(sel, frontString);
         mSelectedListBox->SetSelection(sel-1);
         mHasSelectionChanged = true;
      }
   }
   else if (event.GetEventObject() == mDownButton)
   {
      unsigned int sel = mSelectedListBox->GetSelection();
      
      if (sel+1 >= 1 && (sel+1) < mSelectedListBox->GetCount())
      {
         wxString rearString = mSelectedListBox->GetString(sel+1);
         mSelectedListBox->SetString(sel+1, mSelectedListBox->GetStringSelection());
         mSelectedListBox->SetString(sel, rearString);      
         mSelectedListBox->SetSelection(sel+1);
         mHasSelectionChanged = true;
      }
   }
   else if (event.GetEventObject() == mAddButton)
   {
      #ifdef DEBUG_BUTTON
      MessageInterface::ShowMessage
         ("   Adding parameter selection, mAllowMultiSelect=%d\n", mAllowMultiSelect);
      #endif
      
      if (mAllowMultiSelect)
      {
         // clear old selections
         mLastPropertySelections.Clear();
         
         // set adding mode to true
         mIsAddingMode = true;
         
         if (AddMultipleSelections())
            mHasSelectionChanged = true;
         
         // reset adding mode to false
         mIsAddingMode = false;
      }
      else
      {
         if (AddParameterSelection())
            mHasSelectionChanged = true;
      }
   }
   else if (event.GetEventObject() == mRemoveButton)
   {
      RemoveParameter();
      mHasSelectionChanged = true;
   }
   else if (event.GetEventObject() == mAddAllButton)
   {
      AddAll();
      mHasSelectionChanged = true;
   }
   else if (event.GetEventObject() == mRemoveAllButton)
   {
      mSelectedListBox->Clear();
      mHasSelectionChanged = true;
   }
   
   if (mHasSelectionChanged)
      EnableUpdate(true);

   #ifdef DEBUG_BUTTON
   MessageInterface::ShowMessage("OnButtonClick() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnListBoxSelect(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnListBoxSelect(wxCommandEvent& event)
{
   #ifdef DEBUG_LISTBOX_SELECT
   MessageInterface::ShowMessage("\nOnListBoxSelect() entered\n");
   #endif
   
   wxObject *obj = event.GetEventObject();
   
   if (obj == mObjectListBox)
   {
      if (mObjectListBox->IsEmpty())
         return;
      
      #ifdef DEBUG_LISTBOX_SELECT
      if (mAllowMultiSelect)
      {
         wxArrayInt selections;
         int count = mObjectListBox->GetSelections(selections);
         MessageInterface::ShowMessage
            ("   mObjectListBox %d items selected.\n", count);
      }
      #endif
      
      wxString objType = mObjectTypeComboBox->GetValue();
      wxString objName = GetObjectSelection();
      
      #ifdef DEBUG_LISTBOX_SELECT
      MessageInterface::ShowMessage
         ("   objType: <%s> and objName: <%s> selected\n", objType.c_str(),
          objName.c_str());
      #endif

      if (mShowOption != GuiItemManager::SHOW_WHOLE_OBJECT_ONLY)
      {
         if (objType == "Spacecraft")
         {
            // Build attached hardware list
            BuildAttachedHardware(objName);
            ShowObjectProperties();
         }
         else if (objType == "SpacePoint")
         {
            ShowObjectProperties();
         }
         else if (objType == "Array")
         {
            // Show array element if not showing whole array
            ShowArrayIndex(!mAllowWholeObject);
         }
      }
   }
   else if (obj == mHardwareListBox)
   {
      // If the same hardware clicked, deselect it
      if (mLastHardwareSelection == mHardwareListBox->GetSelection())
      {
         mHardwareListBox->Deselect(mLastHardwareSelection);
         mLastHardwareSelection = -1;
         ShowObjectProperties();
      }
      else
      {
         wxString objName = GetObjectSelection();
         wxString hwObjStr = mHardwareListBox->GetStringSelection();
         mLastHardwareSelection = mHardwareListBox->GetSelection();
         #ifdef DEBUG_LISTBOX_SELECT
         MessageInterface::ShowMessage
            ("   Item in hardware ListBox clicked, selected spacecraft='%s', "
             "hwObjStr='%s'\n", objName.c_str(), hwObjStr);
         #endif
         ShowHardwareProperties(objName, hwObjStr);
      }
   }
   else if (obj == mPropertyListBox)
   {
      #ifdef DEBUG_LISTBOX_SELECT
      if (mAllowMultiSelect)
      {
         wxArrayInt selections;
         int count = mPropertyListBox->GetSelections(selections);
         MessageInterface::ShowMessage
            ("   mPropertyListBox %d item(s) selected\n", count);
      }
      #endif
      
      // Show coordinate system or central body
      wxString objType = mObjectTypeComboBox->GetValue();
      //bool objIsBurn = (objType == "ImpulsiveBurn");
      bool objIsBurn = (objType == "ImpulsiveBurn" || objType == "FiniteBurn");
      ShowCoordSystem(objIsBurn);
   }
   
   #ifdef DEBUG_LISTBOX_SELECT
   MessageInterface::ShowMessage("OnListBoxSelect() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnListBoxDoubleClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnListBoxDoubleClick(wxCommandEvent& event)
{
   #ifdef DEBUG_LISTBOX_DOUBLE_CLICK
   MessageInterface::ShowMessage("OnListBoxDoubleClick() entered\n");
   #endif
   
   wxObject *obj = event.GetEventObject();
   
   if (obj == mObjectListBox)
   {
      #ifdef DEBUG_LISTBOX_DOUBLE_CLICK
      MessageInterface::ShowMessage("   mObjectListBox double clicked\n");
      #endif
      
      if (AddWholeObject())
         mHasSelectionChanged = true;
   }
   else if (obj == mPropertyListBox)
   {
      #ifdef DEBUG_LISTBOX_DOUBLE_CLICK
      MessageInterface::ShowMessage("   mPropertyListBox double clicked\n");
      #endif
      
      if (mAllowWholeObject)
      {
         DisplayWarning("Please uncheck the Select Entire Object check box\n"
                        "before adding the property to the list.");
         return;
      }
      
      if (AddParameterSelection())
         mHasSelectionChanged = true;
   }
   else if (obj == mSelectedListBox)
   {
      #ifdef DEBUG_LISTBOX_DOUBLE_CLICK
      MessageInterface::ShowMessage("   mSelectedListBox double clicked\n");
      #endif
      
      RemoveParameter();
      mHasSelectionChanged = true;
   }
   
   if (mHasSelectionChanged)
      EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnComboBoxChange(wxCommandEvent& event)
{
   #ifdef DEBUG_COMBOBOX_CHANGE
   MessageInterface::ShowMessage
      ("\nOnComboBoxChange() entered, mShowObjectOption=%d\n", mShowObjectOption);
   #endif
   
   wxObject *obj = event.GetEventObject();
   
   if (obj == mObjectTypeComboBox)
   {
      wxString objType = mObjectTypeComboBox->GetValue();
      
      #ifdef DEBUG_COMBOBOX_CHANGE
      MessageInterface::ShowMessage
         ("   ObjectTypeComboBox changed to %s\n", objType.c_str());
      #endif
      
      // Clear object ListBox
      mObjectListBox->Clear();
      
      // Hide array index
      ShowArrayIndex(false);
      
      // Hide hardware and reset last hardware selection
      ShowAttachedHardware(false);
      mLastHardwareSelection = -1;
      
      if (objType == "Spacecraft")
         ShowSpacecraft();
      else if (objType == "SpacePoint")
         ShowSpacePoints();
      else if (objType == "ImpulsiveBurn")
         ShowImpulsiveBurns();
      else if (objType == "FiniteBurn")
         ShowFiniteBurns();
      else if (objType == "Variable")
         ShowVariables();
      else if (objType == "Array")
         ShowArrays();
      else if (objType == "String")
         ShowStrings();
      else
         mPropertyListBox->Clear();
      
      // Refresh to show arrow buttons properly
      Refresh();
      
      // Clear last multiple selections
      mLastObjectSelections.Clear();
      mObjectListBox->SetSelection(0);
      
      #ifdef DEBUG_COMBOBOX_CHANGE
      wxArrayInt selections;
      int count = mObjectListBox->GetSelections(selections);
      MessageInterface::ShowMessage
         ("   mAllowMultiSelect=%d, mAllowWholeObject=%d, numSelected=%d\n",
          mAllowMultiSelect, mAllowWholeObject, count);
      wxString objName = mObjectListBox->GetStringSelection();
      MessageInterface::ShowMessage("   objName=<%s>\n", objName.c_str());
      #endif
      
      // Fire ObjectListBox select event if single selection
      if ((!mAllowWholeObject) && (obj != mCoordSysComboBox))
      {
         wxCommandEvent tempEvent;
         tempEvent.SetEventObject(mObjectListBox);
         OnListBoxSelect(tempEvent);
      }
   }
   else if(obj == mCoordSysComboBox)
   {
      if (mLastCoordSysName != "") mPreviousCoordSysName = mLastCoordSysName;
      mLastCoordSysName = mCoordSysComboBox->GetValue();

      #ifdef DEBUG_COMBOBOX_CHANGE
      MessageInterface::ShowMessage
         ("   CoordSysComboBox changed to %s\n", mLastCoordSysName.c_str());
      MessageInterface::ShowMessage
         ("   mPreviousCoordSysName changed to %s\n", mPreviousCoordSysName.c_str());
      #endif
   }
   
   #ifdef DEBUG_COMBOBOX_CHANGE
   MessageInterface::ShowMessage
      ("OnComboBoxChange() leaving, mShowObjectOption=%d\n", mShowObjectOption);
   #endif
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnCheckBoxChange(wxCommandEvent& event)
{
   #ifdef DEBUG_CHECKBOX
   MessageInterface::ShowMessage("\nOnCheckBoxChange() entered\n");
   #endif
   if (event.GetEventObject() == mEntireObjectCheckBox)
   {
      mAllowWholeObject = mEntireObjectCheckBox->IsChecked();
      #ifdef DEBUG_CHECKBOX
      MessageInterface::ShowMessage
         ("   mEntireObjectCheckBox IsChecked()=%d, mAllowWholeObject=%d\n",
          mEntireObjectCheckBox->IsChecked(), mAllowWholeObject);
      #endif
      
      wxString objType = mObjectTypeComboBox->GetValue();
      
      // if user can select whole object, set ListBox style to wxLB_EXTENDED
      if (mAllowWholeObject)
      {
         #ifdef DEBUG_CHECKBOX
         MessageInterface::ShowMessage("   Setting ObjectListBox to wxLB_EXTENDED\n");
         #endif
         mObjectListBox->SetWindowStyle(wxLB_EXTENDED);
         if (objType == "Array")
         {
            ShowArrayIndex(false);
         }
         else if (objType == "Spacecraft")
         {
            ShowAttachedHardware(false);
            ShowObjectProperties();
         }
         else if (objType == "SpacePoint")
         {
            ShowObjectProperties();
         }
      }
      else
      {
         #ifdef DEBUG_CHECKBOX
         MessageInterface::ShowMessage("   Setting ObjectListBox to wxLB_SINGLE\n");
         #endif
         mObjectListBox->SetWindowStyle(wxLB_SINGLE);
         if (mObjectTypeComboBox->GetValue() == "Array")
            ShowArrayIndex(true);
         else if ((objType == "Spacecraft")    || (objType == "SpacePoint") ||
                  (objType == "ImpulsiveBurn") || (objType == "FiniteBurn"))
         {
            ShowObjectProperties();
            if (objType == "Spacecraft")
               ShowAttachedHardware(true);
         }
      }
      
      // Refresh to new window style to take place immediately
      Refresh();
   }
   #ifdef DEBUG_CHECKBOX
   MessageInterface::ShowMessage("OnCheckBoxChange() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// bool AddWholeObject()
//------------------------------------------------------------------------------
/*
 * @return true if selected object was added to the list. It will add if
 * whole object is allowed or Variable or String.
 */
//------------------------------------------------------------------------------
bool ParameterSelectDialog::AddWholeObject()
{
   #ifdef DEBUG_WHOLE_OBJECT
   MessageInterface::ShowMessage
      ("AddWholeObject() entered, mEntireObjectCheckBox->IsShown()=%d, "
       "mAllowWholeObject=%d\n", mEntireObjectCheckBox->IsShown(), mAllowWholeObject);
   #endif
   
   wxString objType = mObjectTypeComboBox->GetValue();
   wxString objName = GetObjectSelection();
   
   #ifdef DEBUG_WHOLE_OBJECT
   MessageInterface::ShowMessage
      ("   objType='%s', objName='%s'\n", objType.c_str(), objName.c_str());
   #endif

   // check if type is String
   if (objType == "String")
   {
      if (mShowOption == GuiItemManager::SHOW_PLOTTABLE)
      {
         DisplayWarning("Selection of String object type is not allowed.");
         return false;
      }
      else
      {
         AddParameter(objName);
         return true;
      }
   }
   
   // check if whole object is allowed other than Variable or String
   if (mAllowWholeObject)
   {
      if ((mShowObjectOption == 1) || 
          (mShowObjectOption == 2 && objType == "Array"))
      {
         AddParameter(objName);
         return true;
      }
      else
      {
         DisplayWarning("Selection of entire object is not allowed.");
         return false;
      }
   }
   else
   {
      AddParameterSelection();
   }
   
   #ifdef DEBUG_WHOLE_OBJECT
   MessageInterface::ShowMessage("AddWholeObject() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool AddParameterSelection()
//------------------------------------------------------------------------------
/*
 * @return true if parameter added to selected list box
 */
//------------------------------------------------------------------------------
bool ParameterSelectDialog::AddParameterSelection()
{
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage
      ("AddParameterSelection() entered, mAllowMultiSelect=%d, mAllowWholeObject=%d\n",
       mAllowMultiSelect, mAllowWholeObject);
   #endif
   
   if (mAllowMultiSelect)
   {
      wxArrayInt objectSelects;
      wxArrayInt propertySelects;
      int objCount = mObjectListBox->GetSelections(objectSelects);
      #ifdef DEBUG_PARAMETER
      int propCount = mPropertyListBox->GetSelections(propertySelects);
      MessageInterface::ShowMessage
         ("   objCount=%d, propCount=%d\n", objCount, propCount);
      #endif
      if (objCount == 0)
      {
         DisplayWarning("Please select an object.");
         
         #ifdef DEBUG_PARAMETER
         MessageInterface::ShowMessage
            ("AddParameterSelection() nothing selected, returning false\n");
         #endif
         
         return false;
      }
   }
   
   // now compose parameter name by adding property to object name
   wxString newParam = FormParameterName();
   
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage
      ("   Now adding selected Parameter '%s'\n", newParam.c_str());
   #endif
   
   // if newParam is properly created
   if (newParam != "")
   {
      // Create a system paramete if it does not exist
      if (mAllowSysParam && mCreateParam)
      {
         Parameter *param = GetParameter(newParam);
         if (param == NULL)
         {
            DisplayWarning("Cannot create a Parameter" + newParam);
            return false;
         }
      }
      
      AddParameter(newParam);
      return true;
   }
   
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage("AddParameterSelection() returning false\n");
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// void AddParameter(const wxString &param)
//------------------------------------------------------------------------------
/**
 * Adds selected object or object property to selected ListBox.
 */
//------------------------------------------------------------------------------
void ParameterSelectDialog::AddParameter(const wxString &param)
{
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage("AddParameter() entered, param=<%s>\n", param.c_str());
   #endif
   
   // if the parameter string wasn't found in the selected list, insert it
   if (mSelectedListBox->FindString(param, true) == wxNOT_FOUND)
   {
      if (!mAllowMultiSelect)
         mSelectedListBox->Clear();
      
      mSelectedListBox->Append(param);
      mSelectedListBox->SetStringSelection(param);
   }
   else
   {
      mSelectedListBox->SetStringSelection(param);
      #ifdef DEBUG_PARAMETER
      DisplayWarning("\"" + param + "\"" + " is already selected");
      #endif
   }
   
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage("AddParameter() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// bool AddMultipleSelections()
//------------------------------------------------------------------------------
/**
 * Addes multiple selection of objects or object properties to selected ListBox.
 */
//------------------------------------------------------------------------------
bool ParameterSelectDialog::AddMultipleSelections()
{
   wxString objType = mObjectTypeComboBox->GetValue();
   
   #ifdef DEBUG_MULTI_SELECT
   MessageInterface::ShowMessage
      ("AddMultipleSelections() entered, mAllowWholeObject=%d, objType='%s'\n",
       mAllowWholeObject, objType);
   #endif
   
   wxArrayInt selections;
   int count = 0;
   
   // Add multiple selections if entire object is selected or object type
   // is Variable or String since these don't have additional fields
   if (mAllowWholeObject || objType == "Variable" || objType == "String")
   {
      count = mObjectListBox->GetSelections(selections);
      
      #ifdef DEBUG_MULTI_SELECT
      MessageInterface::ShowMessage("   Number of entire object selection=%d\n", count);
      #endif
      
      if (count > 0)
      {      
         for (int i=0; i<count; i++)
         {
            #ifdef DEBUG_MULTI_SELECT
            MessageInterface::ShowMessage("   setting selection %d\n", selections[i]);
            #endif
            
            // fire mObjectListBox select and double click event
            mObjectListBox->SetSelection(selections[i]);
            
            wxCommandEvent tempEvent;
            tempEvent.SetEventObject(mObjectListBox);
            OnListBoxSelect(tempEvent);
            OnListBoxDoubleClick(tempEvent);
            
            mObjectListBox->Deselect(selections[i]);
         }
         
         return true;
      }
   }
   
   // Not selecting whole object or not a Variable or String
   if (mPropertyListBox->IsEmpty())
   {
      #ifdef DEBUG_MULTI_SELECT
      MessageInterface::ShowMessage("   property list is empty\n");
      #endif
      
      if (objType == "Array")
      {
         count = mObjectListBox->GetSelections(selections);
         
         #ifdef DEBUG_MULTI_SELECT
         MessageInterface::ShowMessage("   number of array component selection=%d\n", count);
         #endif
         
         if (AddParameterSelection())
         {
            #ifdef DEBUG_MULTI_SELECT
            MessageInterface::ShowMessage("AddMultipleSelections() returning true\n");
            #endif
            return true;
         }
      }
   }
   else
   {
      #ifdef DEBUG_MULTI_SELECT
      MessageInterface::ShowMessage("   now continue with property selection\n");
      #endif
      
      // check if any object is selected
      wxArrayInt selections;
      if (mObjectListBox->GetSelections(selections) == 0)
      {
         DisplayWarning("Please select an object.");
         return false;
      }
      
      count = mPropertyListBox->GetSelections(selections);
      if (count == 0)
      {
         DisplayWarning("Please select a property.");
         return false;
      }
      
      //----------------------------------------------------
      // deselect all selections first
      //----------------------------------------------------
      for (int i=0; i<count; i++)
      {
         #ifdef DEBUG_MULTI_SELECT
         MessageInterface::ShowMessage("   deselecting %d\n", selections[i]);
         #endif
         mPropertyListBox->Deselect(selections[i]);
      }
      
      //----------------------------------------------------
      // select one property at a time
      //----------------------------------------------------
      for (int i=0; i<count; i++)
      {
         #ifdef DEBUG_MULTI_SELECT
         MessageInterface::ShowMessage("   setting selection %d\n", selections[i]);
         #endif
         
         mPropertyListBox->SetSelection(selections[i]);
         
         // fire mPropertyListBox select and double click event
         wxCommandEvent tempEvent;
         tempEvent.SetEventObject(mPropertyListBox);
         OnListBoxSelect(tempEvent);
         OnListBoxDoubleClick(tempEvent);
         
         mPropertyListBox->Deselect(selections[i]);
      }
      
      return true;
   }
   
   #ifdef DEBUG_MULTI_SELECT
   MessageInterface::ShowMessage("AddMultipleSelections() returning false\n");
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// void AddAll()
//------------------------------------------------------------------------------
void ParameterSelectDialog::AddAll()
{
   #ifdef DEBUG_ADD_ALL
   MessageInterface::ShowMessage
      ("AddAll() entered, mAllowMultiSelect=%d, mAllowWholeObject=%d\n",
       mAllowMultiSelect, mAllowWholeObject);
   #endif
   
   wxString objType = mObjectTypeComboBox->GetValue();
   
   if (objType == "Variable" || objType == "String" || mAllowWholeObject)
   {
      if (mAllowMultiSelect)
      {
         // go through all of the property listbox
         int count = mObjectListBox->GetCount();
         for (int i=0; i<count; i++)
            mObjectListBox->SetSelection(i);
         
         AddMultipleSelections();
      }
      else
      {
         // go through object listbox
         int count = mObjectListBox->GetCount();
         for (int i=0; i<count; i++)
         {
            mObjectListBox->SetSelection(i);
            
            // fire ObjectListBox double click event
            wxCommandEvent tempEvent;
            tempEvent.SetEventObject(mObjectListBox);
            OnListBoxDoubleClick(tempEvent);
         }
      }
   }
   else
   {
      if (mAllowMultiSelect)
      {
         // check if object is selected
         if (GetObjectSelection() == "")
         {
            DisplayWarning("Please select an object.");
         }
         else
         {
            // go through all of the property listbox
            int count = mPropertyListBox->GetCount();
            for (int i=0; i<count; i++)
            {
               mPropertyListBox->SetSelection(i);         
               AddMultipleSelections();
            }
         }
      }
      else
      {
         // go through all of the property listbox
         int count = mPropertyListBox->GetCount();
         for (int i=0; i<count; i++)
         {
            mPropertyListBox->SetSelection(i);
            
            // fire mPropertyListBox select and double click event
            wxCommandEvent tempEvent;
            tempEvent.SetEventObject(mPropertyListBox);
            OnListBoxSelect(tempEvent);
            OnListBoxDoubleClick(tempEvent);
         }
      }
   }
   
   #ifdef DEBUG_ADD_ALL
   MessageInterface::ShowMessage
      ("AddAll() leaving, mAllowMultiSelect=%d, mAllowWholeObject=%d\n",
       mAllowMultiSelect, mAllowWholeObject);
   #endif
}


//------------------------------------------------------------------------------
// void RemoveParameter()
//------------------------------------------------------------------------------
void ParameterSelectDialog::RemoveParameter()
{
   int sel = mSelectedListBox->GetSelection();
   mSelectedListBox->Delete(sel);
   
   if (sel-1 < 0)
      mSelectedListBox->SetSelection(0);
   else
      mSelectedListBox->SetSelection(sel-1);
}


//------------------------------------------------------------------------------
// void ShowSpacecraft()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowSpacecraft()
{
   // Show or hide entire object option
   mEntireObjectCheckBox->Hide();
   mAllowWholeObject = false;
   if (mShowObjectOption == 1)
   {
      mEntireObjectCheckBox->Show();
      mAllowWholeObject = mEntireObjectCheckBox->IsChecked();
   }
   
   #ifdef DEBUG_SHOW_SPACECRAFT
   MessageInterface::ShowMessage
      ("ShowSpacecraft() entered, mAllowWholeObject=%d\n", mAllowWholeObject);
   #endif
   
   // Show Spacecraft objects
   mObjectListBox->InsertItems(theGuiManager->GetSpacecraftList(), 0);
   mObjectListBox->SetToolTip(mConfig->Read(_T("SpacecraftListHint")));
   mObjectListBox->SetSelection(0);
   
   // Show hardware list if not showing entire object
   if (!mAllowWholeObject)
   {
      ShowAttachedHardware(true);
      ShowObjectProperties();
   }
   
   #ifdef DEBUG_SHOW_SPACECRAFT
   MessageInterface::ShowMessage
      ("ShowSpacecraft() leaving, mAllowWholeObject=%d\n", mAllowWholeObject);
   #endif
}


//------------------------------------------------------------------------------
// void ShowSpacePoints()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowSpacePoints()
{
   // Show or hide entire object option
   mEntireObjectCheckBox->Hide();
   mAllowWholeObject = false;
   if (mShowObjectOption == 1)
   {
      mEntireObjectCheckBox->Show();
      mAllowWholeObject = mEntireObjectCheckBox->IsChecked();
   }

   #ifdef DEBUG_SHOW_SPACEPOINTS
   MessageInterface::ShowMessage
      ("ShowSpacePoints() entered, mAllowWholeObject=%d\n", mAllowWholeObject);
   #endif

   // Show Spacecraft objects
   mObjectListBox->InsertItems(theGuiManager->GetSpacePointList(true), 0);
   mObjectListBox->SetToolTip(mConfig->Read(_T("SpacePointListHint")));
   mObjectListBox->SetSelection(0);

   // Show object properties if not showing entire object
   if (!mAllowWholeObject)
   {
      ShowObjectProperties();
   }

   #ifdef DEBUG_SHOW_SPACECRAFT
   MessageInterface::ShowMessage
      ("ShowSpacePoints() leaving, mAllowWholeObject=%d\n", mAllowWholeObject);
   #endif
}

//------------------------------------------------------------------------------
// void ShowImpulsiveBurns()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowImpulsiveBurns()
{
   #ifdef DEBUG_SHOW_BURNS
   MessageInterface::ShowMessage("ShowImpulsiveBurns() entered\n");
   #endif
   
   // Show or hide entire object option
   mEntireObjectCheckBox->Hide();
   mAllowWholeObject = false;
   if (mShowObjectOption == 1)
   {
      mEntireObjectCheckBox->Show();
      mAllowWholeObject = mEntireObjectCheckBox->IsChecked();
   }
   
   // Show ImpulsiveBurn objects
   mObjectListBox->InsertItems(theGuiManager->GetImpulsiveBurnList(), 0);
   mObjectListBox->SetToolTip(mConfig->Read(_T("ImpulsiveBurnListHint")));
   mObjectListBox->SetSelection(0);
   
   ShowObjectProperties();
   
   #ifdef DEBUG_SHOW_BURNS
   MessageInterface::ShowMessage("ShowImpulsiveBurns() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void ShowFiniteBurns()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowFiniteBurns()
{
   #ifdef DEBUG_SHOW_BURNS
   MessageInterface::ShowMessage("ShowFiniteBurns() entered\n");
   #endif
   
   // Show or hide entire object option
   mEntireObjectCheckBox->Hide();
   mAllowWholeObject = false;
   if (mShowObjectOption == 1)
   {
      mEntireObjectCheckBox->Show();
      mAllowWholeObject = mEntireObjectCheckBox->IsChecked();
   }
   
   // Show FiniteBurn objects
   mObjectListBox->InsertItems(theGuiManager->GetFiniteBurnList(), 0);
   mObjectListBox->SetToolTip(mConfig->Read(_T("FiniteBurnListHint")));
   mObjectListBox->SetSelection(0);
   
   ShowObjectProperties();
   
   #ifdef DEBUG_SHOW_BURNS
   MessageInterface::ShowMessage("ShowFiniteBurns() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void ShowArrays()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowArrays()
{
   // Show or hide entire object option
   mEntireObjectCheckBox->Hide();
   mAllowWholeObject = false;
   if (mShowObjectOption != 0 && mAllowArrayElement)
   {
      mEntireObjectCheckBox->Show();
      mAllowWholeObject = mEntireObjectCheckBox->IsChecked();
   }
   
   // Show Arrays
   mObjectListBox->InsertItems(theGuiManager->GetUserArrayList(), 0);
   mObjectListBox->SetToolTip(mConfig->Read(_T("ArrayListHint")));
   mObjectListBox->SetSelection(0);
   
   // There is no properties
   ClearProperties();
}


//------------------------------------------------------------------------------
// void ShowStrings()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowStrings()
{
   #ifdef DEBUG_COMBOBOX_CHANGE
   MessageInterface::ShowMessage
      ("   theGuiManager->GetNumUserString() = %d\n",
       theGuiManager->GetNumUserString());
   #endif
   
   // Hide entire object option
   mEntireObjectCheckBox->Hide();
   mAllowWholeObject = false;
   
   // Show Strings
   mObjectListBox->InsertItems(theGuiManager->GetUserStringList(), 0);
   mObjectListBox->SetToolTip(mConfig->Read(_T("StringListHint")));
   mObjectListBox->SetSelection(0);
      
   // There is no properties
   ClearProperties();
}


//------------------------------------------------------------------------------
// void ShowVariables()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowVariables()
{
   // Hide entire object option
   mEntireObjectCheckBox->Hide();
   mAllowWholeObject = false;
   
   // Show Variables
   mObjectListBox->InsertItems(theGuiManager->GetUserVariableList(), 0);
   mObjectListBox->SetToolTip(mConfig->Read(_T("VariableListHint")));
   mObjectListBox->SetSelection(0);
   
   // There is no properties
   ClearProperties();
}


//------------------------------------------------------------------------------
// void ShowArrayIndex(bool show)
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowArrayIndex(bool show)
{
   #ifdef DEBUG_SHOW_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("ShowArrayIndex() entered, show=%d, mAllowWholeObject=%d, mAllowArrayElement=%d\n",
       show, mAllowWholeObject, mAllowArrayElement);
   #endif
   
   if (!mAllowArrayElement)
   {
      #ifdef DEBUG_SHOW_ARRAY_INDEX
      MessageInterface::ShowMessage("ShowArrayIndex() leaving, since array element is not allowed\n");
      #endif
      return;
   }
   
   mParameterSizer->Show(mRowStaticText, show, true);
   mParameterSizer->Show(mColStaticText, show, true);
   mParameterSizer->Show(mRowTextCtrl, show, true);
   mParameterSizer->Show(mColTextCtrl, show, true);
   mParameterSizer->Layout();
   
   // If showing and not allowing whole object
   if (show && !mAllowWholeObject)
   {
      // Show row and column
      wxString objName = GetObjectSelection();
      if (objName == "")
      {
         #ifdef DEBUG_SHOW_ARRAY_INDEX
         MessageInterface::ShowMessage
            ("ShowArrayIndex() leaving, there is no array object, show=%d\n", show);
         #endif
         return;
      }
      
      Parameter *param = theGuiInterpreter->GetParameter(objName.c_str());
      Array *array = (Array*)param;
      
      // save ro and col so that we can do range check
      mNumRow = array->GetRowCount();
      mNumCol = array->GetColCount();
      
      #ifdef DEBUG_SHOW_ARRAY_INDEX
      MessageInterface::ShowMessage
         ("   item=<%s>, mNumRow=%d, mNumCol=%d\n", objName.c_str(), mNumRow, mNumCol);
      #endif
      
      wxString str;
      str.Printf("%d", mNumRow);
      mRowStaticText->SetLabel("Row [" + str + "]");
      str.Printf("%d", mNumCol);
      mColStaticText->SetLabel("Col [" + str + "]");
   }
   
   #ifdef DEBUG_SHOW_ARRAY_INDEX
   MessageInterface::ShowMessage("ShowArrayIndex() leaving, show=%d\n", show);
   #endif
}


//------------------------------------------------------------------------------
// void ShowAttachedHardware(bool show)
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowAttachedHardware(bool show)
{
   mParameterSizer->Show(mHardwareStaticText, show, true);
   mParameterSizer->Show(mHardwareListBox, show, true);
   mParameterSizer->Layout();
}


//------------------------------------------------------------------------------
// void BuildAttachedHardware(const wxString &scName)
//------------------------------------------------------------------------------
/**
 * Builds attached hardware list of the requested spacecraft.
 *
 * @param scName Name of the spacecraft
 */
//------------------------------------------------------------------------------
void ParameterSelectDialog::BuildAttachedHardware(const wxString &scName)
{
   if (!mAllowWholeObject)
   {
      mHardwareListBox->Clear();
      mHardwareListBox->InsertItems(theGuiManager->GetAttachedHardwareList(scName), 0);
      mHardwareListBox->SetToolTip(mConfig->Read(_T("AttachedHardwareListHint")));
      mParameterSizer->Layout();
   }
}


//------------------------------------------------------------------------------
// void ShowObjectProperties()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowObjectProperties()
{
   wxString objName = mObjectListBox->GetString(0);
   if (mAllowWholeObject)
   {
      mPropertyListBox->Clear();
   }
   else
   {
      // Set object property based on the first item
      mPropertyListBox->
         Set(theGuiManager->GetPropertyList(objName, "", mShowOption,
                                            mShowSettableOnly, mForStopCondition));
      
      mPropertyListBox->SetSelection(0);
      wxString objTypeSelect = mObjectTypeComboBox->GetValue();
      if ((objTypeSelect == "Spacecraft")    ||
          (objTypeSelect == "ImpulsiveBurn") ||
          (objTypeSelect == "FiniteBurn")    ||
          (objTypeSelect == "SpacePoint"))
      {
         bool showBlank = (objTypeSelect == "ImpulsiveBurn" ||
                           objTypeSelect == "FiniteBurn");
         ShowCoordSystem(showBlank);
      }
   }
}


//------------------------------------------------------------------------------
// void ShowHardwareProperties(const wxString &scName, const wxString &hwName)
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowHardwareProperties(const wxString &scName,
                                                   const wxString &hwName)
{
   #ifdef DEBUG_SHOW_HARDWARE
   MessageInterface::ShowMessage
      ("ShowHardwareProperties() entered, scName='%s', hwName='%s'\n",
       scName.WX_TO_C_STRING, hwName.WX_TO_C_STRING);
   #endif
   
   mPropertyListBox->Clear();   
   mPropertyListBox->InsertItems
      (theGuiManager->GetPropertyList(scName, hwName, mShowOption,
                                      mShowSettableOnly, mForStopCondition), 0);
   mParameterSizer->Layout();
   mPropertyListBox->SetSelection(0);
   
   #ifdef DEBUG_SHOW_HARDWARE
   MessageInterface::ShowMessage
      ("ShowHardwareProperties() leaving, scName='%s', hwName='%s'\n",
       scName.WX_TO_C_STRING, hwName.WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
// void ShowCoordSystem(bool showBlank = false)
//------------------------------------------------------------------------------
void ParameterSelectDialog::ShowCoordSystem(bool showBlank)
{
   #ifdef DEBUG_CS
   MessageInterface::ShowMessage("ShowCoordSystem() entered\n");
   #endif
   
   std::string property = GetPropertySelection().WX_TO_STD_STRING;
   
   if (property == "")
   {
      #ifdef DEBUG_CS
      MessageInterface::ShowMessage("ShowCoordSystem() property is empty, so just return\n");
      #endif
      mCoordSysSizer->Detach(mCentralBodyComboBox);
      mCoordSysSizer->Detach(mCoordSysComboBox);
      mCoordSysSizer->Detach(mODEModelComboBox);
      mCoordSysLabel->Hide();
      mCoordSysComboBox->Hide();
      mCentralBodyComboBox->Hide();
      mODEModelComboBox->Hide();
      mCoordSysSizer->Layout();
      mParameterSizer->Layout();
      return;
   }
   
   ParameterInfo *pInfo = ParameterInfo::Instance();
   GmatParam::DepObject depObj = pInfo->GetDepObjectType(property);
   
   #ifdef DEBUG_CS
   MessageInterface::ShowMessage("   depObj=%d\n", depObj);
   #endif
   
   if (depObj == GmatParam::COORD_SYS)
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Coordinate " GUI_ACCEL_KEY "System");
      
      if (showBlank)
      {
         // Add blank to the front of the list if it is not already on it
         int blankPos = mCoordSysComboBox->FindString("");
         if (blankPos == wxNOT_FOUND)
         {
            mCoordSysComboBox->Insert("",0);
            // Make sure to save the previous coordinate system as we don't
            // want blank to be used for other property types
            if (mLastCoordSysName != "")
               mPreviousCoordSysName = mLastCoordSysName;
            mLastCoordSysName = "";
         }
         mCoordSysComboBox->SetStringSelection(mLastCoordSysName);
      }
      else
      {
         // Parameter object has not been created at this point,
         // so query ParameterInfo if it requires BodyFixed CS only
         bool reqFixedCS    = pInfo->RequiresBodyFixedCS(property);
         bool reqCBCSOrigin = pInfo->RequiresCelestialBodyCSOrigin(property);
         #ifdef DEBUG_CS
//         MessageInterface::ShowMessage
//            ("   reqFixedCS=%d, reqCBCSOrigin=%d, mLastCSHasFixedOnly=%d, mLastCSHasCBOriginOnly = %d\n   mPreviousCoordSysName='%s', "
//             "mLastCoordSysName='%s\n", reqFixedCS, reqCBCSOrigin, mLastCSHasFixedOnly,  mLastCSHasCBOriginOnly, mPreviousCoordSysName.c_str(),
//             mLastCoordSysName.c_str());
         MessageInterface::ShowMessage
            ("   reqFixedCS=%d, reqCBCSOrigin=%d\n   mPreviousCoordSysName='%s', "
             "mLastCoordSysName='%s\n", reqFixedCS, reqCBCSOrigin, mPreviousCoordSysName.c_str(),
             mLastCoordSysName.c_str());
         #endif

         std::string ofAxesType = "";
         if (reqFixedCS) ofAxesType = "BodyFixedAxes";

         // Get the appropriate list of coordinate systems
         mCoordSysComboBox->Clear();
         mCoordSysComboBox->Append(theGuiManager->GetCoordSystemWithAxesOf(ofAxesType, reqCBCSOrigin));
         // If last coordinate system name used is not in the new list, select first one
         if (mCoordSysComboBox->FindString(mLastCoordSysName, true) == wxNOT_FOUND)
         {
            mCoordSysComboBox->SetSelection(0);
            mLastCoordSysName = mCoordSysComboBox->GetValue();
         }
         // WCS 2014.03.31 added check for reqCBCSOrigin and so modified code to
         // rebuild the list each time
//         if (reqFixedCS)
//         {
//            if (!mLastCSHasFixedOnly)
//            {
//               // Planetodetic Parameters should only show BodyFixed coordinate system
//               // So get the list from the GuiItemManager
//               mCoordSysComboBox->Clear();
//               mCoordSysComboBox->Append(theGuiManager->GetCoordSystemWithAxesOf("BodyFixedAxes"));
//               // If last coordinate system name used is not in the new list, select first one
//               if (mCoordSysComboBox->FindString(mLastCoordSysName, true) == wxNOT_FOUND)
//               {
//                  mCoordSysComboBox->SetSelection(0);
//                  mLastCoordSysName = mCoordSysComboBox->GetValue();
//               }
//            }
//            mLastCSHasFixedOnly = true;
//         }
//         else
//         {
//            if (mLastCSHasFixedOnly)
//            {
//               // Show all coordinate systems
//               mCoordSysComboBox->Clear();
//               mCoordSysComboBox->Append(theGuiManager->GetCoordSystemWithAxesOf(""));
//            }
//            mLastCSHasFixedOnly = false;
//         }

         int blankPos = mCoordSysComboBox->FindString("");
         if (blankPos != wxNOT_FOUND)  mCoordSysComboBox->Delete((unsigned int) blankPos);
         if (mLastCoordSysName == "")
            mCoordSysComboBox->SetStringSelection(mPreviousCoordSysName);
         else
            mCoordSysComboBox->SetStringSelection(mLastCoordSysName);
      }
      
      mCoordSysSizer->Detach(mCoordSysComboBox);
      mCoordSysSizer->Detach(mCentralBodyComboBox);
      mCoordSysSizer->Detach(mODEModelComboBox);
      mCoordSysSizer->Add(mCoordSysComboBox);
      mCoordSysComboBox->Show();
      mCentralBodyComboBox->Hide();
      mODEModelComboBox->Hide();
      mCoordSysSizer->Layout();
      mParameterSizer->Layout();
   }
   else if (depObj == GmatParam::ORIGIN)
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("Central " GUI_ACCEL_KEY "Body");
      
      // I had to remove mCoordSysComboBox first and then mCentralBodyComboBox,
      // otherwise, mCentralBodyComboBox shows too far to right
      mCoordSysSizer->Detach(mCoordSysComboBox);
      mCoordSysSizer->Detach(mCentralBodyComboBox);
      mCoordSysSizer->Detach(mODEModelComboBox);
      mCoordSysSizer->Add(mCentralBodyComboBox);
      mCentralBodyComboBox->Show();
      mCoordSysComboBox->Hide();
      mODEModelComboBox->Hide();
      mCoordSysSizer->Layout();
      mParameterSizer->Layout();
   }
   else if (depObj == GmatParam::ODE_MODEL)
   {
      mCoordSysLabel->Show();
      mCoordSysLabel->SetLabel("ODE" GUI_ACCEL_KEY "Model");
      
      mCoordSysSizer->Detach(mCoordSysComboBox);
      mCoordSysSizer->Detach(mCentralBodyComboBox);
      mCoordSysSizer->Detach(mODEModelComboBox);
      mCoordSysSizer->Add(mODEModelComboBox);
      mODEModelComboBox->Show();
      mCentralBodyComboBox->Hide();
      mCoordSysComboBox->Hide();
      mCoordSysSizer->Layout();
      mParameterSizer->Layout();
   }
   else
   {
      mCoordSysSizer->Detach(mCentralBodyComboBox);
      mCoordSysSizer->Detach(mCoordSysComboBox);
      mCoordSysSizer->Detach(mODEModelComboBox);
      mCoordSysLabel->Hide();
      mCoordSysComboBox->Hide();
      mCentralBodyComboBox->Hide();
      mODEModelComboBox->Hide();
      mCoordSysSizer->Layout();
      mParameterSizer->Layout();
   }
   
   #ifdef DEBUG_CS
   MessageInterface::ShowMessage("ShowCoordSystem() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void ClearProperties()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ClearProperties()
{
   mPropertyListBox->Clear();
   
   mCoordSysLabel->Hide();
   mCoordSysComboBox->Hide();
   mODEModelComboBox->Hide();
   mCentralBodyComboBox->Hide();
   mParameterSizer->Layout();
}


//------------------------------------------------------------------------------
// void DeselectAllObjects()
//------------------------------------------------------------------------------
void ParameterSelectDialog::DeselectAllObjects()
{
   wxArrayInt selections;
   int count = mObjectListBox->GetSelections(selections);
   
   for (int i=0; i<count; i++)
      mObjectListBox->Deselect(selections[i]);

   mLastObjectSelections.Clear();

   #ifdef DEBUG_DESELECT_ALL
   wxString objName = GetObjectSelection();
   MessageInterface::ShowMessage("DeselectAllObjects() objName=<%s>\n", objName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void DeselectObjects(wxArrayInt &newSelects, wxArrayInt &oldSelects)
//------------------------------------------------------------------------------
void ParameterSelectDialog::DeselectObjects(wxArrayInt &newSelects,
                                            wxArrayInt &oldSelects)
{
   #ifdef DEBUG_MULTI_SELECT
   MessageInterface::ShowMessage
      ("DeselectObjects() entered, count of newSelects=%d, oldSelects=%d\n",
       newSelects.GetCount(), oldSelects.GetCount());
   #endif
   
   if (newSelects.GetCount() == 1)
   {
      #ifdef DEBUG_MULTI_SELECT
      MessageInterface::ShowMessage
         ("DeselectObjects() only one selection, so just return\n");
      #endif
      
      return;
   }
   
   for (unsigned int i=0; i<oldSelects.GetCount(); i++)
   {
      #ifdef DEBUG_MULTI_SELECT
      MessageInterface::ShowMessage("   oldSelects[%d]=%d\n", i, oldSelects[i]);
      #endif
      
      for (unsigned int j=0; j<newSelects.GetCount(); j++)
      {
         #ifdef DEBUG_MULTI_SELECT
         MessageInterface::ShowMessage("   newSelects[%d]=%d\n", j, newSelects[j]);
         #endif
         
         if (oldSelects[i] == newSelects[j])
            mObjectListBox->Deselect(oldSelects[i]);
      }
   }
   
   // update selections
   mObjectListBox->GetSelections(newSelects);
   
   #ifdef DEBUG_MULTI_SELECT
   MessageInterface::ShowMessage
      ("DeselectObjects() exiting, count of newSelects=%d, oldSelects=%d\n",
       newSelects.GetCount(), oldSelects.GetCount());
   #endif
}


//------------------------------------------------------------------------------
// int GetLastPropertySelection()
//------------------------------------------------------------------------------
int ParameterSelectDialog::GetLastPropertySelection()
{
   wxArrayInt selections;
   int newCount = mPropertyListBox->GetSelections(selections);
   int lastSelect = -1;
   
   #ifdef DEBUG_PROPERTY
   MessageInterface::ShowMessage
      ("GetLastPropertySelection() newCount=%d, oldCount=%d\n", newCount,
       mLastPropertySelections.GetCount());
   #endif
   
   if (newCount == 1)
   {
      lastSelect = selections[0];
   }
   else
   {
      for (int i=0; i<newCount; i++)
      {
         if (mLastPropertySelections.Index(selections[i]) == wxNOT_FOUND)
         {
            lastSelect = selections[i];
            break;
         }
      }
   }
   
   mLastPropertySelections = selections;
   
   #ifdef DEBUG_PROPERTY
   MessageInterface::ShowMessage
      ("GetLastPropertySelection() return %d\n", lastSelect);
   #endif
   
   return lastSelect;
}


//------------------------------------------------------------------------------
// wxString GetObjectSelection()
//------------------------------------------------------------------------------
/*
 * Returns selected string of object ListBox.
 * If multiple selection is allowed, it returns first string selection.
 *
 * For Array, multiple selection is not allowed if entire object is not checked, 
 * since it has to show row and column.
 */
//------------------------------------------------------------------------------
wxString ParameterSelectDialog::GetObjectSelection()
{
   #ifdef DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("GetObjectSelection() entered, mAllowMultiSelect=%d\n", mAllowMultiSelect);
   #endif
   
   wxString objName;
   if (mAllowMultiSelect)
   {
      wxArrayInt selections;
      int newCount = mObjectListBox->GetSelections(selections);
      int oldCount = mLastObjectSelections.GetCount();
      
      #ifdef DEBUG_PSDIALOG_OBJECT
      MessageInterface::ShowMessage
         ("   oldCount=%d, newCount=%d\n", oldCount, newCount);
      #endif
      
      wxString objectType = mObjectTypeComboBox->GetValue();
      
      // check if only one selections is allowed, if so deselect one one
      if (objectType == "Array" || objectType == "Spacecraft" ||
          objectType == "ImpulsiveBurn" || objectType == "SpacePoint")
      {
         // Allow only one selection if entire object selection is not allowed
         if (oldCount > 0 && mEntireObjectCheckBox->GetValue() == false)
            DeselectObjects(selections, mLastObjectSelections);
         
         #ifdef DEBUG_PSDIALOG_OBJECT
         MessageInterface::ShowMessage
            ("   selected count=%d\n", selections.GetCount());
         #endif
         
         if (newCount > 0)
            objName = mObjectListBox->GetString(selections[0]);
         
         mLastObjectSelections = selections;
      }
      else
      {
         if (newCount > 0)
            objName = mObjectListBox->GetString(selections[0]);
      }
   }
   else
   {
      objName = mObjectListBox->GetStringSelection();
   }
   
   #ifdef DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("GetObjectSelection() returning %s\n", objName.c_str());
   #endif
   
   return objName;
}


//------------------------------------------------------------------------------
// wxString GetPropertySelection()
//------------------------------------------------------------------------------
/*
 * return selected string of property ListBox.
 * If multiple selection is allowed, it returns last string selection in selection
 * mode and returns first string selections in adding mode.
 */
//------------------------------------------------------------------------------
wxString ParameterSelectDialog::GetPropertySelection()
{
   #ifdef DEBUG_PROPERTY
   MessageInterface::ShowMessage
      ("GetPropertySelection() entered, mIsAddingMode=%d\n", mIsAddingMode);
   #endif
   
   wxString property;
   if (mAllowMultiSelect)
   {
      if (mIsAddingMode)
      {
         wxArrayInt selections;
         int count = mPropertyListBox->GetSelections(selections);
         
         if (count > 0)
            property = mPropertyListBox->GetString(selections[0]);
      }
      else
      {
         int lastSelect = GetLastPropertySelection();
         
         if (lastSelect != -1)
            property = mPropertyListBox->GetString(lastSelect);
      }
   }
   else
   {
      property = mPropertyListBox->GetStringSelection();
   }
   
   #ifdef DEBUG_PROPERTY
   MessageInterface::ShowMessage("GetPropertySelection() returning %s\n",
                                 property.c_str());
   #endif
   
   return property;
}


//------------------------------------------------------------------------------
// wxString FormArrayElement()
//------------------------------------------------------------------------------
/**
 * Forms array element by adding array index.
 */
//------------------------------------------------------------------------------
wxString ParameterSelectDialog::FormArrayElement()
{
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage
      ("FormArrayElement() entered, mAllowArrayElement=%d\n", mAllowArrayElement);
   #endif
   
   if (!mAllowArrayElement)
   {
      wxString arrayStr = GetObjectSelection();
      #ifdef DEBUG_PARAMETER
      MessageInterface::ShowMessage
         ("FormArrayElement() returning '%s', since array element is not allowd\n",
          arrayStr.c_str());
      #endif
      return arrayStr;
   }
   
   wxString rowStr = mRowTextCtrl->GetValue();
   wxString colStr = mColTextCtrl->GetValue();
   rowStr = rowStr.Strip(wxString::both);
   colStr = colStr.Strip(wxString::both);
   
   Integer row = -1;
   Integer col = -1;
   bool valid = false;
   
   // check for valid integer value first
   valid = CheckInteger(row, rowStr.c_str(), "Row", "Integer >= 1 and =< [Dimension]");
   valid = valid && CheckInteger(col, colStr.c_str(), "Col", "Integer >= 1 and =< [Dimension]");
   
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage("   valid=%d, row=%d, col=%d\n", valid, row, col);
   MessageInterface::ShowMessage("   mNumRow=%d, mNumCol=%d\n", mNumRow, mNumCol);
   #endif
   
   if (!valid)
      return "";
   
   valid = true;
   
   // do row range checking
   if (row < 1 || row > mNumRow)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Row index \"%s\" is out of range.\n"
          "Valid range is between 1 and %d\n", rowStr.WX_TO_C_STRING, mNumRow);
      valid = false;
   }
   
   // do column range checking
   if (col < 1 || col > mNumCol)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Column index \"%s\" is out of range.\n"
          "Valid range is between 1 and %d\n", colStr.WX_TO_C_STRING, mNumCol);
      valid = false;
   }
   
   if (!valid)
      return "";
   
   wxString arrayStr = GetObjectSelection();
   wxString arrayElem = arrayStr + "(" + rowStr + "," + colStr + ")";
   
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage("FormArrayElement() returning <%s>\n", arrayElem.c_str());
   #endif
   
   return arrayElem;
}


//------------------------------------------------------------------------------
// wxString FormParameterName()
//------------------------------------------------------------------------------
/**
 * Forms object name or Parameter name from combination of ListBox and
 * CoordinateSystem/Origin ComboBox.
 */
//------------------------------------------------------------------------------
wxString ParameterSelectDialog::FormParameterName()
{
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage("FormParameterName() entered\n");
   #endif
   
   // make sure object is selected
   if (GetObjectSelection() == "")
   {
      DisplayWarning("Please select an object.");
      return "";
   }
   
   wxString typeName = mObjectTypeComboBox->GetValue();
   wxString objectName = GetObjectSelection();
   
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage
      ("   type=<%s>, object = <%s>\n", typeName.c_str(), objectName.c_str());
   #endif
   
   // return whole object or Variable or String
   if (mAllowWholeObject || typeName == "Variable" || typeName == "String")
      return objectName;
   else if (typeName == "Array")
      return FormArrayElement();

   // now check for property name
   wxString paramName;
   wxString depObjName;
   wxString hwName;
   wxString propertyName = GetPropertySelection();
   
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage("   propertyName=<%s>\n", propertyName.c_str());
   #endif
   
   if (propertyName == "")
   {
      DisplayWarning("Please select a property.");
      return "";
   }
   
   // now compose object.dep.property
   if (mCoordSysComboBox->IsShown())
      depObjName = mCoordSysComboBox->GetValue();
   else if (mCentralBodyComboBox->IsShown())
      depObjName = mCentralBodyComboBox->GetValue();
   else if (mODEModelComboBox->IsShown())
      depObjName = mODEModelComboBox->GetValue();
   
   if (mHardwareListBox->IsShown())
      hwName = mHardwareListBox->GetStringSelection();
   
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage
      ("   depObjName=<%s>, hwName=<%s>, mSkipDependency=%d\n", depObjName.c_str(),
       hwName.c_str(), mSkipDependency);
   #endif

   // If dependency is blank or skipping dependency
   if (depObjName == "" || mSkipDependency)
   {
      if (hwName == "")
         paramName = objectName + "." + propertyName;
      else
         paramName = objectName + "." + hwName + "." + propertyName;
   }
   else
      paramName = objectName + "." + depObjName + "." + propertyName;
   
   #ifdef DEBUG_PARAMETER
   MessageInterface::ShowMessage
      ("FormParameterName() returning paramName=%s\n", paramName.c_str());
   #endif
   
   return paramName;
}


//------------------------------------------------------------------------------
// Parameter* GetParameter(const wxString &name)
//------------------------------------------------------------------------------
/*
 * @return existing parameter pointer, return newly created parameter pointer
 *         if it does not exist.
 */
//------------------------------------------------------------------------------
Parameter* ParameterSelectDialog::GetParameter(const wxString &name)
{
   Parameter *param = 
      theGuiInterpreter->GetParameter(std::string(name.c_str()));
   
   // create a parameter if it does not exist
   if (param == NULL)
   {
      #ifdef DEBUG_PARAMETER
      MessageInterface::ShowMessage
         ("ParameterSelectDialog::GetParameter() Creating parameter:<%s>\n",
          name.c_str());
      #endif
      
      std::string paramName(name.c_str());
      std::string objTypeName(mObjectTypeComboBox->GetValue().c_str());
      std::string objName(GetObjectSelection().c_str());
      std::string propName(GetPropertySelection().c_str());
      std::string depObjName = "";
      
      if (mCoordSysComboBox->IsShown())
         depObjName = std::string(mCoordSysComboBox->GetValue().c_str());
      else if (mCentralBodyComboBox->IsShown())
         depObjName = std::string(mCentralBodyComboBox->GetValue().c_str());
      else if (mODEModelComboBox->IsShown())
         depObjName = std::string(mODEModelComboBox->GetValue().c_str());
      
      param = theGuiInterpreter->CreateParameter(propName, paramName);
      
      if (objTypeName == "Spacecraft")
         param->SetRefObjectName(Gmat::SPACECRAFT, objName);
      else if (objTypeName == "ImpulsiveBurn")
         param->SetRefObjectName(Gmat::IMPULSIVE_BURN, objName);
      else if (objTypeName == "FiniteBurn")
         param->SetRefObjectName(Gmat::FINITE_BURN, objName);
      else if (objTypeName == "SpacePoint")
         param->SetRefObjectName(Gmat::SPACE_POINT, objName);
      else
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "*** WARNING *** %s is not a valid object for property %s\n"
             "There will be no report generated for this parameter.\n", objTypeName.c_str(),
             propName.c_str());
      
      if (depObjName != "")
      {
         param->SetStringParameter("DepObject", depObjName);
      
         if (param->IsCoordSysDependent())
            param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, depObjName);
      }
   }
   
   return param;
}


//------------------------------------------------------------------------------
// void DisplayWarning(const wxString &arg1, const wxString &arg2)
//------------------------------------------------------------------------------
void ParameterSelectDialog::DisplayWarning(const wxString &arg1, const wxString &arg2)
{
   //wxLogMessage(msg);
   if (arg2 == "")
      MessageInterface::PopupMessage(Gmat::WARNING_, arg1.c_str());
   else
      MessageInterface::PopupMessage(Gmat::WARNING_, arg1.WX_TO_C_STRING, arg2.WX_TO_C_STRING);
}

