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


#include "ConfigManager.hpp"
#include "ConfigManagerException.hpp"
#include "StringUtil.hpp"               // for GmatStringUtil::
#include "PluginItemManager.hpp"

//#define DEBUG_RENAME 1
//#define DEBUG_CONFIG 1
//#define DEBUG_CONFIG_SS 1
//#define DEBUG_CONFIG_ADD
//#define DEBUG_CONFIG_ADD_CLONE 1
//#define DEBUG_CONFIG_CHANGE 1
//#define DEBUG_CONFIG_REMOVE
//#define DEBUG_CONFIG_REMOVE_MORE
//#define DEBUG_CONFIG_OBJ_USING
//#define DEBUG_FIND_ITEMS 1

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static members
//---------------------------------


ConfigManager* ConfigManager::theConfigManager = NULL;


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ConfigManager* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain the singleton.
 *
 * @return the singleton instance of the configuration manager.
 */
//------------------------------------------------------------------------------
ConfigManager* ConfigManager::Instance()
{
   if (!theConfigManager)
      theConfigManager = new ConfigManager;
        
   return theConfigManager;
}


//------------------------------------------------------------------------------
// ConfigManager()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
ConfigManager::ConfigManager() :
   configChanged        (false),
   defaultSolarSystem   (NULL),
   solarSystemInUse     (NULL)
{
}

// class destructor
//------------------------------------------------------------------------------
// ~ConfigManager()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ConfigManager::~ConfigManager()
{
   RemoveAllItems();
}


//------------------------------------------------------------------------------
// std::string GetNewName(const std::string &name, Integer startCount)
//------------------------------------------------------------------------------
/*
 * It gives new name by adding counter to the input name.
 *
 * @param <name> Base name to used to generate new name
 * @param <startCount> Starting counter
 * @return new name
 */
//------------------------------------------------------------------------------
std::string ConfigManager::GetNewName(const std::string &name, Integer startCount)
{
   #if DEBUG_CONFIG_NEW_NAME
   MessageInterface::ShowMessage
      ("ConfigManager::GetNewName() name = %s\n", name.c_str());
   #endif
   
   if (name == "")
      return "";
   
   // get initial new name
   Integer counter = 0;
   std::string baseName = GmatStringUtil::RemoveLastNumber(name, counter);
   if (counter == 0)
      counter = startCount;
   
   std::string newName = baseName + GmatStringUtil::ToString(counter, 1);
   
   // construct new name while it exists
   while (GetItem(newName))
   {
      ++counter;
      newName = baseName + GmatStringUtil::ToString(counter,1);
   }
   
   #if DEBUG_CONFIG_NEW_NAME
   MessageInterface::ShowMessage
      ("ConfigManager::GetNewName() newName = %s\n", newName.c_str());
   #endif
   
   return newName;
}


//------------------------------------------------------------------------------
// void AddObject(UnsignedInt objType, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds an object to the configuration.
 *
 * @param objType Type of the object
 * @param obj Pointer to the object instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddObject(UnsignedInt objType, GmatBase *obj)
{
   #ifdef DEBUG_ADD_OBJECT
   MessageInterface::ShowMessage
      ("ConfigManager::AddObject() entered, objType=%d, obj=<%p>\n", objType, obj);
   #endif
   
   if (obj == NULL)
      throw ConfigManagerException("Cannot add NULL object");
   
   std::string name = obj->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   
   if ((!obj->IsOfType(objType)) && (objType < Gmat::USER_DEFINED_OBJECT))
      throw ConfigManagerException(name + " is not a valid object type");
   
   AddObject(obj);
}


//------------------------------------------------------------------------------
// void AddPhysicalModel(PhysicalModel *pm)
//------------------------------------------------------------------------------
/**
 * Adds a PhysicalModel to the configuration.
 *
 * @param pm Pointer to the PhysicalModel instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddPhysicalModel(PhysicalModel *pm)
{
   if (pm == NULL)
      throw ConfigManagerException("Cannnot add NULL PhysicalModel object");
   
   std::string name = pm->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   
   if (!pm->IsOfType(Gmat::PHYSICAL_MODEL))
      throw ConfigManagerException(name + " is not a PhysicalModel");

   AddObject(pm);
}

//------------------------------------------------------------------------------
// void ConfigManager::AddPropagator(Propagator *prop)
//------------------------------------------------------------------------------
/**
 * Adds a Propagator to the configuration.
 *
 * @param prop Pointer to the Propagator instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddPropagator(Propagator *prop)
{
   if (prop == NULL)
      throw ConfigManagerException("Cannot add NULL Propagator object");

   std::string name = prop->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!prop->IsOfType(Gmat::PROPAGATOR))
      throw ConfigManagerException(name + " is not a Propagator");

   AddObject(prop);
}


//------------------------------------------------------------------------------
// void AddODEModel(ForceModel *fm)
//------------------------------------------------------------------------------
/**
 * Adds a ODEModel to the configuration.
 *
 * @param fm Pointer to the ForceModel instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddODEModel(ODEModel *fm)
{
   if (fm == NULL)
      throw ConfigManagerException("Cannot add NULL ODEModel object");

   std::string name = fm->GetName();

   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!fm->IsOfType(Gmat::ODE_MODEL))
      throw ConfigManagerException(name + " is not a ForceModel");

   AddObject(fm);
}


//------------------------------------------------------------------------------
// void AddSubscriber(Subscriber *subs)
//------------------------------------------------------------------------------
/**
 * Adds a Subscriber to the configuration.
 *
 * @param subs Pointer to the Subscriber.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddSubscriber(Subscriber *subs)
{
   if (subs == NULL)
      throw ConfigManagerException("Cannot add NULL Subscriber object");

   std::string name = subs->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!subs->IsOfType(Gmat::SUBSCRIBER))
      throw ConfigManagerException(name + " is not a Subscriber");

   AddObject(subs);
}


//------------------------------------------------------------------------------
// void AddSolarSystem(SolarSystem *solarSys)
//------------------------------------------------------------------------------
/**
 * Adds a SolarSystem to the configuration.
 *
 * @param solarSys Pointer to the SolarSystem instance.
 *
 * @todo Add solar systems to the ConfigManager
 */
//------------------------------------------------------------------------------
void ConfigManager::AddSolarSystem(SolarSystem *solarSys)
{
   throw ConfigManagerException("SolarSystem objects are not yet managed");
}


//------------------------------------------------------------------------------
// void AddPropSetup(PropSetup* propSetup)
//------------------------------------------------------------------------------
/**
 * Adds a PropSetup to the configuration.
 *
 * @param propSetup Pointer to the PropSetup instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddPropSetup(PropSetup* propSetup)
{
   if (propSetup == NULL)
      throw ConfigManagerException("Cannot add NULL PropSetup object");

   std::string name = propSetup->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!propSetup->IsOfType(Gmat::PROP_SETUP))
      throw ConfigManagerException(name + " is not a PropSetup");

   AddObject(propSetup);
}


//------------------------------------------------------------------------------
// void AddSpacecraft(SpaceObject *sc)
//------------------------------------------------------------------------------
/**
 * Adds a Spacecraft and formations to the configuration.
 *
 * @param sc Pointer to the Spacecraft/Formation instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddSpacecraft(SpaceObject *sc)
{
   if (sc == NULL)
      throw ConfigManagerException("Cannot add NULL Spacecraft object");

   std::string name = sc->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!sc->IsOfType(Gmat::SPACEOBJECT))
      throw ConfigManagerException(name + " is not a SpaceObject");

   AddObject(sc);
}


//------------------------------------------------------------------------------
// void AddSpacePoint(SpacePoint *sp)
//------------------------------------------------------------------------------
/**
 * Adds a SpacePoint to the configuration.
 * 
 * NOTE: Spacecraft and Formations are handled in the AddSpacecraft method. 
 *
 * @param sp Pointer to the SpacePoint instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddSpacePoint(SpacePoint *sp)
{
   if (sp == NULL)
      throw ConfigManagerException("Cannot add NULL SpacePoint object");

   std::string name = sp->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!sp->IsOfType(Gmat::SPACE_POINT))
      throw ConfigManagerException(name + " is not a SpacePoint");

   AddObject(sp);
}


//------------------------------------------------------------------------------
// void AddPlate(Plate *pl)
//------------------------------------------------------------------------------
/**
* Adds a Plate to the configuration.
*
* @param pl Pointer to the Plate instance.
*/
//------------------------------------------------------------------------------
void ConfigManager::AddPlate(Plate *pl)
{
	if (pl == NULL)
		throw ConfigManagerException("Cannot add NULL Plate object");

	std::string name = pl->GetName();
	if (name == "")
		throw ConfigManagerException("Unnamed objects cannot be managed");

	if (!pl->IsOfType(Gmat::PLATE))
		throw ConfigManagerException(name + " is not a Plate");

	AddObject(pl);
}


