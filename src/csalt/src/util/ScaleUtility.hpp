//------------------------------------------------------------------------------
//                              ScaleUtility
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
// Created: 2016.07.22
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 *   NONDIMENSIONLIZER: Base class for non-dimensionalization of decision
 *   vector and sparse Jacobian.
 *
 *   This class performs scaling of decision vector, cost, constraints,
 *   and Jacobian.
 *
 *   Determine and store the weights and shifts properties for
 *   the different quantities to be scaled such as decision vector,
 *   constraint vector, cost function, and Jacobian.
 *   Function: Initialize(), SetDecVecScaling_Bounds,
 *   SetConstraintScaling_UserDefined(), SetCostScaling_UserDefined()
 *
 *   Scale and unscale the decision vector.
 *   Functions: ScaleDecisionVector(), UnScaleDecisionVector()
 *
 *   Scale and unscale the constraint vector.
 *   Functions: ScaleConstraintVector(), UnScaleConstraintVector(),
 *   ScaleCostConstraintVector()
 *
 *   Scale and unscale the cost function.
 *   Functions: ScaleCostFunction(), UnScaleCostFunction(),
 *   ScaleCostConstraintVector()
 *
 *   Scale and unscale the Jacobian.
 *   Functions: ScaleJacobian(), UnScaleJacobian()
 */
//------------------------------------------------------------------------------

#ifndef ScaleUtility_hpp
#define ScaleUtility_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "DecisionVector.hpp"
#include "GuessGenerator.hpp"
#include "ProblemCharacteristics.hpp"
#include "UserPathFunction.hpp"
#include "UserPathFunctionManager.hpp"
#include "FunctionOutputData.hpp"
#include "FunctionInputData.hpp"
#include "SparseMatrixUtil.hpp"

class ScaleUtility
{
public:
   
   ScaleUtility();
   ScaleUtility(const ScaleUtility &copy);
   ScaleUtility& operator=(const ScaleUtility &copy);
   virtual ~ScaleUtility();
   
   virtual void    Initialize(Integer numV,
                              Integer numC,
                              IntegerArray jacRowIdxs,
                              IntegerArray jacColIdxs);
   
   virtual void    ScaleDecisionVector(Rvector &decVec);
   virtual void    UnScaleDecisionVector(Rvector &decVec);
   
   virtual void    ScaleConstraintVector(Rvector &conVec);
   virtual void    UnScaleConstraintVector(Rvector &conVec);
   
   virtual Real    ScaleCostFunction(Real cost);
   virtual Real    UnScaleCostFunction(Real cost);
   
   virtual void    ScaleCostConstraintVector(Rvector &cost);
   
   virtual void    ScaleJacobian(RSMatrix &jac);
   virtual void    UnScaleJacobian(RSMatrix &jac);  
   
   virtual void    SetDecVecScalingBounds(const Rvector &decVecUpper,
                                          const Rvector &decVecLower);
   virtual void    SetConstraintScalingUserDefined(const Rvector &conVecWt);

   virtual void    SetCostScalingUserDefined(Real costWt);

   virtual void    SetConstraintScalingJacobian(RSMatrix &jacArray);
   
   virtual void    SetConstraintScalingDefectAndUser(RSMatrix &jacArray); 
   
   virtual Real    GetCostWeight();
   virtual Rvector GetConVecWeights();
   virtual void    SetWhichStateVar(const IntegerArray &whichStateVar);
   
   virtual void    SetIfDefect(const BooleanArray &ifD);
   virtual bool    IsInitialized();

   
protected:

   /// The quantities to be scaled

   /// The number of decision variables
   Integer      numVars;
   /// The number of constraints
   Integer      numCons;
   
   /// Quantities used in scaling
   
   /// Quantities that multiply the decision variables
   Rvector      decVecWeight;
   /// Quantities added to decision variables
   Rvector      decVecShift;
   /// Quantity that multiplies the cost Function
   Real         costWeight;
   /// Quantities that multiply the constraints
   Rvector      conVecWeight;
   /// Array of indeces for non-zero rows in Jacobian
   IntegerArray jacRowIdxVec;
   /// Array of indeces for non-zero cols in Jacobian
   IntegerArray jacColIdxVec;
   /// number of entries in jacRowIdxVec and jacColIdxVec
   Integer      numRowsinSparsity;
         
   /// Functional/Operational variables
         
   /// Which state variable corresponds to each defect constraint
   IntegerArray whichStateVar;
   /// Vector to track whether constraints are defect or not
   BooleanArray ifDefect;
   /// Flag to track if the utility has been given scale parameters yet or not
   bool         isInitialized;

};

#endif // ScaleUtility_hpp
