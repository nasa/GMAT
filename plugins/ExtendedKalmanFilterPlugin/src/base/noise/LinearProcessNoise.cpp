//$Id$
//------------------------------------------------------------------------------
//                             LinearProcessNoise
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. under
// contract number NNG15CR67C
//
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2018/09/19
/**
 * A class for process noise using a Linear model
 */
//------------------------------------------------------------------------------

#include "LinearProcessNoise.hpp"
#include "NoiseException.hpp"
#include "StringUtil.hpp"

//---------------------------------
// static data
//---------------------------------

const std::string
LinearProcessNoise::PARAMETER_TEXT[LinearProcessNoiseParamCount - ProcessNoiseBaseParamCount] =
{
   "RateVector",
};

const Gmat::ParameterType
LinearProcessNoise::PARAMETER_TYPE[LinearProcessNoiseParamCount - ProcessNoiseBaseParamCount] =
{
   Gmat::RVECTOR_TYPE,     // "RateVector",
};


//------------------------------------------------------------------------------
// LinearProcessNoise(const std::string &itsTypeName,
//       const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the new object
 */
//------------------------------------------------------------------------------
LinearProcessNoise::LinearProcessNoise(const std::string &name) :
   ProcessNoiseBase    ("LinearProcessNoise",name, "LinearTime")
{
   objectTypeNames.push_back("LinearProcessNoise");

   parameterCount = LinearProcessNoiseParamCount;

   rateVec.Set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}


//------------------------------------------------------------------------------
// ~LinearProcessNoise()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
LinearProcessNoise::~LinearProcessNoise()
{
}


//------------------------------------------------------------------------------
// LinearProcessNoise(const LinearProcessNoise& pnm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
LinearProcessNoise::LinearProcessNoise(
      const LinearProcessNoise& lpn) :
   ProcessNoiseBase (lpn),
   rateVec          (lpn.rateVec)
{
}



//------------------------------------------------------------------------------
// LinearProcessNoise& operator=(const LinearProcessNoise& pnm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
LinearProcessNoise& LinearProcessNoise::operator=(
      const LinearProcessNoise& lpn)
{
   if (this != &lpn)
   {
      ProcessNoiseBase::operator=(lpn);

      rateVec = lpn.rateVec;
   }
   return *this;
}


//------------------------------------------------------------------------------
// Rmatrix66 GetProcessNoise(const GmatEpoch elapsedTime, const GmatTime &epoch)
//------------------------------------------------------------------------------
/**
 * Get the process noise for a specified elaped time
 *
 * @param elapsedTime The elapsed time to evaluate the process noise over.
 * @param epoch The epoch to evaluate the process noise coordinate conversion at.
 *
 * @return The process noise matrix.
 */
//------------------------------------------------------------------------------
Rmatrix66 LinearProcessNoise::GetProcessNoise(const GmatEpoch elapsedTime, const GmatTime &epoch)
{
   Rmatrix66 result(false);

   for (UnsignedInt ii = 0U; ii < 6U; ii++)
      result(ii,ii) = (rateVec(ii)*elapsedTime)*(rateVec(ii)*elapsedTime);

   ConvertMatrix(result, epoch);
   return result;
}


//------------------------------------------------------------------------------
//  LinearProcessNoise* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the LinearProcessNoise.
 *
 * @return clone of the LinearProcessNoise.
 *
 */
//------------------------------------------------------------------------------
GmatBase* LinearProcessNoise::Clone(void) const
{
   return (new LinearProcessNoise(*this));
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType LinearProcessNoise::GetParameterType(const Integer id) const
{
   if (id >= ProcessNoiseBaseParamCount && id < LinearProcessNoiseParamCount)
      return PARAMETER_TYPE[id - ProcessNoiseBaseParamCount];
   else
      return ProcessNoiseBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string LinearProcessNoise::GetParameterTypeString(const Integer id) const
{
   if (id >= ProcessNoiseBaseParamCount && id < LinearProcessNoiseParamCount)
      return ProcessNoiseBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return ProcessNoiseBase::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string LinearProcessNoise::GetParameterText(const Integer id) const
{
   if (id >= ProcessNoiseBaseParamCount && id < LinearProcessNoiseParamCount)
      return PARAMETER_TEXT[id - ProcessNoiseBaseParamCount];
   else
      return ProcessNoiseBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer LinearProcessNoise::GetParameterID(const std::string &str) const
{
   for (int i=ProcessNoiseBaseParamCount; i<LinearProcessNoiseParamCount; i++)
   {
      if (str == LinearProcessNoise::PARAMETER_TEXT[i - ProcessNoiseBaseParamCount])
         return i;
   }
   
   return ProcessNoiseBase::GetParameterID(str);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id,
//                        const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real LinearProcessNoise::GetRealParameter(const Integer id,
                                         const Integer index) const
{
   if (id == RATE_VECTOR)
   {
      return rateVec(index);
   }

   return ProcessNoiseBase::GetRealParameter(id, index);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label,
//                       const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real LinearProcessNoise::GetRealParameter(const std::string &label,
                                         const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value,
//                        const Integer index)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real LinearProcessNoise::SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index)
{
   if (id == RATE_VECTOR)
   {
      // Enforce size
      if (index >= rateVec.GetSize())
      {
         throw NoiseException("RateVector must be size 6");
      }
      rateVec.SetElement(index, value);
      return value;
   }

   return ProcessNoiseBase::SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real LinearProcessNoise::SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& LinearProcessNoise::GetRvectorParameter(const Integer id) const
{
   std::string name;

   switch (id)
   {
   case RATE_VECTOR:
      return rateVec;
   default:
      return ProcessNoiseBase::GetRvectorParameter(id);
   }
}

//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& LinearProcessNoise::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const Rvector& SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& LinearProcessNoise::SetRvectorParameter(const Integer id, const Rvector &value)
{
   switch (id)
   {
   case RATE_VECTOR:
      if (value.GetSize() == 6)
      {
         rateVec = value.GetRealArray();
      }
      else
      {
         NoiseException ne("");
         ne.SetDetails(errorMessageFormat.c_str(),
            GmatStringUtil::ToString(value.GetSize(), 16).c_str(),
            "Cd", "Real Number >= 0.0");
         throw ne;
      }
   default:
      return ProcessNoiseBase::SetRvectorParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// const Rvector& SetStringParameter(const std::string &label, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& LinearProcessNoise::SetRvectorParameter(const std::string &label,
                                   const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}
