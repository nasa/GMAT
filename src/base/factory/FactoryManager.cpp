//$Id$
//------------------------------------------------------------------------------
//                               Factory Manager
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
// Created: 2003/09/11
//
/**
 * Implementation code for the FactoryManager singleton class.
 */
//------------------------------------------------------------------------------
#include "FactoryManager.hpp"
#include "Factory.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_FACTORY_CREATE

/// initialize the only instance allowed for this singleton class
FactoryManager* FactoryManager::onlyInstance = NULL;

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Instance()
//------------------------------------------------------------------------------
/**
 * Returns (and creates initially) the only instance allowed for this singleton
 * class. 
 *
 * @return Instance of the FactoryManager.
 */
//------------------------------------------------------------------------------
FactoryManager* FactoryManager::Instance()
{
   if (onlyInstance == NULL)
      onlyInstance = new FactoryManager();

   return onlyInstance;
}

//------------------------------------------------------------------------------
//  RegisterFactory()
//------------------------------------------------------------------------------
/**
 * Registers the input factory with the FactoryManager.
 *
 * All factories that are needed to create objects needed for the system to run
 * must be registered with the FactoryManager.  The Moderator should register
 * all predefined factories (e.g. Propagator, GmatCommand, etc.); a user-defined
 * class must have a corresponding factory, which must be registered in order
 * for objects of that class to be created in GMAT.
 *
 * @param <fact> the factory to register.
 *
 * @return bool indicating success of the registration.
 *
 * @notes Will need to add code later to check to make sure that the user is not
 * trying to register an already-registered factory or one that creates objects
 * of the same type as an already-registered factory.
 */
