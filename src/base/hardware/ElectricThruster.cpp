//$Id$
//------------------------------------------------------------------------------
//                               ElectricThruster
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
// Created: 2014.06.25
//
/**
 * Class implementation for the Spacecraft electric engines
 * (aka ElectricThrusters).
 */
//------------------------------------------------------------------------------


#include "ElectricThruster.hpp"
//#include "ObjectReferencedAxes.hpp"
#include "Spacecraft.hpp"
#include "StringUtil.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include <sstream>
#include <math.h>          // for pow(real, real)

//#define DEBUG_ELECTRIC_THRUSTER
//#define DEBUG_ELECTRIC_THRUSTER_POWER
//#define DEBUG_ELECTRIC_THRUSTER_CONSTRUCTOR
//#define DEBUG_ELECTRIC_THRUSTER_SET
//#define DEBUG_ELECTRIC_THRUSTER_REF_OBJ
//#define DEBUG_ELECTRIC_THRUSTER_INIT
//#define DEBUG_MASS_FLOW_THRUST_VECTOR

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

/// labels/strings for the ThrustModel
StringArray ElectricThruster::thrustModelLabels;
/// Thrust coefficient units
StringArray ElectricThruster::thrustCoeffUnits;
/// Mass Flow coefficient units
StringArray ElectricThruster::mfCoeffUnits;

/// Labels used for the ElectricThruster element parameters.
const std::string
ElectricThruster::PARAMETER_TEXT[ElectricThrusterParamCount - ThrusterParamCount] =
{
   "ThrustModel",
   "MaximumUsablePower",
   "MinimumUsablePower",
   "ThrustCoeff1",
   "ThrustCoeff2",
   "ThrustCoeff3",
   "ThrustCoeff4",
   "ThrustCoeff5",
   "MassFlowCoeff1",
   "MassFlowCoeff2",
   "MassFlowCoeff3",
   "MassFlowCoeff4",
   "MassFlowCoeff5",
   "FixedEfficiency",
   "Isp",
   "ConstantThrust",
   "T_UNITS",
   "MF_UNITS",
};

/// Types of the parameters used by ElectricThrusters.
const Gmat::ParameterType
ElectricThruster::PARAMETER_TYPE[ElectricThrusterParamCount - ThrusterParamCount] =
{
   Gmat::ENUMERATION_TYPE, // "ThrustModel"
   Gmat::REAL_TYPE,        // "MaximumUsablePower"
   Gmat::REAL_TYPE,        // "MinimumUsablePower"
   Gmat::REAL_TYPE,        // "ThrustCoeff1"
   Gmat::REAL_TYPE,        // "ThrustCoeff2"
   Gmat::REAL_TYPE,        // "ThrustCoeff3"
   Gmat::REAL_TYPE,        // "ThrustCoeff4"
   Gmat::REAL_TYPE,        // "ThrustCoeff5"
   Gmat::REAL_TYPE,        // "MassFlowCoeff1"
   Gmat::REAL_TYPE,        // "MassFlowCoeff2"
   Gmat::REAL_TYPE,        // "MassFlowCoeff3"
   Gmat::REAL_TYPE,        // "MassFlowCoeff4"
   Gmat::REAL_TYPE,        // "MassFlowCoeff5"
   Gmat::REAL_TYPE,        // "FixedEfficiency"
   Gmat::REAL_TYPE,        // "Isp"
   Gmat::REAL_TYPE,        // "CnstantThrust"
   Gmat::STRINGARRAY_TYPE, // "T_UNITS"
   Gmat::STRINGARRAY_TYPE, // "MF_UNITS"
};


//------------------------------------------------------------------------------
//  ElectricThruster(std::string nomme)
//------------------------------------------------------------------------------
/**
 * ElectricThruster constructor.
 *
 * @param nomme Name of the ElectricThruster.
 *
 */
