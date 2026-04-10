//------------------------------------------------------------------------------
//                           CoverageChecker
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.05.10
//
/**
 * Implementation of the CoverageChecker class
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "CoverageChecker.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "Rmatrix33.hpp"
#include "TATCException.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_COV_CHECK
//#define DEBUG_COV_CHECK_FOV
//#define DEBUG_GRID

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// @todo DON'T hard-code body radius
const Real CoverageChecker::BODY_RADIUS = 6378.1363;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  CoverageChecker(PointGroup *ptGroup, Spacecraft *sat)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param ptGroup  pointer to the PointGroup object to use
 * @param sat      pointer to the Spacecraft object to use
 */
//------------------------------------------------------------------------------
CoverageChecker::CoverageChecker(PointGroup *ptGroup, Spacecraft *sat) :
   pointGroup        (ptGroup),
   sc                (sat),
   centralBody       (NULL),
   timeIdx           (-1) ,
   coverageStart     (0.0),
   coverageEnd       (0.0)
{
   timeSeriesData.clear();
   
   dateData.clear();
   numEventsPerPoint.clear();
   pointArray.clear();
   feasibilityTest.clear();
   computePOIGeometryData = false;

   centralBody    = new Earth();
   Integer numPts = pointGroup->GetNumPoints();
   IntegerArray emptyIntArray;  // empty array
   VisiblePOIReport emtpyReport;
   discreteEventData.resize(numPts);
   for (Integer ii = 0; ii < numPts; ii++)
   {
      timeSeriesData.push_back(emptyIntArray);
      std::vector < VisiblePOIReport > emptyPOIVector;
      discreteEventData[ii].push_back(emtpyReport);
//      dateData.push_back(noDate); // want to accumulate these as we go along
      numEventsPerPoint.push_back(0);
      
      /// @TODO This should not be set here - we should store both
      /// positions and unitized positions in the PointGroup and
      /// then access those arrays when needed <<<<<<<<<<<<<<
      Rvector3 *ptPos1  = pointGroup->GetPointPositionVector(ii);
      Rvector3 *posUnit = new Rvector3(ptPos1->GetUnitVector());
      #ifdef DEBUG_GRID
         MessageInterface::ShowMessage("CovCheck: posUnit = %s\n",
                                       posUnit->ToString(12).c_str());
      #endif
      pointArray.push_back(posUnit);
      feasibilityTest.push_back(false);
   }
}

//------------------------------------------------------------------------------
//  CoverageChecker(const CoverageChecker &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param copy  the object to copy
 * 
 */
//------------------------------------------------------------------------------
CoverageChecker::CoverageChecker(const CoverageChecker &copy) :
   pointGroup        (copy.pointGroup),
   sc                (copy.sc),
   centralBody       (copy.centralBody),
   timeIdx           (copy.timeIdx),
   coverageStart     (copy.coverageStart),  // or 0.0?
   coverageEnd       (copy.coverageEnd)     // or 0.0?
{
   timeSeriesData.clear();
   for (Integer ii = 0; ii < copy.timeSeriesData.size(); ii++)
   {
      IntegerArray ia = copy.timeSeriesData.at(ii);
      timeSeriesData.push_back(ia);
   }

   dateData.clear();
   for (Integer dd = 0; dd < copy.dateData.size(); dd++)
      dateData.push_back(copy.dateData.at(dd));
   
   numEventsPerPoint.clear();
   for (Integer nn = 0; nn < copy.numEventsPerPoint.size(); nn++)
      numEventsPerPoint.push_back(copy.numEventsPerPoint.at(nn));

   for (Integer ii = 0; ii < pointArray.size(); ii++)
      delete pointArray.at(ii);
   pointArray.clear();
   for (Integer ii = 0; ii < copy.pointArray.size(); ii++)
   {
      // these Rvector3s are coordinates (x,y,z)
      Rvector3 *rv = new Rvector3(*copy.pointArray.at(ii));
      pointArray.push_back(rv);
   }
   feasibilityTest.clear();
   for (Integer ff = 0; ff < copy.feasibilityTest.size(); ff++)
      feasibilityTest.push_back(copy.feasibilityTest.at(ff));

   computePOIGeometryData = copy.computePOIGeometryData;

}

