//------------------------------------------------------------------------------
//                              JacobianData
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
// Created: 2015.08.14
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "JacobianData.hpp"
#include "MessageInterface.hpp"
#include "LowThrustException.hpp"
#include "DecisionVector.hpp"

//#define DEBUG_JACOBIAN_DATA

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
JacobianData::JacobianData() :
numPhases     (0),
hasFunctions  (false),
numFunctions  (0)
{
   // all std::vector items are initially empty
   // all Rmatrix items are initially empty
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
JacobianData::JacobianData(const JacobianData &copy) :
   numPhases     (copy.numPhases),
   hasFunctions  (copy.hasFunctions),
   numFunctions  (copy.numFunctions)
{
   CopyArrays(copy);
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
JacobianData& JacobianData::operator=(const JacobianData &copy)
{
   
   if (&copy == this)
      return *this;
   
   numPhases    = copy.numPhases;
   hasFunctions = copy.hasFunctions;
   numFunctions = copy.numFunctions;
   
   CopyArrays(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
JacobianData::~JacobianData()
{
   // we don't delete phases here - we assume that done is somewhere else
}


//------------------------------------------------------------------------------
// void Initialize(Integer numF, bool hasF,
//                 std::vector<Phase*> pList,
//                 IntegerArray        dvStartIdxs);
//------------------------------------------------------------------------------
/**
 * This method initializes the JacobianData
 *
 * @param <numF>         number of functions
 * @param <hasF>         does this JacobianData have functions?
 * @param <pList>        list of phases
 * @param <dvStartIdxs>  array of start indexes
 *
 */
//------------------------------------------------------------------------------
void JacobianData::Initialize(Integer              numF,
                              bool                 hasF,
                              std::vector<Phase*>  pList,
                              const IntegerArray   &dvStartIdxs)
{
   hasFunctions      = hasF;
   numFunctions      = numF;
   numPhases         = pList.size();
   // Need to 'empty' existing list here? (probably not?)
   phaseList         = pList;
   decVecStartIdxs   = dvStartIdxs;
   
   InitJacobianArrays();
   InitDataIndexes();
   InitDependencies();
}


//------------------------------------------------------------------------------
// bool InitDataIndexes()
//------------------------------------------------------------------------------
/**
 * This method initializes the data indexes
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool JacobianData::InitDataIndexes()
{
   initialStateIdxs.clear();
   finalStateIdxs.clear();
   
   initialTimeIdxs.clear();
   finalTimeIdxs.clear();

   // YK mods static params
   staticIdxs.clear();
   
   for (Integer ii = 0; ii < numPhases; ii++)
   {
      DecisionVector *dv        = phaseList[ii]->GetDecisionVector();
      Integer        dvStart    = decVecStartIdxs.at(ii);
      IntegerArray   initSIdxs  = dv->GetInitialStateIdxs();
      IntegerArray   finalSIdxs = dv->GetFinalStateIdxs();
      Integer        initTIdx   = dv->GetInitialTimeIdx();
      Integer        finalTIdx  = dv->GetFinalTimeIdx();
      IntegerArray   stcIdxs    = dv->GetStaticIdxs();

      // Initial state indexes
      for (unsigned int jj = 0; jj < initSIdxs.size(); jj++)
      {
         initSIdxs.at(jj) += dvStart;  // no -1 here
      }
      initialStateIdxs.push_back(initSIdxs);
      // Final state indexes
      for (unsigned int jj = 0; jj < finalSIdxs.size(); jj++)
      {
         finalSIdxs.at(jj) += dvStart;  // no -1 here
      }
      finalStateIdxs.push_back(finalSIdxs);
      // Initial Time Indexes
      IntegerArray initTime;
      initTime.push_back(initTIdx + dvStart); // no -1 here
      initialTimeIdxs.push_back(initTime);
      // Final Time Indexes
      IntegerArray finalTime;
      finalTime.push_back(finalTIdx + dvStart); // no -1 here
      finalTimeIdxs.push_back(finalTime);

      // static indices: if no static vars. stcIdxs[0] == -1
      if (stcIdxs[0] > 0)
      {
         for (unsigned int jj = 0; jj < stcIdxs.size(); jj++)
         {
            stcIdxs.at(jj) += dvStart;  // no -1 here
         }
      }
      staticIdxs.push_back(stcIdxs);
      
   }
   
   return true;
}

//------------------------------------------------------------------------------
// bool InitJacobianArrays()
//------------------------------------------------------------------------------
/**
 * This method initializes the jacobian arrays
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool JacobianData::InitJacobianArrays()
{
   
   // Initializes/dimensions boundary function data members
   for (Integer ii = 0; ii < numPhases; ii++)
   {
      Phase   *currentPhase  = phaseList.at(ii);
      Integer numStateVars   = currentPhase->GetNumStateVars();
      
      // Rmatrix items are all zeros by default
      Rmatrix initTimeJacP(numFunctions, 1);
      initTimeJacobianPattern.push_back(initTimeJacP);
   
      Rmatrix initStateJacP(numFunctions, numStateVars);
      initStateJacobianPattern.push_back(initStateJacP);
 
      Rmatrix finalTimeJacP(numFunctions, 1);
      finalTimeJacobianPattern.push_back(finalTimeJacP);

      Rmatrix finalStateJacP(numFunctions, numStateVars);
      finalStateJacobianPattern.push_back(finalStateJacP);
 
      Rmatrix initTimeJac(numFunctions, 1);
      initTimeJacobian.push_back(initTimeJac);
      
      Rmatrix initStateJac(numFunctions, numStateVars);
      initStateJacobian.push_back(initStateJac);
      
      Rmatrix finalTimeJac(numFunctions, 1);
      finalTimeJacobian.push_back(finalTimeJac);
      
      Rmatrix finalStateJac(numFunctions, numStateVars);
      finalStateJacobian.push_back(finalStateJac);

      // YK mod static params
      Integer numStaticVars = currentPhase->GetNumStaticVars();

      // Rmatrix items are all zeros by default
      Rmatrix staticJacP;
      Rmatrix staticJac(numFunctions, numStateVars);
      
      if (numStaticVars > 0)
      {
         staticJac.SetSize(numFunctions, numStaticVars);
         staticJacP.SetSize(numFunctions, numStaticVars);
      }
      else
      {
         // there is no static jacobian but set a sized array to prevent error.
         staticJac.SetSize(numFunctions, 1);
         staticJacP.SetSize(numFunctions, 1);
      }
      staticJacobian.push_back(staticJac);
      staticJacobianPattern.push_back(staticJacP);
   }
   return true;
}

//------------------------------------------------------------------------------
// bool InitDependencies()
//------------------------------------------------------------------------------
/**
 * This method initializes the dependencies
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool JacobianData::InitDependencies()
{
   for (Integer ii = 0; ii < numPhases; ii++)
   {
      hasInitTimeDependency.push_back(false);
      hasFinalTimeDependency.push_back(false);
      hasInitStateDependency.push_back(false);
      hasFinalStateDependency.push_back(false);
      hasStaticDependency.push_back(false); // YK mod
   }
   return true;
}

//------------------------------------------------------------------------------
// Rmatrix GetInitTimeJacobianPattern(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the initial time jacobian pattern for the given phase 
 * index
 *
 * @param <phaseIdx> the phase index
 *
 * @return the initial time jacobian pattern for the given phase index
 *
 */
//------------------------------------------------------------------------------
Rmatrix JacobianData::GetInitTimeJacobianPattern(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return initTimeJacobianPattern.at(phaseIdx);
}

//------------------------------------------------------------------------------
// Rmatrix GetFinalTimeJacobianPattern(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the final time jacobian pattern for the given phase
 * index
 *
 * @param <phaseIdx> the phase index
 *
 * @return the final time jacobian pattern for the given phase index
 *
 */
//------------------------------------------------------------------------------
Rmatrix JacobianData::GetFinalTimeJacobianPattern(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return finalTimeJacobianPattern.at(phaseIdx);
}

//------------------------------------------------------------------------------
// Rmatrix GetInitStateJacobianPattern(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the initial state jacobian pattern for the given phase
 * index
 *
 * @param <phaseIdx> the phase index
 *
 * @return the initial state jacobian pattern for the given phase index
 *
 */
//------------------------------------------------------------------------------
Rmatrix JacobianData::GetInitStateJacobianPattern(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return initStateJacobianPattern.at(phaseIdx);
}

//------------------------------------------------------------------------------
// Rmatrix GetFinalStateJacobianPattern(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the final state jacobian pattern for the given phase
 * index
 *
 * @param <phaseIdx> the phase index
 *
 * @return the final state jacobian pattern for the given phase index
 *
 */
//------------------------------------------------------------------------------
Rmatrix JacobianData::GetFinalStateJacobianPattern(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return finalStateJacobianPattern.at(phaseIdx);
}

//------------------------------------------------------------------------------
// Rmatrix GetStaticJacobianPattern(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
* This method returns the static jacobian pattern for the given phase
* index
*
* @param <phaseIdx> the phase index
*
* @return the static jacobian pattern for the given phase index
*
*/
//------------------------------------------------------------------------------
Rmatrix JacobianData::GetStaticJacobianPattern(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return staticJacobianPattern.at(phaseIdx);
}

//------------------------------------------------------------------------------
// Rmatrix GetInitTimeJacobian(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the initial time jacobian for the given phase
 * index
 *
 * @param <phaseIdx> the phase index
 *
 * @return the initial time jacobian for the given phase index
 *
 */
//------------------------------------------------------------------------------
Rmatrix JacobianData::GetInitTimeJacobian(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return initTimeJacobian.at(phaseIdx);
}

//------------------------------------------------------------------------------
// Rmatrix GetFinalTimeJacobian(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the final time jacobian for the given phase
 * index
 *
 * @param <phaseIdx> the phase index
 *
 * @return the final time jacobian for the given phase index
 *
 */
//------------------------------------------------------------------------------
Rmatrix JacobianData::GetFinalTimeJacobian(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return finalTimeJacobian.at(phaseIdx);
}

//------------------------------------------------------------------------------
// Rmatrix GetInitStateJacobian(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the initial state jacobian for the given phase
 * index
 *
 * @param <phaseIdx> the phase index
 *
 * @return the initial state jacobian for the given phase index
 *
 */
//------------------------------------------------------------------------------
Rmatrix JacobianData::GetInitStateJacobian(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return initStateJacobian.at(phaseIdx);
}

//------------------------------------------------------------------------------
// Rmatrix GetFinalStateJacobian(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the final state jacobian for the given phase
 * index
 *
 * @param <phaseIdx> the phase index
 *
 * @return the final state jacobian for the given phase index
 *
 */
//------------------------------------------------------------------------------
Rmatrix JacobianData::GetFinalStateJacobian(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return finalStateJacobian.at(phaseIdx);
}

//------------------------------------------------------------------------------
// Rmatrix GetStaticJacobian(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
* This method returns the static jacobian for the given phase
* index
*
* @param <phaseIdx> the phase index
*
* @return the static jacobian for the given phase index
*
*/
//------------------------------------------------------------------------------
Rmatrix JacobianData::GetStaticJacobian(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return staticJacobian.at(phaseIdx);
}

//------------------------------------------------------------------------------
// IntegerArray GetInitStateIdxs(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns an array of initial state indexes
 *
 * @param <phaseIdx> the phase index
 *
 * @return array of initial state indexes
 *
 */
//------------------------------------------------------------------------------
IntegerArray JacobianData::GetInitStateIdxs(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return initialStateIdxs.at(phaseIdx);
}

//------------------------------------------------------------------------------
// IntegerArray GetFinalStateIdxs(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns an array of final state indexes
 *
 * @param <phaseIdx> the phase index
 *
 * @return array of final state indexes
 *
 */
//------------------------------------------------------------------------------
IntegerArray JacobianData::GetFinalStateIdxs(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return finalStateIdxs.at(phaseIdx);
}

//------------------------------------------------------------------------------
// IntegerArray GetStaticIdxs(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
* This method returns an array of static indexes
*
* @param <phaseIdx> the phase index
*
* @return array of static indexes
*
*/
//------------------------------------------------------------------------------
IntegerArray JacobianData::GetStaticIdxs(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return staticIdxs.at(phaseIdx);
}

//------------------------------------------------------------------------------
// IntegerArray GetInitTimeIdxs(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns an array of initial time indexes
 *
 * @param <phaseIdx> the phase index
 *
 * @return array of initial time indexes
 *
 */
//------------------------------------------------------------------------------
IntegerArray JacobianData::GetInitTimeIdxs(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return initialTimeIdxs.at(phaseIdx);
}

//------------------------------------------------------------------------------
// IntegerArray GetFinalTimeIdxs(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns an array of final time indexes
 *
 * @param <phaseIdx> the phase index
 *
 * @return array of final time indexes
 *
 */
//------------------------------------------------------------------------------
IntegerArray JacobianData::GetFinalTimeIdxs(Integer phaseIdx)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   return finalTimeIdxs.at(phaseIdx);
}

//------------------------------------------------------------------------------
// void SetInitTimeJacobian(Integer phaseIdx, Integer row, Integer col,
//                          Real val)
//------------------------------------------------------------------------------
/**
 * This method sets the initial time jacobian for the given phase at the given
 * (row, col) to the given value
 *
 * @param <phaseIdx> the phase index
 * @param <row>      the row
 * @param <col>      the columnd
 * @param <val>      the value
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetInitTimeJacobian(Integer phaseIdx,
                                       Integer row, Integer col, Real val)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   Rmatrix *mat = &initTimeJacobian.at(phaseIdx);
   Integer r, c;
   mat->GetSize(r,c);
   if (row < 0 || (row >= r) || (col < 0) || (col >= c))
      throw LowThrustException("Row or column out-of-bounds\n");
   mat->SetElement(row, col, val);
}

//------------------------------------------------------------------------------
// void SetFinalTimeJacobian(Integer phaseIdx,Integer row, Integer col,
//                           Real val)
//------------------------------------------------------------------------------
/**
 * This method sets the final time jacobian for the given phase at the given
 * (row, col) to the given value
 *
 * @param <phaseIdx> the phase index
 * @param <row>      the row
 * @param <col>      the columnd
 * @param <val>      the value
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetFinalTimeJacobian(Integer phaseIdx,
                                        Integer row, Integer col, Real val)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   Rmatrix *mat = &finalTimeJacobian.at(phaseIdx);
   Integer r, c;
   mat->GetSize(r,c);
   if (row < 0 || (row >= r) || (col < 0) || (col >= c))
      throw LowThrustException("Row or column out-of-bounds\n");
   mat->SetElement(row, col, val);
}

//------------------------------------------------------------------------------
// void SetInitStateJacobian(Integer phaseIdx, Integer row, Integer col,
//                           Real val)
//------------------------------------------------------------------------------
/**
 * This method sets the initial state jacobian for the given phase at the given
 * (row, col) to the given value
 *
 * @param <phaseIdx> the phase index
 * @param <row>      the row
 * @param <col>      the columnd
 * @param <val>      the value
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetInitStateJacobian(Integer phaseIdx,
                                        Integer row, Integer col, Real val)
{
   #ifdef DEBUG_JACOBIAN_DATA
      MessageInterface::ShowMessage(
            "SetInitStateJacobian: phaseIdx = %d, row = %d, col = %d, "
            "val = %12.10f, numPhases = %d\n",
            phaseIdx, row, col, val, numPhases);
   #endif
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   Rmatrix *mat = &initStateJacobian.at(phaseIdx);
   Integer r, c;
   mat->GetSize(r,c);
   if (row < 0 || (row >= r) || (col < 0) || (col >= c))
      throw LowThrustException("Row or column out-of-bounds\n");
   #ifdef DEBUG_JACOBIAN_DATA
      MessageInterface::ShowMessage(
                        "  ---> NOW setting row and col (%d, %d) to %12.10f\n",
                        row, col, val);
   #endif
   mat->SetElement(row, col, val);
   #ifdef DEBUG_JACOBIAN_DATA
      MessageInterface::ShowMessage(" LEAVING SetInitStateJacobian\n");
   #endif
}

//------------------------------------------------------------------------------
// void SetFinalStateJacobian(Integer phaseIdx, Integer row, Integer col,
//                            Real val)
//------------------------------------------------------------------------------
/**
 * This method sets the final state jacobian for the given phase at the given
 * (row, col) to the given value
 *
 * @param <phaseIdx> the phase index
 * @param <row>      the row
 * @param <col>      the columnd
 * @param <val>      the value
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetFinalStateJacobian(Integer phaseIdx,
                                         Integer row, Integer col, Real val)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   Rmatrix *mat = &finalStateJacobian.at(phaseIdx);
   Integer r, c;
   mat->GetSize(r,c);
   if (row < 0 || (row >= r) || (col < 0) || (col >= c))
      throw LowThrustException("Row or column out-of-bounds\n");
   mat->SetElement(row, col, val);
}

//------------------------------------------------------------------------------
// void SetStaticJacobian(Integer phaseIdx, Integer row, Integer col,
//                            Real val)
//------------------------------------------------------------------------------
/**
* This method sets the Static jacobian for the given phase at the given
* (row, col) to the given value
*
* @param <phaseIdx> the phase index
* @param <row>      the row
* @param <col>      the columnd
* @param <val>      the value
*
*/
//------------------------------------------------------------------------------
void JacobianData::SetStaticJacobian(Integer phaseIdx,
                                     Integer row, Integer col, Real val)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   Rmatrix *mat = &staticJacobian.at(phaseIdx);
   Integer r, c;
   mat->GetSize(r, c);
   if (row < 0 || (row >= r) || (col < 0) || (col >= c))
      throw LowThrustException("Row or column out-of-bounds\n");
   mat->SetElement(row, col, val);
}

//------------------------------------------------------------------------------
// void SetInitTimeJacobianPattern(Integer phaseIdx,
//                                 const Rmatrix &toPattern)
//------------------------------------------------------------------------------
/**
 * This method sets the initial time jacobian pattern for the given phase 
 * to the given value
 *
 * @param <phaseIdx>   the phase index
 * @param <toPattern>  the jacobian pattern
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetInitTimeJacobianPattern(Integer phaseIdx,
                                              const Rmatrix &toPattern)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   if (numPhases != (Integer) initTimeJacobianPattern.size())
      throw LowThrustException("initTimeJacobian has incorrect size\n");
   
   initTimeJacobianPattern.at(phaseIdx) = toPattern;
}

//------------------------------------------------------------------------------
// void SetFinalTimeJacobianPattern(Integer phaseIdx,
//                                 const Rmatrix &toPattern)
//------------------------------------------------------------------------------
/**
 * This method sets the final time jacobian pattern for the given phase to the 
 * given value
 *
 * @param <phaseIdx>   the phase index
 * @param <toPattern>  the jacobian pattern
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetFinalTimeJacobianPattern(Integer phaseIdx,
                                               const Rmatrix &toPattern)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   if (numPhases != (Integer) finalTimeJacobianPattern.size())
      throw LowThrustException("finalTimeJacobianPattern has incorrect size\n");

   finalTimeJacobianPattern.at(phaseIdx) = toPattern;
}

//------------------------------------------------------------------------------
// void SetInitStateJacobianPattern(Integer phaseIdx,
//                                 const Rmatrix &toPattern)
//------------------------------------------------------------------------------
/**
 * This method sets the initial state jacobian pattern for the given phase to 
 * the given value
 *
 * @param <phaseIdx>   the phase index
 * @param <toPattern>  the jacobian pattern
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetInitStateJacobianPattern(Integer phaseIdx,
                                               const Rmatrix &toPattern)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   initStateJacobianPattern.at(phaseIdx) = toPattern;
}

//------------------------------------------------------------------------------
// void SetFinalStateJacobianPattern(Integer phaseIdx,
//                                 const Rmatrix &toPattern)
//------------------------------------------------------------------------------
/**
 * This method sets the final state jacobian pattern for the given phase to the 
 * given value
 *
 * @param <phaseIdx>   the phase index
 * @param <toPattern>  the jacobian pattern
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetFinalStateJacobianPattern(Integer phaseIdx,
                                                const Rmatrix &toPattern)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   finalStateJacobianPattern.at(phaseIdx) = toPattern;
}

//------------------------------------------------------------------------------
// void SetStaticJacobianPattern(Integer phaseIdx,
//                                 const Rmatrix &toPattern)
//------------------------------------------------------------------------------
/**
* This method sets the Static jacobian pattern for the given phase to the
* given value
*
* @param <phaseIdx>   the phase index
* @param <toPattern>  the jacobian pattern
*
*/
//------------------------------------------------------------------------------
void JacobianData::SetStaticJacobianPattern(Integer phaseIdx,
                                            const Rmatrix &toPattern)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   staticJacobianPattern.at(phaseIdx) = toPattern;
}

//------------------------------------------------------------------------------
// void SetInitTimeJacobianPattern(Integer phaseIdx,
//                                 Integer row, Integer col,
//                                 Real val)
//------------------------------------------------------------------------------
/**
 * This method sets the initial time jacobian pattern for the given phase at
 * the given (row, col) to the given value
 *
 * @param <phaseIdx>   the phase index
 * @param <row>        the row
 * @param <col>        the column
 * @param <val>        the value
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetInitTimeJacobianPattern(Integer phaseIdx,
                                     Integer row, Integer col, 
                                     Real val)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   Rmatrix *mat = &initTimeJacobianPattern.at(phaseIdx);
   Integer r, c;
   mat->GetSize(r,c);
   if (row < 0 || (row >= r) || (col < 0) || (col >= c))
      throw LowThrustException("Row or column out-of-bounds\n");
   mat->SetElement(row, col, val);
}

//------------------------------------------------------------------------------
// void SetFinalTimeJacobianPattern(Integer phaseIdx,
//                                  Integer row, Integer col,
//                                  Real val)
//------------------------------------------------------------------------------
/**
 * This method sets the final time jacobian pattern for the given phase at
 * the given (row, col) to the given value
 *
 * @param <phaseIdx>   the phase index
 * @param <row>        the row
 * @param <col>        the column
 * @param <val>        the value
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetFinalTimeJacobianPattern(Integer phaseIdx,
                                      Integer row, Integer col, 
                                      Real val)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   Rmatrix *mat = &finalTimeJacobianPattern.at(phaseIdx);
   Integer r, c;
   mat->GetSize(r,c);
   if (row < 0 || (row >= r) || (col < 0) || (col >= c))
      throw LowThrustException("Row or column out-of-bounds\n");
   mat->SetElement(row, col, val);
}

//------------------------------------------------------------------------------
// void SetInitStateJacobianPattern(Integer phaseIdx,
//                                  Integer row, Integer col,
//                                  Real val)
//------------------------------------------------------------------------------
/**
 * This method sets the initial state jacobian pattern for the given phase at
 * the given (row, col) to the given value
 *
 * @param <phaseIdx>   the phase index
 * @param <row>        the row
 * @param <col>        the column
 * @param <val>        the value
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetInitStateJacobianPattern(Integer phaseIdx,
                                      Integer row, Integer col, 
                                      Real val)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   Rmatrix *mat = &initStateJacobianPattern.at(phaseIdx);
   Integer r, c;
   mat->GetSize(r,c);
   if (row < 0 || (row >= r) || (col < 0) || (col >= c))
      throw LowThrustException("Row or column out-of-bounds\n");
   mat->SetElement(row, col, val);
}

//------------------------------------------------------------------------------
// void SetFinalStateJacobianPattern(Integer phaseIdx,
//                                   Integer row, Integer col,
//                                   Real val)
//------------------------------------------------------------------------------
/**
 * This method sets the final state jacobian pattern for the given phase at
 * the given (row, col) to the given value
 *
 * @param <phaseIdx>   the phase index
 * @param <row>        the row
 * @param <col>        the column
 * @param <val>        the value
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetFinalStateJacobianPattern(Integer phaseIdx,
                                       Integer row, Integer col, 
                                       Real val)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   Rmatrix *mat = &finalStateJacobianPattern.at(phaseIdx);
   Integer r, c;
   mat->GetSize(r,c);
   if (row < 0 || (row >= r) || (col < 0) || (col >= c))
      throw LowThrustException("Row or column out-of-bounds\n");
   mat->SetElement(row, col, val);
}

//------------------------------------------------------------------------------
// void SetStaticJacobianPattern(Integer phaseIdx,
//                                   Integer row, Integer col,
//                                   Real val)
//------------------------------------------------------------------------------
/**
* This method sets the Static jacobian pattern for the given phase at
* the given (row, col) to the given value
*
* @param <phaseIdx>   the phase index
* @param <row>        the row
* @param <col>        the column
* @param <val>        the value
*
*/
//------------------------------------------------------------------------------
void JacobianData::SetStaticJacobianPattern(Integer phaseIdx,
                                            Integer row, Integer col,
                                            Real val)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   Rmatrix *mat = &staticJacobianPattern.at(phaseIdx);
   Integer r, c;
   mat->GetSize(r, c);
   if (row < 0 || (row >= r) || (col < 0) || (col >= c))
      throw LowThrustException("Row or column out-of-bounds\n");
   mat->SetElement(row, col, val);
}

