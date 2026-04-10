//$Id$
//------------------------------------------------------------------------------
//                             FactoryManager
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
// Author: Wendy Shoan
// Created: 2003/08/27
//
/**
 * This class is the interface between the Moderator and Factories.  It is a
 * singleton - only one instance of this class can be created.
 */
//------------------------------------------------------------------------------
#ifndef FactoryManager_hpp
#define FactoryManager_hpp

#include <list>
#include <string>
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Spacecraft.hpp"
#include "Plate.hpp"
#include "Parameter.hpp"
#include "Propagator.hpp"
#include "ODEModel.hpp"
#include "PhysicalModel.hpp"
#include "PropSetup.hpp"
#include "StopCondition.hpp"
#include "CalculatedPoint.hpp"
#include "CelestialBody.hpp"
#include "SolarSystem.hpp"
#include "Solver.hpp"
#include "Subscriber.hpp"
#include "GmatCommand.hpp"
#include "Burn.hpp"
#include "AtmosphereModel.hpp"
#include "Function.hpp"
#include "AxisSystem.hpp"
#include "CoordinateSystem.hpp"
#include "MathNode.hpp"
#include "Attitude.hpp"

class MeasurementModelBase;
//class CoreMeasurement;
class ErrorModel;
class DataFilter;
class DataFile;
class ObType;
//class TrackingSystem;
//class TrackingData;
class EphemerisFile;
class Interface;
class EventLocator;

/**
 * GMAT Factory Manager Class, the interface between the Moderator and the
 * factories.
 *
 * The Factory Manager manages all of the factories that are needed for
 * execution of the system for a particular run.  Each factory must be
 * registered with the FactoryManager in order for its objects to be able to be
 * created.  The Moderator will register all of the predefined factories;
 * user-defined factories must be registered as they are added to the system.
 */
class GMAT_API FactoryManager
{
public:
   // class instance method (this is a singleton class)
   static FactoryManager* Instance();
   // all factories must be registered via this method
   bool                   RegisterFactory(Factory* fact);

   // Generic method to create an object
   GmatBase*              CreateObject(const UnsignedInt generalType,
                                       const std::string &ofType,
                                       const std::string &withName = "");
   
   // methods to create and return objects of the various types
   SpaceObject*           CreateSpacecraft(const std::string &ofType,
                                           const std::string &withName = "");

   Plate*                 CreatePlate(const std::string &ofType, 
                                      const std::string &withName = "");

   Parameter*             CreateParameter(const std::string &ofType,
                                          const std::string &withName = "",
                                            GmatBase* owner = NULL);
   Propagator*            CreatePropagator(const std::string &ofType,
                                           const std::string &withName = "");
   PhysicalModel*         CreatePhysicalModel(const std::string &ofType,
                                              const std::string &withName = "");
   StopCondition*         CreateStopCondition(const std::string &ofType,
                                              const std::string &withName = "");
   CalculatedPoint*       CreateCalculatedPoint(const std::string &ofType,
                                                const std::string &withName = "");
   CelestialBody*         CreateCelestialBody(const std::string &ofType,
                                              const std::string &withName = "");
   Solver*                CreateSolver(const std::string &ofType,
                                       const std::string &withName = "");
   Subscriber*            CreateSubscriber(const std::string &ofType,
                                           const std::string &withName = "");
   EphemerisFile*         CreateEphemerisFile(const std::string &ofType,
                                              const std::string &withName = "");
   GmatCommand*           CreateCommand(const std::string &ofType,
                                        const std::string &withName = "");
   Burn*                  CreateBurn(const std::string &ofType,
                                     const std::string &withName = "");
   AtmosphereModel*       CreateAtmosphereModel(const std::string &ofType,
                                                const std::string &withName = "");
   Function*              CreateFunction(const std::string &ofType,
                                         const std::string &withName = "");
   Hardware*              CreateHardware(const std::string &ofType,
                                         const std::string &withName = "");
   FieldOfView*           CreateFieldOfView(const std::string &ofType,
                                            const std::string &withName = "");
   AxisSystem*            CreateAxisSystem(const std::string &ofType,
                                           const std::string &withName = "");
   MathNode*              CreateMathNode(const std::string &ofType,
                                         const std::string &withName = "");
   Attitude*              CreateAttitude(const std::string &ofType,
                                         const std::string &withName = "");
   SpacePoint*            CreateSpacePoint(const std::string &ofType,
                                           const std::string &withName = "");

//   CoreMeasurement*       CreateMeasurement(const std::string &ofType,
//                                            const std::string &withName = "");