//------------------------------------------------------------------------------
//  CoverageChecker& operator=(const CoverageChecker &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the CoverageChecker object
 *
 * @param copy  the object to copy
 * 
 */
//------------------------------------------------------------------------------
CoverageChecker& CoverageChecker::operator=(const CoverageChecker &copy)
{
   if (&copy == this)
      return *this;
   
   pointGroup        = copy.pointGroup;
   sc                = copy.sc;
   centralBody       = copy.centralBody;
   timeIdx           = copy.timeIdx;
   coverageStart     = copy.coverageStart;
   coverageEnd       = copy.coverageEnd;

   timeSeriesData.clear();
   for (Integer ii = 0; ii < copy.timeSeriesData.size(); ii++)
   {
      IntegerArray ia = copy.timeSeriesData.at(ii);
      timeSeriesData.push_back(ia);
   }

   dateData.clear();
   for (Integer dd = 0; dd < copy.dateData.size(); dd++)
      dateData.push_back(copy.dateData.at(dd));
   
   numEventsPerPoint.clear();
   for (Integer nn = 0; nn < copy.numEventsPerPoint.size(); nn++)
      numEventsPerPoint.push_back(copy.numEventsPerPoint.at(nn));

   for (Integer ii = 0; ii < pointArray.size(); ii++)
      delete pointArray.at(ii);
   pointArray.clear();
   for (Integer ii = 0; ii < copy.pointArray.size(); ii++)
   {
      // these Rvector3s are coordinates (x,y,z)
      Rvector3 *rv = new Rvector3(*copy.pointArray.at(ii));
      pointArray.push_back(rv);
   }
   feasibilityTest.clear();
   for (Integer ff = 0; ff < copy.feasibilityTest.size(); ff++)
      feasibilityTest.push_back(copy.feasibilityTest.at(ff));

   computePOIGeometryData = copy.computePOIGeometryData;

   return *this;
}

//------------------------------------------------------------------------------
//  ~CoverageChecker()
//------------------------------------------------------------------------------
/**
 * Destructor
 * 
 */
//------------------------------------------------------------------------------
CoverageChecker::~CoverageChecker()
{
   delete centralBody;
   for(int x = 0; x < pointArray.size(); x++)
   {
      delete pointArray[x];
   }
}


//------------------------------------------------------------------------------
// IntegerArray CheckPointCoverage(const Rvector6 &theState,
//                                 Real           theTime,
//                                 const Rvector6 &cartState)
//------------------------------------------------------------------------------
/**
 * Checks the point coverage.
 *
 * @return  array of indexes 
 * 
 */