//------------------------------------------------------------------------------
// void AddHardware(Hardware *hw)
//------------------------------------------------------------------------------
/**
 * Adds a Hardware object to the configuration.
 *
 * @param hw Pointer to the Hardware object.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddHardware(Hardware *hw)
{
   if (hw == NULL)
      throw ConfigManagerException("Cannot add NULL Hardware object");
   
   std::string name = hw->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   
   if (!hw->IsOfType(Gmat::HARDWARE))
      throw ConfigManagerException(name + " is not Hardware");
   
   AddObject(hw);
}

//------------------------------------------------------------------------------
// void AddFieldOfView(FieldOfView *fov)
//------------------------------------------------------------------------------
/**
 * Adds a FieldOfView object to the configuration.
 *
 * @param fov Pointer to the FieldOfView object.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddFieldOfView(FieldOfView *fov)
{
   if (fov == NULL)
      throw ConfigManagerException("Cannot add NULL FieldOfView object");
   
   std::string name = fov->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   
   if (!fov->IsOfType(Gmat::FIELD_OF_VIEW))
      throw ConfigManagerException(name + " is not a FieldOfView");
   
   AddObject(fov);
}


//------------------------------------------------------------------------------
// void AddStopCondition(StopCondition* stopCond)
//------------------------------------------------------------------------------
/**
 * Adds a StopCondition to the configuration.
 *
 * @param stopCond Pointer to the StopCondition instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddStopCondition(StopCondition* stopCond)
{
   if (stopCond == NULL)
      throw ConfigManagerException("Cannot add NULL StopCondition object");

   std::string name = stopCond->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!stopCond->IsOfType(Gmat::STOP_CONDITION))
      throw ConfigManagerException(name + " is not a StopCondition");

   AddObject(stopCond);
}


//------------------------------------------------------------------------------
// void AddParameter(Parameter* parameter)
//------------------------------------------------------------------------------
/**
 * Adds a Parameter to the configuration.
 *
 * @param parameter Pointer to the Parameter instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddParameter(Parameter* parameter)
{
   if (parameter == NULL)
      throw ConfigManagerException("Cannot add NULL Parameter object");

   std::string name = parameter->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!parameter->IsOfType(Gmat::PARAMETER))
      throw ConfigManagerException(name + " is not a Parameter");

   AddObject(parameter);
}


//------------------------------------------------------------------------------
// void AddBurn(Burn* burn)
//------------------------------------------------------------------------------
/**
 * Adds a Burn to the configuration.
 *
 * @param burn Pointer to the Burn instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddBurn(Burn* burn)
{
   if (burn == NULL)
      throw ConfigManagerException("Cannot add Burn object");

   std::string name = burn->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!burn->IsOfType(Gmat::BURN))
      throw ConfigManagerException(name + " is not a Burn");

   AddObject(burn);
}

//------------------------------------------------------------------------------
// void AddSolver(Solver* solver)
//------------------------------------------------------------------------------
/**
 * Adds a Solver to the configuration.
 *
 * @param solver Pointer to the Solver instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddSolver(Solver* solver)
{
   if (solver == NULL)
      throw ConfigManagerException("Cannot add NULL Solver object");

   std::string name = solver->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!solver->IsOfType(Gmat::SOLVER))
      throw ConfigManagerException(name + " is not a Solver");

   AddObject(solver);
}

//------------------------------------------------------------------------------
// void AddAtmosphereModel(AtmosphereModel* atmosModel)
//------------------------------------------------------------------------------
/**
 * Adds an AtmosphereModel to the configuration.
 *
 * @param pm atmosModel to the AtmosphereModel instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddAtmosphereModel(AtmosphereModel* atmosModel)
{
   if (atmosModel == NULL)
      throw ConfigManagerException("Cannot add NULL AtmosphereModel object");

   std::string name = atmosModel->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!atmosModel->IsOfType(Gmat::ATMOSPHERE))
      throw ConfigManagerException(name + " is not an AtmosphereModel");

   AddObject(atmosModel);
}

//------------------------------------------------------------------------------
// void AddFunction(Function* function)
//------------------------------------------------------------------------------
/**
 * Adds a Function to the configuration.
 *
 * @param function Pointer to the Function instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddFunction(Function* function)
{
   if (function == NULL)
      throw ConfigManagerException("Cannot add NULL Function object");

   std::string name = function->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   
   if (!function->IsOfType(Gmat::FUNCTION))
      throw ConfigManagerException(name + " is not a Function");

   AddObject(function);
}

//------------------------------------------------------------------------------
// void AddCoordinateSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Adds a CoordinateSystem to the configuration.
 *
 * @param cs Pointer to the CoordinateSystem instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddCoordinateSystem(CoordinateSystem *cs)
{
   if (cs == NULL)
      throw ConfigManagerException("Cannot add NULL CoordinateSystem object");

   std::string name = cs->GetName();

   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!cs->IsOfType(Gmat::COORDINATE_SYSTEM))
      throw ConfigManagerException(name + " is not a CoordinateSystem");

   AddObject(cs);
}

//------------------------------------------------------------------------------
// void AddCalculatedPoint(CalculatedPoint *cp)
//------------------------------------------------------------------------------
/**
 * Adds a CalculatedPoint to the configuration.
 *
 * @param cp Pointer to the CalculatedPoint instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddCalculatedPoint(CalculatedPoint *cp)
{
   if (cp == NULL)
      throw ConfigManagerException("Cannot add NULL CalculatedPoint object");

   std::string name = cp->GetName();

   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!cp->IsOfType(Gmat::CALCULATED_POINT))
      throw ConfigManagerException(name + " is not a CalculatedPoint");

   AddObject(cp);
}

//------------------------------------------------------------------------------
// void AddMeasurementModel(MeasurementModel *mModel)
//------------------------------------------------------------------------------
/**
 * Adds a MeasurementModel to the configuration.
 *
 * @param mModel Pointer to the MeasurementModel instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddMeasurementModel(MeasurementModelBase *mModel)
{
   if (mModel == NULL)
      throw ConfigManagerException("Cannot add NULL MeasurementModel object");

   GmatBase *obj = (GmatBase*)mModel;

   std::string name = obj->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!obj->IsOfType(Gmat::MEASUREMENT_MODEL))
      throw ConfigManagerException(name + " is not a MeasurementModel");

   AddObject(obj);
}

//////------------------------------------------------------------------------------
////// void AddTrackingSystem(TrackingSystem *ts)
//////------------------------------------------------------------------------------
/////**
//// * Adds a TrackingSystem to the configuration.
//// *
//// * @param ts Pointer to the TrackingSystem instance.
//// */
//////------------------------------------------------------------------------------
////void ConfigManager::AddTrackingSystem(TrackingSystem *ts)
////{
////   if (ts == NULL)
////      throw ConfigManagerException("Cannot add NULL TrackingSystem object");
////
////   GmatBase *obj = (GmatBase*)ts;
////
////   std::string name = obj->GetName();
////   if (name == "")
////      throw ConfigManagerException("Unnamed objects cannot be managed");
////
////   if (!obj->IsOfType(Gmat::TRACKING_SYSTEM))
////      throw ConfigManagerException(name + " is not a TrackingSystem");
////
////   AddObject(obj);
////}
////
//////------------------------------------------------------------------------------
////// void AddTrackingData(TrackingData *td)
//////------------------------------------------------------------------------------
/////**
//// * Adds a TrackingData object to the configuration.
//// *
//// * @param td Pointer to the TrackingData instance.
//// */
//////------------------------------------------------------------------------------
////void ConfigManager::AddTrackingData(TrackingData *td)
////{
////   if (td == NULL)
////      throw ConfigManagerException("Cannot add NULL TrackingData object");
////
////   GmatBase *obj = (GmatBase*)td;
////
////   std::string name = obj->GetName();
////   if (name == "")
////      throw ConfigManagerException("Unnamed objects cannot be managed");
////
////   if (!obj->IsOfType(Gmat::TRACKING_DATA))
////      throw ConfigManagerException(name + " is not a TrackingData object");
////
////   AddObject(obj);
////}


//////------------------------------------------------------------------------------
////// void AddMeasurementModel(MeasurementModel *mModel)
//////------------------------------------------------------------------------------
/////**
//// * Adds a MeasurementModel to the configuration.
//// *
//// * @param mModel Pointer to the MeasurementModel instance.
//// */
//////------------------------------------------------------------------------------
////void ConfigManager::AddMeasurement(CoreMeasurement *meas)
////{
////   if (meas == NULL)
////      throw ConfigManagerException("Cannot add NULL Measurement object");
////
////   GmatBase *obj = (GmatBase*)meas;
////   std::string name = obj->GetName();
////   if (name == "")
////      throw ConfigManagerException("Unnamed objects cannot be managed");
////
////   if (!obj->IsOfType(Gmat::CORE_MEASUREMENT))
////      throw ConfigManagerException(name + " is not a Measurement");
////
////   MessageInterface::ShowMessage("Warning: Core measurement %s configured; it "
////         "should be hidden inside of a MeasurementModel", name.c_str());
////   AddObject(obj);
////}


