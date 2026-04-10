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
#include "GuessGenerator.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_GUESS_GEN

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
GuessGenerator::GuessGenerator() :
   numStates                   (-1),
   numStatePoints              (-1),
   numControls                 (-1),
   numControlPoints            (-1),
   guessMode                   ("") // default??
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
GuessGenerator::GuessGenerator(const GuessGenerator &copy) :
   numStates                   (copy.numStates),
   numStatePoints              (copy.numStatePoints),
   numControls                 (copy.numControls),
   numControlPoints            (copy.numControlPoints),
   guessMode                   (copy.guessMode)
{

   Integer sz = (Integer) copy.timeVector.GetSize();
   timeVector.SetSize(sz);
   timeVector = copy.timeVector;
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
GuessGenerator& GuessGenerator::operator=(const GuessGenerator &copy)
{
   if (&copy == this)
      return *this;

   numStates        = copy.numStates;
   numStatePoints   = copy.numStatePoints;
   numControls      = copy.numControls;
   numControlPoints = copy.numControlPoints;
   guessMode        = copy.guessMode;

   Integer sz = (Integer) copy.timeVector.GetSize();
   timeVector.SetSize(sz);
   timeVector = copy.timeVector;

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
GuessGenerator::~GuessGenerator()
{
   // nothing to do here
}

//------------------------------------------------------------------------------
//  void Rvector& Initialize(const Rvector     &timeVec,
//                           Integer           numS,
//                           Integer           numSPts,
//                           Integer           numC,
//                           Integer           numCPts,
//                           const std::string &gMode)
//------------------------------------------------------------------------------
/**
 * This method initializes the GuessGenerator
 *
 * @param <timeVec> The vector of times for the phase discretization
 * @param <numS>    The number of states in the optimal control problem.  
 *                  For orbit problem with mass flow, numS is 
 *                  7 (x,y,z,vx,vy,vz,m)
 * @param <numSPts) The number of state points in the optimal control 
 *                  discretization
 * @param <numC>    The number of controls in the optimal control problem.  For 
 *                  orbit problem with [ux, uy, and uz], the number of controls 
 *                  equals three
 * @param <numCPts> The number of control points in the optimal control 
 *                  discretization
 * @param <gMode>   Guess mode
 *
 * NOTE: do we need to return a bool - true if successful?
 *
 */
//------------------------------------------------------------------------------
void GuessGenerator::Initialize(const Rvector     &timeVec,
                                Integer           numS,
                                Integer           numSPts,
                                Integer           numC,
                                Integer           numCPts,
                                const std::string &gMode)
{
   // Initialize the state and control array
   Integer sz = (Integer) timeVec.GetSize();
   timeVector.SetSize(sz);
   timeVector       = timeVec;
   numStates        = numS;
   numStatePoints   = numSPts;
   numControls      = numC;
   numControlPoints = numCPts;
   guessMode        = gMode;
}

//------------------------------------------------------------------------------
//  Rmatrix ComputeLinearStateGuess(const Rvector &initGuess,
//                                  const Rvector &finalGuess)
//------------------------------------------------------------------------------
/**
 * This method computes the guess for the linear state guess method
 *
 * @param <initGuess>  Real array of numStates x 1.
 *                     This is the guess for the state at the end of the phase
 *                     (i.e. at the beginning of the phase)
 * @param <finalGuess> Real array of numStates x 1.
 *                     This is the guess for the state at the end of the phase
 *
 * @return an array numStatePoints x numStates containing the initial guess 
 *         for the state
 *
 */
//------------------------------------------------------------------------------
Rmatrix GuessGenerator::ComputeLinearStateGuess(const Rvector &initGuess,
                                                const Rvector &finalGuess)
{
   #ifdef DEBUG_GUESS_GEN
      MessageInterface::ShowMessage("Entering ComputeLinearStateGuess ...\n");
   #endif

   // Fill in the state array
   Rmatrix stateGuess(numStatePoints, numStates);

   for (Integer cc = 0; cc < numStates; cc++)
   {
      Rvector column = GetEvenlySpacedArray(initGuess(cc),
                       finalGuess(cc), numStatePoints);
      for (Integer rr = 0; rr < numStatePoints; rr++)
         stateGuess(rr, cc) = column(rr);
   }
   #ifdef DEBUG_GUESS_GEN
      MessageInterface::ShowMessage("EXITing ComputeLinearStateGuess ...\n");
   #endif
   return stateGuess;
}


//------------------------------------------------------------------------------
//  Rmatrix ComputeConstantControlGuess(Real controlMag)
//------------------------------------------------------------------------------
/**
 * This method computes the guess for constant control
 *
 * @param <controlMag>  The magnitude of each control vector component.  Each 
 *                      element of returned guess will be controlMag
 *
 * @return an array numControlPoints x numControls containing the initial guess 
 *         for the control
 *
 */
//------------------------------------------------------------------------------
Rmatrix GuessGenerator::ComputeConstantControlGuess(Real controlMag)
{
   // Fill in the control array
   Rmatrix controlGuess(numControlPoints, numControls);

   for (Integer cc = 0; cc < numControls; cc++)
   {
      Rvector column = GetEvenlySpacedArray(controlMag,
                       controlMag, numControlPoints);
      for (Integer rr = 0; rr < numControlPoints; rr++)
         controlGuess(rr, cc) = column(rr);
   }
   return controlGuess;
}


//------------------------------------------------------------------------------
//  void ComputeLinearGuess(const Rvector &initGuess,
//                          const Rvector &finalGuess,
//                          Rmatrix       &stateMat,
//                          Rmatrix       &controlMat)
//------------------------------------------------------------------------------
/**
 * This method computes the guess based on straight line state and control
 *
 * @param <initGuess>  initial guess
 * @param <finalGuess> final guess
 * @param <stateMat>   OUTPUT state matrix
 * @param <controlMat> OUTPUT control matrix
 *
 * @return state and control matrices
 *
 */
//------------------------------------------------------------------------------
void GuessGenerator:: ComputeLinearGuess(const Rvector &initGuess,
                                         const Rvector &finalGuess,
                                         Rmatrix       &stateMat,
                                         Rmatrix       &controlMat)
{
   //  Compute guess based on straight line state and control
   //  Compute the state guess
   stateMat = ComputeLinearStateGuess(initGuess, finalGuess);
   // Compute the control guess.
   Integer controlMag = 0;
   if (guessMode == "LinearUnityControl")
      controlMag = 1;

   if ((guessMode == "LinearUnityControl") || (guessMode == "LinearNoControl"))
         controlMat = ComputeConstantControlGuess(controlMag);
   else
      controlMat.SetSize(0,0);
}

//------------------------------------------------------------------------------
//  void ComputeUserFunctionGuess(const std::string &userFunctionName,
//                                const std::string &timeVecType,
//                                Rmatrix           &stateMat,
//                                Rmatrix           &controlMat)
//------------------------------------------------------------------------------
/**
 * This method calls a user guess function to generate a guess
 *
 * @param <userFunctionName>  user function name
 * @param <timeVecType>       time vector type
 * @param <stateMat>          OUTPUT state matrix
 * @param <controlMat>        OUTPUT control matrix
 *
 * @return state and control matrices
 *
 */
//------------------------------------------------------------------------------
void GuessGenerator::ComputeUserFunctionGuess(TrajectoryData    *userClass,
                                              ScalingUtility    *scaleUtil,
                                              const std::string &timeVecType,
                                              Rmatrix           &stateMat,
                                              Rmatrix           &controlMat)
{
   userClass->SetScalingUtility(scaleUtil);
   Rvector requestedTimes = ConvertTrajectoryTimeToGuessTime(timeVecType,
                                                             timeVector);

   //  Compute the state guess
   Rmatrix tempStateMat   = userClass->GetState(requestedTimes);
   Rmatrix tempControlMat = userClass->GetControl(requestedTimes);

   stateMat.SetSize(numStatePoints, numStates);
   for (Integer idx = 0; idx < numStatePoints; idx++)
      for (Integer jdx = 0; jdx < numStates; jdx++)
         stateMat(idx,jdx) = tempStateMat(idx,jdx);

   controlMat.SetSize(numControlPoints, numControls);
      for (Integer idx = 0; idx < numControlPoints; idx++)
         for (Integer jdx = 0; jdx < numControls; jdx++)
            controlMat(idx,jdx) = tempControlMat(idx,jdx);
}

//------------------------------------------------------------------------------
//  void ComputeGuessFromOCHFile(const std::string &OCHFileName,
//                               const std::string &timeVecType,
//                               Rmatrix           &stateMat,
//                               Rmatrix           &controlMat)
//------------------------------------------------------------------------------
/**
 * This method calls a user guess function to generate a guess
 *
 * @param <OCHFileName>       OCH file name
 * @param <timeVecType>       time vector type
 * @param <stateMat>          OUTPUT state matrix
 * @param <controlMat>        OUTPUT control matrix
 *
 * @return state and control matrices
 *
 */
//------------------------------------------------------------------------------
void GuessGenerator::ComputeGuessFromOCHFile(const std::string &OCHFileName,
                                             const std::string &timeVecType,
                                             Rmatrix           &stateMat,
                                             Rmatrix           &controlMat)
{
    
    Rvector requestedTimes = ConvertTrajectoryTimeToGuessTime(timeVecType,
                                                              timeVector);
    
    OCHTrajectoryData* guessData = new OCHTrajectoryData(OCHFileName);

    guessData->SetInterpType(TrajectoryData::NOTAKNOT);
    guessData->SetAllowInterSegmentExtrapolation(true);
    guessData->SetAllowExtrapolation(true);

    std::vector<TrajectoryDataStructure> interpGuessData =
                                         guessData->Interpolate(requestedTimes);

    stateMat.SetSize(numStatePoints, numStates);
    for (Integer idx = 0; idx < numStatePoints; idx++)
        for (Integer jdx = 0; jdx < numStates; jdx++)
            stateMat(idx,jdx) = interpGuessData.at(idx).states(jdx);

    controlMat.SetSize(numControlPoints, numControls);
    for (Integer idx = 0; idx < numControlPoints; idx++)
        for (Integer jdx = 0; jdx < numControls; jdx++)
            controlMat(idx,jdx) = interpGuessData.at(idx).controls(jdx);

    delete guessData;
}

//------------------------------------------------------------------------------
//  void ComputeGuessFromArrayData(ArrayTrajectoryData *guessArrayData,
//                                 const std::string   &timeVecType,
//                                 Rmatrix             &stateMat,
//                                 Rmatrix             &controlMat)
//------------------------------------------------------------------------------
/**
 * This method calls a user guess function to generate a guess
 *
 * @param <OCHFileName>       OCH file name
 * @param <timeVecType>       time vector type
 * @param <stateMat>          OUTPUT state matrix
 * @param <controlMat>        OUTPUT control matrix
 *
 * @return state and control matrices
 *
 */
//------------------------------------------------------------------------------
void GuessGenerator::ComputeGuessFromArrayData(
                                     ArrayTrajectoryData  *guessArrayData,
                                     const std::string    &timeVecType,
                                     Rmatrix              &stateMat,
                                     Rmatrix              &controlMat)
{
    
    Rvector requestedTimes = ConvertTrajectoryTimeToGuessTime(timeVecType,
                                                              timeVector);
    
    guessArrayData->SetInterpType(TrajectoryData::NOTAKNOT);
    guessArrayData->SetAllowInterSegmentExtrapolation(true);
    guessArrayData->SetAllowExtrapolation(true);

    std::vector<TrajectoryDataStructure> interpGuessData =
                                    guessArrayData->Interpolate(requestedTimes);

    stateMat.SetSize(numStatePoints, numStates);
    for (Integer idx = 0; idx < numStatePoints; idx++)
        for (Integer jdx = 0; jdx < numStates; jdx++)
            stateMat(idx,jdx) = interpGuessData.at(idx).states(jdx);

    controlMat.SetSize(numControlPoints, numControls);
    for (Integer idx = 0; idx < numControlPoints; idx++)
        for (Integer jdx = 0; jdx < numControls; jdx++)
            controlMat(idx,jdx) = interpGuessData.at(idx).controls(jdx);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

Rvector GuessGenerator::ConvertTrajectoryTimeToGuessTime(
                             const std::string &timeVecType,Rvector inputTimes)
{
    // TODO:: Convert time based on the input type! It needs to match whatever
    // units the OCH file converts them to
    return inputTimes;
}

//------------------------------------------------------------------------------
//  Rvector GetEvenlySpacedArray(Real start, Real end, Integer numPts)
//------------------------------------------------------------------------------
/**
 * This method returns an evenly-spaced array of numPts points between the
 * input start and end
 *
 * @param <start>    start of interval across which to space the points
 * @param <end>      end of interval across which to space the points
 * @param <numPts>   number of points requested
 *
 * @return vector of numPts evenly-spaced points across the start-end interval
 *
 */
//------------------------------------------------------------------------------
Rvector GuessGenerator::GetEvenlySpacedArray(Real start, Real end,
                                             Integer numPts)
{
    Rvector result(numPts);

    result(0)        = start;
    result(numPts-1) = end;
    Real spacing     = (end - start) / (numPts - 1);

    for (Integer ii = 1; ii < (numPts - 1); ii++)
       result(ii) = result(ii-1) + spacing;

    return result;
}
