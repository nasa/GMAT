//$Id$
//------------------------------------------------------------------------------
//                               ChemicalThruster
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
// Author: Wendy Shoan/GSFC/GSSB
// Created: 2014.07.01
/**
 * Class implementation for the Spacecraft chemical engines (aka Thrusters).
 * Code pulled from original Thruster class.
 */
//------------------------------------------------------------------------------


#include "ChemicalThruster.hpp"
#include "StringUtil.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include <sstream>
#include <math.h>          // for pow(real, real)

//#define DEBUG_CHEMICAL_THRUSTER
//#define DEBUG_CHEMICAL_THRUSTER_CONSTRUCTOR
//#define DEBUG_CHEMICAL_THRUSTER_SET
//#define DEBUG_CHEMICAL_THRUSTER_REF_OBJ
//#define DEBUG_CHEMICAL_THRUSTER_INIT
//#define DEBUG_CHEMICAL_THRUSTER_CONVERT
//#define DEBUG_CHEMICAL_THRUSTER_CONVERT_ROTMAT
//#define DEBUG_BURN_CONVERT_ROTMAT
//#define DEBUG_THRUST_ISP

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

/// C-coefficient units
StringArray ChemicalThruster::cCoefUnits;
/// K-coefficient units
StringArray ChemicalThruster::kCoefUnits;

/// Labels used for the thruster element parameters.
const std::string
ChemicalThruster::PARAMETER_TEXT[ChemicalThrusterParamCount - ThrusterParamCount] =
{
   "C1",  "C2",  "C3",  "C4",  "C5",  "C6",  "C7",  "C8",
   "C9", "C10", "C11", "C12", "C13", "C14", "C15", "C16",

   "K1",  "K2",  "K3",  "K4",  "K5",  "K6",  "K7",  "K8",
   "K9", "K10", "K11", "K12", "K13", "K14", "K15", "K16",
   "C_UNITS",
   "K_UNITS",
};

/// Types of the parameters used by thrusters.
const Gmat::ParameterType
ChemicalThruster::PARAMETER_TYPE[ChemicalThrusterParamCount - ThrusterParamCount] =
{
   // Cs
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   // Ks
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
};


//------------------------------------------------------------------------------
//  ChemicalThruster(std::string nomme)
//------------------------------------------------------------------------------
/**
 * ChemicalThruster constructor with default VNB Local CoordinateSystem.
 *
 * @param nomme Name of the thruster.
 *
 * @note coordSystem and spacecraft are set through SetRefObject() during
 *       Sandbox initialization. localOrigin and j2000Body are reset when
 *       solarSystem is set. localCoordSystem is created during initialization
 *       or when new spacecraft is set
 */
