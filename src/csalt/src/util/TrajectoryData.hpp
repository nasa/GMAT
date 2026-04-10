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

#ifndef TrajectoryData_hpp
#define TrajectoryData_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "MessageInterface.hpp"
#include "Interpolator.hpp"
#include "TrajectorySegment.hpp"
#include "TimeSystemConverter.hpp"
#include "ScalingUtility.hpp"

class CSALT_API TrajectoryData
{
public:

   /// Interpolation selection
   enum InterpSelection
   {
     LINEAR,
     SPLINE,
     NOTAKNOT,
     LAGRANGE
   };

   /// Data Type
   enum DataType
   {
     STATE,
     CONTROL,
     INTEGRAL,
     ALL
   };

   /// default constructor
   TrajectoryData();
   /// copy constructor
   TrajectoryData(const TrajectoryData &copy);
   /// operator=
   TrajectoryData& operator=(const TrajectoryData &copy);
   /// destructor
   virtual ~TrajectoryData(); 

   virtual void              SetInterpType(Integer inputType);
   virtual void              SetNumSegments(Integer num);
   virtual Integer           GetNumSegments();
   virtual TrajectorySegment GetSegment(Integer idx);
   virtual Integer           GetRelevantSegment(Real requestedTime);
   virtual Integer           GetMaxNumControl();

   virtual void              SetNumControlParams(Integer s,Integer params);
   virtual void              SetNumStateParams(Integer s,Integer params);
   virtual void              SetNumIntegralParams(Integer s,Integer params);

   virtual void              SetAllowInterSegmentExtrapolation(bool input);
   virtual void              SetAllowExtrapolation(bool input);

   virtual void              SetScalingUtility(ScalingUtility *scalingUtil);

   virtual void              AddDataPoint(Integer s,
                                        TrajectoryDataStructure inputData);

   virtual Rmatrix           GetState(Rvector requestedTimes);
   virtual Rmatrix           GetControl(Rvector requestedTimes);
   virtual Rmatrix           GetIntegral(Rvector requestedTimes);

   virtual std::vector<TrajectoryDataStructure>
                             Interpolate(Rvector  requestedTimes,
                                       DataType type = ALL);

   virtual void              WriteToFile(std::string fileName) = 0;  // or implemented?
   
protected:
   
   /// Number of segments in this trajectory data
   Integer      numSegments;

   /// The type of interpolation to use
   Integer      interpType;

   /// A pointer to the interpolator object
   Interpolator *interpolator;

   /// Number of points needed for the interpolation
   Integer      interpPoints;

   /// Which point to copy to requested time in the event interpolation 
   /// doesn't work due to duplicate times
   Integer      pointToCopy;

   /// Should extrapolation between segments be allowed?
   bool         allowInterSegmentExtrapolation;

   /// Should extrapolation outside of the overall bounds be allowed?
   bool         allowExtrapolation;

   /// Boolean of whether a waning message has been printed about 
   /// overlapping segments to avoid many messages from being printed
   bool         segmentWarningPrinted;

   /// Whether or not a duplicate time was found when setting 
   /// interpolation coefficients
   bool         duplicateTimeFound;

   /// Boolean array of whether each segment has detected duplicate times
   BooleanArray hasSegmentHadDuplicates;

   /// Vector of pointers to the segments which hold the data
   std::vector<TrajectorySegment*> segments_;

   /// Pointer to time converter object
   TimeSystemConverter *theTimeConverter;

   /// Scaling utility that can be used to scale trajectory data
   ScalingUtility* scaleUtil;


   void    CopyArrays(const TrajectoryData &copy);
   void    UpdateInterpolator();
   void    UpdateInterpPoints(Integer currSegment, Real requestedTime,
                            DataType type, Integer dataIdx);
};

#endif // TrajectoryData_hpp
