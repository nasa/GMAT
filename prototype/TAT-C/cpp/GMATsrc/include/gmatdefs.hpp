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
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
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


typedef double          Real;              // 8 byte float
typedef int             Integer;           // 4 byte signed integer
typedef unsigned char   Byte;              // 1 byte
typedef unsigned int    UnsignedInt;       // 4 byte unsigned integer

typedef std::vector<Real>        RealArray;
typedef std::vector<Integer>     IntegerArray;
typedef std::vector<UnsignedInt> UnsignedIntArray;
typedef std::vector<std::string> StringArray;
typedef std::vector<bool>        BooleanArray;

class GmatBase;        // Forward reference for ObjectArray
class ElementWrapper;  // Forward reference for ElementWrapper
class A1Mjd;           // Forward reference for A1Mjd (epoch)
class Rvector6;        // Forward reference for Rvector6 (ephem state)
typedef std::vector<GmatBase*>                 ObjectArray;
typedef std::vector<ElementWrapper*>           WrapperArray;
typedef std::vector<Rvector6*>                 StateArray;
typedef std::vector<A1Mjd*>                    EpochArray;
typedef std::map<std::string, Integer>         IntegerMap;
typedef std::map<std::string, UnsignedInt>     ColorMap;
typedef std::map<std::string, GmatBase*>       ObjectMap;
typedef std::map<std::string, ElementWrapper*> WrapperMap;
typedef std::stack<ObjectMap*>                 ObjectMapStack;

/// Forward reference to the event handler class used to plug in GUI components
class GmatEventHandler;

typedef struct geoparms
{
   Real xtemp;  /// minimum global exospheric temperature (degrees K)
   Real tkp;    /// geomagnetic index

} GEOPARMS;

/// GMAT's epoch representation; eventually a struct holding MJ day & sec of day
typedef Real GmatEpoch;


// GMAT's Radians representation
typedef Real Radians;

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
      
      EPHEMERIS_FILE,
      PROP_SETUP,
      FUNCTION,
      FUEL_TANK,
      THRUSTER,
      
      CHEMICAL_THRUSTER,
      ELECTRIC_THRUSTER,
      CHEMICAL_FUEL_TANK,
      ELECTRIC_FUEL_TANK,

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
      SENSOR,              // For RFHardwares and Antennas
      RF_HARDWARE,
      ANTENNA,
      
      USER_DEFINED_OBJECT, // Used for user defined objects that do not fall
                           // into any of the above categories, and for 
                           // internal objects that users don't access
      
      USER_OBJECT_ID_NEEDED = USER_DEFINED_OBJECT + 500,

      /// @todo: DJC - Do we need this for backwards compatibility?
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


#ifdef EXPORT_TEMPLATES

   /**
    * The following code cleans up template import/export issues between GMAT and 
    * Visual Studio 2010.  These blocks of code might result in multiply defined
    * structures when using plugin libraries, or when refactoring the base code 
    * into separate libraries.  If that happens, they need to be moved into a
    * .cpp file that is only built in the base library.
    */

    // Instantiate STL template classes used in GMAT  
    // This does not create an object. It only forces the generation of all
    // of the members of the listed classes. It exports them from the DLL 
    // and imports them into the .exe file.

    // This fixes std::string:
    // Only do this if the export is not already in a different module (wx, I'm looking at you!)
    #ifdef IMPEXP_STDSTRING
        EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<char>;
        EXPIMP_TEMPLATE template class DECLSPECIFIER std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
    #endif

