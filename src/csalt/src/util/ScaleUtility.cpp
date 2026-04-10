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
 */
//------------------------------------------------------------------------------

#include "ScaleUtility.hpp"
#include "LowThrustException.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
ScaleUtility::ScaleUtility() :
   numVars           (0),
   numCons           (0),
   costWeight        (1.0),
   numRowsinSparsity (0),
   isInitialized     (false)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ScaleUtility::ScaleUtility(const ScaleUtility &copy) :
   numVars           (copy.numVars),
   numCons           (copy.numCons),
   costWeight        (copy.costWeight),
   numRowsinSparsity (copy.numRowsinSparsity),
   isInitialized     (copy.isInitialized)
{
   Integer sz        = (Integer) copy.decVecWeight.GetSize();
   decVecWeight.SetSize(sz);
   decVecWeight      = copy.decVecWeight;
   
   sz                = (Integer) copy.decVecShift.GetSize();
   decVecShift.SetSize(sz);
   decVecShift       = copy.decVecShift;
   
   sz                = (Integer) copy.conVecWeight.GetSize();
   conVecWeight.SetSize(sz);
   conVecWeight      = copy.conVecWeight;

   jacRowIdxVec      = copy.jacRowIdxVec;
   jacColIdxVec      = copy.jacColIdxVec;
   whichStateVar     = copy.whichStateVar;
   ifDefect          = copy.ifDefect;
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ScaleUtility& ScaleUtility::operator=(const ScaleUtility &copy)
{
   if (&copy == this)
      return *this;

   numVars           = copy.numVars;
   numCons           = copy.numCons;
   costWeight        = copy.costWeight;
   numRowsinSparsity = copy.numRowsinSparsity;
   isInitialized     = copy.isInitialized;

   Integer sz        = (Integer) copy.decVecWeight.GetSize();
   decVecWeight.SetSize(sz);
   decVecWeight      = copy.decVecWeight;
   
   sz                = (Integer) copy.decVecShift.GetSize();
   decVecShift.SetSize(sz);
   decVecShift       = copy.decVecShift;
   
   sz                = (Integer) copy.conVecWeight.GetSize();
   conVecWeight.SetSize(sz);
   conVecWeight      = copy.conVecWeight;

   jacRowIdxVec      = copy.jacRowIdxVec;
   jacColIdxVec      = copy.jacColIdxVec;
   whichStateVar     = copy.whichStateVar;
   ifDefect          = copy.ifDefect;

   return *this;
   
}
   
//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ScaleUtility::~ScaleUtility()
{
   // nothing to do here 
}


//------------------------------------------------------------------------------
//  void Initialize(Integer numV,
//                  Integer numC,
//                  IntegerArray jacRowIdxs,
//                  IntegerArray jacColIdxs)
//------------------------------------------------------------------------------
/**
 * This method initializes the ScaleUtility.  Configures the non-dimensionalizer
 * and prepares for use.  Initializes weights to ones and shifts to zeros.
 *
 * @param <numV>  number of variables in the decision vector
 * @param <numC>  number of constraints in the constraint vector
 * @param <jacRowIdxs>  column vector containing row indexes of
 *                      non zero Jacobian entries - must be same length as
 *                      jacColIdxVec
 * @param <jacColIdxs>  column vector containing column indexes of
 *                      non zero Jacobian entries - must be same length as
 *                      jacRowIdxs
 */
//------------------------------------------------------------------------------
void ScaleUtility::Initialize(Integer numV,
                              Integer numC,
                              IntegerArray jacRowIdxs,
                              IntegerArray jacColIdxs)
{
   // Set number of variables, constraints and sparsity info.
   numVars           = numV;
   numCons           = numC;
   jacRowIdxVec      = jacRowIdxs;
   jacColIdxVec      = jacColIdxs;
   numRowsinSparsity = jacColIdxVec.size();
   // @todo Validate that jacRowIdxVec and jacColIdxVec are the
   // same length
   // @todo Call the class specific methods to intialize
   // data if necessary.
   
   costWeight   = 1;
   conVecWeight.SetSize(numCons+1);
   conVecWeight(numCons) = 1.0;
   for (Integer ii = 0; ii < numCons; ii++)
   {
      ifDefect.push_back(false);
      whichStateVar.push_back(0);
      conVecWeight(ii) = 1.0;
   }

   // Initialize scaling parameters to ones and zeros.
   decVecWeight.SetSize(numVars);
   decVecShift.SetSize(numVars);
   for (Integer ii = 0; ii < numVars; ii++)
   {
      decVecWeight(ii) = 1.0;
      decVecShift(ii)  = 0.0;
   }
}

//------------------------------------------------------------------------------
//  void ScaleDecisionVector(Rvector &decVec)
//------------------------------------------------------------------------------
/**
 * This method scales the decision vector
 *
 * @param <decVec>  the decision vector in unscaled units
 *
 * @return the decision vector in scaled units
 */
//------------------------------------------------------------------------------
void ScaleUtility::ScaleDecisionVector(Rvector &decVec)
{
   // @todo add validation for size of vector
   for (Integer varIdx = 0; varIdx < numVars; varIdx++)
       decVec(varIdx) = decVec(varIdx) * decVecWeight(varIdx) +
                        decVecShift(varIdx);
}

//------------------------------------------------------------------------------
//  void UnScaleDecisionVector(Rvector &decVec)
//------------------------------------------------------------------------------
/**
 * This method unscales the decision vector
 *
 * @param <decVec>  the decision vector in scaled units
 *
 * @return the decision vector in unscaled units
 */
//------------------------------------------------------------------------------
void ScaleUtility::UnScaleDecisionVector(Rvector &decVec)
{
   // @todo add validation for size of vector
   for (Integer varIdx = 0; varIdx < numVars; varIdx++)
       decVec(varIdx) = (decVec(varIdx) - decVecShift(varIdx))/
                        decVecWeight(varIdx);
}

//------------------------------------------------------------------------------
//  void ScaleConstraintVector(Rvector &conVec)
//------------------------------------------------------------------------------
/**
 * This method scales the constraint vector
 *
 * @param <decVec>  the constraint vector in unscaled units
 *
 * @return the constraint vector in scaled units
 */
//------------------------------------------------------------------------------
void ScaleUtility::ScaleConstraintVector(Rvector &conVec)
{
   // @todo add validation for size of vector

   //  Ensure the utility is fully initialized
   if (!isInitialized)
   {
      throw LowThrustException("Constraint scaling must be calculated"
           " before scaling the constraint vector\n");
   }

   for (Integer conIdx = 0; conIdx < numCons; conIdx++)
       conVec(conIdx) = conVec(conIdx) * conVecWeight(conIdx+1);
}

//------------------------------------------------------------------------------
//  void UnScaleConstraintVector(Rvector &conVec)
//------------------------------------------------------------------------------
/**
 * This method unscales the constraint vector
 *
 * @param <decVec>  the constraint vector in scaled units
 *
 * @return the constraint vector in unscaled units
 */
//------------------------------------------------------------------------------
void ScaleUtility::UnScaleConstraintVector(Rvector &conVec)
{
   // @todo add validation for size of vector

   // Ensure the utility is fully initialized
   if (!isInitialized)
   {
      throw LowThrustException("Constraint scaling must be calculated"
           " before unscaling the constraint vector\n");
   }

   for (Integer conIdx = 0; conIdx < numCons; conIdx++)
       conVec(conIdx) = conVec(conIdx) / conVecWeight(conIdx+1);
}

//------------------------------------------------------------------------------
//  Real ScaleCostFunction(Real cost)
//------------------------------------------------------------------------------
/**
 * This method scales the cost function
 *
 * @param <cost>  the cost function in unscaled units
 *
 * @return the cost function in scaled units
 */
//------------------------------------------------------------------------------
Real ScaleUtility::ScaleCostFunction(Real cost)
{
   return cost * costWeight;
}

//------------------------------------------------------------------------------
//  Real UnScaleCostFunction(Real cost)
//------------------------------------------------------------------------------
/**
 * This method unscales the cost function
 *
 * @param <cost>  the cost function in scaled units
 *
 * @return the cost function in unscaled units
 */
//------------------------------------------------------------------------------
Real ScaleUtility::UnScaleCostFunction(Real cost)
{
   return cost / costWeight;
}

//------------------------------------------------------------------------------
//  void ScaleCostConstraintVector(Rvector &cost)
//------------------------------------------------------------------------------
/**
 * This method scales the cost function and constraints when in a single
 * vector.
 *
 * @param <cost>  Vector with cost and constraints in
 *                single vector in dimensional form
 *
 * @return Vector with cost and constraints in
 *                single vector in non-dimensional form
 */
//------------------------------------------------------------------------------
void ScaleUtility::ScaleCostConstraintVector(Rvector &cost)
{
   // @todo add validation for size of vector

   cost(0)    = ScaleCostFunction(cost(0));
   Integer sz = (Integer) cost.GetSize();
   Rvector conVec(sz - 1);    // this is inefficient - how to do this better?
   for (Integer ii = 0; ii < sz-1; ii++)
      conVec(ii) = cost(ii+1);
   ScaleConstraintVector(conVec);
   for (Integer ii = 0; ii < sz-1; ii++)
      cost(ii+1) = conVec(ii);
}

//------------------------------------------------------------------------------
//  void ScaleJacobian(RSMatrix &jac)
//------------------------------------------------------------------------------
/**
 * This method scales the sparse jacobian
 *
 * @param <jac>  The sparse jacobian matrix in dimensional form
 *
 * @return The sparse jacobian matrix in non-dimensional form
 */
//------------------------------------------------------------------------------
void ScaleUtility::ScaleJacobian(RSMatrix &jac)
{
   // Ensure the utility is fully initialized
   if (!isInitialized)
   {
      throw LowThrustException("Constraint scaling must be calculated"
           " before scaling the jacobian\n");
   }

   // If Jacobian is undefined (i.e. optimizer is doing FD), then
   // there is nothing to do.
   if (numRowsinSparsity == 0)
       return;

   // Scale the Jacobian
   for (Integer arrIdx = 0; arrIdx < numRowsinSparsity; arrIdx++)
   {
       Integer funIdx = jacRowIdxVec.at(arrIdx);
       Integer varIdx = jacColIdxVec.at(arrIdx);
       jac(funIdx,varIdx) = jac(funIdx,varIdx) * conVecWeight(funIdx) / 
                            decVecWeight(varIdx);
   }
}

//------------------------------------------------------------------------------
//  void UnScaleJacobian(RSMatrix &jac)
//------------------------------------------------------------------------------
/**
 * This method unscales the sparse jacobian
 *
 * @param <jac>  The sparse jacobian matrix in non-dimensional form
 *
 * @return The sparse jacobian matrix in dimensional form
 */
//------------------------------------------------------------------------------
void ScaleUtility::UnScaleJacobian(RSMatrix &jac)  // or RSMatrix?
{
   // Ensure the utility is fully initialized
   if (!isInitialized)
   {
      throw LowThrustException("Constraint scaling must be calculated"
           " before unscaling the jacobian\n");
   }

   // If Jacobian is undefined (i.e. optimizer is doing FD), then
   // there is nothing to do.
   if (numRowsinSparsity == 0)
       return;
   
   // Unscale the Jacobian
   for (Integer arrIdx = 0; arrIdx < numRowsinSparsity; arrIdx++)
   {
       Integer funIdx = jacRowIdxVec.at(arrIdx);
       Integer varIdx = jacColIdxVec.at(arrIdx);
       jac(funIdx,varIdx) = jac(funIdx,varIdx) / conVecWeight(funIdx) *
                            decVecWeight(varIdx);
   }
}

//------------------------------------------------------------------------------
//  void SetDecVecScalingBounds(const Rvector &decVecUpper,
//                              const Rvector &decVecLower)
//------------------------------------------------------------------------------
/**
 * This method sets the decision vector weights and shifts based on bounds
 *
 * @param <decVecUpper>  Array of upper bound values for the decision vector
 * @param <decVecLower>  Array of lower bound values for the decision vector
 */
//------------------------------------------------------------------------------
void ScaleUtility::SetDecVecScalingBounds(const Rvector &decVecUpper,
                                          const Rvector &decVecLower)
{
   // @todo add validation for size of vector(s)

   for (Integer varIdx = 0; varIdx < numVars; varIdx++)
   {
      decVecWeight(varIdx) = 1.0 / (decVecUpper(varIdx) -
                             decVecLower(varIdx));
      decVecShift(varIdx)  = 0.5 - decVecUpper(varIdx) *
                             decVecWeight(varIdx);
   }
}

//------------------------------------------------------------------------------
//  void SetConstraintScalingUserDefined(const Rvector &conVecWt)
//------------------------------------------------------------------------------
/**
 * This method sets the constraint weights based on user defined values
 *
 * @param <conVecWt>  Array of constraint weights of length numCon x 1
 */
//------------------------------------------------------------------------------
void ScaleUtility::SetConstraintScalingUserDefined(const Rvector &conVecWt)
{
   // Validate the input dimensions
   if (conVecWt.GetSize() != numCons)
      throw LowThrustException("Length of constraint weight vector"
           " must be equal to numCons\n");
   
   // Set the weight array
   conVecWeight(0) = costWeight;
   for (Integer ii = 1; ii < conVecWeight.GetSize(); ii++)
      conVecWeight(ii) = conVecWt(ii-1); 
}

//------------------------------------------------------------------------------
//  void SetCostScalingUserDefined(Real costWt)
//------------------------------------------------------------------------------
/**
 * This method sets the cost function weight based on user defined value
 *
 * @param <costWt>  Scalar cost function weight
 */
//------------------------------------------------------------------------------
void ScaleUtility::SetCostScalingUserDefined(Real costWt)
{
   // Set the weight array
   costWeight      = costWt;
   conVecWeight(0) = costWt;
}

//------------------------------------------------------------------------------
//  void SetConstraintScalingJacobian(RSMatrix &jacArray)
//------------------------------------------------------------------------------
/**
 * This method sets the constraint weights based on norm of rows of Jacobian
 * NOTE: you MUST set decision vector scale BEFORE calling this function.
 *
 * @param <jacArray>  input Jacobian
 *
 * @return the scaled Jacobian
 */
//------------------------------------------------------------------------------
void ScaleUtility::SetConstraintScalingJacobian(RSMatrix &jacArray)
{
   // Validate the input dimensions
   // @todo

   // First scale the Jacobian by the decision vector scales.
   Integer r, c;
   r = jacArray.size1();
   c = jacArray.size2();

   for (Integer varIdx = 0; varIdx < numVars; varIdx++)
   {
      for (Integer rr = 0; rr < r; rr++)
         jacArray(rr,varIdx) = jacArray(rr,varIdx) *
                               decVecWeight(varIdx);
   }
  
   conVecWeight(0) = costWeight;

   // Set the weight array
   Real scale = 1.0;
   for (Integer conIdx = 0; conIdx < numCons; conIdx++)
   {
      Rvector rowVec(c);
      for (Integer col = 0; col < c; col++)
         rowVec(col) = jacArray(conIdx, col);

      Real normRow = 1.0 / rowVec.GetMagnitude();
      if (GmatMathUtil::Abs(normRow) >= 1.0e-4 && GmatMathUtil::Abs(normRow) <= 1.0e10)
          scale = normRow;
      else
          scale = 1.0;

      conVecWeight(conIdx+1) = scale;
   }

   // The scaling has been set, so the utility is fully initialized.
   isInitialized = true;
}

//------------------------------------------------------------------------------
//  void SetConstraintScalingDefectAndUser(RSMatrix &jacArray)
//------------------------------------------------------------------------------
/**
 * This method sets the constraint weights based on norm of rows of Jacobian 
 * for any constraints that are NOT defect. If they are defect, then
 * they should be scaled by the relevant state vector scaling.
 * NOTE: you MUST set decision vector scale BEFORE calling
 * this function and ifDefect and whichStateVar should be set externally
 *
 * @param <jacArray>  input Jacobian
 *
 * @return the scaled Jacobian
 */
//------------------------------------------------------------------------------
void ScaleUtility::SetConstraintScalingDefectAndUser(RSMatrix &jacArray)
{
   // First scale the Jacobian by the decision vector scales.
   Integer r, c;
   r = jacArray.size1();
   c = jacArray.size2();

   for (Integer varIdx = 0; varIdx < numVars; varIdx++)
   {
      for (Integer rr = 0; rr < r; rr++)
         jacArray(rr,varIdx) = jacArray(rr,varIdx) *
                               decVecWeight(varIdx);
   }

   // Set the scaling for the cost function in the constraint scaling array
   conVecWeight(0) = costWeight;

   // Set the weight array
   for (Integer conIdx = 0; conIdx < numCons; conIdx++)
   {
      if (ifDefect.at(conIdx))
      {
         // If the constraint is a defect constraint, scale by the relevant
         // state variable's scaling
         conVecWeight(conIdx+1) = decVecWeight(whichStateVar.at(conIdx)+2);
      }
      else
      {
         // If the constraint is not a defect constraint, scale by an amount
         // which makes the norm of the jacobian row equal to 1
         // Set the weight array
         Real scale = 1.0;
         Rvector rowVec(c);
         for (Integer col = 0; col < c; col++)
            rowVec(col) = jacArray(conIdx, col);

         Real normRow = 1.0 / rowVec.GetMagnitude();
         if (GmatMathUtil::Abs(normRow) >= 1.0e-4 &&
             GmatMathUtil::Abs(normRow) <= 1.0e10)
             scale = normRow;
         else
             scale = 1.0;

         conVecWeight(conIdx+1) = scale;
      }
   }

 // The scaling has been set, so the utility is fully initialized.
   isInitialized = true;  // how can this be set in two different methods?
}

//------------------------------------------------------------------------------
//  Real GetCostWeight()
//------------------------------------------------------------------------------
/**
 * This method returns the cost weight value.
 *
 * @return the cost weight
 */
//------------------------------------------------------------------------------
Real ScaleUtility::GetCostWeight()
{
   return costWeight;
}

//------------------------------------------------------------------------------
//  Rvector GetConVecWeights()
//------------------------------------------------------------------------------
/**
 * This method returns the constraint vector weights.
 *
 * @return the constraint vector weights
 */
//------------------------------------------------------------------------------
Rvector ScaleUtility::GetConVecWeights()
{
   Integer sz = conVecWeight.GetSize();
   Rvector toEnd(sz-1);
   for (Integer ii = 0; ii < (sz-1); ii++)
      toEnd(ii) = conVecWeight(ii+1);
   return toEnd;
}

//------------------------------------------------------------------------------
//  void SetWhichStateVar(const IntegerArray &whichSVar)
//------------------------------------------------------------------------------
/**
 * This method sets the array of state variables corresponding to each defect
 * constraint
 *
 * @param <whichSVar> array of state variables corresponding to each defect 
 *                    constraint
 */
//------------------------------------------------------------------------------
void ScaleUtility::SetWhichStateVar(const IntegerArray &whichSVar)
{
   whichStateVar = whichSVar;
}

//------------------------------------------------------------------------------
//  void SetIfDefect(const BooleanArray &ifD)
//------------------------------------------------------------------------------
/**
 * This method sets the vector to track whether constraints are defect or not
 *
 * @param <ifD> vector to track whether constraints are defect or not
 */
//------------------------------------------------------------------------------
void ScaleUtility::SetIfDefect(const BooleanArray &ifD)
{
   ifDefect = ifD;
}

//------------------------------------------------------------------------------
//  bool IsInitialized()
//------------------------------------------------------------------------------
/**
 * This flag indicating whether or not this ScaleUtilty has been initialized.
 *
 * @return flag indicating whether or not this ScaleUtilty has been initialized
 */
//------------------------------------------------------------------------------
bool ScaleUtility::IsInitialized()
{
   return isInitialized;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
