//$Id$
//------------------------------------------------------------------------------
//                            Factory
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
// Created: 2003/09/16
//
/**
 *  Implementation code for the base class, Factory, responsible for creating
 *  objects; subclasses will create instances of specific objects.
 */
//------------------------------------------------------------------------------
#include "Factory.hpp"
#include "FactoryException.hpp"
#include "MessageInterface.hpp"
#include <algorithm>              // for find()

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Returns a GmatBase object pointer to a new object. 
 *
 * @param <ofType>   specific type of object to create.
 * @param <withName> name to give to the newly created object.
 *
 * @return pointer to a new object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of the requested type.
 */
//------------------------------------------------------------------------------
GmatBase* Factory::CreateObject(const std::string &ofType,
                                const std::string &withName)
{
   throw FactoryException("Generic factory creation method not implemented for " +
                          ofType);
}

//------------------------------------------------------------------------------
//  Spacecraft* CreateSpacecraft(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create Spacecraft objects -
 * in that case, it returns a new Spacecraft object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Spacecraft.
 *
 * @param <ofType>   specific type of Spacecraft object to create.
 * @param <withName> name to give to the newly created Spacecraft object.
 *
 * @return pointer to a new Spacecraft object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type Spacecraft.
 */
//------------------------------------------------------------------------------
SpaceObject* Factory::CreateSpacecraft(const std::string &ofType,
                                       const std::string &withName)
{
   throw FactoryException("requested object must be of type SpaceObject");
}


//------------------------------------------------------------------------------
//  Plate* CreatePlate(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
* Must be implemented by derived classes that create Plate objects -
* in that case, it returns a new Plate object.  Otherwise, it
* throws an exception indicating that the class does not create objects of
* type Plate.
*
* @param <ofType>   specific type of Plate object to create.
* @param <withName> name to give to the newly created Plate object.
*
* @return pointer to a new Plate object.
*
* @exception <FactoryException> thrown if the factory does not create
* objects of type Plate.
*/
//------------------------------------------------------------------------------
Plate* Factory::CreatePlate(const std::string &ofType,
	const std::string &withName)
{
	throw FactoryException("requested object must be of type Plate");
}

//------------------------------------------------------------------------------
//  SpacePoint* CreateSpacePoint(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create SpacePoint objects -
 * in that case, it returns a new SpacePoint object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Spacecraft.
 * 
 * This method is used to create objects that have a physical location modeled 
 * in a GMAT mission: GroundStation objects, other objects used for modeling 
 * contact events, and other fixed or mobile objects that are needed to model
 * mission details.  Note that the Spacecraft and CelestialBody objects are
 * created using different Factory methods; this method should be used for 
 * SpacePoints that fall outside of those categories.
 *
 * @param <ofType>   specific type of SpacePoint object to create.
 * @param <withName> name to give to the newly created SpacePoint object.
 *
 * @return pointer to a new SpacePoint object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type SpacePoint.
 */
//------------------------------------------------------------------------------
SpacePoint* Factory::CreateSpacePoint(const std::string &ofType,
                                      const std::string &withName)        
{
   throw FactoryException("requested object must be of type SpacePoint");
}

//------------------------------------------------------------------------------
//  GroundStation* CreateGroundStation(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create GroundStation objects -
 * in that case, it returns a new GroundStation object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type GroundStation.
 *
 * @param <ofType>   specific type of GroundStation object to create.
 * @param <withName> name to give to the newly created GroundStation object.
 *
 * @return pointer to a new GroundStation object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type GroundStation.
 */
//------------------------------------------------------------------------------
//   GroundStation* Factory::CreateGroundStation(const std::string &ofType,
//                                                   const std::string &withName);
//{
//   throw FactoryException("requested object must be of type GroundStation");
//}

//------------------------------------------------------------------------------
//  Propagator* CreatePropagator(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create Propagator objects -
 * in that case, it returns a new Propagator object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Propagator.
 *
 * @param <ofType>   specific type of Propagator object to create.
 * @param <withName> name to give to the newly created Propagator object.
 *
 * @return pointer to a new Propagator object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type Propagator.
 */
//------------------------------------------------------------------------------
Propagator* Factory::CreatePropagator(const std::string &ofType,
                                      const std::string &withName)
{
   throw FactoryException("requested object must be of type Propagator");
}


//------------------------------------------------------------------------------
//  Parameter* CreateParameter(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create Parameter objects -
 * in that case, it returns a new Parameter object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Parameter.
 *
 * @param <ofType>   specific type of Parameter object to create.
 * @param <withName> name to give to the newly created Parameter object.
 *
 * @return pointer to a new Parameter object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type Parameter.
 */