//------------------------------------------------------------------------------
// void AddErrorModel(ErrorModel *errorModel)
//------------------------------------------------------------------------------
/**
 * Adds a ErrorModel to the configuration.
 *
 * @param errorModel Pointer to the ErrorModel instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddErrorModel(ErrorModel *errorModel)
{
   if (errorModel == NULL)
      throw ConfigManagerException("Cannot add NULL ErrorModel object");

   GmatBase *obj = (GmatBase*)errorModel;
   std::string name = obj->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!obj->IsOfType(Gmat::ERROR_MODEL))
      throw ConfigManagerException(name + " is not an ErrorModel");

   AddObject(obj);
}


//------------------------------------------------------------------------------
// void AddDataFilter(DataFilter *filter)
//------------------------------------------------------------------------------
/**
 * Adds a DataFilter to the configuration.
 *
 * @param filter Pointer to the DataFilter instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddDataFilter(DataFilter *filter)
{
   if (filter == NULL)
      throw ConfigManagerException("Cannot add NULL DataFilter object");

   GmatBase *obj = (GmatBase*)filter;
   std::string name = obj->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!obj->IsOfType(Gmat::DATA_FILTER))
      throw ConfigManagerException(name + " is not a DataFilter");

   AddObject(obj);
}


//------------------------------------------------------------------------------
// void AddDataFile(DataFile *df)
//------------------------------------------------------------------------------
/**
 * Adds a DataFile to the configuration
 *
 * @param df The DataFile object that is being added to the configuration
 */
//------------------------------------------------------------------------------
void ConfigManager::AddDataFile(DataFile *df)
{
   if (df == NULL)
      throw ConfigManagerException("Cannot add NULL DataFile object");

   GmatBase *obj = (GmatBase*)df;

   std::string name = obj->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if ((!obj->IsOfType(Gmat::DATA_FILE)) && (!obj->IsOfType(Gmat::DATASTREAM)))
      throw ConfigManagerException(name + " is not a DataFile or DataStream");

   AddObject(obj);
}


//------------------------------------------------------------------------------
// void AddObType(ObType *ot)
//------------------------------------------------------------------------------
/**
 * Adds a named ObType to the configuration
 *
 * @param ot The ObType that is being added
 */
//------------------------------------------------------------------------------
void ConfigManager::AddObType(ObType *ot)
{
   if (ot == NULL)
      throw ConfigManagerException("Cannot add NULL ObType object");

   GmatBase *obj = (GmatBase*)ot;

   std::string name = obj->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!obj->IsOfType(Gmat::OBTYPE))
      throw ConfigManagerException(name + " is not an ObType");

   MessageInterface::ShowMessage("Warning: ObType %s configured; it "
         "should be hidden inside of a DataFile", name.c_str());
   AddObject(obj);
}


//------------------------------------------------------------------------------
// void AddEventLocator(EventLocator *el)
//------------------------------------------------------------------------------
/**
 * Adds a named EventLocator to the configuration
 *
 * @param el The EventLocator that is being added
 */
//------------------------------------------------------------------------------
void ConfigManager::AddEventLocator(EventLocator *el)
{
   if (el == NULL)
      throw ConfigManagerException("Cannot add NULL EventLocator object");

   GmatBase *obj = (GmatBase*)el;

   std::string name = obj->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!obj->IsOfType(Gmat::EVENT_LOCATOR))
      throw ConfigManagerException(name + " is not an EventLocator");

   AddObject(obj);
}


//------------------------------------------------------------------------------
// void AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds an object to the configuration.
 *
 * @param obj Pointer to the object instance
 */
