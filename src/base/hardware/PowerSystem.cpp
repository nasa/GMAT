//$Id$
//------------------------------------------------------------------------------
//                               PowerSystem
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
// Author: Wendy C. Shoan
// Created: 2014.04.28
//
//
/**
 * Class implementation for the Power System.
 */
//------------------------------------------------------------------------------


#include "PowerSystem.hpp"
#include "StringUtil.hpp"          // for GmatStringUtil
#include "Spacecraft.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "TimeSystemConverter.hpp"
#include "GmatConstants.hpp"
#include "GregorianDate.hpp"
#include "DateUtil.hpp"
#include <sstream>

//#define DEBUG_POWER_SYSTEM
//#define DEBUG_POWER_SYSTEM_SET
//#define DEBUG_DATE_FORMAT
//#define DEBUG_POWER_SYSTEM_SC_SUN

//---------------------------------
// static data
//---------------------------------

/// Labels used for the power system parameters.
const std::string
PowerSystem::PARAMETER_TEXT[PowerSystemParamCount - HardwareParamCount] =
{
   "EpochFormat",
   "InitialEpoch",
   "InitialMaxPower",
   "AnnualDecayRate",
   "Margin",
   "BusCoeff1",
   "BusCoeff2",
   "BusCoeff3",
   "TotalPowerAvailable",  // ?? need these three or add to CalculationUtilities?
   "RequiredBusPower",
   "ThrustPowerAvailable",
};

/// Types of the parameters used by fuel tanks.
const Gmat::ParameterType
PowerSystem::PARAMETER_TYPE[PowerSystemParamCount - HardwareParamCount] =
{
   Gmat::STRING_TYPE,      // "EpochFormat",
   Gmat::STRING_TYPE,      // "InitialEpoch",
   Gmat::REAL_TYPE,        // "InitialMaxPower",
   Gmat::REAL_TYPE,        // "AnnualDecayRate",
   Gmat::REAL_TYPE,        // "Margin",
   Gmat::REAL_TYPE,        // "BusCoeff1",
   Gmat::REAL_TYPE,        // "BusCoeff2",
   Gmat::REAL_TYPE,        // "BusCoeff3",
   Gmat::REAL_TYPE,        // "TotalPowerAvailable",
   Gmat::REAL_TYPE,        // "RequiredBusPower",
   Gmat::REAL_TYPE,        // "ThrustPowerAvailable",
};


//------------------------------------------------------------------------------
//  PowerSystem()
//------------------------------------------------------------------------------
/**
 * Power System constructor.
 *
 * @param systemType  type of power system
 * @param nomme       name for the power system.
 */
//------------------------------------------------------------------------------
PowerSystem::PowerSystem(const std::string &systemType, const std::string &nomme) :
   Hardware             (Gmat::POWER_SYSTEM, systemType, nomme),
   epochFormat          ("UTCGregorian"),
   initialEpoch         ("01 Jan 2000 11:59:28.000"),
   initialMaxPower      (1.2),
   annualDecayRate      (5.0),
   margin               (5.0),
   busCoeff1            (0.3),
   busCoeff2            (0.0),
   busCoeff3            (0.0),
   initialEp            (21545.0),
   solarSystem          (NULL),
   sun                  (NULL),
   spacecraft           (NULL),
   scOrigin             (NULL),
   sunRadius            (GmatSolarSystemDefaults::STAR_EQUATORIAL_RADIUS)
{
   objectTypes.push_back(Gmat::POWER_SYSTEM);
   objectTypeNames.push_back("PowerSystem");

   parameterCount = PowerSystemParamCount;
   theTimeConverter = TimeSystemConverter::Instance();

   for (Integer i = HardwareParamCount; i < PowerSystemParamCount; ++i)
      parameterWriteOrder.push_back(i);
}


//------------------------------------------------------------------------------
//  ~PowerSystem()
//------------------------------------------------------------------------------
/**
 * Power System destructor.
 */
//------------------------------------------------------------------------------
PowerSystem::~PowerSystem()
{
}


//------------------------------------------------------------------------------
//  PowerSystem(const PowerSystem& copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * This method is called by the Clone method to replicate power systems.
 *
 * @param copy Reference to the system that gets replicated.
 */
