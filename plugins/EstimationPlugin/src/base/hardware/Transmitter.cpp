//$Id: Transmitter.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             Transmitter
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
// Author: Darrel J. Conway
// Created: 2010/02/22 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the Transmitter class
 */
//------------------------------------------------------------------------------

#include "Transmitter.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "TimeSystemConverter.hpp"
#include <algorithm>

//#define DEBUG_SET_REAL_PARA

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Text strings used to script Transmitter properties
const std::string
Transmitter::PARAMETER_TEXT[TransmitterParamCount - RFHardwareParamCount] =
{
   "FrequencyModel",
   "Frequency",
   "FrequencyBand",
   "FrequencyBias",
   "FrequencyBiasSigma",
   "ReferenceEpoch",
   "EpochFormat",
   "SolveFors",
};

/// Integer IDs associated with the Transmitter properties
const Gmat::ParameterType
Transmitter::PARAMETER_TYPE[TransmitterParamCount - RFHardwareParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
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
// Transmitter(const std::string &ofType, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name for the transmitter
 */
//------------------------------------------------------------------------------
Transmitter::Transmitter(const std::string &ofType, const std::string &name) :
   RFHardware     (ofType, name),
   frequencyModel ("constant"),
   frequency      (2000.0),                            // unit: MHz
   frequencyBand  ("None"),
   initialEpochStr("FromSpacecraft"),
   initialEpochTypeStr("TAIModJulian")
{
   objectTypeNames.push_back("Transmitter");
   parameterCount = TransmitterParamCount;

   isTransmitted1 = true;
   signal1 = new Signal();

   for (Integer i = RFHardwareParamCount; i < TransmitterParamCount; ++i)
      parameterWriteOrder.push_back(i);

   supportedFrequencyBands = { "None", "S", "C", "X", "K" };


   freqPolynomialCoeffs.Set(1, 0.0);
   freqPolynomialCoeffSigmas.Set(1, 1.0e70);
   covariance.AddCovarianceElement("FrequencyBias", this);
   Integer start = covariance.GetSubMatrixLocationStart("FrequencyBias");
   for (Integer i = 0; i < freqPolynomialCoeffs.GetSize(); ++i)
      covariance(start + i, start + i) = freqPolynomialCoeffSigmas[i] * freqPolynomialCoeffSigmas[i];
   
}

//------------------------------------------------------------------------------
// ~Transmitter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Transmitter::~Transmitter()
{
   if (signal1 != NULL)
      delete signal1;
}


//------------------------------------------------------------------------------
// Transmitter(const Transmitter & trans)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param trans The Transmitter that provides configuration data for this new
 *              one.
 */
//------------------------------------------------------------------------------
Transmitter::Transmitter(const Transmitter & trans) :
   RFHardware                 (trans),
   frequencyModel             (trans.frequencyModel),
   frequency                  (trans.frequency),
   frequencyBand              (trans.frequencyBand),
   freqPolynomialCoeffs       (trans.freqPolynomialCoeffs),
   freqPolynomialCoeffSigmas  (trans.freqPolynomialCoeffSigmas),
   initialEpochStr            (trans.initialEpochStr),
   initialEpochTypeStr        (trans.initialEpochTypeStr),
   solveforNames              (trans.solveforNames)
{
   signal1->SetValue(frequency);
   std::string toStr;
   if(initialEpochStr != "FromSpacecraft")
      TimeSystemConverter::Instance()->Convert(initialEpochTypeStr, -999.999, initialEpochStr, "A1ModJulian", initialEpoch, toStr);
   
   for (Integer i = RFHardwareParamCount; i < TransmitterParamCount; ++i)
      parameterWriteOrder.push_back(i);
   covariance.AddCovarianceElement("FrequencyBias", this);
   Integer start = covariance.GetSubMatrixLocationStart("FrequencyBias");
   for (Integer i = 0; i < freqPolynomialCoeffs.GetSize(); ++i)
      covariance(start + i, start + i) = freqPolynomialCoeffSigmas[i] * freqPolynomialCoeffSigmas[i];
}


//------------------------------------------------------------------------------
// Transmitter& operator =(const Transmitter & trans)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param trans The Transmitter that provides configuration data for this one.
 *
 * @return This Transmitter, set to match trans.
 */
//------------------------------------------------------------------------------
Transmitter& Transmitter::operator =(const Transmitter & trans)
{
   if (this != &trans)
   {
      frequencyModel             = trans.frequencyModel;
      frequency                  = trans.frequency;
      frequencyBand              = trans.frequencyBand;
      freqPolynomialCoeffs       = trans.freqPolynomialCoeffs;
      freqPolynomialCoeffSigmas  = trans.freqPolynomialCoeffSigmas;
      initialEpochStr            = trans.initialEpochStr;
      initialEpochTypeStr        = trans.initialEpochTypeStr;
      solveforNames              = trans.solveforNames;
      RFHardware::operator=(trans);
      
      std::string toStr;
      if (initialEpochStr != "FromSpacecraft")
         TimeSystemConverter::Instance()->Convert(initialEpochTypeStr, -999.999, initialEpochStr, "A1ModJulian", initialEpoch, toStr);
      covariance.AddCovarianceElement("FrequencyBias", this);
      Integer start = covariance.GetSubMatrixLocationStart("FrequencyBias");
      for (Integer i = 0; i < freqPolynomialCoeffs.GetSize(); ++i)
         covariance(start + i, start + i) = freqPolynomialCoeffSigmas[i] * freqPolynomialCoeffSigmas[i];
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this transmitter
 *
 * @return A new Transmitter configured like this one.
 */
//------------------------------------------------------------------------------
GmatBase *Transmitter::Clone() const
{
   return new Transmitter(*this);
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
void Transmitter::Copy(const GmatBase* orig)
{
   operator=(*((Transmitter *)(orig)));
}



//-----------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//-----------------------------------------------------------------------------
/**
 * Method used to find the integer ID associated with the script string for an
 * Transmitter object property.
 *
 * @param str The script string
 *
 * @return The integer ID associated with the string
 */
//-----------------------------------------------------------------------------
Integer Transmitter::GetParameterID(const std::string & str) const
{
   for (Integer i = RFHardwareParamCount; i < TransmitterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - RFHardwareParamCount])
      {
         if (IsParameterReadOnly(i))
            throw HardwareException("Error: Parameter '" + str + "' was not defined in GMAT Transmitter's syntax.\n");

         return i;
      }
   }

   return RFHardware::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Finds the script string associated with a Transmitter object's property,
 * given the integer ID for that property.
 *
 * @param id The property's ID.
 *
 * @return The string associated with the property.
 */
//------------------------------------------------------------------------------
std::string Transmitter::GetParameterText(const Integer id) const
{
   if (id >= RFHardwareParamCount && id < TransmitterParamCount)
      return PARAMETER_TEXT[id - RFHardwareParamCount];
   return RFHardware::GetParameterText(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the Transmitter property associated
 * with the input ID.
 *
 * @param id The property's ID
 *
 * @return A text description of the property's type.
 */
//------------------------------------------------------------------------------
std::string Transmitter::GetParameterTypeString(const Integer id) const
{
   return RFHardware::PARAM_TYPE_STRING[GetParameterType(id)];
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
Gmat::ParameterType Transmitter::GetParameterType(const Integer id) const
{
   if (id >= RFHardwareParamCount && id < TransmitterParamCount)
      return PARAMETER_TYPE[id - RFHardwareParamCount];

   return RFHardware::GetParameterType(id);
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
 * data for the Transmitter properties will need to be specified.
 */
//------------------------------------------------------------------------------
std::string Transmitter::GetParameterUnit(const Integer id) const
{
   switch (id)
   {
      case FREQUENCY_MODEL:
         return "";
      case FREQUENCY:
         return "MHz";               // Unit of frequency is MHz
      case FREQUENCY_BAND:
         return "";
      case FREQUENCY_POLYNOMIAL_COEFFICIENTS:
         return "";
      case FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA:
         return "";
      case TRANS_INITIAL_EPOCH:
         return "";
      case TRANS_INITIAL_EPOCH_TYPE:
         return "";
      default:
         break;
   }

   return RFHardware::GetParameterUnit(id);
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
bool Transmitter::IsParameterReadOnly(const std::string& label) const
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
bool Transmitter::IsParameterReadOnly(const Integer id) const
{
   if (id == FREQUENCY_MODEL)
      return true;

   return RFHardware::IsParameterReadOnly(id);
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
Real Transmitter::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case FREQUENCY:
         return frequency;
      default:
         break;
   }

   return RFHardware::GetRealParameter(id);
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
Real Transmitter::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_SET_REAL_PARA
      MessageInterface::ShowMessage("Transmitter::SetRealParameter(id = %d, value = %le)  name of transmiiter = '%s'\n",id, value, GetName().c_str());
   #endif

   switch (id)
   {
      case FREQUENCY:
         if (value >= 0.0)
            frequency = value;
         else
            throw HardwareException("Error: frequency set to " + GetName() + ".Frequency is a negative number.\n");

         return frequency;

      //case HARDWARE_DELAY:
      //   MessageInterface::ShowMessage("Warning: the script to assign %lf to '%s.%s' parameter was skipped. In the current GMAT version, this parameter is not used.\n", value, GetName().c_str(), GetParameterText(id).c_str());
      //   return 0.0;

      default:
         break;
   }

   return RFHardware::SetRealParameter(id, value);
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
Real Transmitter::GetRealParameter(const std::string & label) const
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
Real Transmitter::SetRealParameter(const std::string & label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param id The integer ID of the property
 * @param index The index number of the property
 *
 * @return The property's value
 */
 //------------------------------------------------------------------------------
Real Transmitter::GetRealParameter(const Integer id, const Integer index) const
{
   switch (id)
   {
      case FREQUENCY_POLYNOMIAL_COEFFICIENTS:
         if ((index < 0) || (index >= freqPolynomialCoeffs.GetSize()))
            throw HardwareException("Error: index is out of bound when getting value of " + GetName() + ".FrequencyBias parameter.");

         return freqPolynomialCoeffs[index];
         break;

      case FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA:
         if ((index < 0) || (index >= freqPolynomialCoeffSigmas.GetSize()))
            throw HardwareException("Error: index is out of bound when getting value of " + GetName() + ".FrequencyBiasSigma parameter.");

         return freqPolynomialCoeffs[index];
         break;

      default:
         break;
   }
   return RFHardware::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param id The integer ID of the property
 * @param value The new value for the property
 * @param index The index number of the property
 *
 * @return The property's value at the end of the call
 */
 //------------------------------------------------------------------------------
Real Transmitter::SetRealParameter(const Integer id, const Real value, const Integer index)
{

   switch (id)
   {
      case FREQUENCY_POLYNOMIAL_COEFFICIENTS:
         if ((0 <= index) && (index < freqPolynomialCoeffs.GetSize()))
            freqPolynomialCoeffs[index] = value;
         else if (index == freqPolynomialCoeffs.GetSize())
         {
            Rvector temp = freqPolynomialCoeffs;
            freqPolynomialCoeffs.Resize(temp.GetSize() + 1);
            for (Integer i = 0; i < temp.GetSize(); ++i)
               freqPolynomialCoeffs.SetElement(i, temp[i]);
            freqPolynomialCoeffs.SetElement(temp.GetSize(), value);
            if (freqPolynomialCoeffs.GetSize() > freqPolynomialCoeffSigmas.GetSize())
            {
               temp = freqPolynomialCoeffSigmas;
               freqPolynomialCoeffSigmas.Resize(freqPolynomialCoeffs.GetSize());
               for (Integer i = 0; i < temp.GetSize(); ++i)
                  freqPolynomialCoeffSigmas.SetElement(i, temp[i]);
               for (Integer i = temp.GetSize(); i < freqPolynomialCoeffs.GetSize(); ++i)
                  freqPolynomialCoeffSigmas.SetElement(i, 1.0e70);
               
               
               Integer elementSize = covariance.GetElementSize("FrequencyBias");
               if (elementSize < freqPolynomialCoeffSigmas.GetSize())
               {
                  // Increasing its size
                  Integer submatrixSize = covariance.GetElementSize("FrequencyBias");
                  covariance.IncreasingElementSize("FrequencyBias", submatrixSize, 1);
               }
               Integer start = covariance.GetSubMatrixLocationStart("FrequencyBias");
               for (Integer i = temp.GetSize(); i < freqPolynomialCoeffs.GetSize(); ++i)
                  covariance(start + i, start + i) = 1.0e70 * 1.0e70;
            }
         }
         else
            throw HardwareException("Error: index is out of bound when setting value to " + GetName() + ".FrequencyBias parameter.");

         return freqPolynomialCoeffs[index];
         break;

      case FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA:
         if ((0 <= index) && (index < freqPolynomialCoeffSigmas.GetSize()))
         {
            freqPolynomialCoeffSigmas[index] = value;
            Integer start = covariance.GetSubMatrixLocationStart("FrequencyBias");
            covariance(start + index, start + index) = value * value;
         }
         else if (index == freqPolynomialCoeffSigmas.GetSize())
         {
            Rvector temp = freqPolynomialCoeffSigmas;
            freqPolynomialCoeffSigmas.Resize(temp.GetSize() + 1);
            for (Integer i = 0; i < temp.GetSize(); ++i)
               freqPolynomialCoeffSigmas.SetElement(i, temp[i]);
            freqPolynomialCoeffSigmas.SetElement(temp.GetSize(), value);

            Integer elementSize = covariance.GetElementSize("FrequencyBias");
            if (elementSize < freqPolynomialCoeffSigmas.GetSize())
            {
               // Increasing its size
               Integer submatrixSize = covariance.GetElementSize("FrequencyBias");
               covariance.IncreasingElementSize("FrequencyBias", submatrixSize, 1);
            }
            Integer index = freqPolynomialCoeffSigmas.GetSize() - 1;
            Integer start = covariance.GetSubMatrixLocationStart("FrequencyBias");
            covariance(start + index, start + index) = value * value;
         }
         else
            throw HardwareException("Error: index is out of bound when setting value to " + GetName() + ".FrequencyBiasSigma parameter.");

         return freqPolynomialCoeffSigmas[index];
         break;

      default:
         break;
   }
   return RFHardware::SetRealParameter(id, value, index);
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
Real Transmitter::GetRealParameter(const std::string & label, const Integer index) const
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
Real Transmitter::SetRealParameter(const std::string & label, const Real value,
   const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
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
std::string Transmitter::GetStringParameter(const Integer id) const
{
   switch (id)
   {
      case FREQUENCY_MODEL:
         return frequencyModel;
      case FREQUENCY_BAND:
         return frequencyBand;
      case TRANS_INITIAL_EPOCH:
         return initialEpochStr;
      case TRANS_INITIAL_EPOCH_TYPE:
         return initialEpochTypeStr;
      default:
         break;
   }

   return RFHardware::GetStringParameter(id);
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
bool Transmitter::SetStringParameter(const Integer id,
                     const std::string &value)
{
   switch (id)
   {
      case FREQUENCY_MODEL:
//       frequencyModel = value;
         MessageInterface::ShowMessage("Warning: the script to assign '%s' to '%s.%s' parameter was skipped. In the current GMAT version, this parameter is not used.\n", value.c_str(), GetName().c_str(), GetParameterText(id).c_str());
         return true;
      case FREQUENCY_BAND:
         if (std::count(supportedFrequencyBands.begin(), supportedFrequencyBands.end(), value)) 
         {
            frequencyBand = value;
            return true;
         }
         else
         {
            std::string strBands = supportedFrequencyBands[0];
            for (int i = 1; i < supportedFrequencyBands.size(); i++)
               strBands += ", " + supportedFrequencyBands[i];
            throw HardwareException("Error: frequency band set to " + GetName() + " is invalid. Currently supported bands are: " + strBands +  "\n");
         }
         break;
      case TRANS_INITIAL_EPOCH:
         initialEpochStr = value;
         return true;
         break;
      case TRANS_INITIAL_EPOCH_TYPE:
         try
         {
            std::string timeSystem;
            std::string timeFormat;
            TimeSystemConverter::Instance()->GetTimeSystemAndFormat(value, timeSystem, timeFormat);
         }
         catch (...)
         {
            throw HardwareException("Error: " + GetName() + "." + GetParameterText(TRANS_INITIAL_EPOCH_TYPE) + " parameter has invalid value (" + value + ").");
         }

         initialEpochTypeStr = value;
         return true;
         break;
      default:
         break;
   }

   return RFHardware::SetStringParameter(id, value);
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
std::string Transmitter::GetStringParameter(const std::string &label) const
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
bool Transmitter::SetStringParameter(const std::string &label,
                     const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string property at a given index in an array
 *
 * @param id The parameter ID
 * @param index The index in the array
 *
 * @return The property's value
 */
 //------------------------------------------------------------------------------
std::string Transmitter::GetStringParameter(const Integer id, const Integer index) const
{

   switch (id)
   {
      case TRANS_SOLVEFORS:
         if ((index < 0) || (index >= solveforNames.size()))
            throw HardwareException("Error: index is out of bound when getting value of " + GetName() + ".SolveFors parameter.");

         return solveforNames[index];
         break;
      default:
         break;
   }
   return RFHardware::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// std::string SetStringParameter(const Integer id, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string property at a given index in an array
 *
 * @param id The parameter ID
 * @param &value A reference to the value that is to be set
 * @param index The index in the array
 *
 * @return The property's value
 */
 //------------------------------------------------------------------------------
bool Transmitter::SetStringParameter(const Integer id, const std::string &value, const Integer index)
{

   switch (id)
   {
      case TRANS_SOLVEFORS:
         // if it is an empty list, then clear the solve-for list
         if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
         {
            solveforNames.clear();
            return true;
         }

         // If is not "{}", it is a string containing solve-for variable  
         // Check for valid identity
         if (value != "FrequencyBias")
            throw HardwareException("Error: " + GetName() +
               ".SolveFors parameter has invalid value ('" + value +
               "').\n Valid value is 'FrequencyBias'.");

         if (find(solveforNames.begin(), solveforNames.end(), value) == solveforNames.end())
            solveforNames.push_back(value);
         else
            throw HardwareException("Error: '" + value + "' set to " + GetName() + ".SolveFors parameter is replicated.\n");

         return true;
         break;

      default:
         break;
   }
   return RFHardware::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string property from a given index in an array
 *
 * @param label The scriptable label for the property
 * @param index The index in the array
 *
 * @return The property's value
 */
 //------------------------------------------------------------------------------
std::string Transmitter::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// std::string SetStringParameter(const std::string &label, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string property at a given index in an array
 *
 * @param label The scriptable label for the property
 * @param &value A reference to the value that is to be set
 * @param index The index in the array
 *
 * @return The property's value
 */
 //------------------------------------------------------------------------------
bool Transmitter::SetStringParameter(const std::string &label, const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string array
 *
 * @param id The parameter ID
 *
 * @return The property's value
 */
 //------------------------------------------------------------------------------
const StringArray& Transmitter::GetStringArrayParameter(const Integer id) const
{
   if (id == TRANS_SOLVEFORS)
      return solveforNames;

   return Transmitter::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string array
 *
 * @param label The scriptable label for the property
 *
 * @return The property's value
 */
 //------------------------------------------------------------------------------
const StringArray& Transmitter::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));;
}

//---------------------------------------------------------------------------
//  const Rvector& GetRvectorParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Rvector parameter.
*
* @param label The name a of the parameter
*
* @return The parameter's value.
*/
//------------------------------------------------------------------------------
const Rvector& Transmitter::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
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
const Rvector& Transmitter::GetRvectorParameter(const Integer id) const
{
   if (id == FREQUENCY_POLYNOMIAL_COEFFICIENTS)
      return freqPolynomialCoeffs;
   if (id == FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA)
      return freqPolynomialCoeffSigmas;

   return RFHardware::GetRvectorParameter(id);
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
const Rvector& Transmitter::SetRvectorParameter(const Integer id,
   const Rvector& value)
{
   if (id == FREQUENCY_POLYNOMIAL_COEFFICIENTS)
   {
      freqPolynomialCoeffs.Resize(value.GetSize());
      freqPolynomialCoeffs = value;
      return freqPolynomialCoeffs;
   }
   if (id == FREQUENCY_POLYNOMIAL_COEFFICIENTS_NOISESIGMA)
   {
      freqPolynomialCoeffSigmas.Resize(value.GetSize());
      freqPolynomialCoeffSigmas = value;

      return freqPolynomialCoeffSigmas;
   }

   return RFHardware::SetRvectorParameter(id, value);
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
Rmatrix* Transmitter::GetParameterCovariances(Integer parameterId)
{
   if (isInitialized)
      return covariance.GetCovariance(parameterId);
   else
      throw GmatBaseException("Error: cannot get '" + GetName() + "' Transmitter's covariance when it is not initialized.\n");

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
bool Transmitter::IsEstimationParameterValid(const Integer item)
{
   bool retval = false;

   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;    // convert Estimation ID to object parameter ID

   switch (id)
   {
   case FREQUENCY_POLYNOMIAL_COEFFICIENTS:
      retval = true;
      break;

      // All other values call up the hierarchy
   default:
      retval = RFHardware::IsEstimationParameterValid(item);
   }

   return retval;
}

bool Transmitter::Initialize()
{
   bool reval = false;
   if (RFHardware::Initialize())
   {
      // Verify setting value
      if (freqPolynomialCoeffs.GetSize() > freqPolynomialCoeffSigmas.GetSize())
      {
         Rvector temp = freqPolynomialCoeffSigmas;
         freqPolynomialCoeffSigmas.Resize(freqPolynomialCoeffs.GetSize());
         for (Integer i = 0; i < temp.GetSize(); ++i)
            freqPolynomialCoeffSigmas.SetElement(i, temp[i]);
         for (Integer i = temp.GetSize(); i < freqPolynomialCoeffs.GetSize(); ++i)
            freqPolynomialCoeffSigmas.SetElement(i, 1.0e70);
         
         Integer elementSize = covariance.GetElementSize("FrequencyBias");
         if (elementSize < freqPolynomialCoeffSigmas.GetSize())
         {
            // Increasing its size
            Integer submatrixSize = covariance.GetElementSize("FrequencyBias");
            covariance.IncreasingElementSize("FrequencyBias", submatrixSize, 1);
         }
         Integer start = covariance.GetSubMatrixLocationStart("FrequencyBias");
         for (Integer i = temp.GetSize(); i < freqPolynomialCoeffs.GetSize(); ++i)
            covariance(start + i, start + i) = 1.0e70 * 1.0e70;
      }
      if (freqPolynomialCoeffs.GetSize() != freqPolynomialCoeffSigmas.GetSize())
         throw HardwareException("Error: Real vectors setting to "
            + GetName() + ".FrequencyBias and "
            + GetName() + ".FrequencyBiasSigma has a different size."
            );
      
     
      // Specify Transmitter initial time in A1ModJulian
      std::string epSystem;
      std::string epFormat;
      try
      {
         TimeSystemConverter::Instance()->GetTimeSystemAndFormat(initialEpochTypeStr, epSystem, epFormat);
      }
      catch (...)
      {
         throw HardwareException("Error: " + GetName() + "." + GetParameterText(TRANS_INITIAL_EPOCH_TYPE)
            + " has invalid value ('" + initialEpochTypeStr + "').");
      }


      try
      {
         std::string toStr;
         if (initialEpochStr != "FromSpacecraft")
            TimeSystemConverter::Instance()->Convert(initialEpochTypeStr, -999.999, initialEpochStr, "A1ModJulian", initialEpoch, toStr);
      }
      catch (...)
      {
         throw HardwareException("Error: " + GetName() + "." + GetParameterText(TRANS_INITIAL_EPOCH)
            + " has invalid value ('" + initialEpochStr + "').");
      }

         // Specify transmit frequency
      SetEpoch(initialEpoch);
      signal1->SetValue(frequency);
      reval = true;
   }

   return reval;
}

//-------------------------------------------------------------------------------
// Real Transmitter::SetEpoch(GmatTime currentEpoch)
//-------------------------------------------------------------------------------
/**
* This function is use to set current epoch to Transmitter in order to specify
* transmit frequency at that epoch.
*
* @param epoch    the epoch which is used to specify transmit frequency
*
*/
//-------------------------------------------------------------------------------
void Transmitter::SetEpoch(GmatTime epoch)
{
   currentEpoch = epoch;

   // Specify frequency at the current epoch
   if (freqPolynomialCoeffs.GetSize() == 0)
      throw HardwareException("Error: Transmitter's polynomial coefficients vector has size of zero.");

   if (frequencyModel == "Taylor series")
   {
      frequency = GetFrequency();                 // unit: Mhz
      Signal * signal = GetSignal();
      signal->SetEpoch(currentEpoch);
      signal->SetValue(frequency);
   }
   else if (frequencyModel == "Fourier series")
   {
      throw HardwareException("Error: Transmitter's Fourier's series frequency model is not implemented yet.");
   }
}

//-------------------------------------------------------------------------
// Integer Transmitter::HasParameterCovariances(Integer parameterId)
//-------------------------------------------------------------------------
/**
* This function is used to verify whether a parameter (with ID specified by
* parameterId) having a covariance or not.
*
* @param parameterId      ID of a parameter
* @return                 size of covariance matrix associated with the parameter
*                         return -1 when the parameter has no covariance
*/
//-------------------------------------------------------------------------
Integer Transmitter::HasParameterCovariances(Integer parameterId)
{
   if (parameterId == FREQUENCY_POLYNOMIAL_COEFFICIENTS)
      return freqPolynomialCoeffs.GetSize();

   return GmatBase::HasParameterCovariances(parameterId);
}

//------------------------------------------------------------------------------
// Integer GetEstimationParameterSize(const Integer item)
//------------------------------------------------------------------------------
/**
* This function returns the size of a given estimated parameter
*
* @param item             ID of a parameter
* @return                 Integer length of the array
*/
//-------------------------------------------------------------------------
Integer Transmitter::GetEstimationParameterSize(const Integer item)
{
   Integer retval = 0;

   Integer id = (item > ESTIMATION_TYPE_ALLOCATION ?
      item - type * ESTIMATION_TYPE_ALLOCATION : item);

#ifdef DEBUG_ESTIMATION
   MessageInterface::ShowMessage("Transmitter::GetEstimationParameterSize(%d)"
      " called; parameter ID is %d\n", item, id);
#endif

   switch (id)
   {
      case FREQUENCY_POLYNOMIAL_COEFFICIENTS:
         return freqPolynomialCoeffs.GetSize();
         break;
      default:
         // All other values call up the hierarchy
         return RFHardware::GetEstimationParameterSize(id);
         break;
   }
}

//-----------------------------------------------------------------------
// Real Transmitter::GetFrequency()
//-----------------------------------------------------------------------
/**
* This function is used to get Transmitter's frequency at the current time.
*
* @return    the frequency at the current time (unit: Mhz)
*/
//-----------------------------------------------------------------------
Real Transmitter::GetFrequency()
{
   // Equation 3.3.7 in TDRSS and BTRS Math Spec
   return frequency + GetFrequencyOscillatorBias();                 // unit: Mhz
}




//-----------------------------------------------------------------------
// Real Transmitter::GetFrequency(const GmatTime atTime)
//-----------------------------------------------------------------------
/**
* This function is used to get Transmitter's frequency at a given time.
*
* @param atTime   time at which it gets frequency
* @return         the frequency at the current time (unit: Mhz)
*/
//-----------------------------------------------------------------------
Real Transmitter::GetFrequency(const GmatTime atTime)
{
   // Equation 3.3.7 in TDRSS and BTRS Math Spec 
   Real freq = frequency;
   return freq + GetFrequencyOscillatorBias(atTime);                 // unit: Mhz
}

//-----------------------------------------------------------------------
// Real Transmitter::GetFrequencyOscillatorBias()
//-----------------------------------------------------------------------
/**
* This function is used to get Transmitter's frequency at the current time.
*
* @return    the frequency at the current time (unit: Mhz)
*/
//-----------------------------------------------------------------------
Real Transmitter::GetFrequencyOscillatorBias()
{
   // Equation 3.3.7 in TDRSS and BTRS Math Spec
   Real freq = 0.0;
   Real t_powk = 1.0;
   Real t = (currentEpoch - initialEpoch).GetMjd() * 86400;
   for (Integer k = 0; k < freqPolynomialCoeffs.GetSize(); ++k)
   {
      freq = freq + freqPolynomialCoeffs[k] * t_powk;
      t_powk = t_powk * t;
   }
   return freq;                 // unit: Mhz
}




//-----------------------------------------------------------------------
// Real Transmitter::GetFrequency(const GmatTime atTime)
//-----------------------------------------------------------------------
/**
* This function is used to get Transmitter's frequency at a given time.
*
* @param atTime   time at which it gets frequency
* @return         the frequency at the current time (unit: Mhz)
*/
//-----------------------------------------------------------------------
Real Transmitter::GetFrequencyOscillatorBias(const GmatTime atTime)
{
   // Equation 3.3.7 in TDRSS and BTRS Math Spec 
   Real freq = 0.0;
   Real t_powk = 1.0;
   Real t = (atTime - initialEpoch).GetMjd() * 86400;
   for (Integer k = 0; k < freqPolynomialCoeffs.GetSize(); ++k)
   {
      freq = freq + freqPolynomialCoeffs[k] * t_powk;
      t_powk = t_powk * t;
   }
   return freq;                 // unit: Mhz
}

//------------------------------------------------------------------------------
// Real GetOutPutFrequency()
//------------------------------------------------------------------------------
/**
 * Get the output frequency of Transmitter.
 *
 * @return the output frequency.
 */
//------------------------------------------------------------------------------
Real Transmitter::GetOutPutFrequency()
{
   // Write code to calculate output frequency here:
   Real outputFreq = frequency;       // for frequencyModel = "constant"

   return outputFreq;
}


//------------------------------------------------------------------------------
// Real GetDelay(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This function is used to get hardware delay.
 *
 * @return hardware delay
 */
//------------------------------------------------------------------------------
Real Transmitter::GetDelay(Integer whichOne)
{
   if (whichOne == 0)
      return RFHardware::GetDelay();
   else
      throw HardwareException("Delay index is out of bound\n");
}


//------------------------------------------------------------------------------
// bool SetDelay(Real delay, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This function is used to set hardware delay.
 *
 * @return true if it is successfully set, otherwise it throws an exception.
 */
//------------------------------------------------------------------------------
bool Transmitter::SetDelay(Real delay, Integer whichOne)
{
   switch(whichOne)
   {
      case 0:
          hardwareDelay1 = delay;
          return true;
     default:
          throw HardwareException("Delay index is out of bound\n");
   }
}



//------------------------------------------------------------------------------
// Integer GetSignalCount()
//------------------------------------------------------------------------------
/**
 * Get the number of signals.
 *
 * @return the number of signals. For a transmitter, number of signal is 1
 */
//------------------------------------------------------------------------------
Integer Transmitter::GetSignalCount()
{
   return 1;
}


//------------------------------------------------------------------------------
// bool IsTransmitted(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Verify a given signal having ability to transmit or not.
 *
 * @param whichOne    The index specifying a given signal.
 *
 * @return true for ability to transmit, false otherwise.
 */
//------------------------------------------------------------------------------
bool Transmitter::IsTransmitted(Integer whichOne)
{
   return this->isTransmitted1;
}


//------------------------------------------------------------------------------
// Signal* GetSignal(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Get a specified signal.
 *
 * @param whichOne    The index specifying a given signal.
 *
 * @return    a signal for a given index.
 */
//------------------------------------------------------------------------------
Signal* Transmitter::GetSignal(Integer whichOne)
{
   return RFHardware::GetSignal();
}


//------------------------------------------------------------------------------
// bool SetSignal(Signal* s, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Set a signal for a given index.
 *
 * @param s          The signal needed to set to
 * @param whichOne   The index specifying a given signal.
 *
 * @return    true if signal is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Transmitter::SetSignal(Signal* s,Integer whichOne)
{
   return RFHardware::SetSignal(s);
}

//-----------------------------------------------------------------------------------
// const RealArray& GetFrequencyBiasDerivative()
//-----------------------------------------------------------------------------------
/**
* This function is used to calculate derivative of Transmitter's frequency at current
* time w.r.t. frequency polynomial coefficients (vector unit: [1  s  s^2  s^3 ...]).
*
* @return         derivative of Transmitter's frequency w.r.t. frequency polynomial
*                 coefficients (vector unit: [1  s  s^2  s^3 ...])
*/
//-----------------------------------------------------------------------------------
const RealArray& Transmitter::GetFrequencyBiasDerivative() const
{
   static RealArray deriv;
   deriv.clear();

   Real t_powk = 1.0;
   Real t = (currentEpoch - initialEpoch).GetMjd() * 86400;
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
// const RealArray& GetFrequencyBiasDerivative(const GmatTime atTime)
//-----------------------------------------------------------------------------------
/**
* This function is used to calculate derivative of Transmitter's frequency at a give
* time w.r.t. frequency polynomial coefficients (vector unit: [1  s  s^2  s^3 ...]).
*
* @param atTime   the time at which it needs to take derivative
*
* @return         derivative of transmitter's frequency w.r.t. frequency polynomial
*                 coefficients (vector unit: [1  s  s^2  s^3 ...])
*/
//-----------------------------------------------------------------------------------
const RealArray& Transmitter::GetFrequencyBiasDerivative(const GmatTime atTime) const
{
   static RealArray deriv;
   deriv.clear();

   Real t_powk = 1.0;
   Real t = (atTime - initialEpoch).GetMjd() * 86400;
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