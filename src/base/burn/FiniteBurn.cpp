//$Id$
//------------------------------------------------------------------------------
//                              FiniteBurn
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/12/20
//
/**
 * Implements the FiniteBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#include "FiniteBurn.hpp"
#include "BurnException.hpp"
#include "StringUtil.hpp"          // for ToString()
#include "MessageInterface.hpp"
#include "ElectricThruster.hpp"  // for Min/MaxUsablePower

//#define DEBUG_RENAME
//#define DEBUG_BURN_ORIGIN
//#define DEBUG_FINITE_BURN
//#define DEBUG_FINITEBURN_FIRE
//#define DEBUG_FINITEBURN_SET
//#define DEBUG_FINITEBURN_INIT
//#define DEBUG_FINITEBURN_OBJECT
//#define DEBUG_FINITE_BURN_POWER
//#define DEBUG_MASS_FLOW
//#define DEBUG_IS_FIRING
//#define DEBUG_FINITEBURN_FIRE_ACCEL


#ifdef DEBUG_FINITEBURN_FIRE
   #define DEBUG_FINITEBURN_FIRE_ACCEL
#endif

//---------------------------------
// static data
//---------------------------------

/// Labels used for the finite burn parameters.
const std::string
FiniteBurn::PARAMETER_TEXT[FiniteBurnParamCount - BurnParamCount] =
{
   "Thrusters",
   "Tanks",
   "BurnScaleFactor",
   "ThrottleLogicAlgorithm",
};

/// Types of the parameters used by finite burns.
const Gmat::ParameterType
FiniteBurn::PARAMETER_TYPE[FiniteBurnParamCount - BurnParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,
   Gmat::OBJECTARRAY_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
//  FiniteBurn(const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * FiniteBurn constructor (default constructor).
 * 
 * @param nomme Name of the constructed object.
 */
//------------------------------------------------------------------------------
FiniteBurn::FiniteBurn(const std::string &nomme) :
   Burn (Gmat::FINITE_BURN, "FiniteBurn", nomme),
   throttleLogicAlgorithm   ("MaxNumberOfThrusters"),
   isElectricBurn           (false),       // default is Chemical
   burnEpoch                (-1),
   burnState                (nullptr),
   burnMass                 (-1.0),
   burnOrigin               (nullptr)
{
   objectTypes.push_back(Gmat::FINITE_BURN);
   objectTypeNames.push_back("FiniteBurn");
   parameterCount = FiniteBurnParamCount;
   
   thrusterNames.clear();
}


//------------------------------------------------------------------------------
//  ~FiniteBurn()
//------------------------------------------------------------------------------
/**
 * FiniteBurn destructor.
 */
//------------------------------------------------------------------------------
FiniteBurn::~FiniteBurn()
{
}


//------------------------------------------------------------------------------
//  FiniteBurn(const FiniteBurn& fb)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param fb The FiniteBurn that is copied.
 */
//------------------------------------------------------------------------------
FiniteBurn::FiniteBurn(const FiniteBurn& fb) :
   Burn                   (fb),
   thrusterNames          (fb.thrusterNames),
   throttleLogicAlgorithm (fb.throttleLogicAlgorithm),
   isElectricBurn         (fb.isElectricBurn),
   burnEpoch              (-1),           // Always reset these
   burnState              (nullptr),
   burnMass               (-1.0),
   burnOrigin             (nullptr)
{
   parameterCount = fb.parameterCount;
}


//------------------------------------------------------------------------------
//  FiniteBurn& operator=(const FiniteBurn& fb)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param fb The FiniteBurn that is copied.
 * 
 * @return this instance, with parameters set to match fb.
 */
//------------------------------------------------------------------------------
FiniteBurn& FiniteBurn::operator=(const FiniteBurn& fb)
{
   if (&fb == this)
      return *this;
      
   Burn::operator=(fb);
   
   thrusterNames          = fb.thrusterNames;
   throttleLogicAlgorithm = fb.throttleLogicAlgorithm;
   isElectricBurn         = fb.isElectricBurn;
   burnEpoch              = -1;
   burnState              = nullptr;
   burnMass               = -1.0;
   burnOrigin             = nullptr;
   
   return *this;
}


//------------------------------------------------------------------------------
//  void SetSpacecraftToManeuver(Spacecraft *sat)
//------------------------------------------------------------------------------
/**
 * Accessor method used by Maneuver to pass in the spacecraft pointer
 * 
 * @param sat the Spacecraft
 */