//------------------------------------------------------------------------------
ElectricThruster::ElectricThruster(const std::string &nomme) :
   Thruster("ElectricThruster",nomme),
   thrustModel     ("ThrustMassPolynomial"),
   maxUsablePower  (7.266),
   minUsablePower  (0.638),
   efficiency      (0.7),
   isp             (4200),
   constantThrust  (0.237),
   powerToUse      (0.0),
   powerToUse2     (0.0),
   powerToUse3     (0.0),
   powerToUse4     (0.0)
{
   objectTypes.push_back(Gmat::ELECTRIC_THRUSTER);
   objectTypeNames.push_back("ElectricThruster");
   parameterCount = ElectricThrusterParamCount;
   blockCommandModeAssignment = false;

   thrustCoeff[0]   =  -5.19082;
   thrustCoeff[1]   =   2.96519;
   thrustCoeff[2]   = -14.4789;
   thrustCoeff[3]   =  54.05382;
   thrustCoeff[4]   = - 0.00100092;

   massFlowCoeff[0] = -0.004776;
   massFlowCoeff[1] =  0.05717;
   massFlowCoeff[2] = -0.09956;
   massFlowCoeff[3] =  0.03211;
   massFlowCoeff[4] =  2.13781;

   // Available thrust model labels
   // Since it is static data, clear it first
   thrustModelLabels.clear();
   thrustModelLabels.push_back("ThrustMassPolynomial");
   thrustModelLabels.push_back("ConstantThrustAndIsp");
   thrustModelLabels.push_back("FixedEfficiency");

   // Thrust coefficient units
   thrustCoeffUnits.clear();
   thrustCoeffUnits.push_back("See Docs");
   thrustCoeffUnits.push_back("See Docs");
   thrustCoeffUnits.push_back("See Docs");
   thrustCoeffUnits.push_back("See Docs");
   thrustCoeffUnits.push_back("See Docs");

   // Mass Flow coefficient units
   mfCoeffUnits.clear();
   mfCoeffUnits.push_back("See Docs");
   mfCoeffUnits.push_back("See Docs");
   mfCoeffUnits.push_back("See Docs");
   mfCoeffUnits.push_back("See Docs");
   mfCoeffUnits.push_back("See Docs");

   for (Integer i=ThrusterParamCount; i < ElectricThrusterParamCount; i++)  // is this right?
      parameterWriteOrder.push_back(i);
}


//------------------------------------------------------------------------------
//  ~ElectricThruster()
//------------------------------------------------------------------------------
/**
 * ElectricThruster destructor.
 */
//------------------------------------------------------------------------------
ElectricThruster::~ElectricThruster()
{
}


//------------------------------------------------------------------------------
//  ElectricThruster(const ElectricThruster& th)
//------------------------------------------------------------------------------
/**
 * ElectricThruster copy constructor.
 *
 * @param th The object being copied.
 *
 */
//------------------------------------------------------------------------------
ElectricThruster::ElectricThruster(const ElectricThruster& th) :
   Thruster             (th),
   thrustModel          (th.thrustModel),
   maxUsablePower       (th.maxUsablePower),
   minUsablePower       (th.minUsablePower),
   efficiency           (th.efficiency),
   isp                  (th.isp),
   constantThrust       (th.constantThrust),
   powerToUse           (th.powerToUse),
   powerToUse2          (th.powerToUse2),
   powerToUse3          (th.powerToUse3),
   powerToUse4          (th.powerToUse4)
{
   #ifdef DEBUG_ELECTRIC_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("ElectricThruster::ElectricThruster(copy) entered, this = <%p>'%s', copying from <%p>'%s'\n",
       this, GetName().c_str(), &th, th.GetName().c_str());
   #endif

   parameterCount      = th.parameterCount;

   thrustModelLabels   = th.thrustModelLabels;
   thrustCoeffUnits    = th.thrustCoeffUnits;
   mfCoeffUnits        = th.mfCoeffUnits;

   memcpy(thrustCoeff,   th.thrustCoeff,   ELECTRIC_COEFF_COUNT * sizeof(Real));
   memcpy(massFlowCoeff, th.massFlowCoeff, ELECTRIC_COEFF_COUNT * sizeof(Real));

   isInitialized = false;

   for (Integer i=ThrusterParamCount; i < ElectricThrusterParamCount; i++)  // is this right?
      parameterWriteOrder.push_back(i);

   #ifdef DEBUG_ELECTRIC_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage("ElectricThruster::ElectricThruster(copy) exiting\n");
   #endif
}


//------------------------------------------------------------------------------
//  ElectricThruster& operator=(const ElectricThruster& th)
//------------------------------------------------------------------------------
/**
 * ElectricThruster assignment operator.
 *
 * @param th The object being copied.
 *
 * @return this object, with parameters set to the input object's parameters.
 *
 */