//------------------------------------------------------------------------------
PowerSystem::PowerSystem(const PowerSystem& copy) :
   Hardware             (copy),
   epochFormat          (copy.epochFormat),
   initialEpoch         (copy.initialEpoch),
   initialMaxPower      (copy.initialMaxPower),
   annualDecayRate      (copy.annualDecayRate),
   margin               (copy.margin),
   busCoeff1            (copy.busCoeff1),
   busCoeff2            (copy.busCoeff2),
   busCoeff3            (copy.busCoeff3),
   initialEp            (copy.initialEp),
   solarSystem          (NULL),
   sun                  (NULL),
   spacecraft           (NULL),
   scOrigin             (NULL),
   sunRadius            (copy.sunRadius)
{
   theTimeConverter = copy.theTimeConverter;
   for (Integer i = HardwareParamCount; i < PowerSystemParamCount; ++i)
      parameterWriteOrder.push_back(i);
}


//------------------------------------------------------------------------------
//  PowerSystem& operator=(const PowerSystem& copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * Sets the parameters for one power system equal to another's.
 *
 * @param copy Reference to the system that gets replicated.
 */
//------------------------------------------------------------------------------
PowerSystem& PowerSystem::operator=(const PowerSystem& copy)
{
   #ifdef DEBUG_POWER_SYSTEM_SET
      MessageInterface::ShowMessage("Calling assignment operator on %s\n",
            instanceName.c_str());
   #endif
   if (&copy != this)
   {
      Hardware::operator=(copy);

      initialMaxPower       = copy.initialMaxPower;
      epochFormat           = copy.epochFormat;
      initialEpoch          = copy.initialEpoch;
      annualDecayRate       = copy.annualDecayRate;
      margin                = copy.margin;
      busCoeff1             = copy.busCoeff1;
      busCoeff2             = copy.busCoeff2;
      busCoeff3             = copy.busCoeff3;
      initialEp             = copy.initialEp;
      solarSystem           = NULL;
      sun                   = NULL;
      spacecraft            = NULL;
      scOrigin              = NULL;
      sunRadius             = copy.sunRadius;
      theTimeConverter      = copy.theTimeConverter;
   }

   return *this;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the Power System.
 */
//------------------------------------------------------------------------------
bool PowerSystem::Initialize()
{
//   if (!solarSystem)
//   {
//      std::string errmsg = "No solar system set on Power System ";
//      errmsg += instanceName + ".\n";
//      throw HardwareException(errmsg);
//   }

   // Spacecraft pointer will not be set until the PowerSystem is
   // attached to a Spacecraft (the Spacecraft passes its own
   // pointer to the PowerSystem).

//   initialEp   = EpochToReal(initialEpoch);  // already done

   isInitialized = true;

   return isInitialized;
}

//---------------------------------------------------------------------------
//  void Copy(GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 *
 * @return A GmatBase pointer to the cloned power system.
 */
//---------------------------------------------------------------------------
void PowerSystem::Copy(const GmatBase* orig)
{
   operator=(*((PowerSystem *)(orig)));
}

//------------------------------------------------------------------------------
//  void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void PowerSystem::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
   sun         = solarSystem->GetBody(GmatSolarSystemDefaults::SUN_NAME);
   sunRadius   = sun->GetEquatorialRadius();
}

//------------------------------------------------------------------------------
//  void SetSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
void PowerSystem::SetSpacecraft(Spacecraft *sc)
{
   if (!sc)
   {
      std::string errmsg = "Cannot set spacecraft on Power System ";
      errmsg += instanceName + ": sc is NULL\n";
      throw HardwareException(errmsg);
   }
   spacecraft = sc;
   scOrigin   = sc->GetOrigin();
}

//------------------------------------------------------------------------------
//  bool TakeAction(const std::string &action,
//                  const std::string &actionData = "")
//------------------------------------------------------------------------------
bool PowerSystem::TakeAction(const std::string &action,
                             const std::string &actionData)
{
   return true;  // default action is to do nothing
}


//------------------------------------------------------------------------------
//  Real GetBasePower()
//------------------------------------------------------------------------------
Real PowerSystem::GetBasePower() const
{
   Real atEpoch        = spacecraft->GetEpoch();
   Real yearsFromStart = (atEpoch - initialEp) / GmatTimeConstants::DAYS_PER_YEAR;

   #ifdef DEBUG_POWER_SYSTEM
      MessageInterface::ShowMessage(
            "In PowerSystem::GetBasePower, atEpoch = %12.10f, initialEp = %12.10f\n",
            atEpoch, initialEp);
      MessageInterface::ShowMessage("yearsFromStart = %12.10f\n", yearsFromStart);
   #endif
   // Englander Eq. 18
   Real basePower = initialMaxPower *
         GmatMathUtil::Pow((1 - annualDecayRate/100.00),yearsFromStart);
   return basePower;
}