//------------------------------------------------------------------------------
void ConfigManager::AddObject(GmatBase *obj)
{
   if (obj == NULL)
      throw ConfigManagerException("Cannot add NULL Object");

   std::string name = obj->GetName();
   
   #ifdef DEBUG_CONFIG_ADD
   MessageInterface::ShowMessage
      ("ConfigManager::AddObject() adding <%p><%s> '%s'\n", obj,
       obj->GetTypeName().c_str(), name.c_str());
   #endif
   
   if (mapping.find(name) != mapping.end())
   {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else
   {
      objects.push_back(obj);
      mapping[name] = obj;
   }
   
   // Object was added, so set configuration changed to true.
   // Until we can add TextEphemFile to resource tree, we don't want to
   // write to script file on save script. (LOJ: 2007.04.07)
   // We can ignore BuiltinGmatFunction such as GetEphemState since it is
   // added internally. (LOJ: 2016.08.31)
   if ((obj->GetTypeName() != "TextEphemFile") &&
       (!obj->IsOfType("BuiltinGmatFunction")))
   {
      #ifdef DEBUG_CONFIG_CHANGE
      MessageInterface::ShowMessage
         ("ConfigManager::AddObject() Object <%p><%s>'%s'was added, so setting "
          "configuration changed to true\n", obj, obj->GetTypeName().c_str(),
          obj->GetName().c_str());
      #endif
      configChanged = true;
   }
   
} // AddObject(GmatBase *obj)


//------------------------------------------------------------------------------
// void SetDefaultSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the default SolarSystem.
 *
 * @param ss The SolarSystem object pointer.
 *
 */
//------------------------------------------------------------------------------
void ConfigManager::SetDefaultSolarSystem(SolarSystem *ss)
{
   defaultSolarSystem = ss;
}


//------------------------------------------------------------------------------
// void SetSolarSystemInUse(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the current SolarSystem.
 *
 * @param ss The SolarSystem object pointer.
 *
 */
//------------------------------------------------------------------------------
void ConfigManager::SetSolarSystemInUse(SolarSystem *ss)
{
   #if DEBUG_CONFIG_SS
   MessageInterface::ShowMessage
      ("ConfigManager::SetSolarSystemInUse() name=%s\n", ss->GetName().c_str());
   #endif
   
   solarSystemInUse = ss;
}


//------------------------------------------------------------------------------
// bool SetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the name for the current SolarSystem.
 *
 * @param name The SolarSystem name.
 *
 * @note This method is not yet used in GMAT.
 */
//------------------------------------------------------------------------------
bool ConfigManager::SetSolarSystemInUse(const std::string &name)
{
   throw ConfigManagerException
      ("ConfigManager::SetSolarSystemInUse(name) has not been implemented.\n");
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfAllItems()
//------------------------------------------------------------------------------
/**
 * Retrieves a list of all configured objects.
 *
 * @return The list of objects.
 */
//------------------------------------------------------------------------------
const StringArray& ConfigManager::GetListOfAllItems()
{
   listOfItems.erase(listOfItems.begin(), listOfItems.end());
    
   std::vector<GmatBase*>::iterator current =
      (std::vector<GmatBase*>::iterator)(objects.begin());
   
   while (current != (std::vector<GmatBase*>::iterator)(objects.end()))
   {
      listOfItems.push_back((*current)->GetName());
      ++current;
   }
   return listOfItems;
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfItemsHas(UnsignedInt type, const std::string &name,
//                                      bool includeSysParam)
//------------------------------------------------------------------------------
/**
 * Checks a specific item used in anywhere.
 *
 * @param type The type of the object that is being checked.
 * @param name The name of the object.
 * @param includeSysParam True if system parameter to be included [true]
 *
 * @return array of item names where the name is used.
 */
//------------------------------------------------------------------------------
const StringArray& ConfigManager::GetListOfItemsHas(UnsignedInt type,
                                                    const std::string &name,
                                                    bool includeSysParam)
{
   StringArray items = GetListOfAllItems();
   std::string::size_type pos;
   GmatBase *obj;
   std::string objName;
   std::string objString;
   StringArray genStringArray;
   static StringArray itemList;
   itemList.clear();
   
   #if DEBUG_FIND_ITEMS
   MessageInterface::ShowMessage
      ("ConfigManager::GetListOfItemsHas() type=%d, name='%s', includeSysParam=%d\n",
       type, name.c_str(), includeSysParam);
   #endif
   
   try
   {
      for (UnsignedInt itemIndex=0; itemIndex<items.size(); itemIndex++)
      {
         obj = GetItem(items[itemIndex]);
         
         #if DEBUG_FIND_ITEMS
         MessageInterface::ShowMessage
            ("===> obj[%d]=%s, %s\n", itemIndex, obj->GetTypeName().c_str(),
             obj->GetName().c_str());
         #endif
         
         // if same type and name, skip
         // Added to check for the same name since FuelTank and Thruster are
         // both HARDWARE type to fix bug 2314 (LOJ: 2011.01.19)
         if (obj->IsOfType(type) && obj->GetName() == name)
            continue;
         
         bool isSystemParam = false;
         if (obj->IsOfType(Gmat::PARAMETER))
            if (((Parameter*)obj)->GetKey() == GmatParam::SYSTEM_PARAM)
               isSystemParam = true;
         
         // if system parameter not to be included, skip
         if (!includeSysParam && isSystemParam)
            continue;
         
         objName = obj->GetName();
         
         // We need to check names in RHS of equal sign so use GetGeneratingStringArray
         // This fixes bug 2222 (LOJ: 2010.12.01) 
         //objString = obj->GetGeneratingString();
         //pos = objString.find(name);
         
         #if DEBUG_FIND_ITEMS
         MessageInterface::ShowMessage
            ("   ==> objName='%s', Calling obj->GetGeneratingStringArray()\n", objName.c_str());
         #endif
         
         genStringArray = obj->GetGeneratingStringArray(Gmat::NO_COMMENTS);
         
         #if DEBUG_FIND_ITEMS
         MessageInterface::ShowMessage("   genStringArray.size()=%d\n", genStringArray.size());
         for (UnsignedInt ii = 0; ii < genStringArray.size(); ii++)
            MessageInterface::ShowMessage
               ("      genStringArray[%d]='%s'\n", ii, genStringArray[ii].c_str());
         #endif
         
         if (genStringArray.empty())
         {
            // Add Parameters to list (LOJ: 2011.01.11 - to fix bug 2321)
            if (obj->IsOfType(Gmat::PARAMETER))
            {
               objString = obj->GetGeneratingString();
               #if DEBUG_FIND_ITEMS
               MessageInterface::ShowMessage
                  ("   objString='%s'\n", objString.c_str());
               #endif
               pos = objString.find(name);
               if (pos != objString.npos)
               {
                  #if DEBUG_FIND_ITEMS
                  MessageInterface::ShowMessage
                     ("   ==> '%s' found in Parameter, so adding '%s'\n", name.c_str(),
                      objName.c_str());
                  #endif
                  itemList.push_back(objName);
               }
            }
         }
         else
         {
            std::string rhsString;
            for (StringArray::iterator iter = genStringArray.begin();
                 iter < genStringArray.end(); iter++)
            {
               objString = *iter;
               StringArray parts = GmatStringUtil::SeparateBy(objString, "=");
               
               #if DEBUG_FIND_ITEMS
               MessageInterface::ShowMessage("   parts.size()=%d\n", parts.size());
               for (UnsignedInt j = 0; j < parts.size(); j++)
                  MessageInterface::ShowMessage
                     ("      parts[%d]='%s'\n", j, parts[j].c_str());
               #endif
               
               if (parts.size() > 1)
               {
                  rhsString = parts[1];
                  rhsString = GmatStringUtil::Trim(rhsString, GmatStringUtil::BOTH, true, true);
                  
                  #if DEBUG_FIND_ITEMS
                  MessageInterface::ShowMessage
                     ("   objString='%s', rhsString='%s'\n", objString.c_str(),
                      rhsString.c_str());
                  #endif
                  
                  pos = rhsString.find(name);
                  std::string nameDot = name + ".";   // for Parameter or object property
                  std::string nameParen = name + "("; // for array element
                  if (pos != rhsString.npos)
                  {
                     // Add to list if name not found in string enclosed with single quote
                     if (!GmatStringUtil::IsEnclosedWith(rhsString, "'"))
                     {
                        #if DEBUG_FIND_ITEMS
                        MessageInterface::ShowMessage
                           ("   Item name found and it is not enclosed with quotes, so checking further\n");
                        #endif
                        // Check if it is a whole name or name with dot such as "Sat1." (LOJ: 2012.02.17)
                        // Remove {} first and parse by comma
                        std::string rhs = GmatStringUtil::RemoveOuterString(rhsString, "{", "}");
                        StringArray parts = GmatStringUtil::SeparateByComma(rhs);
                        for (UnsignedInt i = 0; i < parts.size(); i++)
                        {
                           #if DEBUG_FIND_ITEMS
                           MessageInterface::ShowMessage("   Checking '%s'\n", parts[i].c_str());
                           #endif
                           
                           if (parts[i] == name || parts[i].find(nameDot) != rhs.npos ||
                               parts[i].find(nameParen) != rhs.npos)
                           {
                              #if DEBUG_FIND_ITEMS
                              MessageInterface::ShowMessage
                                 ("   ==> '%s' found in RHS, so adding object name '%s'\n", name.c_str(), objName.c_str());
                              #endif
                              itemList.push_back(objName);
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage("*** Error: %s\n", e.GetFullMessage().c_str());
      throw;
   }
   
   return itemList;
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfItems(UnsignedInt itemType)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of all configured objects of a given type.
 *
 * @param itemType The type of object requested.
 * @param typeMax  Upper range value when a range is requested
 *
 * @return The list of objects.
 */
//------------------------------------------------------------------------------
const StringArray& ConfigManager::GetListOfItems(UnsignedInt itemType,
      UnsignedInt typeMax)
{
   listOfItems.erase(listOfItems.begin(), listOfItems.end());
   if (typeMax == 0)
      typeMax = itemType;
    
   std::vector<GmatBase*>::iterator current =
      (std::vector<GmatBase*>::iterator)(objects.begin());
   while (current != (std::vector<GmatBase*>::iterator)(objects.end()))
   {
      if (itemType <= Gmat::USER_OBJECT_ID_NEEDED)
      {
         UnsignedInt theType = (*current)->GetType();
         if ((theType >= itemType) && (theType <= typeMax))
            listOfItems.push_back((*current)->GetName());
         else
            if ((*current)->IsOfType(itemType))
               listOfItems.push_back((*current)->GetName());
      }
      else if ((*current)->GetType() > Gmat::USER_OBJECT_ID_NEEDED)
            listOfItems.push_back((*current)->GetName());
      ++current;
   }
   return listOfItems;
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfItems(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of all configured objects of a given type name.
 *
 * @param typeName The type name of object requested.
 *
 * @return The list of objects.
 */
//------------------------------------------------------------------------------
const StringArray& ConfigManager::GetListOfItems(const std::string &typeName)
{
   listOfItems.clear();
   
   std::vector<GmatBase*>::iterator current =
      (std::vector<GmatBase*>::iterator)(objects.begin());
   while (current != (std::vector<GmatBase*>::iterator)(objects.end()))
   {
      if ((*current)->IsOfType(typeName))
         listOfItems.push_back((*current)->GetName());
      ++current;
   }
   return listOfItems;
}


//------------------------------------------------------------------------------
// GmatBase* AddClone(const std::string &name, std::string &cloneName)
//------------------------------------------------------------------------------
/*
 * Adds the clone of the named object to configuration.
 * It gives new name by adding counter to the name to be cloned.
 *
 * @parameter  name  The name of the object to be cloned
 * @parameter  cloneName  Name of the cloned object if object was cloned and
 *                added to configuration, blank otherwise
 *
 * @return  Cloned object pointer, NULL if it was not cloned
 */
//------------------------------------------------------------------------------
GmatBase* ConfigManager::AddClone(const std::string &name, std::string &cloneName)
{
   if (name == "")
      return NULL;
   
   GmatBase *obj1 = GetItem(name);
   std::string newName = GetNewName(name, 2);
   
   GmatBase* obj2 = obj1->Clone();
   obj2->SetName(newName, newName);
   AddObject(obj2);
   cloneName = newName;
   
   #if DEBUG_CONFIG_ADD_CLONE
   MessageInterface::ShowMessage
      ("ConfigManager::AddClone() cloneName = %s, type = %s\n", obj2->GetName().c_str(),
       obj2->GetTypeName().c_str());
   #endif
   
   return obj2;
}


//------------------------------------------------------------------------------
// GmatBase* GetFirstItemUsing(UnsignedInt type, const std::string &name,
//                            bool includeSysParam = true);
//------------------------------------------------------------------------------
/**
 * Retrives first object that uses given object type and name.
 *
 * @param type The type of the object that is being checked.
 * @param name The name of the object.
 * @param includeSysParam True if system parameter to be included
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* ConfigManager::GetFirstItemUsing(UnsignedInt type,
                                           const std::string &name,
                                           bool includeSysParam)
{
   #ifdef DEBUG_CONFIG_OBJ_USING
   MessageInterface::ShowMessage
      ("ConfigManager::GetFirstItemUsing() type=%d, name='%s', includeSysParam=%d\n",
       type, name.c_str(), includeSysParam);
   #endif
   
   StringArray objList = GetListOfItemsHas(type, name, includeSysParam);
   
   #ifdef DEBUG_CONFIG_OBJ_USING
   MessageInterface::ShowMessage
      ("   There are %d objects using '%s'\n", objList.size(), name.c_str());
   #endif
   
   GmatBase *obj = NULL;
   
   for (UnsignedInt i = 0; i < objList.size(); i++)
   {
      obj = GetItem(objList[i]);
      
      #ifdef DEBUG_CONFIG_OBJ_USING
      MessageInterface::ShowMessage
         ("      obj = <%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(),
          obj->GetName().c_str());
      #endif
      
      if (obj->GetName() != name)
         break;
   }
   
   #ifdef DEBUG_CONFIG_OBJ_USING
   MessageInterface::ShowMessage
      ("ConfigManager::GetFirstItemUsing() returning <%p>\n", obj);
   #endif
   
   return obj;
}


//------------------------------------------------------------------------------
// GmatBase* GetItem(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves an object by name.
 *
 * @param name The name of the object requested.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* ConfigManager::GetItem(const std::string &name)
{
   #ifdef DEBUG_CONFIG_GET_ITEM
   MessageInterface::ShowMessage("ConfigManager::GetItem() name='%s'\n", name.c_str());
   #endif
   
   GmatBase *obj = NULL;
   
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->GetName() == name)
      {
         obj = mapping[name];
      }
   }
   
   #ifdef DEBUG_CONFIG_GET_ITEM
   MessageInterface::ShowMessage("===> ConfigManager::GetItem() returning <%p>\n", obj);
   #endif
   
   return obj;
}

//------------------------------------------------------------------------------
// bool ChangeMappingName(UnsignedInt itemType, const std::string &oldName,
//                        const std::string &newName, GmatBase *mapObj)
//------------------------------------------------------------------------------
bool ConfigManager::ChangeMappingName(UnsignedInt itemType,
                                      const std::string &oldName,
                                      const std::string &newName,
                                      GmatBase **mapObj)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ConfigManager::ChangeMappingName() itemType=%d, oldName='%s', newName='%s'\n",
       itemType, oldName.c_str(), newName.c_str());
   #endif
   
   //--------------------------------------------------
   // change mapping name
   //--------------------------------------------------
   GmatBase *cfObj = NULL;
   bool renamed = false;
   if (mapping.find(oldName) != mapping.end())
   {
      cfObj = mapping[oldName];
      
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("   cfObj=<%p><%s>'%s'\n", cfObj, cfObj ? cfObj->GetTypeName().c_str() : "NULL",
          cfObj ? cfObj->GetName().c_str() : "NULL");
      #endif
      if (cfObj->IsOfType(itemType))
      {
         // if newName does not exist, change name
         if (mapping.find(newName) == mapping.end())
         {
            mapping.erase(oldName);
            mapping[newName] = cfObj;
            cfObj->SetName(newName);
            renamed = true;
            #if DEBUG_RENAME
            MessageInterface::ShowMessage
               ("   Rename mapping obj=%s\n", cfObj->GetName().c_str());
            #endif
         }
         else
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "%s already exists, Please enter a different name.\n",
                newName.c_str());
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("ConfigManager::ChangeMappingName() oldName has different type:%d\n",
             cfObj->GetType());
      }
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ConfigManager::ChangeMappingName() returning %d\n", renamed);
   #endif
   
   *mapObj = cfObj;
   return renamed;
}

//------------------------------------------------------------------------------
// bool RenameItem(UnsignedInt type, const std::string &oldName,
//                 const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Changes the name for a configured object.
 *
 * @param type The type of object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The new name for the object.
 *
 * @return true if the object was renamed, false if not.
 */
//------------------------------------------------------------------------------
bool ConfigManager::RenameItem(UnsignedInt itemType,
                               const std::string &oldName,
                               const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ConfigManager::RenameItem() itemType=%d, oldName='%s', newName='%s'\n",
       itemType, oldName.c_str(), newName.c_str());
   #endif
   
   #ifdef DUMP_CONFIGURED_OBJECTS
      MessageInterface::ShowMessage("Before rename, Configured objects:\n");
      for (UnsignedInt i = 0; i < objects.size(); ++i)
         MessageInterface::ShowMessage("   %s\n",
               objects[i]->GetName().c_str());
      for (UnsignedInt i = 0; i < newObjects.size(); ++i)
         MessageInterface::ShowMessage("   N:   %s\n",
               newObjects[i]->GetName().c_str());
   #endif
   
   changedItemType.clear();
   oldRelatedName.clear();
   newRelatedName.clear();
   GmatBase *mapObj = NULL;
   
   bool renamed = ChangeMappingName(itemType, oldName, newName, &mapObj);
   
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   mapObj=<%p><%s>'%s'\n", mapObj, mapObj ? mapObj->GetTypeName().c_str() : "NULL",
       mapObj ? mapObj->GetName().c_str() : "NULL");
   #endif
   
   if (!renamed)
   {
      #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("ConfigManager::RenameItem() Unable to rename: oldName not found.\n");
      #endif
      return false;
   }
   
   //----------------------------------------------------
   // Rename ref. objects
   //----------------------------------------------------
   GmatBase *obj = NULL;
   StringArray itemList;
   
   try
   {
      itemList = GetListOfItemsHas(itemType, oldName);
   }
   catch(BaseException &be)
   {
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("ConfigManager::RenameItem() Exception Caught so re-changed the name and rethrowing:\n%s\n",
          be.GetFullMessage().c_str());
      #endif
      // Change back name
      GmatBase *mapObj2 = NULL;
      bool renamed = ChangeMappingName(itemType, newName, oldName, &mapObj2);
      throw;
   }
   
   #if DEBUG_RENAME
      MessageInterface::ShowMessage("   =====> There are %d items has '%s'\n",
            itemList.size(), oldName.c_str());
   #endif
   for (UnsignedInt i=0; i<itemList.size(); i++)
   {
      obj = GetItem(itemList[i]);
      if (obj)
      {
         #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("      calling  %s->RenameRefObject()\n", obj->GetName().c_str());
         #endif
         
         renamed = obj->RenameRefObject(itemType, oldName, newName);
      }
   }
   
   //----------------------------------------------------
   // Rename owned object ODEModel in the PropSetup.
   //----------------------------------------------------
   if (itemType == Gmat::PROP_SETUP)
   {
      bool newFMName = false;
      // Change _ForceMode name if _ForceModel is configured
      std::string oldFmName = oldName + "_ForceModel";
      std::string newFmName = newName + "_ForceModel";
      // Rename the ODEModel if it is associated with and named by the PropSetup
      if ((mapping.find(oldFmName) != mapping.end()) &&
          (mapObj->GetStringParameter("FM") == oldFmName))
      {
         GmatBase *propSetup = mapObj;
         mapObj = mapping[oldFmName];
         // if newName does not exist, change name
         if (mapping.find(newFmName) == mapping.end())
         {
            mapping.erase(oldFmName);
            mapping[newFmName] = mapObj;
            mapObj->SetName(newFmName);

            // Update the prop setup with the new name
            propSetup->SetStringParameter("FM", newFmName);
            propSetup->RenameRefObject(mapObj->GetType(), oldFmName, newFmName);
            newFMName = true;
            
            #if DEBUG_RENAME
            MessageInterface::ShowMessage
               ("   Rename mapping mapObj=%s\n", mapObj->GetName().c_str());
            #endif
         }
      }
      
      #if DEBUG_RENAME
      MessageInterface::ShowMessage("   Calling PropSetup::RenameRefObject()\n");
      #endif
      mapObj->RenameRefObject(itemType, oldName, newName);

      if (newFMName)
      {
         // Call to reset references to the force model by name
         #if DEBUG_RENAME
            MessageInterface::ShowMessage("      *** Force model name changed "
                  "from %s to %s; making call to handle that piece\n",
                  oldFmName.c_str(), newFmName.c_str());
         #endif
         //----------------------------------------------------
         // Rename ref. objects using FM
         //----------------------------------------------------
         GmatBase *obj = NULL;
         StringArray itemList = GetListOfItemsHas(Gmat::ODE_MODEL, oldFmName);
         #if DEBUG_RENAME
            MessageInterface::ShowMessage("      =====> %d items have '%s'\n",
                  itemList.size(), oldFmName.c_str());
         #endif
         for (UnsignedInt i=0; i<itemList.size(); i++)
         {
            obj = GetItem(itemList[i]);
            if (obj)
            {
               #if DEBUG_RENAME
               MessageInterface::ShowMessage
                  ("      calling  %s->RenameRefObject(%d, %s, %s)\n",
                   obj->GetName().c_str(), Gmat::ODE_MODEL, oldFmName.c_str(),
                   newFmName.c_str());
               #endif

               renamed = obj->RenameRefObject(Gmat::ODE_MODEL, oldFmName,
                               newFmName);

               if (newFmName != oldFmName)
               {
                  changedItemType.push_back(Gmat::ODE_MODEL);
                  oldRelatedName.push_back(oldFmName);
                  newRelatedName.push_back(newFmName);
               }

            }
         }

         StringArray params = GetListOfItems(Gmat::PARAMETER);
         std::string oldParamName, newParamName;
         Parameter *param;
         std::string::size_type pos;

         #if DEBUG_RENAME
         MessageInterface::ShowMessage("   Now going through all Parameters "
               "in the map\n");
         #endif
         for (unsigned int i=0; i<params.size(); i++)
         {
            #if DEBUG_RENAME
            MessageInterface::ShowMessage("   params[%2d]=%s\n", i,
                  params[i].c_str());
            #endif

            param = GetParameter(params[i]);
            // if system parameter, change its name
            if (param->GetKey() == GmatParam::SYSTEM_PARAM)
            {
               oldParamName = param->GetName();
               pos = oldParamName.find(oldFmName);
               // Rename actual parameter name
               if (pos != oldParamName.npos)
               {
                  newParamName = oldParamName;
                  newParamName = GmatStringUtil::ReplaceName(oldParamName,
                        oldFmName, newFmName);

                  #if DEBUG_RENAME
                  MessageInterface::ShowMessage
                     ("   oldParamName=%s, newParamName=%s\n",
                      oldParamName.c_str(), newParamName.c_str());
                  #endif

                  // Change parameter mapping name
                  if (mapping.find(oldParamName) != mapping.end())
                  {
                     mapping.erase(oldParamName);
                     mapping[newParamName] = (GmatBase*)param;
                     // Give a Parameter new name
                     param->SetName(newParamName, oldParamName);
                     renamed = true;
                  }

                  if (newParamName != oldParamName)
                  {
                     param->RenameRefObject(Gmat::ODE_MODEL, oldFmName,
                           newFmName);
                     changedItemType.push_back(Gmat::PARAMETER);
                     oldRelatedName.push_back(oldParamName);
                     newRelatedName.push_back(newParamName);
                  }
               }
            }
            // If variable, need to change expression
            else if (param->GetTypeName() == "Variable")
            {
               param->RenameRefObject(Gmat::PARAMETER, oldFmName, newFmName);
            }
         }
      }
   }
   
   //----------------------------------------------------
   // Rename owned tanks in the thrusters.
   // Tank is ReadOnly parameter so it does show in
   // GeneratingString()
   //----------------------------------------------------
   
   if (itemType == Gmat::HARDWARE)
   {
      itemList = GetListOfItems(Gmat::HARDWARE);
      
      for (unsigned int i=0; i<itemList.size(); i++)
      {
         obj = GetItem(itemList[i]);
         if (obj->IsOfType("Thruster"))
             obj->RenameRefObject(itemType, oldName, newName);
      }
   }
   
   //----------------------------------------------------
   // Rename system parameters and expression of variables
   //----------------------------------------------------
   // Since new hardware Parameters were added, check for the Hardware also.
   
   if (itemType == Gmat::SPACECRAFT || itemType == Gmat::COORDINATE_SYSTEM || 
		 itemType == Gmat::PLATE || 
       itemType == Gmat::CALCULATED_POINT || itemType == Gmat::BURN ||
       itemType == Gmat::IMPULSIVE_BURN || itemType == Gmat::HARDWARE ||
       itemType == Gmat::THRUSTER || itemType == Gmat::FUEL_TANK ||
       itemType == Gmat::ODE_MODEL)
   {
      StringArray params = GetListOfItems(Gmat::PARAMETER);
      std::string oldParamName, newParamName;
      Parameter *param;
      std::string::size_type pos;
      
      #if DEBUG_RENAME
      MessageInterface::ShowMessage("   Now going through all Parameters in the map\n");
      #endif
      for (unsigned int i=0; i<params.size(); i++)
      {
         #if DEBUG_RENAME
         MessageInterface::ShowMessage("   params[%2d]=%s\n", i, params[i].c_str());
         #endif
         
         param = GetParameter(params[i]);
         
         // if system parameter, change it's own name
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            oldParamName = param->GetName();
            pos = oldParamName.find(oldName);
            
            // Rename actual parameter name
            if (pos != oldParamName.npos)
            {
               newParamName = oldParamName;
               newParamName = GmatStringUtil::ReplaceName(oldParamName, oldName,
                     newName);
               
               #if DEBUG_RENAME
               MessageInterface::ShowMessage
                  ("   oldParamName=%s, newParamName=%s\n",
                   oldParamName.c_str(), newParamName.c_str());
               #endif
               
               // Change parameter mapping name
               if (mapping.find(oldParamName) != mapping.end())
               {
                  mapping.erase(oldParamName);
                  mapping[newParamName] = (GmatBase*)param;
                  // Give a Parameter new name
                  param->SetName(newParamName, oldParamName);
                  renamed = true;
               }
            }
         }
         // If variable, need to change expression
         else if (param->GetTypeName() == "Variable")
         {
            param->RenameRefObject(Gmat::PARAMETER, oldName, newName);
         }
      }
   }
   
   // Item was removed, so set configuration changed flag to true
   #ifdef DEBUG_CONFIG_CHANGE
   MessageInterface::ShowMessage
      ("ConfigManager::RenameItem() Item was removed, so setting configuration "
       "changed to true\n");
   #endif
   configChanged = true;
   
   #if DEBUG_RENAME
   StringArray allItems = GetListOfAllItems();
   MessageInterface::ShowMessage("   =====> After rename object\n");
   for (UnsignedInt i=0; i<allItems.size(); i++)
      MessageInterface::ShowMessage("   item[%2d] = %s\n", i, allItems[i].c_str());
   MessageInterface::ShowMessage
      ("ConfigManager::RenameItem() returning %d\n", renamed);
   #endif
   
   #ifdef DUMP_CONFIGURED_OBJECTS
      MessageInterface::ShowMessage("After rename, Configured objects:\n");
      for (UnsignedInt i = 0; i < objects.size(); ++i)
         MessageInterface::ShowMessage("   %s\n",
               objects[i]->GetName().c_str());
      for (UnsignedInt i = 0; i < newObjects.size(); ++i)
         MessageInterface::ShowMessage("   N:   %s\n",
               newObjects[i]->GetName().c_str());
   #endif

   return renamed;
} // RenameItem()


//------------------------------------------------------------------------------
// bool RemoveAllItems()
//------------------------------------------------------------------------------
/**
 * Removes all configured objects from memory
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool ConfigManager::RemoveAllItems()
{
   // delete objects
   #ifdef DEBUG_CONFIG_REMOVE
   MessageInterface::ShowMessage
      ("ConfigManager::RemoveAllItems() Deleting %d objects\n", objects.size());
   #endif
   
   for (unsigned int i=0; i<objects.size(); i++)
   {
      #ifdef DEBUG_CONFIG_REMOVE_MORE
      MessageInterface::ShowMessage
         ("   deleting <%p><%s>'%s'\n", objects[i], objects[i]->GetTypeName().c_str(),
          objects[i]->GetName().c_str());
      #endif
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (objects[i], objects[i]->GetName(), "ConfigManager::RemoveAllItems()",
          " deleting configured obj");
      #endif
      
      delete objects[i];
      objects[i] = NULL;
   }
   
   #ifdef DEBUG_CONFIG_REMOVE
   MessageInterface::ShowMessage("Deleting %d new objects\n", newObjects.size());
   #endif
   // delete objects that were reconfigured, ie, just object pointer reset in the map
   for (unsigned int i=0; i<newObjects.size(); i++)
   {
      #ifdef DEBUG_CONFIG_REMOVE_MORE
      MessageInterface::ShowMessage
         ("   deleting <%p><%s>'%s'\n", newObjects[i], newObjects[i]->GetTypeName().c_str(),
          newObjects[i]->GetName().c_str());
      #endif
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (newObjects[i], newObjects[i]->GetName(), "ConfigManager::RemoveAllItems()",
          " deleting configured obj");
      #endif
      
      delete newObjects[i];
      newObjects[i] = NULL;
   }
   
   objects.clear();
   newObjects.clear();
   mapping.clear();
   
   PluginItemManager::Instance()->ClearAllPluginItems();

   return true;
}


//------------------------------------------------------------------------------
// bool RemoveItem(UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Removes a specific item from memory.
 *
 * @param type The type of the object that is being removed.
 * @param name The name of the object.
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool ConfigManager::RemoveItem(UnsignedInt type, const std::string &name,
                               bool removeAssociatedSysParam)
{
   #ifdef DEBUG_CONFIG_REMOVE
   MessageInterface::ShowMessage
      ("ConfigManager::RemoveItem() entered, type=%d, typeString='%s', "
       "name='%s', removeAssociatedSysParam=%d\n", type,
       GmatBase::GetObjectTypeString(type).c_str(), name.c_str(),
       removeAssociatedSysParam);
   #endif
   
   bool status = false;
   
   #ifdef DEBUG_CONFIG_REMOVE
   MessageInterface::ShowMessage("   \nThere are %d objects\n", objects.size());
   #endif
   #ifdef DEBUG_CONFIG_REMOVE_MORE
   ObjectArray::iterator iter = objects.begin();
   while (iter != objects.end())
   {
      MessageInterface::ShowMessage
         ("   obj=<%p><%s>'%s'\n", (*iter), (*iter)->GetTypeName().c_str(),
          (*iter)->GetName().c_str());
      ++iter;
   }
   #endif
   
   // remove from objects
   ObjectArray::iterator objIter = objects.begin();
   while (objIter != objects.end())
   {
      GmatBase *obj = (*objIter);
      if (obj->IsOfType(type))
      {
         if (obj->GetName() == name)
         {
            #ifdef DEBUG_CONFIG_REMOVE
            MessageInterface::ShowMessage
               ("   Removing '%s' from objects\n", name.c_str());
            #endif
            objects.erase(objIter);
            break;
         }
      }
      ++objIter;
   }
   
   // Remove associated system Parameter from objects
   if (removeAssociatedSysParam)
   {
      #ifdef DEBUG_CONFIG_REMOVE
      MessageInterface::ShowMessage("   \nNow checking associated Parameters in objects\n");
      MessageInterface::ShowMessage("   There are %d objects\n", objects.size());
      #endif
      std::string objName;
      std::string nameToFind = name + ".";
      objIter = objects.begin();
      while (objIter != objects.end())
      {
         bool checkNext = true;
         GmatBase *obj = (*objIter);
         objName = obj->GetName();
         #ifdef DEBUG_CONFIG_REMOVE_MORE
         MessageInterface::ShowMessage
            ("   obj=<%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(),
             obj->GetName().c_str());
         #endif
         if (objName.find(nameToFind) != objName.npos)
         {
            if (obj->IsOfType(Gmat::PARAMETER))
            {
               #ifdef DEBUG_CONFIG_REMOVE
               MessageInterface::ShowMessage
                  ("   Removing '%s' from objects\n", objName.c_str());
               #endif
               objects.erase(objIter);
               checkNext = false;
            }
         }
         
         if (checkNext)
            ++objIter;
      }
   }
   
   // remove and delete from mapping
   #ifdef DEBUG_CONFIG_REMOVE
   MessageInterface::ShowMessage
      ("   \nThere are %d objects in the mapping\n", mapping.size());
   #endif
   
   if (mapping.find(name) != mapping.end())
   {
      GmatBase *obj = mapping[name];
      if (obj != NULL)
      {
         #ifdef DEBUG_CONFIG_REMOVE_MORE
         MessageInterface::ShowMessage
            ("   obj=<%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(),
             obj->GetName().c_str());
         #endif
         if (obj->IsOfType(type))
         {
            mapping.erase(name);
            #ifdef DEBUG_CONFIG_REMOVE
            MessageInterface::ShowMessage
               ("   Deleting '%s' from mapping\n", name.c_str());
            #endif
            
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (obj, name, "ConfigManager::RemoveItem()",
                "deleting object from mapping");
            #endif
            
            delete obj;
            status = true;
         }
      }
      else
      {
         #ifdef DEBUG_CONFIG_REMOVE
         MessageInterface::ShowMessage
            ("   The obj from the mapping is NULL\n");
         #endif
      }
   }
   
   // Remove and delete associated system Parameter from mapping
   if (removeAssociatedSysParam)
   {
      #ifdef DEBUG_CONFIG_REMOVE
      MessageInterface::ShowMessage("   \nNow checking associated Parameters in mapping\n");
      MessageInterface::ShowMessage("   There are %d objects in the mapping\n", mapping.size());
      #endif
      std::string objName;
      std::string nameToFind = name + ".";
      GmatBase *obj = NULL;
      ObjectMap::iterator mapIter = mapping.begin();
      while (mapIter != mapping.end())
      {
         bool checkNext = true;
         objName = mapIter->first;
         obj = mapIter->second;
         #ifdef DEBUG_CONFIG_REMOVE_MORE
         MessageInterface::ShowMessage
            ("   objName='%s', obj=<%p><%s>'%s'\n", objName.c_str(), obj,
             obj ? obj->GetTypeName().c_str() : "NULL",
             obj ? obj->GetName().c_str() : "NULL");
         #endif
         
         if (obj)
         {            
            if (objName.find(nameToFind) != objName.npos)
            {
               if (obj->IsOfType(Gmat::PARAMETER))
               {
                  #ifdef DEBUG_CONFIG_REMOVE
                  MessageInterface::ShowMessage
                     ("   Deleting '%s' from mapping\n", objName.c_str());
                  #endif
                  // Need to increment mapIter before erase
                  mapIter++;
                  mapping.erase(objName);
                  
                  #ifdef DEBUG_MEMORY
                  MemoryTracker::Instance()->Remove
                     (obj, objName, "ConfigManager::RemoveItem()",
                      "deleting object from mapping");
                  #endif
                  
                  #ifdef DEBUG_CONFIG_REMOVE
                  MessageInterface::ShowMessage("   Deleting obj<%p>\n", obj);
                  #endif
                  delete obj;
                  checkNext = false;
               }
            }
         }
         
         if (checkNext)
            mapIter++;
      }
   }
   
   // Item was removed, so set conguration changed flag to true
   #ifdef DEBUG_CONFIG_CHANGE
   MessageInterface::ShowMessage
      ("ConfigManager::RemoveItem() Item was removed, so setting configuration "
       "changed to true\n");
   #endif
   configChanged = true;
   
   #ifdef DEBUG_CONFIG_REMOVE
   MessageInterface::ShowMessage
      ("ConfigManager::RemoveItem() exiting, '%s' %s removed\n", name.c_str(),
       status ? "was" : "was not");
   #endif
   return status;
}


//------------------------------------------------------------------------------
// bool ReconfigureItem(GmatBase *newobj, const std::string &name)
//------------------------------------------------------------------------------
/*
 * Sets configured object pointer with new pointer.
 *
 * @param  newobj  New object pointer
 * @param  name  Name of the configured object to be reset
 *
 * @return  true if pointer was reset, false otherwise
 */
//------------------------------------------------------------------------------
bool ConfigManager::ReconfigureItem(GmatBase *newobj, const std::string &name)
{
   GmatBase *obj = NULL;
   
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->GetName() == name)
      {
         obj = mapping[name];
         
         #if DEBUG_CONFIG_RECONFIGURE
         MessageInterface::ShowMessage
            ("ConfigManager::ReconfigureItem() obj=%p newobj=%p\n", obj,
             newobj);
         #endif
         
         // We want to replace if classsified as the same sub type
         if (newobj->IsOfType(obj->GetTypeName()))
         {
            mapping[name] = newobj;
            newObjects.push_back(newobj);
            return true;
         }
      }
   }
   
   return false;
}


//------------------------------------------------------------------------------
// PhysicalModel* GetPhysicalModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a PhysicalModel object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
PhysicalModel* ConfigManager::GetPhysicalModel(const std::string &name)
{
   PhysicalModel *physicalModel = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::PHYSICAL_MODEL))
      {
         physicalModel = (PhysicalModel *)mapping[name];
      }
   }
   return physicalModel;
}

//------------------------------------------------------------------------------
// Propagator* GetPropagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Propagator object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Propagator* ConfigManager::GetPropagator(const std::string &name)
{
   Propagator *prop = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::PROPAGATOR))
      {
         prop = (Propagator *)mapping[name];
      }
   }
   return prop;
}


//------------------------------------------------------------------------------
// ForceModel* GetForceModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a ForceModel object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
ODEModel* ConfigManager::GetODEModel(const std::string &name)
{
   ODEModel *fm = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name] == NULL)
         throw ConfigManagerException
            ("ConfigManager::GetForceModel(name) is finding a NULL object in the mapping.\n");
     
      if (mapping[name]->IsOfType(Gmat::ODE_MODEL))
      {
         fm = (ODEModel *)mapping[name];
      }
   }
   return fm;
}


//------------------------------------------------------------------------------
// SpaceObject* GetSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Spacecraft or Formation object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
SpaceObject* ConfigManager::GetSpacecraft(const std::string &name)
{
   SpaceObject *sc = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if ((mapping[name]->IsOfType(Gmat::SPACECRAFT)) ||
          (mapping[name]->IsOfType(Gmat::FORMATION)))
      {
         sc = (SpaceObject *)mapping[name];
      }
   }
   return sc;
}