//------------------------------------------------------------------------------
void FiniteBurn::SetSpacecraftToManeuver(Spacecraft *sat)
{
   #ifdef DEBUG_FINITEBURN_SET
   MessageInterface::ShowMessage
      ("FiniteBurn::SetSpacecraftToManeuver() sat=<%p>'%s'\n", sat,
       sat->GetName().c_str());
   Real *state = (sat->GetState()).GetState();
   MessageInterface::ShowMessage
      ("   state = [%.13f %.13f %.13f %.13f %.13f %.13f]\n", state[0],
       state[1], state[2], state[3], state[4], state[5]);
   #endif
   
   if (sat == NULL)
      return;
   
   // FiniteBurn does not require CoordinateSystem conversion
   // so we don't need Burn::SetSpacecraftToManeuver(sat);
   // The thruster will handle CoordinateSystem conversion
   
   // If spacecraft changed, re-associate tank of the spacecraft
   if (spacecraft != sat)
   {
      spacecraft = sat;
      SetThrustersFromSpacecraft();
   }

   // Load starting data into the data buffer
   Real epoch = sat->GetEpoch();
   Real burnData[4];
   Fire(burnData, epoch);
   
   #ifdef DEBUG_FINITEBURN_SET
   MessageInterface::ShowMessage
      ("FiniteBurn::SetSpacecraftToManeuver() returning\n");
   #endif
}

/**
 * Method to set additional data needed for position and mass based computations
 *
 * @param epoch The A.1 epoch of the state data
 * @param state Pointer to the J2000 Cartesian state for the maneuvering spacecraft
 * @param mass The current total mass for the maneuvering spacecraft
 * @param origin Origin for the force model that needs the burn data
 */
void FiniteBurn::SetManeuverEpochAndState(Real epoch, Real* state,
      Real mass, CelestialBody *origin)
{
   burnEpoch  = epoch;
   burnState  = state;
   burnMass   = mass;
   burnOrigin = origin;
}


//------------------------------------------------------------------------------
//  bool Fire(Real *burnData, Real epoch, bool backwards)
//------------------------------------------------------------------------------
/**
 * Fire does not currently perform any action for FiniteBurn objects.  The 
 * BeginManeuver/EndManeuver commands replace the actions that fire would 
 * perform.
 * 
 * @param burnData   Pointer to an array that will be filled with the
 *                   acceleration and mass flow data.  The data returned in 
 *                   burnData has the format
 *                     burnData[0]  dVx/dt
 *                     burnData[1]  dVy/dt
 *                     burnData[2]  dVz/dt
 *                     burnData[3]  dM/dt
 * @param epoch      Epoch of the burn fire
 * @param backwards  Flag used by impulsive burns to indicate application as if
 *                   in a backprop
 *
 * @return true on success; throws on failure.
 */
