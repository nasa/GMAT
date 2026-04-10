//$Id$
//------------------------------------------------------------------------------
//                               ElectricTank
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
// Author: Wendy Shoan
// Created: 2014.07.22
//
/**
 * Class implementation for the Electric Fuel Tanks.
 */
//------------------------------------------------------------------------------


#include "ElectricTank.hpp"
#include "StringUtil.hpp"          // for GmatStringUtil
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include <sstream>


//#define DEBUG_TANK_SETTING
//#define DEBUG_FUEL_TANK_SET
//#define DEBUG_MASS_FLOW

//---------------------------------
// static data
//---------------------------------
///// Labels used for the fuel tank parameters.
//const std::string
//ElectricTank::PARAMETER_TEXT[ElectricTankParamCount - FuelTankParamCount] =
//{
//};
//
///// Types of the parameters used by fuel tanks.
//const Gmat::ParameterType
//ElectricTank::PARAMETER_TYPE[ElectricTankParamCount - FuelTankParamCount] =
//{
//};


//------------------------------------------------------------------------------
//  ElectricTank()
//------------------------------------------------------------------------------
/**
 * Electric fuel tank constructor.
 *
 * @param nomme Name for the tank.
 */
//------------------------------------------------------------------------------
ElectricTank::ElectricTank(const std::string &nomme) :
   FuelTank             ("ElectricTank", nomme)
{
   objectTypes.push_back(Gmat::ELECTRIC_FUEL_TANK);
   objectTypeNames.push_back("ElectricTank");

   parameterCount = ElectricTankParamCount;
   blockCommandModeAssignment = false;
}


//------------------------------------------------------------------------------
//  ~ElectricTank()
//------------------------------------------------------------------------------
/**
 * Fuel tank destructor.
 */
//------------------------------------------------------------------------------
ElectricTank::~ElectricTank()
{
}


//------------------------------------------------------------------------------
//  ElectricTank(const ElectricTank& ft)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * This method is called by the Clone method to replicate electric fuel tanks.
 *
 * @param ft Reference to the tank that gets replicated.
 */
//------------------------------------------------------------------------------
ElectricTank::ElectricTank(const ElectricTank& ft) :
   FuelTank             (ft)
{
   parameterCount = ft.parameterCount;
   isInitialized  = Initialize();
}


//------------------------------------------------------------------------------
//  ElectricTank& operator=(const ElectricTank& ft)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * Sets the parameters for one tank equal to another's.
 *
 * @param ft Reference to the tank that gets replicated.
 */
//------------------------------------------------------------------------------
ElectricTank& ElectricTank::operator=(const ElectricTank& ft)
{
   #ifdef DEBUG_TANK_SETTING
      MessageInterface::ShowMessage("ELectricFuelTank::Calling assignment operator on %s\n",
            instanceName.c_str());
   #endif
   if (&ft != this)
   {
      FuelTank::operator=(ft);
      isInitialized  = Initialize();
   }

   return *this;
}


////------------------------------------------------------------------------------
////  std::string  GetParameterText(const Integer id) const
////------------------------------------------------------------------------------
///**
// * This method returns the parameter text, given the input parameter ID.
// *
// * @param <id> Id for the requested parameter text.
// *
// * @return parameter text for the requested parameter.
// *
// */
////------------------------------------------------------------------------------
//std::string ElectricTank::GetParameterText(const Integer id) const
//{
//   if (id >= FuelTankParamCount && id < ElectricTankParamCount)
//      return PARAMETER_TEXT[id - FuelTankParamCount];
//   return FuelTank::GetParameterText(id);
//}
//

////------------------------------------------------------------------------------
////  std::string  GetParameterUnit(const Integer id) const
////------------------------------------------------------------------------------
///**
// * @see GmatBase
// */
////------------------------------------------------------------------------------
//std::string ElectricTank::GetParameterUnit(const Integer id) const
//{
//   switch (id)
//   {
//   }
//}
//
//
////------------------------------------------------------------------------------
////  Integer  GetParameterID(const std::string &str) const
////------------------------------------------------------------------------------
///**
// * This method returns the parameter ID, given the input parameter string.
// *
// * @param <str> string for the requested parameter.
// *
// * @return ID for the requested parameter.
// *
// */
////------------------------------------------------------------------------------
//Integer ElectricTank::GetParameterID(const std::string &str) const
//{
//   for (Integer i = FuelTankParamCount; i < ElectricTankParamCount; i++)
//   {
//      if (str == PARAMETER_TEXT[i - FuelTankParamCount])
//         return i;
//   }
//
//   return FuelTank::GetParameterID(str);
//}
//
//
////------------------------------------------------------------------------------
////  Gmat::ParameterType  GetParameterType(const Integer id) const
////------------------------------------------------------------------------------
///**
// * This method returns the parameter type, given the input parameter ID.
// *
// * @param <id> ID for the requested parameter.
// *
// * @return parameter type of the requested parameter.
// *
// */
////------------------------------------------------------------------------------
//Gmat::ParameterType ElectricTank::GetParameterType(const Integer id) const
//{
//   if (id >= FuelTankParamCount && id < ElectricTankParamCount)
//      return PARAMETER_TYPE[id - FuelTankParamCount];
//
//   return FuelTank::GetParameterType(id);
//}
//
//
////---------------------------------------------------------------------------
////  std::string GetParameterTypeString(const Integer id) const
////---------------------------------------------------------------------------
///**
// * Retrieve the string associated with a parameter.
// *
// * @param <id> The integer ID for the parameter.
// *
// * @return Text description for the type of the parameter, or the empty
// *         string ("").
// */
////---------------------------------------------------------------------------
//std::string ElectricTank::GetParameterTypeString(const Integer id) const
//{
//   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
//}
//
//
////---------------------------------------------------------------------------
////  bool IsParameterReadOnly(const Integer id) const
////---------------------------------------------------------------------------
///**
// * Checks to see if the requested parameter is read only.
// *
// * @param <id> Description for the parameter.
// *
// * @return true if the parameter is read only, false (the default) if not,
// *         throws if the parameter is out of the valid range of values.
// */
////---------------------------------------------------------------------------
//bool ElectricTank::IsParameterReadOnly(const Integer id) const
//{
//   if (id == PRESSURE_REGULATED)
//      return true;
//
//   return FuelTank::IsParameterReadOnly(id);
//}
//
//
////------------------------------------------------------------------------------
//// bool IsParameterCommandModeSettable(const Integer id) const
////------------------------------------------------------------------------------
///**
// * Tests to see if an object property can be set in Command mode
// *
// * @param id The ID of the object property
// *
// * @return true if the property can be set in command mode, false if not.
// */
////------------------------------------------------------------------------------
//bool ElectricTank::IsParameterCommandModeSettable(const Integer id) const
//{
////   if ((id == ALLOW_NEGATIVE_FUEL_MASS) ||
////       (id == PRESSURE_MODEL) ||
////       (id == PRESSURE_REGULATED))
//   if ((id == PRESSURE_MODEL) ||
//       (id == PRESSURE_REGULATED))
//      return false;
//
//   // Activate all of the other tank specific IDs  // not sure what to do here ...
//   if (id >= FuelTankParamCount)
//      return true;
//
//   return FuelTank::IsParameterCommandModeSettable(id);
//}


