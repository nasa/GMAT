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

#ifndef SNCProcessNoise_hpp
#define SNCProcessNoise_hpp

#include "ProcessNoiseBase.hpp"
#include "Rvector3.hpp"

/**
 * Defines interfaces used in the SNCProcessNoise, contained in libEKF.
 */
class KALMAN_API SNCProcessNoise : public ProcessNoiseBase
{
public:
   SNCProcessNoise(const std::string &name);

   virtual ~SNCProcessNoise();

   SNCProcessNoise(const SNCProcessNoise& spn);
   SNCProcessNoise& operator=(const SNCProcessNoise& spn);

   virtual Rmatrix66       GetProcessNoise(const GmatEpoch elapsedTime, const GmatTime &epoch);
   virtual Rmatrix         GetProcessNoise(Real elapsedTime);

   // Override from GmatBase
   virtual GmatBase*    Clone(void) const;

   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);

   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);

   
   virtual GmatTime         GetGmatTimeParameter(const Integer id) const;
   virtual GmatTime         SetGmatTimeParameter(const Integer id,
                                             const GmatTime value);
   virtual GmatTime         GetGmatTimeParameter(const std::string &label) const;
   virtual GmatTime         SetGmatTimeParameter(const std::string &label,
                                             const GmatTime value);
   
   virtual const Rvector& GetRvectorParameter(const Integer id) const;
   virtual const Rvector& GetRvectorParameter(const std::string &label) const;
   virtual const Rvector& SetRvectorParameter(const Integer id,
                                              const Rvector &value);
   virtual const Rvector& SetRvectorParameter(const std::string &label,
                                              const Rvector &value);
   
   virtual const Rmatrix& GetRmatrixParameter(const Integer id) const;
   virtual const Rmatrix& GetRmatrixParameter(const std::string &label) const;
   virtual const Rmatrix& SetRmatrixParameter(const Integer id, const Rmatrix &value);
   virtual const Rmatrix& SetRmatrixParameter(const std::string &label, const Rmatrix &value);

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:

   Rvector3             accelSigmaVec;
   Real                 elapsedTime;
   Real                 currentEpoch;
   GmatTime             currentEpochGT;
   Rmatrix            currentProcessNoise;

   /// Parameter IDs for the SNCProcessNoise
   enum
   {
      ACCEL_SIGMA_VECTOR = ProcessNoiseBaseParamCount,
      ELAPSED_TIME,
      CURRENT_EPOCH,
      CURRENT_EPOCH_GT,
      CURRENT_NOISE_MAT,
      SNCProcessNoiseParamCount
   };

   /// Strings describing the ProcessNoiseModel parameters
   static const std::string
                           PARAMETER_TEXT[SNCProcessNoiseParamCount -
                                              ProcessNoiseBaseParamCount];
   /// Types of the ProcessNoiseModel parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[SNCProcessNoiseParamCount -
                                              ProcessNoiseBaseParamCount];
};

#endif // SNCProcessNoise_hpp