//------------------------------------------------------------------------------
// SpacePoint* GetSpacePoint(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a SpacePoint object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
SpacePoint* ConfigManager::GetSpacePoint(const std::string &name)
{
   SpaceObject *sp = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::SPACE_POINT))
      {
         sp = (SpaceObject *)mapping[name];
      }
   }
   return sp;
}


//------------------------------------------------------------------------------
// Plate* GetPlate(const std::string &name)
//------------------------------------------------------------------------------
/**
* Retrieves a Plate object.
*
* @param name The name of the object.
*
* @return A pointer to the object.
*/
//------------------------------------------------------------------------------
Plate* ConfigManager::GetPlate(const std::string &name)
{
	Plate *pl = NULL;
	if (mapping.find(name) != mapping.end())
	{
		if (mapping[name]->IsOfType(Gmat::PLATE))
		{
			pl = (Plate *)mapping[name];
		}
	}
	return pl;
}


//------------------------------------------------------------------------------
// Hardware* GetHardware(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Hardware object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Hardware* ConfigManager::GetHardware(const std::string &name)
{
   Hardware *hw = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::HARDWARE))
      {
         hw = (Hardware *)mapping[name];
      }
   }
   return hw;
}

//------------------------------------------------------------------------------
// FieldOfView* GetFieldOfView(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a FieldOfView object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
FieldOfView* ConfigManager::GetFieldOfView(const std::string &name)
{
   FieldOfView *fov = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::FIELD_OF_VIEW))
      {
         fov = (FieldOfView *)mapping[name];
      }
   }
   return fov;
}