//------------------------------------------------------------------------------
Burn* Factory::CreateBurn(const std::string &ofType, const std::string &withName)
{
   throw FactoryException("requested object must be of type Burn");
}

//------------------------------------------------------------------------------
//  Propagator* CreateParameter(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create Parameter objects -
 * in that case, it returns a new Parameter object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Parameter.
 *
 * @param <ofType>   specific type of Parameter object to create.
 * @param <withName> name to give to the newly created Parameter object.
 *
 * @return pointer to a new Parameter object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type Parameter.
 */
//------------------------------------------------------------------------------
Parameter* Factory::CreateParameter(const std::string &ofType,
                                    const std::string &withName)
{
   throw FactoryException("requested object must be of type Parameter");
}

//------------------------------------------------------------------------------
//  ODEModel* CreateODEModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  ODEModel objects -
 * in that case, it returns a new  ODEModel object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  ODEModel.
 *
 * @param <ofType>   specific type of  ODEModel object to create.
 * @param <withName> name to give to the newly created  ODEModel object.
 *
 * @return pointer to a new  ODEModel object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  ODEModel.
 */
//------------------------------------------------------------------------------
ODEModel* Factory::CreateODEModel(const std::string &ofType,
                                      const std::string &withName)
{
   throw FactoryException("requested object must be of type ODEModel");
}

//------------------------------------------------------------------------------
//  PhysicalModel* CreatePhysicalModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  PhysicalModel objects -
 * in that case, it returns a new  PhysicalModel object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type PhysicalModel.
 *
 * @param <ofType>   specific type of PhysicalModel object to create.
 * @param <withName> name to give to the newly created PhysicalModel object.
 *
 * @return pointer to a new PhysicalModel object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type PhysicalModel.
 */
//------------------------------------------------------------------------------
PhysicalModel* Factory::CreatePhysicalModel(const std::string &ofType,
                                            const std::string &withName)
{
   throw FactoryException("requested object must be of type PhysicalModel");
}

//------------------------------------------------------------------------------
//  PropSetup* CreatePropSetup(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  PropSetup objects -
 * in that case, it returns a new  PropSetup object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  PropSetup.
 *
 * @param <ofType>   specific type of  PropSetup object to create.
 * @param <withName> name to give to the newly created  PropSetup object.
 *
 * @return pointer to a new  PropSetup object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  PropSetup.
 */
//------------------------------------------------------------------------------
PropSetup* Factory::CreatePropSetup(const std::string &ofType,
                                    const std::string &withName)
{
   throw FactoryException("requested object must be of type PropSetup");
}

//------------------------------------------------------------------------------
//  CalculatedParameter* CreateCalculatedParameter(const std::string &ofType,
//                                                 const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  CalculatedParameter
 * objects - in that case, it returns a new  CalculatedParameter object.
 * Otherwise, it throws an exception indicating that the class does not create
 * objects of type  CalculatedParameter.
 *
 * @param <ofType>   specific type of  CalculatedParameter object to create.
 * @param <withName> name to give to the newly created  CalculatedParameter
 *                   object.
 *
 * @return pointer to a new  CalculatedParameter object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  CalculatedParameter.
 */
//------------------------------------------------------------------------------
//CalculatedParameter* Factory::CreateCalculatedParameter(
//                                                      const std::string &ofType,
//                                                      const std::string &withName)
//{
//   throw FactoryException(
   //                  "requested object must be of type CalculatedParameter");
//}

//------------------------------------------------------------------------------
//  StopCondition* CreateStopCondition(const std::string &ofType,
//                                     const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  StopCondition objects -
 * in that case, it returns a new  StopCondition object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  StopCondition.
 *
 * @param <ofType>   specific type of  StopCondition object to create.
 * @param <withName> name to give to the newly created  StopCondition object.
 *
 * @return pointer to a new  StopCondition object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  StopCondition.
 */
//------------------------------------------------------------------------------
StopCondition* Factory::CreateStopCondition(const std::string &ofType,
                                            const std::string &withName)
{
   throw FactoryException("requested object must be of type StopCondition");
}

//------------------------------------------------------------------------------
//  CalculatedPoint* CreateCalculatedPoint(const std::string &ofType,
//                                         const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  CalculatedPoint objects -
 * in that case, it returns a new  CalculatedPoint object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  CalculatedPoint.
 *
 * @param <ofType>   specific type of  CalculatedPoint object to create.
 * @param <withName> name to give to the newly created  CalculatedPoint object.
 *
 * @return pointer to a new  CalculatedPoint object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  CalculatedPoint.
 */