//------------------------------------------------------------------------------
bool FiniteBurn::Fire(Real *burnData, Real epoch, bool /*backwards*/)
{
   #ifdef DEBUG_FINITEBURN_FIRE
      MessageInterface::ShowMessage
         ("FiniteBurn::Fire() this<%p>'%s' entered, epoch=%.12f, spacecraft=<%p>'%s'\n",
          this, instanceName.c_str(), epoch, spacecraft,
          spacecraft ? spacecraft->GetName().c_str() : "NULL");
   #endif
   
   if (isInitialized == false)
      Initialize();
   
   if (!spacecraft)
      throw BurnException("Maneuver initial state undefined (No spacecraft?)");
   
   // Accumulate the individual accelerations from the thrusters
   Real dm = 0.0, tMass, tOverM, *dir, norm;
   deltaV[0] = deltaV[1] = deltaV[2] = 0.0;
   totalAccel[0] = totalAccel[1] = totalAccel[2]  = 0.0;
   totalThrust[0] = totalThrust[1] = totalThrust[2]  = 0.0;
   Thruster *current;
   
   if (burnMass < 0.0)
      tMass = spacecraft->GetRealParameter("TotalMass");
   else
      tMass = burnMass;
   
   #ifdef DEBUG_BURN_ORIGIN
   Real *satState = spacecraft->GetState().GetState();
   MessageInterface::ShowMessage
      ("FiniteBurn Vectors:\n   "
       "Sat   = [%.15f %.15f %.15f %.15f %.15f %.15f]\n   "
       "Frame = [%.15f %.15f %.15f\n   " 
       "         %.15f %.15f %.15f\n   "
       "         %.15f %.15f %.15f]\n\n",
       satState[0], satState[1], satState[2], satState[3], satState[4], satState[5], 
       frameBasis[0][0], frameBasis[0][1], frameBasis[0][2],
       frameBasis[1][0], frameBasis[1][1], frameBasis[1][2],
       frameBasis[2][0], frameBasis[2][1], frameBasis[2][2]);
   #endif
   
   // If this FiniteBurn uses electric thrusters, we must compute the throttle
   // logic based on the total power available to the thrusters
   if (isElectricBurn)
   {
      Real availablePower = spacecraft->GetThrustPower();
      ComputeThrottleLogic(availablePower);
   }

   for (StringArray::iterator i = thrusterNames.begin(); 
        i != thrusterNames.end(); ++i)
   {
      #ifdef DEBUG_FINITE_BURN
         MessageInterface::ShowMessage
            ("   Accessing thruster '%s' from spacecraft <%p>'%s'\n", (*i).c_str(),
             spacecraft, spacecraft->GetName().c_str());
      #endif
      
      current = (Thruster *)spacecraft->GetRefObject(Gmat::THRUSTER, *i);
      if (!current)
         throw BurnException("FiniteBurn::Fire requires thruster named \"" +
            (*i) + "\" on spacecraft " + spacecraft->GetName());
      
      // Save current thruster so that GetRefObject() can return it (LOJ: 2009.08.28)
      thrusterMap[current->GetName()] = current;
      
      // FiniteBurn class is friend of Thruster class, so we can access
      // member data directly
      current->ComputeInertialDirection(epoch);
      dir = current->inertialDirection;
      norm = sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
      
      #ifdef DEBUG_FINITE_BURN
         MessageInterface::ShowMessage
         ("   Thruster Direction: %.15f  %.15f  %.15f\n"
          "                 norm: %.15f\n", dir[0], dir[1], dir[2], norm);
      #endif
         
      if (norm == 0.0)
         throw BurnException("FiniteBurn::Fire thruster " + (*i) +
                             " on spacecraft " + spacecraft->GetName() +
                             " has no direction.");
      
      if (burnState)
         current->SetDerivativeState(burnState, burnOrigin);

      dm += current->CalculateMassFlow();
      //tOverM = current->thrust / (tMass * norm * 1000.0); //old code
      tOverM = current->thrust * current->thrustScaleFactor *
               current->dutyCycle / (tMass * norm * 1000.0);
      
      // deltaV is really totalAcceleration (SPH)
      deltaV[0] += dir[0] * tOverM;
      deltaV[1] += dir[1] * tOverM;
      deltaV[2] += dir[2] * tOverM;
      
      // Compute thrust magnitude
      // Now appliedThrustMag is computed in the Thruster (LOJ: 2016.06.27)
      // Real appliedThrustMag = current->thrust * current->thrustScaleFactor *
      //    current->dutyCycle;
      
      // Add in thrust from this thruster for totalThrust
      totalThrust[0] += dir[0] / norm * current->appliedThrustMag;
      totalThrust[1] += dir[1] / norm * current->appliedThrustMag;
      totalThrust[2] += dir[2] / norm * current->appliedThrustMag;
      
      #ifdef DEBUG_FINITEBURN_FIRE
         MessageInterface::ShowMessage("   Thruster %s = %s details:\n", 
            (*i).c_str(), current->GetName().c_str());
         MessageInterface::ShowMessage(
            "          thrust = %.15f\n"
            "appliedThrustMag = %.15f\n"
            "              dM = %.15e\n            Mass = %.15f\n"
            "             TSF = %.15f\n           |Acc| = %.15e\n"
            "             Acc = [%.15e   %.15e   %.15e]\n", current->thrust,
            current->appliedThrustMag, dm, tMass, current->thrustScaleFactor,
            tOverM, deltaV[0], deltaV[1], deltaV[2]);
      #endif
   }
   
   // deltaV is in inertial coordinate system, so copy it to deltaVInertial
   deltaVInertial[0] = deltaV[0];
   deltaVInertial[1] = deltaV[1];
   deltaVInertial[2] = deltaV[2];
   
   // deltaV is really total acceleration so copy it to totalAccel
   totalAccel[0] = deltaV[0];
   totalAccel[1] = deltaV[1];
   totalAccel[2] = deltaV[2];
   
   #ifdef DEBUG_FINITEBURN_FIRE
   MessageInterface::ShowMessage
      ("     totalThrust = [%.15e   %.15e   %.15e]\n"
       "      totalAccel = [%.15e   %.15e   %.15e]\n",
       totalThrust[0], totalThrust[1], totalThrust[2],
       totalAccel[0], totalAccel[1], totalAccel[2]);
   #endif

   // Build the acceleration
   burnData[0] = deltaV[0]*frameBasis[0][0] +
                 deltaV[1]*frameBasis[0][1] +
                 deltaV[2]*frameBasis[0][2];
   burnData[1] = deltaV[0]*frameBasis[1][0] +
                 deltaV[1]*frameBasis[1][1] +
                 deltaV[2]*frameBasis[1][2];
   burnData[2] = deltaV[0]*frameBasis[2][0] +
                 deltaV[1]*frameBasis[2][1] +
                 deltaV[2]*frameBasis[2][2];
   burnData[3] = dm;

   // Save total mass flow rate
   totalMassFlowRate = dm;
      
   #ifdef DEBUG_FINITEBURN_FIRE_ACCEL
      MessageInterface::ShowMessage(
          "FiniteBurn::Fire() this<%p>'%s' at %.12lf returning\n"
          "   Acceleration:  %.15e  %.15e  %.15e  dm: %.15e\n", this,
          GetName().c_str(), epoch, burnData[0], burnData[1], burnData[2], dm);
   #endif

   hasFired = true;
   epochAtLastFire = epoch;
   return true;
}