//------------------------------------------------------------------------------
ElectricThruster& ElectricThruster::operator=(const ElectricThruster& th)
{
   if (&th == this)
      return *this;

   #ifdef DEBUG_ELECTRIC_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage("ElectricThruster::operator= entered\n");
   #endif

   Thruster::operator=(th);

   thrustModel         = th.thrustModel;
   maxUsablePower      = th.maxUsablePower;
   minUsablePower      = th.minUsablePower;
   efficiency          = th.efficiency;
   isp                 = th.isp;
   constantThrust      = th.constantThrust;
   powerToUse          = th.powerToUse;
   powerToUse2         = th.powerToUse2;
   powerToUse3         = th.powerToUse3;
   powerToUse4         = th.powerToUse4;

   thrustModelLabels   = th.thrustModelLabels;
   thrustCoeffUnits    = th.thrustCoeffUnits;
   mfCoeffUnits        = th.mfCoeffUnits;

   memcpy(thrustCoeff,   th.thrustCoeff,   ELECTRIC_COEFF_COUNT * sizeof(Real));
   memcpy(massFlowCoeff, th.massFlowCoeff, ELECTRIC_COEFF_COUNT * sizeof(Real));

   isInitialized = false;

   #ifdef DEBUG_ELECTRIC_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage("ElectricThruster::operator= exiting\n");
   #endif

   return *this;
}


//---------------------------------------------------------------------------
//  GmatBase* Clone() const
//---------------------------------------------------------------------------
/**
 * Provides a clone of this object by calling the copy constructor.
 *
 * @return A GmatBase pointer to the cloned ElectricThruster.
 */
//---------------------------------------------------------------------------
GmatBase* ElectricThruster::Clone() const
{
   return new ElectricThruster(*this);
}


//---------------------------------------------------------------------------
//  void Copy(GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 *
 * @return A GmatBase pointer to the cloned ElectricThruster.
 */
