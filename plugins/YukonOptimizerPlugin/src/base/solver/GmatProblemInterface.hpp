//$Id$
//------------------------------------------------------------------------------
//                              GmatProblemInterface
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Created: 2017/10/30, Joshua Raymond, Thinking Systems, Inc.
//
/**
 * Declares GmatProblemInterface methods, derived from MiNLPUserProblem.  
 * Interface is used to collect required optimizer data from other sources in
 * Gmat.
 */
//------------------------------------------------------------------------------

#ifndef GmatProblemInterface_hpp
#define GmatProblemInterface_hpp

#include "YukonUserProblem.hpp"

class YUKON_API Yukonad;

class YUKON_API GmatProblemInterface : public YukonUserProblem
{
public:
   GmatProblemInterface();

   virtual void GetNLPInfo(Integer &totalNumVars, Integer &totalNumCons);
   virtual Rvector GetStartingPoint();
   virtual void GetBoundsInfo(Integer numVars, Integer numCons,
      Rvector &varLowerBounds, Rvector &varUpperBounds,
      Rvector &conLB, Rvector &conUB);
   virtual Real EvaluateCostFunc(Integer numVars, Rvector decVector, bool isNewX);
   virtual Rvector EvaluateCostJac(Integer numVars, Rvector decVector, bool isNewX);
   virtual Rvector EvaluateConFunc(Integer numVars, Rvector decVector, bool isNewX);
   virtual Rmatrix EvaluateConJac(Integer numVars, Rvector decVector, bool isNewX);
   virtual void EvaluateConJacDimensions(Integer numVariables, Rvector decVector, bool isNewX, Integer &rowCount, Integer &colCount);
   virtual std::vector <Real> GetMaxVarStepSize();
   void SetPointerToOptimizer(Yukonad *inputDataPointer);
   void SetConFunction(Integer id, Real value, const std::string &conType);

private:
   Yukonad *optimizerData;
};
#endif