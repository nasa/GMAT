//$Id$
//------------------------------------------------------------------------------
//                             NumericJacobian
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
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2018.07.10
//
/**
* Declares the NumericJacobian class.  This class is a conversion of the numjac
* function from MATLAB.
*/
//------------------------------------------------------------------------------
#ifndef NumericJacobian_hpp
#define NumericJacobian_hpp

#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

class GMATUTIL_API NumericJacobian
{
public:
   enum JacState
   {
      INITIALIZING,
      PERTURBING,
      CALCULATING,
      REFINING,
      FINISHED
   };

   NumericJacobian();
   NumericJacobian(const NumericJacobian &copy);
   NumericJacobian&         operator=(const NumericJacobian &copy);
   ~NumericJacobian();

   JacState AdvanceState();
   JacState GetState();
   void SetInitialValues(Rvector initialState, Rvector initialDerivs, Rmatrix threshold, Rvector inputfac);
   Rvector GetCurrentVars();
   void SetDerivs(Rvector inputDerivs);
   Rmatrix GetJacobian();
   Rvector GetWorkingStorage();

protected:
   bool CheckInitialParams();
   void CalculatePerturbations();
   void PerturbVars();
   void CalculateJacobian();
   void PrepareForRefinement();
   void CalcRefinement();
   void RefineJacColumn();
   void UpdateWorkingStorage();

   JacState currentState;
   Rvector currentDerivs;
   Rvector y;
   Rvector yscale;
   Rvector Fty;
   Rvector del;
   Rvector currentVars;
   Rvector fac;
   Rmatrix ydel;
   Rmatrix Fdel;
   Rmatrix threshScal;
   Rmatrix S;
   IntegerArray g;
   Rmatrix dFdy;
   Integer ny;
   Integer nF;
   Integer perturbIdx;
   Integer nfevals;
   Integer nfcalls;
   bool stepSizeCalculated;

   // Parameters for refining the Jacobian
   Rvector diffMax;
   IntegerArray rowMax;
   Rvector absFdelRm;
   bool refineCols;
   bool refineCurrCol;
   Integer refineColIdx;
   BooleanArray colsToRefine;
   BooleanArray k1;
   Rvector absFty;
   Rvector absFtyRm;
   Rvector Fscale;
   Rvector fdel;
   Real tmpfac;
   Real delVal;

   // Bounds for working storage values
   Real br;
   Real bl;
   Real bu;
   Real facmin;
   Real facmax;
};
#endif