//------------------------------------------------------------------------------
// PropSetup* GetPropSetup(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a PropSetup object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
PropSetup* ConfigManager::GetPropSetup(const std::string &name)
{
   PropSetup *ps = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::PROP_SETUP))
      {
         ps = (PropSetup *)mapping[name];
      }
   }
   return ps;
}


//------------------------------------------------------------------------------
// Subscriber* GetSubscriber(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Subscriber object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Subscriber* ConfigManager::GetSubscriber(const std::string &name)
{
   Subscriber *sub = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::SUBSCRIBER))
      {
         sub = (Subscriber *)mapping[name];
      }
   }
   return sub;
}


//------------------------------------------------------------------------------
// SolarSystem* GetDefaultSolarSystem()
//------------------------------------------------------------------------------
/**
 * Retrieves the default SolarSystem object.
 *
 * @return A pointer to the object.
 *
 * @note This method is not yet used in GMAT.
 */
//------------------------------------------------------------------------------
SolarSystem* ConfigManager::GetDefaultSolarSystem()
{
   return defaultSolarSystem;
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse()
//------------------------------------------------------------------------------
/**
 * Retrieves the current SolarSystem object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
SolarSystem* ConfigManager::GetSolarSystemInUse()
{
   return solarSystemInUse;
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves the current SolarSystem object by name
 *
 * @return A pointer to the object.
 *
 * @note This method is not yet used in GMAT.
 */
//------------------------------------------------------------------------------
SolarSystem* ConfigManager::GetSolarSystemInUse(const std::string &name)
{
   #if DEBUG_CONFIG_SS
   MessageInterface::ShowMessage
      ("ConfigManager::GetSolarSystemInUse() name=%s\n", name.c_str());
   #endif
   
   throw ConfigManagerException
      ("ConfigManager::GetSolarSystemInUse(name) has not been implemented.\n");
}


//------------------------------------------------------------------------------
// StopCondition* GetStopCondition(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a StopCondition object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
StopCondition* ConfigManager::GetStopCondition(const std::string &name)
{
   StopCondition *sc = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::STOP_CONDITION))
      {
         sc = (StopCondition *)mapping[name];
      }
   }
   return sc;
}


