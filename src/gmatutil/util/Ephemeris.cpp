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

#include "Ephemeris.hpp"

#include "LagrangeInterpolator.hpp"
#include "HermiteInterpolator.hpp"
#include "GmatConstants.hpp"           // For SECS_PER_DAY
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

#include <sstream>

//#define TEST_HERMITE_INTERP


//------------------------------------------------------------------------------
// Ephemeris()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
Ephemeris::Ephemeris() :
   ephemerisFileName             (""),
   a1StartEpoch                  (-1.0),
   a1EndEpoch                    (999999.0),
   interp                        (NULL),
   order                         (7),
   currentOrder                  (-1),
   warnInterpolationDegradation  (true),
   useHermite                    (true)
{
   #ifdef TEST_HERMITE_INTERP
      // Temporary code to test the Hermite interpolator
      interp = new HermiteInterpolator("", 3, 3);

      MessageInterface::ShowMessage("\n*** Testing the Hermite interpolator\n");

      Real t, val[3];
      for (int i = 0; i < 3; ++i)
      {
         t = i / 2.0;
         val[0] = sin(t * 3.14159);
         val[1] = t*t*t*t - 2.15*t + 0.3;
         val[2] = sin(t * 3.14159);
         interp->AddPoint(t, val);
      }

      t = 0.25;
      interp->Interpolate(t, val);
      MessageInterface::ShowMessage("   H(%lf), no dv = [%.10le %.10le %.10le]\n", t, val[0], val[1], val[2]);

      for (int i = 0; i < 3; ++i)
      {
         t = i / 2.0;
         val[0] = cos(t * 3.14159);
         val[1] = -9.9999999999e99; // 4.0*t*t*t - 2.15;
         val[2] = cos(t * 3.14159);
         ((HermiteInterpolator*)interp)->AddDerivative(t, val);
      }

      t = 0.25;
      interp->Interpolate(t, val);
      MessageInterface::ShowMessage("   H(%lf) =  [%.10le %.10le %.10le]\n", t, val[0], val[1], val[2]);

      delete interp;
      interp = NULL;
   #endif
}

//------------------------------------------------------------------------------
// ~Ephemeris()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Ephemeris::~Ephemeris()
{
   if (interp)
      delete interp;
}

//------------------------------------------------------------------------------
// Ephemeris(const Ephemeris& ephem) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ephem The object used to set properties for this new one
 */
//------------------------------------------------------------------------------
Ephemeris::Ephemeris(const Ephemeris& ephem) :
   ephemerisFileName             (ephem.ephemerisFileName),
   a1StartEpoch                  (-1.0),
   a1EndEpoch                    (999999.0),
   interp                        (NULL),
   order                         (ephem.order),
   currentOrder                  (-1),
   warnInterpolationDegradation  (true),
   useHermite                    (ephem.useHermite)
{
}

//------------------------------------------------------------------------------
// Ephemeris& operator=(const Ephemeris& ephem)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ephem The object being copied
 *
 * @return This object configured to match ephem
 */
//------------------------------------------------------------------------------
Ephemeris& Ephemeris::operator=(const Ephemeris& ephem)
{
   if (this != &ephem)
   {
      ephemerisFileName = ephem.ephemerisFileName;
      a1StartEpoch = -1.0;
      a1EndEpoch   = 999999.0;
      if (interp)
         delete interp;
      interp       = NULL;
      order        = ephem.order;
      currentOrder = -1;

      warnInterpolationDegradation = true;
      useHermite                   = ephem.useHermite;
      segmentStartTimes.clear();
   }

   return *this;
}

//------------------------------------------------------------------------------
// const RealArray& GetSegmentTimes()
//------------------------------------------------------------------------------
/**
 * Access method used to find the start of the ephem segments.
 *
 * All ephemerides have one or more segments.  Subclasses define how this
 * segmentation occurs.
 *
 * @return An array of the epoch for the start of each segment
 */
//------------------------------------------------------------------------------
const RealArray& Ephemeris::GetSegmentTimes()
{
   return segmentStartTimes;
}


//------------------------------------------------------------------------------
// Integer FindSegment(const GmatEpoch forEpoch)
//------------------------------------------------------------------------------
/**
 * Method used to find the segment of the ephem containing an epoch
 *
 * @param forEpoch The epoch
 *
 * @return The index of the segment, or -1 if there is no segment with the epoch
 */
//------------------------------------------------------------------------------
Integer Ephemeris::FindSegment(const GmatEpoch forEpoch)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < theEphem.size(); ++i)
   {
      #ifdef DEBUG_INTERPOLATION
         MessageInterface::ShowMessage("Checking if %.12lf is between %.12lf and %.12lf\n",
               forEpoch, theEphem[i].segStart, theEphem[i].segEnd);
      #endif

      if ((theEphem[i].segStart <= forEpoch) && (forEpoch < theEphem[i].segEnd))
      {
         retval = i;
         break;
      }

      // Special case: Only one point in the segment
      if ((theEphem[i].segStart == forEpoch) && (forEpoch == theEphem[i].segEnd))
         retval = i;
   }

   // Handle the last point on the ephemeris
   if (forEpoch == a1EndEpoch)
      retval = theEphem.size() - 1;

   return retval;
}

//------------------------------------------------------------------------------
// Integer PointsInSegment(const Integer forSegment)
//------------------------------------------------------------------------------
/**
 * Returns the number of points in a data segment
 *
 * @param forSegment The index of the segment
 *
 * @return The number of points, or -1 if the segment is not in the ephem
 */
//------------------------------------------------------------------------------
Integer Ephemeris::PointsInSegment(const Integer forSegment)
{
   Integer retval = -1;

   if ((forSegment >= 0) && (theEphem.size() > forSegment))
      retval = theEphem[forSegment].points.size();

   return retval;
}

