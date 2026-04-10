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

#include "FirstOrderGaussMarkov.hpp"
#include "EstimatedParameterException.hpp"
#include "Rmatrix33.hpp"
#include "StringUtil.hpp"

//---------------------------------
// static data
//---------------------------------

const std::string
FirstOrderGaussMarkov::PARAMETER_TEXT[FirstOrderGaussMarkovParamCount - EstimatedParameterModelParamCount] =
{
   "SteadyStateValue",
   "SteadyStateSigma",
   "HalfLife",
};

const Gmat::ParameterType
FirstOrderGaussMarkov::PARAMETER_TYPE[FirstOrderGaussMarkovParamCount - EstimatedParameterModelParamCount] =
{
   Gmat::REAL_TYPE,     // "SteadyStateValue",
   Gmat::REAL_TYPE,     // "SteadyStateSigma",
   Gmat::REAL_TYPE,     // "HalfLife",
};


//------------------------------------------------------------------------------
// FirstOrderGaussMarkov(const std::string &itsTypeName,
//       const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the new object
 */
//------------------------------------------------------------------------------
FirstOrderGaussMarkov::FirstOrderGaussMarkov(const std::string &name) :
   EstimatedParameterModel    ("FirstOrderGaussMarkov",name,"FirstOrderGaussMarkov"),
   steadyStateValue(1.0),
   steadyStateSigma(0.1),
   halfLife(7200)
{
   objectTypeNames.push_back("FirstOrderGaussMarkov");

   allowedSolveFors.push_back("Cd");
   allowedSolveFors.push_back("AtmosDensityScaleFactor");

   parameterCount = FirstOrderGaussMarkovParamCount;
}


//------------------------------------------------------------------------------
// ~FirstOrderGaussMarkov()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
FirstOrderGaussMarkov::~FirstOrderGaussMarkov()
{
}


//------------------------------------------------------------------------------
// FirstOrderGaussMarkov(const FirstOrderGaussMarkov& pnm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
FirstOrderGaussMarkov::FirstOrderGaussMarkov(
      const FirstOrderGaussMarkov& fogm) :
   EstimatedParameterModel (fogm),
   steadyStateValue  (fogm.steadyStateValue),
   steadyStateSigma  (fogm.steadyStateSigma),
   halfLife          (fogm.halfLife)
{
}



//------------------------------------------------------------------------------
// FirstOrderGaussMarkov& operator=(const FirstOrderGaussMarkov& pnm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
FirstOrderGaussMarkov& FirstOrderGaussMarkov::operator=(
      const FirstOrderGaussMarkov& fogm)
{
   if (this != &fogm)
   {
      EstimatedParameterModel::operator=(fogm);

      steadyStateValue = fogm.steadyStateValue;
      steadyStateSigma = fogm.steadyStateSigma;
      halfLife = fogm.halfLife;
   }
   return *this;
}


//------------------------------------------------------------------------------
// Rmatrix GetProcessNoise(const GmatEpoch elapsedTime, const Real accelVector[3])
//------------------------------------------------------------------------------
/**
 * Get the process noise for a specified elapsed time
 *
 * @param elapsedTime The elapsed time to evaluate the process noise over.
 * @param accelVector The acceleration vector from the associated force,
 *                    used in the calculation of the process noise matrix.
 *
 * @return The process noise matrix.
 */
 //------------------------------------------------------------------------------
