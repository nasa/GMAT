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

#ifndef DecisionVector_hpp
#define DecisionVector_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"

class CSALT_API DecisionVector
{
public:
   // class methods
   /// default constructor
   DecisionVector();
   /// copy constructor
   DecisionVector( const DecisionVector &copy);
   /// operator =
   DecisionVector& operator=(const DecisionVector &copy);
   
   /// destructor
   virtual ~DecisionVector();

   void Initialize(Integer nStateVars,       Integer nControlVars,
                   Integer nIntegralParams,  Integer nStaticParams,
                   Integer nStateMeshPoints, Integer nControlMeshPoints,
                   Integer nStateStagePoints,Integer nControlStagePoints);
   
   virtual bool     SetDecisionVector(const Rvector &decVector);
   virtual bool     SetTimeVector(const Rvector &tVector);
   virtual bool     SetStaticVector(const Rvector &sVector);
   virtual bool     SetIntegralVector(const Rvector &iVector);
   virtual Rvector  GetDecisionVector();
   virtual const Rvector*  GetDecisionVectorPointer();
   virtual Rvector  GetTimeVector();
   virtual Rvector  GetStaticVector();
   virtual Rvector  GetIntegralVector();
   virtual Rvector  GetFirstStateVector();
   virtual Rvector  GetLastStateVector();
   
   virtual Integer  GetNumDecisionParams();
   virtual Integer  GetNumStatePoints();
   virtual Integer  GetNumControlPoints();
   virtual Integer  GetNumStateVars();
   virtual Integer  GetNumControlVars();
   
   virtual Real     GetFirstTime();
   virtual Real     GetLastTime();
   virtual IntegerArray GetTimeIdxs();
   virtual Rvector  GetStateAtMeshPoint(Integer meshIdx, Integer stageIdx = 0);
   virtual Rvector  GetControlAtMeshPoint(Integer meshIdx,
                                          Integer stageIdx = 0);
   
   virtual Rvector  GetInterpolatedStateVector(Real atTime);
   virtual Rvector  GetInterpolatedControlVector(Real atTime);

   // Abstract public methods
   virtual Rvector  GetStateVector(Integer meshIdx, Integer stateIdx)       = 0;
   virtual Rvector  GetControlVector(Integer meshIdx, Integer stateIdx)     = 0;
   virtual IntegerArray GetFinalStateIdxs()                                 = 0;
   virtual IntegerArray GetInitialStateIdxs()                               = 0;
   virtual IntegerArray GetStateIdxsAtMeshPoint(Integer meshIdx,
                                                Integer stageIdx)           = 0;
   virtual IntegerArray GetControlIdxsAtMeshPoint(Integer meshIdx,
                                                  Integer stageIdx)         = 0;
   virtual IntegerArray GetStaticIdxs()                                     = 0;
   virtual Integer  GetFinalTimeIdx()                                       = 0;
   virtual Integer  GetInitialTimeIdx()                                     = 0;
   virtual Rmatrix  GetControlArray()                                       = 0;
   virtual Rmatrix  GetStateArray()                                         = 0;
   virtual void     SetControlArray(const Rmatrix &cArray)                  = 0;
   virtual void     SetStateArray(const Rmatrix &sArray)                    = 0;
   
   // These are public for test drivers only???
   virtual bool     SetStateVector(Integer meshIdx,Integer stageIdx,
                                   const Rvector &stateVec)                 = 0;
   virtual bool     SetControlVector(Integer meshIdx,Integer stageIdx,
                                     const Rvector &controlVec)             = 0;

protected:
   /// Number of states in the optimal control problem.
   Integer numStateVars;
   /// Number of controls in the optimal control problem.
   Integer numControlVars;
   
   /// Number of mesh points that have state variables
   Integer numStateMeshPoints;
   /// Number of mesh points that have control variables
   Integer numControlMeshPoints;
   /// Number of stage points that have state variables
   Integer numStateStagePoints;
   /// Number of stage points that have control variables
   Integer numControlStagePoints;
   /// The number of points (mesh+stage) that have state vars
   Integer numStatePoints;
   /// The number of points (mesh+stage) that have control vars
   Integer numControlPoints;
   /// The number of state points in each mesh interval
   Integer numStatePointsPerMesh;
   /// The number of control points in each mesh interval
   Integer numControlPointsPerMesh;
   
   /// The number of optimization variables
   Integer numDecisionParams;
   /// The number of state optimization variables in NLP
   Integer numStateParams;
   /// The number of control optimization variables in NLP
   Integer numControlParams;
   /// Number of integral params in the opt. control problem
   Integer numIntegralParams;
   /// Number of static params in the optimal control problem.
   Integer numStaticParams;
   
   /// The index for the start of the integral dec. vec. chunk
   Integer integralStartIdx;
   /// The index for the end of the integral dec. vec. chunk
   Integer integralStopIdx;
   /// The index for the start of the time dec. vec. chunk
   Integer timeStartIdx;
   /// The index for the end of the time dec. vec. chunk
   Integer timeStopIdx;
   /// The index for the start of the static dec. vec. chunk
   Integer staticStartIdx;
   /// The index for the end of the static dec. vec. chunk
   Integer staticStopIdx;
   
   /// The decision vector
   Rvector decisionVector;
   
   // protected methods
   virtual void SetChunkIndeces() = 0;
};

#endif // DecisionVector_hpp