//------------------------------------------------------------------------------
IntegerArray CoverageChecker::CheckPointCoverage(const Rvector6 &theState,
                                                 Real           theTime, 
                                                 const Rvector6 &cartState)   
{
   #ifdef DEBUG_COV_CHECK
      MessageInterface::ShowMessage("In CoverageChecker::CheckPointCoverage\n");
      MessageInterface::ShowMessage("  theState = %s\n",
                                    theState.ToString(12).c_str());
   #endif
   // Check coverage given a spacecraft location in body fixed coordinates
   IntegerArray  result;
   AbsoluteDate  aDate;
   aDate.SetJulianDate(theTime);
   
   Rvector6      centralBodyFixedState = theState;
   Integer       covCount              = 0;
   Rvector3      centralBodyFixedPos(centralBodyFixedState[0],
                                     centralBodyFixedState[1],
                                     centralBodyFixedState[2]);
   Rvector3      centralBodyFixedVel(centralBodyFixedState[3],
                                     centralBodyFixedState[4],
                                     centralBodyFixedState[5]);

   Rvector3 rangeVector;
   Integer  numPts = pointGroup->GetNumPoints();
   
   #ifdef DEBUG_COV_CHECK
      MessageInterface::ShowMessage(" --- numPoints from pointGroup = %d\n",
                                     numPts);
      MessageInterface::ShowMessage(" --- Checking Feasibility ...\n");
   #endif
   
   CheckGridFeasibility(centralBodyFixedPos);
   for ( Integer pointIdx = 0; pointIdx < numPts; pointIdx++)
   {
      // Simple line of site test for each point
//      if (CheckGridFeasibility(pointIdx, centralBodyFixedPos)) // this is slower
      if (feasibilityTest.at(pointIdx)) //  > 0)
      {
         #ifdef DEBUG_COV_CHECK
            MessageInterface::ShowMessage(
                              " --- feasibilty at point %d is TRUE!\n",
                              pointIdx);
         #endif

         Integer  sensorNum = 0; // 1; // Currently only works for one sensor!!

         bool     inView    = false;
         Rvector3 pointLocation = (*pointArray.at(pointIdx)) *
                                   centralBody->GetRadius();
         Rvector3 satToTargetVec = pointLocation - centralBodyFixedPos;

//         Rmatrix33 R_fixed_to_nadir =
//                   sc->GetBodyToInertialMatrix(centralBodyFixedState);
//         Rvector3  viewVector = R_fixed_to_nadir * satToTargetVec;

         if (sc->HasSensors())
         {
            // NOTE - should be sc->ChTV(satToTargetVec, state, sensorNum);
            // Then the sc computes the R_NI, R_BN, and calls the sensor for
            // the R_SB to detmine the visibility
//            inView = sc->CheckTargetVisibility(viewVector, sensorNum);
            inView = sc->CheckTargetVisibility(theState, satToTargetVec,
                                               theTime,  sensorNum);
         }
         else
         {
            // No sensor, just perform horizon test
            rangeVector              = -satToTargetVec;
            Real rangeMag            = rangeVector.GetMagnitude();
            Real bodyFixedMag        = centralBodyFixedState.GetMagnitude();
            Real cosineOffNadirAngle = rangeVector * centralBodyFixedPos /
                                       rangeMag / bodyFixedMag;
            Real offNadirAngle       = GmatMathUtil::ACos(cosineOffNadirAngle);
            if ((offNadirAngle < (GmatMathConstants::PI_OVER_TWO -
                                  GmatMathUtil::ACos(BODY_RADIUS/bodyFixedMag)))
                                  && rangeVector(2) > 0.0)
               inView = true;
            else
               inView = false;
         }

         #ifdef DEBUG_COV_CHECK
            MessageInterface::ShowMessage(
                            " --- In CheckPointCoverage, centralBodyFixedState = %s\n",
                            centralBodyFixedState.ToString(12).c_str());
         #endif
         
         #ifdef DEBUG_COV_CHECK_FOV
            MessageInterface::ShowMessage(
                      " --- In CheckPointCoverage, pointArray = %s\n",
                      (pointArray.at(pointIdx))->ToString(12).c_str());
            MessageInterface::ShowMessage(
                      " --- In CheckPointCoverage, centralBodyFixedState = %s\n",
                      centralBodyFixedState.ToString(12).c_str());
            MessageInterface::ShowMessage(
                      " --- In CheckPointCoverage, rangeVector = %s\n",
                      rangeVector.ToString(12).c_str());
//            MessageInterface::ShowMessage(
//                      " --- In CheckPointCoverage, offNadirAngle =  %12.10f\n",
//                      offNadirAngle);
         #endif
         if (inView)
         {
            result.push_back(pointIdx);   // covCount'th entry
            covCount++;
            numEventsPerPoint.at(pointIdx) = numEventsPerPoint.at(pointIdx) + 1;
            timeSeriesData.at(pointIdx).push_back(timeIdx);
            if (computePOIGeometryData)
            {

               // Compute Azimuth Angle, Zenith Angle, Range of the spacecraft
               // w/r/t coverage point
               Real lat;
               Real lon;
               pointGroup->GetLatAndLon(pointIdx,lat,lon);
               Rvector3 topoRangeVec =
                     centralBody->FixedToTopocentric(-satToTargetVec, lat, lon);
               Real xLocal = topoRangeVec[0];
               Real yLocal = topoRangeVec[1];
               Real zLocal = topoRangeVec[2];
               // theta is the angle of the range vector, measured in the
               // xy plane, from the x-axis
               // obsAzimuthAngle is the angle of the range vector,
               // measured in the xy-plane,
               // from the minus x-axis, measured counter clockwise
               Real theta = GmatMathUtil::Mod(
                            GmatMathUtil::ATan2(yLocal, xLocal), 2 *
                            GmatMathConstants::PI);
               Real obsAzimuthAngle = GmatMathUtil::Mod(
                                      GmatMathConstants::PI - theta, 2 *
                                      GmatMathConstants::PI);
               Real obsRange = topoRangeVec.GetMagnitude();
               Real obsZenithAngle = GmatMathUtil::ASin(GmatMathUtil::Sqrt(
                                    xLocal*xLocal + yLocal*yLocal) / obsRange);

               // Compute Azimuth Angle, Zenith Angle of the sun w/r/t coverage point
               Rvector3 sunVecFixed = centralBody->GetSunPositionInBodyCoords(
                                                   theTime, "Cartesian");
               Rvector3 sunVecTopo = centralBody->FixedToTopocentric(sunVecFixed, lat, lon);
               Real xSunTopo = sunVecTopo[0];
               Real ySunTopo = sunVecTopo[1];
               Real sunRange = sunVecTopo.GetMagnitude();
               Real suntheta = GmatMathUtil::Mod(GmatMathUtil::ATan2(
                               ySunTopo, xSunTopo), 2 * GmatMathConstants::PI);
               Real sunAz = GmatMathUtil::Mod(GmatMathConstants::PI - suntheta,
                                              2 * GmatMathConstants::PI);
               Real sunZenithAngle = GmatMathUtil::ASin(GmatMathUtil::Sqrt(
                                     xSunTopo*xSunTopo + ySunTopo*ySunTopo) /
                                     sunRange);

               // create VisiblePOIReport object and set the data
               VisiblePOIReport visReport;
               visReport.SetEndDate(aDate);
               visReport.SetStartDate(aDate);
               visReport.SetPOIIndex(pointIdx);
               visReport.SetObsRange(obsRange);
               visReport.SetObsAzimuth(obsAzimuthAngle );
               visReport.SetObsZenith(obsZenithAngle);
               visReport.SetSunAzimuth(sunAz);
               visReport.SetSunZenith(sunZenithAngle);
               Rvector3 inertialPos   = cartState.GetR();
               Rvector3 inertialVel   = cartState.GetV();
               visReport.SetObsPosInertial(inertialPos);
               visReport.SetObsVelInertial(inertialVel);
               discreteEventData[pointIdx].push_back(visReport);
            }
            #ifdef DEBUG_COV_CHECK
               MessageInterface::ShowMessage(
                                    " --- In CheckPointCoverage, setting "
                                    "numEventsPerPoint (%d) to %d\n",
                                    pointIdx, numEventsPerPoint.at(pointIdx));
               MessageInterface::ShowMessage(
                                " --- Added timeIdx %d to timeSeriesData(%d)\n",
                                timeIdx, pointIdx);
            #endif
         }
      }
   }
   #ifdef DEBUG_COV_CHECK
      for (Integer ii = 0; ii < pointGroup->GetNumPoints(); ii++)
         MessageInterface::ShowMessage(" --- numEventsPerPoint(%d) = %d\n",
                                       ii, numEventsPerPoint.at(ii));
   #endif

   return result;
}

