//------------------------------------------------------------------------------
//                             ArrayTrajectoryData
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
// Author: Jeremy Knittel / NASA
// Created: 2017.02.23
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "ArrayTrajectoryData.hpp"
#include "LowThrustException.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
ArrayTrajectoryData::ArrayTrajectoryData() :
   TrajectoryData()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ArrayTrajectoryData::ArrayTrajectoryData(const ArrayTrajectoryData &copy) :
   TrajectoryData(copy)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ArrayTrajectoryData& ArrayTrajectoryData::operator=(
                                          const ArrayTrajectoryData &copy)
{
   if (&copy == this)
      return *this;   
   
   TrajectoryData::operator=(copy);
         
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ArrayTrajectoryData::~ArrayTrajectoryData()
{
    // Nothing to do here
}

//------------------------------------------------------------------------------
// Real ProcessTimeValue(std::string input, std::string timeSystem)
//------------------------------------------------------------------------------
/**
* This method processes a time value based on the time system specified
*
* @param <input>      the time value input
* @param <timeSystem> the time system format of the input
* @return the time value converted to A1ModJulian
*
*/
//------------------------------------------------------------------------------
Real ArrayTrajectoryData::ProcessTimeValue(Real input, std::string timeSystem)
{
   Real value;
   std::string system, format;
   theTimeConverter->
      GetTimeSystemAndFormat(timeSystem, system,
         format);
   Integer origTimeTypeID =
      theTimeConverter->GetTimeTypeID(system);
   std::string a1ModJulainName = "A1";
   Integer a1ModJulianTypeID =
      theTimeConverter->GetTimeTypeID(a1ModJulainName);
   value = theTimeConverter->Convert(input, origTimeTypeID,
      a1ModJulianTypeID);
   return value;
}

//------------------------------------------------------------------------------
// void ReadFromArray(Rmatrix guessArray, std::string coordSystem,
//                    std::string timeSystem, std::string centralBody)
//------------------------------------------------------------------------------
/**
* This method collects guess data from provided matrix, currently states are
* assumed to be Keplarian states (with mass included) and it is assumed that no
* integrals are used
*
* @param <guessArray> The matrix containing the guess data, each row is a new
*                     data point
* @param <coordSystem> The coordinate system being used by the guess data
* @param <timeSystem> The time system being used by the guess data
* @param <centralBody> The central body being used by the guess data
*/
//------------------------------------------------------------------------------
void ArrayTrajectoryData::ReadFromArray(Rmatrix guessArray,
                                        std::string coordSystem,
                                        std::string timeSystem,
                                        std::string centralBody)
{
   // Clear the vectors
   for (Integer ii = 0; ii < segments_.size(); ii++)
      delete segments_.at(ii);
   segments_.clear();
   hasSegmentHadDuplicates.clear();

   // @todo Currently only one segment is supported for a GmatArray input
   OCHTrajectorySegment *trajSeg = new OCHTrajectorySegment();
   segments_.push_back(trajSeg);
   hasSegmentHadDuplicates.push_back(false);
   numSegments++;

   // These assignments are needed if output is an och file
   trajSeg->SetCentralBody(centralBody);
   trajSeg->SetRefFrame(coordSystem);
   trajSeg->SetTimeSystem(timeSystem);

   // Set number of states, controls and integrals

   // @todo The state and integral vectors are currently set to predetermined
   // values, they will need to be generalized
   SetNumStateParams(0, 7);
   SetNumControlParams(0, guessArray.GetNumColumns() - 8);
   SetNumIntegralParams(0, 0);
   TrajectoryDataStructure localData;
   localData.states.SetSize(7);
   localData.controls.SetSize(guessArray.GetNumColumns() - 8);
   localData.integrals.SetSize(0);

   // Apply the values from the guess array to the local data vectors
   for (Integer dataPointIdx = 0; dataPointIdx < guessArray.GetNumRows();
      ++dataPointIdx)
   {
      localData.time = ProcessTimeValue(guessArray(dataPointIdx, 0),
         timeSystem);
      for (Integer stateIdx = 0; stateIdx < 7; ++stateIdx)
         localData.states(stateIdx) = guessArray(dataPointIdx, stateIdx + 1);
      for (Integer controlIdx = 0; controlIdx < guessArray.GetNumColumns() - 8;
         ++controlIdx)
      {
         localData.controls(controlIdx) =
            guessArray(dataPointIdx, controlIdx + 8);
      }
      try
      {
         trajSeg->AddDataPoint(localData);
      }
      catch (LowThrustException &lt)
      {
         // Assume exception thrown is for non-monotonic times
         throw LowThrustException("ERROR initializing guess from input array: "
                                  "data points are not in the correct temporal "
                                  "order\n");
      }
   }
}

//------------------------------------------------------------------------------
// void WriteToFile(std::string fileName)
//------------------------------------------------------------------------------
/**
* This method writes all data to a text file
*
* @param <fileName> the file to write to
*
*/
//------------------------------------------------------------------------------
void ArrayTrajectoryData::WriteToFile(std::string fileName)
{
   #ifdef DEBUG_READ_OCH
      MessageInterface::ShowMessage(
                        " Entering ArrayTrajectoryData::WriteToFile with %s\n",
                        fileName.c_str());
   #endif
   
   MessageInterface::ShowMessage(
             "Writing ArrayTrajectoryData to a file is not yet implemented.\n");
}
