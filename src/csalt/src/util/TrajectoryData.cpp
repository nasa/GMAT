//------------------------------------------------------------------------------
//                             TrajectoryData
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
// Created: 2017.02.01
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "TrajectoryData.hpp"
#include "LinearInterpolator.hpp"
#include "CubicSplineInterpolator.hpp"
#include "NotAKnotInterpolator.hpp"
#include "LagrangeInterpolator.hpp"
#include "LowThrustException.hpp"

#include <sstream>

//#define DEBUG_INTERPOLATE

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
TrajectoryData::TrajectoryData() :
    numSegments                    (0),
    interpType                     (NOTAKNOT),
    interpolator                   (NULL),
    interpPoints                   (5),
    pointToCopy                    (-1),
    allowInterSegmentExtrapolation (false),
    allowExtrapolation             (false),
    segmentWarningPrinted          (false),
    duplicateTimeFound             (false),
    scaleUtil                      (NULL)
{
   theTimeConverter = theTimeConverter->Instance();
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
TrajectoryData::TrajectoryData(const TrajectoryData &copy) :
    numSegments                    (copy.numSegments),
    interpType                     (copy.interpType),
    interpolator                   (copy.interpolator),
    interpPoints                   (copy.interpPoints),
    pointToCopy                    (copy.pointToCopy),
    allowInterSegmentExtrapolation (copy.allowInterSegmentExtrapolation),
    allowExtrapolation             (copy.allowExtrapolation),
    segmentWarningPrinted          (copy.segmentWarningPrinted),
    duplicateTimeFound             (copy.duplicateTimeFound),
    theTimeConverter               (copy.theTimeConverter),
    scaleUtil                      (copy.scaleUtil)
{
   CopyArrays(copy);
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
TrajectoryData& TrajectoryData::operator=(const TrajectoryData &copy)
{
   
   if (&copy == this)
      return *this;      

   numSegments                    = copy.numSegments; 
   interpType                     = copy.interpType;
   interpolator                   = copy.interpolator;
   interpPoints                   = copy.interpPoints;
   pointToCopy                    = copy.pointToCopy;
   allowInterSegmentExtrapolation = copy.allowInterSegmentExtrapolation;
   allowExtrapolation             = copy.allowExtrapolation;
   segmentWarningPrinted          = copy.segmentWarningPrinted;
   duplicateTimeFound             = copy.duplicateTimeFound;
   theTimeConverter               = copy.theTimeConverter;
   scaleUtil                      = copy.scaleUtil;
         
   CopyArrays(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
TrajectoryData::~TrajectoryData()
{
   if (interpolator)
      delete interpolator;

   for (Integer ii = 0; ii < segments_.size(); ii++)
      delete segments_.at(ii);
   segments_.clear();
}

//------------------------------------------------------------------------------
//void SetInterpType(Integer inputType);
//------------------------------------------------------------------------------
/**
* This method sets the type of interpolation to use
*
* @param <inputType> the type of interpolation to use
*/
//------------------------------------------------------------------------------          
void TrajectoryData::SetInterpType(Integer inputType)
{
   interpType = inputType;
}   

//------------------------------------------------------------------------------
//void SetNumSegments(Integer input);
//------------------------------------------------------------------------------
/**
* This method sets the number of segments
*
* @param <input> the number of segments
*/
//------------------------------------------------------------------------------          
void TrajectoryData::SetNumSegments(Integer num)
{
   bool isLarger = num > numSegments;
   if (isLarger)
   {
      for (Integer ii = 0; ii < (num - numSegments); ii++)
      {
         segments_.push_back(new TrajectorySegment());
         hasSegmentHadDuplicates.push_back(false);
      }
   }
   else
   {
      segments_.resize(num);
      hasSegmentHadDuplicates.resize(num);
   }
   numSegments = num;
}

//------------------------------------------------------------------------------
//Integer GetNumSegments();
//------------------------------------------------------------------------------
/**
* This method returns the number of segments
*
* @return  the number of segments
*/
//------------------------------------------------------------------------------          
Integer TrajectoryData::GetNumSegments()
{
   return numSegments;
} 

//------------------------------------------------------------------------------
//TrajectorySegment GetSegment(Integer idx);
//------------------------------------------------------------------------------
/**
* This method returns a Trajectory segment at the input index
*
* @param <idx> the index of the segment to return
*
* @return the trajectory segment at the specified index
*/
//------------------------------------------------------------------------------  
TrajectorySegment TrajectoryData::GetSegment(Integer idx)
{
   return *segments_.at(idx);
}

//------------------------------------------------------------------------------
// Integer TrajectoryData::GetRelevantSegment(Real requestedTime)
//------------------------------------------------------------------------------
/**
 * This method returns the segment which contains an input time value
 *
 * @param <requestedTime> the time value
 *
 * @return Integer of the matching segment
 *
 */
//------------------------------------------------------------------------------
Integer TrajectoryData::GetRelevantSegment(Real requestedTime)
{
   Integer sz  = segments_.size();  // should be equal to numSegments
   Integer ndp = segments_.at(sz-1)->GetNumDataPoints();
#ifdef DEBUG_INTERPOLATE
   MessageInterface::ShowMessage(
                                 "Entering GetRelevantSegment for time = %12.20f\n",
                                 requestedTime);
   MessageInterface::ShowMessage(" --- number of segments = %d (%d)\n",
                                 sz, numSegments);
   MessageInterface::ShowMessage(
                                 " --- number of data points (last segment) = %d\n", sz);
   MessageInterface::ShowMessage(
                                 "   segment time (0) = %12.20f\n",
                                 segments_.at(0)->GetTime(0));
   MessageInterface::ShowMessage(
                                 "   segment time (last) = %12.20f\n",
                                 segments_.at(sz-1)->GetTime(ndp-1));
#endif
   
   if (requestedTime <= (segments_.at(0)->GetTime(0) +
                         GmatRealConstants::REAL_EPSILON))
   {
      if (allowExtrapolation)
         return 0;
      else
         throw LowThrustException("ERROR - TrajectoryData: requested time"
                                  " is outside of data points and extrapolation not allowed.");
   }
   else if (requestedTime > (segments_.at(sz-1)->GetTime(ndp-1) -
                             GmatRealConstants::REAL_EPSILON))
   {
      if (allowExtrapolation)
         return numSegments-1;
      else
         throw LowThrustException("ERROR - TrajectoryData: requested time"
                                  " is outside of data points and extrapolation not allowed.");
   }
   else
   {
      for (Integer s = 0; s < numSegments; s++)
      {
         Integer numData = segments_.at(s)->GetNumDataPoints();
#ifdef DEBUG_INTERPOLATE
         MessageInterface::ShowMessage("checking times %12.10f to $12.10f\n",
                                       segments_.at(s)->GetTime(0), segments_.at(s)->GetTime(numData-1));
         MessageInterface::ShowMessage(
                                       "allowInterSegmentExtrapolation = %s\n",
                                       (allowInterSegmentExtrapolation?  "true" : "false"));
#endif
         if (requestedTime >= segments_.at(s)->GetTime(0) &&
             requestedTime <= segments_.at(s)->GetTime(numData-1))
            return s;
      }
#ifdef DEBUG_INTERPOLATE
      MessageInterface::ShowMessage("s was NOT returned!!!!\n");
      MessageInterface::ShowMessage("allowInterSegmentExtrapolation = %s\n",
                                    (allowInterSegmentExtrapolation?  "true" : "false"));
#endif
      
      if (allowInterSegmentExtrapolation)
      {
         for (Integer s = 0; s < numSegments-1; s++)
            if (requestedTime <= segments_.at(s+1)->GetTime(0) &&
                requestedTime >= segments_.at(s)->GetTime(segments_.at(s)->
                                                          GetNumDataPoints()-1))
               return fabs(requestedTime - segments_.at(s+1)->GetTime(0)) <
               fabs(requestedTime - segments_.at(s)->
                    GetTime(segments_.at(s)->GetNumDataPoints()-1)) ?
               s+1 : s ;
      }
      else
         throw LowThrustException("ERROR - TrajectoryData: requested time"
                                  " is between segments and intersegment extrapolation "
                                  "not allowed.");
   }
#ifdef DEBUG_INTERPOLATE
   MessageInterface::ShowMessage(
                                 "Exiting GetRelevantSegment with ERROR!!!\n");
#endif
   return -1;
}

//------------------------------------------------------------------------------
// Integer GetMaxNumControl()
//------------------------------------------------------------------------------
/**
 * This method returns the maximum number of control params per segment
 *
 * @return  max number of control parameters per segment
 *
 */
//------------------------------------------------------------------------------
Integer TrajectoryData::GetMaxNumControl()
{
   Integer maxControlParams = 0;
   if (segments_.size() > 0)
   {
      maxControlParams = segments_[0]->GetNumControls();
      for (Integer s = 1; s < numSegments; s++)
         if (segments_[s]->GetNumControls() > maxControlParams)
            maxControlParams = segments_[s]->GetNumControls();
   }
   return maxControlParams;
}

//------------------------------------------------------------------------------
// void SetNumControlParams(Integer s, Integer num);
//------------------------------------------------------------------------------
/**
* This method sets the number of parameters in the control data vector
*
* @param <s>   the segment to set the size
* @param <num> number of paramaters
*
*/
//------------------------------------------------------------------------------
void TrajectoryData::SetNumControlParams(Integer s,Integer params)
{
   segments_[s]->SetNumControlParams(params);
}

//------------------------------------------------------------------------------
// void SetNumStateParams(Integer s, Integer num);
//------------------------------------------------------------------------------
/**
* This method sets the number of parameters in the state data vector
*
* @param <s>   the segment to set the size
* @param <num> number of paramaters
*
*/
//------------------------------------------------------------------------------
void TrajectoryData::SetNumStateParams(Integer s,Integer params)
{
   segments_[s]->SetNumStateParams(params);
}

//------------------------------------------------------------------------------
// void SetAllowInterSegmentExtrapolation(bool input)
//------------------------------------------------------------------------------
/**
* This method sets the flag to allow extrapolation between segments
*
* @param <input> the new flag value
*
*/
//------------------------------------------------------------------------------
void TrajectoryData::SetAllowInterSegmentExtrapolation(bool input)
{
   allowInterSegmentExtrapolation = input;
}

//------------------------------------------------------------------------------
// void SetAllowExtrapolation(bool input)
//------------------------------------------------------------------------------
/**
* This method sets the flag to allow extrapolation outside of all segments
*
* @param <input> the new flag value
*
*/
//------------------------------------------------------------------------------
void TrajectoryData::SetAllowExtrapolation(bool input)
{
   allowExtrapolation = input;
}

//------------------------------------------------------------------------------
// void SetScalingUtility(ScalingUtility *scalingUtil)
//------------------------------------------------------------------------------
/**
* This method sets the scaling utility that can be used for scaling trajectory
* data
*
* @param <input> the new scaling utility
*
*/
//------------------------------------------------------------------------------
void TrajectoryData::SetScalingUtility(ScalingUtility *scalingUtil)
{
   scaleUtil = scalingUtil;
}

//------------------------------------------------------------------------------
// void SetNumIntegralParams(Integer s, Integer num);
//------------------------------------------------------------------------------
/**
* This method sets the number of parameters in the integral data vector
*
* @param <s>   the segment to set the size
* @param <num> number of paramaters
*
*/
//------------------------------------------------------------------------------
void TrajectoryData::SetNumIntegralParams(Integer s,Integer params)
{
   segments_[s]->SetNumIntegralParams(params);
}

//------------------------------------------------------------------------------
// void AddDataPoint(Integer s, TrajectoryDataStructure inputData)
//------------------------------------------------------------------------------
/**
* This method adds a data point
*
* @param <s>         the segment to add the data
* @param <inputData> a structure of new data
*
*/
//------------------------------------------------------------------------------
void TrajectoryData::AddDataPoint(Integer s, TrajectoryDataStructure inputData)
{
   // Pass the data to the segment itself
   segments_[s]->AddDataPoint(inputData);
}

//------------------------------------------------------------------------------
// Rmatrix GetState(Rvector requestedTimes)
//------------------------------------------------------------------------------
/**
* This method performs interpolation at a vector of desired time values 
* to get state values
*
* @param <requestedTimes> the time values to poll OCH data at
*
* @return A matrix of data corresponding to each requested time
*
*/
//------------------------------------------------------------------------------
Rmatrix TrajectoryData::GetState(Rvector requestedTimes)
{
   Rmatrix output;

   std::vector<TrajectoryDataStructure> dataOut = Interpolate(requestedTimes,
                                                            STATE);

   Integer maxStateParams = segments_[0]->GetNumStates();
   for (Integer s = 1; s < numSegments; s++)
      if (segments_[s]->GetNumStates() > maxStateParams)
         maxStateParams = segments_[s]->GetNumStates();

   output.SetSize(requestedTimes.GetSize(),maxStateParams);

   for (UnsignedInt idx = 0; idx < dataOut.size(); idx++)
   {
      for (Integer jdx = 0; jdx < dataOut.at(idx).states.GetSize(); jdx++)
         output(idx,jdx) = dataOut.at(idx).states(jdx);
      for (Integer jdx = dataOut.at(idx).states.GetSize();
           jdx < maxStateParams; jdx++)
         output(idx,jdx) = 0.0;
   }

   return output;
}

//------------------------------------------------------------------------------
// Rmatrix GetControl(Rvector requestedTimes)
//------------------------------------------------------------------------------
/**
* This method performs interpolation at a vector of desired time values 
* to get control values
*
* @param <requestedTimes> the time values to poll OCH data at
*
* @return A matrix of data corresponding to each requested time
*
*/
//------------------------------------------------------------------------------
Rmatrix TrajectoryData::GetControl(Rvector requestedTimes)
{
   Rmatrix output;

   std::vector<TrajectoryDataStructure> dataOut = Interpolate(requestedTimes,
                                                            CONTROL);

   Integer maxControlParams = segments_[0]->GetNumControls();
   for (Integer s = 1; s < numSegments; s++)
      if (segments_[s]->GetNumControls() > maxControlParams)
         maxControlParams = segments_[s]->GetNumControls();

   output.SetSize(requestedTimes.GetSize(),maxControlParams);

   for (UnsignedInt idx = 0; idx < dataOut.size(); idx++)
   {
      for (Integer jdx = 0; jdx < dataOut.at(idx).controls.GetSize(); jdx++)
         output(idx,jdx) = dataOut.at(idx).controls(jdx);
      for (Integer jdx = dataOut.at(idx).controls.GetSize();
           jdx < maxControlParams; jdx++)
         output(idx,jdx) = 0.0;
   }

   return output;
}

//------------------------------------------------------------------------------
// Rmatrix GetIntegral(Rvector requestedTimes)
//------------------------------------------------------------------------------
/**
* This method performs interpolation at a vector of desired time values 
* to get integral values
*
* @param <requestedTimes> the time values to poll OCH data at
*
* @return A matrix of data corresponding to each requested time
*
*/
//------------------------------------------------------------------------------
Rmatrix TrajectoryData::GetIntegral(Rvector requestedTimes)
{
   Rmatrix output;

   std::vector<TrajectoryDataStructure> dataOut =
                                      Interpolate(requestedTimes,INTEGRAL);

   Integer maxIntegralParams = segments_[0]->GetNumIntegrals();
   for (Integer s = 1; s < numSegments; s++)
      if (segments_[s]->GetNumIntegrals() > maxIntegralParams)
         maxIntegralParams = segments_[s]->GetNumIntegrals();

   output.SetSize(requestedTimes.GetSize(),maxIntegralParams);

   for (UnsignedInt idx = 0; idx < dataOut.size(); idx++)
   {
      for (Integer jdx = 0; jdx < dataOut.at(idx).integrals.GetSize(); jdx++)
         output(idx,jdx) = dataOut.at(idx).integrals(jdx);
      for (Integer jdx = dataOut.at(idx).integrals.GetSize();
           jdx < maxIntegralParams; jdx++)
         output(idx,jdx) = 0.0;
   }

   return output;
}

//------------------------------------------------------------------------------
// std::vector<TrajectoryDataStructure> Interpolate(Rvector requestedTimes)
//------------------------------------------------------------------------------
/**
* This method performs interpolation at a vector of desired time values
*
* @param <requestedTimes> the time values to poll OCH data at
*
* @return A vector of data structures corresponding to each requested time
*
*/
//------------------------------------------------------------------------------
std::vector<TrajectoryDataStructure> TrajectoryData::Interpolate(
                                     Rvector requestedTimes,DataType type)
{
   #ifdef DEBUG_INTERPOLATE
      MessageInterface::ShowMessage("Entering Interpolate wth type = %d\n",
                                    type);
      MessageInterface::ShowMessage("size of requestedTimes = %d\n",
                                    requestedTimes.GetSize());
   #endif
   
   UpdateInterpolator();

   #ifdef DEBUG_INTERPOLATE
      MessageInterface::ShowMessage("---> Interpolator updated\n");
   #endif

   std::vector<TrajectoryDataStructure> output;
   TrajectoryDataStructure localData;

   localData.states.SetSize(0);
   localData.controls.SetSize(0);
   localData.integrals.SetSize(0);

   // Double check that the time values are consecutive
   Real segTimes[2];
   Real tPrecision = 1.0e-10, relT;

   for (Integer s = 1; s < numSegments; s++)
   {
      segTimes[0] = segments_.at(s-1)->GetTime(segments_.at(s-1)->
                    GetNumDataPoints()-1);
      segTimes[1] = segments_.at(s)->GetTime(0);
      relT = segTimes[0] - segTimes[1];
      if (fabs(segTimes[0]) > 0.1)
         relT /= segTimes[0];

      if (relT > tPrecision && !segmentWarningPrinted)
      {
         std::stringstream msg;
         msg.precision(14);
         msg << "WARNING - TrajectoryData: "
            << "Time inputs between segments are not monotonically "
            << "increasing.  For overlapping times, the first segment "
            << "detected containing the requested time will be used for "
            << "interpolation.  For gaps between segments, interpolation "
            << "will be attempted normally.\n ";
             
         MessageInterface::ShowMessage(msg.str());
         segmentWarningPrinted = true;
      }
   }

   for (Integer idx = 0; idx < requestedTimes.GetSize(); idx++)
   {
      Integer currentSegment = GetRelevantSegment(requestedTimes(idx));
      #ifdef DEBUG_INTERPOLATE
         MessageInterface::ShowMessage(
            "---> relevant segment retrieved for index %d at time %12.10f\n",
            idx, requestedTimes(idx));
      #endif

      // start with empty arrays each time
      localData.states.SetSize(0);
      localData.controls.SetSize(0);
      localData.integrals.SetSize(0);
    
      localData.time = requestedTimes(idx);
      bool success = false;
      if (type == ALL || type == STATE)
      {
         localData.states.SetSize(
                          segments_.at(currentSegment)->GetNumStates());
         for (Integer jdx = 0;
              jdx < segments_.at(currentSegment)->GetNumStates(); jdx++)
         {
            UpdateInterpPoints(currentSegment,requestedTimes(idx),
                                STATE,jdx);

            if (duplicateTimeFound)
            {
               // In this case, assign the requested data as the known
               // data with the nearest time
               break;
            }

            success = interpolator->Interpolate(requestedTimes(idx),
               &localData.states(jdx));
            if (!success)
            {
               throw LowThrustException("ERROR - TrajectoryData: "
                  "Interpolation of state data failed at time " +
                  GmatStringUtil::ToString(requestedTimes(idx), 14) +
                  " at setgment " +
                  GmatStringUtil::ToString(currentSegment, 1) + ".\n");
            }
         }

         if (duplicateTimeFound)
         {
            if (pointToCopy != -1)
            {
               for (Integer jdx = 0;
                  jdx < segments_.at(currentSegment)->GetNumStates(); jdx++)
               {
                  // Duplicate was found, copy data of closest time to requested
                  // time
                  localData.states(jdx) =
                     segments_.at(currentSegment)->GetState(pointToCopy, jdx);
               }
               // Reset duplicate time data
               duplicateTimeFound = false;
               pointToCopy = -1;
            }
            else
            {
               throw LowThrustException("ERROR - TrajectoryData: "
                  "Interpolation of state data failed at time " +
                  GmatStringUtil::ToString(requestedTimes(idx), 14) +
                  " at setgment " +
                  GmatStringUtil::ToString(currentSegment, 1) + ".\n");
            }
         }
      }

      if (type == ALL || type == CONTROL)
      {
         localData.controls.SetSize(
                            segments_.at(currentSegment)->GetNumControls());
         for (Integer jdx = 0;
              jdx < segments_.at(currentSegment)->GetNumControls(); jdx++)
         {
            UpdateInterpPoints(currentSegment,requestedTimes(idx),
                                CONTROL,jdx);

            if (duplicateTimeFound)
            {
               // In this case, assign the requested data as the known
               // data with the nearest time
               break;
            }

            success = interpolator->Interpolate(requestedTimes(idx),
               &localData.controls(jdx));
            if (!success)
            {
               throw LowThrustException("ERROR - TrajectoryData: "
                  "Interpolation of control data failed at time " +
                  GmatStringUtil::ToString(requestedTimes(idx), 14) +
                  " at setgment " +
                  GmatStringUtil::ToString(currentSegment, 1) + ".\n");
            }
         }

         if (duplicateTimeFound)
         {
            if (pointToCopy != -1)
            {
               for (Integer jdx = 0;
                  jdx < segments_.at(currentSegment)->GetNumControls(); jdx++)
               {
                  // Duplicate was found, copy data of closest time to requested
                  // time
                  localData.controls(jdx) =
                     segments_.at(currentSegment)->GetControl(pointToCopy, jdx);
               }
               // Reset duplicate time data
               duplicateTimeFound = false;
               pointToCopy = -1;
            }
            else
            {
               throw LowThrustException("ERROR - TrajectoryData: "
                  "Interpolation of control data failed at time " +
                  GmatStringUtil::ToString(requestedTimes(idx), 14) +
                  " at setgment " +
                  GmatStringUtil::ToString(currentSegment, 1) + ".\n");
            }
         }
      }

      if (type == ALL || type == INTEGRAL)
      {
         localData.integrals.SetSize(
                   segments_.at(currentSegment)->GetNumIntegrals());
         for (Integer jdx = 0;
              jdx < segments_.at(currentSegment)->GetNumIntegrals(); jdx++)
         {
            UpdateInterpPoints(currentSegment,requestedTimes(idx),
                                INTEGRAL,jdx);

            if (duplicateTimeFound)
            {
               // In this case, assign the requested data as the known
               // data with the nearest time
               break;
            }

            success = interpolator->Interpolate(requestedTimes(idx),
               &localData.integrals(jdx));
            if (!success)
            {
               throw LowThrustException("ERROR - TrajectoryData: "
                  "Interpolation of control data failed at time " +
                  GmatStringUtil::ToString(requestedTimes(idx), 14) +
                  " at setgment " +
                  GmatStringUtil::ToString(currentSegment, 1) + ".\n");
            }
         }

         if (duplicateTimeFound)
         {
            if (pointToCopy != -1)
            {
               for (Integer jdx = 0;
                  jdx < segments_.at(currentSegment)->GetNumIntegrals(); jdx++)
               {
                  // Duplicate was found, copy data of closest time to requested
                  // time
                  localData.integrals(jdx) =
                     segments_.at(currentSegment)->GetIntegral(pointToCopy, jdx);
               }
               // Reset duplicate time data
               duplicateTimeFound = false;
               pointToCopy = -1;
            }
            else
            {
               throw LowThrustException("ERROR - TrajectoryData: "
                  "Interpolation of integral data failed at time " +
                  GmatStringUtil::ToString(requestedTimes(idx), 14) +
                  " at setgment " +
                  GmatStringUtil::ToString(currentSegment, 1) + ".\n");
            }
         }
      }

      output.push_back(localData);
    }

    return output;
}

//------------------------------------------------------------------------------
// void UpdateInterpPoints(Integer segment, Real requestedTime, DataType type,
//                         Integer dataIdx)
//------------------------------------------------------------------------------
/**
* This method updates the data points held by the interpolator
*
* @param <currSegment>   the current segment
* @param <requestedTime> the time for which data is requested
* @param <type>          the data type
* @param <dataIdx>       the data index
*/
//------------------------------------------------------------------------------
void TrajectoryData::UpdateInterpPoints(Integer currSegment, Real requestedTime,
                                        DataType type, Integer dataIdx)
{
   #ifdef DEBUG_INTERPOLATE
      MessageInterface::ShowMessage("Entering UpdateInterpPoints\n");
   #endif
   // Reset the interpolator first
   interpolator->Clear();
   interpolator->SetExtrapolation(false);

   Integer nPoints = segments_.at(currSegment)->GetNumDataPoints();

   // Create a vector of indices of data that will be added to the interpolator
   Rvector indicesToAdd;

   // If there are fewer data points than the nominally requested
   // interpolation data points, reset the holder variable
   Integer interpPointsLocal = interpPoints > nPoints ? nPoints : interpPoints;

   // Size the vector with how many points we are going to add
   indicesToAdd.SetSize(interpPointsLocal);

   // Double check if we need to extrapolate on the low side for this time
   if (requestedTime < segments_.at(currSegment)->GetTime(0))
   {
      if ((currSegment > 0 && allowInterSegmentExtrapolation) ||
          (currSegment == 0 && allowExtrapolation))
      {
         // We do. Set the interpolator's flag
         interpolator->SetExtrapolation(true);

         // Add the first points
         for (Integer idx = 0; idx < interpPointsLocal; idx++)
             indicesToAdd(idx) = idx;
      }
      else
         throw LowThrustException("ERROR - TrajectoryData: requested time"
                 " is outside of segment and extrapolation is not allowed.");    
   }
   // Double check if we need to extrapolate on the high side for this time
   else if (requestedTime > segments_.at(currSegment)->GetTime(nPoints-1))
   {
      if ((currSegment < numSegments-1 && allowInterSegmentExtrapolation) ||
          (currSegment == numSegments-1 && allowExtrapolation))
      {
         // We do. Set the interpolator's flag
         interpolator->SetExtrapolation(true);

         // Add the final points
         Integer startIndex = 0;
         for (Integer idx = nPoints-interpPointsLocal; idx < nPoints; idx++)
         {
             indicesToAdd(startIndex) = idx;
             startIndex++;
         }
      }
      else
         throw LowThrustException("ERROR - TrajectoryData: requested time"
                 " is outside of segment and extrapolation is not allowed."); 
   }
   else
   {
      // First we will loop to find the closest value in the data array.
      // This assumes that the data is monotonically increasing. A check was
      // done earlier.

      // Create some helper variables
      Integer closestIndex = -1; // Index of the closest value
      Real currentDistance = 0; // Time distance from the current data point
      Real lastDistance = fabs(
                       requestedTime - segments_.at(currSegment)->GetTime(0));
            // Time distance from the last data point

      // Loop through each time value in the known data set. Starting with the
      // second data point
      for (Integer idx = 1; idx < nPoints; idx++)
      {
         // Update the current distance
         currentDistance = fabs(
                   requestedTime - segments_.at(currSegment)->GetTime(idx));

         // If the current distance is greater than the last distance, we are
         // moving further from the the data point, so we found the
         // data point
         if (currentDistance >= lastDistance)
         {
             // Set the index and finish the loop
             closestIndex = idx - 1;
             break;
         }
         // We didnt find the data point yet, so continue looping after
         // resetting the tracker distance
         lastDistance = currentDistance;
      }

      // If a closest index wasnt found, the final data value must be
      // the closest
      if (closestIndex == -1)
         closestIndex = nPoints - 1;

      // Now we need to add indices for the closest points surrounding the
      // closest point itself

      // First declare some helper variables
      Integer startIndex, delta0, delta;

      // Check if we are on the left or right side of the closest point
      if (requestedTime >= segments_.at(currSegment)->GetTime(closestIndex))
      {
         // The requested time is on the right. The index of first point
         // to add will be one greater than the initial point
         startIndex = Integer(floor(float(interpPointsLocal-1)/2));
         delta0 = delta = 1;
      }
      else
      {
         // The requested time is on the left. The first point to add will
         // also be one lesser than the initial point
         startIndex = Integer(ceil(float(interpPointsLocal-1)/2));
         delta0 = delta = -1;
      }

      // Check if we have enough points on the left side of the closest point
      if (closestIndex + delta * ceil(float(interpPointsLocal-1)/2) < 0 ||
          closestIndex - delta * floor(float(interpPointsLocal-1)/2) < 0)
      {
         // We do not. Add the first points
         for (Integer idx = 0; idx < interpPointsLocal; idx++)
         {
            indicesToAdd(idx) = idx;
         }
      }
      // Check if we have enough points on the right side of the closest point
      else if (closestIndex + delta * ceil(float(interpPointsLocal-1)/2) >=
              nPoints || closestIndex - delta *
              floor(float(interpPointsLocal-1)/2) >= nPoints)
      {
         // We do not. Add the final points
         startIndex = 0;
         for (Integer idx = nPoints-interpPointsLocal; idx < nPoints; idx++)
         {
            indicesToAdd(startIndex) = idx;
            startIndex++;
         }
      }
      else
      {
         // Start by adding the closest point
         indicesToAdd(startIndex) = closestIndex;

         // Loop through and add as many points as neceessary (interpPoints)
         for (Integer idx = 0; idx < interpPointsLocal - 1; idx++)
         {
            indicesToAdd(startIndex+delta) = closestIndex + delta;

            // Flip the sign of delta, so that we add a point on the other
            // side of the closest point next
            delta *= -1;

            // Increase the magnitude of delta if we are back to the
            // original side of the closest point
            if (float(idx+1)/2 == floor((idx+1)/2)) delta += delta0;
         }
      }
   }

   // Loop through the indices of the points to add and make sure that data
   // exists in all of them
   std::vector<Integer> badIdxs;
   for (Integer idx = 0; idx < interpPointsLocal; idx++)
   {
      switch (type)
      {
         case (STATE):
             if (!segments_.at(currSegment)->GetStateSize(indicesToAdd(idx)))
                 badIdxs.push_back(idx);
             break;
         case (CONTROL):
             if (!segments_.at(currSegment)->GetControlSize(
                                             indicesToAdd(idx)))
                 badIdxs.push_back(idx);
             break;
         case (INTEGRAL):
             if (!segments_.at(currSegment)->GetIntegralSize(
                                             indicesToAdd(idx)))
                 badIdxs.push_back(idx);
             break;
         case (ALL):
             throw LowThrustException(
                     "ERROR - TrajectoryData: ALL data type not"
                     " possible here.");
             break;
      }
   }
   if (badIdxs.size() > 1)
         throw LowThrustException(
                 "ERROR - TrajectoryData: Data not present in multiple"
                 " places needed for interpolation.");
   if (badIdxs.size() == 1)
   {
      if (badIdxs[0] == interpPointsLocal-1)
      {
         if (indicesToAdd(interpPointsLocal-1) == nPoints-1 &&
             allowExtrapolation)
         {
            interpolator->SetExtrapolation(true);
         }

         for (Integer idx = 0; idx < interpPointsLocal; idx++)
            indicesToAdd(idx) -= 1;
                     
         if (indicesToAdd(0) < 0)
            throw LowThrustException(
                 "ERROR - TrajectoryData: Data not present where"
                 " needed for interpolation.");
      }
      else
         throw LowThrustException(
             "ERROR - TrajectoryData: Data not present where"
             " needed for interpolation.");
   }

   // Check if there are any duplicate times trying to be used for 
   // interpolation coefficients
   RealArray timesToAdd;
   for (Integer idx = 0; idx < interpPointsLocal; idx++)
   {
      timesToAdd.push_back(segments_.at(currSegment)->GetTime(
         indicesToAdd(idx)));
   }

   for (Integer idx = 0; idx < interpPointsLocal; idx++)
   {
      for (Integer idx2 = idx + 1; idx2 < interpPointsLocal; idx2++)
      {
         if (timesToAdd.at(idx) == timesToAdd.at(idx2))
         {
            duplicateTimeFound = true;

            if (hasSegmentHadDuplicates.size() == 0)
            {
               for (Integer i = 0; i < numSegments; ++i)
                  hasSegmentHadDuplicates.push_back(false);
            }

            if (!hasSegmentHadDuplicates.at(currSegment))
            {
               std::string warnMsg = "WARNING - TrajectoryData: "
                  "Identical time values detected when "
                  "creating interpolator coefficients in phase %d.  "
                  "Interpolation cannot be completed.  The requested "
                  "dependent variables will be estimated as the "
                  "same dependent variables of the closest "
                  "known time value\n";
               MessageInterface::ShowMessage(warnMsg.c_str(), currSegment + 1);
               hasSegmentHadDuplicates.at(currSegment) = true;
            }
            break;
         }
      }

      if (duplicateTimeFound)
         break;
   }

   // Find which point is closest to the requested point if a duplicate was
   // found
   if (duplicateTimeFound && timesToAdd.size() > 0)
   {
      Real timeDiff = GmatMathUtil::Abs(requestedTime - timesToAdd.at(0));
      pointToCopy = indicesToAdd(0);
      for (Integer idx = 1; idx < interpPointsLocal; idx++)
      {
         if (GmatMathUtil::Abs(requestedTime - timesToAdd.at(idx)) < timeDiff)
         {
            timeDiff = GmatMathUtil::Abs(requestedTime - timesToAdd.at(idx));
            pointToCopy = indicesToAdd(idx);
         }
      }
   }

   // Finally, loop through the indices of the points to add, and add the data
   // values to the interpolator
   for (Integer idx = 0; idx < interpPointsLocal; idx++)
   {
      Real val;
      switch (type)
      {
         case (STATE):
            val = segments_.at(currSegment)->GetState(indicesToAdd(idx),
                                                      dataIdx);
            break;
         case (CONTROL):
            val = segments_.at(currSegment)->GetControl(indicesToAdd(idx),
                                                        dataIdx);
            break;
         case (INTEGRAL):
            val = segments_.at(currSegment)->GetIntegral(indicesToAdd(idx),
                                                         dataIdx);
            break;
         case (ALL):
            throw LowThrustException(
                     "ERROR - TrajectoryData: ALL data type not"
                     " possible here.");
            break;
      }
      interpolator->AddPoint(segments_.at(currSegment)->GetTime(
                             indicesToAdd(idx)),&val);
   }
   #ifdef DEBUG_INTERPOLATE
      MessageInterface::ShowMessage("Exiting UpdateInterpPoints\n");
   #endif
}

//------------------------------------------------------------------------------
// void UpdateInterpolator()
//------------------------------------------------------------------------------
/**
 * This method resets the interpolator for a new interpolation call.
 *
 *
 */
//------------------------------------------------------------------------------
void TrajectoryData::UpdateInterpolator()
{   
   // In case we need a new interpolator class, delete the old one
   if (interpolator)
      delete interpolator;

   // Create a new interpolator of the type that the user has requested
   if (interpType == SPLINE)
   {
      interpolator = new CubicSplineInterpolator("",1);
      interpPoints = 5;
   }
   else if (interpType == LINEAR)
   {
      interpolator = new LinearInterpolator("",1);
      interpPoints = 2;
   }
   else if (interpType == NOTAKNOT)
   {
      interpolator = new NotAKnotInterpolator("",1);
      interpPoints = 5;
   }
   else if (interpType == LAGRANGE)
   {
      interpolator = new LagrangeInterpolator("",1);
      interpPoints = 80;
   }
}

//------------------------------------------------------------------------------
// void CopyArrays(const TrajectoryData &copy)
//------------------------------------------------------------------------------
/**
 * This method copies the array values to those of the input TrajectoryData
 *
 * @param <td>   trajectory data to copy
 *
 */
//------------------------------------------------------------------------------
void TrajectoryData::CopyArrays(const TrajectoryData &copy)
{
   hasSegmentHadDuplicates.clear();
   for (Integer i = 0; i < copy.hasSegmentHadDuplicates.size(); ++i)
      hasSegmentHadDuplicates.push_back(copy.hasSegmentHadDuplicates.at(i));

   segments_.resize(numSegments);
   segments_ = copy.segments_;
}