//---------------------------------------------------------------------------
void ElectricThruster::Copy(const GmatBase* orig)
{
   operator=(*((ElectricThruster *)(orig)));
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
 */
//------------------------------------------------------------------------------
std::string ElectricThruster::GetParameterText(const Integer id) const
{
   if (id >= ThrusterParamCount && id < ElectricThrusterParamCount)
      return PARAMETER_TEXT[id - ThrusterParamCount];

   return Thruster::GetParameterText(id);
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
Integer ElectricThruster::GetParameterID(const std::string &str) const
{
   for (Integer i = ThrusterParamCount; i < ElectricThrusterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ThrusterParamCount])
         return i;
   }

   return Thruster::GetParameterID(str);
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
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElectricThruster::GetParameterType(const Integer id) const
{
   if (id >= ThrusterParamCount && id < ElectricThrusterParamCount)
      return PARAMETER_TYPE[id - ThrusterParamCount];

   return Thruster::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string ElectricThruster::GetParameterTypeString(const Integer id) const
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
bool ElectricThruster::IsParameterReadOnly(const Integer id) const
{
   if (id == T_UNITS || id == MF_UNITS)
      return true;

   return Thruster::IsParameterReadOnly(id);
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
bool ElectricThruster::IsParameterCommandModeSettable(const Integer id) const
{

   return Thruster::IsParameterCommandModeSettable(id);
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
Real ElectricThruster::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case MAXIMUM_USABLE_POWER:
         return maxUsablePower;
      case MINIMUM_USABLE_POWER:
         return minUsablePower;
      case THRUST_COEFF1:
         return thrustCoeff[0];
      case THRUST_COEFF2:
         return thrustCoeff[1];
      case THRUST_COEFF3:
         return thrustCoeff[2];
      case THRUST_COEFF4:
         return thrustCoeff[3];
      case THRUST_COEFF5:
         return thrustCoeff[4];
      case MASS_FLOW_COEFF1:
         return massFlowCoeff[0];
      case MASS_FLOW_COEFF2:
         return massFlowCoeff[1];
      case MASS_FLOW_COEFF3:
         return massFlowCoeff[2];
      case MASS_FLOW_COEFF4:
         return massFlowCoeff[3];
      case MASS_FLOW_COEFF5:
         return massFlowCoeff[4];
      case EFFICIENCY:
         return efficiency;
      case ISP:
         return isp;
      case CONSTANT_THRUST:
         return constantThrust;

      default:
         break;   // Default just drops through
   }

   return Thruster::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real ElectricThruster::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_ELECTRIC_THRUSTER_SET
   MessageInterface::ShowMessage
      ("ElectricThruster::SetRealParameter() '%s' entered, id=%d, value=%f\n",
       GetName().c_str(), id, value);
   #endif

   switch (id)
   {
      case MAXIMUM_USABLE_POWER:
         if (value <= 0.0)
         {
            std::stringstream ss("");
            ss << value;
            std::string errmsg =
               "The value of \"" + ss.str() + "\" for field \"MaximumUsablePower\""
               " on object \"" + instanceName + "\" is not an allowed value.\n"
               "The allowed values are: [Real number > 0]. ";
            throw HardwareException(errmsg);
         }
         return maxUsablePower = value;
      case MINIMUM_USABLE_POWER:
         if (value <= 0.0)
         {
            std::stringstream ss("");
            ss << value;
            std::string errmsg =
               "The value of \"" + ss.str() + "\" for field \"MinimumUsablePower\""
               " on object \"" + instanceName + "\" is not an allowed value.\n"
               "The allowed values are: [Real number > 0]. ";
            throw HardwareException(errmsg);
         }
         return minUsablePower   = value;
      case THRUST_COEFF1:
         return thrustCoeff[0]   = value;
      case THRUST_COEFF2:
         return thrustCoeff[1]   = value;
      case THRUST_COEFF3:
         return thrustCoeff[2]   = value;
      case THRUST_COEFF4:
         return thrustCoeff[3]   = value;
      case THRUST_COEFF5:
         return thrustCoeff[4]   = value;
      case MASS_FLOW_COEFF1:
         return massFlowCoeff[0] = value;
      case MASS_FLOW_COEFF2:
         return massFlowCoeff[1] = value;
      case MASS_FLOW_COEFF3:
         return massFlowCoeff[2] = value;
      case MASS_FLOW_COEFF4:
         return massFlowCoeff[3] = value;
      case MASS_FLOW_COEFF5:
         return massFlowCoeff[4] = value;
      case EFFICIENCY:
         return efficiency       = value;
      case ISP:
         return isp              = value;
      case CONSTANT_THRUST:
         return constantThrust   = value;

      default:
         break;   // Default just drops through
   }

   return Thruster::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or throw ab=n exception if
 *         there is no string association.
 */
//---------------------------------------------------------------------------
std::string ElectricThruster::GetStringParameter(const Integer id) const
{
   if (id == THRUST_MODEL)
      return thrustModel;

   return Thruster::GetStringParameter(id);
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 *
 * @return true if the string is stored, throw if the string is not stored.
 */
//---------------------------------------------------------------------------
bool ElectricThruster::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_ELECTRIC_THRUSTER_SET
   MessageInterface::ShowMessage
      ("ElectricThruster::SetStringParameter() '%s' entered, id=%d, value='%s'\n",
       GetName().c_str(), id, value.c_str());
   #endif

   switch (id)
   {
   case THRUST_MODEL:
      if (find(thrustModelLabels.begin(), thrustModelLabels.end(), value)
          == thrustModelLabels.end())
      {
         std::string modellist = thrustModelLabels[0];
         for (UnsignedInt n = 1; n < thrustModelLabels.size(); ++n)
            modellist += ", " + thrustModelLabels[n];

         std::string msg =
            "The value of \"" + value + "\" for field \"ThrustModel\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [ " + modellist + " ]. ";

         throw HardwareException(msg);
      }
      thrustModel = value;
      return true;
   default:
      return Thruster::SetStringParameter(id, value);
   }
}

//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& ElectricThruster::GetStringArrayParameter(const Integer id) const
{
   if (id == T_UNITS)
      return thrustCoeffUnits;

   if (id == MF_UNITS)
      return mfCoeffUnits;

   return Thruster::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray& GetPropertyEnumStrings(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Access an array of enumerated string data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& ElectricThruster::GetPropertyEnumStrings(const Integer id) const
{
   if (id == THRUST_MODEL)
      return thrustModelLabels;

   return Thruster::GetPropertyEnumStrings(id);
}


//------------------------------------------------------------------------------
//  const StringArray& GetPropertyEnumStrings(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Access an array of enumerated string data.
 *
 * @param <label> The parameter name.
 *
 * @return The requested StringArray
 */
//------------------------------------------------------------------------------
const StringArray& ElectricThruster::GetPropertyEnumStrings(const std::string &label) const
{
   return GetPropertyEnumStrings(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Sets up the bodies used in the thrust calculations.
 */
//------------------------------------------------------------------------------
bool ElectricThruster::Initialize()
{
   #ifdef DEBUG_ELECTRIC_THRUSTER_INIT
   MessageInterface::ShowMessage
      ("ElectricThruster::Initialize() <%p>'%s' entered, thrustModel=%s\n",
       this, GetName().c_str(), thrustModel.c_str());
   #endif

   bool retval = Thruster::Initialize();
   if (!retval)
      return false;

   // CHECK maxUsablePower > minUsablePower
   if (maxUsablePower <= minUsablePower)
   {
      std::string msg =
         "The value of field \"MaximumUsablePower\" on Electric Thruster \"";
      msg += instanceName + "\" must be greater than ";
      msg += "the value of field \"MinimumUsablePower\".\n";
      throw HardwareException(msg);
   }

   // Check that all attached tanks are ElectricTanks
   for (UnsignedInt i=0; i<tanks.size(); i++)
   {
      if (!tanks.at(i)->IsOfType("ElectricTank"))
      {
         std::string errmsg = "All tanks set on ElectricThruster ";
         errmsg += instanceName + " must be of type ElectricTank.\n";
         throw HardwareException(errmsg);
      }
   }

   #ifdef DEBUG_ELECTRIC_THRUSTER_INIT
   MessageInterface::ShowMessage
      ("ElectricThruster::Initialize() <%p>'%s' returning %s\n", this, GetName().c_str(),
       retval ? "true" : "false");
   #endif

   return retval;
}

//------------------------------------------------------------------------------
//  bool SetPower(Real allocatedPower)
//------------------------------------------------------------------------------
/**
 * Sets the allocated power level for the thruster.  This method does not
 * do much in this base class; it must be overridden in a class that
 * needs more functionality (e.g. ElectricThruster class).
 */
//------------------------------------------------------------------------------
bool ElectricThruster::SetPower(Real allocatedPower)
{
   power = allocatedPower;
   #ifdef DEBUG_ELECTRIC_THRUSTER_POWER
      MessageInterface::ShowMessage("For thruster %s, setting power to %12.10f\n",
            instanceName.c_str(), power);
   #endif
   // @todo - deal with units here!!!!
   // write warning here (once) if allocatedPower does not fall into
   // the specified minimum-maximum power range?
   return true;
}




//---------------------------------------------------------------------------
//  bool CalculateThrustAndIsp()
//---------------------------------------------------------------------------
/**
 * Evaluates the thrust and specific impulse polynomials.
 *
 *
 * @return true on successful evaluation.
 */
//---------------------------------------------------------------------------
bool ElectricThruster::CalculateThrustAndIsp()
{
   #ifdef DEBUG_MASS_FLOW_THRUST_VECTOR
      MessageInterface::ShowMessage(
            "Entering ElectricThruster::CalculateThrustAndIsp, power = %12.10f, minUsablePower = %12.10f\n",
            power, minUsablePower);
      MessageInterface::ShowMessage("    powerToUse  = %12.10f\n", powerToUse);
      MessageInterface::ShowMessage("    powerToUse2 = %12.10f\n", powerToUse2);
      MessageInterface::ShowMessage("    powerToUse3 = %12.10f\n", powerToUse3);
      MessageInterface::ShowMessage("    powerToUse4 = %12.10f\n", powerToUse4);
   #endif
   if (!thrusterFiring)
   {
      #ifdef DEBUG_MASS_FLOW_THRUST_VECTOR
         MessageInterface::ShowMessage(
               "In ElectricThruster::CalculateThrustAndIsp, thruster %s NOT FIRING!!\n",
               instanceName.c_str());
      #endif
      thrust  = 0.0;
      impulse = 0.0;
   }
   else
   {
      if (tanks.empty())
         throw HardwareException("ElectricThruster \"" + instanceName +
                                 "\" does not have a fuel tank");
      impulse = isp;

      if (powerToUse < minUsablePower)
      {
         thrust = 0.0;
         impulse = 0.0;
      }
      else if (thrustModel == "ThrustMassPolynomial")
      {
         thrust = ((thrustCoeff[4] * powerToUse4) +
                   (thrustCoeff[3] * powerToUse3) +
                   (thrustCoeff[2] * powerToUse2) +
                   (thrustCoeff[1] * powerToUse)  +
                   thrustCoeff[0]) / 1.0e3;

         Real mdot = ((massFlowCoeff[4] * powerToUse4) +
                      (massFlowCoeff[3] * powerToUse3) +
                      (massFlowCoeff[2] * powerToUse2) +
                      (massFlowCoeff[1] * powerToUse) +
                       massFlowCoeff[0]) / 1.0e6;

         impulse = thrust / (mdot * gravityAccel);
      }
      else if (thrustModel == "ConstantThrustAndIsp")
      {
         thrust = constantThrust; //  / 1.0e-3;
      }
      else // FixedEfficiency
      {
         thrust = (2.0 * efficiency * powerToUse) / //  * 0.001) /
                  (isp * gravityAccel * 0.001);
      }
   }

   if (thrust < 0.0)
   {
      thrust = 0.0;
      impulse = 0.0;
   }
   
   // Calculate applied thrust magnitude
   // Value of appliedThrustMag will be returned when ThrustMagnitude Parameter
   // gets evaluated
   appliedThrustMag = thrustScaleFactor * dutyCycle * thrust;

   return true;
}


//---------------------------------------------------------------------------
//  Real CalculateMassFlow()
//---------------------------------------------------------------------------
/**
 * Evaluates the time rate of change of mass due to a ElectricThruster firing.
 *
 *  \f[\frac{dm}{dt} = \frac{F_T}{I_{sp}} \f]
 *
 * @return the mass flow rate from this ElectricThruster, used in integration.
 */
//---------------------------------------------------------------------------
Real ElectricThruster::CalculateMassFlow()
{
   #ifdef DEBUG_MASS_FLOW_THRUST_VECTOR
      MessageInterface::ShowMessage(
            "Entering ElectricThruster::CalculateMassFlow, power = %12.10f, minUsablePower = %12.10f\n",
            power, minUsablePower);
   #endif
   powerToUse = power; // power was set by FiniteBurn
   if (!thrusterFiring)
   {
      #ifdef DEBUG_MASS_FLOW_THRUST_VECTOR
         MessageInterface::ShowMessage("ElectricThruster %s is not firing\n",
               instanceName.c_str());
      #endif

      return 0.0;
   }
   else
   {
      if (tanks.empty())
         throw HardwareException("ElectricThruster \"" + instanceName +
                                 "\" does not have a fuel tank");

      if (powerToUse < minUsablePower)
      {
         #ifdef DEBUG_MASS_FLOW_THRUST_VECTOR
            MessageInterface::ShowMessage("RETURNING zero!!!!\n");
         #endif
         mDot   = 0.0;
         thrust = 0.0;
         return 0.0;
      }

      if (powerToUse > maxUsablePower)
         powerToUse = maxUsablePower;

      powerToUse2 = powerToUse  * powerToUse;
      powerToUse3 = powerToUse2 * powerToUse;
      powerToUse4 = powerToUse3 * powerToUse;

      #ifdef DEBUG_MASS_FLOW_THRUST_VECTOR
         MessageInterface::ShowMessage("power = %12.10f, powerToUse = %12.10f\n",
               power, powerToUse);
      #endif

      // For now, always calculate T and I_sp
      if (!CalculateThrustAndIsp())
         throw HardwareException("ElectricThruster \"" + instanceName +
                                 "\" could not calculate dm/dt");

      if (thrustModel == "ThrustMassPolynomial")
      {
         mDot = ((massFlowCoeff[4] * powerToUse4) +
                 (massFlowCoeff[3] * powerToUse3) +
                 (massFlowCoeff[2] * powerToUse2) +
                 (massFlowCoeff[1] * powerToUse) +
                  massFlowCoeff[0]) / 1.0e6;
      }
      else if (thrustModel == "ConstantThrustAndIsp")
      {
         mDot = constantThrust  / (isp * gravityAccel); // do I need to divide by 1.0e-3 here?  or put the 0.001 * in there?
      }
      else // "FixedEfficiency"
      {
         Real ispG  = (isp * gravityAccel * 0.001);
         Real ispG2 =  ispG * ispG;
         mDot = (2.0 * efficiency * powerToUse * 0.001) / ispG2;
      }

      if (!decrementMass)
         mDot = 0.0;

//         if (impulse == 0.0)
//            throw HardwareException("ElectricThruster \"" + instanceName +
//                                    "\" has specific impulse == 0.0");
   }

   #ifdef DEBUG_MASS_FLOW_THRUST_VECTOR
      MessageInterface::ShowMessage(
            "   Thrust = %15.10f, Isp = %15.10f, gravity accel = %12.10f, TSF = %12.10f, "
            "dutyCycle = %15.10f, MassFlow = %15.10f T/Isp =  %12.10f\n",
            thrust, impulse, gravityAccel, thrustScaleFactor, dutyCycle, mDot,
            thrust/impulse);
   #endif

   // Update mDot here so that MassFlowRate Parameter can retrieve this mDot
   mDot = mDot * -dutyCycle;
   //return mDot * -dutyCycle;  // Flow rate should be negative in ODEs - CORRECT?
   return mDot;
}
