//$Id$
//------------------------------------------------------------------------------
//                            FirstOrderGaussMarkov
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
// Created: 2020/02/04
/**
 * A class for first order Gauss Markov estimated parameters
 */
//------------------------------------------------------------------------------

#ifndef FirstOrderGaussMarkov_hpp
#define FirstOrderGaussMarkov_hpp

#include "EstimatedParameterModel.hpp"
#include "Rvector3.hpp"

/**
 * Defines interfaces used in the FirstOrderGaussMarkov, contained in libEKF.
 */
class KALMAN_API FirstOrderGaussMarkov : public EstimatedParameterModel
{
public:
   FirstOrderGaussMarkov(const std::string &name);

   virtual ~FirstOrderGaussMarkov();

   FirstOrderGaussMarkov(const FirstOrderGaussMarkov& fogm);
   FirstOrderGaussMarkov& operator=(const FirstOrderGaussMarkov& fogm);

   virtual Rmatrix      GetProcessNoise(GmatEpoch elapsedTime,
                                        const Real accelVector[3]);

   // Override from GmatBase
   virtual GmatBase*    Clone(void) const;

   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:

   Real                 steadyStateValue;
   Real                 steadyStateSigma;
   Real                 halfLife;

   /// Parameter IDs for the FirstOrderGaussMarkov
   enum
   {
      VALUE = EstimatedParameterModelParamCount,
      SIGMA,
      HALF_LIFE,
      FirstOrderGaussMarkovParamCount
   };

   /// Strings describing the ProcessNoiseModel parameters
   static const std::string
                           PARAMETER_TEXT[FirstOrderGaussMarkovParamCount -
                                              EstimatedParameterModelParamCount];
   /// Types of the ProcessNoiseModel parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[FirstOrderGaussMarkovParamCount -
                                              EstimatedParameterModelParamCount];
};

#endif // FirstOrderGaussMarkov_hpp