//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Parameter object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Parameter* ConfigManager::GetParameter(const std::string &name)
{
   Parameter *param = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::PARAMETER))
      {
         param = (Parameter *)mapping[name];
      }
   }
   return param;
}

//------------------------------------------------------------------------------
// Burn* GetBurn(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Burn object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Burn* ConfigManager::GetBurn(const std::string &name)
{
   Burn *burn = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::BURN))
      {
         burn = (Burn *)mapping[name];
      }
   }
   return burn;
}

//------------------------------------------------------------------------------
// Solver* GetSolver(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Solver object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Solver* ConfigManager::GetSolver(const std::string &name)
{
   Solver *solver = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::SOLVER))
      {
         solver = (Solver *)mapping[name];
      }
   }
   return solver;
}

//------------------------------------------------------------------------------
// AtmosphereModel* GetAtmosphereModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves an Atmosphere object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
AtmosphereModel* ConfigManager::GetAtmosphereModel(const std::string &name)
{
   AtmosphereModel *atmosModel = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::ATMOSPHERE))
      {
         atmosModel = (AtmosphereModel *)mapping[name];
      }
   }
   return atmosModel;
}

//------------------------------------------------------------------------------
// Function* GetFunction(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Function object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Function* ConfigManager::GetFunction(const std::string &name)
{
   Function *function = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::FUNCTION))
      {
         function = (Function *)mapping[name];
      }
   }
   return function;
}

