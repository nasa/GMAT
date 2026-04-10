//------------------------------------------------------------------------------
//                              DecisionVector
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
// Created: 2015.06.17
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 *
 *  This class manages parts of a decision vector allowing you to create
 *  a decision vector, extract parts, or set parts of the vector.
 *
 */
//------------------------------------------------------------------------------

#include "DecisionVector.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SET
//#define DEBUG_GET
//#define DEBUG_INIT
//#define DEBUG_DEC_VEC
//#define DEBUG_DEC_VEC_STATE

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
DecisionVector::DecisionVector() :
   numStateVars            (0),
   numControlVars          (0),
   numStateMeshPoints      (0),
   numControlMeshPoints    (0),
   numStateStagePoints     (0),
   numControlStagePoints   (0),
   numStatePoints          (0),
   numControlPoints        (0),
   numStatePointsPerMesh   (0),
   numControlPointsPerMesh (0),
   numDecisionParams       (0),
   numStateParams          (0),
   numControlParams        (0),
   numIntegralParams       (0),
   numStaticParams         (0),
   integralStartIdx        (0),
   integralStopIdx         (0),
   timeStartIdx            (0),
   timeStopIdx             (0),
   staticStartIdx          (0),
   staticStopIdx           (0)
{
   // by default decisionVector is unsized
}


