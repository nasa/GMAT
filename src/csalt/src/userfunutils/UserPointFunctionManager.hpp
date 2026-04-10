//------------------------------------------------------------------------------
//                           UserPointFunctionManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Author: Wendy Shoan / NASA
// Created: 2016.07.13
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef UserPointFunctionManager_hpp
#define UserPointFunctionManager_hpp

#include "csaltdefs.hpp"
#include "UserFunctionManager.hpp"
#include "UserFunctionProperties.hpp"
#include "UserPointFunction.hpp"
#include "Rmatrix.hpp"
#include "BoundData.hpp"
#include "PointFunctionContainer.hpp"
#include "FunctionInputData.hpp"
#include "FunctionOutputData.hpp"
#include "NLPFuncUtil_MultiPoint.hpp"
#include "OptimalControlFunction.hpp"

class UserPointFunctionManager : public UserFunctionManager
{
public:
   
   UserPointFunctionManager();
   UserPointFunctionManager(const UserPointFunctionManager &copy);
   UserPointFunctionManager& operator=(const UserPointFunctionManager &copy);
   virtual ~UserPointFunctionManager();
   
   
   virtual void Initialize(UserPointFunction          *uData,
                           const std::vector<Phase*>  &pList,
                           Integer                    numDecParams,
                           const IntegerArray         &startIdxs);
   
   virtual void           InitializeUserFunctionProperties();
   virtual void           InitializeUserObject();
   virtual void           InitializeInputData();
   
   virtual bool           HasBoundaryFunctions();
   virtual Integer        GetNumberBoundaryFunctions();
   
   virtual StringArray    GetFunctionNames();

   virtual void           PrepareInputData();
   
   virtual void           EvaluateUserJacobian();
   virtual void           EvaluateUserFunction();
   virtual void           EvaluatePreparedUserFunction();

   virtual Rvector        GetConLowerBound();
   virtual Rvector        GetConUpperBound();
   virtual Rvector        GetStateLowerBound(Integer phaseIdx);
   virtual Rvector        GetStateUpperBound(Integer phaseIdx);
   virtual Real           GetTimeLowerBound(Integer phaseIdx);
   virtual Real           GetTimeUpperBound(Integer phaseIdx);
   // yk mod static params
   virtual Rvector        GetStaticLowerBound(Integer phaseIdx);
   virtual Rvector        GetStaticUpperBound(Integer phaseIdx);

   virtual Rvector        ComputeBoundNLPFunctions();
   virtual RSMatrix       ComputeBoundNLPJacobian();
   // YK mod RSMatrix to RSMatrix*
   virtual RSMatrix*      ComputeBoundNLPSparsityPattern();

   virtual Rvector        ComputeCostNLPFunctions();
   virtual RSMatrix       ComputeCostNLPJacobian();
   // YK mod RSMatrix to RSMatrix*
   virtual RSMatrix*      ComputeCostNLPSparsityPattern();
   
   virtual void           ComputeSparsityPatterns();
   
   // --- should these be protected? ---
   virtual void           ComputeTimeJacobian();
   virtual void           ComputeStateJacobian();
   virtual void           ComputeStaticJacobian();

   //virtual void           ComputeTimeSparsity(Real tUB, Real tLB); // mod by YK
   virtual void           ComputeTimeSparsity();
   virtual void           ComputeStateSparsity();
   virtual void           ComputeStaticSparsity();
   
   virtual void           UpdateStateSparsityPattern(Integer phaseIdx);
   virtual void           UpdateTimeSparsityPattern(Integer phaseIdx);
   virtual void           UpdateStaticSparsityPattern(Integer phaseIdx);
   // --- should these be protected? ---
   
   
protected:
   /// Number of boundary functions
   Integer                numBoundaryFunctions;
   
   /// Indicates if user has boundary functions
   bool                   hasBoundaryFunctions;
   
   /// Indicates if user optimal control function vector has been initialized
   bool                   optControlFuncsInitialized;

   /// Upper bound on boundary function values
   Rvector                conUpperBound;
   /// Lower bound on boundary function values
   Rvector                conLowerBound;
   
   /// Number of phases
   Integer                numPhases;
      
   /// Total number of NLP variables in the problem
   Integer                totalNumDecisionParams;
   
   /// The phase list
   std::vector <Phase*>   phaseList;
   PointFunctionContainer *pfContainer;
   
   /// The user's object
   UserPointFunction      *userObject;
   /// Jacobian data for bound functions
   JacobianData           *boundJacobianData;
   /// Jacobian data for cost functions
   JacobianData           *costJacobianData;
   
   /// bound multi-point
   NLPFuncUtil_MultiPoint *boundNLPUtil;
   /// cost multi-point
   NLPFuncUtil_MultiPoint *costNLPUtil;
   
   /// Starting index for each phase into the NLP decision vector
   IntegerArray                     decVecStartIdxs;
   /// data for the beginning of each phase
   std::vector<FunctionInputData*>  initialInputData;
   /// data for the end of each phase
   std::vector<FunctionInputData*>  finalInputData;
   /// data for each user boundary function
   std::vector<OptimalControlFunction*>  optControlFuncVec;
   /// PointFunctionContainer object
   
   virtual void           InitializeBoundNLPUtil();
   virtual void           InitializeCostNLPUtil();
   virtual void           DetermineFunctionDependencies(); 
   virtual void           FillJacobianRows(
      const OptimalControlFunction::VariableType jacType,
      std::vector<std::vector<Rmatrix>> &phaseJacs,
      std::vector<std::vector<std::string>> &phasePos);
   
   Real GetMax(const Rmatrix &mat);

};
#endif // UserPointFunctionManager_hpp

