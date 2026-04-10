//------------------------------------------------------------------------------
//                           VisiblePOIReport
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
// Created: 2016.05.02
//
/**
 * Definition of the visibility POI report class.  This class is the 
 * container for data on POI interval events.
 */
//------------------------------------------------------------------------------
#ifndef VisiblePOIReport_hpp
#define VisiblePOIReport_hpp

#include "gmatdefs.hpp"
#include "AbsoluteDate.hpp"
#include "VisibilityReport.hpp"

class VisiblePOIReport : public VisibilityReport
{
public:
   
   /// class construction/destruction
   VisiblePOIReport();
   VisiblePOIReport( const VisiblePOIReport &copy);
   VisiblePOIReport& operator=(const VisiblePOIReport &copy);
   
   virtual ~VisiblePOIReport();
   
   /// Set/Get the POI index
   virtual void     SetPOIIndex(Integer toIdx);
   virtual Integer  GetPOIIndex();
   /// Set/Get the observation zenith angle
   virtual void     SetObsZenith(Real obsZenithIn);
   virtual Real     GetObsZenith();
   /// Set/Get the observation azimuth
   virtual void     SetObsAzimuth(Real obsZenithIn);
   virtual Real     GetObsAzimuth();
   /// Set/Get the observation range
   virtual void     SetObsRange(Real obsRangeIn);
   virtual Real     GetObsRange();
   /// Set/Get the Sun zenith angle
   virtual void     SetSunZenith(Real sunZenithIn);
   virtual Real     GetSunZenith();
   /// Set/Get the Sun azimuth
   virtual void     SetSunAzimuth(Real obsZenithIn);
   virtual Real     GetSunAzimuth();
   /// Observer position in Inertial
   virtual void     SetObsPosInertial(Rvector3 obsPosIn);
   virtual Rvector3 GetObsPosInertial();
   /// Observer velocity in Inertial
   virtual void     SetObsVelInertial(Rvector3 obsVelIn);
   virtual Rvector3 GetObsVelInertial();
   
protected:
   
   /// Index of point of interest
   Integer poiIndex;
   /// The observation zenith angle
   Real    obsZenith;
   /// The observation azimuth angle
   Real    obsAzimuth;
   /// The observation range angle
   Real    obsRange;
   /// The Sun zenith angle
   Real    sunZenith;
   /// The Sun azimuth angle
   Real    sunAzimuth;
   /// Observer Position in Inertial
   Rvector3    obsPos;
   /// Observer Velocity in Inertial
   Rvector3    obsVel;
   
};
#endif // VisiblePOIReport_hpp