//------------------------------------------------------------------------------
CalculatedPoint* Factory::CreateCalculatedPoint(const std::string &ofType,
                                                const std::string &withName)
{
   throw FactoryException("requested object must be of type CalculatedPoint");
}


//------------------------------------------------------------------------------
//  CelestialBody* CreateCelestialBody(const std::string &ofType,
//                                     const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  CelestialBody objects -
 * in that case, it returns a new  CelestialBody object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  CelestialBody.
 *
 * @param <ofType>   specific type of  CelestialBody object to create.
 * @param <withName> name to give to the newly created  CelestialBody object.
 *
 * @return pointer to a new  CelestialBody object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  CelestialBody.
 */
//------------------------------------------------------------------------------
CelestialBody* Factory::CreateCelestialBody(const std::string &ofType,
                                            const std::string &withName)
{
   throw FactoryException("requested object must be of type CelestialBody");
}

//------------------------------------------------------------------------------
//  SolarSystem* CreateSolarSystem(const std::string &ofType,
//                                 const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  SolarSystem objects -
 * in that case, it returns a new  SolarSystem object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  SolarSystem.
 *
 * @param <ofType>   specific type of  SolarSystem object to create.
 * @param <withName> name to give to the newly created  SolarSystem object.
 *
 * @return pointer to a new  SolarSystem object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  SolarSystem.
 */
//------------------------------------------------------------------------------
SolarSystem* Factory::CreateSolarSystem(const std::string &ofType,
                                        const std::string &withName)
{
   throw FactoryException("requested object must be of type SolarSystem");
}

//------------------------------------------------------------------------------
//  Solver* CreateSolver(const std::string &ofType,
//                       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create Solver objects -
 * in that case, it returns a new  Solver object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  Solver.
 *
 * @param <ofType>   specific type of  Solver object to create.
 * @param <withName> name to give to the newly created  Solver object.
 *
 * @return pointer to a new  Solver object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  Solver.
 */
//------------------------------------------------------------------------------
Solver* Factory::CreateSolver(const std::string &ofType,
                              const std::string &withName)
{
   throw FactoryException("requested object must be of type Solver");
}

//------------------------------------------------------------------------------
//  Subscriber* CreateSubscriber(const std::string &ofType, 
//                               const std::string &withName,
//                               const std::string &fileName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  Subscriber objects -
 * in that case, it returns a new  Subscriber object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  Subscriber.
 *
 * @param <ofType>   specific type of  Subscriber object to create.
 * @param <withName> name to give to the newly created  Subscriber object.
 * @param <fileName> file name if subscriber is ReportFile, ignored otherwise
 *
 * @return pointer to a new  Subscriber object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  Subscriber.
 */
//------------------------------------------------------------------------------
Subscriber* Factory::CreateSubscriber(const std::string &ofType,
                                      const std::string &withName)
{
   throw FactoryException("requested object must be of type Subscriber");
}

//------------------------------------------------------------------------------
//  EphemerisFile* CreateEphemerisFile(const std::string &ofType, 
//                                     const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  EphemerisFile objects -
 * in that case, it returns a new  EphemerisFile object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  EphemerisFile.
 *
 * @param <ofType>   specific type of  EphemerisFile object to create.
 * @param <withName> name to give to the newly created  EphemerisFile object.
 *
 * @return pointer to a new  EphemerisFile object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  EphemerisFile.
 */
//------------------------------------------------------------------------------
EphemerisFile* Factory::CreateEphemerisFile(const std::string &ofType,
                                            const std::string &withName)
{
   throw FactoryException("requested object must be of type EphemerisFile");
}

//------------------------------------------------------------------------------
//  GmatCommand* CreateCommand(const std::string &ofType,
//                             const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  Command objects -
 * in that case, it returns a new  GmatCommand object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  GmatCommand.
 *
 * @param <ofType>   specific type of  GmatCommand object to create.
 * @param <withName> name to give to the newly created  GmatCommand object.
 *
 * @return pointer to a new  GmatCommand object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  GmatCommand.
 */
//------------------------------------------------------------------------------
GmatCommand* Factory::CreateCommand(const std::string &ofType,
                                    const std::string &withName)
{
   throw FactoryException("requested object must be of type GmatCommand");
}

