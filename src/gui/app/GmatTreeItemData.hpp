//$Id$
//------------------------------------------------------------------------------
//                             GmatTreeItemData
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
// Author: Allison Greene
// Created: 2003/10/08
/**
 * This class provides a data type for tree nodes.
 */
//------------------------------------------------------------------------------
#ifndef GmatTreeItemData_hpp
#define GmatTreeItemData_hpp

#include "gmatwxdefs.hpp"
#include "wx/treectrl.h"

#include "GmatCommand.hpp"

namespace GmatTree
{
   // RESOURCE_ICON_FOLDER should start from 0 since ResourceTree::AddIcons()
   // creates icons by the order in ResourceIconType
   enum ResourceIconType
   {
      RESOURCE_ICON_FOLDER,
      RESOURCE_ICON_FILE,
      RESOURCE_ICON_OPEN_FOLDER,
      
      RESOURCE_ICON_SPACECRAFT,
      RESOURCE_ICON_FORMATION,
      RESOURCE_ICON_GROUND_STATION,
      
      RESOURCE_ICON_HARDWARE,
      RESOURCE_ICON_CHEMICAL_TANK,
      RESOURCE_ICON_ELECTRIC_TANK,
      RESOURCE_ICON_CHEMICAL_THRUSTER,
      RESOURCE_ICON_ELECTRIC_THRUSTER,
      RESOURCE_ICON_SOLAR_POWER_SYSTEM,
      RESOURCE_ICON_NUCLEAR_POWER_SYSTEM,
      RESOURCE_ICON_ANTENNA,
      RESOURCE_ICON_TRANSMITTER,
      RESOURCE_ICON_RECEIVER,
      RESOURCE_ICON_TRANSPONDER,
      
      RESOURCE_ICON_IMPULSIVE_BURN,
      RESOURCE_ICON_FINITE_BURN,
      
      RESOURCE_ICON_PROPAGATOR,
      
      RESOURCE_ICON_SOLARSYSTEM,
      RESOURCE_ICON_SUN,
      RESOURCE_ICON_MERCURY,
      RESOURCE_ICON_VENUS,
      RESOURCE_ICON_EARTH,
      RESOURCE_ICON_MARS,
      RESOURCE_ICON_JUPITER,
      RESOURCE_ICON_SATURN,
      RESOURCE_ICON_URANUS,
      RESOURCE_ICON_NEPTUNE,
      RESOURCE_ICON_PLUTO,
      
      RESOURCE_ICON_MOON,
      RESOURCE_ICON_MOON_GENERIC,
      RESOURCE_ICON_PLANET_GENERIC,
      RESOURCE_ICON_COMET,
      RESOURCE_ICON_ASTEROID,
      RESOURCE_ICON_BARYCENTER,
      RESOURCE_ICON_LIBRATION_POINT,
      
      RESOURCE_ICON_SOLVER,
      RESOURCE_ICON_BOUNDARY_VALUE_SOLVER,
      RESOURCE_ICON_DIFFERENTIAL_CORRECTOR,
      RESOURCE_ICON_OPTIMIZER,
      RESOURCE_ICON_SIMULATOR,
      RESOURCE_ICON_ESTIMATOR,
      
      RESOURCE_ICON_SUBSCRIBER,
      RESOURCE_ICON_REPORT_FILE,
      RESOURCE_ICON_EPHEMERIS_FILE,
      RESOURCE_ICON_ORBIT_VIEW,
      RESOURCE_ICON_XY_PLOT,
      RESOURCE_ICON_GROUND_TRACK_PLOT,
      
      RESOURCE_ICON_MATLAB,
      RESOURCE_ICON_MATLAB_SERVER,
      
      RESOURCE_ICON_SCRIPT,
      
      RESOURCE_ICON_VARIABLE,
      RESOURCE_ICON_ARRAY,
      RESOURCE_ICON_STRING,
      