//------------------------------------------------------------------------------
//  Real GetSpacecraftBusPower()
//------------------------------------------------------------------------------
Real PowerSystem::GetSpacecraftBusPower() const
{
   Real     atEpoch    = spacecraft->GetEpoch();
   Real     *state     = (spacecraft->GetState()).GetState();

   Real sunDist  = GetSunToSCDistance(atEpoch);

   // Englander: Eq. 19
   Real busPower = busCoeff1 + (busCoeff2/sunDist) +
                  (busCoeff3/(sunDist*sunDist));
   #ifdef DEBUG_POWER_SYSTEM
      MessageInterface::ShowMessage(
            "In PowerSystem::GetSCBasePower, busPower = %12.10f\n",
            busPower);
   #endif
   return busPower;
}

//------------------------------------------------------------------------------
//  Real GetThrustPower()
//------------------------------------------------------------------------------
Real PowerSystem::GetThrustPower() const
{
   Real atEpoch        = spacecraft->GetEpoch();
   Real powerGenerated = GetPowerGenerated();
   Real busPower       = GetSpacecraftBusPower();

   //  Englander: Eq. 16
   Real powerAvailable = (1-margin /100.00) * (powerGenerated - busPower);
   if (powerAvailable < 0)
       powerAvailable = 0;
   #ifdef DEBUG_POWER_SYSTEM
      MessageInterface::ShowMessage(
            "In PowerSystem::GetThrustPower, powerAvailable = %12.10f\n",
            powerAvailable);
   #endif
   return powerAvailable;
}

//------------------------------------------------------------------------------
//  void SetEpoch(std::string ep)
//------------------------------------------------------------------------------
/**
 * Set the epoch.
 *
 * @param <ep> The new epoch.
 */
//------------------------------------------------------------------------------
void PowerSystem::SetEpoch(const std::string &ep)
{
   #ifdef DEBUG_DATE_FORMAT
   MessageInterface::ShowMessage
      ("PowerSystem::SetEpoch() Setting epoch  for spacecraft %s to %s\n",
       instanceName.c_str(), ep.c_str());
   #endif

   std::string timeSystem;
   std::string timeFormat;
   theTimeConverter->GetTimeSystemAndFormat(epochFormat, timeSystem, timeFormat);
   if (timeFormat == "ModJulian") // numeric - save and output without quotes
      initialEpoch = GmatStringUtil::RemoveEnclosingString(ep, "'");
   else // "Gregorian" - not numeric - save and output with quotes
   {
      if (!GmatStringUtil::IsEnclosedWith(ep, "'"))
         initialEpoch = GmatStringUtil::AddEnclosingString(ep, "'");
      else
         initialEpoch = ep;
   }

   #ifdef DEBUG_DATE_FORMAT
   MessageInterface::ShowMessage
      ("PowerSystem::SetEpoch() Calling EpochToReal with %s\n",
       initialEpoch.c_str());
   #endif
   initialEp = EpochToReal(initialEpoch);
   #ifdef DEBUG_DATE_FORMAT
   MessageInterface::ShowMessage
      ("PowerSystem::SetEpoch() Setting initialEp (A1Mjd) to %12.15f\n",
       initialEp);
   #endif
}

