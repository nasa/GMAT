//$Id$
// *** File Name : gmatdefs.h
// *** Created   : May 20, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 595)                  ***
// ***  Under Contract:  P.O.  GSFC S-67573-G                             ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 5/20/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
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
//                           : 2003/09/16 - W. Shoan/GSFC/583
//                             added ObjectTypes enum type in namespace Gmat
//                           : 11/9/2003 - D. Conway, Thinking Systems, Inc.
//                             Added OBJECT_TYPE for member objects
// **************************************************************************

#ifndef GMATDEFS_HPP
#define GMATDEFS_HPP


#include <string>               // For std::string
#include <cstring>              // Resolves issues for GCC 4.3
#include <vector>               // For std::vector
#include <map>                  // For std::map
#include <stack>                // for std::stack
#include <list>                 // To fix VS DLL import/export issues

#include "utildefs.hpp"


#ifdef _WIN32  // Windows
   #ifdef _MSC_VER  // Microsoft Visual C++

      #define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
      // Windows Header File entry point:
      #include <windows.h>

      #define  _USE_MATH_DEFINES  // makes Msoft define pi

   #endif  // End Microsoft C++ specific block

   #ifdef _DYNAMICLINK  // Only used for Visual C++ Windows DLLs
      #ifdef GMAT_EXPORTS
         #define GMAT_API __declspec(dllexport)
      #else
         #define GMAT_API __declspec(dllimport)
      #endif

      // Provide the storage class specifier (extern for an .exe file, null
      // for DLL) and the __declspec specifier (dllimport for .an .exe file,
      // dllexport for DLL).
      // You must define EXP_STL when compiling the DLL.
      // You can now use this header file in both the .exe file and DLL - a
      // much safer means of using common declarations than two different
      // header files.
      #ifdef EXP_STL
      #    define DECLSPECIFIER __declspec(dllexport)
      #    define EXPIMP_TEMPLATE
      #else
      #    define DECLSPECIFIER __declspec(dllimport)
      #    define EXPIMP_TEMPLATE extern
      #endif

      #define EXPORT_TEMPLATES
   #endif
#endif //  End of OS nits

#ifndef GMAT_API
   #define GMAT_API
#endif


/// Forward reference to the event handler class used to plug in GUI components
class GmatEventHandler;

typedef struct geoparms
{
   Real xtemp;  ///< minimum global exospheric temperature (degrees K)
   Real tkp;    ///< geomagnetic index

} GEOPARMS;

// Macros defining default NULL behavior for RenameRefObject and HasLocalClones
#define DEFAULT_TO_NO_CLONES virtual bool HasLocalClones() { return false; }
#define DEFAULT_TO_NO_REFOBJECTS virtual bool RenameRefObject( \
      const UnsignedInt type, const std::string &oldName, \
      const std::string &newName) { return true; }


namespace Gmat
{
   /**
    * The list of object types
    *
    * This list needs to be synchronized with the GmatBase::OBJECT_TYPE_STRING
    * list found in base/Foundation/GmatBase.cpp
    */
   enum ObjectType
   {
      SPACECRAFT= 101,
      FORMATION,
      SPACEOBJECT,
      GROUND_STATION,
      PLATE,
      BURN,
      
      IMPULSIVE_BURN,
      FINITE_BURN,
      COMMAND,
      PROPAGATOR,
      ODE_MODEL,
      
      PHYSICAL_MODEL,
      TRANSIENT_FORCE,
      INTERPOLATOR,
      SOLAR_SYSTEM,
      SPACE_POINT,
      
      CELESTIAL_BODY,
      CALCULATED_POINT,
      LIBRATION_POINT,
      BARYCENTER,
      ATMOSPHERE,
      
      PARAMETER,
      VARIABLE,
      ARRAY,
      STRING,
      STOP_CONDITION,
      
      SOLVER,
      SUBSCRIBER,
      REPORT_FILE,
      XY_PLOT,
      ORBIT_VIEW,
      DYNAMIC_DATA_DISPLAY,
      GROUNDTRACK,
      
      EPHEMERIS_FILE,
      PROP_SETUP,
      FUNCTION,
      FUEL_TANK,
      THRUSTER,
      