//------------------------------------------------------------------------------
// IntegerArray AccumulateCoverageData()
//------------------------------------------------------------------------------
/**
 * Accumulates the coverage data after the propagation update
 *
 * @return  array of indexes
 *
 */
//------------------------------------------------------------------------------
IntegerArray CoverageChecker::AccumulateCoverageData()
{
   // Accumulates coverage data after propagation update
   // Get the state and date here
   Real     theDate   = sc->GetJulianDate();
   Rvector6 cartState = sc->GetCartesianState();
   Rvector6 theState  = GetEarthFixedSatState(theDate, cartState);
   dateData.push_back(theDate);
   timeIdx++;
   return CheckPointCoverage(theState, theDate, cartState);
}

//------------------------------------------------------------------------------
// IntegerArray AccumulateCoverageData(Real atTime)
//------------------------------------------------------------------------------
/**
 * Accumulates the coverage data after the propagation update
 *
 * @return  array of indexes
 *
 */
//------------------------------------------------------------------------------
IntegerArray CoverageChecker::AccumulateCoverageData(Real atTime)
{
#ifdef DEBUG_COV_CHECK
   MessageInterface::ShowMessage(
            " --- In AccumulateCoverageData, atTime = %12.10f:\n",
            atTime);
#endif
   // Accumulates coverage data after propagation update
   // Get the state here
   Rvector6 cartState = sc->Interpolate(atTime);
   Rvector6 theState  = GetEarthFixedSatState(atTime, cartState);
   dateData.push_back(atTime);
   timeIdx++;
   return CheckPointCoverage(theState, atTime, cartState);
}