//------------------------------------------------------------------------------
//  AtmosphereModel* CreateAtmosphereModel(const std::string &ofType,
//                                         const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  AtmosphereModel objects -
 * in that case, it returns a new  AtmosphereModel object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  AtmosphereModel.
 *
 * @param <ofType>   specific type of  AtmosphereModel object to create.
 * @param <withName> name to give to the newly created  AtmosphereModel object.
 *
 * @return pointer to a new  AtmosphereModel object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  AtmosphereModel.
 */
//------------------------------------------------------------------------------
AtmosphereModel* Factory::CreateAtmosphereModel(const std::string &ofType,
                                                const std::string &withName)
{
   throw FactoryException
      ("Factory::CreateAtmosphereModel() must be implemented by AtmosphereFactory()\n");
}

//------------------------------------------------------------------------------
//  Function* CreateFunction(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  Function objects -
 * in that case, it returns a new  Function object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  Function.
 *
 * @param <ofType>   specific type of  Function object to create.
 * @param <withName> name to give to the newly created  Function object.
 *
 * @return pointer to a new  Function object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  Function.
 */
//------------------------------------------------------------------------------
Function* Factory::CreateFunction(const std::string &ofType,
                                  const std::string &withName)
{
   throw FactoryException
      ("Factory::CreateFunction() must be implemented by FunctionFactory()\n");
}


//------------------------------------------------------------------------------
//  Hardware* CreateHardware(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
* Creates tanks, thrusters, and other hardware attached to the GMAT spacecraft.
 * 
 * Must be implemented by derived classes that create Hardware objects -
 * in that case, it returns a new Hardware object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Hardware.
 *
 * @param <ofType>   specific type of Hardware object to create.
 * @param <withName> name to give to the newly created Hardware object.
 *
 * @return pointer to a new Hardware object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type Hardware.
 */
//------------------------------------------------------------------------------
Hardware* Factory::CreateHardware(const std::string &ofType,
                                  const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateHardware() must be implemented by the HardwareFactory\n");
}

//------------------------------------------------------------------------------
//  FieldOfView* CreateFieldOfView(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 *
 * Must be implemented by derived classes that create FieldOfView objects -
 * in that case, it returns a new FieldOfView object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type FieldOfView.
 *
 * @param <ofType>   specific type of FieldOfView object to create.
 * @param <withName> name to give to the newly created FieldOfView object.
 *
 * @return pointer to a new FieldOfView object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type FieldOfView.
 */
//------------------------------------------------------------------------------
FieldOfView* Factory::CreateFieldOfView(const std::string &ofType,
                                  const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateFieldOfView() must be implemented by the FieldOfViewFactory\n");
}


//------------------------------------------------------------------------------
//  AxisSystem* CreateAxisSystem(const std::string &ofType,
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates as AxisSystem object.
 * 
 * Must be implemented by derived classes that create AxisSystem objects -
 * in that case, it returns a new AxisSystem object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type AxisSystem.
 *
 * @param <ofType>   specific type of AxisSystem object to create.
 * @param <withName> name to give to the newly created AxisSystem object.
 *
 * @return pointer to a new AxisSystem object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type AxisSystem.
 */
//------------------------------------------------------------------------------
AxisSystem* Factory::CreateAxisSystem(const std::string &ofType,
                                      const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateAxisSystem() must be implemented by the AxisSystemFactory\n");
}


//------------------------------------------------------------------------------
//  CoordinateSystem* CreateCoordinateSystem(const std::string &ofType,
//                                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates as CoordinateSystem object.
 * 
 * Must be implemented by derived classes that create CoordinateSystem objects -
 * in that case, it returns a new CoordinateSystem object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type CoordinateSystem.
 *
 * @param <ofType>   specific type of CoordinateSystem object to create.
 * @param <withName> name to give to the newly created CoordinateSystem object.
 *
 * @return pointer to a new CoordinateSystem object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type CoordinateSystem.
 */
//------------------------------------------------------------------------------
CoordinateSystem* Factory::CreateCoordinateSystem(const std::string &ofType,
                                                  const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateCoordinateSystem() must be implemented by the "
    "CoordinateSystemFactory\n");
}


//------------------------------------------------------------------------------
//  MathNode* CreateMathNode(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates as MathNode object.
 * 
 * Must be implemented by derived classes that create MathNode objects -
 * in that case, it returns a new MathNode object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type MathNode.
 *
 * @param <ofType>   specific type of MathNode object to create.
 * @param <withName> name to give to the newly created MathNode object.
 *
 * @return pointer to a new MathNode object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type MathNode.
 */