Rmatrix FirstOrderGaussMarkov::GetProcessNoise(const GmatEpoch elapsedTime,
                                               const Real accelVector[3])
{
   Rmatrix result(7, 7);

   // Pre-calculate some values
   Real dt2 = elapsedTime * elapsedTime;
   Real dt3 = elapsedTime * dt2;

   Real tau = halfLife / GmatMathUtil::Ln(2.0);

   Real tau2 = tau * tau;
   Real tau3 = tau * tau2;
   Real tau4 = tau * tau3;
   Real tau5 = tau * tau4;

   Real exp2 = GmatMathUtil::Exp(-2 * elapsedTime / tau);
   Real exp1 = GmatMathUtil::Exp(-elapsedTime / tau);

   Real sigma2 = 2.0 * steadyStateSigma * steadyStateSigma /
                  (convFactor * convFactor * tau);

   Rmatrix33 accelOuterMatrix;
   for (UnsignedInt ii = 0U; ii < 3U; ii++)
      for (UnsignedInt jj = 0U; jj < 3U; jj++)
         accelOuterMatrix(ii, jj) = accelVector[ii] * accelVector[jj];

   Real gamma_pp, gamma_pv, gamma_pa, gamma_vv, gamma_va, gamma_aa;

   Real tolerance = 0.01;
   if (GmatMathUtil::Abs(elapsedTime / tau) >= tolerance)
   {
      // Use explicit equation if dt/tau is not too small
      gamma_pp = tau5 * (0.5 - 0.5 * exp2)
         + tau4 * (elapsedTime - 2 * elapsedTime * exp1)
         + tau3 * (-dt2)
         + tau2 * (1 / 3. * dt3);

      gamma_pv = tau4 * (0.5 + 0.5 * exp2 - exp1)
         + tau3 * (elapsedTime * exp1 - elapsedTime)
         + tau2 * (0.5 * dt2);

      gamma_pa = tau3 * (0.5 - 0.5 * exp2) + tau2 * (-elapsedTime * exp1);
      gamma_vv = tau3 * (-1.5 - 0.5 * exp2 + 2 * exp1) + tau2 * (elapsedTime);
      gamma_va = tau2 * (0.5 + 0.5 * exp2 - exp1);
      gamma_aa = tau * (0.5 - 0.5 * exp2);
   }
   else
   {
      // Use taylor series if dt/tau is small enough there are numerical
      // precision issues for exp() terms
      Real dt4 = elapsedTime * dt3;
      Real dt5 = elapsedTime * dt4;

      Real dtOverTau = elapsedTime / tau;
      Real dtOverTau2 = dtOverTau * dtOverTau;

      gamma_pp = dt5 * (1/20. - dtOverTau / 36 + 5 * dtOverTau2 / 504);
      gamma_pv = dt4 * (1/8. - dtOverTau / 12 + 5 * dtOverTau2 / 144);
      gamma_pa = dt3 * (1/6. - dtOverTau / 6 + 11 * dtOverTau2 / 120);
      gamma_vv = dt3 * (1/3. - dtOverTau / 4 + 7 * dtOverTau2 / 60);
      gamma_va = dt2 * (1/2. - dtOverTau / 2 + 7 * dtOverTau2 / 24);
      gamma_aa = elapsedTime * (1. - dtOverTau + 2 * dtOverTau2 / 3);
   }

   // Start populating the output noise matrix
   for (UnsignedInt ii = 0U; ii < 3U; ii++)
   {
      for (UnsignedInt jj = 0U; jj < 3U; jj++)
      {
         result(ii, jj) = sigma2 * gamma_pp * accelOuterMatrix(ii, jj);
         result(ii, jj + 3) = sigma2 * gamma_pv * accelOuterMatrix(ii, jj);
         result(ii + 3, jj) = sigma2 * gamma_pv * accelOuterMatrix(ii, jj);
         result(ii + 3, jj + 3) = sigma2 * gamma_vv * accelOuterMatrix(ii, jj);
      }

      // Populate the last row and column corresponding to the FOGM parameter
      result(ii, 6) = sigma2 * gamma_pa * accelVector[ii];
      result(6, ii) = sigma2 * gamma_pa * accelVector[ii];

      result(ii + 3, 6) = sigma2 * gamma_va * accelVector[ii];
      result(6, ii + 3) = sigma2 * gamma_va * accelVector[ii];
   }

   result(6, 6) = sigma2 * gamma_aa;

   return result;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the FirstOrderGaussMarkov.
 *
 * @return clone of the FirstOrderGaussMarkov.
 *
 */
//------------------------------------------------------------------------------
GmatBase* FirstOrderGaussMarkov::Clone(void) const
{
   return (new FirstOrderGaussMarkov(*this));
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType FirstOrderGaussMarkov::GetParameterType(const Integer id) const
{
   if (id >= EstimatedParameterModelParamCount && id < FirstOrderGaussMarkovParamCount)
      return PARAMETER_TYPE[id - EstimatedParameterModelParamCount];
   else
      return EstimatedParameterModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string FirstOrderGaussMarkov::GetParameterTypeString(const Integer id) const
{
   if (id >= EstimatedParameterModelParamCount && id < FirstOrderGaussMarkovParamCount)
      return EstimatedParameterModel::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return EstimatedParameterModel::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string FirstOrderGaussMarkov::GetParameterText(const Integer id) const
{
   if (id >= EstimatedParameterModelParamCount && id < FirstOrderGaussMarkovParamCount)
      return PARAMETER_TEXT[id - EstimatedParameterModelParamCount];
   else
      return EstimatedParameterModel::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer FirstOrderGaussMarkov::GetParameterID(const std::string &str) const
{
   for (int i=EstimatedParameterModelParamCount; i<FirstOrderGaussMarkovParamCount; i++)
   {
      if (str == FirstOrderGaussMarkov::PARAMETER_TEXT[i - EstimatedParameterModelParamCount])
         return i;
   }
   
   return EstimatedParameterModel::GetParameterID(str);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real FirstOrderGaussMarkov::GetRealParameter(const Integer id) const
{
   if (id == VALUE)
   {
      return steadyStateValue;
   }
   if (id == SIGMA)
   {
      return steadyStateSigma;
   }
   if (id == HALF_LIFE)
   {
      return halfLife;
   }

   return EstimatedParameterModel::GetRealParameter(id);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real FirstOrderGaussMarkov::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real FirstOrderGaussMarkov::SetRealParameter(const Integer id,
                                             const Real value)
{
   if (id == VALUE)
   {
      steadyStateValue = value;
      return steadyStateValue;
   }

   if (id == SIGMA)
   {
      if (value <= 0.0)
      {
         EstimatedParameterException epe("");
         epe.SetDetails(errorMessageFormat.c_str(),
            GmatStringUtil::ToString(value, 16).c_str(),
            "SteadyStateSigma", "Real Number > 0.0");
         throw epe;
      }

      steadyStateSigma = value;
      return steadyStateSigma;
   }

   if (id == HALF_LIFE)
   {
      if (value <= 0.0)
      {
         EstimatedParameterException epe("");
         epe.SetDetails(errorMessageFormat.c_str(),
            GmatStringUtil::ToString(value, 16).c_str(),
            "HalfLife", "Real Number > 0.0");
         throw epe;
      }

      halfLife = value;
      return halfLife;
   }

   return EstimatedParameterModel::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real FirstOrderGaussMarkov::SetRealParameter(const std::string &label,
                                             const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}
