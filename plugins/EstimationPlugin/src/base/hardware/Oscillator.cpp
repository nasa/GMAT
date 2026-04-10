//$Id: Oscillator.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             Oscillator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Tuan D. Nguyen
// Created: 2021/02/19 by Tuan D. Nguyen (GSFC-NASA)
//
/**
 * Implementation for the Oscillator class
 */
//------------------------------------------------------------------------------

#include "Oscillator.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include "TimeSystemConverter.hpp"
#include "StringUtil.hpp"

//#define DEBUG_CONSTRUCTOR
//#define DEBUG_INITIALIZE
//#define DEBUG_SET_REAL_PARA

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Text strings used to script Oscillator properties
const std::string
Oscillator::PARAMETER_TEXT[OscillatorParamCount - TransmitterParamCount] =
{
   "FrequencyDrift",
   "FrequencyDriftNoiseSigma",
   "FrequencyPolynomialCoefficients",
   "FrequencyPolynomialCoefficientsNoiseSigmas",
   "InitialSettingTime",
   "SettingTimeType",
   "SolveFors",
};

/// Integer IDs associated with the Oscillator properties
const Gmat::ParameterType
Oscillator::PARAMETER_TYPE[OscillatorParamCount - TransmitterParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
};

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Oscillator(const std::string &ofType, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name for the Oscillator
 */
//------------------------------------------------------------------------------
Oscillator::Oscillator(const std::string &ofType, const std::string &name) :
   Transmitter           (ofType, name),
   freqDrift             (0.0),                    // unit: second
   freqDriftSigma        (1.0e35),                 // unit: second
   initialEpochStr       ("21545"),
   initialEpochTypeStr   ("TAIModJulian")
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("Oscillator::Oscillator() <%p,%s>:default constructor start\n", this, GetName().c_str());
#endif

   objectTypeNames.push_back("Oscillator");
   parameterCount = OscillatorParamCount;

   frequencyModel = "Taylor's series";                    // frequency model may be "Taylor's series", or "Fourier's series"

   for (Integer i = TransmitterParamCount; i < OscillatorParamCount; ++i)
      parameterWriteOrder.push_back(i);
   
   // Set default value for polynomial coefficients
   // Default value is C0 = default constant frequency of Transmitter and all other Ck = 0
   // Default C0's noise sigma is infinitive. Numerically, it is set to 1.0e35
   freqPolynomialCoeffs.Set(1, frequency);
   freqPolynomialCoeffSigmas.Set(1, 1.0e35);
   
   covariance.AddCovarianceElement("FrequencyDrift", this);
   covariance.AddCovarianceElement("FrequencyPolynomialCoefficients", this);

   Integer start = covariance.GetSubMatrixLocationStart("FrequencyDrift");
   covariance(start, start) = freqDriftSigma * freqDriftSigma;
   start = covariance.GetSubMatrixLocationStart("FrequencyPolynomialCoefficients");
   for (Integer i = 0; i < freqPolynomialCoeffs.GetSize(); ++i)
      covariance(start + i, start + i) = freqPolynomialCoeffSigmas[i] * freqPolynomialCoeffSigmas[i];


#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("Oscillator::Oscillator() <%p,%s>:default constructor end\n", this, GetName().c_str());
#endif
}

//------------------------------------------------------------------------------
// ~Oscillator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Oscillator::~Oscillator()
{
}


//------------------------------------------------------------------------------
// Oscillator(const Oscillator & trans)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param trans The Oscillator that provides configuration data for this new
 *              one.
 */
//------------------------------------------------------------------------------
Oscillator::Oscillator(const Oscillator & osc) :
   Transmitter                (osc),
   freqDrift                  (osc.freqDrift),
   freqDriftSigma             (osc.freqDriftSigma),
   freqPolynomialCoeffs       (osc.freqPolynomialCoeffs),
   freqPolynomialCoeffSigmas  (osc.freqPolynomialCoeffSigmas),
   initialEpochStr            (osc.initialEpochStr),
   initialEpochTypeStr        (osc.initialEpochTypeStr),
   solveforNames              (osc.solveforNames)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("Oscillator::Oscillator(<%p,%s>) <%p,%s>:copy constructor start\n", &osc, osc.GetName().c_str(), this, GetName().c_str());
#endif

   std::string toStr;
   TimeSystemConverter::Instance()->Convert(initialEpochTypeStr, NULL, initialEpochStr, "A1ModJulian", initialEpoch, toStr);

