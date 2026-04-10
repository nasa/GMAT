//$Id$
//------------------------------------------------------------------------------
//                               SNCProcessNoise
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
 * A class for process noise using the SNC model
 */
//------------------------------------------------------------------------------

#include "SNCProcessNoise.hpp"
#include "NoiseException.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"


//#define Q_MATRIX_UPDATE
//---------------------------------
// static data
//---------------------------------

const std::string
SNCProcessNoise::PARAMETER_TEXT[SNCProcessNoiseParamCount - ProcessNoiseBaseParamCount] =
{
   "AccelNoiseSigma",
   "ElapsedTime",
   "CurrentEpoch",
   "CurrentEpochGT",
   "CurrentNoiseMatrix",
};

const Gmat::ParameterType
SNCProcessNoise::PARAMETER_TYPE[SNCProcessNoiseParamCount - ProcessNoiseBaseParamCount] =
{
   Gmat::RVECTOR_TYPE,     // "AccelNoiseSigma",
   Gmat::REAL_TYPE,     // "ElapsedTime",
   Gmat::REAL_TYPE,     // "CurrentEpoch",
   Gmat::GMATTIME_TYPE,     // "   "CurrentEpochGT",
   Gmat::RMATRIX_TYPE,     // "CurrentNoiseMatrix",
};


//------------------------------------------------------------------------------
// SNCProcessNoise(const std::string &itsTypeName,
//       const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the new object
 */
//------------------------------------------------------------------------------
SNCProcessNoise::SNCProcessNoise(const std::string &name) :
   ProcessNoiseBase    ("SNCProcessNoise",name,"StateNoiseCompensation")
{
   objectTypeNames.push_back("SNCProcessNoise");

   parameterCount = SNCProcessNoiseParamCount;

   accelSigmaVec.Set(1e-8, 1e-8, 1e-8);
}


//------------------------------------------------------------------------------
// ~SNCProcessNoise()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SNCProcessNoise::~SNCProcessNoise()
{
}


