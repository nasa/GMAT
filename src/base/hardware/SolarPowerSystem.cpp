//$Id$
//------------------------------------------------------------------------------
//                               SolarPowerSystem
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
// Created: 2014.05.01
//
//
/**
 * Class implementation for the Solar Power System.
 */
//------------------------------------------------------------------------------


#include "SolarPowerSystem.hpp"
#include "StringUtil.hpp"          // for GmatStringUtil
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "Spacecraft.hpp"

//#define DEBUG_SOLAR_POWER
//#define DEBUG_SOLAR_POWER_SYSTEM_SET
//#define DEBUG_SOLAR_RENAME
//#define DEBUG_SOLAR_POWER_OBJECT
//#define DEBUG_SET
//#define DEBUG_SOLAR_POWER_PERCENT


//---------------------------------
// static data
//---------------------------------

/// Labels used for the power system parameters.
const std::string
SolarPowerSystem::PARAMETER_TEXT[SolarPowerSystemParamCount - PowerSystemParamCount] =
{
      "ShadowModel",
      "ShadowBodies",
      "SolarCoeff1",
      "SolarCoeff2",
      "SolarCoeff3",
      "SolarCoeff4",
      "SolarCoeff5",
};

/// Types of the parameters used by fuel tanks.
const Gmat::ParameterType
SolarPowerSystem::PARAMETER_TYPE[SolarPowerSystemParamCount - PowerSystemParamCount] =
{
      Gmat::STRING_TYPE,      // "ShadowModel"
      Gmat::STRINGARRAY_TYPE, // "ShadowBodies" // was OBJECTARRAY_TYPE
      Gmat::REAL_TYPE,        // "SolarCoeff1",
      Gmat::REAL_TYPE,        // "SolarCoeff2",
      Gmat::REAL_TYPE,        // "SolarCoeff3",
      Gmat::REAL_TYPE,        // "SolarCoeff4",
      Gmat::REAL_TYPE,        // "SolarCoeff5",
};

bool SolarPowerSystem::occultationWarningWritten = false;


//------------------------------------------------------------------------------
//  SolarPowerSystem()
//------------------------------------------------------------------------------
/**
 * Nuclear Power System constructor.
 *
 * @param nomme Name for the power system.
 */
//------------------------------------------------------------------------------
SolarPowerSystem::SolarPowerSystem(const std::string &nomme) :
   PowerSystem          ("SolarPowerSystem",nomme),
   solarCoeff1          (1.32077),
   solarCoeff2          (-0.10848),
   solarCoeff3          (-0.11665),
   solarCoeff4          (0.10843),
   solarCoeff5          (-0.01279),
   shadowModel          ("DualCone"),
   shadowState          (NULL),
   settingNoBodies      (false)
{
   objectTypes.push_back(Gmat::SOLAR_POWER_SYSTEM);
   objectTypeNames.push_back("SolarPowerSystem");

   parameterCount = SolarPowerSystemParamCount;

   shadowBodyNames.clear();
   shadowBodies.clear();

   defaultShadowBodyNames.clear();
   defaultShadowBodyNames.push_back("Earth");

   // we need to make sure the shadow bodies are written correctly to the
   // script (even if it's empty)
   writeEmptyStringArray = true;
   shadowState    = new ShadowState();

   for (Integer i = PowerSystemParamCount; i < SolarPowerSystemParamCount; ++i)
      parameterWriteOrder.push_back(i);
}


//------------------------------------------------------------------------------
//  ~SolarPowerSystem()
//------------------------------------------------------------------------------
/**
 * Nuclear Power System destructor.
 */
//------------------------------------------------------------------------------
SolarPowerSystem::~SolarPowerSystem()
{
   if (shadowState) delete shadowState;
}


//------------------------------------------------------------------------------
//  SolarPowerSystem(const SolarPowerSystem& copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * This method is called by the Clone method to replicate power systems.
 *
 * @param copy Reference to the system that gets replicated.
 */
