//------------------------------------------------------------------------------
//                              NLPFuncUtil_MultiPoint
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
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "csaltdefs.hpp"
#include "NLPFuncUtil_MultiPoint.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MULTI_POINT

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
NLPFuncUtil_MultiPoint::NLPFuncUtil_MultiPoint() :
   NLPFuncUtil(),
   functionData     (NULL),
   numFunctions     (0),
   functionOffset   (-1), // ?
   numPhases        (0),
   numVars          (0)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
NLPFuncUtil_MultiPoint::NLPFuncUtil_MultiPoint(
                                    const NLPFuncUtil_MultiPoint &copy) :
   NLPFuncUtil(copy),
   functionData   (copy.functionData), // or CLONE?
   numFunctions   (copy.numFunctions),
   functionOffset (copy.functionOffset),
   numPhases      (copy.numPhases),
   numVars        (copy.numVars)
{
   phaseList = copy.phaseList;  // need CLONES here? I suspect not
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
NLPFuncUtil_MultiPoint& NLPFuncUtil_MultiPoint::operator=(
                                    const NLPFuncUtil_MultiPoint &copy)
{
   if (&copy == this)
      return *this;
   
   NLPFuncUtil::operator=(copy);
   
   functionData   = copy.functionData;  // or CLONE?
   numFunctions   = copy.numFunctions;
   functionOffset = copy.functionOffset;
   numPhases      = copy.numPhases;
   numVars        = copy.numVars;

   phaseList      = copy.phaseList;  // need CLONES here? I suspect not

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
NLPFuncUtil_MultiPoint::~NLPFuncUtil_MultiPoint()
{
   if (functionData)
      delete functionData;
}
   
//------------------------------------------------------------------------------
//  void Initialize(const std::vector<Phase*> &pList,
//                  const FunctionOutputData  &funcData,
//                  JacobianData              *jData,
//                  Integer                   numVarsNLP)
//------------------------------------------------------------------------------
/**
 * This method initializes the multipoint
 *
 * @param <pList>          list of Phase pointers
 * @param <funcData>       function output data
 * @param <jData>          the jacobian data
 * @param <numVarsNLP>     number of variables
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_MultiPoint::Initialize(const std::vector<Phase*> &pList,
                                        const FunctionOutputData  &funcData,
                                        JacobianData              *jData,
                                        Integer                   numVarsNLP)
{
   
   #ifdef DEBUG_MULTI_POINT
      MessageInterface::ShowMessage("Entering MP::Initalize ...\n");
   #endif
   // NOTE: jData changes every time we compute function and jacobian? 
   // otherwise, we can make it as a private property and use it when we compute
   // functions and jacobians all the time.

   // Create data helper utility and initialize it
   functionData                = new NLPFunctionData();
   phaseList                   = pList;
   numFunctions                = funcData.GetNumFunctions();
   numVars                     = numVarsNLP; //added by YK
   numPhases                   = pList.size();     
   
   if (numFunctions == 0)
      throw LowThrustException("ERROR - NLPFuncUtil_MultiPoint: failed to "
         "initialize, zero functions were provided\n");

   Integer numFuncDependencies = 1;
   #ifdef DEBUG_MULTI_POINT
      MessageInterface::ShowMessage(
               "In MP::Initalize, about to initialize the functionData ...\n");
   #endif
   functionData->Initialize(numFunctions,numVars,
                            numFunctions * numFuncDependencies);
   
   // Initialize A and B matrices.  A is zero so nothing to do.
   IntegerArray funcIdxs;
   for (Integer ii = 0; ii < numFunctions; ii++)
      funcIdxs.push_back(ii);
   
   // Need an identity matrix
   Rmatrix eye(numFunctions, numFunctions);   // all zeros by default
   for (Integer ii = 0; ii < numFunctions; ii++)
      for (Integer jj = 0; jj < numFunctions; jj++)
      {
         if (ii == jj) eye(ii,jj) = 1.0;
      }
   
   #ifdef DEBUG_MULTI_POINT
      MessageInterface::ShowMessage(
         "Entering MP::InsertBMatPartition ... funcIdxs = %d\n", funcIdxs[0]);
   #endif
   functionData->InsertBMatPartition(funcIdxs[0], funcIdxs[0], &eye);

   // Initialize D Matrix
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      Phase *currentPhase = phaseList.at(phaseIdx);
      bool  hasStateVars  = (currentPhase->GetNumStateVars() > 0);
      if (hasStateVars)
      {
         Rmatrix      initJacPattern  =
                      jData->GetInitStateJacobianPattern(phaseIdx);
         IntegerArray initIdxs        =
                      jData->GetInitStateIdxs(phaseIdx);
         Rmatrix      finalJacPattern =
                      jData->GetFinalStateJacobianPattern(phaseIdx);
         IntegerArray finalIdxs       =
                      jData->GetFinalStateIdxs(phaseIdx);
         // D matrix sub element for initial state Jacobian

         // if initIdxs is continuing natural numbers, then the following line
         // must work. Otherwise, it is a bit complicated. - YK
         #ifdef DEBUG_MULTI_POINT
            MessageInterface::ShowMessage(
                     "Entering MP::InsertDMatPartition (initial) ... %d %d\n",
                     funcIdxs[0], initIdxs[0]);
            Integer r, c;
            initJacPattern.GetSize(r, c);
            MessageInterface::ShowMessage(
                     "   size of initJacPattern = (%d, %d)\n", r, c);
         #endif
         functionData->InsertDMatPartition(funcIdxs[0],
                                           initIdxs[0], &initJacPattern);
 
         // D matrix sub element of final state Jacobian
         #ifdef DEBUG_MULTI_POINT
            MessageInterface::ShowMessage(
                        "Entering MP::InsertDMatPartition (final) ... %d %d\n",
                        funcIdxs[0], finalIdxs[0]);
            finalJacPattern.GetSize(r, c);
            MessageInterface::ShowMessage(
                        "   size of finalJacPattern = (%d, %d)\n", r, c);
         #endif
         // if finalIdxs is continuing natural numbers, then the following line
         // must work. Otherwise, it is a bit complicated. - YK
         functionData->InsertDMatPartition(funcIdxs[0],
                                           finalIdxs[0], &finalJacPattern);
      }

      Rmatrix      initTimeJacPattern  =
                   jData->GetInitTimeJacobianPattern(phaseIdx);
      IntegerArray initTimeIdxs        =
                   jData->GetInitTimeIdxs(phaseIdx);
      Rmatrix      finalTimeJacPattern =
                   jData->GetFinalTimeJacobianPattern(phaseIdx);
      IntegerArray finalTimeIdxs       =
                   jData->GetFinalTimeIdxs(phaseIdx);
      // Initialize arrays for time Jacobian partials
      // D matrix sub element for initial time Jacobian
      // if initTimeIdxs is continuing natural numbers, then the following line
      // must work. Otherwise, it is a bit complicated. - YK
      #ifdef DEBUG_MULTI_POINT
         MessageInterface::ShowMessage(
                  "Entering MP::InsertDMatPartition (initial time) ... %d %d\n",
                  funcIdxs[0], initTimeIdxs[0]);
      #endif
      functionData->InsertDMatPartition(funcIdxs[0],
                                        initTimeIdxs[0], &initTimeJacPattern);

      // D matrix sub element of final time Jacobian
      // if initTimeIdxs is continuing natural numbers, then the following line
      // must work. Otherwise, it is a bit complicated. - YK
      #ifdef DEBUG_MULTI_POINT
         MessageInterface::ShowMessage(
                  "Entering MP::InsertDMatPartition (final time) ... %d %d\n",
                  funcIdxs[0], finalTimeIdxs[0]);
      #endif
      functionData->InsertDMatPartition(funcIdxs[0],
                                        finalTimeIdxs[0], &finalTimeJacPattern);

      
      Integer numStaticVars = currentPhase->GetNumStaticVars();
      if (numStaticVars > 0)
      {
         IntegerArray staticIdxs = jData->GetStaticIdxs(phaseIdx);
         Rmatrix      staticJacPattern =
                                      jData->GetStaticJacobianPattern(phaseIdx);
         functionData->InsertDMatPartition(funcIdxs[0],
                                           staticIdxs[0], &staticJacPattern);
      }
   }
   #ifdef DEBUG_MULTI_POINT
      MessageInterface::ShowMessage("LEAVING MP::Initalize ...\n");
   #endif
}
   
//------------------------------------------------------------------------------
//  void FillUserNLPMatrices(const FunctionOutputData &funcData,
//                           JacobianData             *jData,
//                           Rvector                  &funcValues,
//                           RSMatrix                 &jacArray)
//------------------------------------------------------------------------------
/**
 * This method initializes the multipoint
 *
 * @param <funcData>       function output data
 * @param <jData>          the jacobian data
 * @param <funcValues>     output - the function values
 * @param <jacArray>       output - the jacobian matrix
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_MultiPoint::FillUserNLPMatrices(
                             const FunctionOutputData &funcData,
                             JacobianData             *jData,
                             Rvector                  &funcValues,
                             RSMatrix                 &jacArray)
{
   // set the sparsity pattern of the parQMatrix before setting values to
   // save time.
   const RSMatrix* ptrDMatrix = functionData->GetDMatrixPtr();
   bool            hasZeros   = true;
   jacArray = SparseMatrixUtil::GetSparsityPattern(ptrDMatrix, hasZeros);

   funcValues.SetSize(numFunctions); // resize funcValues to prevent size issues

   // Update arrays that are not constant (q, and dq/dz)
   
   // A is all zeros so nothing to do.
   IntegerArray funcIdxs;
   for (Integer ii = 0; ii < numFunctions; ii++)
      funcIdxs.push_back(ii);
   
   if (numFunctions > 0)
   {
      funcValues = funcData.GetFunctionValues(); // added by YK
   }
   
   // Initialize Q Matrix
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      Phase *currentPhase = phaseList.at(phaseIdx);
      bool  hasStateVars  = (currentPhase->GetNumStateVars() > 0);
      if (hasStateVars)
      {
         
         Rmatrix      initJac = jData->GetInitStateJacobian(phaseIdx);
         IntegerArray initIdxs = jData->GetInitStateIdxs(phaseIdx);
         Rmatrix      finalJac = jData->GetFinalStateJacobian(phaseIdx);
         IntegerArray finalIdxs = jData->GetFinalStateIdxs(phaseIdx);

         // D matrix sub element for initial state Jacobian
         // if initIdxs is continuing natural numbers, then the following line
         // must work. Otherwise, it is a bit complicated. Added by YK
         SparseMatrixUtil::SetSparseBLockMatrix(jacArray, funcIdxs[0],
                                                initIdxs[0], &initJac);
         // D matrix sub element of final state Jacobian
         // if finalIdxs is continuing natural numbers, then the following line
         // must work. Otherwise, it is a bit complicated. Added by YK
         SparseMatrixUtil::SetSparseBLockMatrix(jacArray, funcIdxs[0],
                                                finalIdxs[0], &finalJac);
      }
      Rmatrix      initTimeJac  = jData->GetInitTimeJacobian(phaseIdx);
      IntegerArray initTimeIdxs        = jData->GetInitTimeIdxs(phaseIdx);
      Rmatrix      finalTimeJac = jData->GetFinalTimeJacobian(phaseIdx);
      IntegerArray finalTimeIdxs       = jData->GetFinalTimeIdxs(phaseIdx);
      // NOTE: we no longer we q and dq/dz in NLPFunctionData because
      // they are temporary (not like A, B, and D matrices) and change every
      // time. So, we compute them as they are needed.
      
      // Initialize arrays for time Jacobian partials
      // D matrix sub element for initial time Jacobian
      // if initTimeIdxs is continuing natural numbers, then the following line
      // must work. Otherwise, it is a bit complicated.  Added by YK
      SparseMatrixUtil::SetSparseBLockMatrix(jacArray, funcIdxs[0],
                                             initTimeIdxs[0], &initTimeJac);
      // D matrix sub element of final time Jacobian
      // if initTimeIdxs is continuing natural numbers, then the following line
      // must work. Otherwise, it is a bit complicated.   Added by YK
      SparseMatrixUtil::SetSparseBLockMatrix(jacArray, funcIdxs[0],
                                             finalTimeIdxs[0], &finalTimeJac);

      
      Integer numStaticVars = currentPhase->GetNumStaticVars();
      if (numStaticVars > 0)
      {
         Rmatrix      staticJac  = jData->GetStaticJacobian(phaseIdx);         
         IntegerArray staticIdxs = jData->GetStaticIdxs(phaseIdx);
         SparseMatrixUtil::SetSparseBLockMatrix(jacArray, funcIdxs[0],
                                                staticIdxs[0], &staticJac);
      }
   }
}

//------------------------------------------------------------------------------
//  void ComputeFuncAndJac(const FunctionOutputData &funcData,
//                         JacobianData             *jData,
//                         Rvector                  &funcValues,
//                         RSMatrix                 &jacArray)
//------------------------------------------------------------------------------
/**
 * This method computes the functions and jacobian given the inputs. Added by YK
 *
 * @param <funcData>       function output data
 * @param <jData>          the jacobian data
 * @param <funcValues>     output - the function values
 * @param <jacArray>       output - the jacobian matrix
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_MultiPoint::ComputeFuncAndJac(
                             const FunctionOutputData &funcData,
                             JacobianData             *jData,
                             Rvector                  &funcValues,
                             RSMatrix                 &jacArray)
{
   Rvector QVector;
   RSMatrix parQMatrix;
   FillUserNLPMatrices(funcData, jData, QVector, parQMatrix);

   functionData->ComputeFunctions(&QVector, funcValues);
   functionData->ComputeJacobian(&parQMatrix, jacArray);
}

//------------------------------------------------------------------------------
//  RSMatrix* ComputeSparsityPattern()
//------------------------------------------------------------------------------
/**
 * This method computes the sparsity pattern
 *
 * @return   the computed sparsity pattern
 *
 */
//------------------------------------------------------------------------------
RSMatrix* NLPFuncUtil_MultiPoint::ComputeSparsityPattern()
{
   functionData->ComputeJacSparsityPattern();
   return functionData->GetJacSparsityPatternPointer();
}
