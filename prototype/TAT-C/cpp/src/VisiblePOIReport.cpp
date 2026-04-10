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
 * Implementation of the visibility POI report class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "VisiblePOIReport.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  VisiblePOIReport()
//---------------------------------------------------------------------------
/**
 * Default constructor for VisiblePOIReport.
 *
 */
//---------------------------------------------------------------------------
VisiblePOIReport::VisiblePOIReport() :
   VisibilityReport(),
   poiIndex        (-1),
   obsZenith       (-999.0),
   obsAzimuth      (-999.0),
   obsRange        (-999.0),
   sunZenith       (-999.0),
   sunAzimuth      (-999.0),
   obsPos          (0,0,0),
   obsVel          (0,0,0)   
{    
}

//---------------------------------------------------------------------------
//  VisiblePOIReport(const VisiblePOIReport &copy)
//---------------------------------------------------------------------------
/**
 * Copy constructor for VisiblePOIReport.
 *
 */
//---------------------------------------------------------------------------
VisiblePOIReport::VisiblePOIReport(const VisiblePOIReport &copy) :
   VisibilityReport(copy),
   poiIndex   (copy.poiIndex),
   obsZenith  (copy.obsZenith),
   obsAzimuth (copy.obsAzimuth),
   obsRange   (copy.obsRange),
   sunZenith  (copy.sunZenith),
   sunAzimuth (copy.sunAzimuth),
   obsPos     (copy.obsPos),
   obsVel     (copy.obsVel)
{
}
//---------------------------------------------------------------------------
//  VisiblePOIReport& operator=(const VisiblePOIReport &copy)
//---------------------------------------------------------------------------
/**
 * operator= for VisiblePOIReport.
 *
 */
//---------------------------------------------------------------------------
VisiblePOIReport& VisiblePOIReport::operator=(const VisiblePOIReport &copy)
{
   if (&copy == this)
      return *this;
   
   VisibilityReport::operator=(copy);
   
   poiIndex   = copy.poiIndex;
   obsZenith  = copy.obsZenith;
   obsAzimuth = copy.obsAzimuth;
   obsRange   = copy.obsRange;
   sunZenith  = copy.sunZenith;
   sunAzimuth = copy.sunAzimuth;
   obsPos     = copy.obsPos;
   obsVel     = copy.obsVel;
   
   return *this;
}

//---------------------------------------------------------------------------
//  ~VisiblePOIReport()
//---------------------------------------------------------------------------
/**
 * Destructor for VisiblePOIReport.
 *
 */
//---------------------------------------------------------------------------
VisiblePOIReport::~VisiblePOIReport()
{
}

//------------------------------------------------------------------------------
// void SetPOIIndex(Integer toIdx)
//---------------------------------------------------------------------------
/**
 * Sets the POI Index for the report.
 *
 * @param <toIdx> Index for the report
 */
//---------------------------------------------------------------------------
void VisiblePOIReport::SetPOIIndex(Integer toIdx)
{
   poiIndex = toIdx;
}

//------------------------------------------------------------------------------
// Integer GetPOIIndex()
//---------------------------------------------------------------------------
/**
 * Returns the POI Index for the report.
 *
 * @return <toIdx> Index for the report
 */