//------------------------------------------------------------------------------
// std::string GetEpochString()
//------------------------------------------------------------------------------
std::string PowerSystem::GetEpochString()
{
   Real outMjd = -999.999;
   std::string outStr;

   theTimeConverter->Convert("A1ModJulian", initialEp, "",
                              epochFormat, outMjd, outStr);

   return outStr;
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
std::string PowerSystem::GetParameterText(const Integer id) const
{
   if (id >= HardwareParamCount && id < PowerSystemParamCount)
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
std::string PowerSystem::GetParameterUnit(const Integer id) const
{
   switch (id)
   {
   case INITIAL_MAX_POWER:
      return "kW";
   case ANNUAL_DECAY_RATE:
      return "percent/year";
   case MARGIN:
      return "percent";
   case BUS_COEFF1:
      return "kW";
   case BUS_COEFF2:
      return "kW*AU";
   case BUS_COEFF3:
      return "kW*AU^2";
   default:
      return Hardware::GetParameterUnit(id);
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
Integer PowerSystem::GetParameterID(const std::string &str) const
{
   for (Integer i = HardwareParamCount; i < PowerSystemParamCount; i++)
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
Gmat::ParameterType PowerSystem::GetParameterType(const Integer id) const
{
   if (id >= HardwareParamCount && id < PowerSystemParamCount)
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
std::string PowerSystem::GetParameterTypeString(const Integer id) const
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
bool PowerSystem::IsParameterReadOnly(const Integer id) const
{
   // These are output only, computed quantities for use as parameters
   if ((id == TOTAL_POWER_AVAILABLE) || (id == REQUIRED_BUS_POWER) ||
       (id == THRUST_POWER_AVAILABLE))
      return true;

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
bool PowerSystem::IsParameterCommandModeSettable(const Integer id) const
{
   // These are output only, computed quantities for use as parameters
   if ((id == TOTAL_POWER_AVAILABLE) || (id == REQUIRED_BUS_POWER) ||
       (id == THRUST_POWER_AVAILABLE))
      return false;

   return true;   // all settable for now
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
Real PowerSystem::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case INITIAL_MAX_POWER:
         return initialMaxPower;

      case ANNUAL_DECAY_RATE:
         return annualDecayRate;

      case MARGIN:
         return margin;

      case BUS_COEFF1:
         return busCoeff1;

      case BUS_COEFF2:
         return busCoeff2;

      case BUS_COEFF3:
         return busCoeff3;

      case TOTAL_POWER_AVAILABLE:
         return GetPowerGenerated();

      case REQUIRED_BUS_POWER:
         return GetSpacecraftBusPower();

      case THRUST_POWER_AVAILABLE:
         return GetThrustPower();

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
 * This code checks the validity of selected power system parameters.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real PowerSystem::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_POWER_SYSTEM_SET
   MessageInterface::ShowMessage
      ("PowerSystem::SetRealParameter(), id=%d, value=%f\n", id, value);
   #endif

   switch (id)
   {
      case INITIAL_MAX_POWER:
         {
            #ifdef DEBUG_POWER_SYSTEM_SET
               MessageInterface::ShowMessage("Updating initial max power to %lf\n",
                     value);
            #endif
            if (value >= 0.0)
            {
               initialMaxPower = value;
               return initialMaxPower;
            }
            else
            {
               HardwareException hwe("");
               hwe.SetDetails(errorMessageFormat.c_str(),
                              GmatStringUtil::ToString(value, 16).c_str(),
                              "InitalMaxPower", "Real Number >= 0.0");
               throw hwe;
            }
         }

      case ANNUAL_DECAY_RATE:

         if ((value >= 0.0) && (value <= 100.0))
            annualDecayRate = value;
         else
         {
            HardwareException hwe("");
            hwe.SetDetails(errorMessageFormat.c_str(),
                           GmatStringUtil::ToString(value, 16).c_str(),
                           "AnnuaDecayRate", "0 <= Real Number <= 100");
            throw hwe;
         }
         return annualDecayRate;

      case MARGIN:
         if ((value >= 0.0) && (value <= 100.0))
            margin = value;
         else
         {
            HardwareException hwe("");
            hwe.SetDetails(errorMessageFormat.c_str(),
                           GmatStringUtil::ToString(value, 16).c_str(),
                           "Margin", "0 <= Real Number <= 100");
            throw hwe;
         }
         return margin;

      case BUS_COEFF1:
         busCoeff1 = value;
         return busCoeff1;

      case BUS_COEFF2:
         busCoeff2 = value;
         return busCoeff2;

      case BUS_COEFF3:
         busCoeff3 = value;
         return busCoeff3;

      default:
         break;   // Default just drops through
   }

   return Hardware::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
Real PowerSystem::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//---------------------------------------------------------------------------
Real PowerSystem::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string PowerSystem::GetStringParameter(const Integer id) const
{
   if (id == EPOCH_FORMAT)
      return epochFormat;

   if (id == INITIAL_EPOCH)
      return initialEpoch;

   return Hardware::GetStringParameter(id);
}


//---------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool PowerSystem::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_POWER_SYSTEM_SET
   MessageInterface::ShowMessage
      ("PowerSystem::SetStringParameter() entered, id=%d, value='%s'\n", id,
       value.c_str());
   #endif

   if (id == EPOCH_FORMAT)
   {
      if (theTimeConverter->IsValidTimeSystem(value))
      {
         epochFormat = value;
         return true;
      }
      else
      {
         HardwareException hwe("");
         hwe.SetDetails(errorMessageFormat.c_str(),
                        value.c_str(),
                        "EpochFormat", "Valid Time Format");
         throw hwe;
      }
   }
   if (id == INITIAL_EPOCH)
   {
      SetEpoch(value);
      return true;
   }

   return Hardware::SetStringParameter(id, value);
}


//---------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//---------------------------------------------------------------------------
std::string PowerSystem::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//---------------------------------------------------------------------------
bool PowerSystem::SetStringParameter(const std::string &label,
                                  const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id,const Integer index) const
//---------------------------------------------------------------------------
std::string PowerSystem::GetStringParameter(const Integer id,
                                            const Integer index) const
{
   return Hardware::GetStringParameter(id, index);
}

//---------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//---------------------------------------------------------------------------
bool PowerSystem::SetStringParameter(const Integer id,
                                     const std::string &value,
                                     const Integer index)
{

   return Hardware::SetStringParameter(id, value, index);
}



//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Real EpochToReal(const std::string &ep)
//------------------------------------------------------------------------------
Real PowerSystem::EpochToReal(const std::string &ep)
{
   Real fromMjd = -999.999;
   Real outMjd = -999.999;
   std::string outStr;

   // remove enclosing quotes for the validation and conversion
   std::string epNoQuote = GmatStringUtil::RemoveEnclosingString(ep, "'");

   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage
         ("PowerSystem::EpochToReal() Converting from %s to A1ModJulian\n", epochFormat.c_str());
   #endif

   if (epochFormat.find("Gregorian") != std::string::npos)
   {
      if (!GregorianDate::IsValid(epNoQuote))
      {
         std::string errmsg = "PowerSystem error: epoch ";
         errmsg += ep + " is not a valid Gregorian date.\n";
         throw HardwareException(errmsg);
      }
   }

   theTimeConverter->Convert(epochFormat, fromMjd, epNoQuote, "A1ModJulian", outMjd,
                              outStr);
   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage
         ("PowerSystem::EpochToReal() Done converting from %s to A1ModJulian\n", epochFormat.c_str());
   #endif

   if (outMjd == -999.999)
   {
      #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage("PowerSystem::EpochToReal() oops!  outMjd = -999.999!!\n");
      #endif
   }
   return outMjd;
}

//------------------------------------------------------------------------------
//  Real GetSunToSCDistance(Real atEpoch)
//------------------------------------------------------------------------------
Real PowerSystem::GetSunToSCDistance(Real atEpoch) const
{
   Real     *state     = (spacecraft->GetState()).GetState(); // wrt Earth NOT wrt Origin

   Rvector3 sunState   = sun->GetMJ2000Position(atEpoch);     // wrt Earth
//   Rvector3 origState  = scOrigin->GetMJ2000Position(atEpoch);// wrt Earth
//   Rvector3 sunToOrig  = origState - sunState;
//   Rvector3 scToSun(state[0] + sunToOrig[0], state[1] + sunToOrig[1],
//                    state[2] + sunToOrig[2]);
   Rvector3 sunToSC(state[0] - sunState[0], state[1] - sunState[1],
                    state[2] - sunState[2]);

//   Real sunSCDist      = scToSun.GetMagnitude() /
   Real sunSCDist      = sunToSC.GetMagnitude() /
                         GmatPhysicalConstants::ASTRONOMICAL_UNIT;
   #ifdef DEBUG_POWER_SYSTEM_SC_SUN
      MessageInterface::ShowMessage("In GetSunToSCDistance for Spacecraft %s...\n",
            spacecraft->GetName().c_str());
      MessageInterface::ShowMessage("state      = %12.10f  %12.10f  %12.10f\n",
             state[0], state[1], state[2]);
//      MessageInterface::ShowMessage("origState (%s)  = %12.10f  %12.10f  %12.10f\n",
//            scOrigin->GetName().c_str(), origState[0], origState[1], origState[2]);
      MessageInterface::ShowMessage("sunState   = %12.10f  %12.10f  %12.10f\n",
             sunState[0], sunState[1], sunState[2]);
//      MessageInterface::ShowMessage("scToSun    = %12.10f  %12.10f  %12.10f\n",
//            scToSun[0], scToSun[1], scToSun[2]);
      MessageInterface::ShowMessage("sunToSC    = %12.10f  %12.10f  %12.10f\n",
            sunToSC[0], sunToSC[1], sunToSC[2]);
       MessageInterface::ShowMessage("sunSCDist = %12.10f\n", sunSCDist);
   #endif
   return sunSCDist;
}