      RESOURCE_ICON_COORDINATE_SYSTEM,
      
      RESOURCE_ICON_ECLIPSE_LOCATOR,
      RESOURCE_ICON_CONTACT_LOCATOR,
      
      RESOURCE_ICON_GMAT_FUNCTION,
      RESOURCE_ICON_MATLAB_FUNCTION,
	  RESOURCE_ICON_FILEINTERFACE,
      
      RESOURCE_ICON_MEASUREMENT_MODEL,
      RESOURCE_ICON_NETWORK,
      
      RESOURCE_ICON_DEFAULT,
      // RESOURCE_ICON_COUNT should be the last one, it is used in ResourceTree::AddIcons()
      RESOURCE_ICON_COUNT,
   };

   // MISSION_ICON_PROPAGATE should start from 0 since MissionTree::AddIcons()
   // creates icons by the order in MissionIconType
   enum MissionIconType
   {
      MISSION_ICON_PROPAGATE,
      MISSION_ICON_TARGET,
      MISSION_ICON_FOLDER,
      MISSION_ICON_FILE,
      MISSION_ICON_OPENFOLDER,

      MISSION_ICON_WHILE,
      MISSION_ICON_FOR,
      MISSION_ICON_IF,
      MISSION_ICON_SCRIPTEVENT,
      MISSION_ICON_VARY,

      MISSION_ICON_ACHIEVE,
      MISSION_ICON_DELTA_V,
      MISSION_ICON_CALL_GMAT_FUNCTION,
      MISSION_ICON_CALL_MATLAB_FUNCTION,
      MISSION_ICON_NEST_RETURN,
      MISSION_ICON_SAVE_OBJECT,

      MISSION_ICON_ASSIGNMENT,
      MISSION_ICON_TOGGLE,
      MISSION_ICON_BEGIN_FB,
      MISSION_ICON_END_FB,
      MISSION_ICON_REPORT,
      MISSION_ICON_WRITE,

      MISSION_ICON_SET,
      MISSION_ICON_STOP,
      MISSION_ICON_PEN_UP,
      MISSION_ICON_PEN_DOWN,
      MISSION_ICON_MARK_POINT,
      MISSION_ICON_CLEAR_PLOT,
      
      MISSION_ICON_GLOBAL,
      MISSION_ICON_SAVE_MISSION,
      MISSION_ICON_OPTIMIZE,
      MISSION_ICON_MINIMIZE,
      MISSION_ICON_NONLINEAR_CONSTRAINT,
      
      MISSION_ICON_RUN_SIMULATOR,
      MISSION_ICON_RUN_ESTIMATOR,

      MISSION_ICON_FIND_EVENTS,
      MISSION_ICON_DEFAULT,
      
      MISSION_ICON_COUNT,
      MISSION_NO_ICON,
   };
   
   // OutputTree icons used in OutputTree::AddIcons()
   enum OutputIconType
   {
      OUTPUT_ICON_CLOSEDFOLDER,
      OUTPUT_ICON_OPENFOLDER,
      OUTPUT_ICON_REPORT_FILE,
      OUTPUT_ICON_TEXT_EPHEM_FILE,
      OUTPUT_ICON_ORBIT_VIEW,
      OUTPUT_ICON_GROUND_TRACK_PLOT,
      OUTPUT_ICON_XY_PLOT,
      OUTPUT_ICON_FILE,
      OUTPUT_ICON_DEFAULT,
      
      // OUTPUT_ICON_COUNT should be the last one
      OUTPUT_ICON_COUNT
   };
   
   //----------------------------------------------------------------------
   // Note: Do not change the order of ItemType. The wrong order of dataType
   // will not work propery in GmatMainFrame::CreateChild()
   //----------------------------------------------------------------------
   enum ItemType
   {
      UNKNOWN_ITEM = -1,

