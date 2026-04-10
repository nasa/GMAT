//$Id$
//------------------------------------------------------------------------------
//                               ChemicalTank
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
// Author: Darrel J. Conway (original FuelTank code)
// Created: 2004/11/08
//
/**
 * Class implementation for the Fuel Tanks.
 */
//------------------------------------------------------------------------------


#include "ChemicalTank.hpp"
#include "StringUtil.hpp"          // for GmatStringUtil
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include <sstream>


#define ZERO_REFERENCE_TEMPERATURE_THRESHOLD 0.01

//#define DEBUG_TANK_SETTING
//#define DEBUG_FUEL_TANK_SET
//#define DEBUG_MASS_FLOW

//---------------------------------
// static data
//---------------------------------

/// Available pressure models
StringArray ChemicalTank::pressureModelList;

/// Labels used for the fuel tank parameters.
const std::string
ChemicalTank::PARAMETER_TEXT[ChemicalTankParamCount - FuelTankParamCount] =
{
//   "AllowNegativeFuelMass",
//   "FuelMass",
   "Pressure",
   "Temperature",
   "RefTemperature",
   "Volume",
   "FuelDensity",
   "PressureModel",
   "PressureRegulated",  // deprecated
};

/// Types of the parameters used by fuel tanks.
const Gmat::ParameterType
ChemicalTank::PARAMETER_TYPE[ChemicalTankParamCount - FuelTankParamCount] =
{
//   Gmat::BOOLEAN_TYPE,     // "AllowNegativeFuelMass"
//   Gmat::REAL_TYPE,        // "FuelMass",
   Gmat::REAL_TYPE,        // "Pressure",
   Gmat::REAL_TYPE,        // "Temperature",
   Gmat::REAL_TYPE,        // "RefTemperature",
   Gmat::REAL_TYPE,        // "Volume",
   Gmat::REAL_TYPE,        // "FuelDensity",
   Gmat::ENUMERATION_TYPE, // "PressureModel",
   Gmat::BOOLEAN_TYPE,     // deprecated
};


//------------------------------------------------------------------------------
//  ChemicalTank()
//------------------------------------------------------------------------------
/**
 * Fuel tank constructor.
 *
 * @param nomme Name for the tank.
 */
//------------------------------------------------------------------------------
ChemicalTank::ChemicalTank(const std::string &nomme) :
   FuelTank             ("ChemicalTank", nomme),
//   fuelMass             (756.0),       // 0.6 m^3 of fuel
   pressure             (1500.0),
   temperature          (20.0),
   refTemperature       (20.0),
   volume               (0.75),
   density              (1260.0),      // Hydrazine/H2O2 mixture
//   allowNegativeFuelMass(false),
   pressureModel        (TPM_PRESSURE_REGULATED)
{
   objectTypes.push_back(Gmat::CHEMICAL_FUEL_TANK);
   objectTypeNames.push_back("ChemicalTank");

   parameterCount = ChemicalTankParamCount;
   blockCommandModeAssignment = false;

   // Available pressure model list
   // Since it is static data, clear it first
   pressureModelList.clear();
   pressureModelList.push_back("PressureRegulated");
   pressureModelList.push_back("BlowDown");

   for (Integer i = FuelTankParamCount; i < ChemicalTankParamCount; ++i)
      parameterWriteOrder.push_back(i);
}


//------------------------------------------------------------------------------
//  ~ChemicalTank()
//------------------------------------------------------------------------------
/**
 * Fuel tank destructor.
 */
//------------------------------------------------------------------------------
ChemicalTank::~ChemicalTank()
{
}


//------------------------------------------------------------------------------
//  ChemicalTank(const ChemicalTank& ft)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * This method is called by the Clone method to replicate fuel tanks.
 *
 * @param ft Reference to the tank that gets replicated.
 */
//------------------------------------------------------------------------------
ChemicalTank::ChemicalTank(const ChemicalTank& ft) :
   FuelTank             (ft),
//   fuelMass             (ft.fuelMass),
   pressure             (ft.pressure),
   temperature          (ft.temperature),
   refTemperature       (ft.refTemperature),
   volume               (ft.volume),
   density              (ft.density),
//   allowNegativeFuelMass(ft.allowNegativeFuelMass),
   pressureModel        (ft.pressureModel),
   gasVolume            (ft.gasVolume),
   pvBase               (ft.pvBase)
{
   parameterCount = ft.parameterCount;
   isInitialized  = Initialize();

   for (Integer i = FuelTankParamCount; i < ChemicalTankParamCount; ++i)
      parameterWriteOrder.push_back(i);
}


