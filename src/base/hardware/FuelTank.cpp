//$Id$
//------------------------------------------------------------------------------
//                               FuelTank
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
// Created: 2004/11/08
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class implementation for the Fuel Tanks.
 */
//------------------------------------------------------------------------------


#include "FuelTank.hpp"
#include "StringUtil.hpp"          // for GmatStringUtil
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include <sstream>

//#define DEBUG_TANK_SETTING
//#define DEBUG_FUELTANK_SET
//#define DEBUG_MASS_FLOW

//
//#define ZERO_REFERENCE_TEMPERATURE_THRESHOLD 0.01

//---------------------------------
// static data
//---------------------------------

/// Labels used for the fuel tank parameters.
const std::string
FuelTank::PARAMETER_TEXT[FuelTankParamCount - HardwareParamCount] =
{
   "AllowNegativeFuelMass",
   "FuelMass",
   "FuelCenterOfMassX",
   "FuelCenterOfMassY",
   "FuelCenterOfMassZ",
   "FuelMomentOfInertiaXX",
   "FuelMomentOfInertiaXY",
   "FuelMomentOfInertiaXZ",
   "FuelMomentOfInertiaYY",
   "FuelMomentOfInertiaYZ",
   "FuelMomentOfInertiaZZ",
   "FuelCenterOfMassX_BCS",
   "FuelCenterOfMassY_BCS",
   "FuelCenterOfMassZ_BCS",
   "FuelMomentOfInertiaXX_BCS",
   "FuelMomentOfInertiaXY_BCS",
   "FuelMomentOfInertiaXZ_BCS",
   "FuelMomentOfInertiaYY_BCS",
   "FuelMomentOfInertiaYZ_BCS",
   "FuelMomentOfInertiaZZ_BCS",
};

/// Types of the parameters used by fuel tanks.
const Gmat::ParameterType
FuelTank::PARAMETER_TYPE[FuelTankParamCount - HardwareParamCount] =
{
   Gmat::BOOLEAN_TYPE,     // "AllowNegativeFuelMass"
   Gmat::REAL_TYPE,        // "FuelMass",
   Gmat::REAL_TYPE,        // "FuelCenterOfMassX",
   Gmat::REAL_TYPE,        // "FuelCenterOfMassY",
   Gmat::REAL_TYPE,        // "FuelCenterOfMassZ",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaXX",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaXY",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaXZ",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaYY",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaYZ",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaZZ"
   Gmat::REAL_TYPE,        // "FuelCenterOfMassX_BCS",
   Gmat::REAL_TYPE,        // "FuelCenterOfMassY_BCS",
   Gmat::REAL_TYPE,        // "FuelCenterOfMassZ_BCS",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaXX_BCS",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaXY_BCS",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaXZ_BCS",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaYY_BCS",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaYZ_BCS",
   Gmat::REAL_TYPE,        // "FuelMomentOfInertiaZZ_BCS"
};


//------------------------------------------------------------------------------
//  FuelTank()
//------------------------------------------------------------------------------
/**
 * Fuel tank constructor.
 *
 * @param nomme Name for the tank.
 */
//------------------------------------------------------------------------------
FuelTank::FuelTank(const std::string &typeStr, const std::string &nomme) :
   Hardware             (Gmat::FUEL_TANK, typeStr, nomme),
   fuelMass             (756.0),          // 0.6 m^3 of fuel
   fuelCM               (0.0, 0.0, 0.0),
   fuelMOI              (99*Rmatrix33(true)),
   allowNegativeFuelMass(false),
   noThrusterNeeded     (false),
   firstValidation      (true)
{
   objectTypes.push_back(Gmat::FUEL_TANK);
   objectTypeNames.push_back("FuelTank");

   parameterCount = FuelTankParamCount;
   blockCommandModeAssignment = false;

   for (Integer i = HardwareParamCount; i < FuelTankParamCount; ++i)
      parameterWriteOrder.push_back(i);

   // Set directions for rotation matrix from BCS to be identity initially
   direction[0] = 0.0;
   direction[1] = 0.0;
   direction[2] = 1.0;

   secondDirection[0] = 0.0;
   secondDirection[1] = -1.0;
   secondDirection[2] = 0.0;
}


//------------------------------------------------------------------------------
//  ~FuelTank()
//------------------------------------------------------------------------------
/**
 * Fuel tank destructor.
 */
//------------------------------------------------------------------------------
FuelTank::~FuelTank()
{
}


//------------------------------------------------------------------------------
//  FuelTank(const FuelTank& ft)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * This method is called by the Clone method to replicate fuel tanks.
 *
 * @param ft Reference to the tank that gets replicated.
 */