      //---------- Resource Tree
      RESOURCES_FOLDER = 40000,
      GROUND_STATION_FOLDER,
      SPACECRAFT_FOLDER,
      HARDWARE_FOLDER,
      FORMATION_FOLDER,
      BURN_FOLDER,
      PROPAGATOR_FOLDER,
      SOLVER_FOLDER,
      BOUNDARY_SOLVER_FOLDER,
      OPTIMIZER_FOLDER,
      SUBSCRIBER_FOLDER,
      INTERFACE_FOLDER,
      VARIABLE_FOLDER,
      FUNCTION_FOLDER,
      COORDINATE_SYSTEM_FOLDER,
      SPECIAL_POINT_FOLDER,
      PREDEFINED_FUNCTION_FOLDER,
      SCRIPT_FOLDER,
      CONSTELLATION_FOLDER,
      EVENT_LOCATOR_FOLDER,
      // Added for user modules
      PLUGIN_FOLDER,
      END_OF_RESOURCE_FOLDER,

      // not openable
      INTERFACE,           // Interface objects are now openable (e.g. FileInterface)
      MATLAB_INTERFACE,
      MATLAB_SERVER,

      VIEW_SOLVER_VARIABLES,
      VIEW_SOLVER_GOALS,

      // openable resource
      BEGIN_OF_RESOURCE = 41000,
      SPACECRAFT,
      GROUND_STATION,
      
      HARDWARE,
      FUELTANK_CHEMICAL,
      FUELTANK_ELECTRIC,
      THRUSTER_CHEMICAL,  // need these two as they will have different submenus and icons
      THRUSTER_ELECTRIC,
      POWER_SYSTEM,
      SENSOR,
      
      FORMATION,
      FORMATION_SPACECRAFT,
      CONSTELLATION_SATELLITE,
      
      PROPAGATOR = 41100,
      //SPK_PROPAGATOR,
      
      IMPULSIVE_BURN = 41200,
      FINITE_BURN,

      SOLAR_SYSTEM = 41300,
      CELESTIAL_BODY,
      CELESTIAL_BODY_STAR,    // need to differentiate these because they will have different
      CELESTIAL_BODY_PLANET,  // submenus for adding orbiting body(ies)
      CELESTIAL_BODY_MOON,    //
      CELESTIAL_BODY_COMET,   // wcs 2009.01.09
      CELESTIAL_BODY_ASTEROID,//
      PREDEFINED_BARYCENTER,
      BARYCENTER,
      LIBRATION_POINT,
      
      SOLVER = 41400,
      BOUNDARY_VALUE_SOLVER,
      DIFFERENTIAL_CORRECTOR,
      BROYDEN,
      OPTIMIZER,
      QUASI_NEWTON,
      SQP,
      SIMULATOR,
      ESTIMATOR,
      
      SUBSCRIBER = 41500,
      REPORT_FILE,
      XY_PLOT,
      ORBIT_VIEW,
      GROUND_TRACK_PLOT,
      EPHEMERIS_FILE,
      DYNAMIC_DATA_DISPLAY,
      GROUND_TRACK_AREA,
      
      VARIABLE = 41600,
      ARRAY,
      STRING,

      EVENT_LOCATOR = 41650,
      MEASUREMENT_MODEL,
      
      GMAT_FUNCTION = 41675,
      MATLAB_FUNCTION,
      PREDEFINED_COORDINATE_SYSTEM,
      USER_COORDINATE_SYSTEM,

      INTERFACE_OPENABLE,           // Kludge to make data interfaces work

      USER_DEFINED_OBJECT = 10001,  // 41750 gave results in conflict with other IDs

      // scripts
      SCRIPT_FILE = 41800,
      END_OF_RESOURCE,

      //------------------------------------------
      // Note: MissionTree uses EVT_MENU_RANGE
      //------------------------------------------
      //---------- Mission Tree
      MISSIONS_FOLDER = 42000,
      MISSION_SEQ_TOP_FOLDER,
      MISSION_SEQ_SUB_FOLDER,
      MISSION_SEQ_COMMAND,
      MISSION_TREE_UNDOCKED,
      