//------------------------------------------------------------------------------
// bool IsFiring()
//------------------------------------------------------------------------------
/**
 * Checks if thruster used in the FiniteBurn is firing and return the status.
 */
//------------------------------------------------------------------------------
bool FiniteBurn::IsFiring()
{
   #ifdef DEBUG_IS_FIRING
   MessageInterface::ShowMessage
      ("FiniteBurn::IsFiring() <%p>'%s' entered, thrusterNames.size()=%d, "
       "spacecraft=<%p>'%s'\n", this, GetName().c_str(), thrusterNames.size(),
       spacecraft, spacecraft ? spacecraft->GetName().c_str() : "NULL");
   #endif
   
   if (thrusterNames.empty() || spacecraft == NULL)
   {
      #ifdef DEBUG_IS_FIRING
      MessageInterface::ShowMessage
         ("FiniteBurn::IsFiring() <%p>'%s' returning false, no thrusters or "
          "spacecaft specified\n", this, GetName().c_str(), isFiring);
      #endif
      return false;
   }
   
   Thruster *current = NULL;
   int firingCount = 0;
   for (StringArray::iterator i = thrusterNames.begin(); i != thrusterNames.end(); ++i)
   {
      #ifdef DEBUG_IS_FIRING
      MessageInterface::ShowMessage
         ("   Accessing thruster '%s' from spacecraft <%p>'%s'\n", (*i).c_str(),
          spacecraft, spacecraft->GetName().c_str());
      #endif
      
      current = (Thruster *)spacecraft->GetRefObject(Gmat::THRUSTER, *i);
      if (!current)
         throw BurnException("FiniteBurn::Fire requires thruster named \"" +
            (*i) + "\" on spacecraft " + spacecraft->GetName());
      
      // FiniteBurn class is friend of Thruster class, so we can access
      // member data directly
      if (current->thrusterFiring)
      {
         #ifdef DEBUG_IS_FIRING
         MessageInterface::ShowMessage("   Thruster '%s' is firing\n", (*i).c_str());
         #endif
         firingCount++;
      }
      else
      {
         #ifdef DEBUG_IS_FIRING
         MessageInterface::ShowMessage("   Thruster '%s' is not firing\n", (*i).c_str());
         #endif
      }
   }
   
   isFiring = false;
   if (firingCount > 0)
      isFiring = true;
   
   #ifdef DEBUG_IS_FIRING
   MessageInterface::ShowMessage
      ("FiniteBurn::IsFiring() <%p>'%s' returning %d\n", this, GetName().c_str(), isFiring);
   #endif
   return isFiring;
}