//------------------------------------------------------------------------------
ChemicalThruster::ChemicalThruster(const std::string &nomme) :
   Thruster             ("ChemicalThruster", nomme)
{
   objectTypes.push_back(Gmat::CHEMICAL_THRUSTER);
   objectTypeNames.push_back("ChemicalThruster");
   parameterCount = ChemicalThrusterParamCount;
//   blockCommandModeAssignment = false;

   cCoefficients[0]  = 10.0;
   for (int i=1; i<COEFFICIENT_COUNT; i++)
      cCoefficients[i] = 0.0;

   kCoefficients[0]  = 300.0;
   for (int i=1; i<COEFFICIENT_COUNT; i++)
      kCoefficients[i] = 0.0;

   // C coefficient units
   cCoefUnits.clear();
   cCoefUnits.push_back("N");
   cCoefUnits.push_back("N/kPa");
   cCoefUnits.push_back("N");
   cCoefUnits.push_back("N/kPa");
   cCoefUnits.push_back("N/kPa^2");
   cCoefUnits.push_back("N/kPa^C7");
   cCoefUnits.push_back("None");
   cCoefUnits.push_back("N/kPa^C9");
   cCoefUnits.push_back("None");
   cCoefUnits.push_back("N/kPa^C11");
   cCoefUnits.push_back("None");
   cCoefUnits.push_back("N");
   cCoefUnits.push_back("None");
   cCoefUnits.push_back("1/kPa");
   cCoefUnits.push_back("None");
   cCoefUnits.push_back("1/kPa");

   // K coefficient units
   kCoefUnits.clear();
   kCoefUnits.push_back("s");
   kCoefUnits.push_back("s/kPa");
   kCoefUnits.push_back("s");
   kCoefUnits.push_back("s/kPa");
   kCoefUnits.push_back("s/kPa^2");
   kCoefUnits.push_back("s/kPa^K7");
   kCoefUnits.push_back("None");
   kCoefUnits.push_back("s/kPa^K9");
   kCoefUnits.push_back("None");
   kCoefUnits.push_back("s/kPa^K11");
   kCoefUnits.push_back("None");
   kCoefUnits.push_back("s");
   kCoefUnits.push_back("None");
   kCoefUnits.push_back("1/kPa");
   kCoefUnits.push_back("None");
   kCoefUnits.push_back("1/kPa");

   for (Integer i=ThrusterParamCount; i < ChemicalThrusterParamCount; ++i)
      parameterWriteOrder.push_back(i);
}


//------------------------------------------------------------------------------
//  ~ChemicalThruster()
//------------------------------------------------------------------------------
/**
 * ChemicalThruster destructor.
 */
//------------------------------------------------------------------------------
ChemicalThruster::~ChemicalThruster()
{
}


//------------------------------------------------------------------------------
//  ChemicalThruster(const ChemicalThruster& th)
//------------------------------------------------------------------------------
/**
 * ChemicalThruster copy constructor.
 *
 * @param th The object being copied.
 *
 */
//------------------------------------------------------------------------------
ChemicalThruster::ChemicalThruster(const ChemicalThruster& th) :
   Thruster             (th)
{
   #ifdef DEBUG_CHEMICAL_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("ChemicalThruster::ChemicalThruster(copy) entered, this = <%p>'%s', copying from <%p>'%s'\n",
       this, GetName().c_str(), &th, th.GetName().c_str());
   #endif

   parameterCount = th.parameterCount;

   memcpy(cCoefficients, th.cCoefficients, COEFFICIENT_COUNT * sizeof(Real));
   memcpy(kCoefficients, th.kCoefficients, COEFFICIENT_COUNT * sizeof(Real));

   for (Integer i=ThrusterParamCount; i < ChemicalThrusterParamCount; ++i)
      parameterWriteOrder.push_back(i);

   #ifdef DEBUG_CHEMICAL_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage("ChemicalThruster::ChemicalThruster(copy) exiting\n");
   #endif
}


//------------------------------------------------------------------------------
//  ChemicalThruster& operator=(const ChemicalThruster& th)
//------------------------------------------------------------------------------
/**
 * ChemicalThruster assignment operator.
 *
 * @param th The object being copied.
 *
 * @return this object, with parameters set to the input object's parameters.
 *
 */
//------------------------------------------------------------------------------
ChemicalThruster& ChemicalThruster::operator=(const ChemicalThruster& th)
{
   if (&th == this)
      return *this;

   #ifdef DEBUG_CHEMICAL_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage("Thruster::operator= entered\n");
   #endif

   Thruster::operator=(th);

   memcpy(cCoefficients, th.cCoefficients, COEFFICIENT_COUNT * sizeof(Real));
   memcpy(kCoefficients, th.kCoefficients, COEFFICIENT_COUNT * sizeof(Real));


   #ifdef DEBUG_CHEMICAL_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage("ChemicalThruster::operator= exiting\n");
   #endif

   return *this;
}


//---------------------------------------------------------------------------
//  GmatBase* Clone() const
//---------------------------------------------------------------------------
/**
 * Provides a clone of this object by calling the copy constructor.
 *
 * @return A GmatBase pointer to the cloned thruster.
 */
