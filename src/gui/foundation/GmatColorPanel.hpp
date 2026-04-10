//$Id$
//------------------------------------------------------------------------------
//                            GmatColorPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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
// number NNG04CC06P.
//
//
// Author: Linda Jun
// Created: 2013/11/08
/**
 * This class sets up SpacePoint orbit and target colors
 */
//------------------------------------------------------------------------------
#ifndef GmatColorPanel_hpp
#define GmatColorPanel_hpp

#include "gmatdefs.hpp"
#include "SpacePoint.hpp"
#include "gmatwxdefs.hpp"
#include <wx/clrpicker.h>           // for wxColorPickerCtrl
#include "GmatPanel.hpp"
#include "ColorTypes.hpp"           // for GmatColor::

class GmatColorPanel: public wxPanel
{
public:
   GmatColorPanel(wxWindow *parent, GmatPanel *parentGmatPanel,
                  SpacePoint *clonedSpacePoint, bool useInputObjColor = false,
                  bool overrideOrbitColor = false, bool showOrbitColorOnly = false,
                  bool showOverrideOrbitColorCheckBox = false,
                  const std::string &objName = "", UnsignedInt overrdingColor = 0);
   ~GmatColorPanel();
   void SaveData();
   void LoadData();
   
   bool HasColorChanged() { return mHasColorChanged; }
   bool HasOverrideColorChanged() { return mHasOverrideColorChanged; }
   bool GetOverrideColor() { return mOverrideOrbitColor; }
   UnsignedInt GetOrbitColor() { return mOrbitIntColor; }
   UnsignedInt GetTargetColor() { return mTargetIntColor; }
   
private:
   
   bool mHasColorChanged;
   bool mHasOverrideColorChanged;
   bool mUseInputObjectColor;
   bool mOverrideOrbitColor;
   bool mShowOrbitColorOnly;
   bool mShowOverrideOrbitColorCheckBox;
   std::string mSpacePointName;

   UnsignedInt mOverrdingColor;
   UnsignedInt mDefaultOrbitColor;
   UnsignedInt mOrbitIntColor;
   UnsignedInt mTargetIntColor;
   
   GmatPanel  *theParentGmatPanel;
   SpacePoint *theClonedSpacePoint;
   
   // Color pickers
   wxStaticText *mOrbitColorLabel;
   wxStaticText *mTargetColorLabel;
   wxColourPickerCtrl *mOrbitColorCtrl;
   wxColourPickerCtrl *mTargetColorCtrl;
   wxCheckBox *mOverrideOrbitColorCheckBox;
   wxColour mOrbitColor;
   wxColour mTargetColor;
   
   void Create();
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnColorPickerChange(wxColourPickerEvent &event);
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_COLOR_CTRL = 30200,
      ID_CHECKBOX
   };
};
#endif

