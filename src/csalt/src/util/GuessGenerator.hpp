//------------------------------------------------------------------------------
//                              GuessGenerator
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
// Created: 2016.04.12
//
/**
 * From original MATLAB prototype:
 *  Author. S. Hughes
 *  add by E.K. Dec. 02. 2014
 *
 *  This class computes the initial guess for optimal control problems
 *  Inputs are the dimensions of the problem, discretization points,
 *  and details required to compute the initial guess (method,
 *  and possibly pointers to user dynamics functions.
 *  The returned values are the guess for the states and controls at
 *  the discretizetion points.
 */
//------------------------------------------------------------------------------

#ifndef GuessGenerator_hpp
#define GuessGenerator_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "TrajectoryData.hpp"
#include "OCHTrajectoryData.hpp"
#include "ArrayTrajectoryData.hpp"
#include "ScalingUtility.hpp"

class CSALT_API GuessGenerator
{
public:

   GuessGenerator();
   GuessGenerator(const GuessGenerator &copy);
   GuessGenerator& operator=(const GuessGenerator &copy);
   virtual ~GuessGenerator();

   /// Initializes the GuessGenerator
   virtual void            Initialize(const Rvector     &timeVec,
                                      Integer           numS,
                                      Integer           numSPts,
                                      Integer           numC,
                                      Integer           numCPts,
                                      const std::string &gMode);

   virtual Rmatrix         ComputeLinearStateGuess(
                           const Rvector &initGuess,
                           const Rvector &finalGuess);

   virtual Rmatrix         ComputeConstantControlGuess(
                           Real controlMag);

   virtual void            ComputeLinearGuess(
                           const Rvector &initGuess,
                           const Rvector &finalGuess,
                           Rmatrix &stateMat,
                           Rmatrix &controlMat);

   virtual void            ComputeUserFunctionGuess(
                           TrajectoryData *userClass,
                           ScalingUtility *scaleUtil,
                           const std::string &timeVecType,
                           Rmatrix &stateMat,
                           Rmatrix &controlMat);
        
   virtual void            ComputeGuessFromOCHFile(
                           const std::string &OCHFileName,
                           const std::string &timeVecType,
                           Rmatrix &stateMat,
                           Rmatrix &controlMat);

   virtual void            ComputeGuessFromArrayData(
                           ArrayTrajectoryData* guessArrayData,
                           const std::string &timeVecType,
                           Rmatrix &stateMat,
                           Rmatrix &controlMat);

protected:
   /// vector of times in the phase
   Rvector               timeVector;
   /// the number of states in the optimal control problem
   Integer               numStates;
   /// the number of state mesh points
   Integer               numStatePoints;
   /// the number of controls in the optimal control problem
   Integer               numControls;
   /// the number of control mesh points
   Integer               numControlPoints;
   /// indicates model for the guess
   std::string           guessMode;

   Rvector GetEvenlySpacedArray(Real start, Real end, Integer numPts);
   Rvector ConvertTrajectoryTimeToGuessTime(const std::string &timeVecType,
                                            Rvector inputTimes);
};

#endif // GuessGenerator_hpp
