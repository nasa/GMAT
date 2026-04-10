//------------------------------------------------------------------------------
//                              DecVecTypeBetts
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
// Created: 2015.06.18
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 *
 * DecVecTypeBetts DecisionVector organized similar to that used by
 * Bett's.
 *  Z = [t_o t_f y_10 u_10 y_11 u_11 ... y_nm u_nm s_1 .. s_o w_1..w_p]
 *
 * y (mesh index, stage index)
 * for radau numStages = 0
 
 * state control (y) ... state control (u)
 * s_1 ... s_p not implemented  (or is it? - not called yet)
 * w_1 ... w_p not implemented  (or is it? - not called yet)
 *
 *
 */
//------------------------------------------------------------------------------

#include "DecVecTypeBetts.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SET
//#define DEBUG_GET
//#define DEBUG_INIT

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
DecVecTypeBetts::DecVecTypeBetts() :
   DecisionVector(),
   numParamsPerMesh         (0),
   numStateAndControlVars   (0),
   numStagePoints           (0),
   hasControlAtFinalMesh    (false)
{
   
}


//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
DecVecTypeBetts::DecVecTypeBetts(const DecVecTypeBetts &copy) :
   DecisionVector(copy),
   numParamsPerMesh         (copy.numParamsPerMesh),
   numStateAndControlVars   (copy.numStateAndControlVars),
   numStagePoints           (copy.numStagePoints),
   hasControlAtFinalMesh    (copy.hasControlAtFinalMesh)
{
   
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
DecVecTypeBetts& DecVecTypeBetts::operator=(const DecVecTypeBetts &copy)
{
   
   if (&copy == this)
      return *this;
   
   DecisionVector::operator=(copy);
   
   numParamsPerMesh        = copy.numParamsPerMesh;
   numStateAndControlVars  = copy.numStateAndControlVars;
   numStagePoints          = copy.numStagePoints;
   hasControlAtFinalMesh   = copy.hasControlAtFinalMesh;
   
    return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
DecVecTypeBetts::~DecVecTypeBetts()
{
   // nothing to do here
}


//------------------------------------------------------------------------------
//  IntegerArray GetStateIdxsAtMeshPoint(Integer meshIdx, Integer stageIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the state indexes at the given mesh and stage points
 *
 * @param <meshIdx>      mesh point
 * @param <stageIdx>     stage point
 *
 * @return array of state indexes at the given mesh and stage point
 *
 */
//------------------------------------------------------------------------------
IntegerArray DecVecTypeBetts::GetStateIdxsAtMeshPoint(Integer meshIdx,
                                                      Integer stageIdx)
{
   #ifdef DEBUG_SET
      MessageInterface::ShowMessage("GetStateIdxsAtMeshPoint ...\n");
      MessageInterface::ShowMessage("   meshIdx        = %d\n", meshIdx);
      MessageInterface::ShowMessage("   stageIdx       = %d\n", stageIdx);
      MessageInterface::ShowMessage("   timeStopIdx    = %d\n", timeStopIdx);
      MessageInterface::ShowMessage("   numStagePoints = %d\n", numStagePoints);
      MessageInterface::ShowMessage("   numStateVars   = %d\n", numStateVars);
      MessageInterface::ShowMessage("   numStateAndControlVars = %d\n",
                                    numStateAndControlVars);
   #endif
   // Extract state param indeces at mesh point and stage point
   ValidateMeshStageIndeces(meshIdx,stageIdx);
   Integer indStart = timeStopIdx +
              (meshIdx) * (numStagePoints + 1) *
              numStateAndControlVars +
              (stageIdx) * numStateAndControlVars + 1;
   Integer indStop  = indStart + numStateVars - 1;
   #ifdef DEBUG_SET
      MessageInterface::ShowMessage("   indStart     = %d\n", indStart);
      MessageInterface::ShowMessage("   indStop      = %d\n", indStop);
   #endif
   IntegerArray idxs;
   for (Integer ii = indStart; ii <= indStop; ii++)
      idxs.push_back(ii);
   return idxs;
}

//------------------------------------------------------------------------------
// IntegerArray GetControlIdxsAtMeshPoint(Integer meshIdx, Integer stageIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the control indexes at the given mesh and stage points
 *
 * @param <meshIdx>      mesh point
 * @param <stageIdx>     stage point
 *
 * @return array of control indexes at the given mesh and stage point
 *
 */
//------------------------------------------------------------------------------
IntegerArray DecVecTypeBetts::GetControlIdxsAtMeshPoint(Integer meshIdx,
                                                        Integer stageIdx)
{
   // Extract control param indeces at mesh point and stage point
   ValidateMeshStageIndeces(meshIdx,stageIdx);
   Integer indStart = timeStopIdx + (meshIdx) * (numStagePoints + 1) *
              numStateAndControlVars + (stageIdx) * numStateAndControlVars +
              numStateVars + 1;
   Integer indStop  = indStart + numControlVars - 1;
   IntegerArray idxs;
   for (Integer ii = indStart; ii <= indStop; ii++)
      idxs.push_back(ii);
   return idxs;
}


//------------------------------------------------------------------------------
// IntegerArray GetStaticIdxs()
//------------------------------------------------------------------------------
/**
* This method returns the static indexes; YK mod static vars
*
* @return array of static indexes 
*
*/
//------------------------------------------------------------------------------
IntegerArray DecVecTypeBetts::GetStaticIdxs()
{  
 
   IntegerArray idxs;
   // if there exists static vars, return idxs
   if (staticStartIdx <= staticStopIdx)
   {
      for (Integer ii = staticStartIdx; ii <= staticStopIdx; ii++)
         idxs.push_back(ii);
   }
   else
      idxs.push_back(-1);
   
   return idxs;
}


//------------------------------------------------------------------------------
// bool SetStateVector(Integer meshIdx,
//                     Integer stageIdx, const Rvector &stateVec)
//------------------------------------------------------------------------------
/**
 * This method sets the state vector for the input mesh and stage index
 *
 * @param <meshIdx>      mesh point
 * @param <stageIdx>     stage point
 * @param >stateVec>     state vector value
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool DecVecTypeBetts::SetStateVector(Integer meshIdx,Integer stageIdx,
                                     const Rvector &stateVec)
{
   // Given mesh, state and stage, insert into decision vector
   IntegerArray idxs = GetStateIdxsAtMeshPoint(meshIdx, stageIdx);
   #ifdef DEBUG_SET
      MessageInterface::ShowMessage("SetStateVector ...\n");
      MessageInterface::ShowMessage("... meshIdx = %d, stageIdx %d\n",
                                    meshIdx, stageIdx);
      MessageInterface::ShowMessage("... and state idxs (%d) are:\n",
                                    (Integer) idxs.size());
      for (Integer ii = 0; ii < (Integer) idxs.size(); ii++)
         MessageInterface::ShowMessage("   %d\n", idxs.at(ii));
   #endif
   Integer sz = stateVec.GetSize();
   for (Integer ii = 0; ii < sz; ii++)
      decisionVector[idxs[ii]] = stateVec[ii];
   return true;
}

//------------------------------------------------------------------------------
// Rvector GetStateVector(Integer meshIdx, Integer stageIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the state vector at the input mesh and stage index
 *
 * @param <meshIdx>      mesh point
 * @param <stageIdx>     stage point
 *
 * @return state vector at the input mesh and stage indexes
 *
 */
//------------------------------------------------------------------------------
Rvector DecVecTypeBetts::GetStateVector(Integer meshIdx, Integer stageIdx)
{
   // Given state and stage indeces, extract state from dec. vec.
   IntegerArray idxs = GetStateIdxsAtMeshPoint(meshIdx, stageIdx);
   Integer sz = idxs.size();
   #ifdef DEBUG_GET
      MessageInterface::ShowMessage("GetStateVector ...\n");
      MessageInterface::ShowMessage("   meshIdx   = %d\n", meshIdx);
      MessageInterface::ShowMessage("   stageIdx  = %d\n", stageIdx);
      MessageInterface::ShowMessage("   sz        = %d\n", sz);
   #endif
   Rvector sVector(sz);
   for (Integer ii = 0; ii < sz; ii++)
      sVector[ii] = decisionVector[idxs[ii]];

   return sVector;
}

//------------------------------------------------------------------------------
// void SetStateArray(const Rmatrix &sArray)
//------------------------------------------------------------------------------
/**
 * This method sets the state array
 *
 * @param <sArray>      state array
 *
 */
//------------------------------------------------------------------------------
void DecVecTypeBetts::SetStateArray(const Rmatrix &sArray)
{
   // Given a state array, insert states in the decision vector
   #ifdef DEBUG_SET
      MessageInterface::ShowMessage("SetStateArray ...\n");
   #endif
   
   Integer row, col;
   sArray.GetSize(row, col);
   if (row != numStatePoints || col != numStateVars)
   {
      {
         std::string errmsg = "For DecVecTypeBetts::SetStateArray, ";
         errmsg += "state Array is not valid dimension\n";
         throw LowThrustException(errmsg);
      }
   }
   #ifdef DEBUG_SET
      MessageInterface::ShowMessage(
                        "   row (%d) and col (%d) are as expected\n",
                        row, col);
      MessageInterface::ShowMessage("   numStateMeshPoints = %d\n",
                                    numStateMeshPoints);
      MessageInterface::ShowMessage("   numStateStagePoints = %d\n",
                                    numStateStagePoints);
   #endif
   
   // Loop through the matrix setting and insert into dec. vec.
   Integer rowIdx = 0;
   for (Integer mm = 0; mm < numStateMeshPoints-1; mm++)
   {
      for (Integer ss = 0; ss <= numStateStagePoints; ss++)
      {
         IntegerArray idxs = GetStateIdxsAtMeshPoint(mm, ss);
         for (Integer ii = 0; ii < numStateVars; ii++)
         {
            #ifdef DEBUG_SET
               MessageInterface::ShowMessage(
                                 "   Setting decisionVector(%d) to %12.10f\n",
                                 (idxs[ii]), sArray(rowIdx, ii));
            #endif
            decisionVector(idxs[ii]) = sArray(rowIdx, ii);
         }
         rowIdx++;
      }
   }
   // Set the last mesh point - why isn't this part of the above loop???
   IntegerArray idxsFinal = GetStateIdxsAtMeshPoint(numStateMeshPoints-1, 0);
   for (Integer ii = 0; ii < numStateVars; ii++)
   {
      #ifdef DEBUG_SET
            MessageInterface::ShowMessage(
                              "   Setting decisionVector(%d) to %12.10f\n",
                              (idxsFinal[ii]), sArray(rowIdx, ii));
      #endif
      decisionVector(idxsFinal[ii]) = sArray(rowIdx, ii);
   }
}

//------------------------------------------------------------------------------
// Rmatrix  GetStateArray()
//------------------------------------------------------------------------------
/**
 * This method returns the state array
 *
 * @return the state array
 *
 * NOTE:
 *
 * stateArray is numStatePoints x numStateVars
 *
 * stateArray = [X1 Y1 Z1 VX1 VY1 VZ1 M1]
 *              [X2 Y2 Z2 VX2 VY2 VZ2 M2]
 *              [XN YN ZN VXN VYN VZN M1]
 *
 * Loop through the mesh and stages and populate the array
 * stateArray = zeros(obj.numStatePoints,obj.numStateVars);
 */
//------------------------------------------------------------------------------
Rmatrix DecVecTypeBetts::GetStateArray()
{
   // Get the array of state params for all mesh/stage points
   Rmatrix stateArray(numStatePoints,numStateVars);
   Integer rowIdx     = 0;
   for (Integer mm = 0; mm < numStateMeshPoints-1; mm++)
   {
      for (Integer ss = 0; ss <= numStateStagePoints; ss++)
      {
         IntegerArray idxs = GetStateIdxsAtMeshPoint(mm, ss);
         for (Integer stateIdx = 0;stateIdx < numStateVars;stateIdx++)
         {
            Integer colIdx = idxs[stateIdx];
            stateArray(rowIdx, stateIdx) = decisionVector(colIdx);
         }
         rowIdx++;  
      }
   }
   
   // Set the last mesh point
   IntegerArray idxsFinal = GetStateIdxsAtMeshPoint(numStateMeshPoints-1, 0);
   for (Integer stateIdx = 0;stateIdx < numStateVars;stateIdx++)
   {
      Integer colIdx = idxsFinal[stateIdx];
      stateArray(rowIdx, stateIdx) = decisionVector(colIdx);
   }
   return stateArray;
}


//------------------------------------------------------------------------------
// bool SetControlVector(Integer meshIdx,Integer stageIdx,
//                       const Rvector &controlVec)
//------------------------------------------------------------------------------
/**
 * This method sets the control vector at the input mesh and stage indexes
 *
 * @param <meshIdx>      mesh point
 * @param <stageIdx>     stage point
 * @param <controlVec>   control vector
 *
 * @return true if successful; false, otherwise
 */
//------------------------------------------------------------------------------
bool DecVecTypeBetts::SetControlVector(Integer meshIdx,Integer stageIdx,
                                       const Rvector &controlVec)
{
   if (controlVec.GetSize() != numControlVars)
   {
      std::string errmsg = "For DecVecTypeBetts::SetControlVector, ";
      errmsg += "control vector is not valid dimension\n";
      throw LowThrustException(errmsg);
   }
   // Given mesh, state and stage, insert into decision vector
   IntegerArray idxs = GetControlIdxsAtMeshPoint(meshIdx,  stageIdx);
   for (Integer ii = 0; ii < numControlVars; ii++)
      decisionVector(idxs[ii]) = controlVec(ii);
   
   return true;
}


//------------------------------------------------------------------------------
// Rvector GetControlVector(Integer meshIdx, Integer stageIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the control vector at the input mesh and stage indexes
 *
 * @param <meshIdx>      mesh point
 * @param <stageIdx>     stage point
 *
 * @return the control vector
 */
//------------------------------------------------------------------------------
Rvector DecVecTypeBetts::GetControlVector(Integer meshIdx, Integer stageIdx)
{
   Rvector controlVector;
   // Given state and stage indeces, extract state from dec. vec.
   IntegerArray idxs = GetControlIdxsAtMeshPoint(meshIdx,  stageIdx);
   controlVector.SetSize(idxs.size());
   for (Integer ii = 0; ii < numControlVars; ii++)
      controlVector(ii) = decisionVector(idxs[ii]);
   return controlVector;
}


//------------------------------------------------------------------------------
// void SetControlArray(const Rmatrix &cArray)
//------------------------------------------------------------------------------
/**
 * This method sets the control array
 *
 * @param <cArray>      control array
 *
 */
//------------------------------------------------------------------------------
void DecVecTypeBetts::SetControlArray(const Rmatrix &cArray)
{
   // Given a control array, insert Controls in the decision vector
   if (numControlVars == 0)
      return;
   
   Integer r,c;
   cArray.GetSize(r,c);
   if ((r != numControlPoints) || (c != numControlVars))
   {
      std::string errmsg = "For DecVecTypeBetts::SetControlArray, ";
      errmsg += "Control Array is not valid dimension\n";
      throw LowThrustException(errmsg);
   }

   // Loop through the matrix setting and insert into dec. vec.
   Integer rowIdx = 0;
   Integer numMeshLoops = 0;
   if (hasControlAtFinalMesh)
      numMeshLoops = numControlMeshPoints - 1;
   else
      numMeshLoops = numControlMeshPoints;

   for (Integer mm = 0; mm < numMeshLoops; mm++)
   {
      for (Integer ss = 0; ss <= numControlStagePoints; ss++)
      {
         IntegerArray idxs = GetControlIdxsAtMeshPoint(mm, ss);
         for (Integer cc = 0; cc < numControlVars; cc++)
            decisionVector(idxs[cc]) = cArray(rowIdx, cc);
         rowIdx++;
      }
   }

   // Set the last mesh point
   if (hasControlAtFinalMesh)
   {
      IntegerArray idxsFinal = GetControlIdxsAtMeshPoint(numMeshLoops,0);
      for (Integer cc = 0; cc < numControlVars; cc++)
         decisionVector(idxsFinal[cc]) = cArray(rowIdx, cc);
   }
}


//------------------------------------------------------------------------------
// Rmatrix GetControlArray()
//------------------------------------------------------------------------------
/**
 * This method returns the control array
 *
 * @return  the control array
 *
 * NOTE: 
 * ControlArray is numControlPoints x numControlVars
 *
 *  Build a dummy W matrix in Eq. 41.
 *  ControlArray = [Ux1 Uy1 Uz1]
 *                 [Ux2 Uy2 Uz2]
 *                 [UxN UyN UzN]
 *
 */
//------------------------------------------------------------------------------
Rmatrix DecVecTypeBetts::GetControlArray()
{
   // Get the array of Control params for all mesh/stage points
   Rmatrix controlArray(numControlPoints, numControlVars);
   Integer numMeshLoops = 0;
   // Loop through the mesh and stages and populate the array
   if (hasControlAtFinalMesh)
      numMeshLoops = numControlMeshPoints - 1;
   else
      numMeshLoops = numControlMeshPoints;

   Integer rowIdx = 0;
   for (Integer mm = 0; mm < numMeshLoops; mm++)
   {
      for (Integer ss = 0; ss <= numControlStagePoints; ss++)
      {
         IntegerArray idxs = GetControlIdxsAtMeshPoint(mm, ss);
         for (Integer cc = 0; cc < numControlVars; cc++)
         {
            Integer colIdx = idxs[cc];
            controlArray(rowIdx, cc) = decisionVector(colIdx);
         }
         rowIdx++;
      }
   }

   // Set the last mesh point
   if (hasControlAtFinalMesh)
   {
      IntegerArray idxsFinal = GetControlIdxsAtMeshPoint(numMeshLoops,0);
      for (Integer cc = 0; cc < numControlVars; cc++)
      {
         Integer colIdx = idxsFinal[cc];
         controlArray(rowIdx, cc) = decisionVector(colIdx);
      }
      rowIdx++;
   }
   return controlArray;
}

//------------------------------------------------------------------------------
// IntegerArray GetFinalStateIdxs()
//------------------------------------------------------------------------------
/**
 * This method returns the array of final state idxs
 *
 * @return  array of final state indexes
 *
 */
//------------------------------------------------------------------------------
IntegerArray DecVecTypeBetts::GetFinalStateIdxs()
{
   return GetStateIdxsAtMeshPoint(numStateMeshPoints-1,0);
}

//------------------------------------------------------------------------------
// IntegerArray GetInitialStateIdxs(Integer &indStart, Integer &indStop)
//------------------------------------------------------------------------------
/**
 * This method returns the array of initial state idxs
 *
 * @return  array of initial state indexes
 *
 */
//------------------------------------------------------------------------------
IntegerArray DecVecTypeBetts::GetInitialStateIdxs()
{
   return GetStateIdxsAtMeshPoint(0 ,0);
}

//------------------------------------------------------------------------------
// Integer GetFinalTimeIdx()
//------------------------------------------------------------------------------
/**
 * This method returns final time index
 *
 * @return  final time index
 *
 */
//------------------------------------------------------------------------------
Integer DecVecTypeBetts::GetFinalTimeIdx()
{
   return 1;
}

//------------------------------------------------------------------------------
// Integer GetInitialTimeIdx()
//------------------------------------------------------------------------------
/**
 * This method returns initial time index
 *
 * @return  initial time index
 *
 */
//------------------------------------------------------------------------------
Integer DecVecTypeBetts::GetInitialTimeIdx()
{
   return 0;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void SetChunkIndeces()
//------------------------------------------------------------------------------
/**
 * This method sets the chunk indexes
 *
 */
//------------------------------------------------------------------------------
void DecVecTypeBetts::SetChunkIndeces()  // inherited
{
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("Entering SetChunkIndeces ...\n");
   #endif
   //  Compute the start and stop indeces of chunks
   
   
   numStatePoints = (numStateMeshPoints-1)*
                    (1+numStateStagePoints) + 1;
   if (numStateMeshPoints == numControlMeshPoints)
   {
      hasControlAtFinalMesh = true;
      numControlPoints = (numControlMeshPoints-1)*
                         (1 + numControlStagePoints) + 1;
   }
   else
   {
      hasControlAtFinalMesh = false;
      numControlPoints = (numControlMeshPoints)*
                         (1 + numControlStagePoints);
   }
   
   
   // @todo  Add error checking.  numControlMeshPoints must be
   // either ????????????
   numStatePointsPerMesh   = 1 + numStateStagePoints;
   numControlPointsPerMesh = 1 + numControlStagePoints;
   numStateParams          = numStateVars * numStatePoints;
   numControlParams        = numControlVars * numControlPoints;
   numDecisionParams       = numStateParams +
                             numControlVars * numControlPoints +
                             numIntegralParams + numStaticParams + 2;
   
   timeStartIdx            = 0;
   timeStopIdx             = timeStartIdx +  1;
   //
   staticStartIdx          = 1 + numStateParams +
                             numControlParams + 1;
   staticStopIdx           = staticStartIdx +
                             numStaticParams - 1;
   
   integralStartIdx        = staticStopIdx + 1;
   integralStopIdx         = integralStartIdx +
                             numIntegralParams - 1;
   
   numParamsPerMesh = (numStateMeshPoints + numStateStagePoints) *
                      numStateVars +
                      (numControlMeshPoints + numControlStagePoints) *
                      numControlVars;
   numStateAndControlVars = numStateVars + numControlVars;
   numStagePoints         = numStateStagePoints;
   if (numStateStagePoints != numControlStagePoints)
   {
      std::string errmsg = "For DecVecTypeBetts::SetChunkIndeces, ";
      errmsg += "numStateStagePoints must be equal to numControlStagePoints\n";
      throw LowThrustException(errmsg);
   }
   
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("Leaving SetChunkIndeces:\n");
      MessageInterface::ShowMessage("   numStatePoints          = %d\n",
                                    numStatePoints);
      MessageInterface::ShowMessage("   numControlPoints        = %d\n",
                                    numControlPoints);
      MessageInterface::ShowMessage("   numStatePointsPerMesh   = %d\n",
                                    numStatePointsPerMesh);
      MessageInterface::ShowMessage("   numControlPointsPerMesh = %d\n",
                                    numControlPointsPerMesh);
      MessageInterface::ShowMessage("   numStateParams          = %d\n",
                                    numStateParams);
      MessageInterface::ShowMessage("   numControlParams        = %d\n",
                                    numControlParams);
      MessageInterface::ShowMessage("   numDecisionParams       = %d\n",
                                    numDecisionParams);
      MessageInterface::ShowMessage("   timeStartIdx            = %d\n",
                                    timeStartIdx);
      MessageInterface::ShowMessage("   timeStopIdx             = %d\n",
                                    timeStopIdx);
      MessageInterface::ShowMessage("   staticStartIdx          = %d\n",
                                    staticStartIdx);
      MessageInterface::ShowMessage("   staticStopIdx           = %d\n",
                                    staticStopIdx);
      MessageInterface::ShowMessage("   integralStartIdx        = %d\n",
                                    integralStartIdx);
      MessageInterface::ShowMessage("   integralStopIdx         = %d\n",
                                    integralStopIdx);
      MessageInterface::ShowMessage("   numParamsPerMesh        = %d\n",
                                    numParamsPerMesh);
      MessageInterface::ShowMessage("   numStateAndControlVars  = %d\n",
                                    numStateAndControlVars);
      MessageInterface::ShowMessage("   numStagePoints          = %d\n",
                                    numStagePoints);
   #endif
}

//------------------------------------------------------------------------------
// bool ValidateMeshStageIndeces(Integer meshIdx, Integer stageIdx)
//------------------------------------------------------------------------------
/**
 * This method validates the mesh and stage indexes
 *
 * @param <meshIdx>  the mesh index
 * @param <stageIdx> the stage index
 *
 * @return true if validation passes; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool DecVecTypeBetts::ValidateMeshStageIndeces(Integer meshIdx,
                                               Integer stageIdx)
{
   if (stageIdx  > numStagePoints + 1)
   {
      std::string errmsg = "For DecVecTypeBetts::ValidateMeshStageIndeces, ";
      errmsg += "stageIdx must be less than numStagePoints + 1\n";
      throw LowThrustException(errmsg);
   }

   if (meshIdx  > numStateMeshPoints) 
   {
      std::string errmsg = "For DecVecTypeBetts::ValidateMeshStageIndeces, ";
      errmsg += "meshIdx must be less than numStateMeshPoints\n";
      throw LowThrustException(errmsg);
   }
   return true;
}