//------------------------------------------------------------------------------
MathNode* Factory::CreateMathNode(const std::string &ofType,
                                  const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateMathNode() must be implemented by the MathFactory\n");
}

//------------------------------------------------------------------------------
//  Attitude* CreateAttitude(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates an Attitude object.
 * 
 * Must be implemented by derived classes that create Attitude objects -
 * in that case, it returns a new Attitude object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Attitude.
 *
 * @param <ofType>   specific type of Attitude object to create.
 * @param <withName> name to give to the newly created Attitude object.
 *
 * @return pointer to a new Attitude object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type Attitude.
 */
//------------------------------------------------------------------------------
Attitude* Factory::CreateAttitude(const std::string &ofType,
                                  const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateAttitude() must be implemented by the AttitudeFactory\n");
}


//------------------------------------------------------------------------------
// MeasurementModelBase *CreateMeasurementModel(const std::string & ofType,
//    const std::string & withName)
//------------------------------------------------------------------------------
/**
 * Creates a MeasurementModel or TrackingFileSet object.
 *
 * Must be implemented by derived classes that create MeasurementModel objects -
 * in that case, it returns a new MeasurementModel object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type MeasurementModel.
 *
 * @param <ofType>   specific type of MeasurementModel object to create.
 * @param <withName> name to give to the newly created MeasurementModel object.
 *
 * @return pointer to a new MeasurementModel object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type MeasurementModel.
 */
//------------------------------------------------------------------------------
MeasurementModelBase* Factory::CreateMeasurementModel(const std::string & ofType,
                                         const std::string & withName)
{
   throw FactoryException
   ("Factory::CreateMeasurementModel() must be implemented by a "
         "MeasurementModelFactory\n");
}


////------------------------------------------------------------------------------
//// CoreMeasurement* CreateMeasurement(const std::string & ofType,
////    const std::string & withName)
////------------------------------------------------------------------------------
///**
// * Creates a fundamental measurement object.
// *
// * Must be implemented by derived classes that create CoreMeasurement objects -
// * in that case, it returns a new CoreMeasurement object.  Otherwise, it
// * throws an exception indicating that the class does not create objects of
// * type CoreMeasurement.
// *
// * @param <ofType>   specific type of CoreMeasurement object to create.
// * @param <withName> name to give to the newly created CoreMeasurement object.
// *
// * @return pointer to a new CoreMeasurement object.
// *
// * @exception <FactoryException> thrown if the factory does not create
// *                               objects of type CoreMeasurement.
// */
////------------------------------------------------------------------------------
//CoreMeasurement* Factory::CreateMeasurement(const std::string & ofType,
//                                        const std::string & withName)
//{
//   throw FactoryException
//   ("Factory::CreateMeasurement() must be implemented by a MeasurementFactory\n");
//}


//------------------------------------------------------------------------------
// ErrorModel* CreateErrorModel(const std::string & ofType,
//    const std::string & withName)
//------------------------------------------------------------------------------
/**
 * Creates a fundamental ErrorModel object.
 *
 * Must be implemented by derived classes that create ErrorModel objects -
 * in that case, it returns a new ErrorModel object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type ErrorModel.
 *
 * @param <ofType>   specific type of ErrorModel object to create.
 * @param <withName> name to give to the newly created ErrorModel object.
 *
 * @return pointer to a new ErrorModel object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type ErrorModel.
 */
//------------------------------------------------------------------------------
ErrorModel* Factory::CreateErrorModel(const std::string & ofType,
                                        const std::string & withName)
{
   throw FactoryException
   ("Factory::CreateErrorModel() must be implemented by a ErrorModelFactory\n");
}


//------------------------------------------------------------------------------
// DataFilter* CreateDataFilter(const std::string & ofType,
//    const std::string & withName)
//------------------------------------------------------------------------------
/**
 * Creates a fundamental DataFilter object.
 *
 * Must be implemented by derived classes that create DataFilter objects -
 * in that case, it returns a new DataFilter object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type DataFilter.
 *
 * @param <ofType>   specific type of DataFilter object to create.
 * @param <withName> name to give to the newly created DataFilter object.
 *
 * @return pointer to a new DataFilter object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type DataFilter.
 */
//------------------------------------------------------------------------------
DataFilter* Factory::CreateDataFilter(const std::string & ofType,
                                        const std::string & withName)
{
   throw FactoryException
   ("Factory::CreateDataFilter() must be implemented by a DataFilterFactory\n");
}


