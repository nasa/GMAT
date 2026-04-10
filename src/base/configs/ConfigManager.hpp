//$Id$
//------------------------------------------------------------------------------
//                                ConfigManager
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
// Author: Darrel J. Conway
// Created: 2003/10/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Configuration manager used to manage configured (i.e. named) GMAT objects.
 */
//------------------------------------------------------------------------------

#ifndef ConfigManager_hpp
#define ConfigManager_hpp

#include <vector>
#include <map>

#include "ODEModel.hpp"
#include "Subscriber.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "PropSetup.hpp"
#include "Spacecraft.hpp"
#include "Plate.hpp"
#include "StopCondition.hpp"
#include "PhysicalModel.hpp"
#include "Propagator.hpp"
#include "Parameter.hpp"
#include "GmatCommand.hpp"
#include "Burn.hpp"
#include "Solver.hpp"
#include "AtmosphereModel.hpp"
#include "Function.hpp"
#include "Hardware.hpp"
#include "FieldOfView.hpp"
#include "CoordinateSystem.hpp"
#include "CalculatedPoint.hpp"

class MeasurementModelBase;
//class CoreMeasurement;
class ErrorModel;
class DataFilter;
class DataFile;
class ObType;
//class TrackingSystem;
//class TrackingData;
class EventLocator;


/**
 * Class used to manage configured objects prior to cloning into the Sandbox.
 */
class GMAT_API ConfigManager
{
public:
   static ConfigManager*   Instance();
   
   std::string         GetNewName(const std::string &name, Integer startCount);
   
   void                AddObject(UnsignedInt objType, GmatBase *obj);
   void                AddPhysicalModel(PhysicalModel *pm);
   void                AddPropagator(Propagator *prop);
   void                AddODEModel(ODEModel *fm);
   void                AddSubscriber(Subscriber *subs);
   void                AddSolarSystem(SolarSystem *solarSys);
   void                AddPropSetup(PropSetup *propSetup);
   void                AddSpacecraft(SpaceObject *sc);
   void                AddSpacePoint(SpacePoint *sp);
   void                AddPlate(Plate *pl);
   void                AddHardware(Hardware *hw);
   void                AddFieldOfView(FieldOfView *fov);
   void                AddStopCondition(StopCondition* stopCond);
   void                AddParameter(Parameter* parameter);
   void                AddBurn(Burn* burn);
   void                AddSolver(Solver *solver);
   void                AddAtmosphereModel(AtmosphereModel *atmosModel);
   void                AddFunction(Function *function);
   void                AddCoordinateSystem(CoordinateSystem *cs);
   void                AddCalculatedPoint(CalculatedPoint *cp);
   
   void                SetDefaultSolarSystem(SolarSystem *ss);
   void                SetSolarSystemInUse(SolarSystem *ss);
   bool                SetSolarSystemInUse(const std::string &name);

   void                AddMeasurementModel(MeasurementModelBase *mModel);
//   void                AddMeasurement(CoreMeasurement *meas);
   void                AddErrorModel(ErrorModel *meas);
   void                AddDataFilter(DataFilter *filter);
   void                AddDataFile(DataFile *meas);
   void                AddObType(ObType *meas);
   void                AddEventLocator(EventLocator *el);
//   void                AddTrackingSystem(TrackingSystem *ts);
//   void                AddTrackingData(TrackingData *td);

   const StringArray&  GetListOfAllItems();
   const StringArray&  GetListOfItems(UnsignedInt itemType,
                                      UnsignedInt typeMax = 0);
   const StringArray&  GetListOfItems(const std::string &typeName);
   const StringArray&  GetListOfItemsHas(UnsignedInt type,
                                         const std::string &name,
                                         bool includeSysParam = true);
   GmatBase*           AddClone(const std::string &name, std::string &cloneName);
   GmatBase*           GetFirstItemUsing(UnsignedInt type,
                                         const std::string &name,
                                         bool includeSysParam = true);
   GmatBase*           GetItem(const std::string &name);
   
