//$Id$
//------------------------------------------------------------------------------
//                              GroundTrackPlotPanel
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
// Created: 2013/09/17
/**
 * Declares GroundTrackPlotPanel class. This class allows user to setup ground
 * track plot.
 */
//------------------------------------------------------------------------------
#ifndef GroundTrackPlotPanel_hpp
#define GroundTrackPlotPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GroundTrackPlot.hpp"

#ifdef __USE_COLOR_FROM_SUBSCRIBER__
#include <wx/clrpicker.h>           // for wxColorPickerCtrl
#include "RgbColor.hpp"
#include "ColorTypes.hpp"
#endif

class GroundTrackPlotPanel: public GmatPanel
{
public:
   GroundTrackPlotPanel(wxWindow *parent, const wxString &subscriberName);
   ~GroundTrackPlotPanel();
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(UnsignedInt type,
                                  const wxString &oldName,
                                  const wxString &newName);
protected:
   GroundTrackPlot *mGroundTrackPlot;
   
   bool mHasIntegerDataChanged;
   bool mHasRealDataChanged;
   bool mHasDataOptionChanged;
   bool mHasObjectListChanged;
   bool mHasCentralBodyChanged;
   bool mHasTextureMapChanged;

   wxString mCentralBody;
   wxString mTextureFile;
   
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   bool mHasOrbitColorChanged;
   bool mHasTargetColorChanged;
   std::map<std::string, RgbColor> mOrbitColorMap;
   std::map<std::string, RgbColor> mTargetColorMap;
   wxColour mOrbitColor;
   wxColour mTargetColor;
   wxColourPickerCtrl *mOrbitColorCtrl;
   wxColourPickerCtrl *mTargetColorCtrl;
   #endif

   
   wxTextCtrl *mDataCollectFreqTextCtrl;
   wxTextCtrl *mUpdatePlotFreqTextCtrl;
   wxTextCtrl *mMaxPlottedDataPointsTextCtrl;
   wxTextCtrl *mNumPointsToRedrawTextCtrl;
   wxTextCtrl *mTextureMapTextCtrl;
   
   wxButton *mTextureMapBrowseButton;
   
   wxCheckBox *mShowPlotCheckBox;
   
   wxCheckListBox *mObjectCheckListBox;
   
   wxComboBox *mCentralBodyComboBox;
   wxComboBox *mSolverIterComboBox;
   
   // for initializing data
   void InitializeData();
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handler
   void OnBrowseButton(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnCheckListBoxChange(wxCommandEvent& event);
   void OnCheckListBoxSelect(wxCommandEvent& event);
   
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   void OnColorPickerChange(wxColourPickerEvent& event);
   #endif
   
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXTCTRL = 93000,
      ID_COMBOBOX,
      ID_CHECKLISTBOX,
      ID_CHECKBOX,
      ID_BROWSE_BUTTON,
      #ifdef __USE_COLOR_FROM_SUBSCRIBER__
      ID_COLOR_CTRL,
      #endif
   };
   
private:

   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   void ShowSpacePointColor(const wxString &name,
                            UnsignedInt color = GmatColor::RED);
   void SaveObjectColors(const wxString &which,
                         std::map<std::string, RgbColor> &colorMap);
   #endif
};
#endif


