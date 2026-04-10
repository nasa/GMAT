//------------------------------------------------------------------------------
//                         LobattoIIIaMathUtil
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
// Author: Youngkwang Kim
// Created: 2015/07/08
//
// NOTE: This is a static utility class.  The constructors, destructor, and
// operator= are not implemented.  No instances of this class can be
// instantiated.
//
/**
*/
//------------------------------------------------------------------------------

#include "LobattoIIIaMathUtil.hpp" 
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG
/*
* LobattoIIIaMathUtil class
*/

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void HermiteInterpolation(Real time,
//                            const Rvector *timePoints,
//                            const std::vector< Rvector> &stateArray,
//                            const std::vector< Rvector> &dynArray,
//                            Rvector &psdStateVec, Rvector &psdDynVec)
//------------------------------------------------------------------------------
/**
 * Performs hermite interpolation given inputs
 *
 * @param <time>        target time
 * @param <timePoints>  array of time points
 * @param <stateArray>  state array at time points
 * @param <dynArray>    dynamics array at time points
 * @param <psdStateVec> [output] state vector at target time
 * @param <psdDynVec>   [output] dynamics vector at target time
 *
 */
//------------------------------------------------------------------------------
void LobattoIIIaMathUtil::HermiteInterpolation(Real time,
                                 const Rvector *timePoints,
                                 const std::vector< Rvector> &stateArray,
                                 const std::vector< Rvector> &dynArray,
                                 Rvector &psdStateVec, Rvector &psdDynVec)
{
   Integer numStateVars = stateArray.size();
   if (numStateVars != dynArray.size())
   {
      std::string errMsg("The size mismatch between state array "
                         "and dynamics array.");
         // throw exception in the future.
         // LowThrustException::LowThrustException(errMsg);
   }
   
   psdStateVec.SetSize(numStateVars);
   psdDynVec.SetSize(numStateVars);

   for (Integer idx = 0; idx < numStateVars; idx++)
   {
      Rvector coeff = GetHermiteCoeff(timePoints, stateArray[idx],
                                      dynArray[idx]);
#ifdef DEBUG

      MessageInterface::ShowMessage("*** TEST *** stateArray values are:\n%s\n",
         timePoints->ToString(16).c_str());
      MessageInterface::ShowMessage("*** TEST *** stateArray values are:\n%s\n",
         stateArray[idx].ToString(16).c_str());
      MessageInterface::ShowMessage("*** TEST *** dynArray values are:\n%s\n",
         dynArray[idx].ToString(16).c_str());
      MessageInterface::ShowMessage("*** TEST *** coefficient values are:\n%s\n",
         coeff.ToString(16).c_str());
#endif //DEBUG
      psdStateVec(idx) = ComputeFunctionValue(time, coeff);
      psdDynVec(idx) = ComputeDerivativeValue(time, coeff);
   }
};


//------------------------------------------------------------------------------
//  void HermiteInterpolation(const Rvector *interpPts,
//                            const Rvector *timePoints,
//                            const std::vector< Rvector> &stateArray,
//                            const std::vector< Rvector> &dynArray,
//                            std::vector< Rvector> &psdStateVec, 
//                            std::vector< Rvector> &psdDynVec)
//------------------------------------------------------------------------------
/**
* Performs hermite interpolation given inputs
*
* @param <time>        target time
* @param <timePoints>  array of time points
* @param <stateArray>  state array at time points
* @param <dynArray>    dynamics array at time points
* @param <psdStateVec> [output] state vectors at target time points
* @param <psdDynVec>   [output] dynamics vectors at target time points
*
*/
//------------------------------------------------------------------------------
void LobattoIIIaMathUtil::HermiteInterpolation(const Rvector *interpPts,
                                       const Rvector *timePoints,
                                       const std::vector< Rvector> &stateArray,
                                       const std::vector< Rvector> &dynArray,
                                       std::vector< Rvector> &psdStateVec, 
                                       std::vector< Rvector> &psdDynVec)
{
   Integer numStateVars = stateArray.size();
   Integer numPts = interpPts->GetSize();
   if (numStateVars != dynArray.size())
   {
      std::string errMsg("The size mismatch between state array "
         "and dynamics array.");
      // throw exception in the future.
      // LowThrustException::LowThrustException(errMsg);
   }

   Rmatrix stateData(numPts, numStateVars), dynData(numPts, numStateVars);

   for (Integer idx = 0; idx < numStateVars; idx++)
   {
      Rvector coeff = GetHermiteCoeff(timePoints, stateArray[idx],
                                      dynArray[idx]);
#ifdef DEBUG

      MessageInterface::ShowMessage("*** TEST *** stateArray values are:\n%s\n",
         timePoints->ToString(16).c_str());
      MessageInterface::ShowMessage("*** TEST *** stateArray values are:\n%s\n",
         stateArray[idx].ToString(16).c_str());
      MessageInterface::ShowMessage("*** TEST *** dynArray values are:\n%s\n",
         dynArray[idx].ToString(16).c_str());
      MessageInterface::ShowMessage("*** TEST *** coefficient values are:\n%s\n",
         coeff.ToString(16).c_str());
#endif //DEBUG
      for (Integer idx2 = 0; idx2 < numPts; idx2++)
      {       
         stateData(idx2, idx) = ComputeFunctionValue((*interpPts)(idx2), coeff);
         dynData(idx2, idx) = ComputeDerivativeValue((*interpPts)(idx2), coeff);         
      }
   }
   psdStateVec.clear();
   psdDynVec.clear();
   for (Integer idx2 = 0; idx2 < numPts; idx2++)
   {
      Rvector tmpStateVec(numStateVars), tmpDynVec(numStateVars);
      for (Integer idx = 0; idx < numStateVars; idx++)
      {
         tmpStateVec(idx) = stateData(idx2, idx);
         tmpDynVec(idx) = dynData(idx2, idx);
      }
      psdStateVec.push_back(tmpStateVec);
      psdDynVec.push_back(tmpDynVec);
   }
   
};

//------------------------------------------------------------------------------
//  void HermiteInterpolation(const Rvector *interpPts,
//                            const Rvector *timePoints,
//                            const std::vector< Rvector> &stateArray,
//                            const std::vector< Rvector> &dynArray,
//                            Rmatrix &psdStateVec, 
//                            Rmatrix &psdDynVec)
//------------------------------------------------------------------------------
/**
* Performs hermite interpolation given inputs
*
* @param <time>        target time
* @param <timePoints>  array of time points
* @param <stateArray>  state array at time points
* @param <dynArray>    dynamics array at time points
* @param <psdStateVec> [output] state vectors at target time points
* @param <psdDynVec>   [output] dynamics vectors at target time points
*
*/
//------------------------------------------------------------------------------
void LobattoIIIaMathUtil::HermiteInterpolation(const Rvector *interpPts,
                                       const Rvector *timePoints,
                                       const std::vector< Rvector> &stateArray,
                                       const std::vector< Rvector> &dynArray,
                                       Rmatrix &stateData,
                                       Rmatrix &dynData)
{
   Integer numStateVars = stateArray.size();
   Integer numPts = interpPts->GetSize();
   if (numStateVars != dynArray.size())
   {
      std::string errMsg("The size mismatch between state array "
         "and dynamics array.");
      // throw exception in the future.
      // LowThrustException::LowThrustException(errMsg);
   }
   stateData.SetSize(numPts, numStateVars);
   dynData.SetSize(numPts, numStateVars);

   for (Integer idx = 0; idx < numStateVars; idx++)
   {
      Rvector coeff = GetHermiteCoeff(timePoints, stateArray[idx],
         dynArray[idx]);
#ifdef DEBUG

      MessageInterface::ShowMessage("*** TEST *** stateArray values are:\n%s\n",
         timePoints->ToString(16).c_str());
      MessageInterface::ShowMessage("*** TEST *** stateArray values are:\n%s\n",
         stateArray[idx].ToString(16).c_str());
      MessageInterface::ShowMessage("*** TEST *** dynArray values are:\n%s\n",
         dynArray[idx].ToString(16).c_str());
      MessageInterface::ShowMessage("*** TEST *** coefficient values are:\n%s\n",
         coeff.ToString(16).c_str());
#endif //DEBUG
      for (Integer idx2 = 0; idx2 < numPts; idx2++)
      {
         stateData(idx2, idx) = ComputeFunctionValue((*interpPts)(idx2), coeff);
         dynData(idx2, idx) = ComputeDerivativeValue((*interpPts)(idx2), coeff);
      }
   }
};

//------------------------------------------------------------------------------
//  Real ComputeFunctionValue(Real time, Rvector coeff)
//------------------------------------------------------------------------------
/**
 * Computes the function value given the time and coefficient vector
 *
 * @param <time>        target time
 * @param <coeff>       coefficient vector
 *
 * @return the computed function value
 */
//------------------------------------------------------------------------------
Real LobattoIIIaMathUtil::ComputeFunctionValue(Real time, Rvector coeff)
{
   //Y = polyval(P, X)
   //Y = P(1)*X^N + P(2)*X^(N-1) + ... + P(N)*X + P(N+1)
   Integer n = coeff.GetSize();
   Real Y = 0.0;
   for (Integer idx = 0; idx < n; idx++)
   {
      Y = Y + coeff(idx)*pow(time, n - idx - 1);
   }
   return Y;
};

//------------------------------------------------------------------------------
//  Real ComputeDerivativeValue(Real time, Rvector coeff)
//------------------------------------------------------------------------------
/**
 * Computes the derivative value given the time and coefficient vector
 *
 * @param <time>        target time 
 * @param <coeff>       coefficient vector
 *
 * @return the computed derivative value
 */
//------------------------------------------------------------------------------
Real LobattoIIIaMathUtil::ComputeDerivativeValue(Real time, Rvector coeff)
{
   
   //Y = P(1)*N*X^(N-1) + P(2)*X^(N-2) + ... + P(N)
   Integer n = coeff.GetSize();
   Real Y = 0.0;
   for (Integer idx = 0; idx < n - 1; idx++)
   {
      Y = Y + double(n - idx - 1)*coeff(idx)*pow(time, n - idx - 2);
   }
   return Y;
};

//------------------------------------------------------------------------------
//  Rvector GetHermiteCoeff(const Rvector *timeVec,
//                          const Rvector &funcValues,
//                          const Rvector &dynValues)
//------------------------------------------------------------------------------
/**
 * Computes the hermite coefficient vector
 *
 * @param <timeVec>     time points 
 * @param <funcValues>  function values at time points
 * @param <dynValues>   derivative values at time points 
 *
 * @return the computed hermite coefficients
 */
//------------------------------------------------------------------------------
Rvector LobattoIIIaMathUtil::GetHermiteCoeff(const Rvector *timeVec,
                                             const Rvector &funcValues,
                                             const Rvector &dynValues)
{
   // Compute Hermite polynomial coefficients with derivative values.
   // it is desirable to set timeVec(0) = 0.0 
   // in order to minimize truncation errors in interpolation.

#ifdef DEBUG
   MessageInterface::ShowMessage("*** TEST *** time values are:\n%s\n",
      timeVec->ToString(16).c_str());
   MessageInterface::ShowMessage("*** TEST *** func values are:\n%s\n",
      funcValues.ToString(16).c_str());
   MessageInterface::ShowMessage("*** TEST *** dyn values are:\n%s\n",
      dynValues.ToString(16).c_str());
#endif //DEBUG

   Integer n = timeVec->GetSize();
   Rvector z(2 * n), f(2 * n);

   for (Integer idx = 0; idx < n; idx++)
   {
      z(2 * idx) = (*timeVec)(idx);
      z(1 + 2 * idx) = (*timeVec)(idx);
      f(1 + 2 * idx) = (dynValues)(idx);
   }
   
   f(0) = (funcValues)(0);
   for (Integer idx = 1; idx < n; idx++)
   {
      f(idx * 2) = ((funcValues)(idx) - (funcValues)(idx-1)) /
                   ((*timeVec)(idx) - (*timeVec)(idx-1));
   }
   #ifdef DEBUG
      MessageInterface::ShowMessage("*** TEST *** z values are:\n%s\n",
                                    z.ToString(16).c_str());
      MessageInterface::ShowMessage("*** TEST *** f values are:\n%s\n",
                                    f.ToString(16).c_str());
   #endif //DEBUG
   Rvector g(2 * (n - 1));
   for (Integer idx = 2; idx < 2*n; idx++)
   {
      for (Integer idx2 = idx; idx2 < 2 * n; idx2++)
      {         
         g(idx2-idx) = (f(idx2) - f(idx2 - 1)) / (z(idx2) - z(idx2 - idx));            
      }
      for (Integer idx2 = idx; idx2 < 2 * n; idx2++)
      {
         f(idx2) = g(idx2 - idx);
      }
      
   }
   #ifdef DEBUG
   
      MessageInterface::ShowMessage("*** TEST *** f values are:\n%s\n",
                                    f.ToString(16).c_str());
   #endif //DEBUG
   Rvector hp(2 * n);
   hp.MakeZeroVector();

   Rvector p(1, 1.0);

   for (Integer idx = 1; idx <= 2*n; idx++)
   {
      Rvector q;
      q.SetSize(2 * n - idx + p.GetSize());
      for (Integer idx2 = 0; idx2 < 2 * n - idx; idx2++)
      {
         q(idx2) = 0.0;
      }
      for (Integer idx2 = 2 * n - idx; idx2 < q.GetSize(); idx2++)
      {
         q(idx2) = p(idx2-(2 * n - idx));
      }
      hp = hp + f(idx - 1)*q;
      #ifdef DEBUG
         MessageInterface::ShowMessage("*** TEST *** hp values are:\n%s\n",
                                       hp.ToString(16).c_str());
      #endif //DEBUG
      Rvector tmpVec(2, 1.0, - z(idx - 1));
      Rvector result = Convolution(p, tmpVec);
      p.SetSize(result.GetSize());
      p = result;
      #ifdef DEBUG
         MessageInterface::ShowMessage("*** TEST *** p values are:\n%s\n",
                                       p.ToString(16).c_str());
     #endif //DEBUG
   }
#ifdef DEBUG
   MessageInterface::ShowMessage("*** TEST *** hp values are:\n%s\n",
      hp.ToString(16).c_str());
#endif //DEBUG
   return hp;
};

//------------------------------------------------------------------------------
//  Rvector Convolution(Rvector x, Rvector h)
//------------------------------------------------------------------------------
/**
 * conduct convolution between x and h vectors
 *
 * @param <x>     first input vector
 * @param <h>     second input vector
  *
 * @return the convolution vector
 */
//------------------------------------------------------------------------------
Rvector LobattoIIIaMathUtil::Convolution(Rvector x, Rvector h)
{
   //function Y = convsimple_mat(x, h)
   Integer m = x.GetSize();
   Integer n = h.GetSize();

   Rvector X(m + n), H(m + n);
   // set X
   X.MakeZeroVector();
   for (Integer idx = 0; idx < m; idx++)
   {
      X(idx) = x(idx);
   }
   // set H
   H.MakeZeroVector();
   for (Integer idx = 0; idx < n; idx++)
   {
      H(idx) = h(idx);
   }

   std::vector<Integer> candidate;
   std::vector<Integer>::iterator result;
   candidate.push_back(m + n - 1);
   candidate.push_back(m );
   candidate.push_back(n);

   result = std::max_element(candidate.begin(), candidate.end());

   Rvector Y(*result);
   // set Y
   Y.MakeZeroVector();

   for (Integer idx = 0; idx < n + m - 1; idx++)
   {
      Y(idx) = 0.0;
      for (Integer idx2 = 0; idx2 < m; idx2++)
      {
         #ifdef DEBUG
            MessageInterface::ShowMessage("X(%d) = %le\n", idx2, X(idx2));
         #endif //DEBUG
         
         if (idx - idx2 >= 0)
         {
            #ifdef DEBUG
               MessageInterface::ShowMessage("H(%d) = %le\n", (idx - idx2),
                                             H(idx - idx2));
            #endif //DEBUG
            Y(idx) = Y(idx) + X(idx2)*H(idx - idx2);
         }
         #ifdef DEBUG
            MessageInterface::ShowMessage("Y(%d) = %le\n", idx, Y(idx));
         #endif //DEBUG
      }
   }       
   return Y;
};