//---------------------------------------------------------------------------
Integer VisiblePOIReport::GetPOIIndex()
{
   return poiIndex;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::SetObsZenith(Real obsZenithIn)
//------------------------------------------------------------------------------
/**
* Sets the zenith angle of observation w/r/t target
*
* @param obsZenithIn The observation zenith angle
*/
//------------------------------------------------------------------------------
void VisiblePOIReport::SetObsZenith(Real obsZenithIn)
{
    obsZenith = obsZenithIn;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::GetObsZenith()
//------------------------------------------------------------------------------
/**
* Returns the zenith angle of observation w/r/t target
*
* @return  zenith angle of observation w/r/t target
*/
//------------------------------------------------------------------------------
Real VisiblePOIReport::GetObsZenith()
{
    return obsZenith;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::SetObsAzimuth(Real obsAzimuthIn)
//------------------------------------------------------------------------------
/**
* Sets the Azimuth angle of observation w/r/t target
*
* @param obsAzimuthIn The observation Azimuth angle
*/
//------------------------------------------------------------------------------
void VisiblePOIReport::SetObsAzimuth(Real obsAzimuthIn)
{
    obsAzimuth = obsAzimuthIn;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::GetObsAzimuth()
//------------------------------------------------------------------------------
/**
* Returns the Azimuth angle of observation w/r/t target
*
* @return  Azimuth angle of observation w/r/t target
*/
//------------------------------------------------------------------------------
Real VisiblePOIReport::GetObsAzimuth()
{
    return obsAzimuth;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::SetObsRange(Real obsRangeIn)
//------------------------------------------------------------------------------
/**
* Sets the Range angle of observation w/r/t target
*
* @param obsRangeIn The observation Range angle
*/
//------------------------------------------------------------------------------
void VisiblePOIReport::SetObsRange(Real obsRangeIn)
{
    obsRange = obsRangeIn;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::GetObsRange()
//------------------------------------------------------------------------------
/**
* Returns the Range angle of observation w/r/t target
*
* @return  Range angle of observation w/r/t target
*/
//------------------------------------------------------------------------------
Real VisiblePOIReport::GetObsRange()
{
    return obsRange;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::SetSunZenith(Real sunZenithIn)
//------------------------------------------------------------------------------
/**
* Sets the zenith angle of sun w/r/t target
*
* @param sunZenithIn The sun zenith angle
*/
//------------------------------------------------------------------------------
void VisiblePOIReport::SetSunZenith(Real sunZenithIn)
{
    sunZenith = sunZenithIn;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::GetSunZenith()
//------------------------------------------------------------------------------
/**
* Returns the zenith angle of sun w/r/t target
*
* @return  zenith angle of sun w/r/t target
*/
//------------------------------------------------------------------------------
Real VisiblePOIReport::GetSunZenith()
{
    return sunZenith;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::SetSunAzimuth(Real sunAzimuthIn)
//------------------------------------------------------------------------------
/**
* Sets the zenith angle of sun w/r/t target
*
* @param sunAzimuthIn The sun zenith angle
*/
//------------------------------------------------------------------------------
void VisiblePOIReport::SetSunAzimuth(Real sunAzimuthIn)
{
    sunAzimuth = sunAzimuthIn;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::GetSunAzimuth()
//------------------------------------------------------------------------------
/**
* Returns the zenith angle of sun w/r/t target
*
* @return  zenith angle of sun w/r/t target
*/
//------------------------------------------------------------------------------
Real VisiblePOIReport::GetSunAzimuth()
{
    return sunAzimuth;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::SetObsPosInertial(Real obsPosIn)
//------------------------------------------------------------------------------
/**
* Sets the observer position in Inertial
*
* @param obsPosIn The observer position in Inertial
*/
//------------------------------------------------------------------------------
void VisiblePOIReport::SetObsPosInertial(Rvector3 obsPosIn)
{
    obsPos = obsPosIn;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::GetObsPosInertial()
//------------------------------------------------------------------------------
/**
* Returns the observer position in Inertial
*
* @return  The observer position in Inertial
*/
//------------------------------------------------------------------------------
Rvector3 VisiblePOIReport::GetObsPosInertial()
{
    return obsPos;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::SetObsVelInertial(Real obsVelIn)
//------------------------------------------------------------------------------
/**
* Sets the observer velocity in Inertial
*
* @param obsPosIn The observer velocity in Inertial
*/
//------------------------------------------------------------------------------
void VisiblePOIReport::SetObsVelInertial(Rvector3 obsVelIn)
{
    obsVel = obsVelIn;
}

//------------------------------------------------------------------------------
// void VisiblePOIReport::GetObsVelInertial()
//------------------------------------------------------------------------------
/**
* Returns the observer velocity in Inertial
*
* @return  The observer velocity in Inertial
*/
//------------------------------------------------------------------------------
Rvector3 VisiblePOIReport::GetObsVelInertial()
{
    return obsVel;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
