//$Id$
//------------------------------------------------------------------------------
//                           EphemerisFilePanel
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
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen
// Created: 2010/02/02
/**
 * This class is a generic setup panel used by objects derived from GmatBase
 */
//------------------------------------------------------------------------------

#ifndef EphemerisFilePanel_hpp
#define EphemerisFilePanel_hpp

#include "GmatPanel.hpp"

/**
 * Generic configuration panel for GmatBase derived objects
 * 
 * This class defines a generic object configuration panel that is used when a
 * custom panel has not been coded.  It provides access to all of the object's
 * writable parameters using text controls and comboboxes.
 */
class EphemerisFilePanel : public GmatPanel
{
public:
   EphemerisFilePanel(wxWindow *parent, const wxString &name);
   virtual ~EphemerisFilePanel();
   
protected:

   GmatBase *clonedObj;
   
   virtual void         Create();
   virtual void         LoadData();
   virtual void         SaveData();
   wxControl *          BuildControl(wxWindow *parent, Integer index);
   void                 LoadControl(const std::string &label);
   void                 SaveControl(const std::string &label);
   
   // Text control event method
   void OnTextUpdate(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   void OnComboBoxChange(wxCommandEvent& event);
   void OnComboBoxTextChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);
   void OnBrowse(wxCommandEvent& event);
   void OnCheck(wxCommandEvent& event);
   
   void ShowCoordSystems(const wxString &fileType);
   void ShowCode500Items(const wxString &fileType);
   void ShowInterpolatorAndStepSize(const wxString &fileType);
   
   wxComboBox*     spacecraftComboBox;
   // Only allowed StateType is Cartesian for the 2010 release, per bug 2219
   //wxComboBox*     stateTypeComboBox;
   wxCheckBox*     writeEphemerisCheckBox;
   wxComboBox*     fileFormatComboBox;
   wxTextCtrl*     fileNameTextCtrl;
   wxBitmapButton* browseButton;
   wxComboBox*     interpolatorComboBox;
   wxTextCtrl*     interpolationOrderTextCtrl;
   wxComboBox*     outputFormatComboBox;
   wxComboBox*     epochFormatComboBox;
   wxComboBox*     initialEpochComboBox;
   wxComboBox*     finalEpochComboBox;
   wxCheckBox*     eventBoundariesCheckBox;
   
   wxComboBox*     allCoordSystemComboBox;
   wxStaticText*   allCoordSystemStaticText;
   wxComboBox*     code500ComboBox;
   wxStaticText*   code500StaticText;
   wxComboBox*     onlyMj2000EqComboBox;
   wxStaticText*   onlyMJ2000EqStaticText;
   
   wxStaticText*   allStepSizeStaticText;
   wxComboBox*     allStepSizeComboBox;
   wxStaticText*   allStepSizeUnit;
   wxStaticText*   numericStepSizeStaticText;
   wxTextCtrl*     numericStepSizeTextCtrl;
   wxStaticText*   numericStepSizeUnit;
   
   // STK .e specific widgets
   wxStaticText*   distanceUnitStaticText;
   wxComboBox*     distanceUnitComboBox;

   wxStaticBoxSizer *fileSettingsStaticBoxSizer;
   wxFlexGridSizer *grid1; //LOJ: Made this member data so that CS with only MJ2000Eq axis can be shown or hidden
   wxFlexGridSizer *grid2; //LOJ: Made this member data so that OutputFormat can be shown or hidden
   
   // ComboBox saved value
   wxString        fileFormat;
   wxString        outputFormat;
   wxString        previousEpochFormat;
   wxString        distanceUnit;
   
   wxFileDialog *fileDialog;
   
   /// Managed wxComboBox map used by GuiItemManager
   std::multimap<wxString, wxComboBox*>  managedComboBoxMap;
   
   /// IDs used for event management
   enum
   {
      ID_TEXT = 55000,
      ID_TEXTCTRL,
      ID_COMBOBOX,
      ID_CHECKBOX,
      ID_BUTTON_BROWSE
   };
};

#endif /* EphemerisFilePanel_hpp */