//    const std::basic_string::size_type std::basic_string<char, std::char_traits<char>, std::allocator<char>>::npos = size_t(-1); 
//    EXPIMP_TEMPLATE template const typename DECLSPECIFIER std::basic_string<char, std::char_traits<char>, std::allocator<char>>::npos;


 //   template<class _Elem,
	//class _Traits,
	//class _Alloc>
	//_PGLOBAL const typename basic_string<_Elem, _Traits, _Alloc>::size_type
	//	basic_string<_Elem, _Traits, _Alloc>::npos =
	//		(typename basic_string<_Elem, _Traits, _Alloc>::size_type)(-1);

    // Fix StringArray:
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<std::string>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<std::string>;

    // Fix vector of StringArray
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<StringArray>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<StringArray>;

    // Fix ObjectArray
    class GmatBase;     // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<GmatBase*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<GmatBase*>;

    // Fix IntegerArray
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<Integer>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<Integer>;

    // Fix RealArray
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<Real>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<Real>;

    // Fix UnsignedIntArray
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<UnsignedInt>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<UnsignedInt>;

    // Fix vector of bools
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<bool>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<bool>;

    // Fix ObjectType vector
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<UnsignedInt>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<UnsignedInt>;

    // Fix WrapperDataType
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<Gmat::WrapperDataType>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<Gmat::WrapperDataType>;

    // Fix vector of celestial body pointers
    class CelestialBody;     // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<CelestialBody*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<CelestialBody*>;

    // Fix vector of space point pointers
    class SpacePoint;        // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<SpacePoint*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<SpacePoint*>;

    // Fix vector of space object pointers
    class SpaceObject;        // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<SpaceObject*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<SpaceObject*>;

    // Fix vector of FuelTank pointers
    class FuelTank;          // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<FuelTank*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<FuelTank*>;

    // Fix vector of IntegerArrays
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<IntegerArray>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<IntegerArray>;

    // Fix vector of space force pointers
    class PhysicalModel;        // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<PhysicalModel*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<PhysicalModel*>;

    // Fix vector of finite burn pointers
    class FiniteBurn;        // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<FiniteBurn*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<FiniteBurn*>;

    // Fix vector of coordinate system pointers
    class CoordinateSystem;        // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<CoordinateSystem*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<CoordinateSystem*>;

    // Fix vector of TriggerManager pointers
    class TriggerManager;        // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<TriggerManager*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<TriggerManager*>;

    // Fix vector of Sandbox pointers
    class Sandbox;        // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<Sandbox*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<Sandbox*>;

    // Fix vector of command pointers
    class GmatCommand;        // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<GmatCommand*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<GmatCommand*>;

    // Fix vector of PluginResource pointers
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<Gmat::PluginResource*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<Gmat::PluginResource*>;

    // Maps -- still need to be addressed
    //EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<std::string>;
    //EXPIMP_TEMPLATE template struct DECLSPECIFIER std::less< std::string >;
    //EXPIMP_TEMPLATE template struct DECLSPECIFIER std::pair< std::string,std::string >;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::allocator<std::pair<const std::string,std::string> >;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::_Tmap_traits<std::string,std::string,std::less<std::string>, std::allocator<std::pair<const std::string,std::string> >,false>;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::_Tree_nod<std::_Tmap_traits<std::string,std::string,std::less<std::string>, std::allocator<std::pair<const std::string,std::string> >,false> >;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::allocator<std::_Tree_nod<std::_Tmap_traits<std::string,std::string,std::less<std::string>, std::allocator<std::pair<const std::string,std::string> >,false> > >;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::_Tree_val<std::_Tmap_traits<std::string,std::string,std::less<std::string>, std::allocator<std::pair<const std::string,std::string> >,false> >;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::map<std::string, std::string, std::less< std::string >, std::allocator<std::pair<const std::string,std::string> > >;

/*
   #define EXPORT_STL_MAP( mapkey, mapvalue ) \
     template struct DECLSPECIFIER std::pair< mapkey,mapvalue >; \
     template class DECLSPECIFIER std::allocator< \
       std::pair<const mapkey,mapvalue> >; \
     template struct DECLSPECIFIER std::less< mapkey >; \
     template class DECLSPECIFIER std::allocator< \
       std::_Tree_ptr<std::_Tmap_traits<mapkey,mapvalue,std::less<mapkey>, \
       std::allocator<std::pair<const mapkey,mapvalue> >,false> > >; \
     template class DECLSPECIFIER std::allocator< \
       std::_Tree_nod<std::_Tmap_traits<mapkey,mapvalue,std::less<mapkey>, \
       std::allocator<std::pair<const mapkey,mapvalue> >,false> > >; \
     template class DECLSPECIFIER std::_Tree_nod< \
       std::_Tmap_traits<mapkey,mapvalue,std::less<mapkey>, \
       std::allocator<std::pair<const mapkey,mapvalue> >,false> >; \
     template class DECLSPECIFIER std::_Tree_ptr< \
       std::_Tmap_traits<mapkey,mapvalue,std::less<mapkey>, \
       std::allocator<std::pair<const mapkey,mapvalue> >,false> >; \
     template class DECLSPECIFIER std::_Tree_val< \
       std::_Tmap_traits<mapkey,mapvalue,std::less<mapkey>, \
   	std::allocator<std::pair<const mapkey,mapvalue> >,false> >; \
     template class DECLSPECIFIER std::map< \
       mapkey, mapvalue, std::less< mapkey >, \
       std::allocator<std::pair<const mapkey,mapvalue> > >;

   EXPORT_STL_MAP(std::string, std::string)
*/

    // Here are lists of strings:
    //EXPIMP_TEMPLATE template class DECLSPECIFIER std::list<std::string>;


    // other examples:
//    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<int>;
//    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<char>;

#endif

#endif //GMATDEFS_HPP