//------------------------------------------------------------------------------
FuelTank::FuelTank(const FuelTank& ft) :
   Hardware             (ft),
   fuelMass             (ft.fuelMass),
   fuelCM               (ft.fuelCM),
   fuelMOI              (ft.fuelMOI),
   allowNegativeFuelMass(ft.allowNegativeFuelMass),
   noThrusterNeeded     (ft.noThrusterNeeded),
   firstValidation      (true)
{
   parameterCount = ft.parameterCount;
   isInitialized  = Initialize();

   for (Integer i = HardwareParamCount; i < FuelTankParamCount; ++i)
      parameterWriteOrder.push_back(i);
}


//------------------------------------------------------------------------------
//  FuelTank& operator=(const FuelTank& ft)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * Sets the parameters for one tank equal to anothers.
 *
 * @param ft Reference to the tank that gets replicated.
 */
//------------------------------------------------------------------------------
FuelTank& FuelTank::operator=(const FuelTank& ft)
{
   #ifdef DEBUG_TANK_SETTING
      MessageInterface::ShowMessage("Calling assignment operator on %s\n",
            instanceName.c_str());
   #endif
   if (&ft != this)
   {
      Hardware::operator=(ft);

      fuelMass              = ft.fuelMass;
      fuelCM                = ft.fuelCM;
      fuelMOI               = ft.fuelMOI;
      allowNegativeFuelMass = ft.allowNegativeFuelMass;
      noThrusterNeeded      = ft.noThrusterNeeded;
      firstValidation       = true;

      isInitialized  = Initialize();
   }

   return *this;
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string FuelTank::GetParameterText(const Integer id) const
{
   if (id >= HardwareParamCount && id < FuelTankParamCount)
      return PARAMETER_TEXT[id - HardwareParamCount];
   return Hardware::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string FuelTank::GetParameterUnit(const Integer id) const
{
   if (id == FUEL_MASS)
      return "kg";
   else if (id >= FUEL_CM_X && id <= FUEL_CM_Z)
      return "m";
   else if (id >= FUEL_MOI_XX && id <= FUEL_MOI_ZZ)
      return "kg-m^2";
   else
      return Hardware::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
//  Integer  <GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer FuelTank::GetParameterID(const std::string &str) const
{
   for (Integer i = HardwareParamCount; i < FuelTankParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - HardwareParamCount])
         return i;
   }

   return Hardware::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType FuelTank::GetParameterType(const Integer id) const
{
   if (id >= HardwareParamCount && id < FuelTankParamCount)
      return PARAMETER_TYPE[id - HardwareParamCount];

   return Hardware::GetParameterType(id);
}


//---------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the string associated with a parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return Text description for the type of the parameter, or the empty
 *         string ("").
 */
//---------------------------------------------------------------------------
std::string FuelTank::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool FuelTank::IsParameterReadOnly(const Integer id) const
{
   if (FUEL_CM_X_BCS <= id && id <= FUEL_CM_Z_BCS)
      return true;

   if (FUEL_MOI_XX_BCS <= id && id <= FUEL_MOI_ZZ_BCS)
      return true;

   if ((id == DIRECTION_X) ||
      (id == DIRECTION_Y) || (id == DIRECTION_Z) ||
      (id == SECOND_DIRECTION_X) || (id == SECOND_DIRECTION_Y) ||
      (id == SECOND_DIRECTION_Z) ||
      (id == HW_ORIGIN_BCS_X) || (id == HW_ORIGIN_BCS_Y) ||
      (id == HW_ORIGIN_BCS_Z))
      return false;

   return Hardware::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool FuelTank::IsParameterCommandModeSettable(const Integer id) const
{
   if (id == ALLOW_NEGATIVE_FUEL_MASS)
      return false;

   // Activate all of the other tank specific IDs // not sure what to do here ...
   if (id >= HardwareParamCount)
      return true;

   return Hardware::IsParameterCommandModeSettable(id);
}


//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real FuelTank::GetRealParameter(const Integer id) const
{
   Rvector3 fuelCM_BCS(0, 0, 0);
   Rmatrix33 fuelMOI_BCS(true);

   if (FUEL_CM_X_BCS <= id && id <= FUEL_CM_Z_BCS)
   {
      fuelCM_BCS = GetFuelCM_BCS();
   }

   if (FUEL_MOI_XX_BCS <= id && id <= FUEL_MOI_ZZ_BCS)
   {
      Rvector3 temp_SC_CM(0, 0, 0);
      fuelMOI_BCS = GetFuelMOI_BCS(temp_SC_CM);
   }

   switch (id)
   {
      case FUEL_MASS:
         return fuelMass;

      case FUEL_CM_X:
         return fuelCM(0);

      case FUEL_CM_Y:
         return fuelCM(1);

      case FUEL_CM_Z:
         return fuelCM(2);

      case FUEL_MOI_XX:
         return fuelMOI(0,0);

      case FUEL_MOI_XY:
         return fuelMOI(0,1);

      case FUEL_MOI_XZ:
         return fuelMOI(0,2);

      case FUEL_MOI_YY:
         return fuelMOI(1,1);

      case FUEL_MOI_YZ:
         return fuelMOI(1,2);

      case FUEL_MOI_ZZ:
         return fuelMOI(2,2);

      case FUEL_CM_X_BCS:
         return fuelCM_BCS(0);

      case FUEL_CM_Y_BCS:
         return fuelCM_BCS(1);

      case FUEL_CM_Z_BCS:
         return fuelCM_BCS(2);

      case FUEL_MOI_XX_BCS:
         return fuelMOI_BCS(0,0);

      case FUEL_MOI_XY_BCS:
         return fuelMOI_BCS(0,1);

      case FUEL_MOI_XZ_BCS:
         return fuelMOI_BCS(0,2);

      case FUEL_MOI_YY_BCS:
         return fuelMOI_BCS(1,1);

      case FUEL_MOI_YZ_BCS:
         return fuelMOI_BCS(1,2);

      case FUEL_MOI_ZZ_BCS:
         return fuelMOI_BCS(2,2);

      default:
         break;   // Default just drops through
   }
   return Hardware::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * This code checks the validity of selected tank parameters; specifically, the
 * fuel mass, density, and tank pressure and volume must all be non-negative
 * numbers.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real FuelTank::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_FUELTANK_SET
   MessageInterface::ShowMessage
      ("FuelTank::SetRealParameter(), id=%d, value=%f\n", id, value);
   #endif

   switch (id)
   {
      case FUEL_MASS:
         {
            #ifdef DEBUG_TANK_SETTING
               MessageInterface::ShowMessage("Updating fuel mass to %lf\n",
                     value);
            #endif
            if (value >= 0.0 || allowNegativeFuelMass)
            {
               if (!isInitialized && !firstValidation)
                  Initialize();
               fuelMass = value;
               if (isInitialized)
                  UpdateTank();
               return fuelMass;
            }
            else
            {
               HardwareException hwe("");
               hwe.SetDetails(errorMessageFormat.c_str(),
                              GmatStringUtil::ToString(value, 16).c_str(),
                              "FuelMass", "Real Number >= 0.0");
               throw hwe;
            }
         }

      //  @TODO: For now, setting those parameters until the requirement for
      //         inputs is clarified.
      case FUEL_CM_X:
      {
         fuelCM(0) = value;
         return fuelCM(0);
      }

      case FUEL_CM_Y:
      {
         fuelCM(1) = value;
         return fuelCM(1);
      }

      case FUEL_CM_Z:
      {
         fuelCM(2) = value;
         return fuelCM(2);
      }

      case FUEL_MOI_XX:
      {
         if (value < 0.0)
         {
            HardwareException hwe("");
            hwe.SetDetails(errorMessageFormat.c_str(),
                           GmatStringUtil::ToString(value, 16).c_str(),
                           "FuelMomentOfInertiaXX", "Real Number >= 0.0");
            throw hwe;
         }
         fuelMOI(0,0) = value;
         return fuelMOI(0,0);
      }

      case FUEL_MOI_XY:
      {
         fuelMOI(0,1) = value;
         fuelMOI(1,0) = value;
         return fuelMOI(0,1);
      }

      case FUEL_MOI_XZ:
      {
         fuelMOI(0,2) = value;
         fuelMOI(2,0) = value;
         return fuelMOI(0,2);
      }

      case FUEL_MOI_YY:
      {
         if (value < 0.0)
         {
            HardwareException hwe("");
            hwe.SetDetails(errorMessageFormat.c_str(),
                           GmatStringUtil::ToString(value, 16).c_str(),
                           "FuelMomentOfInertiaYY", "Real Number >= 0.0");
            throw hwe;
         }
         fuelMOI(1,1) = value;
         return fuelMOI(1,1);
      }

      case FUEL_MOI_YZ:
      {
         fuelMOI(1,2) = value;
         fuelMOI(2,1) = value;
         return fuelMOI(1,2);
      }

      case FUEL_MOI_ZZ:
      {
         if (value < 0.0)
         {
            HardwareException hwe("");
            hwe.SetDetails(errorMessageFormat.c_str(),
                           GmatStringUtil::ToString(value, 16).c_str(),
                           "FuelMomentOfInertiaZZ", "Real Number >= 0.0");
            throw hwe;
         }
         fuelMOI(2,2) = value;
         return fuelMOI(2,2);
      }

      default:
         break;   // Default just drops through
   }

   if (FUEL_CM_X_BCS >= id && id >= FUEL_CM_Z_BCS)
   {
      throw HardwareException("The parameter \"" + GetParameterText(id) + 
                              "\" is a calculated parameter and cannot be " +
                              "set on the Hardware " + instanceName);
   }

   if (FUEL_MOI_XX_BCS >= id && id >= FUEL_MOI_ZZ_BCS)
   {
      throw HardwareException("The parameter \"" + GetParameterText(id) +
                              "\" is a calculated parameter and cannot be " +
                              "set on the Hardware " + instanceName);
   }

   return Hardware::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
Real FuelTank::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//---------------------------------------------------------------------------
Real FuelTank::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return the boolean value for this parameter, or throw an exception if the
 *         parameter access in invalid.
 */
//------------------------------------------------------------------------------
bool FuelTank::GetBooleanParameter(const Integer id) const
{
   if (id == ALLOW_NEGATIVE_FUEL_MASS)
   {
      return allowNegativeFuelMass;
   }

   return Hardware::GetBooleanParameter(id);
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const bool value)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new value.
 *
 * @return the boolean value for this parameter, or throw an exception if the
 *         parameter is invalid or not boolean.
 */
//------------------------------------------------------------------------------
bool FuelTank::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == ALLOW_NEGATIVE_FUEL_MASS)
   {
      allowNegativeFuelMass = value;
      return allowNegativeFuelMass;
   }

   return Hardware::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Fills in the data needed for internal calculations of fuel depletion.
 */
//------------------------------------------------------------------------------
bool FuelTank::Initialize()
{
   isInitialized = Hardware::Initialize();

   return isInitialized;
}


//------------------------------------------------------------------------------
// void SetFlowWithoutThruster(bool directFlowAllowed)
//------------------------------------------------------------------------------
/**
 * Method used to toggle the noThrusterNeeded flag
 *
 * @param directFlowAllowed New seting for the flag: true allows mass depletion
 *        without an associated thruster
 */
//------------------------------------------------------------------------------
void FuelTank::SetFlowWithoutThruster(bool directFlowAllowed)
{
   noThrusterNeeded = directFlowAllowed;
}


//------------------------------------------------------------------------------
// bool NoThrusterNeeded()
//------------------------------------------------------------------------------
/**
 * Test for the status of the noThrusterNeeded flag
 *
 * @return The value of the flag
 */
//------------------------------------------------------------------------------
bool FuelTank::NoThrusterNeeded()
{
   return noThrusterNeeded;
}

Real FuelTank::GetFuelMass()
{
   return fuelMass;
}

//------------------------------------------------------------------------------
// bool GetFuelCM_BCS() const
//------------------------------------------------------------------------------
/**
 * Calculate the center of mass of this fuel tank according to its current
 * fuel mass and distance from the center of the spacecraft in meters
 *
 * @return The fuel tank's center of mass
 */
//------------------------------------------------------------------------------
Rvector3 FuelTank::GetFuelCM_BCS() const
{
    // NOTE:  Follow the equation 4:   rcm = rb + M'TB(rT)
    //        MTB = R_SB
    //        rb = HW_ORIGIN_BCS_[XYZ] -> location
    //        rT = fuelCM[xyz]

    Rvector3 rcm;
    Rvector3 rb(location[0], location[1], location[2]);

    Rmatrix33 MTB = R_SB.Transpose();

    rcm = rb + (MTB * fuelCM);

    return rcm;
}

//------------------------------------------------------------------------------
// bool GetFuelMOI_BCS() const
//------------------------------------------------------------------------------
/**
 * Calculate the moment of inertia of this fuel tank according to its current
 * fuel mass and distance from the center of the spacecraft in kg*m^2
 *
 * @return The fuel tank's moment of inertia
 */
//------------------------------------------------------------------------------
Rmatrix33 FuelTank::GetFuelMOI_BCS(const Rvector3 &SC_systemCM) const
{
    // NOTE: Follow the equation 5:   r = rcm - RCM
    //       r is moment arm and RCM is SC_systemCM

    Rvector3 rcm = GetFuelCM_BCS();
    Rvector3 r = rcm - SC_systemCM;

    //  NOTE: Follow the equation 6:  I = M'TB*It*MTB + m[ |r|^2 *I33 - r*r' ]
    //
    //                M'TB = R_SB.Transpose()
    //                It = fuelMOI
    //                MTB = R_SB
    //                m = fuelMass
    //                I33 = Identify Matrix [1 0 0; 0 1 0; 0  0 1]
    //

    Rmatrix33 fuelMOI_BCS(true);
    Rmatrix33 i33(true);

    Rmatrix33 MTB = R_SB.Transpose();

    fuelMOI_BCS = (MTB * fuelMOI * R_SB) +
                  (fuelMass * ((r * r) * i33 - Outerproduct(r, r)));

    return fuelMOI_BCS;
}