//------------------------------------------------------------------------------
// void SetInitialTimeDependency(Integer phaseIdx, bool hasDepend)
//------------------------------------------------------------------------------
/**
 * This method sets the initial time dependency flag for the given phase index
 *
 * @param <phaseIdx>   the phase index
 * @param <hasDepend>  the dependency flag
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetInitialTimeDependency(Integer phaseIdx, bool hasDepend)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   hasInitTimeDependency.at(phaseIdx) = hasDepend;
}

//------------------------------------------------------------------------------
// void SetFinalTimeDependency(Integer phaseIdx, bool hasDepend)
//------------------------------------------------------------------------------
/**
 * This method sets the final time dependency flag for the given phase index
 *
 * @param <phaseIdx>   the phase index
 * @param <hasDepend>  the dependency flag
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetFinalTimeDependency(Integer phaseIdx, bool hasDepend)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   hasFinalTimeDependency.at(phaseIdx) = hasDepend;
}

//------------------------------------------------------------------------------
// void SetInitialStateDependency(Integer phaseIdx, bool hasDepend)
//------------------------------------------------------------------------------
/**
 * This method sets the initial state dependency flag for the given phase index
 *
 * @param <phaseIdx>   the phase index
 * @param <hasDepend>  the dependency flag
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetInitialStateDependency(Integer phaseIdx, bool hasDepend)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   hasInitStateDependency.at(phaseIdx) = hasDepend;
}

//------------------------------------------------------------------------------
// void SetFinalStateDependency(Integer phaseIdx, bool hasDepend)
//------------------------------------------------------------------------------
/**
 * This method sets the final state dependency flag for the given phase index
 *
 * @param <phaseIdx>   the phase index
 * @param <hasDepend>  the dependency flag
 *
 */