//------------------------------------------------------------------------------
// SNCProcessNoise(const SNCProcessNoise& pnm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
SNCProcessNoise::SNCProcessNoise(
      const SNCProcessNoise& spn) :
   ProcessNoiseBase (spn),
   accelSigmaVec    (spn.accelSigmaVec)
{
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool SNCProcessNoise::IsParameterReadOnly(const Integer id) const
{
   // All these are owned parameters
   if (id == ELAPSED_TIME)
      return true;
   if (id == CURRENT_EPOCH)
      return true;
   if (id == CURRENT_EPOCH_GT)
      return true;
   if (id == CURRENT_NOISE_MAT)
      return true;

   return ProcessNoiseBase::IsParameterReadOnly(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
 //---------------------------------------------------------------------------
bool SNCProcessNoise::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}
//------------------------------------------------------------------------------
// SNCProcessNoise& operator=(const SNCProcessNoise& pnm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
SNCProcessNoise& SNCProcessNoise::operator=(
      const SNCProcessNoise& spn)
{
   if (this != &spn)
   {
      ProcessNoiseBase::operator=(spn);

      accelSigmaVec = spn.accelSigmaVec;
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
Rmatrix66 SNCProcessNoise::GetProcessNoise(const GmatEpoch elapsedTime, const GmatTime &epoch)
{

   Rmatrix66 result(false);

   Real dt2 = elapsedTime*elapsedTime;
   Real dt3 = elapsedTime*dt2;

   for (UnsignedInt ii = 0U; ii < 3U; ii++)
   {
      result(ii,ii)     = accelSigmaVec(ii)*accelSigmaVec(ii)*dt3/3.;
      result(ii,ii+3)   = accelSigmaVec(ii)*accelSigmaVec(ii)*dt2/2.;
      result(ii+3,ii)   = accelSigmaVec(ii)*accelSigmaVec(ii)*dt2/2.;
      result(ii+3,ii+3) = accelSigmaVec(ii)*accelSigmaVec(ii)*elapsedTime;
   }

   #ifdef Q_MATRIX_UPDATE
      MessageInterface::ShowMessage("ElapsedTime = %6.13f\n", elapsedTime);
      MessageInterface::ShowMessage("CurrentTime = %6.13f\n", epoch.GetMjd());
      MessageInterface::ShowMessage("Q Pre-Transform = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
            MessageInterface::ShowMessage("   %.12le", result(i, j));
         MessageInterface::ShowMessage("\n");
      }
   #endif
   ConvertMatrix(result, epoch);

   #ifdef Q_MATRIX_UPDATE
      MessageInterface::ShowMessage("Q Prost-Transform = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
            MessageInterface::ShowMessage("   %.12le", result(i, j));
         MessageInterface::ShowMessage("\n");
      }
   #endif
   return result;
}

//------------------------------------------------------------------------------
// Rmatrix66 GetProcessNoise(const Real elapsedTime)
//------------------------------------------------------------------------------
/**
 * Get the process noise for a specified elaped time, made to allow easier 
 * external access
 *
 * @param elapsedTime The elapsed time to evaluate the process noise over.
 *
 * @return The process noise matrix.
 */
 //------------------------------------------------------------------------------
Rmatrix SNCProcessNoise::GetProcessNoise(const Real elapsedTime)
{
   Rmatrix66 result(false);

   Real dt2 = elapsedTime * elapsedTime;
   Real dt3 = elapsedTime * dt2;

   for (UnsignedInt ii = 0U; ii < 3U; ii++)
   {
      result(ii, ii) = accelSigmaVec(ii)*accelSigmaVec(ii)*dt3 / 3.;
      result(ii, ii + 3) = accelSigmaVec(ii)*accelSigmaVec(ii)*dt2 / 2.;
      result(ii + 3, ii) = accelSigmaVec(ii)*accelSigmaVec(ii)*dt2 / 2.;
      result(ii + 3, ii + 3) = accelSigmaVec(ii)*accelSigmaVec(ii)*elapsedTime;
   }
   #ifdef Q_MATRIX_UPDATE
      MessageInterface::ShowMessage("ElapsedTime = %6.13f\n", elapsedTime);
      MessageInterface::ShowMessage("CurrentTime = %6.13f\n", currentEpoch);
      MessageInterface::ShowMessage("Q Pre-Transform = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
            MessageInterface::ShowMessage("   %.12le", result(i, j));
         MessageInterface::ShowMessage("\n");
      }
   #endif
   if(hasPrecisionTime)
   {
      ConvertMatrix(result, currentEpochGT);
   }
   else
   {
      ConvertMatrix(result, currentEpoch);
   }
   currentProcessNoise = result;

   #ifdef Q_MATRIX_UPDATE
      MessageInterface::ShowMessage("Q Prost-Transform = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
            MessageInterface::ShowMessage("   %.12le", result(i, j));
         MessageInterface::ShowMessage("\n");
      }
   #endif
   return currentProcessNoise;
}

//------------------------------------------------------------------------------
//  SNCProcessNoise* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the SNCProcessNoise.
 *
 * @return clone of the SNCProcessNoise.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SNCProcessNoise::Clone(void) const
{
   return (new SNCProcessNoise(*this));
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType SNCProcessNoise::GetParameterType(const Integer id) const
{
   if (id >= ProcessNoiseBaseParamCount && id < SNCProcessNoiseParamCount)
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
std::string SNCProcessNoise::GetParameterTypeString(const Integer id) const
{
   if (id >= ProcessNoiseBaseParamCount && id < SNCProcessNoiseParamCount)
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
std::string SNCProcessNoise::GetParameterText(const Integer id) const
{
   if (id >= ProcessNoiseBaseParamCount && id < SNCProcessNoiseParamCount)
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
Integer SNCProcessNoise::GetParameterID(const std::string &str) const
{
   for (int i=ProcessNoiseBaseParamCount; i<SNCProcessNoiseParamCount; i++)
   {
      if (str == SNCProcessNoise::PARAMETER_TEXT[i - ProcessNoiseBaseParamCount])
         return i;
   }
   
   return ProcessNoiseBase::GetParameterID(str);
}

//---------------------------------------------------------------------------
//  GmatTime GetGmatTimeParameter(const Integer id,
//                        const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
GmatTime SNCProcessNoise::GetGmatTimeParameter(const std::string &label) const
{
   return GetRealParameter(SNCProcessNoise::GetParameterID(label));
}

//---------------------------------------------------------------------------
//  GmatTime GetGmatTimeParameter(const Integer id,
//                        const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
GmatTime SNCProcessNoise::GetGmatTimeParameter(const Integer id) const
{
   if (id == ELAPSED_TIME)
   {
      return elapsedTime;
   }


   return ProcessNoiseBase::GetRealParameter(id);
}

//---------------------------------------------------------------------------
//  GmatTime SetGmatTimeParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
GmatTime SNCProcessNoise::SetGmatTimeParameter(const std::string &label, const GmatTime value)
{
   return SetGmatTimeParameter(SNCProcessNoise::GetParameterID(label), value);
}

//---------------------------------------------------------------------------
//  GmatTime SetGmatTimeParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
GmatTime SNCProcessNoise::SetGmatTimeParameter(const Integer id, const GmatTime value)
{
   if (id == CURRENT_EPOCH_GT)
   {
      currentEpochGT = value;
      if (!hasPrecisionTime)
         hasPrecisionTime = true;
      return currentEpochGT;
   }

   return ProcessNoiseBase::SetGmatTimeParameter(id, value);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id,
//                        const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
Real SNCProcessNoise::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(SNCProcessNoise::GetParameterID(label));
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id,
//                        const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
Real SNCProcessNoise::GetRealParameter(const Integer id) const
{
   if (id == ELAPSED_TIME)
   {
      return elapsedTime;
   }

   if (id == CURRENT_EPOCH)
   {
      return currentEpoch;
   }

   return ProcessNoiseBase::GetRealParameter(id);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
Real SNCProcessNoise::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(SNCProcessNoise::GetParameterID(label),value);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
Real SNCProcessNoise::SetRealParameter(const Integer id, const Real value)
{
   if (id == ELAPSED_TIME)
   {
      // When setting elapsed time from the psm during covariance propagation, update currentProcessNoise too
      elapsedTime = value;
      GetProcessNoise(elapsedTime);
      return elapsedTime;
   }

   if (id == CURRENT_EPOCH)
   {
      currentEpoch = value;
      return currentEpoch;
   }

   return ProcessNoiseBase::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
//  Real GetRmatrixParameter(const Integer id)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
const Rmatrix& SNCProcessNoise::GetRmatrixParameter(const std::string &label) const
{
   return GetRmatrixParameter(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  Real GetRmatrixParameter(const Integer id)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
const Rmatrix& SNCProcessNoise::GetRmatrixParameter(const Integer id) const
{
   if (id == CURRENT_NOISE_MAT)
   {
      return currentProcessNoise;
   }
   return ProcessNoiseBase::GetRmatrixParameter(id);
}


//---------------------------------------------------------------------------
//  Real GetRmatrixParameter(const Integer id)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
const Rmatrix& SNCProcessNoise::SetRmatrixParameter(const std::string &label, const Rmatrix &value)
{
   return SetRmatrixParameter(GetParameterID(label),value);
}

//---------------------------------------------------------------------------
//  Real GetRmatrixParameter(const Integer id)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
const Rmatrix& SNCProcessNoise::SetRmatrixParameter(const Integer id, const Rmatrix &value)
{
   if (id == CURRENT_NOISE_MAT)
   {
      currentProcessNoise = value;
      return currentProcessNoise;
   }
   return ProcessNoiseBase::SetRmatrixParameter(id, value);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id,
//                        const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real SNCProcessNoise::GetRealParameter(const Integer id,
                                         const Integer index) const
{
   if (id == ACCEL_SIGMA_VECTOR)
   {
      return accelSigmaVec(index);
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
Real SNCProcessNoise::GetRealParameter(const std::string &label,
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
Real SNCProcessNoise::SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index)
{
   if (id == ACCEL_SIGMA_VECTOR)
   {
      // Enforce size
      if (index >= accelSigmaVec.GetSize())
      {
         throw NoiseException("AccelNoiseSigma must be size 3");
      }
      else if (value <= 0.0)
      {
         throw NoiseException("A nonpositive number was set to AccelNoiseSigma. A valid value has to be a positive number.\n");
      }
      accelSigmaVec.SetElement(index, value);
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
Real SNCProcessNoise::SetRealParameter(const std::string &label,
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
const Rvector& SNCProcessNoise::GetRvectorParameter(const Integer id) const
{
   std::string name;

   switch (id)
   {
   case ACCEL_SIGMA_VECTOR:
      return accelSigmaVec;
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
const Rvector& SNCProcessNoise::GetRvectorParameter(const std::string &label) const
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
const Rvector& SNCProcessNoise::SetRvectorParameter(const Integer id, const Rvector &value)
{
   switch (id)
   {
   case ACCEL_SIGMA_VECTOR:
      if (value.GetSize() == 3)
      {
         for (UnsignedInt ii = 0U; ii < 3U; ii++)
            if (value(ii) <= 0)
               throw NoiseException("A nonpositive number was set to AccelNoiseSigma. A valid value has to be a positive number.\n");

         accelSigmaVec = value.GetRealArray();
      }
      else
      {
         throw NoiseException("The AccelNoiseSigma vector must be a 3 element vector");
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
const Rvector& SNCProcessNoise::SetRvectorParameter(const std::string &label,
                                   const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}
