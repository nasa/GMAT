//$Id$
//------------------------------------------------------------------------------
//                           Ephemeris
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Dec 20, 2017
/**
 * Ephemeris base class components
 */
//------------------------------------------------------------------------------

#ifndef Ephemeris_hpp
#define Ephemeris_hpp

#include "utildefs.hpp"       // Change to gmatutil for R2018a
#include "Rvector6.hpp"
#include "Interpolator.hpp"

/**
 * Base class for the ephemeris file components.
 *
 * @note: "EphemerisFile" is the subscriber used to create Ephemeris objects
 */
class GMATUTIL_API Ephemeris
{
public:
   Ephemeris();
   virtual ~Ephemeris();
   Ephemeris(const Ephemeris& ephem);
   Ephemeris& operator=(const Ephemeris& ephem);

   const RealArray& GetSegmentTimes();
   virtual Integer  FindSegment(const GmatEpoch forEpoch);
   virtual Integer  PointsInSegment(const Integer forSegment);
   virtual Integer  IndexInSegment(const Integer segNum, const GmatEpoch forEpoch);
//   virtual void     GetPoint(GmatEpoch &epoch, Real *posvel,
//         const Integer pointNumber, const Integer forSegment = 0);
   virtual Rvector6 InterpolatePoint(const GmatEpoch forEpoch);
   virtual void     GetInterpolationInfoFromSegment(Integer segmentNo);

   /// Structure containing the minimal data GMAT needs for an ephem
   struct EphemPoint
   {
      GmatEpoch               theEpoch;
      Rvector6                posvel;
   };

   /// A container for spans of an ephem.
   struct Segment
   {
      GmatEpoch               segStart;
      GmatEpoch               segEnd;
      std::vector<EphemPoint> points;
   };

   /// The full ephem, consisting of one or more segments
   std::vector<Segment>       theEphem;

protected:
   /// @note: Add a precision time version of times for R2019a?

   /// Name of the ephemeris file
   std::string ephemerisFileName;

   /// Initial epoch of the ephem
   GmatEpoch      a1StartEpoch;
   /// Initial epoch of the ephem
   GmatEpoch      a1EndEpoch;
   /// List of epochs that mark the start of a data segment
   RealArray      segmentStartTimes;

   /// Interpolator used for epochs that are not points in the ephem
   Interpolator   *interp;
   /// Interpolation order
   Integer order;
   /// Order of the current interpolator
   Integer currentOrder;
   /// Flag used to alert user - once! -  that the interpolation has degraded
   bool warnInterpolationDegradation;
   /// Flag to toggle between Lagrange and Hermite interpolation
   bool useHermite;
};

#endif /* Ephemeris_hpp */