//---------------------------------------------------------------------------
//  GmatBase* Clone() const
//---------------------------------------------------------------------------
/**
 * Provides a clone of this object by calling the copy constructor.
 *
 * @return A GmatBase pointer to the cloned fuel tank.
 */
//---------------------------------------------------------------------------
GmatBase* ElectricTank::Clone() const
{
   return new ElectricTank(*this);
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void ElectricTank::Copy(const GmatBase* orig)
{
   operator=(*((ElectricTank *)(orig)));
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Fills in the data needed for internal calculations of fuel depletion.
 */
//------------------------------------------------------------------------------
bool ElectricTank::Initialize()
{
   if (!FuelTank::Initialize())
      return false;

   isInitialized = false;

   if (Validate())
   {
//      gasVolume = volume - fuelMass / density;
//      pvBase = pressure * gasVolume;
      firstValidation = false;
      isInitialized = true;
   }

   return isInitialized;
}


// Protected methods

//------------------------------------------------------------------------------
//  void UpdateTank()
//------------------------------------------------------------------------------
/**
 * Updates pressure and volume data using the ideal gas law.
 *
 * GMAT fuel tanks can operate in a pressure-regulated mode (constant pressure,
 * constant temperature) or in a blow-down mode.  When the tank runs in
 * blow-down mode, the pressure is calculated using the ideal gas law:
 *
 *    \f[PV=nRT\f]
 *
 * The right side of this equation is held constant.  Given an initial pressure
 * \f$P_i\f$ and an initial volume \f$V_i\f$, the new pressure is given by
 *
 *    \f[P_f= \frac{P_i V_i}{V_f}\f]
 *
 * The pressurant volume \f$V_G\f$ is calculated from the tank volume
 * \f$V_T\f$, the fuel mass \f$M_F\f$, and the fuel density \f$\rho\f$ using
 *
 *    \f[V_{G}=V_{T}-\frac{M_{F}}{\rho}\f]
 *
 * Mass is depleted from the tank by integrating the mass flow over time, as
 * is described in the Thruster documentation.
 */
//------------------------------------------------------------------------------
void ElectricTank::UpdateTank()
{
//   if (pressureModel != TPM_PRESSURE_REGULATED)
//   {
//      #ifdef DEBUG_MASS_FLOW
//         MessageInterface::ShowMessage("Vol = %.12lf, fuelMass = %.12lf, "
//               "density = %.12lf, P0 = %.12lf, PV = %.12lf", volume, fuelMass,
//               density, pressure, pvBase);
//      #endif
//      if (!isInitialized)
//         Initialize();
//
//      gasVolume = volume - fuelMass / density;
//
////      if (gasVolume < 0.0)
////         throw HardwareException("*** ERROR *** The fuel volume in the "
////               "tank \"" + instanceName + "\" exceeds the volume of the tank.");
//
//      pressure = pvBase / gasVolume;
//
//      #ifdef DEBUG_MASS_FLOW
//         MessageInterface::ShowMessage("Gas Vol = %.12lf, Pf = %.12lf\n",
//               gasVolume, pvBase);
//      #endif
//   }
}


//------------------------------------------------------------------------------
//  void DepleteFuel(Real dm)
//------------------------------------------------------------------------------
/**
 * Depletes fuel from the tank and updates the tank data as needed.
 *
 * @todo Fill in the mathematics for this method.
 */
//------------------------------------------------------------------------------
void ElectricTank::DepleteFuel(Real dm)
{
//   fuelMass -= dm;
//
//   if (fuelMass < 0.0)
//      // For now, throw if the fuel goes below 0
//      throw HardwareException("Fuel in tank " + instanceName +
//                              " completely exhausted.\n");
}

bool ElectricTank::Validate()
{
//   if (density <= 0.0)
//	  return false;
//   if ((volume - fuelMass / density) < 0.0)
//	   throw HardwareException("Fuel volume exceeds tank capacity\n");
   return true;
}

