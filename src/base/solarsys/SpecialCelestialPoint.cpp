//$Id:  $
//------------------------------------------------------------------------------
//                            SpecialCelestialPoint
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Wendy C. Shoan
// Created: 2011.07.15
//
/**
 * Implementation of the SpecialCelestialPoint class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "SpecialCelestialPoint.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"
#include "StringUtil.hpp"

//#define DEBUG_CELESTIAL_POINT
//#define DEBUG_EPHEM_SOURCE
//#define DEBUG_J2000_BODY

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  SpecialCelestialPoint(std::string name)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpecialCelestialPoint class
 * (default constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               point (default is "").
 */
//------------------------------------------------------------------------------
SpecialCelestialPoint::SpecialCelestialPoint(std::string name) :
   CelestialBody     ("SpecialCelestialPoint",name)
{
   objectTypeNames.push_back("SpecialCelestialPoint");
   parameterCount = SpecialCelestialPointParamCount;

   bodyType            = Gmat::SPECIAL_CELESTIAL_POINT;
   bodyNumber          = -1;
   referenceBodyNumber = -1;

   // don't need central body, but set default one to avoid error conditions
   centralBodySet      = true;
   theCentralBodyName  = "Sun";  //    N/A

   // special celestial points have no mass of their own
   mu                  = 0.0;
}

//------------------------------------------------------------------------------
//  SpecialCelestialPoint(const SpecialCelestialPoint &copy)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpecialCelestialPoint class as a copy
 * of the specified SpecialCelestialPoint class (copy constructor).
 *
 * @param <copy> SpecialCelestialPoint object to copy.
 */
//------------------------------------------------------------------------------
SpecialCelestialPoint::SpecialCelestialPoint(const SpecialCelestialPoint &copy) :
   CelestialBody (copy)
{
}

//------------------------------------------------------------------------------
//  SpecialCelestialPoint& operator= (const SpecialCelestialPoint& copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SpecialCelestialPoint class.
 *
 * @param <copy> the SpecialCelestialPoint object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" SpecialCelestialPoint with data of input SpecialCelestialPoint
 *         copy.
 */
//------------------------------------------------------------------------------
SpecialCelestialPoint& SpecialCelestialPoint::operator=(const SpecialCelestialPoint &copy)
{
   if (&copy == this)
      return *this;

   CelestialBody::operator=(copy);
   return *this;
}

//------------------------------------------------------------------------------
//  ~SpecialCelestialPoint()
//------------------------------------------------------------------------------
/**
 * Destructor for the SpecialCelestialPoint class.
 */
//------------------------------------------------------------------------------
SpecialCelestialPoint::~SpecialCelestialPoint()
{
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the SpecialCelestialPoint.
 *
 * @return clone of the SpecialCelestialPoint.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SpecialCelestialPoint::Clone() const
{
   return (new SpecialCelestialPoint(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Set this instance to match the one passed in.
 *
 * @param <orig> The object that is being copied.
 */
//---------------------------------------------------------------------------
void SpecialCelestialPoint::Copy(const GmatBase* orig)
{
   operator=(*((SpecialCelestialPoint *)(orig)));
}

//---------------------------------------------------------------------------
//  bool NeedsOnlyMainSPK()
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the default SPK file contains
 * sufficient data for this SpecialCelestialPoint.
 *
 * @return flag indicating whether or not an additional SPK file is needed
 *         for this SpecialCelestialPoint; true, if only the default one
 *         is needed; false if an additional file is needed.
 */
//---------------------------------------------------------------------------
bool SpecialCelestialPoint::NeedsOnlyMainSPK()
{
   if (instanceName == GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME)
      return true;
   return false;
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not the specified
 * parameter is read-only.
 *
 * @param <id> ID of the specified parameter
 *
 * @return flag indicating indicating whether or not the specified
 *         parameter is read-only
 *
 */
//------------------------------------------------------------------------------
bool SpecialCelestialPoint::IsParameterReadOnly(const Integer id) const
{
   // We don't want to write out any of the parameters for this object
   return true;
}

//------------------------------------------------------------------------------
//  bool SetSource(Gmat::PosVelSource pvSrc)
//------------------------------------------------------------------------------
/**
 * This method sets the position/velocity source for the body.
 *
 * @param <pcSrc> position/velocity source for the body.
 *
 * @return flag indicating success of the method.
 *
 */
//------------------------------------------------------------------------------
bool SpecialCelestialPoint::SetSource(Gmat::PosVelSource pvSrc)
{
   #ifdef DEBUG_EPHEM_SOURCE
   MessageInterface::ShowMessage
      ("SpecialCelestialPoint::SetSource() <%p> %s, Setting source to %d(%s)\n", this,
       GetName().c_str(), pvSrc, Gmat::POS_VEL_SOURCE_STRINGS[pvSrc].c_str());
   #endif

   if (pvSrc == posVelSrc) return true;
   posVelSrc           = pvSrc;
   return true;
}


//------------------------------------------------------------------------------
//  bool SetAllowSpice(const bool allow)
//------------------------------------------------------------------------------
/**
 * This method sets the allow spice flag
 *
 * @param <allow> allow spice flag
 *
 * @return flag indicating success of the method.
 *
 */
//------------------------------------------------------------------------------
bool SpecialCelestialPoint::SetAllowSpice(const bool allow)
{
   allowSpice = allow;
   return true;
}

//------------------------------------------------------------------------------
//  Rvector6 ComputeTwoBody(const A1Mjd &forTime)
//------------------------------------------------------------------------------
/**
 * This method computes the position and velocity at time forTime
 *
 * @return body state (position, velocity) at time forTime, with respect to
 *         the Earth.
 *
 */
//------------------------------------------------------------------------------
Rvector6 SpecialCelestialPoint::ComputeTwoBody(const A1Mjd &forTime)
{
   Rvector6 dummy;

   std::string errmsg = "Two-Body propagation not defined for ";
   errmsg += instanceName + "\n";
   throw SolarSystemException(errmsg);

   return dummy;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time