   ErrorModel*            CreateErrorModel(const std::string &ofType,
                                            const std::string &withName = "");

   DataFilter*            CreateDataFilter(const std::string &ofType,
                                            const std::string &withName = "");

   ObType*                CreateObType(const std::string &ofType,
                                       const std::string &withName = "");
   
   Interface*             CreateInterface(const std::string &ofType,
                                          const std::string &withName = "");
   
   //----- Just container
   SolarSystem*           CreateSolarSystem(const std::string &withName = "");
   PropSetup*             CreatePropSetup(const std::string &withName = "");
   ODEModel*              CreateODEModel(const std::string &ofType,
                                         const std::string &withName = "");
   CoordinateSystem*      CreateCoordinateSystem(const std::string &withName = "");

   MeasurementModelBase*  CreateMeasurementModel(const std::string &ofType,
                                                 const std::string &withName);
   DataFile*              CreateDataFile(const std::string &ofType,
                                         const std::string &withName);
   //TrackingSystem*        CreateTrackingSystem(const std::string &ofType,
   //                                            const std::string &withName);
   //TrackingData*          CreateTrackingData(const std::string &withName = "");
   EventLocator*          CreateEventLocator(const std::string &ofType,
                                             const std::string &withName = "");

   // method to return a list of strings representing the objects of the input
   // type that may be created in the system
   const StringArray&     GetListOfItems(UnsignedInt byType,
                                const std::string &withQualifier = "");
   const StringArray&     GetListOfAllItems();
   const StringArray&     GetListOfAllItemsExcept(const ObjectTypeArray &types);
   const StringArray&     GetListOfViewableItems(UnsignedInt byType);
   const StringArray&     GetListOfUnviewableItems(UnsignedInt byType);
   const ObjectTypeArrayMap& GetAllObjectTypeArrayMap();
   bool                   DoesObjectTypeMatchSubtype(
                                const UnsignedInt coreType,
                                const std::string &theType,
                                const std::string &theSubtype);

   // method to return the base type for the input string
   UnsignedInt            GetBaseTypeOf(const std::string &typeName);
   
   // AWB Extension
   void                   SetOverride(Factory * useFactory,
                                      const std::string &ofType);

protected:
   StringArray            entireList;
   
private:

   // private class data
   /// the list of factories that have been registered and which are available
   /// to create objects
   std::list<Factory*> factoryList;
   /// the list of object types that factory can create
   std::list<UnsignedInt> factoryTypeList;
   /// the map of object types that factory can create
   ObjectTypeArrayMap objectTypeArrayMap;
   /// pointer to the only instance allowed for this singleton class
   static FactoryManager* onlyInstance;
   
   // private methods 
   Factory*               FindFactory(UnsignedInt ofType, const std::string &forType, GmatBase* owner = NULL, const std::string &withName = "");
   const StringArray&     GetList(UnsignedInt ofType, const std::string &withQualifier);
   const StringArray&     GetListOfViewables(UnsignedInt ofType);
   const StringArray&     GetListOfUnviewables(UnsignedInt ofType);

   // AWB Extension
   std::map<std::string, Factory*> overrideFactoryList;
   
   // Hide the default constructor and destructor to preserve singleton status
   // default constructor
   FactoryManager();
   // class destructor
   ~FactoryManager();
};

#endif // FactoryManager_hpp