#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("freqDrift = %.15lf\n", freqDrift);
   MessageInterface::ShowMessage("freqDriftSigma = %.15lf\n", freqDriftSigma);
   MessageInterface::ShowMessage("freqPolynomialCoeffs = [%.15le  %.15le  %.15le]\n", freqPolynomialCoeffs[0], freqPolynomialCoeffs[1], freqPolynomialCoeffs[2]);
   MessageInterface::ShowMessage("initialEpochTypeStr = %s\n", initialEpochTypeStr.c_str());
   MessageInterface::ShowMessage("initialEpochStr = %s\n", initialEpochStr.c_str());
   MessageInterface::ShowMessage("initialEpoch = %.15lf\n", initialEpoch.GetMjd());
#endif
   
   for (Integer i = TransmitterParamCount; i < OscillatorParamCount; ++i)
      parameterWriteOrder.push_back(i);

   Integer start = covariance.GetSubMatrixLocationStart("FrequencyDrift");
   covariance(start, start) = freqDriftSigma * freqDriftSigma;
   start = covariance.GetSubMatrixLocationStart("FrequencyPolynomialCoefficients");
   for (Integer i = 0; i < freqPolynomialCoeffs.GetSize(); ++i)
      covariance(start + i, start + i) = freqPolynomialCoeffSigmas[i] * freqPolynomialCoeffSigmas[i];

#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("Oscillator::Oscillator(<%p,%s>) <%p,%s>:copy constructor end\n", &osc, osc.GetName().c_str(), this, GetName().c_str());
#endif

}


//------------------------------------------------------------------------------
// Oscillator& operator =(const Oscillator &osc)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param trans The Oscillator that provides configuration data for this one.
 *
 * @return This Oscillator, set to match trans.
 */
//------------------------------------------------------------------------------
Oscillator& Oscillator::operator =(const Oscillator &osc)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("Oscillator::operator =(<%p,%s>) <%p,%s>: assign start\n", &osc, osc.GetName().c_str(), this, GetName().c_str());
#endif

   if (this != &osc)
   {
      Transmitter::operator=(osc);

      freqDrift                 = osc.freqDrift;
      freqDriftSigma            = osc.freqDriftSigma;
      freqPolynomialCoeffs      = osc.freqPolynomialCoeffs;
      freqPolynomialCoeffSigmas = osc.freqPolynomialCoeffSigmas;
      initialEpochStr           = osc.initialEpochStr;
      initialEpochTypeStr       = osc.initialEpochTypeStr;
      solveforNames             = osc.solveforNames;

      std::string toStr;
      TimeSystemConverter::Instance()->Convert(initialEpochTypeStr, NULL, initialEpochStr, "A1ModJulian", initialEpoch, toStr);

      Integer start = covariance.GetSubMatrixLocationStart("FrequencyDrift");
      covariance(start, start) = freqDriftSigma * freqDriftSigma;
      start = covariance.GetSubMatrixLocationStart("FrequencyPolynomialCoefficients");
      for (Integer i = 0; i < freqPolynomialCoeffs.GetSize(); ++i)
         covariance(start + i, start + i) = freqPolynomialCoeffSigmas[i] * freqPolynomialCoeffSigmas[i];

   }

#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("Oscillator::operator =(<%p,%s>) <%p,%s>: assign end\n", &osc, osc.GetName().c_str(), this, GetName().c_str());
#endif
   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this Oscillator
 *
 * @return A new Oscillator configured like this one.
 */
//------------------------------------------------------------------------------
GmatBase *Oscillator::Clone() const
{
   return new Oscillator(*this);
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
void Oscillator::Copy(const GmatBase* orig)
{
   operator=(*((Oscillator *)(orig)));
}



//-----------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//-----------------------------------------------------------------------------
/**
 * Method used to find the integer ID associated with the script string for an
 * Oscillator object property.
 *
 * @param str The script string
 *
 * @return The integer ID associated with the string
 */
//-----------------------------------------------------------------------------
Integer Oscillator::GetParameterID(const std::string & str) const
{
   for (Integer i = TransmitterParamCount; i < OscillatorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - TransmitterParamCount])
      {
         if (IsParameterReadOnly(i))
            throw HardwareException("Error: Parameter '" + str + "' was not defined in GMAT Oscillator's syntax.\n");
         
         return i;
      }
   }

   return Transmitter::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Finds the script string associated with a Oscillator object's property,
 * given the integer ID for that property.
 *
 * @param id The property's ID.
 *
 * @return The string associated with the property.
 */
