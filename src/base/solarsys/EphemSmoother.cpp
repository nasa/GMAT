//------------------------------------------------------------------------------
//                              EphemSmoother
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
// Created: 2019.04.03
//
/**
 * Original Python prototype:
 * Author: N. Hatten
 *
 * This code was converted from the original python prototype, and includes
 * original commentary.
 */
//------------------------------------------------------------------------------

#include "EphemSmoother.hpp"
#include "SolarSystemException.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "CoordinateTranslation.hpp"
#include "CubicSpline.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_CUBIC_SPLINE
//#define DEBUG_ARRAYS

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------


//---------------------------------------------------------------------------
//  EphemSmoother(Integer defNRegionsPerRev = 359, Integer defNRegions = 359,
//                Real defH = 86400.0, const std::string defStepSizeType = 'h',
//                Real defET0 = 0.0, Real defETf = 31557600.0);
//---------------------------------------------------------------------------
/**
 * Constructs base EphemSmoother structures used in derived classes
 * (default constructor).
 *
 * @param  defNRegionsPerRev  The default number of time regions to use per 
 *                            revolution of the target body about its 
 *                            central body for computing a spline
 * @param  defNRegions        The default number of time regions to use for 
 *                            computing a spline
 * @param  defH               The default step size (in ET seconds) to use when
 *                            computing a spline
 * @param  defStepSizeType    The default method for calculating the time 
 *                            between knots
 * @param  defET0             The default opening time of the spline domain 
 *                            (in ET seconds)
 * @param  defETf             The default closing time of the spline domain 
 *                            (in ET seconds)
 *
 */
//---------------------------------------------------------------------------
EphemSmoother::EphemSmoother(Integer defNRegionsPerRev, Integer defNRegions,
                             Real defH, const std::string defStepSizeType,
                             Real defT0, Real defTf) :
   defaultNumRegionsPerRev (defNRegionsPerRev),
   defaultNumRegions       (defNRegions),
   defaultH                (defH),
   defaultStepSizeType     (defStepSizeType),
   defaultT0               (defT0),
   defaultTf               (defTf)
{
//   cubicSpline = CubicSpline::Instance();  // will grab as needed
}

//---------------------------------------------------------------------------
//  EphemSmoother(const EphemSmoother &sp);
//---------------------------------------------------------------------------
/**
 * Constructs base EphemSmoother structures used in derived classes, by copying
 * the input instance (copy constructor).
 *
 * @param <copy>  EphemSmoother instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
EphemSmoother::EphemSmoother(const EphemSmoother &copy):
   defaultNumRegionsPerRev (copy.defaultNumRegionsPerRev),
   defaultNumRegions       (copy.defaultNumRegions),
   defaultH                (copy.defaultH),
   defaultStepSizeType     (copy.defaultStepSizeType),
   defaultT0               (copy.defaultT0),
   defaultTf               (copy.defaultTf)
{
   ephemDataList.clear();
   for (Integer ii = 0; ii < copy.ephemDataList.size(); ii++)
   {
      ephemDataList.push_back((copy.ephemDataList.at(ii))->Clone());
   }
}


//---------------------------------------------------------------------------
//  EphemSmoother& operator=(const EphemSmoother &copy)
//---------------------------------------------------------------------------
/**
 * operator= for the class.
 *
 * @param <copy> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
EphemSmoother& EphemSmoother::operator=(const EphemSmoother &copy)
{
   if (&copy == this)
      return *this;

   defaultNumRegionsPerRev  = copy.defaultNumRegionsPerRev;
   defaultNumRegions        = copy.defaultNumRegions;
   defaultH                 = copy.defaultH;
   defaultStepSizeType      = copy.defaultStepSizeType;
   defaultT0                = copy.defaultT0;
   defaultTf                = copy.defaultTf;
   
   ephemDataList.clear();
   for (Integer ii = 0; ii < copy.ephemDataList.size(); ii++)
   {
      ephemDataList.push_back((copy.ephemDataList.at(ii))->Clone());
   }
   
   return *this;
}


//---------------------------------------------------------------------------
//  ~EphemSmoother()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
EphemSmoother::~EphemSmoother()
{
   for (Integer ii = 0; ii < ephemDataList.size(); ii++)
   {
      delete ephemDataList.at(ii);
   }
   ephemDataList.clear();
}

//---------------------------------------------------------------------------
//  void GetState(SpacePoint *refBody, SpacePoint *target,
//                CoordinateSystem *refFrame, Real ephemTime,
//                Rvector &state, Rvector &dState, Rvector &ddState)
//---------------------------------------------------------------------------
/**
 * Returns the state
 *
 * @param  refBody      [in] The body with respect to which the state of
 *                      targetBody is calculated
 * @param  target       [in] The body whose state is splined
 * @param  refFrame     [in] The reference frame in which the spline of
 *                      targetBody w.r.t. referenceBody is calculated
 * @param  ephemTime    [in] The time at which to obtain the state in Spice
 *                      ephemeris time
 * @param  state        [out] State at ET
 * @param  dState       [out] First time derivative of state at ET
 * @param  ddState      [out] Second time derivative of state at ET
 *
 */
