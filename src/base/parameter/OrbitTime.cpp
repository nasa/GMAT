//$Id$
//------------------------------------------------------------------------------
//                           OrbitTime
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS III
// contract, Task Order 128
//
// Author: Claire R Conway, Thinking Systems, Inc.
// Created: Jan 18, 2022
/**
 * Computes the true local time of the current position on an Earth orbit
 */
//------------------------------------------------------------------------------

#include "OrbitTime.hpp"
#include "ParameterException.hpp"

#include "Norm.hpp"

/**
 * Constructor
 *
 * @param name Name for the parameter object
 * @param obj  Reference object for the parameter
 */
OrbitTime::OrbitTime(const std::string &name, GmatBase *obj) :
   OrbitReal(name, "OrbitTime", obj, "Local Time of the Current State", " ",
         GmatParam::COORD_SYS, -999, false),
   theSun   (nullptr)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_24;

}

/**
 * Copy constructor
 *
 * @param copy The OrbitTime that is copied here
 */
OrbitTime::OrbitTime(const OrbitTime &copy) :
   OrbitReal(copy),
   theSun   (nullptr)
{

}

/**
 * Destructor
 */
OrbitTime::~OrbitTime()
{
}


/**
 * Assignment operator
 *
 * @param copy The OrbitTime that is copied here
 *
 * @return This object, set to match copy
 */
OrbitTime& OrbitTime::operator=(const OrbitTime &copy)
{
   if (this != &copy)
   {
      OrbitReal::operator=(copy);
      theSun = nullptr;
   }

   return *this;
}


/**
 * Object cloner
 *
 * @return A copy of this OrbitTime
 */
GmatBase* OrbitTime::Clone() const
{
   return new OrbitTime(*this);
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Performs the calculations needed for orbit time
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool OrbitTime::Evaluate()
{
   // Currently only functional for Earth-centric coordinate systems
   if (mParameterCS->GetOriginName() != "Earth")
   {
      throw ParameterException("The OrbitTime parameter is only valid for Earth"
            "centered orbits");
   }

   if (theSun == nullptr)
      theSun = mSolarSystem->GetBody("Sun");

   Real epoch = mSpacecraft->GetEpoch();

   Rvector6 j2ksv = theSun->GetMJ2000State(epoch);
   Rvector6 j2krv = mSpacecraft->GetMJ2000State(epoch);

   Rvector6 sv, rv;
   mCoordConverter.Convert(epoch, j2ksv, mInternalCS, sv, mParameterCS);
   mCoordConverter.Convert(epoch, j2krv, mInternalCS, rv, mParameterCS);

   Rvector3 sveq = Rvector3(-sv[0], -sv[1], 0);
   Rvector3 rveq = Rvector3(rv[0], rv[1], 0);

   //Rvector3 svnorm = Norm(sveq);
   Real svnorm = sqrt((sv[0]*sv[0]) + (sv[1]*sv[1]));
   Real rvnorm = sqrt((rv[0]*rv[0]) + (rv[1]*rv[1]));

   Rvector3 svunit = 1.0 / svnorm * sveq;
   Rvector3 rvunit = 1.0 / rvnorm * rveq;

   Real rdots = svunit[0]*rvunit[0] + svunit[1]*rvunit[1] + svunit[2]*rvunit[2];

   if (rdots > 1.0)
      rdots = 1.0;
   if (rdots < -1.0)
      rdots = -1.0;

   Real timeAngle = acos(rdots) * GmatMathConstants::DEG_PER_RAD;
   if (svunit[0] * rvunit[1] - svunit[1] * rvunit[0] < 0.0)
      timeAngle = 360.0 - timeAngle;

   mRealValue = timeAngle / 15.0;
   return true;
}
