//$Id$
//------------------------------------------------------------------------------
//                              OpenGlOptionDialog
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
// Created: 2004/12/20
//
/**
 * Declares OpenGlOptionDialog class. This dialog allows user to view and
 * select various OpenGL plot options.
 */
//------------------------------------------------------------------------------

#ifndef OpenGlOptionDialog_hpp
#define OpenGlOptionDialog_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

#include "gmatdefs.hpp"        // for UnsignedIntArray
#include "MdiGlPlotData.hpp"   // for MAX_BODIES, wxStringColorMap, wxStringBoolMap
#include "GuiItemManager.hpp"
#include "RgbColor.hpp"
#include "CoordinateSystem.hpp"

class MdiChildTrajFrame;

class OpenGlOptionDialog : public wxDialog
{
public:
   OpenGlOptionDialog(wxWindow *parent, const wxString &title,
                      const wxArrayString &objectNames,
                      const UnsignedIntArray &objectColors);
   ~OpenGlOptionDialog();
   
   wxArrayString& GetObjectNames()
      { return mObjectNames; }
   UnsignedIntArray& GetObjectColors()
      { return mObjectIntColors; }
   
   void EnableAnimation(bool flag)
      { mViewAnimationButton->Enable(flag); }
   
   void SetDrawXyPlane(bool flag);
   void SetDrawWireFrame(bool flag);
   void SetDrawAxes(bool flag);
   void SetDrawSunLine(bool flag);
   void SetCoordSysName(const wxString &csName);
   void SetAnimationFrameInc(int inc);
   
   void UpdateObjectList(const wxArrayString &objectNames,
                         const wxArrayString &validCSNames,
                         const wxStringBoolMap &showObjects,
                         const wxStringColorMap &objectColors);
   
protected:
   
   // member data
   GuiItemManager *theGuiManager;
   MdiChildTrajFrame *mTrajFrame;
   
   bool mHasRotateAboutXYChanged;
   bool mHasUseViewPointSpecChanged;
   bool mHasUsePerspModeChanged;
   bool mHasDistanceChanged;
   bool mHasGotoObjectChanged;
   bool mHasCoordSysChanged;
   
   bool mHasDrawWireFrameChanged;
   bool mHasDrawAxesChanged;
   bool mHasDrawXyPlaneChanged;
   bool mHasDrawSunLineChanged;
   
   bool mHasXyPlaneColorChanged;
   bool mHasEcPlaneColorChanged;
   bool mHasSunLineColorChanged;
   bool mHasObjectColorChanged;
   
   bool mHasShowObjectChanged;
   bool mHasShowOrbitNormalChanged;

   int   mAnimationUpdInt;
   int   mAnimationFrameInc;
   
   wxArrayString mObjectNames;
   wxArrayString mValidCSNames;
   UnsignedIntArray mObjectIntColors;
   int mObjectCount;
   int mValidCSCount;
   
   wxString mCoordSysName;
   
   wxStringColorMap mObjectColorMap;
   wxStringBoolMap  mInitialShowObjectMap;
   wxStringBoolMap  mShowObjectMap;
   wxStringBoolMap  mShowOrbitNormalMap;
   
   wxTextCtrl *mUpdateIntTextCtrl;
   wxTextCtrl *mFrameIncTextCtrl;
   
   wxComboBox *mCoordSysComboBox;
   
   wxListBox *mObjectListBox;
   
   wxCheckBox *mUseInitialViewDefCheckBox;   
   wxCheckBox *mWireFrameCheckBox;
   wxCheckBox *mXyPlaneCheckBox;
   wxCheckBox *mAxesCheckBox;
   wxCheckBox *mSunLineCheckBox;   
   wxCheckBox *mShowObjectCheckBox;
   wxCheckBox *mShowOrbitNormalCheckBox;

   wxButton *mViewAnimationButton;
   wxButton *mXyPlaneColorButton;
   wxButton *mSunLineColorButton;   
   wxButton *mObjectColorButton;
   wxButton *mCoordSysGoButton;
   wxButton *theApplyButton;
   
   wxColor mXyPlaneColor;
   wxColor mEcPlaneColor;
   wxColor mSunLineColor;
   wxColor mObjectColor;
   
   wxBoxSizer *theDialogSizer;
   wxStaticBoxSizer *mViewObjectSizer;
   
   // member functions
   virtual void Create();
   virtual void ShowData();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   void UpdateObjectListBox();
   bool ShowColorDialog(wxColor &oldColor, wxButton *button);
   void ShowSpacePointOption(const wxString &name);
   
   // event handlers
   void OnTextChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnColorButtonClick(wxCommandEvent& event);
   void OnObjectColorButtonClick(wxCommandEvent& event);
   void OnSatColorButtonClick(wxCommandEvent& event);
   void OnApplyButtonClick(wxCommandEvent& event);
   void OnButtonClick(wxCommandEvent& event);
   void OnSelectObject(wxCommandEvent& event);
   void OnClose(wxCloseEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_APPLY = 8120,
      ID_TEXTCTRL,
      ID_CHECKBOX,
      ID_COMBOBOX,
      ID_LISTBOX,
      ID_BUTTON,
      ID_EQPLANE_COLOR_BUTTON,
      ID_ECPLANE_COLOR_BUTTON,
      ID_SUNLINE_COLOR_BUTTON,
      ID_OBJECT_COLOR_BUTTON,
   };
   
};

#endif // OpenGlOptionDialog_hpp