//------------------------------------------------------------------------------
// Rvector6 GetEarthFixedSatState(Real jd, const Rvector6& scCartState)
//------------------------------------------------------------------------------
/**
 * Returns the Earth-Fixed state at the specified time
 * 
 * @param jd  Julian date 
 *
 * @return  earth-fixed state at the input time
 * 
 */
//------------------------------------------------------------------------------
Rvector6 CoverageChecker::GetEarthFixedSatState(Real jd,
                                                const Rvector6& scCartState)
{
   // Converts state from Earth interial to Earth fixed
   Rvector3 inertialPos   = scCartState.GetR();
   Rvector3 inertialVel   = scCartState.GetV();
   // TODO.  Handle differences in units of points and states.
   // TODO.  This ignores omega cross r term in velocity, which is ok and 
   // perhaps desired for current use cases but is not always desired.
   Rvector3 centralBodyFixedPos  = centralBody->GetBodyFixedState(inertialPos,
                                                                  jd);
   Rvector3 centralBodyFixedVel  = centralBody->GetBodyFixedState(inertialVel,
                                                                  jd);
   Rvector6 earthFixedState(centralBodyFixedPos(0), centralBodyFixedPos(1),
                            centralBodyFixedPos(2),
                            centralBodyFixedVel(0), centralBodyFixedVel(1),
                            centralBodyFixedVel(2));
   return earthFixedState;
}

//------------------------------------------------------------------------------
// std::vector<VisiblePOIReport> ProcessCoverageData()
//------------------------------------------------------------------------------
/**
 * Returns an array of reports of coverage
 * 
 * @return  array of reports of coverage
 * 
 */
//------------------------------------------------------------------------------
std::vector<IntervalEventReport> CoverageChecker::ProcessCoverageData()
{
   #ifdef DEBUG_COV_CHECK
      MessageInterface::ShowMessage(
                        " --- In ProcessCoverageData, feasibilityTest:\n");
      for (Integer ii = 0; ii < feasibilityTest.size(); ii++)
         MessageInterface::ShowMessage(" ... %d ...    %s\n",
                           ii, (feasibilityTest.at(ii)? "TRUE" : "false"));
   #endif
   std::vector<IntervalEventReport> reports;
   
   Integer numCoverageEvents = 0;
   Integer numPts            = pointGroup->GetNumPoints();
   Integer numEvents         = 0;
   Real    startTime;
   Real    endTime;
   
   #ifdef DEBUG_COV_CHECK
      MessageInterface::ShowMessage(
                        " --- In ProcessCoverageData, numPts = %d\n",
                        numPts);
   #endif
   for (Integer pointIdx = 0; pointIdx < numPts; pointIdx++)
   {

      // Only perform if there are interval events (2 or more events)
       bool isEnd = false;
       numEvents  = numEventsPerPoint.at(pointIdx);
       std::vector<VisiblePOIReport> discreteEvents;
       #ifdef DEBUG_COV_CHECK
          MessageInterface::ShowMessage(
                           " --- In ProcessCoverageData, numEvents (%d) = %d\n",
                           pointIdx, numEvents);
       #endif

       if (numEvents >= 2)
       {
           startTime = dateData.at((timeSeriesData.at(pointIdx)).at(0));
          
           for (Integer dateIdx = 1; dateIdx < numEvents; dateIdx++)
           {
              //Accumlate discrete event for this point
              discreteEvents.push_back(discreteEventData[pointIdx][dateIdx]);

              // Test for end of an interval
              Integer atIdx     = (timeSeriesData.at(pointIdx)).at(dateIdx);
              Integer atPrevIdx = (timeSeriesData.at(pointIdx)).at(dateIdx-1);
              if ((atIdx - atPrevIdx) != 1)
              {
                  endTime = dateData.at(atPrevIdx);
                  isEnd = true;
              }
              // Test for the last event for this point
               else if (dateIdx == (numEvents-1))
               {
                   endTime = dateData.at(atIdx);
                   isEnd = true;
               }
              // otherwise, endTime is not set!
               if (isEnd)
               {
                   IntervalEventReport poiReport = CreateNewPOIReport(startTime,
                                                   endTime,pointIdx);
                   numCoverageEvents++;
                   poiReport.SetAllPOIEvents(discreteEvents);
                   reports.push_back(poiReport);
                   startTime = dateData.at(atIdx);
                   isEnd = false;
                   // Clear discrete events for the pass
                   discreteEvents.clear();
               }
           }
       }
   }
   return reports;
}