//------------------------------------------------------------------------------
// DataFile* Factory::CreateDataFile(const std::string & ofType,
//                                   const std::string & withName)
//------------------------------------------------------------------------------
/**
 * Creates a DataFile object.
 *
 * Must be implemented by derived classes that create DataFile objects -
 * in that case, it returns a new DataFile object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type DataFile.
 *
 * @param <ofType>   specific type of DataFile object to create.
 * @param <withName> name to give to the newly created DataFile object.
 *
 * @return pointer to a new DataFile object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type DataFile.
 */
//------------------------------------------------------------------------------
DataFile* Factory::CreateDataFile(const std::string & ofType,
                                  const std::string & withName)
{
   throw FactoryException
      ("Factory::CreateDataFile() must be implemented by a DataFileFactory\n");
}


//------------------------------------------------------------------------------
// ObType* CreateObType(const std::string & ofType, const std::string & withName)
//------------------------------------------------------------------------------
/**
 * Creates an ObType object.
 *
 * Must be implemented by derived classes that create ObType objects -
 * in that case, it returns a new ObType object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type ObType.
 *
 * @param <ofType>   specific type of ObType object to create.
 * @param <withName> name to give to the newly created ObType object.
 *
 * @return pointer to a new ObType object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type ObType.
 */
//------------------------------------------------------------------------------
ObType* Factory::CreateObType(const std::string & ofType,
      const std::string & withName)
{
   throw FactoryException
   ("Factory::CreateObType() must be implemented by an ObTypeFactory\n");
}


//------------------------------------------------------------------------------
// Event* CreateEvent(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates an Event object.
 *
 * Must be implemented by derived classes that create Event objects -
 * in that case, it returns a new Event object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Event.
 *
 * @param <ofType>   specific type of Event object to create.
 * @param <withName> name to give to the newly created Event object.
 *
 * @return pointer to a new Event object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type Event.
 */
//------------------------------------------------------------------------------
Event* Factory::CreateEvent(const std::string &ofType, const std::string &withName)
{
   throw FactoryException
         ("Factory::CreateEvent() must be implemented by an EventFactory\n");
}


//------------------------------------------------------------------------------
// Event* CreateEvent(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates an EventLocator object.
 *
 * Must be implemented by derived classes that create EventLocator objects -
 * in that case, it returns a new EventLocator object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type EventLocator.
 *
 * @param <ofType>   specific type of EventLocator object to create.
 * @param <withName> name to give to the newly created EventLocator object.
 *
 * @return pointer to a new EventLocator object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type EventLocator.
 */
//------------------------------------------------------------------------------
EventLocator* Factory::CreateEventLocator(const std::string &ofType,
                                     const std::string &withName)
{
   throw FactoryException
         ("Factory::CreateEventLocator() must be implemented by an "
               "EventLocatorFactory\n");
}

//------------------------------------------------------------------------------
// Interface* CreateInterface(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates an Interface object.
 *
 * Must be implemented by derived classes that create Interface objects -
 * in that case, it returns a new Interface object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Interface.
 *
 * @param <ofType>   specific type of Interface object to create.
 * @param <withName> name to give to the newly created Interface object.
 *
 * @return pointer to a new Interface object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type Interface.
 */
//------------------------------------------------------------------------------
Interface* Factory::CreateInterface(const std::string &ofType, const std::string &withName)
{
   throw FactoryException
         ("Factory::CreateInterface() must be implemented by an InterfaceFactory\n");
}


//////------------------------------------------------------------------------------
////// TrackingSystem* Factory::CreateTrackingSystem(const std::string &ofType,
//////                                         const std::string &withName)
//////------------------------------------------------------------------------------
/////**
//// * Creates a TrackingSystem object.
//// *
//// * Must be implemented by derived classes that create TrackingSystem objects -
//// * in that case, it returns a new TrackingSystem object.  Otherwise, it
//// * throws an exception indicating that the class does not create objects of
//// * type TrackingSystem.
//// *
//// * @param <ofType>   specific type of TrackingSystem object to create.
//// * @param <withName> name to give to the newly created TrackingSystem object.
//// *
//// * @return pointer to a new TrackingSystem object.
//// *
//// * @exception <FactoryException> thrown if the factory does not create
//// *                               objects of type TrackingSystem.
//// */
//////------------------------------------------------------------------------------
////TrackingSystem* Factory::CreateTrackingSystem(const std::string &ofType,
////         const std::string &withName)
////{
////   throw FactoryException
////      ("Factory::CreateTrackingSystem() must be implemented by a "
////               "TrackingSystemFactory\n");
////}