      CHEMICAL_THRUSTER,
      ELECTRIC_THRUSTER,
      CHEMICAL_FUEL_TANK,
      ELECTRIC_FUEL_TANK,
      
      FIELD_OF_VIEW,
      CONICAL_FOV,
	   RECTANGULAR_FOV,
	   CUSTOM_FOV,

      POWER_SYSTEM,        // for PowerSystems
      SOLAR_POWER_SYSTEM,
      NUCLEAR_POWER_SYSTEM,

      HARDWARE,            // Tanks, Thrusters, Antennae, Sensors, etc.
      COORDINATE_SYSTEM,
      AXIS_SYSTEM,
      ATTITUDE,
      MATH_NODE,
      
      MATH_TREE,
      BODY_FIXED_POINT,
      EVENT,
      EVENT_LOCATOR,
      DATAINTERFACE_SOURCE,



      // Estimation types
      MEASUREMENT_MODEL,   // May be replaced by TrackingSystem
//      CORE_MEASUREMENT,    // For the measurement primitives
      ERROR_MODEL,         // Error model used in a measurement
      
//      TRACKING_DATA,
//      TRACKING_SYSTEM,
      DATASTREAM,          // For DataFile container objects      
      DATA_FILE,           // For DataFile objects
      OBTYPE,              // For the specific observation types

      // Data filters
      DATA_FILTER,         // for data filter
      
      INTERFACE,           // MatlabInterface and other Interfaces
      MEDIA_CORRECTION,    // For media correction model
      IMAGER,              // For Sensors
      SENSOR,              // For RFHardwares and Antennas
      RF_HARDWARE,
      ANTENNA,

      THRUST_SEGMENT,
      // Defined region
      REGION,                 // Region of space
      
      USER_DEFINED_OBJECT, // Used for user defined objects that do not fall
                           // into any of the above categories, and for 
                           // internal objects that users don't access
      
      USER_OBJECT_ID_NEEDED = USER_DEFINED_OBJECT + 500,

      GENERIC_OBJECT,      // Used for user defined objects that do not fall 
                           // into any of the above categories, and for 
                           // internal objects that users don't access
      

      UNKNOWN_OBJECT
   };


   enum WriteMode
   {
      SCRIPTING,
      SHOW_SCRIPT,
      OWNED_OBJECT,
      MATLAB_STRUCT,
      EPHEM_HEADER,
      NO_COMMENTS,
      DEBUG_INSPECT, //used by Breakpoint
      GUI_EDITOR,
      OBJECT_EXPORT
   };
   
   enum StateElementId 
   {
      UNKNOWN_STATE = -1,
      CARTESIAN_STATE = 3700,          // Integrable state representations
      EQUINOCTIAL_STATE,
      ORBIT_STATE_TRANSITION_MATRIX,   // STM for the orbit
      ORBIT_A_MATRIX,
      ORBIT_COVARIANCE_MATRIX,         // Covariance Matrix for the orbit
      CD_EPSILON,
      ATMOS_DENSITY_EPSILON,
      MASS_FLOW,                       // m dot
      PREDEFINED_STATE_MAX,
      USER_DEFINED_BEGIN = 3800,
      USER_DEFINED_END = 3999          // Allow up to 200 dynamic entries
   };

   typedef struct PluginResource
   {
      PluginResource()
      {
         trigger = -1;
         firstId = -1;
         lastId  = -1;
         handler = NULL;
      }
      std::string  nodeName;         // Identifier for the resource
      std::string  parentNodeName;   // Owning type identifier, if any
      ObjectType   type;             // Core type
      std::string  subtype;          // Subtype off of the core

      // GUI plugin elements; ignore if not needed
      std::string  toolkit;          // Toolkit used to create the widget
      std::string  widgetType;       // String identifying the widget to open
      Integer      trigger;          // Event ID/type triggering the call
      Integer      firstId;          // Starting ID for event handling
      Integer      lastId;           // Ending ID for event handling

      // Hook that provides the toolkit specific functions for GUI interfaces
      GmatEventHandler *handler;
   } PLUGIN_RESOURCE;

}

#endif //GMATDEFS_HPP
