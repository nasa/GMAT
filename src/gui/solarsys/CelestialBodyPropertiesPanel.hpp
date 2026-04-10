//$Id$
//------------------------------------------------------------------------------
//                                  CelestialBodyPropertiesPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Wendy C. Shoan
// Created: 2009.01.13
//
/**
 * This is the panel for the Properties tab on the notebook on the CelestialBody
 * Panel.
 *
 */
//------------------------------------------------------------------------------


#ifndef CelestialBodyPropertiesPanel_hpp
#define CelestialBodyPropertiesPanel_hpp

#include "gmatdefs.hpp"
#include "CelestialBody.hpp"
#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"
#include "GmatStaticBoxSizer.hpp"

class CelestialBodyPropertiesPanel : public wxPanel
{
public:
   CelestialBodyPropertiesPanel(GmatPanel *cbPanel, wxWindow *parent, CelestialBody *body);
   ~CelestialBodyPropertiesPanel();
   
   void SaveData();
   void LoadData();
   
   bool IsDataChanged() { return dataChanged;};
   bool CanClosePanel() { return canClose;};
   
private:
   
   bool           userDef;
   bool           allowSpiceForDefaultBodies;
   bool           spiceAvailable;

   bool           dataChanged;
   bool           canClose;
   bool           pckFilesDeleted;
   
   StringArray    pckFiles;
   StringArray    pckFilesToDelete;

   CelestialBody  *theBody;
   GuiInterpreter *guiInterpreter;
   GuiItemManager *guiManager;
   SolarSystem    *ss;
   
   Real           mu;
   Real           eqRad;
   Real           flat;
   
   bool           muChanged;
   bool           eqRadChanged;
   bool           flatChanged;
   bool           pckChanged;
   
   GmatPanel      *theCBPanel;
   
  
   void     Create();
   void     ResetChangeFlags(bool discardMods = false);
   
   //Event Handling
   DECLARE_EVENT_TABLE();
   void     OnMuTextCtrlChange(wxCommandEvent &event);
   void     OnEqRadTextCtrlChange(wxCommandEvent &event);
   void     OnFlatTextCtrlChange(wxCommandEvent &event);
   void     OnPckFileBrowseButton(wxCommandEvent &event);
   void     OnPckFileRemoveButton(wxCommandEvent &event);
   void     OnPckFileListBoxChange(wxCommandEvent &event);
   
   wxString ToString(Real rval);
   
   // wx
   
   StringArray  pckFileArray;

   wxString     *pckFileArrayWX;

   wxStaticText *muStaticText;
   wxStaticText *eqRadStaticText;
   wxStaticText *flatStaticText;
   wxStaticText *pckStaticText;

   wxStaticText *muUnitsStaticText;
   wxStaticText *eqRadUnitsStaticText;
   wxStaticText *flatUnitsStaticText;

   wxTextCtrl   *muTextCtrl;
   wxTextCtrl   *eqRadTextCtrl;
   wxTextCtrl   *flatTextCtrl;
   
   wxListBox    *pckFileListBox;

   /// string versions of current data
   wxString     muString;
   wxString     eqRadString;
   wxString     flatString;
   
   wxButton     *pckFileBrowseButton;
   wxButton     *pckFileRemoveButton;

   GmatStaticBoxSizer *pageSizer;
   
   /// IDs for the controls 
   enum
   {
      ID_TEXT = 7100,
      ID_BROWSE_BUTTON_PCK_FILE,
      ID_REMOVE_BUTTON_PCK_FILE,
      ID_TEXT_CTRL_MU,
      ID_TEXT_CTRL_EQRAD,
      ID_TEXT_CTRL_FLAT,
      ID_LIST_BOX_PCK_FILE,
   };
};
#endif // CelestialBodyPropertiesPanel_hpp