////------------------------------------------------------------------------------
//// TrackingData* Factory::CreateTrackingData(const std::string &ofType,
////                                         const std::string &withName)
////------------------------------------------------------------------------------
///**
// * Creates a TrackingData object.
// *
// * Must be implemented by derived classes that create TrackingData objects -
// * in that case, it returns a new TrackingData object.  Otherwise, it
// * throws an exception indicating that the class does not create objects of
// * type TrackingData.
// *
// * @param <ofType>   specific type of TrackingData object to create.
// * @param <withName> name to give to the newly created TrackingData object.
// *
// * @return pointer to a new TrackingData object.
// *
// * @exception <FactoryException> thrown if the factory does not create
// *                               objects of type TrackingData.
// */
////------------------------------------------------------------------------------
//TrackingData* Factory::CreateTrackingData(const std::string &ofType,
//         const std::string &withName)
//{
//   throw FactoryException
//      ("Factory::CreateTrackingData() must be implemented by a "
//               "TrackingDataFactory\n");
//}

//------------------------------------------------------------------------------
//  StringArray GetListOfCreatableObjects() const
//------------------------------------------------------------------------------
/**
 * This method returns the list of creatable objects for the factory.
 *
 * @param qualifier Qualifier for a list of subtypes.  Derived classes override
 *                  this method to allow for subsets of the creatables list
 *
 * @return list of creatable objects of this factory.
 *
 */
//------------------------------------------------------------------------------
StringArray Factory::GetListOfCreatableObjects(const std::string &qualifier)
{
   // This code prevents plugin commands from entering the list of mission
   // starters unless explicitly placed there 
   if (qualifier == "SequenceStarters")
   {
      qualifiedCreatables.clear();
      return qualifiedCreatables;
   }

   return creatables;
}


//------------------------------------------------------------------------------
// bool DoesObjectTypeMatchSubtype(const std::string &theType,
//       const std::string &theSubtype)
//------------------------------------------------------------------------------
/**
 * Checks if a creatable object type matches a subtype.
 *
 * Occasionally GMAT factories will build types that are subtyped at a
 * granularity finer than is supported in the UnsignedInt enumeration.
 * This method allows detection of the fine grained subtyping.  An example is
 * the Solver subsystem, which provides for targeters and related boundary value
 * solvers, optimizers, estimators, simulators, and parametric scanners.  This
 * method, when overridden in solver factories, can be used to distinguish these
 * subtypes.
 *
 * @param theType The script identifier for the object type
 * @param theSubtype The subtype being checked
 *
 * @return true if the scripted type and subtype match, false if the type does
 *         not match the subtype
 */
//------------------------------------------------------------------------------
bool Factory::DoesObjectTypeMatchSubtype(const std::string &theType,
      const std::string &theSubtype)
{
   // Default is to return false, meaning objects are not subtyped
   return false;
}


//------------------------------------------------------------------------------
//  StringArray GetListOfViewableObjects()
//------------------------------------------------------------------------------
/**
* This method creates and returns the list of viewable objects via GUI.
 *
 * @return list of viewable objects of this factory.
 *
 */
//------------------------------------------------------------------------------
StringArray Factory::GetListOfViewableObjects()
{
   #ifdef DEBUG_FACTORY_VIEWABLES
   MessageInterface::ShowMessage
      ("Factory::GetListOfViewableObjects() entered, there are %d creatables, %d unviewables, "
       "%d viewables\n", creatables.size(), unviewables.size(), viewables.size());
   #endif
   
   if (viewables.empty())
   {
      // Add all creatable objects types not in the unviewables to viewables
      StringArray::iterator pos;
      for (pos = creatables.begin(); pos != creatables.end(); ++pos)
      {
         if (find(unviewables.begin(), unviewables.end(), *pos) == unviewables.end())
            viewables.push_back(*pos);
      }
   }
   
   #ifdef DEBUG_FACTORY_VIEWABLES
   MessageInterface::ShowMessage
      ("Factory::GetListOfViewableObjects() leaving, there are %d creatables, %d unviewables, "
       "%d viewables\n", creatables.size(), unviewables.size(), viewables.size());
   #endif
   
   return viewables;
}

//------------------------------------------------------------------------------
//  StringArray GetListOfUnviewableObjects() const
//------------------------------------------------------------------------------
/**
* This method returns the list of unviewable objects via GUI.
 *
 * @return list of unviewable objects of this factory.
 *
 */
//------------------------------------------------------------------------------
StringArray Factory::GetListOfUnviewableObjects() const
{
   return unviewables;
}