//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 * 
 * @param <id> Integer id for the parameter.
 * 
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
std::string FiniteBurn::GetParameterText(const Integer id) const
{
   if (id >= BurnParamCount && id < FiniteBurnParamCount)
      return PARAMETER_TEXT[id - BurnParamCount];
      
   return Burn::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 * 
 * @param <str> Name of the parameter.
 * 
 * @return The ID.
 */
//------------------------------------------------------------------------------
Integer FiniteBurn::GetParameterID(const std::string &str) const
{
   // Check for deprecated parameters
   if (str == "Tanks")
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** \"Tanks\" field of FiniteBurn "
          "is deprecated and will be removed from a future build.\n");
      return FUEL_TANK;
   }
   
   if (str == "BurnScaleFactor")
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** \"BurnScaleFactor\" field of FiniteBurn "
          "is deprecated and will be removed from a future build.\n");
      return BURN_SCALE_FACTOR;
   }
   
   if (str == "CoordinateSystem")
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** \"CoordinateSystem\" field of FiniteBurn "
          "is deprecated and will be removed from a future build.\n");
      return COORDINATESYSTEM;
   }
   
   if (str == "Origin")
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** \"Origin\" field of FiniteBurn "
          "is deprecated and will be removed from a future build.\n");
      return BURNORIGIN;
   }
   
   if (str == "Axes")
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** \"Axes\" field of FiniteBurn "
          "is deprecated and will be removed from a future build.\n");
      return BURNAXES;
   }
   
   for (Integer i = BurnParamCount; i < FiniteBurnParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - BurnParamCount])
         return i;
   }
   
   return Burn::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType FiniteBurn::GetParameterType(const Integer id) const
{
   if (id >= BurnParamCount && id < FiniteBurnParamCount)
      return PARAMETER_TYPE[id - BurnParamCount];
      
   return Burn::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string FiniteBurn::GetParameterTypeString(const Integer id) const
{
   return Burn::PARAM_TYPE_STRING[GetParameterType(id)];
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
bool FiniteBurn::IsParameterReadOnly(const Integer id) const
{
   if ((id == FUEL_TANK) || (id == BURN_SCALE_FACTOR) ||
       (id == COORDINATESYSTEM) || (id == BURNORIGIN) || (id == BURNAXES) || 
       (id == DELTAV1) || (id == DELTAV2) || (id == DELTAV3))
      return true;
   
   return Burn::IsParameterReadOnly(id);
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
bool FiniteBurn::IsParameterCommandModeSettable(const Integer id) const
{
   if ((id == THRUSTER) || (id == FUEL_TANK))
      return false;

   return Burn::IsParameterCommandModeSettable(id);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string FiniteBurn::GetStringParameter(const Integer id) const
{
   // CoordinateSystem, Origin, Axes are not valid FiniteBurn parameters,
   // so handle here.
   switch (id)
   {
   case COORDINATESYSTEM:
   case BURNORIGIN:
   case BURNAXES:
      return "Deprecated"; // just to ignore
   case THROTTLE_LOGIC_ALGORITHM:
      return throttleLogicAlgorithm;
   default:
      break;
   }
   
   return Burn::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool FiniteBurn::SetStringParameter(const Integer id, const std::string &value)
{
   // CoordinateSystem, Origin, Axes are not valid FiniteBurn parameters,
   // so handle here.
   switch (id)
   {
   case FUEL_TANK:
   case COORDINATESYSTEM:
   case BURNORIGIN:
   case BURNAXES:
      return true; // just to ignore
   case THROTTLE_LOGIC_ALGORITHM:
      if (value != "MaxNumberOfThrusters")
      {
         std::string msg =
            "The value of \"" + value + "\" for field \"ThrottleLogicAlgorithm\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [\"MaxNumberOfThrusters\"]. ";   // will need to add list if other options become available

         throw BurnException(msg);
      }
      throttleLogicAlgorithm = value;   // @todo add validation
      return true;
   default:
      break;
   }
   
   if (id == THRUSTER)
   {
      if (find(thrusterNames.begin(), thrusterNames.end(), value) == thrusterNames.end())
         thrusterNames.push_back(value);
      isInitialized = false;
      return true;
   }
   
   return Burn::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  Real SetStringParameter(const Integer id, const char *value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a specific std::string element in an array.
 *
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * @param <index> Index for the element
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool FiniteBurn::SetStringParameter(const Integer id, const char *value,
                                    const Integer index)
{
   return SetStringParameter(id, std::string(value), index);
}


//------------------------------------------------------------------------------
//  Real SetStringParameter(const Integer id, const std::string &value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a specific std::string element in an array.
 *
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * @param <index> Index for the element
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool FiniteBurn::SetStringParameter(const Integer id, const std::string &value,
                                    const Integer index)
{
   if (id == FUEL_TANK)
      return true;     // just to ignore
   
   Integer count;
   
   if (id == THRUSTER)
   {
      count = thrusterNames.size();
      if (index > count)
         throw BurnException("Attempting to write thruster " + value +
                  " past the allowed range for FiniteBurn " + instanceName);
      if (find(thrusterNames.begin(), thrusterNames.end(), value) != thrusterNames.end())
      {
         if (thrusterNames[index] == value)
            return true;
         throw BurnException("Thruster " + value +
                  " already set for FiniteBurn " + instanceName);
      }
      if (index == count)
         thrusterNames.push_back(value);
      else
         thrusterNames[index] = value;

      isInitialized = false;
      return true;
   }
   
   return Burn::SetStringParameter(id, value, index);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 * 
 * For the Burn classes, calls to this method get passed to the maneuver frame
 * manager when the user requests the frames that are available for the system.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& FiniteBurn::GetStringArrayParameter(const Integer id) const
{
   if (id == FUEL_TANK)
      return tankNames;  // deprecated
   
   if (id == THRUSTER)
      return thrusterNames;
   
   return Burn::GetStringArrayParameter(id);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access the Real data associated with this burn.
 * 
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested Real.
 */
//---------------------------------------------------------------------------
Real FiniteBurn::GetRealParameter(const Integer id) const
{
   if (id == BURN_SCALE_FACTOR)  // deprecated
      return REAL_PARAMETER_UNDEFINED;
   
   return Burn::GetRealParameter(id);
}


//---------------------------------------------------------------------------
//  Real setRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Change the Real data associated with this burn.
 * 
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 *
 * @exception <BurnException> thrown if value is out of range
 * 
 * @return The value of the parameter at the end of the call.
 */
//---------------------------------------------------------------------------
Real FiniteBurn::SetRealParameter(const Integer id, const Real value)
{
   if (id == BURN_SCALE_FACTOR)  // deprecated
      return value;
   
   return Burn::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool FiniteBurn::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& FiniteBurn::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   
   // Get ref. object types from the parent class
   refObjectTypes = Burn::GetRefObjectTypeArray();
   
   // Add ref. object types from this class if not already added
   if (find(refObjectTypes.begin(), refObjectTypes.end(), Gmat::THRUSTER) ==
       refObjectTypes.end())
      refObjectTypes.push_back(Gmat::THRUSTER);
   
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& FiniteBurn::GetRefObjectNameArray(const UnsignedInt type)
{
   #ifdef DEBUG_FINITEBURN_OBJECT
   MessageInterface::ShowMessage
      ("FiniteBurn::GetRefObjectNameArray() this=<%p>'%s' entered, type=%d\n",
       this, GetName().c_str(), type);
   #endif
   
   refObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::HARDWARE)
   {
      // Get ref. objects for requesting type from the parent class
      Burn::GetRefObjectNameArray(type);
      
      // Add ref. objects for requesting type from this class
      refObjectNames.insert(refObjectNames.end(), thrusterNames.begin(),
                            thrusterNames.end());
      
      #ifdef DEBUG_FINITEBURN_OBJECT
      MessageInterface::ShowMessage
         ("FiniteBurn::GetRefObjectNameArray() this=<%p>'%s' returning %d "
          "ref. object names\n", this, GetName().c_str(), refObjectNames.size());
      for (UnsignedInt i=0; i<refObjectNames.size(); i++)
         MessageInterface::ShowMessage("   '%s'\n", refObjectNames[i].c_str());
      #endif
      
      return refObjectNames;
   }
   
   #ifdef DEBUG_FINITEBURN_OBJECT
   MessageInterface::ShowMessage
      ("FiniteBurn::GetRefObjectNameArray() this=<%p>'%s' returning "
       "Burn::GetRefObjectNameArray()\n", this, GetName().c_str());
   #endif
   
   return Burn::GetRefObjectNameArray(type);
}


//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
GmatBase* FiniteBurn::GetRefObject(const UnsignedInt type,
                                   const std::string &name)
{
   if (type == Gmat::THRUSTER)
   {
      if (thrusterMap.find(name) != thrusterMap.end())
         return thrusterMap[name];
      else
         return NULL;
   }
   
   return Burn::GetRefObject(type, name);
}


//---------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool FiniteBurn::SetRefObject(GmatBase *obj, const UnsignedInt type,
                              const std::string &name)
{
   if (type == Gmat::THRUSTER)
   {
      if (thrusterMap.find(name) != thrusterMap.end())
         thrusterMap[name] = obj;
      
      return true;
   }
   
   return Burn::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the FiniteBurn.
 *
 * @return clone of the FiniteBurn.
 */
//------------------------------------------------------------------------------
GmatBase* FiniteBurn::Clone() const
{
   return (new FiniteBurn(*this));
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
void FiniteBurn::Copy(const GmatBase* orig)
{
   operator=(*((FiniteBurn *)(orig)));
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames reference object name used in this class.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 * 
 * @return true if object name changed, false if not.
 */
//---------------------------------------------------------------------------
bool FiniteBurn::RenameRefObject(const UnsignedInt type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("FiniteBurn::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type == Gmat::HARDWARE)
   {
      for (UnsignedInt i=0; i<thrusterNames.size(); i++)
         if (thrusterNames[i] == oldName)
            thrusterNames[i] = newName;
   }
   
   return Burn::RenameRefObject(type, oldName, newName);
}


bool FiniteBurn::DepletesMass()
{
   bool retval = false;

   ObjectArray thrusterArray = spacecraft->GetRefObjectArray(Gmat::THRUSTER);

   // Check each the thruster
   for (ObjectArray::iterator th = thrusterArray.begin();
        th != thrusterArray.end(); ++th)
   {
      if ((*th)->GetBooleanParameter("DecrementMass"))
      {
         retval = true;
         #ifdef DEBUG_MASS_FLOW
            MessageInterface::ShowMessage("Thruster %s decrements mass\n",
                  (*th)->GetName().c_str());
         #endif
         // Save the mass depleting thruster pointer(s) for later access?
      }
   }

   return retval;
}

//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
UnsignedInt FiniteBurn::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case THRUSTER:
      return Gmat::THRUSTER;
   case FUEL_TANK:
      return Gmat::FUEL_TANK;
   default:
      return Burn::GetPropertyObjectType(id);
   }
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string& action, const std::string& actionData)
//------------------------------------------------------------------------------
/**
 * Triggers internal actions on the finite burn object.
 *
 * The GUI uses this method to clear the thruster list.
 *
 * @param action The string describing the requested action
 * @param actionData Ancillary data that may be needed to execute the action.
 *
 * @return true if an action was triggered
 */
//------------------------------------------------------------------------------
bool FiniteBurn::TakeAction(const std::string& action,
      const std::string& actionData)
{
   bool retval = false;
   if (action == "ClearThrusterList")
   {
      thrusterNames.clear();
      retval = true;
   }
   else if (action == "SetData")
   {
      if (spacecraft)
      {
         // Load starting data into the data buffer
         Real epoch = spacecraft->GetEpoch();
         Real burnData[4];
         Fire(burnData, epoch);
      }
   }
   else
      retval = Burn::TakeAction(action, actionData);

   return retval;
}

//------------------------------------------------------------------------------
// bool FiniteBurn::Initialize()
//------------------------------------------------------------------------------
/**
 * This method sets up the data structures and pointers for a finite burn.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool FiniteBurn::Initialize()
{
   #ifdef DEBUG_FINITEBURN_INIT
   MessageInterface::ShowMessage
      ("FiniteBurn::Initialize() <%p>'%s' entered, spacecraft=<%p>\n", this,
       GetName().c_str(), spacecraft);
   #endif
   
   if (Burn::Initialize())
   {
      if (spacecraft == NULL)
         return false;
      
      SetThrustersFromSpacecraft();      
      isInitialized = true;
   }
   
   #ifdef DEBUG_FINITEBURN_INIT
   MessageInterface::ShowMessage
      ("FiniteBurn::Initialize() <%p>'%s' returning %d\n", this, GetName().c_str(),
       initialized);
   #endif
   
   return isInitialized;
}


//------------------------------------------------------------------------------
// bool SetThrustersFromSpacecraft()
//------------------------------------------------------------------------------
bool FiniteBurn::SetThrustersFromSpacecraft()
{
   #ifdef DEBUG_FINITEBURN_SET
   MessageInterface::ShowMessage
      ("FiniteBurn::SetThrustersFromSpacecraft() entered, spacecraft=<%p>'%s'\n",
       spacecraft, spacecraft ? spacecraft->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage("   thrusterNames.size()=%d\n", thrusterNames.size());
   #endif
   
   // Get thrusters and tanks associated to spacecraft
   ObjectArray thrusterArray = spacecraft->GetRefObjectArray(Gmat::THRUSTER);
   ObjectArray tankArray     = spacecraft->GetRefObjectArray(Gmat::FUEL_TANK);
   
   isElectricBurn          = false;  // initially assume Chemical burn
   bool thrusterTypeSet    = false;
   bool isElectricThruster = false;

   // Look up the thruster(s)
   for (ObjectArray::iterator th = thrusterArray.begin(); 
        th != thrusterArray.end(); ++th)
   {
      for (StringArray::iterator thName = thrusterNames.begin();
           thName != thrusterNames.end(); ++thName)
      {
         // Only act on thrusters assigned to this burn
         if ((*th)->GetName() == *thName)
         {
            bool isElectricThruster = (*th)->IsOfType("ElectricThruster");
            if (!thrusterTypeSet)
            {
               if (isElectricThruster)
                  isElectricBurn = true;
               thrusterTypeSet = true;
            }
            else
            {
               if ((isElectricBurn  && !isElectricThruster) ||
                   (!isElectricBurn &&  isElectricThruster))
               {
                  std::string errmsg = "Finite Burn ";
                  errmsg += instanceName + " has a mix of Chemical and ";
                  errmsg += "Electric thrusters.  Thrusters specified for a ";
                  errmsg += "finite burn must all be of the same type.\n";
                  throw BurnException(errmsg);
               }
            }
            Integer paramId = (*th)->GetParameterID("Tank");
            StringArray tankNames = (*th)->GetStringArrayParameter(paramId);
            // Setup the tankNames
            (*th)->TakeAction("ClearTankNames");
            // Loop through each tank for the burn
            for (StringArray::iterator tankName = tankNames.begin();
                 tankName != tankNames.end(); ++tankName)
            {
               ObjectArray::iterator tnk = tankArray.begin();
               // Find the tank on the spacecraft
               while (tnk != tankArray.end())
               {
                  if ((*tnk)->GetName() == *tankName)
                  {
                     // Make the assignment
                     (*th)->SetStringParameter("Tank", *tankName);
                     (*th)->SetRefObject(*tnk, (*tnk)->GetType(), 
                                         (*tnk)->GetName());
                     break;
                  }
                  // Not found; keep looking 
                  ++tnk;
                  if (tnk == tankArray.end())
                     throw BurnException
                        ("FiniteBurn::Initialize() cannot find tank " +
                         (*tankName) + " for burn " + instanceName);
               }
            }
         }
      }
   }
   
   #ifdef DEBUG_FINITEBURN_SET
   MessageInterface::ShowMessage
      ("FiniteBurn::SetThrustersFromSpacecraft() returning true\n");
   #endif
   
   return true;
}


bool FiniteBurn::ComputeThrottleLogic(Real powerAvailable)
{
   ElectricThruster *current         = NULL;
   Integer          numThrusters     = (Integer) thrusterNames.size();
   Real             powerPerThruster = 0.0;
   #ifdef DEBUG_FINITE_BURN_POWER
      MessageInterface::ShowMessage
         ("  Computing throttle logic, Power Available = %12.10f, numThrusters = %d\n",
               powerAvailable, numThrusters);
   #endif

   // Save the pointers since we have to access them again at the end
   std::vector<ElectricThruster*>  electricThrusters;
   // Save the minimum usable power for each
   RealArray                       minUsablePowerPerThruster;
   Real                            minPower = 0.0;

   // First check for errors in accessing the thrusters, and store the
   // MinimumUsablePower for each
//   for (StringArray::iterator i = thrusterNames.begin();
//        i != thrusterNames.end(); ++i)
   if (throttleLogicAlgorithm == "MaxNumberOfThrusters")
   {
      for (Integer ii = 0; ii < numThrusters; ii++)
      {
         #ifdef DEBUG_FINITE_BURN_POWER
            MessageInterface::ShowMessage
               ("  Computing throttle logic, accessing thruster '%s' from spacecraft <%p>'%s'\n",
                     thrusterNames.at(ii).c_str(),
                spacecraft, spacecraft->GetName().c_str());
         #endif

         // Check to make sure there is not an error accessing this thruster
         current = (ElectricThruster *)spacecraft->GetRefObject(Gmat::THRUSTER, thrusterNames.at(ii));
         if (!current)
            throw BurnException("FiniteBurn::Fire requires thruster named \"" +
                  thrusterNames.at(ii) + "\" on spacecraft " + spacecraft->GetName());
         electricThrusters.push_back(current);
         minPower = current->GetRealParameter(current->GetParameterID("MinimumUsablePower"));
         minUsablePowerPerThruster.push_back(minPower);
         #ifdef DEBUG_FINITE_BURN_POWER
            MessageInterface::ShowMessage
               ("  Computing throttle logic, minPower for thruster %d = %12.10f\n",
                     ii, minPower);
         #endif
     }

      Integer numToFire       = numThrusters;  // start at the last one

      for (Integer ii = numThrusters-1; ii >= 0; ii--)
      {
         powerPerThruster = powerAvailable / numToFire;
         // Compute mean usable power, if we were to fire numToFire thrusters
         Real meanMinUsablePower = 0.0;
         for (unsigned int jj = 0; jj < numToFire; jj++)
            meanMinUsablePower += minUsablePowerPerThruster.at(ii);
         meanMinUsablePower /= numToFire;
         // If we can fire numToFire thrusters with the power available, we're done
         if (powerPerThruster > meanMinUsablePower)
            break;
         // Handle the special case where there is not enough power to fire
         // any thrusters
         if ((numToFire == 1) && (powerPerThruster < meanMinUsablePower))
         {
            numToFire = 0;
            break;
         }
         numToFire--;
      }
      #ifdef DEBUG_FINITE_BURN_POWER
         MessageInterface::ShowMessage("numToFire = %d, powerPerThruster = %12.10f\n",
               numToFire, powerPerThruster);
      #endif

      // Divide the power up across thrusters that should fire
      for (unsigned int nn = 0; nn < numToFire; nn++)
      {
         #ifdef DEBUG_FINITE_BURN_POWER
            MessageInterface::ShowMessage("Setting power on thruster %d to %12.10f\n",
                  (Integer) nn, powerPerThruster);
         #endif
         electricThrusters.at(nn)->SetPower(powerPerThruster);
      }

      // Set power to zero for all those that cannot fire
      for (unsigned int nono = numToFire; nono < numThrusters; nono++)
      {
         #ifdef DEBUG_FINITE_BURN_POWER
            MessageInterface::ShowMessage("Setting power on thruster %d to 0.0f\n",
                  (Integer) nono);
         #endif
         electricThrusters.at(nono)->SetPower(0.0);
      }

      electricThrusters.clear();
      minUsablePowerPerThruster.clear();
   }
   else
   {
      throw BurnException("Unknown value for ThrottleLogicAlgorithm\n");
   }

   return true;
}