//------------------------------------------------------------------------------
bool FactoryManager::RegisterFactory(Factory* fact)
{
   if (fact == NULL)
   {
       return false;    // write some kind of error or throw an exception?
   }

   //MessageInterface::ShowMessage
   //   ("FactoryManager::RegisterFactory() adding factory %d\n", fact->GetFactoryType());
   
   factoryList.push_back(fact);  // better to put it at the back of the list?
   factoryTypeList.push_back(fact->GetFactoryType());
   
   // AWB extension
   StringArray overrides = fact->GetOverriddenTypes();
   for (UnsignedInt i = 0; i < overrides.size(); ++i)
      SetOverride(fact, overrides[i]);

   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const UnsignedInt generalType,
//               const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an generic object, with the name withName.
 *
 * @param <ofType> type of the new Spacecraft object (currently defaults
 *                 to "Spacecraft")
 * @param <withName> name of the new Spacecraft object.
 *
 * @return pointer to the newly-created Spacecraft object
 */
//------------------------------------------------------------------------------
GmatBase* FactoryManager::CreateObject(const UnsignedInt generalType,
                                       const std::string &ofType,
                                       const std::string &withName)
{
   Factory* f = FindFactory(generalType, ofType);
   
   if (f != NULL)
      return f->CreateObject(ofType,withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  Spacecraft* CreateSpacecraft(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Spacecraft, with the name withName.
 *
 * @param <ofType> type of the new Spacecraft object (currently defaults
 *                 to "Spacecraft")
 * @param <withName> name of the new Spacecraft object.
 *
 * @return pointer to the newly-created Spacecraft object
 */
//------------------------------------------------------------------------------
SpaceObject* FactoryManager::CreateSpacecraft(const std::string &ofType,
                                              const std::string &withName)
{
   Factory* f = FindFactory(Gmat::SPACECRAFT, ofType);
   if (f != NULL)
      return f->CreateSpacecraft(ofType,withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  Plate* CreatePlate(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
* Create an object of type Plate, with the name withName.
*
* @param <ofType> type of the new Plate object (currently defaults
*                 to "Plate")
* @param <withName> name of the new Plate object.
*
* @return pointer to the newly-created Plate object
*/
//------------------------------------------------------------------------------
Plate* FactoryManager::CreatePlate(const std::string &ofType,
	const std::string &withName)
{
	Factory* f = FindFactory(Gmat::PLATE, ofType);
	if (f != NULL)
		return f->CreatePlate(ofType, withName);
	return NULL;
}

//------------------------------------------------------------------------------
//  Parameter* CreateParameter(const std::string &ofType,
//                             const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Parameter, with the name withName, and of the
 * specific propagator type ofType.
 *
 * @param <ofType>   type name of the new Parameter object.
 * @param <withName> name of the new Parameter object.
 *
 * @return pointer to the newly-created Parameter object
 */
//------------------------------------------------------------------------------
Parameter* FactoryManager::CreateParameter(const std::string &ofType,
                                           const std::string &withName,
                                           GmatBase* owner)
{
   Factory* f = FindFactory(Gmat::PARAMETER, ofType, owner, withName);
   if (f != NULL)
      return f->CreateParameter(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Propagator* CreatePropagator(const std::string &ofType,
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Propagator, with the name withName, and of the
 * specific propagator type ofType.
 *
 * @param <ofType>   type name of the new Propagator object.
 * @param <withName> name of the new Propagator object.
 *
 * @return pointer to the newly-created Propagator object
 */
//------------------------------------------------------------------------------
Propagator* FactoryManager::CreatePropagator(const std::string &ofType,
                                             const std::string &withName)
{
   #ifdef DEBUG_CREATION
      MessageInterface::ShowMessage("Creating an integrator of type %s with "
            "name %s\n", ofType.c_str(), withName.c_str());
   #endif

   Factory* f = FindFactory(Gmat::PROPAGATOR, ofType);

   #ifdef DEBUG_CREATION
      MessageInterface::ShowMessage("Factory pointer: %p\n", f);
   #endif

   if (f != NULL)
      return f->CreatePropagator(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Force* CreatePhysicalModel(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type PhysicalModel, with the name withName, and of the
 * type ofType.
 *
 * @param <ofType>   type name of the new PhysicalModel object.
 * @param <withName> name of the new PhysicalModel object.
 *
 * @return pointer to the newly-created PhysicalModel object
 */
//------------------------------------------------------------------------------
PhysicalModel* FactoryManager::CreatePhysicalModel(const std::string &ofType,
                                                   const std::string &withName)
{
   Factory* f = FindFactory(Gmat::PHYSICAL_MODEL, ofType);
   if (f != NULL)
      return f->CreatePhysicalModel(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  StopCondition* CreateStopCondition(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type StopCondition, with the name withName.
 *
 * @param <ofType>   type name of the new StopCondition object.
 * @param <withName> name of the new StopCondition object.
 *
 * @return pointer to the newly-created StopCondition object
 */
//------------------------------------------------------------------------------
StopCondition* FactoryManager::CreateStopCondition(const std::string &ofType,
                                                   const std::string &withName)
{
   Factory* f = FindFactory(Gmat::STOP_CONDITION, ofType);
   if (f != NULL)
      return f->CreateStopCondition(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  CalculatedPoint* CreateCalculatedPoint(const std::string &ofType,
//                                         const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type CalculatedPoint, with the name withName,
 * and of the type ofType.
 *
 * @param <ofType>   type name of the new CalculatedPoint object.
 * @param <withName> name of the new CalculatedPoint object.
 *
 * @return pointer to the newly-created CalculatedPoint object
 */
//------------------------------------------------------------------------------
CalculatedPoint* FactoryManager::CreateCalculatedPoint(const std::string &ofType,
                                                       const std::string &withName)
{
   Factory* f = FindFactory(Gmat::CALCULATED_POINT, ofType);
   if (f != NULL)
      return f->CreateCalculatedPoint(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  CelestialBody* CreateCelestialBody(const std::string &ofType,
//                                     const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type CelestialBody, with the name withName,
 * and of the type ofType.
 *
 * @param <ofType>   type name of the new CelestialBody object.
 * @param <withName> name of the new CelestialBody object.
 *
 * @return pointer to the newly-created CelestialBody object
 */
//------------------------------------------------------------------------------
CelestialBody* FactoryManager::CreateCelestialBody(const std::string &ofType,
                                                   const std::string &withName)
{
   Factory* f = FindFactory(Gmat::CELESTIAL_BODY, ofType);
   if (f != NULL)
      return f->CreateCelestialBody(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Solver* CreateSolver(const std::string &ofType,
//                       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Solver, with the name withName,
 * and of the type ofType.
 *
 * @param <ofType>   type name of the new Solver object.
 * @param <withName> name of the new Solver object.
 *
 * @return pointer to the newly-created Solver object
 */
//------------------------------------------------------------------------------
Solver* FactoryManager::CreateSolver(const std::string &ofType,
                                     const std::string &withName)
{
   Factory* f = FindFactory(Gmat::SOLVER, ofType);
   if (f != NULL)
      return f->CreateSolver(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Subscriber* CreateSubscriber(const std::string &ofType,
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Subscriber, with the name withName.
 *
 * @param <ofType>   type name of the new Subscriber object.
 * @param <withName> name of the new Subscriber object.
 *
 * @return pointer to the newly-created Subscriber object
 */
//------------------------------------------------------------------------------
Subscriber* FactoryManager::CreateSubscriber(const std::string &ofType,
                                             const std::string &withName)
{
   Factory* f = FindFactory(Gmat::SUBSCRIBER, ofType);

   if (f != NULL)
      return f->CreateSubscriber(ofType, withName);
   
   #ifdef DEBUG_FACTORY_CREATE
   MessageInterface::ShowMessage("Could not find Factory for %s\n", ofType.c_str());
   #endif
   
   return NULL;
}

//------------------------------------------------------------------------------
//  EphemerisFile* CreateEphemerisFile(const std::string &ofType,
//                                     const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type EphemerisFile, with the name withName.
 *
 * @param <ofType>   type name of the new EphemerisFile object.
 * @param <withName> name of the new EphemerisFile object.
 *
 * @return pointer to the newly-created EphemerisFile object
 */
//------------------------------------------------------------------------------
EphemerisFile* FactoryManager::CreateEphemerisFile(const std::string &ofType,
                                                   const std::string &withName)
{
   Factory* f = FindFactory(Gmat::EPHEMERIS_FILE, ofType);
   
   if (f != NULL)
      return f->CreateEphemerisFile(ofType, withName);
   
   #ifdef DEBUG_FACTORY_CREATE
   MessageInterface::ShowMessage("Could not find Factory for %s\n", ofType.c_str());
   #endif
   
   return NULL;
}

//------------------------------------------------------------------------------
//  GmatCommand* CreateCommand(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type GmatCommand, with the name withName.
 *
 * @param <ofType>   type name of the new GmatCommand object.
 * @param <withName> name of the new GmatCommand object.
 *
 * @return pointer to the newly-created GmatCommand object
 */
//------------------------------------------------------------------------------
GmatCommand* FactoryManager::CreateCommand(const std::string &ofType,
                                           const std::string &withName)
{
   Factory* f = FindFactory(Gmat::COMMAND, ofType);
   if (f != NULL)
      return f->CreateCommand(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  Burn* CreateBurn(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Burn, with the name withName.
 *
 * @param <ofType>   type name of the new Burn object.
 * @param <withName> name of the new Burn object.
 *
 * @return pointer to the newly-created Burn object
 */
//------------------------------------------------------------------------------
Burn* FactoryManager::CreateBurn(const std::string &ofType,
                                 const std::string &withName)
{
   Factory* f = FindFactory(Gmat::BURN, ofType);
   if (f != NULL)
      return f->CreateBurn(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  AtmosphereModel* CreateAtmosphereModel(const std::string &ofType,
//                                         const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type AtmosphereModel, with the name withName.
 *
 * @param <ofType>   type name of the new AtmosphereModel object.
 * @param <withName> name of the new AtmosphereModel object.
 *
 * @return pointer to the newly-created AtmosphereModel object
 */
//------------------------------------------------------------------------------
AtmosphereModel* FactoryManager::CreateAtmosphereModel(const std::string &ofType,
                                                       const std::string &withName)
{
   Factory* f = FindFactory(Gmat::ATMOSPHERE, ofType);
   if (f != NULL)
      return f->CreateAtmosphereModel(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  Function* CreateFunction(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Function, with the name withName.
 *
 * @param <ofType>   type name of the new Function object.
 * @param <withName> name of the new Function object.
 *
 * @return pointer to the newly-created Function object
 */
//------------------------------------------------------------------------------
Function* FactoryManager::CreateFunction(const std::string &ofType,
                                         const std::string &withName)
{
   Factory* f = FindFactory(Gmat::FUNCTION, ofType);
   if (f != NULL)
      return f->CreateFunction(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  Hardware* CreateHardware(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Hardware, with the name withName.
 *
 * @param <ofType>   type name of the new Hardware object.
 * @param <withName> name of the new Hardware object.
 *
 * @return pointer to the newly-created Hardware object
 */
//------------------------------------------------------------------------------
Hardware* FactoryManager::CreateHardware(const std::string &ofType,
                                         const std::string &withName)
{
   Factory* f = FindFactory(Gmat::HARDWARE, ofType);
   if (f != NULL)
      return f->CreateHardware(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  FieldOfView* CreateFieldOfView(const std::string &ofType,
//                                 const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type FieldOfView, with the name withName.
 *
 * @param <ofType>   type name of the new FieldOfView object.
 * @param <withName> name of the new FieldOfView object.
 *
 * @return pointer to the newly-created FieldOfView object
 */
//------------------------------------------------------------------------------
FieldOfView* FactoryManager::CreateFieldOfView(const std::string &ofType,
                                               const std::string &withName)
{
   Factory* f = FindFactory(Gmat::FIELD_OF_VIEW, ofType);
   if (f != NULL)
      return f->CreateFieldOfView(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  AxisSystem* CreateAxisSystem(const std::string &ofType,
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an AxisSystem object of type ofType, with the name withName.
 *
 * @param <ofType>   type name of the AxisSystem.
 * @param <withName> name of the new AxisSystem object.
 *
 * @return pointer to the newly-created AxisSystem object
 */
//------------------------------------------------------------------------------
AxisSystem* 
FactoryManager::CreateAxisSystem(const std::string &ofType,
                                 const std::string &withName)
{
   Factory* f = FindFactory(Gmat::AXIS_SYSTEM, ofType);
   if (f != NULL)
      return f->CreateAxisSystem(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  MathNode* CreateMathNode(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an MathNode object of type ofType, with the name withName.
 *
 * @param <ofType>   type name of the MathNode.
 * @param <withName> name of the new MathNode object.
 *
 * @return pointer to the newly-created MathNode object
 */
//------------------------------------------------------------------------------
MathNode* 
FactoryManager::CreateMathNode(const std::string &ofType,
                               const std::string &withName)
{
   Factory* f = FindFactory(Gmat::MATH_NODE, ofType);
   if (f != NULL)
      return f->CreateMathNode(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Attitude* CreateAttitude(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an Attitude object of type ofType, with the name withName.
 *
 * @param <ofType>   type name of the Attitude.
 * @param <withName> name of the new Attitude object.
 *
 * @return pointer to the newly-created Attitude object
 */
//------------------------------------------------------------------------------
Attitude* 
FactoryManager::CreateAttitude(const std::string &ofType,
                               const std::string &withName)
{
   Factory* f = FindFactory(Gmat::ATTITUDE, ofType);
   if (f != NULL)
      return f->CreateAttitude(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
// SpacePoint* CreateSpacePoint(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type SpacePoint, with the name withName.
 *
 * @param <ofType> type of the new SpacePoint object.
 * @param <withName> name of the new SpacePoint object.
 *
 * @return pointer to the newly-created SpacePoint object
 */
//------------------------------------------------------------------------------
SpacePoint* FactoryManager::CreateSpacePoint(const std::string &ofType,
                                             const std::string &withName)
{
   Factory* f = FindFactory(Gmat::SPACE_POINT, ofType);
   if (f != NULL)
      return f->CreateSpacePoint(ofType,withName);
   return NULL;
}

//----- Just container
//------------------------------------------------------------------------------
//  SolarSystem* CreateSolarSystem(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type SolarSystem, with the name withName.
 *
 * @param <withName> name of the new SolarSystem object.
 *
 * @return pointer to the newly-created SolarSystem object
 */
//------------------------------------------------------------------------------
SolarSystem* FactoryManager::CreateSolarSystem(const std::string &withName)
{
   Factory* f = FindFactory(Gmat::SOLAR_SYSTEM, "SolarSystem");
   if (f != NULL)
      return f->CreateSolarSystem("SolarSystem", withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  PropSetup* CreatePropSetup(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type PropSetup, with the name withName.
 *
 * @param <withName> name of the new PropSetup object.
 *
 * @return pointer to the newly-created PropSetup object
 */
//------------------------------------------------------------------------------
PropSetup* FactoryManager::CreatePropSetup(const std::string &withName)
{
   Factory* f = FindFactory(Gmat::PROP_SETUP, "PropSetup");
   if (f != NULL)
      return f->CreatePropSetup("PropSetup", withName);
   return NULL;
}


////------------------------------------------------------------------------------
//// CoreMeasurement* FactoryManager::CreateMeasurement(const std::string &ofType,
////                                          const std::string &withName)
////------------------------------------------------------------------------------
///**
// * This method creates the measurement primitive used in a MeasurementModel
// *
// * @param ofType The type of measuremetn primitive
// * @param withName The name of the measurement prinitive.  This should be an
// *                 empty string in the current implementation.
// *
// * @return The pointer to the new object
// */
////------------------------------------------------------------------------------
//CoreMeasurement* FactoryManager::CreateMeasurement(const std::string &ofType,
//                                         const std::string &withName)
//{
//   Factory* f = FindFactory(Gmat::CORE_MEASUREMENT, ofType);
//   if (f != NULL)
//      return f->CreateMeasurement(ofType, withName);
//   return NULL;
//}


//------------------------------------------------------------------------------
// ErrorModel* FactoryManager::CreateErrorModel(const std::string &ofType,
//                                          const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates an ErrorModel object
 *
 * @param ofType The type of ErrorModel object
 * @param withName The name of ErrorModel object.  This should be an
 *                 empty string in the current implementation.
 *
 * @return The pointer to the new object
 */
//------------------------------------------------------------------------------
ErrorModel* FactoryManager::CreateErrorModel(const std::string &ofType,
                                         const std::string &withName)
{
   Factory* f = FindFactory(Gmat::ERROR_MODEL, ofType);
   if (f != NULL)
      return f->CreateErrorModel(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
// DataFilter* FactoryManager::CreateDataFilter(
//              const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates a DataFilter object
 *
 * @param ofType The type of DataFilter object
 * @param withName The name of DataFilter object.  This should be an
 *                 empty string in the current implementation.
 *
 * @return The pointer to the new object
 */
//------------------------------------------------------------------------------
DataFilter* FactoryManager::CreateDataFilter(const std::string &ofType,
                                         const std::string &withName)
{
   Factory* f = FindFactory(Gmat::DATA_FILTER, ofType);
   if (f != NULL)
      return f->CreateDataFilter(ofType, withName);
   return NULL;
}


//TrackingSystem* FactoryManager::CreateTrackingSystem(const std::string &ofType,
//         const std::string &withName)
//{
//   Factory* f = FindFactory(Gmat::TRACKING_SYSTEM, ofType);
//   if (f != NULL)
//      return f->CreateTrackingSystem(ofType, withName);
//   return NULL;
//}

//TrackingData* FactoryManager::CreateTrackingData(const std::string &withName)
//{
//   Factory* f = FindFactory(Gmat::TRACKING_DATA, "TrackingData");
//   if (f != NULL)
//      return f->CreateTrackingData("TrackingData", withName);
//   return NULL;
//}

EventLocator* FactoryManager::CreateEventLocator(const std::string &ofType,
                                          const std::string &withName)
{
   Factory* f = FindFactory(Gmat::EVENT_LOCATOR, ofType);
   if (f != NULL)
      return f->CreateEventLocator(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
// ObType* FactoryManager::CreateObType(const std::string &ofType,
//                                      const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates the ObType used in a DataFile
 *
 * @param ofType The type of ObType
 * @param withName The name of the ObType.  This should be an
 *                 empty string in the current implementation.
 *
 * @return The pointer to the new object
 */
//------------------------------------------------------------------------------
ObType* FactoryManager::CreateObType(const std::string &ofType,
                                     const std::string &withName)
{
   Factory* f = FindFactory(Gmat::OBTYPE, ofType);
   if (f != NULL)
      return f->CreateObType(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
// Interface* FactoryManager::CreateInterface(const std::string &ofType,
//                                            const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates the Interface used in a DataFile
 *
 * @param ofType The type of Interface
 * @param withName The name of the Interface.  This should be an
 *                 empty string in the current implementation.
 *
 * @return The pointer to the new object
 */
//------------------------------------------------------------------------------
Interface* FactoryManager::CreateInterface(const std::string &ofType,
                                           const std::string &withName)
{
   Factory* f = FindFactory(Gmat::INTERFACE, ofType);
   if (f != NULL)
      return f->CreateInterface(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
// MeasurementModelBase* CreateMeasurementModel(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type MeasurementModelBase, with the name withName.
 *
 * @param withName name of the new MeasurementModel object.
 *
 * @return pointer to the newly-created MeasurementModel object
 */
//------------------------------------------------------------------------------
MeasurementModelBase* FactoryManager::CreateMeasurementModel(const std::string &ofType,
      const std::string &withName)
{
   Factory* f = FindFactory(Gmat::MEASUREMENT_MODEL, ofType);
   if (f != NULL)
      return f->CreateMeasurementModel(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
// DataFile* FactoryManager::CreateDataFile(const std::string &ofType,
//                                          const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type DataFile, with the name withName.
 *
 * @param ofType   type of the new DataFile object.
 * @param withName name of the new DataFile object.
 *
 * @return pointer to the newly-created DataFile object
 */
//------------------------------------------------------------------------------
DataFile* FactoryManager::CreateDataFile(const std::string &ofType,
                                         const std::string &withName)
{
   Factory* f = FindFactory(Gmat::DATA_FILE, ofType);
   if (f != NULL)
      return f->CreateDataFile(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  ODEModel* CreateODEModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type ODEModel, with the name withName.
 *
 * @param <withName> name of the new ODEModel object.
 *
 * @return pointer to the newly-created ODEModel object
 */
//------------------------------------------------------------------------------
ODEModel* FactoryManager::CreateODEModel(const std::string &ofType,
                                         const std::string &withName)
{
   Factory* f = FindFactory(Gmat::ODE_MODEL, ofType);
   if (f != NULL)
      return f->CreateODEModel(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  CoordinateSystem* CreateCoordinateSystem(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type CoordinateSystem, with the name withName.
 *
 * @param <withName> name of the new CoordinateSystem object.
 *
 * @return pointer to the newly-created CoordinateSystem object
 */
//------------------------------------------------------------------------------
CoordinateSystem*
FactoryManager::CreateCoordinateSystem(const std::string &withName)
{
   Factory* f = FindFactory(Gmat::COORDINATE_SYSTEM, "CoordinateSystem");
   if (f != NULL)
      return f->CreateCoordinateSystem("CoordinateSystem", withName);
   return NULL;
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfItems(UnsignedInt byType,
//             const std::string &withQualifier = "")
//------------------------------------------------------------------------------
/**
 * Return a list of items of type byType that can be created.
 *
 * @param byType type of items of which to return a list.
 * @param withQualifier Optional qualifier to retrieve only subtypes
 *
 * @return list of creatable items of type byType.
 */
//------------------------------------------------------------------------------
const StringArray& FactoryManager::GetListOfItems(UnsignedInt byType,
            const std::string &withQualifier)
{
   entireList.clear();
   GetList(byType, withQualifier);
   return entireList;
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfAllItems()
//------------------------------------------------------------------------------
/**
 * Return a list of all items that can be created.
 *
 * @return list of all creatable items.
 */
//------------------------------------------------------------------------------
const StringArray& FactoryManager::GetListOfAllItems()
{
   entireList.clear();
   
   // Build all creatable object list
   // Now we can do this since data member factoryTypeList was added (LOJ: 2010.08.19)
   // factoryTypeList is filled when factory is registered in the Moderator
   std::list<UnsignedInt>::iterator ftype = factoryTypeList.begin();
   while (ftype != factoryTypeList.end())
   {
      GetList(*ftype, "");
      ftype++;
   }
   
   return entireList;
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfAllItemsExcept(const ObjectTypeArray &types)
//------------------------------------------------------------------------------
/**
 * Return a list of all items that can be created except input types
 *
 * @param <type> object types to be excluded
 *
 * @return list of all creatable items excluding input types
 */
//------------------------------------------------------------------------------
const StringArray& FactoryManager::GetListOfAllItemsExcept(const ObjectTypeArray &types)
{
   entireList.clear();
   
   // Build all creatable object list except given types
   std::list<UnsignedInt>::iterator ftype = factoryTypeList.begin();
   while (ftype != factoryTypeList.end())
   {
      if (find(types.begin(), types.end(), *ftype) == types.end())
         GetList((*ftype), "");
      
      ftype++;
   }
   
   return entireList;
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfViewableItems(UnsignedInt byType)
//------------------------------------------------------------------------------
/**
 * Return a list of items of type byType that can be viewed via GUI.
 *
 * @param <byType> type of items of which to return a list.
 *
 * @return list of viewable items of type byType.
 */
//------------------------------------------------------------------------------
const StringArray& FactoryManager::GetListOfViewableItems(UnsignedInt byType)
{
   entireList.clear();
   return GetListOfViewables(byType);
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfUnviewableItems(UnsignedInt byType)
//------------------------------------------------------------------------------
/**
 * Return a list of items of type byType that cannot be viewed via GUI.
 *
 * @param <byType> type of items of which to return a list.
 *
 * @return list of unviewable items of type byType.
 */
//------------------------------------------------------------------------------
const StringArray& FactoryManager::GetListOfUnviewableItems(UnsignedInt byType)
{
   entireList.clear();
   return GetListOfUnviewables(byType);
}

//------------------------------------------------------------------------------
// const ObjectTypeArrayMap& GetAllObjectTypeArrayMap()
//------------------------------------------------------------------------------
/**
 * Return a map of all items that can be created.
 *
 * @return map of all creatable items with object type as a key
 */
//------------------------------------------------------------------------------
const ObjectTypeArrayMap& FactoryManager::GetAllObjectTypeArrayMap()
{
   objectTypeArrayMap.clear();
   
   std::list<UnsignedInt>::iterator ftype = factoryTypeList.begin();
   StringArray currList;
   while (ftype != factoryTypeList.end())
   {
      currList = GetList(*ftype, "");
      // Build creatable object map
      objectTypeArrayMap.insert(std::make_pair(*ftype, currList));
      ftype++;
   }
   
   return objectTypeArrayMap;
}

//------------------------------------------------------------------------------
// bool DoesObjectTypeMatchSubtype(const std::string &theType,
//       const std::string &theSubtype)
//------------------------------------------------------------------------------
/**
 * Checks if a creatable object type matches a subtype.
 *
 * @param coreType The ObjectType of the candidate object
 * @param theType The script identifier for the object type
 * @param theSubtype The subtype being checked
 *
 * @return true if the scripted type and subtype match, false if the type does
 *         not match the subtype
 */
//------------------------------------------------------------------------------
bool FactoryManager::DoesObjectTypeMatchSubtype(const UnsignedInt coreType,
      const std::string &theType, const std::string &theSubtype)
{
   Factory* theFactory = FactoryManager::FindFactory(coreType, theType);
   if (theFactory == NULL)
      return false;
   return theFactory->DoesObjectTypeMatchSubtype(theType, theSubtype);
}


//------------------------------------------------------------------------------
// UnsignedInt GetBaseTypeOf(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Return the base type for the input string.
 *
 * @param <typeName> string type name.
 *
 * @return the base UnsignedInt for this string type name.
 */
//------------------------------------------------------------------------------
UnsignedInt FactoryManager::GetBaseTypeOf(const std::string &typeName)
{
   // Special case for the "Create Propagator" line - do we want this?
   //if (typeName == "Propagator") return Gmat::PROP_SETUP;
   StringArray listByType;
   for (int ii = Gmat::SPACECRAFT; ii < Gmat::UNKNOWN_OBJECT; ii++)
   {
      listByType = GetListOfItems((UnsignedInt)ii);
      unsigned int sz = listByType.size();
      for (unsigned int jj = 0; jj < sz; jj++)
         if (listByType.at(jj) == typeName)  return (UnsignedInt)ii;
   }
   return Gmat::UNKNOWN_OBJECT;
}

//------------------------------------------------------------------------------
//  ~FactoryManager()
//------------------------------------------------------------------------------
/**
* Destructs FactoryManager objects.
 *
 */
//------------------------------------------------------------------------------
FactoryManager::~FactoryManager()
{
   //MessageInterface::ShowMessage("~FactoryManager() size=%d\n", factoryList.size());
   
   std::list<Factory*>::iterator f = factoryList.begin();
   while (f != factoryList.end())
   {
      //MessageInterface::ShowMessage("Deleting FactoryType=%d\n", (*f)->GetFactoryType());
      delete *f;       // delete each factory first
      ++f;
   }
   
};

//---------------------------------
//  protected methods
//---------------------------------
// none


//---------------------------------
//  private methods
//---------------------------------

//------------------------------------------------------------------------------
//  FactoryManager()
//------------------------------------------------------------------------------
/**
 * Constructs a FactoryManager object (default constructor).
 */
//------------------------------------------------------------------------------
FactoryManager::FactoryManager()
{
}


//------------------------------------------------------------------------------
// Factory* FindFactory(UnsignedInt ofType, const std::string forType)
//------------------------------------------------------------------------------
/**
 * Return a pointer to a factory that can create objects of type forType.
 *
 * @param <ofType>  type of creatable object requested.
 * @param <forType> specific type of creatable object requested.
 *
 * @return pointer to a factory that creates objects of the requested type.
 */
//------------------------------------------------------------------------------
Factory* FactoryManager::FindFactory(UnsignedInt ofType,
                                     const std::string &forType,
                                     GmatBase* owner, const std::string &withName)
{
   #ifdef DEBUG_FACTORY_CREATE
   MessageInterface::ShowMessage(
   "Entering FactoryManager::FindFactory with type = %d and forType = %s\n",
   (Integer) ofType, forType.c_str());
   #endif
   
   // AWB extension: Check for override first
   if (overrideFactoryList.find(forType) != overrideFactoryList.end())
      return overrideFactoryList[forType];

   // Search through factoryList for the factory that creates objects of type
   // ofType, specifically of type forType
   std::list<Factory*>::iterator f = factoryList.begin();
   while (f != factoryList.end())
   {
      if ((*f)->GetFactoryType() == ofType)
      {

         // Search through the list of creatable objects to see if one matches
         StringArray listObj = (*f)->GetListOfCreatableObjects();
         bool isCaseSensitive = (*f)->IsTypeCaseSensitive();
         
         #ifdef DEBUG_FACTORY_CREATE
         MessageInterface::ShowMessage(
         "    isCaseSensitive = %s\n",
         (isCaseSensitive?  "TRUE" : "False"));
         #endif
         
         if (!listObj.empty())
         {
//            StringArray::iterator s = listObj.begin();
            std::string objType = forType;
            
            // Make sure that all type name begins with upper case if
            // factory expects case sensitive type names since all GMAT type names
            // begin with upper case.
            // Currently only math types are NOT case sensitive, so we can
            // have for example, both Transpose and transpose, Inverse and inverse
            // for compatibility with MATLAB.
            if (isCaseSensitive)
               objType = GmatStringUtil::Capitalize(objType);
            
            if (find(listObj.begin(), listObj.end(), objType) != listObj.end())
            {
               #ifdef DEBUG_FACTORY_CREATE
               MessageInterface::ShowMessage
                  ("FactoryManager::FindFactory() found factory and returning <%p>\n", (*f));
               #endif

               if (ofType == Gmat::PARAMETER)
               {
                  if (owner)
                  {
                     if (withName != "")
                     {
                        Parameter* param = (*f)->CreateParameter(objType, withName);
                        if (param)
                        {
                           UnsignedInt paramType = param->GetOwnerType();
                           if (owner->IsOfType(paramType))
                           {
                              delete param;
                              return (*f);
                           }
                           delete param;
                        }
                     }
                  }
                  if (!owner || withName == "")
                     return (*f);
               }
               else
                  return (*f);
            }
         }
      }
      ++f;
   }
   #ifdef DEBUG_FACTORY_CREATE
   MessageInterface::ShowMessage(
   "At end of FactoryManager::FindFactory, returning NULL\n");
   #endif
   return NULL;
}

//------------------------------------------------------------------------------
// const StringArray& GetList(UnsignedInt ofType,
//             const std::string &withQualifier) const
//------------------------------------------------------------------------------
/**
 * Return a list of items of type ofType that can be created.
 *
 * @param ofType type of creatable objects of which to return a list.
 * @param withQualifier Optional qualifier to retrieve only subtypes
 *
 * @return list of creatable items of type ofType.
 */
//------------------------------------------------------------------------------
const StringArray& FactoryManager::GetList(UnsignedInt ofType,
            const std::string &withQualifier)
{
   //entireList.clear();
   std::list<Factory*>::iterator f = factoryList.begin();
   static StringArray creatables;
   creatables.clear();
   while (f != factoryList.end())
   {
      if ((*f)->GetFactoryType() == ofType)
      {
         // Add the name(s) to the list 
         StringArray objs = (*f)->GetListOfCreatableObjects(withQualifier);
         if (!objs.empty())
         {
            // Do not add the same name, some how we are getting
            // multiple object types with plugins (LOJ: 2012.03.02)
            //entireList.insert(entireList.end(), objs.begin(), objs.end());
            for (UnsignedInt i = 0; i < objs.size(); i++)
            {
               if (find(entireList.begin(), entireList.end(), objs[i]) == entireList.end())
                  entireList.push_back(objs[i]);
               if (find(creatables.begin(), creatables.end(), objs[i]) == creatables.end())
                  creatables.push_back(objs[i]);
            }
         }
         
         // There may be multiple factories of a given type, so keep looking
         //break;
      }
      ++f;
   }
   
   return creatables;
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfViewables(UnsignedInt ofType) const
//------------------------------------------------------------------------------
/**
 * Return a list of items of type ofType that can be viewed via GUI.
 *
 * @param <ofType> type of viewable objects of which to return a list.
 *
 * @return list of viewable items of type ofType.
 */
//------------------------------------------------------------------------------
const StringArray& FactoryManager::GetListOfViewables(UnsignedInt ofType)
{
   std::list<Factory*>::iterator f = factoryList.begin();
   while (f != factoryList.end())
   {
      if ((*f)->GetFactoryType() == ofType)
      {
         // Add the name(s) to the list 
         StringArray objs = (*f)->GetListOfViewableObjects();
         if (!objs.empty())
            entireList.insert(entireList.end(), objs.begin(), objs.end());
         
         // There may be multiple factories of a given type, so keep looking
         //break;
      }
      ++f;
   }
   return entireList;
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfUnviewables(UnsignedInt ofType) const
//------------------------------------------------------------------------------
/**
 * Return a list of items of type ofType that can be viewed via GUI.
 *
 * @param <ofType> type of viewable objects of which to return a list.
 *
 * @return list of viewable items of type ofType.
 */
//------------------------------------------------------------------------------
const StringArray& FactoryManager::GetListOfUnviewables(UnsignedInt ofType)
{
   std::list<Factory*>::iterator f = factoryList.begin();
   while (f != factoryList.end())
   {
      if ((*f)->GetFactoryType() == ofType)
      {
         // Add the name(s) to the list 
         StringArray objs = (*f)->GetListOfUnviewableObjects();
         if (!objs.empty())
            entireList.insert(entireList.end(), objs.begin(), objs.end());
         
         // There may be multiple factories of a given type, so keep looking
         //break;
      }
      ++f;
   }
   return entireList;
}


//------------------------------------------------------------------------------
// void SetOverride(Factory * useFactory, const std::string &ofType)
//------------------------------------------------------------------------------
/**
 * Overrides the default factory for an object type
 *
 * @param useFactory The factory providing the override
 * @param type       The type that is overridden
 */
//------------------------------------------------------------------------------
void FactoryManager::SetOverride(Factory * useFactory, const std::string &type)
{
   StringArray listObj = useFactory->GetListOfCreatableObjects();
   bool isCaseSensitive = useFactory->IsTypeCaseSensitive();

   std::string objType = type;
   if (isCaseSensitive)
      objType = GmatStringUtil::Capitalize(objType);

   if (find(listObj.begin(), listObj.end(), objType) != listObj.end())
   {
      if (overrideFactoryList.find(objType) != overrideFactoryList.end())
         MessageInterface::ShowMessage("Warning: Override %s is replacing a "
               "previously set override\n", objType.c_str());
      overrideFactoryList[objType] = useFactory;
   }
}
