//------------------------------------------------------------------------------
//                           UserPathFunctionManager
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
// Created: 2015.12.10
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef UserPathFunctionManager_hpp
#define UserPathFunctionManager_hpp

#include "csaltdefs.hpp"
#include "UserFunctionManager.hpp"
#include "UserFunctionProperties.hpp"
#include "Rmatrix.hpp"
#include "BoundData.hpp"
#include "PathFunctionContainer.hpp"
#include "FunctionInputData.hpp"
#include "FunctionOutputData.hpp"
// YK mod for hessian
#include <boost/numeric/ublas/symmetric.hpp>


class UserPathFunctionManager : public UserFunctionManager
{
public:
   // YK mod for hessian

   UserPathFunctionManager();
   UserPathFunctionManager(const UserPathFunctionManager &copy);
   UserPathFunctionManager& operator=(const UserPathFunctionManager &copy);
   virtual ~UserPathFunctionManager();
   
   
   virtual void Initialize(UserPathFunction          *uData,
                           FunctionInputData         *pData,
                           PathFunctionContainer     *fData,
                           BoundData                 *bData);
   
   virtual void WriteDebugData();
   
   virtual PathFunctionContainer* EvaluateUserFunction(
               FunctionInputData   *pData,
               PathFunctionContainer  *fData, 
               UserFunction::FunctionType fType = UserFunction::ALLFUNCTIONS);
   virtual PathFunctionContainer* EvaluateUserJacobian(
               FunctionInputData   *pData,
               PathFunctionContainer  *fData, 
               UserFunction::FunctionType fType = UserFunction::ALLFUNCTIONS,
               bool isComputingFunctions = false);
   
   // should these return const UserFunctionProperties& (need class data)?
   virtual UserFunctionProperties GetDynFunctionProperties();
   virtual UserFunctionProperties GetAlgFunctionProperties();
   virtual UserFunctionProperties GetCostFunctionProperties();
   
   virtual bool           HasDynFunctions();
   virtual bool           HasAlgFunctions();
   
   virtual Integer        GetNumAlgFunctions();
   virtual const Rvector& GetAlgFunctionsUpperBounds();
   virtual const Rvector& GetAlgFunctionsLowerBounds();
   
   virtual void           SetParamData(FunctionInputData *pData);
   virtual void           SetFunctionData(PathFunctionContainer *fData);
   
protected:
   
   /*
   /// Number of algebraic functions
   Integer      numAlgFunctions;
   /// Number of dynamics functions
   Integer      numDynFunctions;
   /// Number of integral functions
   Integer      numIntFunctions;
   */
   std::vector<Integer> numFunctions;

   // YK mod static vars: num variables vector.
   /// in the future, the root class's numStateVars,... are removed
   std::vector<Integer> numVars;

   /* YK mod for hessian
   /// Indicates if user has algebraic functions
   bool         hasAlgFunctions;
   /// Indicates if user has dynamics functions
   bool         hasDynFunctions;
   /// Indicates if user has integral functions
   bool         hasIntFunctions;
   */
   std::vector<bool> hasFunctions;

   /* YK mod for hessian
   /// The jacobian of dynamics wrt time
   Rmatrix      dynFuncTimeJacobian;
   /// The jacobian of dynamics wrt state
   Rmatrix      dynFuncStateJacobian;
   /// The jacobian of dynamics wrt control
   Rmatrix      dynFuncControlJacobian;
   /// The jacobian of algebraic functions wrt time
   Rmatrix      algFuncTimeJacobian;
   /// The jacobian of algebraic functions wrt state
   Rmatrix      algFuncStateJacobian;
   /// The jacobian of algebraic functions wrt control
   Rmatrix      algFuncControlJacobian;
   /// The jacobian of integral cost function wrt time
   Rmatrix      costFuncTimeJacobian;
   /// The jacobian of integral cost function wrt state
   Rmatrix      costFuncStateJacobian;
   /// The jacobian of integral cost function wrt control
   Rmatrix      costFuncControlJacobian;
   */
   std::vector<std::vector<Rmatrix>> jacobian;

   /*
   /// Sparsity of Jacobian of dynamics wrt time
   Rmatrix      dynFuncTimeJacobianPattern;
   /// Sparsity of Jacobian of dynamics wrt state
   Rmatrix      dynFuncStateJacobianPattern;
   /// Sparsity of Jacobian of dynamics wrt control
   Rmatrix      dynFuncControlJacobianPattern;
   /// Sparsity of Jacobian of algebraic functions wrt time
   Rmatrix      algFuncTimeJacobianPattern;
   /// Sparsity of Jacobian of algebraic functions wrt state
   Rmatrix      algFuncStateJacobianPattern;
   /// Sparsity of Jacobian of algebraic functions wrt control
   Rmatrix      algFuncControlJacobianPattern;
   /// Sparsity of Jacobian of cost function wrt time
   Rmatrix      costFuncTimeJacobianPattern;
   /// Sparsity of Jacobian of cost function wrt state
   Rmatrix      costFuncStateJacobianPattern;
   /// Sparsity of Jacobian of cost function wrt control
   Rmatrix      costFuncControlJacobianPattern;
   */