//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
DecisionVector::DecisionVector(const DecisionVector &copy) :
   numStateVars            (copy.numStateVars),
   numControlVars          (copy.numControlVars),
   numStateMeshPoints      (copy.numStateMeshPoints),
   numControlMeshPoints    (copy.numControlMeshPoints),
   numStateStagePoints     (copy.numStateStagePoints),
   numControlStagePoints   (copy.numControlStagePoints),
   numStatePoints          (copy.numStatePoints),
   numControlPoints        (copy.numControlPoints),
   numStatePointsPerMesh   (copy.numStatePointsPerMesh),
   numControlPointsPerMesh (copy.numControlPointsPerMesh),
   numDecisionParams       (copy.numDecisionParams),
   numStateParams          (copy.numStateParams),
   numControlParams        (copy.numControlParams),
   numIntegralParams       (copy.numIntegralParams),
   numStaticParams         (copy.numStaticParams),
   integralStartIdx        (copy.integralStartIdx),
   integralStopIdx         (copy.integralStopIdx),
   timeStartIdx            (copy.timeStartIdx),
   timeStopIdx             (copy.timeStopIdx),
   staticStartIdx          (copy.staticStartIdx),
   staticStopIdx           (copy.staticStopIdx)
{
   Integer sz = (Integer) copy.decisionVector.GetSize();
   decisionVector.SetSize(sz);
   for (Integer ii = 0; ii < sz; ii++)
      decisionVector[ii] = copy.decisionVector[ii];
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
DecisionVector& DecisionVector::operator=(const DecisionVector &copy)
{
   
   if (&copy == this)
      return *this;
   
   numStateVars             = copy.numStateVars;
   numControlVars           = copy.numControlVars;
   numStateMeshPoints       = copy.numStateMeshPoints;
   numControlMeshPoints     = copy.numControlMeshPoints;
   numStateStagePoints      = copy.numStateStagePoints;
   numControlStagePoints    = copy.numControlStagePoints;
   numStatePoints           = copy.numStatePoints;
   numControlPoints         = copy.numControlPoints;
   numStatePointsPerMesh    = copy.numStatePointsPerMesh;
   numControlPointsPerMesh  = copy.numControlPointsPerMesh;
   numDecisionParams        = copy.numDecisionParams;
   numStateParams           = copy.numStateParams;
   numControlParams         = copy.numControlParams;
   numIntegralParams        = copy.numIntegralParams;
   numStaticParams          = copy.numStaticParams;
   integralStartIdx         = copy.integralStartIdx;
   integralStopIdx          = copy.integralStopIdx;
   timeStartIdx             = copy.timeStartIdx;
   timeStopIdx              = copy.timeStopIdx;
   staticStartIdx           = copy.staticStartIdx;
   staticStopIdx            = copy.staticStopIdx;
   
   Integer sz = (Integer) copy.decisionVector.GetSize();
   decisionVector.SetSize(sz);
   for (Integer ii = 0; ii < sz; ii++)
      decisionVector[ii] = copy.decisionVector[ii];

   return *this;
}


//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
DecisionVector::~DecisionVector()
{
   // nothing to do here
}

//------------------------------------------------------------------------------
//  void Initialize(Integer nStateVars,
//                  Integer nControlVars,
//                  Integer nIntegralParams,
//                  Integer nStaticParams,
//                  Integer nStateMeshPoints,
//                  Integer nControlMeshPoints,
//                  Integer nStateStagePoints,
//                  Integer nControlStagePoints)
//------------------------------------------------------------------------------
/**
 * This method initializes the decision vector
 *
 * @param <nStateVars>          number of state variables
 * @param <nControlVars>        number of control variables
 * @param <nIntegralParams>     number of integral parameters
 * @param <nStaticParams>       number of static parameters
 * @param <nStateMeshPoints>    number of state mesh points
 * @param <nControlMeshPoints>  number of control mesh points
 * @param <nStateStagePoints>   number of state stage points
 * @param <nControlStagePoints> number of control stage points
 *
 *
 */
//------------------------------------------------------------------------------
void DecisionVector::Initialize(Integer nStateVars,
                                Integer nControlVars,
                                Integer nIntegralParams,
                                Integer nStaticParams,
                                Integer nStateMeshPoints,
                                Integer nControlMeshPoints,
                                Integer nStateStagePoints,
                                Integer nControlStagePoints)
{
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("In Init ...\n");
   #endif
   // Initialize the decision vector
   
   //  Check that inputs are valid
   if (nStateVars <= 0)
   {
      std::string errmsg = "For DecisionVector, numStateVars ";
      errmsg += "must be greater than 0.\n";
      throw LowThrustException(errmsg);
   }
   if (nStateMeshPoints <= 0)
   {
      std::string errmsg = "For DecisionVector, numStateMeshPoints ";
      errmsg += "must be greater than 0.\n";
      throw LowThrustException(errmsg);
   }
   if (nControlMeshPoints <= 0)
   {
      std::string errmsg = "For DecisionVector, numControlMeshPoints ";
      errmsg += "must be greater than 0.\n";
      throw LowThrustException(errmsg);
   }
   
   // Set optimal control problem sizes
   numStateVars          = nStateVars;
   numControlVars        = nControlVars;
   numIntegralParams     = nIntegralParams;
   numStaticParams       = nStaticParams;
   
   // Set discretization properites
   numStateMeshPoints    = nStateMeshPoints;
   numControlMeshPoints  = nControlMeshPoints;
   numStateStagePoints   = nStateStagePoints;
   numControlStagePoints = nControlStagePoints;
   
   SetChunkIndeces();
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage(
             "In Init ... setting size of decision vector to %d\n",
             numDecisionParams);
   #endif
   decisionVector.SetSize(numDecisionParams);
   decisionVector.MakeZeroVector();
}

//------------------------------------------------------------------------------
//  bool SetDecisionVector(const Rvector &decVector)
//------------------------------------------------------------------------------
/**
 * This method sets the decision vector to the input value
 *
 * @param <decVector>          decision vector
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool DecisionVector::SetDecisionVector(const Rvector &decVector)
{
   //  Set the entire decision vector
   
   // Check that decision vector is the required length
   Integer sz = decisionVector.GetSize();
   if (sz != numDecisionParams)
   {
      std::string errmsg = "For DecisionVector::SetDecisionVector, ";
      errmsg += "state vector is not valid length\n";
      throw LowThrustException(errmsg);
   }
   
   // Set the decision vector
   for (Integer ii = 0; ii < sz; ii++)
      decisionVector[ii]    = decVector[ii];

   return true;
}

//------------------------------------------------------------------------------
//  Rvector GetDecisionVector()
//------------------------------------------------------------------------------
/**
 * This method returns the decision vector value
 *
 * @return the current decision vector
 *
 */
//------------------------------------------------------------------------------
Rvector DecisionVector::GetDecisionVector()
{
   #ifdef DEBUG_GET
      MessageInterface::ShowMessage("GetDecisionVector:\n");
      MessageInterface::ShowMessage("   size of decision vector = %d\n",
                                    decisionVector.GetSize());
   #endif
   return decisionVector;
}


//------------------------------------------------------------------------------
//  const Rvector* GetDecisionVectorPointer()
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to the decision vector
 *
 * @return a pointer to the decision vector
 *
 */
//------------------------------------------------------------------------------
const Rvector* DecisionVector::GetDecisionVectorPointer()
{
   return &decisionVector;
}

//------------------------------------------------------------------------------
//  bool SetTimeVector(const Rvector &tVector)
//------------------------------------------------------------------------------
/**
 * This method sets the time vector
 *
 * @param <tVector> the input time vector
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool DecisionVector::SetTimeVector(const Rvector &tVector)
{
   #ifdef DEBUG_SET
      MessageInterface::ShowMessage("SetTimeVector:\n");
      MessageInterface::ShowMessage("   timeStartIdx = %d\n", timeStartIdx);
      MessageInterface::ShowMessage("   timeStopIdx  = %d\n", timeStopIdx);
   #endif
   // Given a time vector, insert it in the decision vector
   
   if (tVector.GetSize() != 2)
   {
      std::string errmsg = "For DecisionVector::SetTimeVector, ";
      errmsg += "time vector is not valid dimension\n";
      throw LowThrustException(errmsg);
   }

   decisionVector[timeStartIdx] = tVector[0];
   decisionVector[timeStopIdx]  = tVector[1];
   
   return true;
}


//------------------------------------------------------------------------------
//  Rvector GetTimeVector()
//------------------------------------------------------------------------------
/**
 * This method returns the time vector
 *
 * @return the time vector
 *
 */
//------------------------------------------------------------------------------
Rvector DecisionVector::GetTimeVector()
{
   Rvector timeVec(2,decisionVector[timeStartIdx], decisionVector[timeStopIdx]);
   
   return timeVec;
}

//------------------------------------------------------------------------------
//  bool SetStaticVector(const Rvector &sVector)
//------------------------------------------------------------------------------
/**
 * This method sets the static vector
 *
 * @param <sVector> the static vector
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool DecisionVector::SetStaticVector(const Rvector &sVector)
{
   // Given a static vector, insert it in the decision vector
   
   Integer sz = sVector.GetSize(); // check against start-stop too?
   if (sz != numStaticParams)
   {
      std::string errmsg = "For DecisionVector::SetStaticVector, ";
      errmsg += "static vector is not valid dimension\n";
      throw LowThrustException(errmsg);
   }
   for (Integer ii = 0; ii < sz; ii++)
      decisionVector[staticStartIdx +ii] = sVector[ii];
   return true;
}

//------------------------------------------------------------------------------
//  Rvector  GetStaticVector()
//------------------------------------------------------------------------------
/**
 * This method returns the static vector
 *
 * @return the static vector
 *
 */
//------------------------------------------------------------------------------
Rvector DecisionVector::GetStaticVector()
{
   Rvector sVector(numStaticParams);
   for (Integer ii = 0; ii < numStaticParams; ii++)
      sVector[ii] = decisionVector[staticStartIdx + ii];
   
   return sVector;
}

//------------------------------------------------------------------------------
//  bool SetIntegralVector(const Rvector &iVector)
//------------------------------------------------------------------------------
/**
 * This method sets the integral vector
 *
 * @param <iVector> the integral vector
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool DecisionVector::SetIntegralVector(const Rvector &iVector)
{
   // Given an integral vector, insert it in the decision vector
   
   Integer sz = iVector.GetSize();
   if (sz != numIntegralParams)
   {
      std::string errmsg = "For DecisionVector::SetIntegralVector, ";
      errmsg += "integral vector is not valid dimension\n";
      throw LowThrustException(errmsg);
   }
   for (Integer ii = 0; ii < sz; ii++)
      decisionVector[integralStartIdx +ii] = iVector[ii];
   return true;
}


//------------------------------------------------------------------------------
//  Rvector GetIntegralVector()
//------------------------------------------------------------------------------
/**
 * This method returns the integral vector
 *
 * @return the integral vector
 *
 */
//------------------------------------------------------------------------------
Rvector DecisionVector::GetIntegralVector()
{
   Rvector iVector(numIntegralParams);
   for (Integer ii = 0; ii < numIntegralParams; ii++)
      iVector[ii] = decisionVector[integralStartIdx + ii];
   
   return iVector;
}

//------------------------------------------------------------------------------
//  Rvector GetFirstStateVector()
//------------------------------------------------------------------------------
/**
 * This method returns the first state vector
 *
 * @return the first state vector
 *
 */
//------------------------------------------------------------------------------
Rvector DecisionVector::GetFirstStateVector()
{
   return GetStateAtMeshPoint(0);
}

//------------------------------------------------------------------------------
//  Rvector GetLastStateVector()
//------------------------------------------------------------------------------
/**
 * This method returns the last state vector
 *
 * @return the last state vector
 *
 */
//------------------------------------------------------------------------------
Rvector DecisionVector::GetLastStateVector()
{
   return GetStateAtMeshPoint(numStateMeshPoints-1);
}

//------------------------------------------------------------------------------
//  Integer GetNumDecisionParams()
//------------------------------------------------------------------------------
/**
 * This method returns the number of decision parameters
 *
 * @return the number of decision parameters
 *
 */
//------------------------------------------------------------------------------
Integer DecisionVector::GetNumDecisionParams()
{
   return numDecisionParams;
}

//------------------------------------------------------------------------------
//  Integer GetNumStatePoints()
//------------------------------------------------------------------------------
/**
 * This method returns the number of state points
 *
 * @return the number of state points
 *
 */
//------------------------------------------------------------------------------
Integer DecisionVector::GetNumStatePoints()
{
   return numStatePoints;
}

//------------------------------------------------------------------------------
//  Integer GetNumControlPoints()
//------------------------------------------------------------------------------
/**
 * This method returns the number of control points
 *
 * @return the number of control points
 *
 */
//------------------------------------------------------------------------------
Integer DecisionVector::GetNumControlPoints()
{
   return numControlPoints;
}

//------------------------------------------------------------------------------
//  Integer GetNumStateVars()
//------------------------------------------------------------------------------
/**
 * This method returns the number of state variables
 *
 * @return the number of state variables
 *
 */
//------------------------------------------------------------------------------
Integer DecisionVector::GetNumStateVars()
{
   return numStateVars;
}

//------------------------------------------------------------------------------
//  Integer GetNumControlVars()
//------------------------------------------------------------------------------
/**
 * This method returns the number of control variables
 *
 * @return the number of control variables
 *
 */
//------------------------------------------------------------------------------
Integer DecisionVector::GetNumControlVars()
{
   return numControlVars;
}

//------------------------------------------------------------------------------
//  Real GetFirstTime()
//------------------------------------------------------------------------------
/**
 * This method returns the first time
 *
 * @return the first time
 *
 */
//------------------------------------------------------------------------------
Real DecisionVector::GetFirstTime()
{
   return decisionVector[timeStartIdx];
}

//------------------------------------------------------------------------------
//  Real GetLastTime()
//------------------------------------------------------------------------------
/**
 * This method returns the last time
 *
 * @return the last time
 *
 */
//------------------------------------------------------------------------------
Real DecisionVector::GetLastTime()
{
   return decisionVector[timeStopIdx];
}


//------------------------------------------------------------------------------
//  IntegerArray GetTimeIdxs()
//------------------------------------------------------------------------------
/**
 * This method returns the array of time indexes
 *
 * @return array of time indexes
 *
 */
//------------------------------------------------------------------------------
IntegerArray DecisionVector::GetTimeIdxs()
{
   IntegerArray idxs;
   for (Integer ii = timeStartIdx; ii <= timeStopIdx; ii++)
      idxs.push_back(ii);
   return idxs;
}



//------------------------------------------------------------------------------
//  Rvector GetStateAtMeshPoint(Integer meshIdx, Integer stageIdx = 0)
//------------------------------------------------------------------------------
/**
 * This method returns the state at the given mesh point and stage point
 *
 * @param <meshIdx>   input mesh point
 * @param <stageIdx>  input stage point
 *
 * @return state at the given mesh and stage point
 *
 */
//------------------------------------------------------------------------------
Rvector DecisionVector::GetStateAtMeshPoint(Integer meshIdx, Integer stageIdx)
{
   // Check that state vector contains the mesh point
   if (meshIdx < 0 || meshIdx > numStatePoints)
   {
      std::string errmsg = "For DecisionVector::GetStateAtMeshPoint, ";
      errmsg += "meshIdx must be >= 0 and <= numStateMeshPoints\n";
      throw LowThrustException(errmsg);
   }
   
   // Compute indeces of state vector components then extract the
   // state vector
   IntegerArray idxs = GetStateIdxsAtMeshPoint(meshIdx, stageIdx);
   #ifdef DEBUG_DEC_VEC_STATE
      MessageInterface::ShowMessage("In GetStateAtMeshPoint, idxs size = %d\n",
                                    idxs.size());
      MessageInterface::ShowMessage(
                        "In GetStateAtMeshPoint, decisionVector size = %d\n",
                        decisionVector.GetSize());
   #endif
   Integer num = idxs.size();
   Rvector theStateVector(num);
   for (Integer ii = 0; ii < num; ii++)
   {
      #ifdef DEBUG_DEC_VEC_STATE
         MessageInterface::ShowMessage(
                        "                  decisionVector(%d) = %12.10f\n",
                        idxs[ii], decisionVector[idxs[ii]]);
      #endif
      theStateVector[ii] = decisionVector[idxs[ii]];
   }
   
   return theStateVector;
}

//------------------------------------------------------------------------------
//  Rvector GetControlAtMeshPoint(Integer meshIdx, Integer stageIdx = 0)
//------------------------------------------------------------------------------
/**
 * This method returns the control at the given mesh point and stage point
 *
 * @param <meshIdx>   input mesh point
 * @param <stageIdx>  input stage point
 *
 * @return control at the given mesh and stage point
 *
 */
//------------------------------------------------------------------------------
Rvector DecisionVector::GetControlAtMeshPoint(Integer meshIdx, Integer stageIdx)
{
   // Check that state vector contains the mesh point
   if (meshIdx < 0 || meshIdx > numControlPoints)
   {
      std::string errmsg = "For DecisionVector::GetControlAtMeshPoint, ";
      errmsg += "meshIdx must be >= 0 and <= numControlMeshPoints\n";
      throw LowThrustException(errmsg);
   }
   
   // Compute indeces of control vector components the extract the
   // control vector
   IntegerArray idxs = GetControlIdxsAtMeshPoint(meshIdx, stageIdx);
   Integer num = idxs.size();
   Rvector theControlVector(num);
   for (Integer ii = 0; ii < num; ii++)
      theControlVector[ii] = decisionVector[idxs[ii]];
   
   return theControlVector;
}

//------------------------------------------------------------------------------
//  Rvector  GetInterpolatedStateVector(Real atTime)
//------------------------------------------------------------------------------
/**
 * This method returns the interpolated state vector at the given time
 *
 * @param <atTime>   input time
 *
 * @return interpolated state vector at the given time
 *
 */
//------------------------------------------------------------------------------
Rvector DecisionVector::GetInterpolatedStateVector(Real atTime)
{
   // NOTE THIS IS A DUMMY IMPLEMENTATION THAT CURRENTLY ONLY TESTS
   // THAT THE INTERFACES ARE WORKING
   //   warning('DecisionVector::GetInterpolatedStateVector is not implemented')
   Rvector stateVec(numStateVars); // default is all zeroes
   return stateVec;
}


//------------------------------------------------------------------------------
//  Rvector  GetInterpolatedControlVector(Real atTime)
//------------------------------------------------------------------------------
/**
 * This method returns the interpolated control vector at the given time
 *
 * @param <atTime>   input time
 *
 * @return interpolated control vector at the given time
 *
 */
//------------------------------------------------------------------------------
Rvector DecisionVector::GetInterpolatedControlVector(Real atTime)
{
   // NOTE THIS IS A DUMMY IMPLEMENTATION THAT CURRENTLY ONLY TESTS
   // THAT THE INTERFACES ARE WORKING
   //   warning('DecisionVector::GetInterpolatedStateVector is not implemented')
   Rvector controlVec(numControlVars); // default is all zeroes
   return controlVec;
}