//------------------------------------------------------------------------------
void JacobianData::SetFinalStateDependency(Integer phaseIdx, bool hasDepend)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   hasFinalStateDependency.at(phaseIdx) = hasDepend;
}

//------------------------------------------------------------------------------
// void SetStaticDependency(Integer phaseIdx, bool hasDepend)
//------------------------------------------------------------------------------
/**
* This method sets the Static dependency flag for the given phase index
*
* @param <phaseIdx>   the phase index
* @param <hasDepend>  the dependency flag
*
*/
//------------------------------------------------------------------------------
void JacobianData::SetStaticDependency(Integer phaseIdx, bool hasDepend)
{
   if (phaseIdx < 0 || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds\n");
   hasStaticDependency.at(phaseIdx) = hasDepend;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void CopyArrays(const JacobianData &jd)
//------------------------------------------------------------------------------
/**
 * This method copies the array values to those of the input JacobianData
 *
 * @param <jd>   jacobian data to copy
 *
 */
//------------------------------------------------------------------------------
void JacobianData::CopyArrays(const JacobianData &copy)
{
   // Copy list of Phases
   phaseList.clear();
   for (UnsignedInt ii = 0; ii < copy.phaseList.size(); ii++)
      phaseList.push_back(copy.phaseList.at(ii));

   // Copy list of initiali indexes
   decVecStartIdxs.clear();
   for (UnsignedInt ii = 0; ii < copy.decVecStartIdxs.size(); ii++)
      decVecStartIdxs.push_back(copy.decVecStartIdxs.at(ii));

   // Copy the Jacobians
   initTimeJacobian.clear();
   for (UnsignedInt ii = 0; ii < copy.initTimeJacobian.size(); ii++)
      initTimeJacobian.push_back(copy.initTimeJacobian.at(ii));
   initStateJacobian.clear();
   for (UnsignedInt ii = 0; ii < copy.initStateJacobian.size(); ii++)
      initStateJacobian.push_back(copy.initStateJacobian.at(ii));
   finalTimeJacobian.clear();
   for (UnsignedInt ii = 0; ii < copy.finalTimeJacobian.size(); ii++)
      finalTimeJacobian.push_back(copy.finalTimeJacobian.at(ii));
   finalStateJacobian.clear();
   for (UnsignedInt ii = 0; ii < copy.finalStateJacobian.size(); ii++)
      finalStateJacobian.push_back(copy.finalStateJacobian.at(ii));
   staticJacobian.clear();
   for (UnsignedInt ii = 0; ii < copy.staticJacobian.size(); ii++)
      staticJacobian.push_back(copy.staticJacobian.at(ii));
   
   // Copy the Jacobian patterns
   initTimeJacobianPattern.clear();
   for (UnsignedInt ii = 0; ii < copy.initTimeJacobianPattern.size(); ii++)
      initTimeJacobianPattern.push_back(copy.initTimeJacobianPattern.at(ii));
   initStateJacobianPattern.clear();
   for (UnsignedInt ii = 0; ii < copy.initStateJacobianPattern.size(); ii++)
      initStateJacobianPattern.push_back(copy.initStateJacobianPattern.at(ii));
   finalTimeJacobianPattern.clear();
   for (UnsignedInt ii = 0; ii < copy.finalTimeJacobianPattern.size(); ii++)
      finalTimeJacobianPattern.push_back(copy.finalTimeJacobianPattern.at(ii));
   finalStateJacobianPattern.clear();
   for (UnsignedInt ii = 0; ii < copy.finalStateJacobianPattern.size(); ii++)
      finalStateJacobianPattern.push_back(
                                    copy.finalStateJacobianPattern.at(ii));
   staticJacobianPattern.clear();
   for (UnsignedInt ii = 0; ii < copy.staticJacobianPattern.size(); ii++)
      staticJacobianPattern.push_back(copy.staticJacobianPattern.at(ii));

   // Copy the boolean arrays
   hasInitTimeDependency.clear();
   for (UnsignedInt ii = 0; ii < copy.hasInitTimeDependency.size(); ii++)
      hasInitTimeDependency.push_back(copy.hasInitTimeDependency.at(ii));
   hasInitStateDependency.clear();
   for (UnsignedInt ii = 0; ii < copy.hasInitStateDependency.size(); ii++)
      hasInitStateDependency.push_back(copy.hasInitStateDependency.at(ii));
   hasFinalTimeDependency.clear();
   for (UnsignedInt ii = 0; ii < copy.hasFinalTimeDependency.size(); ii++)
      hasFinalTimeDependency.push_back(copy.hasFinalTimeDependency.at(ii));
   hasFinalStateDependency.clear();
   for (UnsignedInt ii = 0; ii < copy.hasFinalStateDependency.size(); ii++)
      hasFinalStateDependency.push_back(copy.hasFinalStateDependency.at(ii));
   hasStaticDependency.clear();
   for (UnsignedInt ii = 0; ii < copy.hasStaticDependency.size(); ii++)
      hasStaticDependency.push_back(copy.hasStaticDependency.at(ii));

   // Copy the indexes
   initialStateIdxs.clear();
   for (UnsignedInt ii = 0; ii < copy.initialStateIdxs.size(); ii++)
      initialStateIdxs.push_back(copy.initialStateIdxs.at(ii));
   finalStateIdxs.clear();
   for (UnsignedInt ii = 0; ii < copy.finalStateIdxs.size(); ii++)
      finalStateIdxs.push_back(copy.finalStateIdxs.at(ii));
   initialTimeIdxs.clear();
   for (UnsignedInt ii = 0; ii < copy.initialTimeIdxs.size(); ii++)
      initialTimeIdxs.push_back(copy.initialTimeIdxs.at(ii));
   finalTimeIdxs.clear();
   for (UnsignedInt ii = 0; ii < copy.finalTimeIdxs.size(); ii++)
      finalTimeIdxs.push_back(copy.finalTimeIdxs.at(ii));   
   staticIdxs.clear();
   for (UnsignedInt ii = 0; ii < copy.staticIdxs.size(); ii++)
      staticIdxs.push_back(copy.staticIdxs.at(ii));
}