//------------------------------------------------------------------------------
SolarPowerSystem::SolarPowerSystem(const SolarPowerSystem& copy) :
   PowerSystem          (copy),
   solarCoeff1          (copy.solarCoeff1),
   solarCoeff2          (copy.solarCoeff2),
   solarCoeff3          (copy.solarCoeff3),
   solarCoeff4          (copy.solarCoeff4),
   solarCoeff5          (copy.solarCoeff5),
   shadowModel          (copy.shadowModel),
   shadowState          (NULL),
   settingNoBodies      (copy.settingNoBodies)
{
   parameterCount = copy.parameterCount;
//   isInitialized  = Initialize();

   shadowBodyNames.clear();
   shadowBodies.clear();
   // copy the list of body names
   for (unsigned int i = 0; i < (copy.shadowBodyNames).size(); i++)
   {
      shadowBodyNames.push_back((copy.shadowBodyNames).at(i));
   }
   defaultShadowBodyNames.clear();
   for (unsigned int j = 0; j < (copy.defaultShadowBodyNames).size(); j++)
   {
      defaultShadowBodyNames.push_back((copy.defaultShadowBodyNames).at(j));
   }
   // copy the list of body pointers - do I want to do this???
   for (unsigned int i = 0; i < (copy.shadowBodies).size(); i++)
   {
      shadowBodies.push_back((copy.shadowBodies).at(i));
   }

   shadowState    = new ShadowState();

   for (Integer i = PowerSystemParamCount; i < SolarPowerSystemParamCount; ++i)
      parameterWriteOrder.push_back(i);
}


//------------------------------------------------------------------------------
//  SolarPowerSystem& operator=(const SolarPowerSystem& copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * Sets the parameters for one power system equal to another's.
 *
 * @param copy Reference to the system that gets replicated.
 */
