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
// Created: 2016.05.09
//
/**
 * Definition of the coverage checker class.  This class checks for point
 * coverae and generates reports.
 */
//------------------------------------------------------------------------------
#ifndef CoverageChecker_hpp
#define CoverageChecker_hpp

#include "gmatdefs.hpp"
#include "AbsoluteDate.hpp"
#include "Spacecraft.hpp"
#include "PointGroup.hpp"
#include "Earth.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "VisiblePOIReport.hpp"
#include "IntervalEventReport.hpp"

class CoverageChecker
{
public:
   
   /// class construction/destruction
   CoverageChecker(PointGroup *ptGroup, Spacecraft *sat);
   CoverageChecker( const CoverageChecker &copy);
   CoverageChecker& operator=(const CoverageChecker &copy);
   
   virtual ~CoverageChecker();
   
   /// Check the point coverage and return the resulting index array
   virtual IntegerArray      CheckPointCoverage(const Rvector6 &theState,
                                                Real           theTime,
                                                const Rvector6 &cartState);
   /// Accumulate the coverage data at the current propagated time
   virtual IntegerArray      AccumulateCoverageData();
   /// Accumulate the coverage data at the input time
   virtual IntegerArray      AccumulateCoverageData(Real atTime);
   /// Process the coverate data, create reports
   virtual std::vector<IntervalEventReport>
                             ProcessCoverageData();
   /// Create a new POI report
   virtual IntervalEventReport
                             CreateNewPOIReport(Real startJd, Real endJd,
                                                Integer poiIdx);

   /// Set the flag indicating whether or not to compute POI geometry data
   virtual void              SetComputePOIGeometryData(bool flag);
   
protected:
   
   /// the points to use for coverage
   PointGroup                 *pointGroup;
   /// The spacecraft object @todo Should this be an array of spacecraft?
   Spacecraft                 *sc;
   /// the central body; the model of Earth's properties & rotation
   Earth                      *centralBody;
   /// the number of accumulated propagation data points // ???
   Integer                    timeIdx;
   /// times when points are visible
   std::vector<IntegerArray>  timeSeriesData; 
   /// discrete event data
   std::vector <std::vector<VisiblePOIReport> > discreteEventData;
   /// the date of each propagation point
   RealArray                  dateData;
   /// the number of propagation times when each point was visible
   IntegerArray               numEventsPerPoint;
   /// array of all points
   std::vector<Rvector3*>     pointArray;
   /// feasibility values for each point
   std::vector<bool>          feasibilityTest;
   /// flag indicating if observer and sun geometry should be computed
   bool                       computePOIGeometryData;
   /// Start time of the coverage
   Real                       coverageStart;
   /// End time of the coverage
   Real                       coverageEnd;
   
   /// <static const> body radius
   static const Real BODY_RADIUS;
   /// Get the Earth Fixed state at the input time for the input cartesian state
   virtual Rvector6          GetEarthFixedSatState(Real jd,
                                                   const Rvector6& scCartState);
   /// Check the grid feasibility for the input point with the input body
   /// fixed state
   virtual bool              CheckGridFeasibility(Integer ptIdx,
                                  const Rvector3& bodyFixedState);
   /// Check the grid feasibility for all points for the input body fixed state
   virtual void              CheckGridFeasibility(
                                  const Rvector3& bodyFixedState);
   
   /// local Rvectors used for Grid Feasibility calculations
   /// (for performance)
   Rvector3 rangeVec;   
   Rvector3 bfState;
   Rvector3 bodyUnit;
   Rvector3 ptPos;

};
#endif // CoverageChecker_hpp