      // command
      BEGIN_OF_COMMAND = 43000,
      PROPAGATE,
      MANEUVER,
      BEGIN_FINITE_BURN,
      END_FINITE_BURN,
      TARGET,
      OPTIMIZE,
      ACHIEVE,
      VARY,
      OPTIMIZE_VARY,
      MINIMIZE,
      NON_LINEAR_CONSTRAINT,
		SAVE,
		FIND_EVENTS,
      MANAGE_OBJECT,
      TOGGLE,
      PLOT_ACTION,
      XY_PLOT_ACTION,
      REPORT,
      WRITE_COMMAND,
      CALL_FUNCTION,
      ASSIGNMENT,
      FREE_FORM_SCRIPT,
      SCRIPT_EVENT,
      SET,
      OTHER_COMMAND,

      USER_COMMAND = 43800,         // Allow plugin commands

      END_OF_COMMAND = 43900,

      // control logic
      BEGIN_OF_CONTROL = 44000,
      IF_CONTROL,
      ELSE_IF_CONTROL,
      FOR_CONTROL,
      WHILE_CONTROL,
      DO_CONTROL,
      SWITCH_CONTROL,
      END_OF_CONTROL,

      //---------- Output Tree
      OUTPUT_FOLDER = 45000,
      REPORTS_FOLDER,
      EPHEM_FILES_FOLDER,
      ORBIT_VIEWS_FOLDER,
      GROUND_TRACK_PLOTS_FOLDER,
      XY_PLOTS_FOLDER,
      EVENTS_FOLDER,

      // for output
      BEGIN_OF_OUTPUT = 46000,
      OUTPUT_BEGIN_PLOT,
      OUTPUT_ORBIT_VIEW,
      OUTPUT_GROUND_TRACK_PLOT,
      OUTPUT_XY_PLOT,
      OUTPUT_PERSISTENT,      // Other output windows that should not close between runs
      OUTPUT_END_PLOT,
      OUTPUT_BEGIN_REPORT,
      OUTPUT_REPORT,
      OUTPUT_EVENT_REPORT,
      OUTPUT_TEXT_EPHEM_FILE,
      OUTPUT_END_REPORT,
      OUTPUT_COMPARE_REPORT,
      OUTPUT_SOLVER_WINDOW,
      END_OF_OUTPUT,

      //---------- NO panels will be created
      BEGIN_NO_PANEL = 47000,
      BEGIN_MISSION_SEQUENCE,
      STOP,
      ADDED_SCRIPT_FOLDER,
      END_TARGET,
      END_OPTIMIZE,
      ELSE_CONTROL,
      END_IF_CONTROL,
      END_FOR_CONTROL,
      END_DO_CONTROL,
      END_WHILE_CONTROL,
      END_SWITCH_CONTROL,
      END_SCRIPT_EVENT,
      END_NO_PANEL,
   };
}


class GmatTreeItemData : public wxTreeItemData
{
public:
   GmatTreeItemData(const wxString &name, GmatTree::ItemType type,
                    const wxString &title = "", bool isClonable = true,
                    bool pluginGui = false);
   
   wxString GetName();
   wxString GetTitle();
   GmatTree::ItemType GetItemType();
   bool IsClonable();
   bool HasPluginGui();
   
   void SetName(const wxString &objName);
   void SetTitle(const wxString &title);
   void SetItemType(GmatTree::ItemType type);
   void SetClonable(bool clonable);
   void SetPluginGui(bool pluginGui);
   
   virtual GmatCommand* GetCommand();
   virtual wxTreeItemId GetNodeId();
   
protected:
   wxString mItemTitle;
   wxString mItemName;
   GmatTree::ItemType mItemType;
   bool mIsClonable;
   bool mHasPluginGui;
};

#endif // GmatTreeItemData_hpp