   bool                ChangeMappingName(UnsignedInt itemType,
                                         const std::string &oldName,
                                         const std::string &newName,
                                         GmatBase **mapObj);
   bool                RenameItem(UnsignedInt itemType,
                                  const std::string &oldName,
                                  const std::string &newName);
   bool                RelatedNameChange(std::vector<UnsignedInt> &itemType,
                                  StringArray &oldName, StringArray &newName);
   
   bool                RemoveAllItems();
   bool                RemoveItem(UnsignedInt type, const std::string &name,
                                  bool removeAssociatedSysParam = false);
   bool                ReconfigureItem(GmatBase *newobj, const std::string &name);
   
   
   PhysicalModel*      GetPhysicalModel(const std::string &name);
   Propagator*         GetPropagator(const std::string &name);
   ODEModel*           GetODEModel(const std::string &name);
   SpaceObject*        GetSpacecraft(const std::string &name);
   SpacePoint*         GetSpacePoint(const std::string &name);
   Plate*              GetPlate(const std::string &name);
   Hardware*           GetHardware(const std::string &name);
   FieldOfView*        GetFieldOfView(const std::string &name);
   PropSetup*          GetPropSetup(const std::string &name);
   Subscriber*         GetSubscriber(const std::string &name);
   SolarSystem*        GetDefaultSolarSystem();
   SolarSystem*        GetSolarSystemInUse();
   SolarSystem*        GetSolarSystemInUse(const std::string &name);
   StopCondition*      GetStopCondition(const std::string &name);
   Parameter*          GetParameter(const std::string &name);
   Burn*               GetBurn(const std::string &name);
   Solver*             GetSolver(const std::string &name);
   AtmosphereModel*    GetAtmosphereModel(const std::string &name);
   Function*           GetFunction(const std::string &name);
   CoordinateSystem*   GetCoordinateSystem(const std::string &name);
   CalculatedPoint*    GetCalculatedPoint(const std::string &name);
   MeasurementModelBase* GetMeasurementModel(const std::string &name);
   ErrorModel*         GetErrorModel(const std::string &name);
   DataFilter*         GetDataFilter(const std::string &name);

//   TrackingSystem*     GetTrackingSystem(const std::string &name);
//   TrackingData*       GetTrackingData(const std::string &name);

   DataFile *          GetDataStream(const std::string &name);

   EventLocator*       GetEventLocator(const std::string &name);

   bool                HasConfigurationChanged();
   void                ConfigurationChanged(bool tf);
   ObjectMap*          GetObjectMap();
   
private:
   
   /// The singleton instance
   static ConfigManager*               theConfigManager;
   /// The managed objects
   std::vector<GmatBase*>              objects;
   std::vector<GmatBase*>              newObjects;
   /// A list of the names of the managed objects
   StringArray                         listOfItems;
   /// Mapping between the object names and their pointers
   std::map<std::string, GmatBase *>   mapping;
   /// Flag indicating that managed object has been added or removed by the user
   bool                                configChanged;

   // Data structures for name changes that need to propagate into commands
   /// Array of type updates
   std::vector<UnsignedInt>       changedItemType;
   /// Old names that were changed
   StringArray                         oldRelatedName;
   /// The new names for the related objects
   StringArray                         newRelatedName;
   
   // Treat default and in use solar system separately until we can manage solar 
   // system by name.  All solar system names are "SolarSystem" for now.
   
   /// Default Solar Systems 
   SolarSystem *defaultSolarSystem;
   /// Solar Systems in use
   SolarSystem *solarSystemInUse;
   
   void                AddObject(GmatBase* obj);
   
   // Hide the default constructor and destructor to preserve singleton status
   ConfigManager();
   ~ConfigManager();
   
};


#endif // ConfigManager_hpp