//------------------------------------------------------------------------------
// Integer IndexInSegment(const Integer segNum, const GmatEpoch forEpoch)
//------------------------------------------------------------------------------
/**
 * Locates the index of the ephem point closest to the input epoch
 *
 * @param segNum The index of the segment containing the epoch
 * @param forEpoch The epoch
 *
 * @return The index
 */
//------------------------------------------------------------------------------
Integer Ephemeris::IndexInSegment(const Integer segNum, const GmatEpoch forEpoch)
{
   Integer retval = -1;

   Real diff = 9e99, temp;
   for (UnsignedInt i = 0; i < theEphem[segNum].points.size(); ++i)
   {
      temp = GmatMathUtil::Abs(theEphem[segNum].points[i].theEpoch - forEpoch);
      if (temp < diff)
      {
         diff = temp;
         retval = i;
      }
   }

   return retval;
}


////------------------------------------------------------------------------------
//// void GetPoint(GmatEpoch &epoch, Real *posvel, const Integer pointNumber,
////       const Integer forSegment)
////------------------------------------------------------------------------------
///**
// * Retrieves a specific ephem point
// *
// * @param epoch A.1 epoch of the retrieved point, set in the call
// * @param posvel Position and velocity of the retrieved point
// * @param pointNumber Index of the ephem point in the segment
// * @param forSegment Index of the segmetn (defaults to 0)
// *
// * @note: Not used in the current implementation
// */
//------------------------------------------------------------------------------
//void Ephemeris::GetPoint(GmatEpoch &epoch, Real *posvel,
//      const Integer pointNumber, const Integer forSegment)
//{
//}


void Ephemeris::GetInterpolationInfoFromSegment(Integer segmentNo)
{
}


//------------------------------------------------------------------------------
// Rvector6 InterpolatePoint(const GmatEpoch forEpoch)
//------------------------------------------------------------------------------
/**
 * Interpolated the ephemeris to retrieve a state at the input epoch
 *
 * @param forEpoch The epoch
 *
 * @return The position and velocity from the ephemeris
 */
//------------------------------------------------------------------------------
Rvector6 Ephemeris::InterpolatePoint(const GmatEpoch forEpoch)
{
   Rvector6 retval;

   Integer segNo = FindSegment(forEpoch);
   if (segNo == -1)
   {
      std::stringstream msg;
      msg.precision(15);
      msg << "**** ERROR **** Attempting to interpolate outside of the span of the ephemeris; "
          << "Requested epoch is " << forEpoch << " and ephemeris span is from "
          << a1StartEpoch << " to " << a1EndEpoch;
      throw UtilityException(msg.str());
   }

   Integer index = IndexInSegment(segNo, forEpoch);
   if (index == -1)
   {
      std::stringstream msg;
      msg.precision(15);
      msg << "**** ERROR **** Unable to find the epoch " << forEpoch << " in the ephemeris span from "
          << a1StartEpoch << " to " << a1EndEpoch;
      throw UtilityException(msg.str());
   }

   GetInterpolationInfoFromSegment(segNo);

   Integer maxOrder = theEphem[segNo].points.size() - 1;
   if (maxOrder > order)
      maxOrder = order;

   if (currentOrder != maxOrder)
   {
      #ifdef DEBUG_INTERPOLATION
         MessageInterface::ShowMessage("Setting interpolator of order %d\n",
               maxOrder);
      #endif

      if (interp != NULL)
         delete interp;
      if (useHermite)
         interp = new HermiteInterpolator("", 6, maxOrder);
      else
         interp = new LagrangeInterpolator("", 6, maxOrder);
      currentOrder = maxOrder;
   }

   if ((currentOrder < order) && warnInterpolationDegradation)
   {
      MessageInterface::ShowMessage("**** WARNING **** Interpolation for the "
            "ephemeris file %s is being performed using a lower order "
            "interpolator (order %d on the first change) than usual\n",
            ephemerisFileName.c_str(), currentOrder);
      warnInterpolationDegradation = false;
   }

   Integer startIndex = index - Integer(currentOrder / 2);
   if (startIndex < 0)
      startIndex = 0;
   if (startIndex + currentOrder + 1 > theEphem[segNo].points.size())
      startIndex = theEphem[segNo].points.size() - currentOrder - 1;

   interp->Clear();
   for (UnsignedInt i = 0; i <= currentOrder; ++i)
      interp->AddPoint(theEphem[segNo].points[startIndex+i].theEpoch,
            theEphem[segNo].points[startIndex+i].posvel.GetDataVector());

   // Use derivative data for problems with lower than 7th order polynomials
   if ((useHermite) && (currentOrder < 7))
   {
      Real vel[6];
      for (UnsignedInt i = 0; i <= currentOrder; ++i)
      {
         const Real *v = theEphem[segNo].points[startIndex+i].posvel.GetDataVector();
         for (Integer j = 0; j < 3; ++j)
         {
            // Since independent variable is in days, scale velocity the same
            vel[j] = v[j+3] * GmatTimeConstants::SECS_PER_DAY;
            vel[j+3] = -9.999999999e99;
         }
         ((HermiteInterpolator*)interp)->AddDerivative(
               theEphem[segNo].points[startIndex+i].theEpoch, vel);
      }
   }

   Real interpolents[6];

   if (useHermite)
   {
      if (((HermiteInterpolator*)interp)->InterpolateCartesianState(forEpoch, interpolents))
      {
         for (Integer i = 3; i < 6; ++i)
            interpolents[i] /= GmatTimeConstants::SECS_PER_DAY;
         retval.Set(interpolents);
      }
   }
   else
   {
      if (interp->Interpolate(forEpoch, interpolents))
         retval.Set(interpolents);
   }
   return retval;
}