   // The sparsity patterns of the Jacobians
   std::vector<std::vector<Rmatrix>> jacPattern;

   /// Upper bound on algebraic function values
   Rvector      algFuncUpperBound;
   /// Lower bound on algebraic function values
   Rvector      algFuncLowerBound;
   
   /// Pointer to a FunctionInputData object
   FunctionInputData     *paramData;
   /// Pointer to the function container
   // funcData in ML NOTE: does this need to be stored sicne it is passed
   // in each time
   PathFunctionContainer *pfContainer;
   
   /*
   /// Indicates if there are any state Jacobians that must be finite
   /// differenced
   bool         needsStateJacobianFiniteDiff;
   /// Indicates if there are any control Jacobians that must be finite
   /// differenced
   bool         needsControlJacobianFiniteDiff;
   /// Indicates if there are any time Jacobians that must be finite
   /// differenced
   bool         needsTimeJacobianFiniteDiff;
   */
   boost::numeric::ublas::matrix<bool> needsJacobianFiniteDiff;


   /// protected methods
   /// Initialize member data
   virtual void CheckIfNeedsFiniteDiffJacobian();
   virtual void InitFunctionData(PathFunctionContainer *toData);
   
   /*
   virtual void InitDynFunctionsData(PathFunctionContainer *toData);
   virtual void InitAlgFunctionsData(PathFunctionContainer *toData);
   virtual void InitCostFunctionData(PathFunctionContainer *toData);
   */
   virtual void InitFunctionsData(UserFunction::FunctionType fType, PathFunctionContainer *toData);

   /// Compute function Jacobians
   /*
   virtual void ComputeStateJacobian(FunctionInputData     *pData,
                                     PathFunctionContainer *fData);
   virtual void ComputeControlJacobian(FunctionInputData     *pData,
                                       PathFunctionContainer *fData);
   virtual void ComputeTimeJacobian(FunctionInputData        *pData,
                                    PathFunctionContainer    *fData);
   */
   virtual void ComputeAll(UserFunction::FunctionType fType, 
                           FunctionInputData        *pData,
                           PathFunctionContainer    *fData, bool isComputingHess = false);

   /// Compute sparsity patterns
   virtual void ComputeSparsityPatterns(FunctionInputData     *pData,
                                        PathFunctionContainer *fData,
                                        BoundData             *bData, 
                                        bool isComputingHess = false);
   /*
   virtual void ComputeStateSparsity(FunctionInputData       *pData,
                                     PathFunctionContainer   *uData,
                                     const Rvector           &tUpper,
                                     const Rvector           &tLower,
                                     const Rvector           &stUpper,
                                     const Rvector           &stLower,
                                     const Rvector           &ctrUpper,
                                     const Rvector           &ctrLower);
   virtual void ComputeControlSparsity(FunctionInputData      *pData,
                                       PathFunctionContainer  *uData,
                                       const Rvector           &tUpper,
                                       const Rvector           &tLower,
                                       const Rvector           &stUpper,
                                       const Rvector           &stLower,
                                       const Rvector           &ctrUpper,
                                       const Rvector           &ctrLower);
   virtual void ComputeTimeSparsity(FunctionInputData      *pData,
                                    PathFunctionContainer  *uData,
                                    const Rvector           &tUpper,
                                    const Rvector           &tLower,
                                    const Rvector           &stUpper,
                                    const Rvector           &stLower,
                                    const Rvector           &ctrUpper,
                                    const Rvector           &ctrLower);
   */

   virtual void ComputeSparsity(UserFunction::FunctionType fType,
                                       FunctionInputData      *pData,
                                       PathFunctionContainer  *uData,
                                       const Rvector           &tUpper,
                                       const Rvector           &tLower,
                                       const Rvector           &stUpper,
                                       const Rvector           &stLower,
                                       const Rvector           &ctrUpper,
                                       const Rvector           &ctrLower,
                                       const Rvector           &stcUpper,
                                       const Rvector           &stcLower,
                                       bool isComputingHess = false);

   /*
   virtual void UpdateStateSparsityPattern();
   virtual void UpdateControlSparsityPattern();
   virtual void UpdateTimeSparsityPattern();
   */
   virtual void UpdateSparsityPattern(UserFunction::FunctionType fType,
                                      FunctionInputData *pData,
                                      PathFunctionContainer *uData,
                                      const Rvector &randTime,
                                      const Rvector &randState,
                                      const Rvector &randControl,
                                      const Rvector &randStatic,
                                      bool isComputingHess = false);
};
#endif // UserPathFunctionManager_hpp

