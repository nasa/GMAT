//------------------------------------------------------------------------------
//                           OptimalControlFunction
//------------------------------------------------------------------------------
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
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
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2018.09.10
//
/**
* Defines the OptimalControlFunction class.
*
* From original MATLAB prototype:
* Author: S. Hughes.  steven.p.hughes@nasa.gov
*/
//------------------------------------------------------------------------------
#ifndef OptimalControlFunction_hpp
#define OptimalControlFunction_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "LowThrustException.hpp"
#include "Phase.hpp"
#include "ScalingUtility.hpp"

class CSALT_API OptimalControlFunction
{
public:
   OptimalControlFunction(std::string functionName);
   OptimalControlFunction(const OptimalControlFunction &copy);
   OptimalControlFunction& operator=(const OptimalControlFunction &copy);
   virtual ~OptimalControlFunction();

   enum VariableType{ STATE, CONTROL, TIME, STATIC };

   virtual void Initialize();
   virtual bool HasDependency(VariableType varType, Integer pointIdx);
   virtual bool HasAnalyticJacobian(VariableType varType, Integer pointIdx);
   virtual void SetJacobian(VariableType varType, Integer pointIdx);
   virtual void SetData(VariableType varType, Integer pointIdx,
      Rvector theData);
   virtual void SetReferenceEpoch(Real referenceEpoch);
   virtual void EvaluateJacobian();
   virtual Rvector EvaluateFunctions() = 0;
   virtual Rvector GetUnscaledFunctionValues();
   virtual Rmatrix GetJacobian(VariableType varType, Integer pointIdx);
   virtual Rvector GetLowerBounds();
   virtual Rvector GetUpperBounds();
   virtual Rvector GetUnscaledLowerBounds();
   virtual Rvector GetUnscaledUpperBounds();
   virtual StringArray GetConstraintNames();
   virtual void SetLowerBounds(Rvector functionLB);
   virtual void SetUpperBounds(Rvector functionUB);
   virtual void ScaleFunctionBounds() = 0;
   virtual void ScaleData();
   virtual Integer GetNumFunctions();
   void SetNumFunctions(Integer numFuncs);
   virtual Integer GetNumPoints();
   void GetPointData(Integer pointIdx, Integer &currPhaseIdx,
      std::string &position);
   void SetPointData(Integer newPhaseIdx, std::string position,
      Integer pointIdx = -1);
   void SetNumPhases(Integer theNumPhases);
   void SetPhaseList(std::vector<Phase*> pList);
   void SetPhaseDependencies(IntegerArray phaseDeps);
   void SetPointDependencies(IntegerArray pointDeps);
   void SetStateDepMap(BooleanArray stateDeps);
   void SetControlDepMap(BooleanArray controlDeps);
   void SetTimeDepMap(BooleanArray timeDeps);
   void SetParamDepMap(BooleanArray paramDeps);
   void SetFuncScalingUtility(ScalingUtility *scaleUtil);

protected:
   virtual void EvaluateAnalyticJacobian(VariableType varType,
      Integer pointIdx, bool &hasAnalyticJac, Rmatrix &jacArray);
   void ValidatePointIdx(Integer pointIdx);
   void ValidateFunctionBounds();
   virtual bool IsValidPhasePosition(const Integer &phasePos);

   /// The name of the algebraic function
   std::string funcName;
   /// The number of discretization points that are independent variables
   Integer numPoints;
   /// The number of functions
   Integer numFunctions;
   /// The number of phases involved in this function
   Integer numPhases;
   /// Map of which phase each discretization point depends on
   IntegerArray phaseDepMap;
   /// Map of which points along each phase from phaseDepMap have a dependency
   IntegerArray pointDepMap;
   /// Map of which discretization points have state dependency
   BooleanArray stateDepMap;
   /// Map of which discretization points have control dependency
   BooleanArray controlDepMap;
   /// Map of which discretization points have time dependency
   BooleanArray timeDepMap;
   /// Map of which discretization points have param dependency
   BooleanArray paramDepMap;
   /// Map of which discretization points have an analytic state Jacobian
   BooleanArray analyticStateJacMap;
   /// Map of which discretization points have an analytic control Jacobian
   BooleanArray analyticControlJacMap;
   /// Map of which discretization points have an analytic time Jacobian
   BooleanArray analyticTimeJacMap;
   /// Map of which discretization points have an analytic param Jacobian
   BooleanArray analyticParamJacMap;
   /// States at each discretization point
   std::vector<Rvector> stateData;
   /// Controls at each discretization point
   std::vector<Rvector> controlData;
   /// Time at each discretization point
   std::vector<Rvector> timeData;
   /// Static parameters
   std::vector<Rvector> paramData;
   /// State Jacobian elements at each discretization point
   std::vector<Rmatrix> stateJacElements;
   /// Control Jacobian elements at each discretization point
   std::vector<Rmatrix> controlJacElements;
   /// Time Jacobian elements at each discretization point
   std::vector<Rmatrix> timeJacElements;
   /// Static parameter Jacobian elements at each discretization point
   std::vector<Rmatrix> paramJacElements;
   /// State working storage for numjac use
   std::vector<Rvector> numjacStateWorkingStorage;
   /// Control working storage for numjac use
   std::vector<Rvector> numjacControlWorkingStorage;
   /// Static parameter working storage for numjac use
   std::vector<Rvector> numjacParamWorkingStorage;
   /// Function upper bounds if built-in
   Rvector lowerBounds;
   /// Function lower bounds if built-in
   Rvector upperBounds;
   /// Used to stored the unscaled version of the function lower bounds
   Rvector unscaledLowerBounds;
   /// Used to stored the unscaled version of the function upper bounds
   Rvector unscaledUpperBounds;
   /// The reference epoch used in time scaling
   Real refEpoch;
   ///// Time scale factor
   //Real TU;
   ///// Distance scale factor
   //Real DU;
   ///// Velocity scale factor
   //Real VU;
   ///// Mass scale factor
   //Real MU;
   /// Array of names describing what a function is constraining, must be
   /// the same size as the function vector
   StringArray constraintNames;
   /// Array of units in a GMAT state vector, can be used with a scaling
   /// utility object
   StringArray funcUnitList;
   /// The scaling utility used by the function
   ScalingUtility *funcScaleUtil;
   /// List of all phase objects in optimization problem
   std::vector<Phase*> phaseList;
   /// Boolean of whether Jacobian initialization has been completed
   bool dataInitialized;
   bool boundsScaled;
};
#endif
