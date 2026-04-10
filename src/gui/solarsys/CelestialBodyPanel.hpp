//$Id$
//------------------------------------------------------------------------------
//                              CelestialBodyPanel
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
// Author: Wendy C. Shoan (original author: Allison Greene 2005.07.06)
// Created: 2009.01.26
// Modification:
// 2015.02.02 Linda Jun - Added visualization tab
//
/**
 * Declares CelestialBodyPanel class.  This panel contains a notebook with
 * three tabs for Properties, Orbit, Orientation, and Visualization
 */
//------------------------------------------------------------------------------

#ifndef CelestialBodyPanel_hpp
#define CelestialBodyPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "CelestialBody.hpp"
#include "CelestialBodyPropertiesPanel.hpp"
#include "CelestialBodyOrbitPanel.hpp"
#include "CelestialBodyOrientationPanel.hpp"
#include "CelestialBodyVisualizationPanel.hpp"
#include "gmatdefs.hpp"

class CelestialBodyPanel: public GmatPanel
{
public:
   
   // constructors
   CelestialBodyPanel(wxWindow *parent, const wxString &name);
   virtual ~CelestialBodyPanel();
    
private:
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   void OnPageChange(wxNotebookEvent &event);
   
   CelestialBody *theCelestialBody;
   CelestialBody *origCelestialBody;
   //Planet *thePlanet;
   wxString      bodyName;
   bool          isUserDefined;
   
   wxNotebook    *cbNotebook;
   
   CelestialBodyPropertiesPanel    *properties;
   CelestialBodyOrbitPanel         *orbit;
   CelestialBodyOrientationPanel   *orientation;
   CelestialBodyVisualizationPanel *visualization;
   
   void EnableAll(bool enable);

   // event handling
   
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_NOTEBOOK,
      ID_BUTTON_OK,
      ID_BUTTON_APPLY,
      ID_BUTTON_CANCEL,
      ID_BUTTON_HELP,
   };
   
};

#endif
