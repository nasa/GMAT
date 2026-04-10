//$Id$
//------------------------------------------------------------------------------
//                            CelestialBodyVisualizationPanel
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
// Author: Linda Jun
// Created: 2015.02.02
//
/**
 * This is the panel for the Visualization tab on the notebook on the CelestialBody
 * Panel.
 *
 */
//------------------------------------------------------------------------------


#ifndef CelestialBodyVisualizationPanel_hpp
#define CelestialBodyVisualizationPanel_hpp

#include "gmatdefs.hpp"
#include "CelestialBody.hpp"
#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"
#include "GmatStaticBoxSizer.hpp"

class CelestialBodyVisualizationPanel : public wxPanel
{
public:
   CelestialBodyVisualizationPanel(GmatPanel *cbPanel, wxWindow *parent, CelestialBody *body);
   ~CelestialBodyVisualizationPanel();
   
   void LoadData();
   void SaveData();
   
   bool IsDataChanged() { return dataChanged;};
   bool CanClosePanel() { return canClose;};
   
private:
   
   bool           dataChanged;
   bool           canClose;
   
   CelestialBody  *theBody;
   GuiItemManager *guiManager;
   
   bool           textureFileChanged;
   bool           view3dModelFileChanged;
   bool           view3dModelOffsetChanged;
   bool           view3dModelRotChanged;
   bool           view3dModelScaleChanged;
   
   GmatPanel      *theCBPanel;
   
  
   void     Create();
   void     ResetChangeFlags(bool discardMods = false);
   
   //Event Handling
   DECLARE_EVENT_TABLE();
   void     OnTextureFileTextCtrlChange(wxCommandEvent &event);
   void     OnView3dModelFileTextCtrlChange(wxCommandEvent &event);
   void     OnView3dModelOffsetTextCtrlChange(wxCommandEvent &event);
   void     OnView3dModelRotTextCtrlChange(wxCommandEvent &event);
   void     OnView3dModelScaleTextCtrlChange(wxCommandEvent &event);
   void     OnBrowseButton(wxCommandEvent &event);
   
   
   wxTextCtrl   *textureFileTextCtrl;
   wxTextCtrl   *view3dModelTextCtrl;
   wxTextCtrl   *view3dModelOffsetXTextCtrl;
   wxTextCtrl   *view3dModelOffsetYTextCtrl;
   wxTextCtrl   *view3dModelOffsetZTextCtrl;
   wxTextCtrl   *view3dModelRotXTextCtrl;
   wxTextCtrl   *view3dModelRotYTextCtrl;
   wxTextCtrl   *view3dModelRotZTextCtrl;
   wxTextCtrl   *view3dModelScaleTextCtrl;
   
   wxBitmapButton *textureFileBrowseButton;
   wxBitmapButton *view3dModelBrowseButton;
      
   GmatStaticBoxSizer *pageSizer;
   
   /// IDs for the controls 
   enum
   {
      ID_TEXT = 7100,
      ID_BUTTON_BROWSE,
      ID_TEXT_CTRL_TEXTURE_FILE,
      ID_TEXT_CTRL_3D_MODEL_FILE,
      ID_TEXT_CTRL_3D_MODEL_OFFSET,
      ID_TEXT_CTRL_3D_MODEL_ROT,
      ID_TEXT_CTRL_3D_MODEL_SCALE,
   };
};
#endif // CelestialBodyVisualizationPanel_hpp