//------------------------------------------------------------------------------
SolarPowerSystem& SolarPowerSystem::operator=(const SolarPowerSystem& copy)
{
   #ifdef DEBUG_SOLAR_POWER
      MessageInterface::ShowMessage("Calling assignment operator on %s\n",
            instanceName.c_str());
   #endif
   if (&copy != this)
   {
      PowerSystem::operator=(copy);

      solarCoeff1       = copy.solarCoeff1;
      solarCoeff2       = copy.solarCoeff2;
      solarCoeff3       = copy.solarCoeff3;
      solarCoeff4       = copy.solarCoeff4;
      solarCoeff5       = copy.solarCoeff5;

      settingNoBodies   = copy.settingNoBodies;

      shadowBodyNames.clear();
      // copy the list of body names
      for (unsigned int i = 0; i < (copy.shadowBodyNames).size(); i++)
      {
         shadowBodyNames.push_back((copy.shadowBodyNames).at(i));
      }
      defaultShadowBodyNames.clear();
      // copy the list of body names
      for (unsigned int i = 0; i < (copy.defaultShadowBodyNames).size(); i++)
      {
         defaultShadowBodyNames.push_back((copy.defaultShadowBodyNames).at(i));
      }
      shadowBodies.clear();
      // copy the list of body pointers - do I want to do this???
      for (unsigned int i = 0; i < (copy.shadowBodies).size(); i++)
      {
         shadowBodies.push_back((copy.shadowBodies).at(i));
      }

      shadowModel = copy.shadowModel;

      if (shadowState)
      {
         delete shadowState;
         shadowState        = NULL;
      }

      shadowState           = new ShadowState();
   }

   #ifdef DEBUG_SOLAR_POWER
      MessageInterface::ShowMessage("Calling assignment operator on %s is EXITING\n",
            instanceName.c_str());
   #endif
   return *this;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the Nuclear Power System.
 */
//------------------------------------------------------------------------------
bool SolarPowerSystem::Initialize()
{
   #ifdef DEBUG_SOLAR_POWER
      MessageInterface::ShowMessage("Calling Initialization on %s\n",
            instanceName.c_str());
      MessageInterface::ShowMessage("number of shadow bodies = %d\n",
            (Integer) shadowBodyNames.size());
   #endif
   PowerSystem::Initialize();

   // Solar System is set by the spacecraft to which this is attached
   if (!solarSystem)
   {
      std::string errmsg = "SolarSystem has not been set on PowerSystem ";
      errmsg            += instanceName + ".\n";
      throw HardwareException(errmsg);
   }

   // if no names were added to the ShadowBodies list, add the Default body
   // This will cause "ShadowBodies = {'Earth'} to be written to the script <<
   if ((shadowBodyNames.empty()) && (!settingNoBodies))
      shadowBodyNames = defaultShadowBodyNames;
//      shadowBodyNames.push_back("Earth");

   // Set up the list of shadowBodies using current solarSystem
   shadowBodies.clear();
   for (unsigned int ii = 0; ii < shadowBodyNames.size(); ii++)
   {
      CelestialBody *body = solarSystem->GetBody(shadowBodyNames.at(ii));
      if (!body)
      {
         std::string errmsg = "SolarPowerSystem ";
         errmsg += instanceName + " cannot find body ";
         errmsg += shadowBodyNames.at(ii) + ".  ShadowBodies must be ";
         errmsg += "Celestial Bodies.\n";
         throw HardwareException(errmsg);
      }
      shadowBodies.push_back(body);
      #ifdef DEBUG_SOLAR_POWER
         MessageInterface::ShowMessage("Adding shadow body %s to %s\n",
               body->GetName().c_str(), instanceName.c_str());
      #endif
   }

   if (!shadowState)
      shadowState = new ShadowState();

   shadowState->SetSolarSystem(solarSystem);

   return isInitialized;
}

//---------------------------------------------------------------------------
//  GmatBase* Clone() const
//---------------------------------------------------------------------------
/**
 * Provides a clone of this object by calling the copy constructor.
 *
 * @return A GmatBase pointer to the cloned power system.
 */
//---------------------------------------------------------------------------
GmatBase* SolarPowerSystem::Clone() const
{
   return new SolarPowerSystem(*this);
}

//---------------------------------------------------------------------------
//  void Copy(GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 *
 * @return A GmatBase pointer to the cloned solar power system.
 */
//---------------------------------------------------------------------------
void SolarPowerSystem::Copy(const GmatBase* orig)
{
   operator=(*((SolarPowerSystem *)(orig)));
}

//------------------------------------------------------------------------------
//  void SetSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
void SolarPowerSystem::SetSpacecraft(Spacecraft *sc)
{
   PowerSystem::SetSpacecraft(sc);
//   shadowState->SetOrigin(scOrigin);
}


//------------------------------------------------------------------------------
//  bool TakeAction(const std::string &action,const std::string &actionData)
//------------------------------------------------------------------------------
bool SolarPowerSystem::TakeAction(const std::string &action,
                                  const std::string &actionData)
{
   if (action == "ClearShadowBodies")
   {
      shadowBodyNames.clear();
      shadowBodies.clear();
   }
   return true;
}



//------------------------------------------------------------------------------
//  Real GetPowerGenerated()
//------------------------------------------------------------------------------
Real SolarPowerSystem::GetPowerGenerated() const
{
   #ifdef DEBUG_SOLAR_POWER
      MessageInterface::ShowMessage("Calling GetPowerGenerated on %s\n",
            instanceName.c_str());
      MessageInterface::ShowMessage("ShadowModel = %s\n", shadowModel.c_str());
      MessageInterface::ShowMessage("number of shadow bodies = %d\n",
            (Integer) shadowBodies.size());
      MessageInterface::ShowMessage("number of shadow body NAMES = %d\n",
            (Integer) shadowBodyNames.size());
   #endif

   Real generatedPower = 0;

   // Get the spacecraft epoch and state
   Real atEpoch           = spacecraft->GetEpoch();

   Real *stateRelToEarth  = (spacecraft->GetState()).GetState();

   // Get the base power
   Real basePower      = GetBasePower();

   // Compute the solar scale factor
   Real sunDist = GetSunToSCDistance(atEpoch);

   // Compute Solar Panel model
   Real sunDist2 = sunDist * sunDist;
   Real num      = solarCoeff1 + (solarCoeff2 /sunDist) +
                   (solarCoeff3 / sunDist2);
   Real denom    = 1.0 + (solarCoeff4 * sunDist) +
                   (solarCoeff5 * sunDist2);

   Real solarScaleFactor = (1.0 / sunDist2) * (num / denom);

   #ifdef DEBUG_SOLAR_POWER
      MessageInterface::ShowMessage("atEpoch          = %12.10f\n", atEpoch);
      MessageInterface::ShowMessage("basePower        = %12.10f\n", basePower);
      MessageInterface::ShowMessage("sunDist          = %12.10f\n", sunDist);
      MessageInterface::ShowMessage("num              = %12.10f\n", num);
      MessageInterface::ShowMessage("denom            = %12.10f\n", denom);
//      MessageInterface::ShowMessage("percentSun       = %12.10f\n", percentSun);
      MessageInterface::ShowMessage("solarScaleFactor = %12.10f\n", solarScaleFactor);
      MessageInterface::ShowMessage("shadowBodies     =\n");
      for (unsigned int ii = 0; ii < shadowBodies.size(); ii++)
         MessageInterface::ShowMessage("     %d   %s\n", (Integer) ii,
               shadowBodies.at(ii)->GetName().c_str());
   #endif

   // Compute the percent sun
   Integer   numOcculted   = 0;
   Real percentSun         = 1;
   Real percentSunAll = 1;
   if ((shadowModel != "None") && !shadowBodies.empty())
   {
      bool lit           = true;
      bool dark          = false;
      Rvector3  sunPos   = sun->GetMJ2000Position(atEpoch); // relative to Earth
      Rvector3  cbPos    = scOrigin->GetMJ2000Position(atEpoch); // relative to Earth
      Real      bodySunVector[3];
      Real      state[3];
      Real      sunSat[3];
      Real      force[3];
      Rvector3  stateRelToOrigin(stateRelToEarth[0] - cbPos[0],
                                 stateRelToEarth[1] - cbPos[1],
                                 stateRelToEarth[2] - cbPos[2]);

      for (unsigned int jj = 0; jj < shadowBodies.size(); jj++)
      {
         std::string bodyName     = shadowBodies.at(jj)->GetName();
         
         //do not check Sun, Sun cannot occult the Sun...
         if (bodyName == GmatSolarSystemDefaults::SUN_NAME)
            continue;

         bool        bodyIsOrigin = (bodyName == scOrigin->GetName());
         Real        bodyRadius   = shadowBodies.at(jj)->GetEquatorialRadius();

         Rvector3 bodyPos  = shadowBodies.at(jj)->GetMJ2000Position(atEpoch); // with respect to Earth
         #ifdef DEBUG_SOLAR_POWER_PERCENT
            MessageInterface::ShowMessage("shadow body is %s\n", bodyName.c_str());
            MessageInterface::ShowMessage("origin is      %s\n", (scOrigin->GetName()).c_str());
            MessageInterface::ShowMessage("   cbPos   = %12.10f  %12.10f  %12.10f\n",
                  cbPos[0], cbPos[1], cbPos[2]);
            MessageInterface::ShowMessage("   bodyPos = %12.10f  %12.10f  %12.10f\n",
                  bodyPos[0], bodyPos[1], bodyPos[2]);
         #endif

         // Find vector from the body to the Sun
         bodySunVector[0]    = sunPos[0] - bodyPos[0];
         bodySunVector[1]    = sunPos[1] - bodyPos[1];
         bodySunVector[2]    = sunPos[2] - bodyPos[2];

         // Convert state to be relative to the current body
         if (!bodyIsOrigin)
         {
            // state is from origin to spacecraft ; cb - body = origin wrt shadow body
            state[0] = stateRelToOrigin[0] + (cbPos[0] - bodyPos[0]);
            state[1] = stateRelToOrigin[1] + (cbPos[1] - bodyPos[1]);
            state[2] = stateRelToOrigin[2] + (cbPos[2] - bodyPos[2]);
         }
         else  // SC state with respect to the origin (which is the current body)
         {
            state[0] = stateRelToOrigin[0];
            state[1] = stateRelToOrigin[1];
            state[2] = stateRelToOrigin[2];
         }

         // Find vector from Sun to the SC
         sunSat[0] = state[0] - bodySunVector[0];
         sunSat[1] = state[1] - bodySunVector[1];
         sunSat[2] = state[2] - bodySunVector[2];

         #ifdef DEBUG_SOLAR_POWER
            MessageInterface::ShowMessage(
                  "In SolarPowerSystem,  body = %s\n",
                  shadowBodies.at(jj)->GetName().c_str());
            MessageInterface::ShowMessage("   bodySunVector = %12.10f  %12.10f  %12.10f\n",
                  bodySunVector[0], bodySunVector[1], bodySunVector[2]);
            MessageInterface::ShowMessage("   sunSat      = %12.10f  %12.10f  %12.10f\n",
                  sunSat[0], sunSat[1], sunSat[2]);
         #endif

         percentSun = shadowState->FindShadowState(state, bodySunVector, sunSat, sunRadius, bodyRadius);

         // Is there more than one occultation? -  we don't currently model that
         if (percentSun < 1.0)
            numOcculted++;
         // For now, we are using the minimum value for percentSun over all bodies
         if (percentSun < percentSunAll)
            percentSunAll = percentSun;
         #ifdef DEBUG_SOLAR_POWER_PERCENT
            MessageInterface::ShowMessage("   percentSun    = %12.10f\n", percentSun);
            MessageInterface::ShowMessage("   percentSunAll = %12.10f\n", percentSunAll);
         #endif
      }
   }
   if ((numOcculted) > 1 && !occultationWarningWritten)
   {
      MessageInterface::ShowMessage("***WARNING*** Multiple occultations detected.  GMAT does not currently "
            "model occultations from more than one body at once.\n");
      occultationWarningWritten = true;
   }

   // Englander Eq. 17
   generatedPower = percentSunAll * basePower * solarScaleFactor;
   #ifdef DEBUG_SOLAR_POWER
      MessageInterface::ShowMessage("   generatedPower    = %12.10f\n", generatedPower);
   #endif

   return generatedPower;
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
std::string SolarPowerSystem::GetParameterText(const Integer id) const
{
   if (id >= PowerSystemParamCount && id < SolarPowerSystemParamCount)
      return PARAMETER_TEXT[id - PowerSystemParamCount];
   return PowerSystem::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string SolarPowerSystem::GetParameterUnit(const Integer id) const
{
   switch (id)
   {
   case SOLAR_COEFF1:
      return "kW?";       // these are TBD
   case SOLAR_COEFF2:
      return "kW?";
   case SOLAR_COEFF3:
      return "kW?";
   case SOLAR_COEFF4:
      return "kW?";
   case SOLAR_COEFF5:
      return "kW?";
   default:
      return PowerSystem::GetParameterUnit(id);
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
Integer SolarPowerSystem::GetParameterID(const std::string &str) const
{
   for (Integer i = PowerSystemParamCount; i < SolarPowerSystemParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - PowerSystemParamCount])
         return i;
   }

   return PowerSystem::GetParameterID(str);
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
Gmat::ParameterType SolarPowerSystem::GetParameterType(const Integer id) const
{
   if (id >= PowerSystemParamCount && id < SolarPowerSystemParamCount)
      return PARAMETER_TYPE[id - PowerSystemParamCount];

   return PowerSystem::GetParameterType(id);
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
std::string SolarPowerSystem::GetParameterTypeString(const Integer id) const
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
bool SolarPowerSystem::IsParameterReadOnly(const Integer id) const
{
   return PowerSystem::IsParameterReadOnly(id);
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
Real SolarPowerSystem::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case SOLAR_COEFF1:
         return solarCoeff1;

      case SOLAR_COEFF2:
         return solarCoeff2;

      case SOLAR_COEFF3:
         return solarCoeff3;

      case SOLAR_COEFF4:
         return solarCoeff4;

      case SOLAR_COEFF5:
         return solarCoeff5;

      default:
         break;   // Default just drops through
   }
   return PowerSystem::GetRealParameter(id);
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
Real SolarPowerSystem::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_POWER_SYSTEM_SET
   MessageInterface::ShowMessage
      ("PowerSystem::SetRealParameter(), id=%d, value=%f\n", id, value);
   #endif

   switch (id)
   {
      case SOLAR_COEFF1:
         solarCoeff1 = value;
         return solarCoeff1;

      case SOLAR_COEFF2:
         solarCoeff2 = value;
         return solarCoeff2;

      case SOLAR_COEFF3:
         solarCoeff3 = value;
         return solarCoeff3;

      case SOLAR_COEFF4:
         solarCoeff4 = value;
         return solarCoeff4;

      case SOLAR_COEFF5:
         solarCoeff5 = value;
         return solarCoeff5;

      default:
         break;   // Default just drops through
   }

   return PowerSystem::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
Real SolarPowerSystem::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//---------------------------------------------------------------------------
Real SolarPowerSystem::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string SolarPowerSystem::GetStringParameter(const Integer id) const
{
   #ifdef DEBUG_SOLAR_POWER_SYSTEM_SET
   MessageInterface::ShowMessage
      ("SolarPowerSystem::GetStringParameter() entered, id=%d, returning '%s'\n", id,
            shadowModel.c_str());
   #endif
   if (id == SHADOW_MODEL)
      return shadowModel;

   return PowerSystem::GetStringParameter(id);
}


//---------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool SolarPowerSystem::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_SOLAR_POWER_SYSTEM_SET
   MessageInterface::ShowMessage
      ("SolarPowerSystem::SetStringParameter() entered, id=%d, value='%s'\n", id,
       value.c_str());
   #endif

   if (id == SHADOW_MODEL)
   {
      if ((value != "None") && (value != "DualCone"))
      {
         HardwareException hwe("");
         hwe.SetDetails(errorMessageFormat.c_str(),
                        value.c_str(),
                        "ShadowModel", "None or Dualcone");
         throw hwe;
      }
      #ifdef DEBUG_SOLAR_POWER_SYSTEM_SET
      MessageInterface::ShowMessage
         ("SolarPowerSystem::SetStringParameter() setting shadowModel to '%s'\n",
          value.c_str());
      #endif
      shadowModel = value;
      return true;
   }

   return PowerSystem::SetStringParameter(id, value);
}


//---------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//---------------------------------------------------------------------------
std::string SolarPowerSystem::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//---------------------------------------------------------------------------
bool SolarPowerSystem::SetStringParameter(const std::string &label,
                                  const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id,const Integer index) const
//---------------------------------------------------------------------------
std::string SolarPowerSystem::GetStringParameter(const Integer id,
                                                 const Integer index) const
{
   if (id == SHADOW_BODIES)
   {
      try
      {
         return shadowBodyNames.at(index);
      }
      catch (const std::exception &)
      {
         throw HardwareException("SolarPowerSystem error: index out-of-range.");
      }
   }

   return PowerSystem::GetStringParameter(id, index);
}

//---------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//---------------------------------------------------------------------------
bool SolarPowerSystem::SetStringParameter(const Integer id,
                                          const std::string &value,
                                          const Integer index)
{
   #ifdef DEBUG_SET
      MessageInterface::ShowMessage(
            "Entering SetStringParameter with id = %d, value = %s, and index = %d\n",
            id, value.c_str(), index);
   #endif
   if (id == SHADOW_BODIES)
   {
      // Check to see if we are setting a blank list here; if we are,
      // then we do NOT want to use the default list of bodies
      Integer sz = value.length();
      if ((sz <= 0) || (GmatStringUtil::IsBlank(value)) ||
          ((value[0] == '{') && (value[sz-1] == '}')))
      {
         settingNoBodies = true;
         #ifdef DEBUG_SET
            MessageInterface::ShowMessage(
                  "In SetStringParameter, settingNoBodies = true!!\n");
         #endif
         return true;
      }
      if ((index < 0) || (index > (Integer) shadowBodyNames.size()))
      {
         std::string errmsg = "For PowerSystem ";
         errmsg            += instanceName + ", index into ShadowBodies is ";
         errmsg            += "out-of-range\n";
         throw HardwareException(errmsg);
      }
      if (value == GmatSolarSystemDefaults::SUN_NAME)
      {
         std::string errmsg = "The Sun cannot be set as a Shadow body ";
         errmsg            += "on Power System " + instanceName;
         errmsg            += "\n";
         throw HardwareException(errmsg);
      }
      // Add to the end of the list ...
      if (index == (Integer) shadowBodyNames.size())
      {
         if (find(shadowBodyNames.begin(), shadowBodyNames.end(), value) == shadowBodyNames.end())
         {
               shadowBodyNames.push_back(GmatStringUtil::Trim(value));
               settingNoBodies = false;
               #ifdef DEBUG_SET
                  MessageInterface::ShowMessage(
                        "In SetStringParameter, settingNoBodies = false\n");
               #endif
         }
      }
      // ... or, replace current name
      else
      {
         shadowBodyNames.at(index) = GmatStringUtil::Trim(value);
         settingNoBodies = false;
         #ifdef DEBUG_SET
            MessageInterface::ShowMessage(
                  "In SetStringParameter, settingNoBodies = false (2)\n");
         #endif
      }
      return true;
   }

   return PowerSystem::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the StringArray parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& SolarPowerSystem::GetStringArrayParameter(const Integer id) const
{
   if (id == SHADOW_BODIES)
   {
      if ((shadowBodyNames.empty()) && (!settingNoBodies))
      {
         return defaultShadowBodyNames;
      }
      else
         return shadowBodyNames;
   }

   return PowerSystem::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter((const std::string &label)
//                       const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& SolarPowerSystem::GetStringArrayParameter(
                                     const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <type>    reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 *
 * @return true if object name changed, false if not.
 */
//------------------------------------------------------------------------------
bool SolarPowerSystem::RenameRefObject(const UnsignedInt type,
                                       const std::string &oldName,
                                       const std::string &newName)
{
   #ifdef DEBUG_SOLAR_RENAME
      MessageInterface::ShowMessage(
            "Entering SolarPowerSystem::Rename with type = %d, oldName = %s, newName = %s\n",
            (Integer) type, oldName.c_str(), newName.c_str());
   #endif
   if (type == Gmat::CELESTIAL_BODY)
   {
      for (unsigned int i=0; i< shadowBodyNames.size(); i++)
      {
         if (shadowBodyNames[i] == oldName)
            shadowBodyNames[i] = newName;
      }
   }
   return true;  // why doesn't this call the parent class?
}

//------------------------------------------------------------------------------
// bool WriteEmptyStringArray
//------------------------------------------------------------------------------
/**
 * Returns a flag specifying whether or not to write out a StringArray to
 * the script even if it is empty
 *
 * @param id    ID of the parameter
 */
//------------------------------------------------------------------------------
bool SolarPowerSystem::WriteEmptyStringArray(Integer id)
{
   if (id == SHADOW_BODIES)
      return true;
   return PowerSystem::WriteEmptyStringArray(id);
}