//------------------------------------------------------------------------------
// CoordinateSystem* GetCoordinateSystem(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a CoordinateSystem object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
CoordinateSystem* ConfigManager::GetCoordinateSystem(const std::string &name)
{
   CoordinateSystem *cs = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::COORDINATE_SYSTEM))
      {
         cs = (CoordinateSystem *)mapping[name];
      }
   }
   return cs;
}


//------------------------------------------------------------------------------
// CalculatedPoint* GetCalculatedPoint(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a CalculatedPoint object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
CalculatedPoint* ConfigManager::GetCalculatedPoint(const std::string &name)
{
   CalculatedPoint *cs = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::CALCULATED_POINT))
      {
         cs = (CalculatedPoint *)mapping[name];
      }
   }
   return cs;
}


//------------------------------------------------------------------------------
// MeasurementModelBase* GetMeasurementModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a measurement model from the configuration
 *
 * @param name The name of the MeasurementModel
 *
 * @return A pointer to the MeasurementModel, or NULL if it was not found
 */
//------------------------------------------------------------------------------
MeasurementModelBase* ConfigManager::GetMeasurementModel(const std::string &name)
{
   MeasurementModelBase *mm = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::MEASUREMENT_MODEL))
      {
         mm = (MeasurementModelBase *)mapping[name];
      }
   }
   return mm;
}


//------------------------------------------------------------------------------
// ErrorModel* GetErrorModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves an error model from the configuration
 *
 * @param name The name of the ErrorModel
 *
 * @return A pointer to the ErrorModel, or NULL if it was not found
 */
//------------------------------------------------------------------------------
ErrorModel* ConfigManager::GetErrorModel(const std::string &name)
{
   ErrorModel *erm = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::ERROR_MODEL))
      {
         erm = (ErrorModel *)mapping[name];
      }
   }
   return erm;
}


//------------------------------------------------------------------------------
// DataFilter* GetDataFilter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a DataFilter from the configuration
 *
 * @param name The name of the DataFilter
 *
 * @return A pointer to the DataFilter, or NULL if it was not found
 */
//------------------------------------------------------------------------------
DataFilter* ConfigManager::GetDataFilter(const std::string &name)
{
   DataFilter *df = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::DATA_FILTER))
      {
         df = (DataFilter *)mapping[name];
      }
   }
   return df;
}


////////------------------------------------------------------------------------------
//////// TrackingSystem* GetTrackingSystem(const std::string &name)
////////------------------------------------------------------------------------------
///////**
////// * Retrieves a TrackingSystem from the configuration
////// *
////// * @param name The name of the TrackingSystem
////// *
////// * @return A pointer to the TrackingSystem, or NULL if it was not found
////// */
////////------------------------------------------------------------------------------
//////TrackingSystem* ConfigManager::GetTrackingSystem(const std::string &name)
//////{
//////   TrackingSystem *obj = NULL;
//////   if (mapping.find(name) != mapping.end())
//////   {
//////      if (mapping[name]->IsOfType(Gmat::TRACKING_SYSTEM))
//////      {
//////         obj = (TrackingSystem *)mapping[name];
//////      }
//////   }
//////   return obj;
//////}
//////
////////------------------------------------------------------------------------------
//////// TrackingData* GetTrackingData(const std::string &name)
////////------------------------------------------------------------------------------
///////**
////// * Retrieves a TrackingData object from the configuration
////// *
////// * @param name The name of the TrackingData object
////// *
////// * @return A pointer to the TrackingData object, or NULL if it was not found
////// */
////////------------------------------------------------------------------------------
//////TrackingData* ConfigManager::GetTrackingData(const std::string &name)
//////{
//////   TrackingData *obj = NULL;
//////   if (mapping.find(name) != mapping.end())
//////   {
//////      if (mapping[name]->IsOfType(Gmat::TRACKING_DATA))
//////      {
//////         obj = (TrackingData *)mapping[name];
//////      }
//////   }
//////   return obj;
//////}

//------------------------------------------------------------------------------
// DataFile* GetDataStream(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a DataStream from the configuration.
 *
 * @param name The name of the DataStream.
 *
 * @return A pointer to the DataStream.
 */
//------------------------------------------------------------------------------
DataFile* ConfigManager::GetDataStream(const std::string &name)
{
   DataFile *df = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::DATASTREAM))
      {
         df = (DataFile *)mapping[name];
      }
   }
   return df;
}

//------------------------------------------------------------------------------
// EventLocator* GetEventLocator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves an EventLocator from the configuration.
 *
 * @param name The name of the EventLocator.
 *
 * @return A pointer to the locator.
 */
//------------------------------------------------------------------------------
EventLocator* ConfigManager::GetEventLocator(const std::string &name)
{
   EventLocator *el = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::EVENT_LOCATOR))
      {
         el = (EventLocator *)mapping[name];
      }
   }
   return el;
}

//------------------------------------------------------------------------------
// bool HasConfigurationChanged()
//------------------------------------------------------------------------------
/**
 * Returns configuration changed flag. This flag is changed when objects are
 * added or removed from the configuration.
 *
 * @return  Returns configuation changed flag.
 */
//------------------------------------------------------------------------------
bool ConfigManager::HasConfigurationChanged()
{
   return configChanged;
}


//------------------------------------------------------------------------------
// void ConfigurationChanged(bool tf)
//------------------------------------------------------------------------------
/**
 * Sets configuration changed flag to true or false.
 *
 * @param tf Value to set configuration changed flag
 */
//------------------------------------------------------------------------------
void ConfigManager::ConfigurationChanged(bool tf)
{
   configChanged = tf;
}


//------------------------------------------------------------------------------
// ObjectMap* GetObjectMap()
//------------------------------------------------------------------------------
/**
 * Retrieves configuration object map.
 *
 * @return object map
 */
//------------------------------------------------------------------------------
ObjectMap* ConfigManager::GetObjectMap()
{
   return &mapping;
}

//------------------------------------------------------------------------------
// bool RelatedNameChange(std::vector<UnsignedInt> &itemType,
//       StringArray& oldName, StringArray& newName)
//------------------------------------------------------------------------------
/**
 * Retrieves name changes that were made as a side effect of other name changes
 *
 * The force model parameters use this code to report force model name and
 * Parameter name changes when a Propagator changes name, resulting in a name
 * change on an associated force model.
 *
 * This method clears the input arrays and then fills them with the name change
 * data.
 *
 * @param itemType An array filled with a list of changed types
 * @param oldName  An array of old object names
 * @param newName  An array of new object names
 *
 * @return true if there are name changes to be processed, false if not
 */
//------------------------------------------------------------------------------
bool ConfigManager::RelatedNameChange(std::vector<UnsignedInt> &itemType,
StringArray& oldName, StringArray& newName)
{
   itemType.clear();
   oldName.clear();
   newName.clear();

   bool retval = false;
   if (oldRelatedName.size() > 0)
   {
      itemType = changedItemType;
      oldName  = oldRelatedName;
      newName  = newRelatedName;
      retval   = true;
   }

   return retval;
}
