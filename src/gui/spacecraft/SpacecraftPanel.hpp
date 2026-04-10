//$Id$
//------------------------------------------------------------------------------
//                            SpacecraftPanel
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
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
/**
 * This class contains information needed to setup spacecraft through GUI
 * 
 */
//------------------------------------------------------------------------------
#ifndef SpacecraftPanel_hpp
#define SpacecraftPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "BallisticsMassPanel.hpp"
#include "OrbitPanel.hpp"
#include "TankPanel.hpp"
#include "AttitudePanel.hpp"
#include "ThrusterPanel.hpp"
#include "PowerSystemPanel.hpp"
#include "SpicePanel.hpp"
#include "GuiInterpreter.hpp"
#include "VisualModelPanel.hpp"

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"
#include "FuelTank.hpp"

class SpacecraftPanel: public GmatPanel
{
public:
   SpacecraftPanel(wxWindow *parent, const wxString &scName);
   ~SpacecraftPanel();   
       
   virtual bool RefreshObjects(UnsignedInt type = Gmat::UNKNOWN_OBJECT);

private:    
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   void OnPageChange(wxCommandEvent &event);

   Spacecraft *theSpacecraft;
   Spacecraft *currentSpacecraft;
   
   wxStaticText *description1;
   wxStaticText *description2;
   wxStaticText *description3;
   wxStaticText *description4;
   wxStaticText *description5;
   wxStaticText *description6;
   
   wxTextCtrl *textCtrl1;
   wxTextCtrl *textCtrl2;
   wxTextCtrl *textCtrl3;
   wxTextCtrl *textCtrl4;
   wxTextCtrl *textCtrl5;
   wxTextCtrl *textCtrl6;
   
   wxStaticText *label1;
   wxStaticText *label2;
   wxStaticText *label3;
   wxStaticText *label4;
   wxStaticText *label5;
   wxStaticText *label6;
   
   wxNotebook *spacecraftNotebook;
   wxNotebook *actuatorNotebook;

   wxPanel *actuators;
   wxPanel *sensors;
   
   BallisticsMassPanel *theBallisticMassPanel;
   OrbitPanel          *theOrbitPanel;
   TankPanel           *theTankPanel;
   ThrusterPanel       *theThrusterPanel;
   AttitudePanel       *theAttitudePanel;
   VisualModelPanel    *theVisualModelPanel;
   SpicePanel          *theSpicePanel;
   PowerSystemPanel    *thePwrSysPanel;
   
   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 30000,
      ID_COMBO,
      ID_TEXTCTRL,
      ID_NOTEBOOK,
      ID_BUTTON,
      ID_ELEMENT_TAG1,
      ID_ELEMENT_TAG2,
      ID_ELEMENT_TAG3,
      ID_ELEMENT_TAG4,
      ID_ELEMENT_TAG5,
      ID_ELEMENT_TAG6,
      
      ID_ELEMENT_VALUE1,
      ID_ELEMENT_VALUE2,
      ID_ELEMENT_VALUE3,
      ID_ELEMENT_VALUE4,
      ID_ELEMENT_VALUE5,
      ID_ELEMENT_VALUE6,
      
      ID_STATIC_COORD,
      ID_STATIC_ORBIT,
      ID_STATIC_ELEMENT,
      
      ID_CB_BODY,
      ID_CB_FRAME,
      ID_CB_EPOCH,
      ID_CB_STATE,
      
      ID_EPOCH_VALUE,
      
      ID_BUTTON_OK,
      ID_BUTTON_APPLY,
      ID_BUTTON_CANCEL,
      ID_BUTTON_HELP
   };
};
#endif