//---------------------------------------------------------------------------
void EphemSmoother::GetState(SpacePoint *refBody, SpacePoint *target,
                            CoordinateSystem *refFrame, Real ephemTime,
                            Rvector &state, Rvector &dState, Rvector &ddState)
{
   CubicSpline *cubicSpline = CubicSpline::Instance();
   Integer idx = ValidateInterpolationRequest(refBody, target, refFrame,
                                              ephemTime);
   if (idx == -1)  // interpolation data does not already exist
   {
      Real theT0 = defaultT0;
      Real theTf = defaultTf;
      
      if (ephemTime < defaultT0)
         theT0 = ephemTime - 365.25;
      else if (ephemTime > defaultTf)
         theTf = ephemTime + 365.25;
      
      CreateSmoothedEphem(refBody, target, refFrame, theT0, theTf, defaultH,
                          defaultNumRegions, defaultNumRegionsPerRev,
                          defaultStepSizeType);
      idx = ephemDataList.size() - 1; // newly added smoothed ephem
   }
   EphemData *ed = ephemDataList.at(idx);
   cubicSpline->EvaluateClampedCubicSplineVectorized(ed->a, ed->b, ed->c,
                                                     ed->d, ed->theTimes, ephemTime,
                                                     state, dState, ddState);
   // return state, dstate, ddstate via arguments
}


//---------------------------------------------------------------------------
//  void CreateSmoothedEphem(SpacePoint *refBody, SpacePoint *target,
//                           CoordinateSystem *refFrame,
//                           Real theTime0, Real theTimef,
//                           Real stepSize = 1.0,
//                           Integer nRegions = 1,
//                           Integer nRegionsPerRev = 1,
//                           std::string stepSizeType = "h"
//---------------------------------------------------------------------------
/**
 * Creates a smoothed ephem.
 *
 * @param  TBD
 *
 */