//------------------------------------------------------------------------------
//  bool SetListOfCreatableObjects(StringArray itsList)
//------------------------------------------------------------------------------
/**
* This method sets the list of creatable objects for the factory.
 *
 * @param <newList>  list of creatable objects of this factory.
 *
 * @return flag indicating success of the operation
 *
 */
//------------------------------------------------------------------------------
bool Factory::SetListOfCreatableObjects(StringArray itsList)
{
   if (!creatables.empty())
      creatables.clear();
   creatables = itsList;
   return true;
}

//------------------------------------------------------------------------------
//  bool AddCreatableObjects(void) const
//------------------------------------------------------------------------------
/**
* This method adds to the list of creatable objects for the factory.
 *
 * @param <newList>  list of additional creatable objects of this factory.
 *
 * @return flag indicating success of the operation
 *
 */
//------------------------------------------------------------------------------
bool Factory::AddCreatableObjects(StringArray newList)
{
   if (creatables.size() > 0)
   {
      StringArray::iterator si = newList.begin();
      while (si != newList.end())
      {
         creatables.push_back(*si);
         ++si;
      }
   }
   //      creatables.splice(creatables.end(),newList);
   else
      creatables = newList;
   return true;
}

//------------------------------------------------------------------------------
// StringArray GetOverriddenTypes()
//------------------------------------------------------------------------------
/**
 * Method used to identify class overrides for the Factory subsystem
 *
 * @return The list of class overrides from this factory
 */
//------------------------------------------------------------------------------
StringArray Factory::GetOverriddenTypes()
{
   return overrides;
}

//------------------------------------------------------------------------------
//  UnsignedInt GetFactoryType() const
//------------------------------------------------------------------------------
/**
 * This method returns the type of objects created by this factory.
 *
 * @return type of objects created by this factory.
 *
 */
//------------------------------------------------------------------------------
UnsignedInt Factory::GetFactoryType() const
{
   return itsType;
}


//------------------------------------------------------------------------------
//  bool IsTypeCaseSensitive() const
//------------------------------------------------------------------------------
/**
 * This method returns the type of objects created by this factory.
 *
 * @return type of objects created by this factory.
 *
 */
//------------------------------------------------------------------------------
bool Factory::IsTypeCaseSensitive() const
{
   return isCaseSensitive;
}

//---------------------------------
//  protected methods
//---------------------------------
//------------------------------------------------------------------------------
//  Factory(UnsignedInt ofType)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the base class Factory (called by
 * derived classes).  (default constructor)
 *
 * @param <ofType> optional parameter indicating type of objects created by
 *                 this factory.
 */
//------------------------------------------------------------------------------
Factory::Factory(UnsignedInt ofType)
{
   itsType = ofType;
   isCaseSensitive = true;
   // creatables list is empty at this point
}

//------------------------------------------------------------------------------
//  Factory(StringArray createList, UnsignedInt ofType)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the base class Factory (called by
 * derived classes), and sets the list of creatable objects. 
 *
 * @param <createList> list of creatable objects for this factory
 * @param <ofType> optional parameter indicating type of objects created by
 *                 this factory.
 */
//------------------------------------------------------------------------------
Factory::Factory(StringArray createList, UnsignedInt ofType)
{
   itsType    = ofType;
   isCaseSensitive = true;
   creatables = createList;
}

//------------------------------------------------------------------------------
//  Factory(const Factory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the base class Factory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
Factory::Factory(const Factory& fact) :
   itsType (fact.itsType),
   isCaseSensitive (fact.isCaseSensitive)
{
   if (!creatables.empty())
      creatables.clear();
   creatables = fact.creatables;
   if (!unviewables.empty())
      unviewables.clear();
   unviewables = fact.unviewables;

   /// AWB extension
   overrides = fact.overrides;
}

//------------------------------------------------------------------------------
//  Factory& operator= (const Factory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Factory base class.
 *
 * @param <fact> the factory object whose data to assign to "this" factory.
 *
 * @return "this" factory with data of input factory fact.
 */
//------------------------------------------------------------------------------
Factory& Factory::operator= (const Factory& fact)
{
   itsType    = fact.itsType;
   isCaseSensitive = fact.isCaseSensitive;
   creatables = fact.creatables;

   /// AWB extension
   overrides = fact.overrides;

   return *this;
}

//------------------------------------------------------------------------------
// ~Factory()
//------------------------------------------------------------------------------
/**
 * Destructor for the Factory base class.
 *
 */
//------------------------------------------------------------------------------
Factory::~Factory()
{
   // delete creatables;
   //creatables.~vector<std::string>(); //loj: 4/7/06 commented
}