//---------------------------------------------------------------------------
GmatBase* ChemicalThruster::Clone() const
{
   return new ChemicalThruster(*this);
}


//---------------------------------------------------------------------------
//  void Copy(GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 *
 * @return A GmatBase pointer to the cloned thruster.
 */
//---------------------------------------------------------------------------
void ChemicalThruster::Copy(const GmatBase* orig)
{
   operator=(*((ChemicalThruster *)(orig)));
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
std::string ChemicalThruster::GetParameterText(const Integer id) const
{
   if (id >= ThrusterParamCount && id < ChemicalThrusterParamCount)
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
Integer ChemicalThruster::GetParameterID(const std::string &str) const
{
   for (Integer i = ThrusterParamCount; i < ChemicalThrusterParamCount; i++)
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
Gmat::ParameterType ChemicalThruster::GetParameterType(const Integer id) const
{
   if (id >= ThrusterParamCount && id < ChemicalThrusterParamCount)
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
std::string ChemicalThruster::GetParameterTypeString(const Integer id) const
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
bool ChemicalThruster::IsParameterReadOnly(const Integer id) const
{
   if (id == C_UNITS || id == K_UNITS)
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
bool ChemicalThruster::IsParameterCommandModeSettable(const Integer id) const
{
   // reenabled setting per ED
   if (id == C1  || id == C2  || id == C3  || id == C4  ||
       id == C5  || id == C6  || id == C7  || id == C8  ||
       id == C9  || id == C10 || id == C11 || id == C12 ||
       id == C13 || id == C14 || id == C15 || id == C16 ||
       id == K1  || id == K2  || id == K3  || id == K4  ||
       id == K5  || id == K6  || id == K7  || id == K8  ||
       id == K9  || id == K10 || id == K11 || id == K12 ||
       id == K13 || id == K14 || id == K15 || id == K16)
       return true;

   // Activate all of the other thruster specific IDs  // not sure about this **
   if (id >= ThrusterParamCount)
      return true;

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
Real ChemicalThruster::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case C1:
         return cCoefficients[0];
      case C2:
         return cCoefficients[1];
      case C3:
         return cCoefficients[2];
      case C4:
         return cCoefficients[3];
      case C5:
         return cCoefficients[4];
      case C6:
         return cCoefficients[5];
      case C7:
         return cCoefficients[6];
      case C8:
         return cCoefficients[7];
      case C9:
         return cCoefficients[8];
      case C10:
         return cCoefficients[9];
      case C11:
         return cCoefficients[10];
      case C12:
         return cCoefficients[11];
      case C13:
         return cCoefficients[12];
      case C14:
         return cCoefficients[13];
      case C15:
         return cCoefficients[14];
      case C16:
         return cCoefficients[15];

      case K1:
         return kCoefficients[0];
      case K2:
         return kCoefficients[1];
      case K3:
         return kCoefficients[2];
      case K4:
         return kCoefficients[3];
      case K5:
         return kCoefficients[4];
      case K6:
         return kCoefficients[5];
      case K7:
         return kCoefficients[6];
      case K8:
         return kCoefficients[7];
      case K9:
         return kCoefficients[8];
      case K10:
         return kCoefficients[9];
      case K11:
         return kCoefficients[10];
      case K12:
         return kCoefficients[11];
      case K13:
         return kCoefficients[12];
      case K14:
         return kCoefficients[13];
      case K15:
         return kCoefficients[14];
      case K16:
         return kCoefficients[15];

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
Real ChemicalThruster::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_CHEMICAL_THRUSTER_SET
   MessageInterface::ShowMessage
      ("ChemicalThruster::SetRealParameter() '%s' entered, id=%d, value=%f\n",
       GetName().c_str(), id, value);
   #endif

   switch (id)
   {
      // Thrust coefficients
      case C1:
         return cCoefficients[0] = value;
      case C2:
         if (value != 0.0)
            constantExpressions = false;
         return cCoefficients[1] = value;
      case C3:
         return cCoefficients[2] = value;
      case C4:
         if (value != 0.0)
            constantExpressions = false;
         return cCoefficients[3] = value;
      case C5:
         if (value != 0.0)
            constantExpressions = false;
         return cCoefficients[4] = value;
      case C6:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[5] = value;
      case C7:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[6] = value;
      case C8:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[7] = value;
      case C9:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[8] = value;
      case C10:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[9] = value;
      case C11:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[10] = value;
      case C12:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[11] = value;
      case C13:
         if ((value != 0.0) && (value != 1.0))
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[12] = value;
      case C14:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[13] = value;
      case C15:
         return cCoefficients[14] = value;
      case C16:
         return cCoefficients[15] = value;

      // Isp Coefficients
      case K1:
         return kCoefficients[0] = value;
      case K2:
         if (value != 0.0)
            constantExpressions = false;
         return kCoefficients[1] = value;
      case K3:
         return kCoefficients[2] = value;
      case K4:
         if (value != 0.0)
            constantExpressions = false;
         return kCoefficients[3] = value;
      case K5:
         if (value != 0.0)
            constantExpressions = false;
         return kCoefficients[4] = value;
      case K6:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[5] = value;
      case K7:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[6] = value;
      case K8:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[7] = value;
      case K9:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[8] = value;
      case K10:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[9] = value;
      case K11:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[10] = value;
      case K12:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[11] = value;
      case K13:
         if ((value != 0.0) && (value != 1.0))
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[12] = value;
      case K14:
         if (value != 0.0)
         {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[13] = value;
      case K15:
         return kCoefficients[14] = value;
      case K16:
         return kCoefficients[15] = value;

      default:
         break;   // Default just drops through
   }

   return Thruster::SetRealParameter(id, value);
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
const StringArray& ChemicalThruster::GetStringArrayParameter(const Integer id) const
{
   if (id == C_UNITS)
      return cCoefUnits;

   if (id == K_UNITS)
      return kCoefUnits;

   return Thruster::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Sets up the bodies used in the thrust calculations.
 */
//------------------------------------------------------------------------------
bool ChemicalThruster::Initialize()
{
   #ifdef DEBUG_CHEMICAL_THRUSTER_INIT
   MessageInterface::ShowMessage
      ("ChemicalThruster::Initialize() <%p>'%s' entered, spacecraft=<%p>, "
       "usingLocalCoordSys=%d, coordSystem=<%p>, localCoordSystem=<%p>\n",
       this, GetName().c_str(), spacecraft, usingLocalCoordSys,
       coordSystem, localCoordSystem);
   #endif

   bool retval = Thruster::Initialize();
   if (!retval)
      return false;

   // Check that all attached tanks are ChemicalTanks
   for (UnsignedInt i=0; i<tanks.size(); i++)
   {
      if (!tanks.at(i)->IsOfType("ChemicalTank"))
      {
         std::string errmsg = "All tanks set on ChemicalThruster ";
         errmsg += instanceName + " must be of type ChemicalTank.\n";
         throw HardwareException(errmsg);
      }
   }

   return retval;
}


//---------------------------------------------------------------------------
//  bool CalculateThrustAndIsp()
//---------------------------------------------------------------------------
/**
 * Evaluates the thrust and specific impulse polynomials.
 *
 * GMAT uses polynomial expressions for the thrust and specific impulse
 * imparted to the spacecraft by thrusters attached to the spacecraft.
 * Both thrust and specific impulse are expressed as functions of pressure
 * and temperature. The pressure and temperature are values obtained
 * from fuel tanks containing the fuel. All measurements in GMAT are
 * expressed in metric units. The thrust, in Newtons, applied by a spacecraft
 * engine is given by
 *
   \f[
    F_{T}(P,T) = C_{1}+C_{2}P + \left\{ C_{3}+C_{4}P+C_{5}P^{2}+C_{6}P^{C_{7}}+
                   C_{8}P^{C_{9}}+C_{10}P^{C_{11}}+
                   C_{12}C_{13}^{C_{14}P}\right\} \left(\frac{T}{T_{ref}}
                   \right)^{1+C_{15}+C_{16}P}\f]
 *
 * Pressures are expressed in kilopascals, and temperatures in degrees
 * centigrade. The coefficients C1 - C16 are set by the user. Each coefficient
 * is expressed in units commiserate with the final expression in Newtons;
 * for example, C1 is expressed in Newtons, C2 in Newtons per kilopascal,
 * and so forth.
 *
 * Specific Impulse, measured in sec is expressed using a similar equation:
 *
   \f[
    I_{sp}(P,T) = K_{1}+K_{2}P + \left\{ K_{3}+K_{4}P+K_{5}P^{2}+K_{6}P^{K_{7}}+
                    K_{8}P^{K_{9}}+K_{10}P^{K_{11}}+K_{12}K_{13}^{K_{14}P}\right\}
         \left(\frac{T}{T_{ref}}\right)^{1+K_{15}+K_{16}P}\f]
 *
 * @return true on successful evaluation.
 */
//---------------------------------------------------------------------------
bool ChemicalThruster::CalculateThrustAndIsp()
{
   #ifdef DEBUG_THRUST_ISP
   MessageInterface::ShowMessage
      ("ChemicalThruster::CalculateThrustAndIsp() <%p>'%s' entered, thrusterFiring=%d, "
       "constantExpressions=%d\n", this, instanceName.c_str(), thrusterFiring, constantExpressions);
   #endif
   
   if (!thrusterFiring)
   {
      thrust  = 0.0;
      impulse = 0.0;
   }
   else
   {
      if (tanks.empty())
         throw HardwareException("ChemicalThruster \"" + instanceName +
                                 "\" does not have a fuel tank");

      // Require that the tanks all be at the same pressure and temperature
      Integer pressID = tanks[0]->GetParameterID("Pressure");
      Integer tempID = tanks[0]->GetParameterID("Temperature");
      Integer refTempID = tanks[0]->GetParameterID("RefTemperature");

//      pressure = tanks[0]->GetRealParameter(pressID);
//      temperatureRatio = tanks[0]->GetRealParameter(tempID) /
//                         tanks[0]->GetRealParameter(refTempID);

      // Build the weighted temperature and pressure
      Real mixTotal = 0.0;
      Real pressureSum = 0.0;
      Real tempSum = 0.0;
      Real refTempSum = 0.0;
      for (UnsignedInt i = 0; i < mixRatio.GetSize(); ++i)
      {
         mixTotal += mixRatio[i];
         pressureSum += tanks[i]->GetRealParameter(pressID) * mixRatio[i];
         tempSum += tanks[i]->GetRealParameter(tempID) * mixRatio[i];
         refTempSum += tanks[i]->GetRealParameter(refTempID) * mixRatio[i];
      }
      pressure = pressureSum / mixTotal;

      // Note: numerator and denominator both divide by mixTotal, so dividends cancel
      temperatureRatio = tempSum / refTempSum;

      thrust = cCoefficients[2];
      impulse = kCoefficients[2];

      if (!constantExpressions)
      {

         thrust  += pressure*(cCoefficients[3] + pressure*cCoefficients[4]);
         impulse += pressure*(kCoefficients[3] + pressure*kCoefficients[4]);

         // For efficiency, if thrust and Isp are simple, don't bother evaluating
         // higher order terms
         if (!simpleExpressions) {
            thrust  += cCoefficients[5] * pow(pressure, cCoefficients[6]) +
                       cCoefficients[7] * pow(pressure, cCoefficients[8]) +
                       cCoefficients[9] * pow(pressure, cCoefficients[10]) +
                       cCoefficients[11] * pow(cCoefficients[12],
                                              pressure * cCoefficients[13]);

            impulse += kCoefficients[5] * pow(pressure, kCoefficients[6]) +
                       kCoefficients[7] * pow(pressure, kCoefficients[8]) +
                       kCoefficients[9] * pow(pressure, kCoefficients[10]) +
                       kCoefficients[11] * pow(kCoefficients[12],
                                              pressure * kCoefficients[13]);
         }
      }

//       thrust  *= pow(temperatureRatio, (1.0 + cCoefficients[14] +
//                      pressure*cCoefficients[15])) * thrustScaleFactor * dutyCycle;
      thrust  *= pow(temperatureRatio, (1.0 + cCoefficients[14] +
                     pressure*cCoefficients[15]));
      impulse *= pow(temperatureRatio, (1.0 + kCoefficients[14] +
                     pressure*kCoefficients[15]));

      // Now add the temperature independent pieces
      thrust  += cCoefficients[0] + cCoefficients[1] * pressure;
      impulse += kCoefficients[0] + kCoefficients[1] * pressure;
   }
   
   // Calculate applied thrust magnitude
   // Value of appliedThrustMag will be returned when ThrustMagnitude Parameter
   // gets evaluated
   appliedThrustMag = thrustScaleFactor * dutyCycle * thrust;
   
   #ifdef DEBUG_THRUST_ISP
   MessageInterface::ShowMessage
      ("ChemicalThruster::CalculateThrustAndIsp() <%p>'%s' leaving, thrust=%.12f, "
       "impulse=%.12f, appliedThrustMag=%.12f\n", this, instanceName.c_str(),
       thrust, impulse, appliedThrustMag);
   #endif
   return true;
}


//---------------------------------------------------------------------------
//  Real CalculateMassFlow()
//---------------------------------------------------------------------------
/**
 * Evaluates the time rate of change of mass due to a thruster firing.
 *
 *  \f[\frac{dm}{dt} = \frac{F_T}{I_{sp}} \f]
 *
 * @return the mass flow rate from this thruster, used in integration.
 */
//---------------------------------------------------------------------------
Real ChemicalThruster::CalculateMassFlow()
{
   if (!thrusterFiring)
   {
      #ifdef DEBUG_CHEMICAL_THRUSTER
         MessageInterface::ShowMessage("ChemicalThruster %s is not firing\n",
               instanceName.c_str());
      #endif

      mDot = 0.0;
      //return 0.0;
   }
   else
   {
      if (tanks.empty())
         throw HardwareException("ChemicalThruster \"" + instanceName +
                                 "\" does not have a fuel tank");

      // For now, always calculate T and I_sp
      //if (!constantExpressions) {
         if (!CalculateThrustAndIsp())
            throw HardwareException("Thruster \"" + instanceName +
                                    "\" could not calculate dm/dt");
         if (impulse == 0.0)
            throw HardwareException("Thruster \"" + instanceName +
                                    "\" has specific impulse == 0.0");

         // Mass flows out, so need a negative value here

         if (decrementMass)
            mDot = -thrust / (gravityAccel * impulse);
         else
            mDot = 0.0;

         // Old code:
         // mDot = -(thrust/thrustScaleFactor) / (gravityAccel * impulse);
      //}
   }

   #ifdef DEBUG_CHEMICAL_THRUSTER
      MessageInterface::ShowMessage(
            "   Thrust = %15lf, Isp = %15lf, gravity accel = %lf, TSF = %lf, "
            "dutyCycle = %15lf, MassFlow = %15lf T/Isp =  %lf\n",
            thrust, impulse, gravityAccel, thrustScaleFactor, dutyCycle, mDot,
            thrust/impulse);
   #endif

   // Update mDot here so that MassFlowRate Parameter can retrieve this mDot
   mDot = mDot * dutyCycle;
   //return mDot * dutyCycle;
   return mDot;
}