//------------------------------------------------------------------------------
// void SetComputePOIGeometryData(bool flag)
//------------------------------------------------------------------------------
/**
 * Sets the flag indficating whether or not to compute the POI Geometry data
 *
 * @param flag compute the POI geometry data?
 *
 */
//------------------------------------------------------------------------------
void CoverageChecker::SetComputePOIGeometryData(bool flag)
{
    computePOIGeometryData = flag;
    return;
}


//------------------------------------------------------------------------------
// VisiblePOIReport CreateNewPOIReport(Real startJd, Real endJd, Integer poiIdx)
//------------------------------------------------------------------------------
/**
 * Creates a new report of coverage data.
 * 
 * @param startJd  start Julian date for the reportSetComputePOIGeometryData
 * @param endJd    end Julian date for the report
 * @param poiIndex POI index for the created report
 * 
 * @return  report of coverage
 * 
 */
//------------------------------------------------------------------------------
IntervalEventReport CoverageChecker::CreateNewPOIReport(Real startJd, Real endJd,
                                                        Integer poiIdx)
{
   // Creates VisiblePOIReport given point indeces and start/end dates
   IntervalEventReport poiReport;
   AbsoluteDate        startEpoch;
   AbsoluteDate        endEpoch;
   
   poiReport.SetPOIIndex(poiIdx);
   startEpoch.SetJulianDate(startJd);
   endEpoch.SetJulianDate(endJd);
   poiReport.SetStartDate(startEpoch);
   poiReport.SetEndDate(endEpoch);
   return poiReport;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool CheckGridFeasibility(Integer         ptIdx,,
//                           const Rvector3& bodyFixedState)
//------------------------------------------------------------------------------
/**
 * Checks the grid feasibility
 *
 * @param   ptIdx             point index
 * @param   bodyFixedState    input body fixed state
 *
 * @return   output feasibility flag
 *
 */
//------------------------------------------------------------------------------
bool CoverageChecker::CheckGridFeasibility(Integer         ptIdx,
                                           const Rvector3& bodyFixedState)
{
#ifdef DEBUG_GRID
   MessageInterface::ShowMessage(
                     "CheckGridFeasibility: ptIDx = %d, bodyFixedState = %s\n",
                     ptIdx, bodyFixedState.ToString(12).c_str());
#endif
   bool     isFeasible = false;
//   Rvector3 rangeVec;  // defaults to all zeroes
   
   bfState  = bodyFixedState/BODY_RADIUS;
   bodyUnit = bfState.GetUnitVector();

   ptPos    = *(pointArray.at(ptIdx));
   Real  feasibilityReal = ptPos * bodyUnit;
   
   if (feasibilityReal > 0.0)
   {
      rangeVec  = bfState - ptPos;
      Real dot  = rangeVec * ptPos;
      if (dot > 0.0)
         isFeasible = true;
   }
   return isFeasible;
}

//------------------------------------------------------------------------------
// void CheckGridFeasibility(const Rvector3& bodyFixedState)
//------------------------------------------------------------------------------
/**
 * Checks the grid feasibility
 *
 * @param   bodyFixedState    input body fixed state
 *
 */
//------------------------------------------------------------------------------
void CoverageChecker::CheckGridFeasibility(const Rvector3& bodyFixedState)
{
#ifdef DEBUG_GRID
   MessageInterface::ShowMessage("CheckGridFeasibility: bodyFixedState = %s\n",
                                 bodyFixedState.ToString(12).c_str());
#endif
   bfState  = bodyFixedState/BODY_RADIUS;
   bodyUnit = bfState.GetUnitVector();
   
   for (Integer ii = 0; ii < pointArray.size(); ii++)
   {
      ptPos = *(pointArray.at(ii));
      if ((ptPos * bodyUnit) > 0.0)
      {
         rangeVec = bfState - ptPos;
         Real     dot   = rangeVec * ptPos;
         if (dot > 0.0)
            feasibilityTest.at(ii) = true;
         else
            feasibilityTest.at(ii) = false;
      }
   }
}