//------------------------------------------------------------------------------
std::string Oscillator::GetParameterText(const Integer id) const
{
   if (id >= TransmitterParamCount && id < OscillatorParamCount)
      return PARAMETER_TEXT[id - TransmitterParamCount];
   return Transmitter::GetParameterText(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the Oscillator property associated
 * with the input ID.
 *
 * @param id The property's ID
 *
 * @return A text description of the property's type.
 */
//------------------------------------------------------------------------------
std::string Oscillator::GetParameterTypeString(const Integer id) const
{
   return Transmitter::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the enumerated parameter type for a specified property
 *
 * @param id The property's ID
 *
 * @return The ParameterType of the object.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Oscillator::GetParameterType(const Integer id) const
{
   if (id >= TransmitterParamCount && id < OscillatorParamCount)
      return PARAMETER_TYPE[id - TransmitterParamCount];

   return Transmitter::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units for a property
 *
 * @param id The property's ID
 *
 * @return The units for the property
 *
 * @note: This method is not yet used in GMAT.  Once the code uses it, the unit
 * data for the Oscillator properties will need to be specified.
 */
//------------------------------------------------------------------------------
std::string Oscillator::GetParameterUnit(const Integer id) const
{
   switch (id)
   {
      case OSC_FREQUENCY_DRIFT:
         return "sec";
      case OSC_FREQUENCY_DRIFT_NOISESIGMA:
         return "sec";
      case OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS:
         return "";
      case OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA:
         return "";
      case OSC_INITIAL_EPOCH:
         return "";
      case OSC_INITIAL_EPOCH_TYPE:
         return "";
      default:
         break;
   }

   return Transmitter::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Specifies whether a property is an expected user scripted parameter.
 *
 * This method is used when an object is serialized to determine if the
 * corresponding property should be included in the serialization.  Typical use
 * is when an object is written to the screen or to a file; objects identified
 * as "read only" by a return value of true from this method are but places in
 * the text shown on the screen or in the file.
 *
 * Users can script the "read only" parameters, but do so at their own risk.
 *
 * @param label The scriptable label of the property
 *
 * @return true if the property is read only, false if not.
 */
//------------------------------------------------------------------------------
bool Oscillator::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Specifies whether a property is an expected user scripted parameter.
 *
 * This method is used when an object is serialized to determine if the
 * corresponding property should be included in the serialization.  Typical use
 * is when an object is written to the screen or to a file; objects identified
 * as "read only" by a return value of true from this method are but places in
 * the text shown on the screen or in the file.
 *
 * Users can script the "read only" parameters, but do so at their own risk.
 *
 * @param id The integer ID of the property
 *
 * @return true if the property is read only, false if not.
 */
//------------------------------------------------------------------------------
bool Oscillator::IsParameterReadOnly(const Integer id) const
{
   if (id == FREQUENCY_MODEL)
      return true;

   return Transmitter::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param id The integer ID of the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Real Oscillator::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case OSC_FREQUENCY_DRIFT:
         return freqDrift;
      case OSC_FREQUENCY_DRIFT_NOISESIGMA:
         return freqDriftSigma;
      default:
         break;
   }

   return Transmitter::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param id The integer ID of the property
 * @param value The new value for the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Real Oscillator::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_SET_REAL_PARA
      MessageInterface::ShowMessage("Oscillator::SetRealParameter(id = %d, value = %le)  name of oscillator = '%s'\n",id, value, GetName().c_str());
   #endif

   switch (id)
   {
      case OSC_FREQUENCY_DRIFT:
         //if (value >= 0.0)
            freqDrift = value;
         //else
         //   throw HardwareException("Error: frequency drift set to " + GetName() + "."+ GetParameterText(id) +" is a negative number.\n");
         return freqDrift;

      case OSC_FREQUENCY_DRIFT_NOISESIGMA:
         if (value >= 0.0)
         {
            freqDriftSigma = value;
            Integer start = covariance.GetSubMatrixLocationStart("FrequencyDrift");
            covariance(start, start) = freqDriftSigma * freqDriftSigma;
         }
         else
            throw HardwareException("Error: frequency drift noise sigma set to " + GetName() + "." + GetParameterText(id) + " is a negative number.\n");
         return freqDriftSigma;

      default:
         break;
   }

   return Transmitter::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param label The scriptable label for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Real Oscillator::GetRealParameter(const std::string & label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param label The scriptable label for the property
 * @param value The new value for the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Real Oscillator::SetRealParameter(const std::string & label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an element of a real array property.
 *
 * @param id         The integer ID of the property
 * @param index      The index of an element in a real array
 *
 * @return The value of the element specified by index
 */
 //------------------------------------------------------------------------------
Real Oscillator::GetRealParameter(const Integer id, const Integer index) const
{
   switch (id)
   {
   case OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS:
      if ((index < 0)||(index >= freqPolynomialCoeffs.GetSize()))
         throw HardwareException("Error: index is out of bound when getting value of "+GetName()+".FrequencyPolynomialCoefficients parameter.");

      return freqPolynomialCoeffs[index];
      break;

   case OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA:
      if ((index < 0) || (index >= freqPolynomialCoeffSigmas.GetSize()))
         throw HardwareException("Error: index is out of bound when getting value of " + GetName() + ".FrequencyPolynomialCoefficientNoiseSigmas parameter.");
         
      return freqPolynomialCoeffs[index];
      break;

   default:
      break;
   }

   return Transmitter::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, 
//                       const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value of an element of a real array property.
 *
 * @param id         The integer ID of the property
 * @param value      The value set to the array element
 * @param index      The index of an element in a real array
 *
 * @return The value of the element specified by index
 */
 //------------------------------------------------------------------------------
Real Oscillator::SetRealParameter(const Integer id, const Real value, const Integer index)
{
   switch (id)
   {
   case OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS:
      if ((0 <= index) && (index < freqPolynomialCoeffs.GetSize()))
         freqPolynomialCoeffs[index] = value;
      else if (index == freqPolynomialCoeffs.GetSize())
      {
         Rvector temp = freqPolynomialCoeffs;
         freqPolynomialCoeffs.Resize(temp.GetSize() + 1);
         for (Integer i = 0; i < temp.GetSize(); ++i)
            freqPolynomialCoeffs.SetElement(i, temp[i]);
         freqPolynomialCoeffs.SetElement(temp.GetSize(), value);
      }
      else
         throw HardwareException("Error: index is out of bound when setting value to " + GetName() + ".FrequencyPolynomialCoefficients parameter.");

      return freqPolynomialCoeffs[index];
      break;

   case OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA:
      if ((0 <= index) && (index < freqPolynomialCoeffSigmas.GetSize()))
      {
         freqPolynomialCoeffSigmas[index] = value;
         Integer start = covariance.GetSubMatrixLocationStart("FrequencyPolynomialCoefficients");
         covariance(start+index, start+index) = value * value;
      }
      else if (index == freqPolynomialCoeffSigmas.GetSize())
      {
         Rvector temp = freqPolynomialCoeffSigmas;
         freqPolynomialCoeffSigmas.Resize(temp.GetSize() + 1);
         for (Integer i = 0; i < temp.GetSize(); ++i)
            freqPolynomialCoeffSigmas.SetElement(i, temp[i]);
         freqPolynomialCoeffSigmas.SetElement(temp.GetSize(), value);

         Integer elementSize = covariance.GetElementSize("FrequencyPolynomialCoefficients");
         if (elementSize < freqPolynomialCoeffSigmas.GetSize())
         {
            // Increasing its size
            Integer submatrixSize = covariance.GetElementSize("FrequencyPolynomialCoefficients");
            covariance.IncreasingElementSize("FrequencyPolynomialCoefficients", submatrixSize, 1);
         }
         Integer index = freqPolynomialCoeffSigmas.GetSize() - 1;
         Integer start = covariance.GetSubMatrixLocationStart("FrequencyPolynomialCoefficients");
         covariance(start + index, start + index) = value * value;
      }
      else
         throw HardwareException("Error: index is out of bound when setting value to " + GetName() + ".FrequencyPolynomialCoefficientNoiseSigmas parameter.");

      return freqPolynomialCoeffSigmas[index];
      break;

   default:
      break;
   }

   return Transmitter::SetRealParameter(id, value, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param label The scriptable label for the property
 * @param index The index number of the property
 *
 * @return The property's value
 */
 //------------------------------------------------------------------------------
Real Oscillator::GetRealParameter(const std::string & label, const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param label The scriptable label for the property
 * @param value The new value for the property
 * @param index The index number of the property
 *
 * @return The property's value at the end of the call
 */
 //------------------------------------------------------------------------------
Real Oscillator::SetRealParameter(const std::string & label, const Real value,
   const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//---------------------------------------------------------------------------
//  const Rvector& GetRvectorParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Rvector parameter.
*
* @param <id> The integer ID for the parameter.
*
* @return The parameter's value.
*/
//------------------------------------------------------------------------------
const Rvector& Oscillator::GetRvectorParameter(const Integer id) const
{
   if (id == OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS)
      return freqPolynomialCoeffs;
   if (id == OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA)
      return freqPolynomialCoeffSigmas;

   return Transmitter::GetRvectorParameter(id);
}


//---------------------------------------------------------------------------
//  const Rvector& SetRvectorParameter(const Integer id, const Rvector& value)
//---------------------------------------------------------------------------
/**
* Set the value for an Rvector parameter.
*
* @param <id> The integer ID for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or
*         RVECTOR_PARAMETER_UNDEFINED if the parameter id is invalid or the
*         parameter type is not Rvector.
*/
//------------------------------------------------------------------------------
const Rvector& Oscillator::SetRvectorParameter(const Integer id,
   const Rvector& value)
{
   if (id == OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS)
   {
      freqPolynomialCoeffs.Resize(value.GetSize());
      freqPolynomialCoeffs = value;
      return freqPolynomialCoeffs;
   }
   if (id == OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA)
   {
      freqPolynomialCoeffSigmas.Resize(value.GetSize());
      freqPolynomialCoeffSigmas = value;

      return freqPolynomialCoeffSigmas;
   }

   return Transmitter::SetRvectorParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value of a string property.
 *
 * @param id The id for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
std::string Oscillator::GetStringParameter(const Integer id) const
{
   switch (id)
   {
      case OSC_INITIAL_EPOCH:
         return initialEpochStr;
      case OSC_INITIAL_EPOCH_TYPE:
         return initialEpochTypeStr;
      default:
         break;
   }

   return Transmitter::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string property.
 *
 * @param id The id for the property
 * @param value The new value for the property
 *
 * @return true if the setting success
 */
//------------------------------------------------------------------------------
bool Oscillator::SetStringParameter(const Integer id,
                     const std::string &value)
{
   switch (id)
   {
      case OSC_INITIAL_EPOCH:
         initialEpochStr = value;
         return true;

      case OSC_INITIAL_EPOCH_TYPE:
         try
         {
            std::string timeSystem;
            std::string timeFormat;
            TimeSystemConverter::Instance()->GetTimeSystemAndFormat(value, timeSystem, timeFormat);
         }
         catch (...)
         {
            throw HardwareException("Error: "+GetName()+"."+GetParameterText(OSC_INITIAL_EPOCH_TYPE)+" parameter has invalid value ("+value+").");
         }

         initialEpochTypeStr = value;
         return true;

      default:
         break;
   }

   return Transmitter::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string property.
 *
 * @param label The scriptable label for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
std::string Oscillator::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string property.
 *
 * @param label The scriptable label for the property
 * @param value The new value for the property
 *
 * @return true if the setting success.
 */
//------------------------------------------------------------------------------
bool Oscillator::SetStringParameter(const std::string &label,
                     const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


std::string Oscillator::GetStringParameter(const Integer id, const Integer index) const
{
   switch (id)
   {
   case OSC_SOLVEFORS:
      if ((index < 0) || (index >= solveforNames.size()))
         throw HardwareException("Error: index is out of bound when getting value of " + GetName() + ".SolveFors parameter.");

      return solveforNames[index];
      break;
   default:
      break;
   }

   return Transmitter::GetStringParameter(id, index);
}


bool Oscillator::SetStringParameter(const Integer id, const std::string &value, const Integer index)
{
   switch (id)
   {
   case OSC_SOLVEFORS:
      // if it is an empty list, then clear the solve-for list
      if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         solveforNames.clear();
         return true;
      }

      // If is not "{}", it is a string containing solve-for variable  
      // Check for valid identity
      if ((value != "FrequencyDrift")&&
          (value != "FrequencyPolynomialCoefficients"))
         throw HardwareException("Error: " + GetName() +
            ".SolveFors parameter has invalid value ('" + value + 
            "').\n Valid values are 'FrequencyDrift' and 'FrequencyPolynomialCoefficients'.");
      
      if (find(solveforNames.begin(), solveforNames.end(), value) == solveforNames.end())
         solveforNames.push_back(value);
      else
         throw HardwareException("Error: '" + value + "' set to " + GetName() + ".SolveFors parameter is replicated.\n");

      return true;
      break;

   default:
      break;
   }

   return Transmitter::SetStringParameter(id, value, index);
}


std::string Oscillator::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


bool Oscillator::SetStringParameter(const std::string &label, const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

const StringArray& Oscillator::GetStringArrayParameter(const Integer id) const
{
   if (id == OSC_SOLVEFORS)
      return solveforNames;

   return Transmitter::GetStringArrayParameter(id);
}

const StringArray& Oscillator::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));;
}


//-------------------------------------------------------------------------
// Integer Spacecraft::HasParameterCovariances(Integer parameterId)
//-------------------------------------------------------------------------
/**
* This function is used to verify whether a parameter (with ID specified by
* parameterId) having a covariance or not.
*
* @param parameterId      ID of a parameter
* @return                 size of covarian matrix associated with the parameter
*                         return -1 when the parameter has no covariance
*/
//-------------------------------------------------------------------------
Integer Oscillator::HasParameterCovariances(Integer parameterId)
{
   if (parameterId == OSC_FREQUENCY_DRIFT)
      return 1;
   if (parameterId == OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS)
      return freqPolynomialCoeffs.GetSize();

   return GmatBase::HasParameterCovariances(parameterId);
}


//------------------------------------------------------------------------------
// Rmatrix* GetParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
* Get covariance matrix for a given parameter.
*
* @param parameterId      ID of a parameter
* @return                 a pointer to covariance matrix associated with the parameter
*                         return NULL when the parameter has no covariance
*/
//-------------------------------------------------------------------------
Rmatrix* Oscillator::GetParameterCovariances(Integer parameterId)
{
   if (isInitialized)
      return covariance.GetCovariance(parameterId);
   else
      throw GmatBaseException("Error: cannot get '" + GetName() + "' Oscillator's covariance when it is not initialized.\n");

   return NULL;
}


//------------------------------------------------------------------------------
// bool IsEstimationParameterValid(const Integer item)
//------------------------------------------------------------------------------
/**
* This function is used to verify an estimation paramter is either valid or not
*
* @param item      Estimation parameter ID (Note that: it is defferent from object ParameterID)
*
* return           true if it is valid, false otherwise
*/
//------------------------------------------------------------------------------
bool Oscillator::IsEstimationParameterValid(const Integer item)
{
   bool retval = false;

   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;    // convert Estimation ID to object parameter ID

   switch (id)
   {
   case OSC_FREQUENCY_DRIFT:
      retval = true;
      break;
   case OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS:
      retval = true;
      break;

   // All other values call up the hierarchy
   default:
      retval = Transmitter::IsEstimationParameterValid(item);
   }

   return retval;
}




bool Oscillator::Initialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Oscillator::Initialize() <%p,%s> start\n", this, GetName().c_str());
#endif
   bool reval = false;
   if (Transmitter::Initialize())
   {
      // Verify setting value
      if (freqPolynomialCoeffs.GetSize() != freqPolynomialCoeffSigmas.GetSize())
         throw HardwareException("Error: Real vectors setting to "
            + GetName() + ".FrequencyPolynomialCoefficients and "
            + GetName() + ".FrequencyPolynomialCoefficientsNoiseSigma has a different size."
         );

      // If polynomial coefficients are not set, get the default value from Transmitter
      if (freqPolynomialCoeffs.GetSize() == 0)
      {
         // Get set frequency default value as set in Transmitter
         freqPolynomialCoeffs.Resize(1);
         freqPolynomialCoeffs.SetElement(0, frequency);
         freqPolynomialCoeffSigmas.Resize(1);
         freqPolynomialCoeffSigmas.SetElement(0, 1.0e35);
      }

      // Specify oscillator initial time in A1ModJulian
      std::string epSystem;
      std::string epFormat;
      try
      {
         TimeSystemConverter::Instance()->GetTimeSystemAndFormat(initialEpochTypeStr, epSystem, epFormat);
      }
      catch (...)
      {
         throw HardwareException("Error: "+GetName()+"."+GetParameterText(OSC_INITIAL_EPOCH_TYPE)
            +" has invalid value ('"+ initialEpochTypeStr+"').");
      }

      try
      {
         std::string toStr;
         TimeSystemConverter::Instance()->Convert(initialEpochTypeStr, NULL, initialEpochStr, "A1ModJulian", initialEpoch, toStr);
      }
      catch (...)
      {
         throw HardwareException("Error: " + GetName() + "." + GetParameterText(OSC_INITIAL_EPOCH)
            + " has invalid value ('" + initialEpochStr + "').");
      }

      // Specify transmit frequency
      Signal* signal = GetSignal();
      SetEpoch(initialEpoch);
   }

#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Oscillator::Initialize() <%p,%s> end\n", this, GetName().c_str());
#endif
   return reval;
}


//-------------------------------------------------------------------------------
// Real Oscillator::SetEpoch(GmatTime currentEpoch)
//-------------------------------------------------------------------------------
/**
* This function is use to set current epoch to Oscillator in order to specify
* transmit frequency at that epoch.
*
* @param epoch    the epoch which is used to specify transmit frequency
*             
*/
//-------------------------------------------------------------------------------
void Oscillator::SetEpoch(GmatTime epoch)
{
   currentEpoch = epoch;

   // Specify frequency at the current epoch
   if (freqPolynomialCoeffs.GetSize() == 0)
      throw HardwareException("Error: Oscillator's polynomial coefficients vector has size of zero.");

   if (frequencyModel == "Taylor's series")
   {
      frequency = GetFrequency();                 // unit: Mhz
      Signal * signal = GetSignal();
      signal->SetEpoch(currentEpoch);
      signal->SetValue(frequency);
   }
   else if (frequencyModel == "Fourier's series")
   {
      throw HardwareException("Error: Oscillator's Fourier's series frequency model is not implemented yet.");
   }
}


//------------------------------------------------------------------------------
// Integer GetEstimationParameterSize(const Integer item)
//------------------------------------------------------------------------------
Integer Oscillator::GetEstimationParameterSize(const Integer item)
{
   Integer retval = 0;

   Integer id = (item > ESTIMATION_TYPE_ALLOCATION ?
      item - type * ESTIMATION_TYPE_ALLOCATION : item);

#ifdef DEBUG_ESTIMATION
   MessageInterface::ShowMessage("Oscillator::GetEstimationParameterSize(%d)"
      " called; parameter ID is %d\n", item, id);
#endif

   switch (id)
   {
   case OSC_FREQUENCY_DRIFT:
      return 1;
      break;
   case OSC_FREQUENCY_POLYNOMIAL_COEFFICIENTS:
      return freqPolynomialCoeffs.GetSize();
      break;
   default:
      // All other values call up the hierarchy
      return Transmitter::GetEstimationParameterSize(id);
      break;
   }
}

//-----------------------------------------------------------------------
// Real Oscillator::GetFrequency()
//-----------------------------------------------------------------------
/**
* This function is used to get oscillator's frequency at the current time.
*
* @return    the frequency at the current time (unit: Mhz)
*/
//-----------------------------------------------------------------------
Real Oscillator::GetFrequency()
{
   // Equation 3.3.7 in TDRSS and BTRS Math Spec
   Real freq = 0.0;
   Real t_powk = 1.0;
   Real t = (currentEpoch - initialEpoch).GetMjd() + freqDrift;
   for (Integer k = 0; k < freqPolynomialCoeffs.GetSize(); ++k)
   {
      freq = freq + freqPolynomialCoeffs[k] * t_powk;
      t_powk = t_powk * t;
   }
   return freq;                 // unit: Mhz
}


//-----------------------------------------------------------------------
// Real Oscillator::GetFrequency(const GmatTime atTime)
//-----------------------------------------------------------------------
/**
* This function is used to get oscillator's frequency at a given time.
*
* @param atTime   time at which it gets frequency
* @return         the frequency at the current time (unit: Mhz)
*/
//-----------------------------------------------------------------------
Real Oscillator::GetFrequency(const GmatTime atTime)
{
   // Equation 3.3.7 in TDRSS and BTRS Math Spec 
   Real freq = 0.0;
   Real t_powk = 1.0;
   Real t = (atTime - initialEpoch).GetMjd() + freqDrift;
   for (Integer k = 0; k < freqPolynomialCoeffs.GetSize(); ++k)
   {
      freq = freq + freqPolynomialCoeffs[k] * t_powk;
      t_powk = t_powk * t;
   }
   return freq;                 // unit: Mhz
}


//--------------------------------------------------------------------------
// Real Oscillator::GetFrequencyDriftDerivative()
//--------------------------------------------------------------------------
/**
* This function is used to calculate derivative of oscillator's frequency at
* current time w.r.t. frequency drift (unit: Hz/s).
*
* @return     derivative of oscillator's frequency at the current time 
*             w.r.t. frequency drift (unit: Hz/s)
*/
//--------------------------------------------------------------------------
Real Oscillator::GetFrequencyDriftDerivative()
{
   // Sigma term in Equation 3.9.2 TDRSS and BTRS Math Spec
   Real deriv = 0.0;
   Real t_power_kminus1 = 1.0;
   Real t = (currentEpoch - initialEpoch).GetMjd() + freqDrift;
   for (Integer k = 1; k < freqPolynomialCoeffs.GetSize(); ++k)
   {
      deriv = deriv + k * freqPolynomialCoeffs[k] * t_power_kminus1;
      t_power_kminus1 = t_power_kminus1 * t;
   }

   deriv = deriv * 1.0e6;
   //MessageInterface::ShowMessage("derivative of oscillator frequency w.r.t. frequency drift = %.15le Mhz/s  at time = %.15lf\n", deriv, currentEpoch.GetMjd());
   return deriv;                 // convert unit from Mhz/s to hz/s
}


//--------------------------------------------------------------------------
// Real Oscillator::GetFrequencyDriftDerivative(const GmatTime atTime)
//--------------------------------------------------------------------------
/**
* This function is used to calculate derivative of oscillator's frequency at 
* a given time w.r.t. frequency drift (unit: Hz/s).
*
* @param atTime    the given time at which it needs to take derivative
*
* @return          derivative of oscillator's frequency at given time
*                  w.r.t. frequency drift (unit: Hz/s)
*/
//--------------------------------------------------------------------------
Real Oscillator::GetFrequencyDriftDerivative(const GmatTime atTime)
{
   // Sigma term in Equation 3.9.2 TDRSS and BTRS Math Spec
   Real deriv = 0.0;
   Real t_power_kminus1 = 1.0;
   Real t = (atTime - initialEpoch).GetMjd() + freqDrift;
   for (Integer k = 1; k < freqPolynomialCoeffs.GetSize(); ++k)
   {
      deriv = deriv + k * freqPolynomialCoeffs[k] * t_power_kminus1;
      t_power_kminus1 = t_power_kminus1 * t;
   }
   //MessageInterface::ShowMessage("derivative of oscillator frequency w.r.t. frequency drift = %.15le Mhz/s  at time = %.15lf\n", deriv, atTime.GetMjd());
   return deriv*1.0e6;                 // convert unit from Mhz/s to hz/s
}


//-----------------------------------------------------------------------------------
// const RealArray& GetFrequencyPolynomialCoefficientsDerivative()
//-----------------------------------------------------------------------------------
/**
* This function is used to calculate derivative of oscillator's frequency at current
* time w.r.t. frequency polynomial coefficients (vector unit: [1  s  s^2  s^3 ...]).
*
* @return         derivative of oscillator's frequency w.r.t. frequency polynomial
*                 coefficients (vector unit: [1  s  s^2  s^3 ...])
*/
//-----------------------------------------------------------------------------------
const RealArray& Oscillator::GetFrequencyPolynomialCoefficientsDerivative() const
{
   static RealArray deriv;
   deriv.clear();

   Real t_powk = 1.0;
   Real t = (currentEpoch - initialEpoch).GetMjd() + freqDrift;
   //MessageInterface::ShowMessage("t = %.15le\n", t);
   for (Integer k = 0; k < freqPolynomialCoeffs.GetSize(); ++k)
   {
      deriv.push_back(t_powk);
      t_powk = t_powk * t;
   }
   // Unit of dFosc/dCk is MHz/(unit of Ck)

   // Now convert MHz to Hz. It becomes Hz/(unit of Ck)
   //for (Integer i = 0; i < deriv.size(); ++i)
      //deriv[i] *= 1.0e6;

   return deriv;
}


//-----------------------------------------------------------------------------------
// const RealArray& GetFrequencyPolynomialCoefficientsDerivative(const GmatTime atTime)
//-----------------------------------------------------------------------------------
/**
* This function is used to calculate derivative of oscillator's frequency at a give
* time w.r.t. frequency polynomial coefficients (vector unit: [1  s  s^2  s^3 ...]).
*
* @param atTime   the time at which it needs to take derivative
*
* @return         derivative of oscillator's frequency w.r.t. frequency polynomial 
*                 coefficients (vector unit: [1  s  s^2  s^3 ...])
*/
//-----------------------------------------------------------------------------------
const RealArray& Oscillator::GetFrequencyPolynomialCoefficientsDerivative(const GmatTime atTime) const 
{
   static RealArray deriv;
   deriv.clear();

   Real t_powk = 1.0;
   Real t = (atTime - initialEpoch).GetMjd() + freqDrift;
   //MessageInterface::ShowMessage("t = %.15le\n", t);
   for (Integer k = 0; k < freqPolynomialCoeffs.GetSize(); ++k)
   {
      deriv.push_back(t_powk);
      t_powk = t_powk * t;
   }
   // Unit of dFosc/dCk is MHz/(unit of Ck)

   // Now convert MHz to Hz. It becomes Hz/(unit of Ck)
   for (Integer i = 0; i < deriv.size(); ++i)
      deriv[i] *= 1.0e6;

   return deriv;
}