//---------------------------------------------------------------------------
void EphemSmoother::CreateSmoothedEphem(SpacePoint *refBody, SpacePoint *target,
                                        CoordinateSystem *refFrame,
                                        Real theTime0, Real theTimef,
                                        Real stepSize,
                                        Integer nRegions,
                                        Integer nRegionsPerRev,
                                        const std::string &stepSizeType)
{
   
   CubicSpline *cubicSpline = CubicSpline::Instance();
   // Assume state is size 6
   Integer nStates = 6;        // ASSUMES state of size 6 here!!
   Integer h2      = stepSize; // local step size
   Integer nKnots  = 0;
   
   // Calculate the spline coefficients
   if (stepSizeType == "nRegions")
   {
      h2     = (theTimef - theTime0) / (Real) nRegions;
      nKnots = nRegions + 1;
   }
   else if (stepSizeType == "nRegionsPerRevolution")
   {
      // NOT WORKING
      throw SolarSystemException("ERROR in EphemSmoother::CreateSmoothedEphem\n");
   }
   else // "h"
   {
      nRegions = (Integer) GmatMathUtil::Ceiling(
                                                 GmatMathUtil::Abs((theTimef-theTime0) / stepSize));
      nKnots   = nRegions + 1;
   }
   
   // Create a new EphemData object and add it to the list
   EphemData *newED = new EphemData(refBody->GetName(), target->GetName(),
                                    refFrame->GetAxisSystem()->GetTypeName(),
                                    nStates, theTime0, theTimef, stepSize,
                                    nRegions, stepSizeType);
   ephemDataList.push_back(newED);
   
   // All operations after this act on the last-added EphemData object,
   // so we can operate on the pointer newED
   
   // Loop through number of points to get data
   Rvector6 stateWRTOldOrigin;     // all zeroes by default
   std::vector<Rvector6>  states;  // will be nknots entries here
   for (Integer ii = 0; ii < nKnots; ii++)
   {
      Real thisTime = theTime0 + h2 * (Real) ii;
      (newED->theTimes).push_back(thisTime); // add the time to the list
      // ASSUMES state of size 6 here!!
      Rvector6 stateAtii = CoordinateTranslation::TranslateOrigin(thisTime,
                                                             target, stateWRTOldOrigin, refBody,
                                                             refFrame);
      states.push_back(stateAtii);
   }
   
   // approximate the derivatives at the end points so we can do a clamped
   // cubic spline using a 4th order finite difference scheme at each end,
   // which requires 5 points, in addition to the end point;
   // take all 4 points to be equally spaced between the end point and the
   // first knot
   Rvector dydx0(nStates);
   Rvector dydxf(nStates);
   Real    hFD  = h2 / 1.0;  // divided by 1?
   
   Rvector timesFDStart(5);
   Rvector timesFDEnd(5);
   
   timesFDStart(0) = newED->theTimes.at(0);         // first on list
   timesFDEnd(4)   = newED->theTimes.at(nRegions);  // nRegions = nKnots - 1; so last one
   
   std::vector<Rvector6> statesFDStart(5); // assumed size 5
   std::vector<Rvector6> statesFDEnd(5);   // assumed size 5
   
   statesFDStart.at(0) = states.at(0);
   statesFDEnd.at(4)   = states.at(nRegions); // nRegions = nKnots - 1; so last one
   
   // Calculate the inermediate states
   for (Integer ii = 1; ii < 5; ii++)
   {
      timesFDStart(ii) = theTime0 + hFD * (Real) ii;
      timesFDEnd(4-ii) = theTimef - hFD * (Real) ii;
      statesFDStart.at(ii) = CoordinateTranslation::TranslateOrigin(timesFDStart(ii),
                                                               target,
                                                               stateWRTOldOrigin, refBody,
                                                               refFrame);
      statesFDEnd.at(4-ii) = CoordinateTranslation::TranslateOrigin(timesFDEnd(4-ii),
                                                               target,
                                                               stateWRTOldOrigin, refBody,
                                                               refFrame);
   }
   // Do the finite differencing
   for (Integer ii = 0; ii < nStates; ii++)
   {
      // Need column vectors here?
      Rvector vecStart(5);
      Rvector vecEnd(5);
      for (Integer jj = 0; jj < 5; jj++)
      {
         vecStart(jj) = statesFDStart.at(jj)(ii);
         vecEnd(jj)   = statesFDEnd.at(jj)(ii);
      }
      dydx0(ii) = cubicSpline->FiniteDifferenceAtEdge(timesFDStart, vecStart, "left");
      dydxf(ii) = cubicSpline->FiniteDifferenceAtEdge(timesFDEnd, vecEnd, "right");
   }
   
   // Make sure the a, b, c, d matrices are the correct size
   (newED->a).SetSize(nRegions,nStates);
   (newED->b).SetSize(nRegions,nStates);
   (newED->c).SetSize(nRegions,nStates);
   (newED->d).SetSize(nRegions,nStates);
   // once we have all the data, loop through the states to spline
   Rvector theState(nKnots);
   Rvector times(newED->theTimes);// convert RealArray to Rvector
   for (Integer ii = 0; ii < nStates; ii++)
   {
      // Need column vector
      for (Integer jj = 0; jj < nKnots; jj++)
      {
         theState(jj) = states.at(jj)(ii);
      }
      Rvector a1, b1, c1, d1;
      cubicSpline->CalculateClampedCubicSplineCoefficients(times, theState,
                                                           dydx0(ii), dydxf(ii),
                                                           a1, b1, c1, d1); // returned
      // put output into the a, b, c, d matrices; these column vectors should
      // be of size nRegions
      for (Integer kk = 0; kk < nRegions; kk++)
      {
         (newED->a)(kk, ii) = a1(kk);
         (newED->b)(kk, ii) = b1(kk);
         (newED->c)(kk, ii) = c1(kk);
         (newED->d)(kk, ii) = d1(kk);
      }
   }
   
   // clean up
   states.clear();
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//---------------------------------------------------------------------------
//  Integer ValidateInterpolationRequest(SpacePoint *refBody,
//                                       SpacePoint *target,
//                                       CoordinateSystem *refFrame,
//                                       Real ephemTime)
//---------------------------------------------------------------------------
/**
 * Validates the interpolation request.
 *
 * @param  refBody      [in] The body with respect to which the state of
 *                      targetBody is calculated
 * @param  target       [in] The body whose state is splined
 * @param  refFrame     [in] The reference frame in which the spline of
 *                      targetBody w.r.t. referenceBody is calculated
 * @param  ephemTime    [in] The time at which to obtain the state in Spice
 *                      ephemeris time
 *
 * @return  If interpolation data already exists, validIdx is its index. 
 *          Otherwise, returns -1
 *
 */
//---------------------------------------------------------------------------
Integer EphemSmoother::ValidateInterpolationRequest(SpacePoint *refBody,
                                                    SpacePoint *target,
                                                    CoordinateSystem *refFrame,
                                                    Real ephemTime)
{
   Integer idx = -1;
   AxisSystem *refAxisSys = refFrame->GetAxisSystem();
   for (Integer ii = 0; ii < ephemDataList.size(); ii++)
   {
      EphemData *ed = ephemDataList.at(ii);
      if ((refBody->GetName()  == ed->referenceBodyName)       &&
          (target->GetName()   == ed->targetBodyName)          &&
          (refAxisSys->GetTypeName() == ed->referenceFrame)    &&
          (ephemTime >= ed->time0)                             &&
          (ephemTime <= ed->timef))
      {
         idx = ii;
         break;
      }
   }
   return idx;
}