//------------------------------------------------------------------------------
//  ChemicalTank& operator=(const ChemicalTank& ft)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * Sets the parameters for one tank equal to anothers.
 *
 * @param ft Reference to the tank that gets replicated.
 */
//------------------------------------------------------------------------------
ChemicalTank& ChemicalTank::operator=(const ChemicalTank& ft)
{
   #ifdef DEBUG_TANK_SETTING
      MessageInterface::ShowMessage("Calling assignment operator on %s\n",
            instanceName.c_str());
   #endif
   if (&ft != this)
   {
      FuelTank::operator=(ft);

//      fuelMass              = ft.fuelMass;
      pressure              = ft.pressure;
      temperature           = ft.temperature;
      refTemperature        = ft.refTemperature;
      volume                = ft.volume;
      density               = ft.density;
//      allowNegativeFuelMass = ft.allowNegativeFuelMass;
      pressureModel         = ft.pressureModel;
      gasVolume             = ft.gasVolume;
      pvBase                = ft.pvBase;

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
std::string ChemicalTank::GetParameterText(const Integer id) const
{
   if (id >= FuelTankParamCount && id < ChemicalTankParamCount)
      return PARAMETER_TEXT[id - FuelTankParamCount];
   return FuelTank::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ChemicalTank::GetParameterUnit(const Integer id) const
{
   switch (id)
   {
//   case FUEL_MASS:
//      return "kg";
   case PRESSURE:
      return "kPa";
   case TEMPERATURE:
   case REFERENCE_TEMPERATURE:
      return "C";
   case VOLUME:
      return "m^3";
   case FUEL_DENSITY:
      return "kg/m^3";
   default:
      return FuelTank::GetParameterUnit(id);
   }
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
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
Integer ChemicalTank::GetParameterID(const std::string &str) const
{
   for (Integer i = FuelTankParamCount; i < ChemicalTankParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - FuelTankParamCount])
         return i;
   }

   return FuelTank::GetParameterID(str);
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
Gmat::ParameterType ChemicalTank::GetParameterType(const Integer id) const
{
   if (id >= FuelTankParamCount && id < ChemicalTankParamCount)
      return PARAMETER_TYPE[id - FuelTankParamCount];

   return FuelTank::GetParameterType(id);
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
std::string ChemicalTank::GetParameterTypeString(const Integer id) const
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
bool ChemicalTank::IsParameterReadOnly(const Integer id) const
{
   if (id == PRESSURE_REGULATED)
      return true;

   return FuelTank::IsParameterReadOnly(id);
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
bool ChemicalTank::IsParameterCommandModeSettable(const Integer id) const
{
//   if ((id == ALLOW_NEGATIVE_FUEL_MASS) ||
//       (id == PRESSURE_MODEL) ||
//       (id == PRESSURE_REGULATED))
   if ((id == PRESSURE_MODEL) ||
       (id == PRESSURE_REGULATED) ||
       (id == ALLOW_NEGATIVE_FUEL_MASS) ||
       (id == REFERENCE_TEMPERATURE) ||
       (id == VOLUME) ||
       (id == FUEL_DENSITY))
      return false;

   // Activate all of the other tank specific IDs  // not sure what to do here ...
   if (id >= FuelTankParamCount)
      return true;

   return FuelTank::IsParameterCommandModeSettable(id);
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
Real ChemicalTank::GetRealParameter(const Integer id) const
{
   switch (id)
   {
//      case FUEL_MASS:
//         return fuelMass;
//
      case PRESSURE:
         return pressure;

      case TEMPERATURE:
         return temperature;

      case REFERENCE_TEMPERATURE:
         return refTemperature;

      case VOLUME:
         return volume;

      case FUEL_DENSITY:
         return density;

      default:
         break;   // Default just drops through
   }
   return FuelTank::GetRealParameter(id);
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
Real ChemicalTank::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_FUELTANK_SET
   MessageInterface::ShowMessage
      ("ChemicalTank::SetRealParameter(), id=%d, value=%f\n", id, value);
   #endif

   switch (id)
   {
//      case FUEL_MASS:
//         {
//            #ifdef DEBUG_TANK_SETTING
//               MessageInterface::ShowMessage("Updating fuel mass to %lf\n",
//                     value);
//            #endif
//            if (value >= 0.0 || allowNegativeFuelMass)
//            {
//               fuelMass = value;
//               UpdateTank();
//               //isInitialized = false;
//               return fuelMass;
//            }
//            else
//            {
//               HardwareException hwe("");
//               hwe.SetDetails(errorMessageFormat.c_str(),
//                              GmatStringUtil::ToString(value, 16).c_str(),
//                              "FuelMass", "Real Number >= 0.0");
//               throw hwe;
//            }
//         }
//
      case PRESSURE:

         if (value >= 0.0)
            pressure = value;
         else
         {
            HardwareException hwe("");
            hwe.SetDetails(errorMessageFormat.c_str(),
                           GmatStringUtil::ToString(value, 16).c_str(),
                           "Pressure", "Real Number >= 0.0");
            throw hwe;
         }
         isInitialized = false;
         return pressure;

      case TEMPERATURE:
         if (value > GmatPhysicalConstants::ABSOLUTE_ZERO_C)
            temperature = value;
         else
         {
            HardwareException hwe("");
            std::stringstream ss("");
            ss << "Real Number > " << GmatPhysicalConstants::ABSOLUTE_ZERO_C;
            hwe.SetDetails(errorMessageFormat.c_str(),
                           GmatStringUtil::ToString(value, 16).c_str(),
                           "Temperature", (ss.str()).c_str());
            throw hwe;
         }
         isInitialized = false;
         return temperature;

      case REFERENCE_TEMPERATURE:
         if (value > GmatPhysicalConstants::ABSOLUTE_ZERO_C)
         {
            if (GmatMathUtil::Abs(value) <= ZERO_REFERENCE_TEMPERATURE_THRESHOLD)
            {
               HardwareException hwe("");
               std::stringstream ss("");
               ss << "Real Number > " << ZERO_REFERENCE_TEMPERATURE_THRESHOLD
                  << " or Real Number < -"
                  << ZERO_REFERENCE_TEMPERATURE_THRESHOLD;
               hwe.SetDetails(errorMessageFormat.c_str(),
                              GmatStringUtil::ToString(value, 16).c_str(),
                              "RefTemperature", (ss.str()).c_str());
               throw hwe;
            }
            refTemperature = value;
         }
         else
         {
            HardwareException hwe("");
            std::stringstream ss("");
            ss << "Real Number > " << GmatPhysicalConstants::ABSOLUTE_ZERO_C;
            hwe.SetDetails(errorMessageFormat.c_str(),
                           GmatStringUtil::ToString(value, 16).c_str(),
                           "RefTemperature", (ss.str()).c_str());
            throw hwe;
         }
         isInitialized = false;
         return refTemperature;

      case VOLUME:

         if (value >= 0.0)
            volume = value;
         else
         {
            HardwareException hwe("");
            hwe.SetDetails(errorMessageFormat.c_str(),
                           GmatStringUtil::ToString(value, 16).c_str(),
                           "Volume", "Real Number >= 0.0");
            throw hwe;
         }
         isInitialized = false;
         return volume;

      case FUEL_DENSITY:

         if (value > 0.0)
            density = value;
         else
         {
            HardwareException hwe("");
            hwe.SetDetails(errorMessageFormat.c_str(),
                           GmatStringUtil::ToString(value, 16).c_str(),
                           "FuelDensity", "Real Number > 0.0");
            throw hwe;
         }
         isInitialized = false;
         return density;

      default:
         break;   // Default just drops through
   }

   return FuelTank::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
Real ChemicalTank::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//---------------------------------------------------------------------------
Real ChemicalTank::SetRealParameter(const std::string &label, const Real value)
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
bool ChemicalTank::GetBooleanParameter(const Integer id) const
{
   if (id == PRESSURE_REGULATED)
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** \"PressureRegulated\" is deprecated and will be "
          "removed from a future build; please use \"PressureModel\" "
          "instead.\n");
      return true;
   }
//   else if (id == ALLOW_NEGATIVE_FUEL_MASS)
//   {
//      return allowNegativeFuelMass;
//   }

   return FuelTank::GetBooleanParameter(id);
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
bool ChemicalTank::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == PRESSURE_REGULATED)
   {
      if (value)
         pressureModel = TPM_PRESSURE_REGULATED;
      else
         pressureModel = TPM_BLOW_DOWN;

      MessageInterface::ShowMessage
         ("*** WARNING *** \"PressureRegulated\" is deprecated and will be "
          "removed from a future build; please use \"PressureModel\" "
          "instead.\n");
      return value;
   }
//   else if (id == ALLOW_NEGATIVE_FUEL_MASS)
//   {
//      allowNegativeFuelMass = value;
//      return true;
//   }

   return FuelTank::SetBooleanParameter(id, value);
}


//---------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string ChemicalTank::GetStringParameter(const Integer id) const
{
   if (id == PRESSURE_MODEL)
      return pressureModelList[pressureModel];

   return FuelTank::GetStringParameter(id);
}


//---------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool ChemicalTank::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_FUELTANK_SET
   MessageInterface::ShowMessage
      ("ChemicalTank::SetStringParameter() entered, id=%d, value='%s'\n", id,
       value.c_str());
   #endif

   if (id == PRESSURE_MODEL)
   {
      if (find(pressureModelList.begin(), pressureModelList.end(), value) !=
          pressureModelList.end())
      {
         for (UnsignedInt i=0; i<pressureModelList.size(); i++)
            if (value == pressureModelList[i])
               pressureModel = i;
      }
      else
      {
         // write one warning per GMAT session
         static bool firstTimeWarning = true;
         std::string framelist = pressureModelList[0];
         for (UnsignedInt n = 1; n < pressureModelList.size(); ++n)
            framelist += ", " + pressureModelList[n];

         std::string msg =
            "The value of \"" + value + "\" for field \"PressureModel\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [ " + framelist + " ]. ";

         if (firstTimeWarning)
         {
            firstTimeWarning = false;
            throw HardwareException(msg);
         }
      }

      return true;
   }

   return FuelTank::SetStringParameter(id, value);
}


//---------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//---------------------------------------------------------------------------
std::string ChemicalTank::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//---------------------------------------------------------------------------
bool ChemicalTank::SetStringParameter(const std::string &label,
                                  const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
const StringArray& ChemicalTank::GetPropertyEnumStrings(const Integer id) const
{
   if (id == PRESSURE_MODEL)
      return pressureModelList;

   return FuelTank::GetPropertyEnumStrings(id);
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const std::string &label) const
//---------------------------------------------------------------------------
const StringArray& ChemicalTank::GetPropertyEnumStrings(const std::string &label) const
{
   return GetPropertyEnumStrings(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  GmatBase* Clone() const
//---------------------------------------------------------------------------
/**
 * Provides a clone of this object by calling the copy constructor.
 *
 * @return A GmatBase pointer to the cloned fuel tank.
 */
//---------------------------------------------------------------------------
GmatBase* ChemicalTank::Clone() const
{
   return new ChemicalTank(*this);
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
void ChemicalTank::Copy(const GmatBase* orig)
{
   operator=(*((ChemicalTank *)(orig)));
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Fills in the data needed for internal calculations of fuel depletion.
 */
//------------------------------------------------------------------------------
bool ChemicalTank::Initialize()
{
   if (!FuelTank::Initialize())
      return false;

   isInitialized = false;

   if (Validate())
   {
      firstValidation = false;
      gasVolume = volume - fuelMass / density;
      pvBase = pressure * gasVolume;
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
void ChemicalTank::UpdateTank()
{
   if (pressureModel != TPM_PRESSURE_REGULATED)
   {
      #ifdef DEBUG_MASS_FLOW
         MessageInterface::ShowMessage("Vol = %.12lf, fuelMass = %.12lf, "
               "density = %.12lf, P0 = %.12lf, PV = %.12lf", volume, fuelMass,
               density, pressure, pvBase);
      #endif
      if (!isInitialized)
      {
         throw HardwareException("The chemical tank \"" + instanceName +
            "\" cannot be updated, the hardware has not been initialized.\n");
      }

      gasVolume = volume - fuelMass / density;

//      if (gasVolume < 0.0)
//         throw HardwareException("*** ERROR *** The fuel volume in the "
//               "tank \"" + instanceName + "\" exceeds the volume of the tank.");

      pressure = pvBase / gasVolume;

      #ifdef DEBUG_MASS_FLOW
         MessageInterface::ShowMessage("Gas Vol = %.12lf, Pf = %.12lf\n",
               gasVolume, pvBase);
      #endif
   }
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
void ChemicalTank::DepleteFuel(Real dm)
{
   fuelMass -= dm;

   if (fuelMass < 0.0)
      // For now, throw if the fuel goes below 0
      throw HardwareException("Fuel in tank " + instanceName +
                              " completely exhausted.\n");
}

bool ChemicalTank::Validate()
{
   if (density <= 0.0)
	  return false;
   if ((volume - fuelMass / density) < 0.0)
	   throw HardwareException("Fuel volume exceeds tank capacity\n");
   return true;
}

