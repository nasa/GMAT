//$Id: AlternateEquinoctialParameters.cpp 2013-08-26 00:07:00 CBNU Hwayeong KIM $
//------------------------------------------------------------------------------
//                            File: AlternateEquinoctialParameters.cpp
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Hwayeong Kim
// Created: 2013/08/26
//
/**
 * Declares Equinoctial related parameter classes.
 *   AltEquinP, AltEquinQ, AltEquinState
 */
//------------------------------------------------------------------------------

#include "AlternateEquinoctialParameters.hpp"
#include "ColorTypes.hpp"

//#define DEBUG_CARTESIAN_PARAM 1


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS


//==============================================================================
//                              AltEquinoctialP
//==============================================================================

//------------------------------------------------------------------------------
// AltEquinP::AltEquinP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
AltEquinP::AltEquinP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "AltEquinoctialP", obj, "AltEquinoctial P", "", GmatParam::COORD_SYS, ALT_EQ_P, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}

//------------------------------------------------------------------------------
// AltEquinP::AltEquinP(const AltEquinP &copy)
//------------------------------------------------------------------------------
AltEquinP::AltEquinP(const AltEquinP &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// AltEquinP& AltEquinP::operator=(const AltEquinP&right)
//------------------------------------------------------------------------------
AltEquinP& AltEquinP::operator=(const AltEquinP&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// AltEquinP::~AltEquinP()
//------------------------------------------------------------------------------
AltEquinP::~AltEquinP()
{
}

//------------------------------------------------------------------------------
// bool AltEquinP::Evaluate()
//------------------------------------------------------------------------------
bool AltEquinP::Evaluate()
{
   mRealValue = OrbitData::GetAltEquinReal(ALT_EQ_P);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* AltEquinP::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AltEquinP::Clone(void) const
{
   return new AltEquinP(*this);
}


//==============================================================================
//                              AltEquinoctialQ
//==============================================================================

//------------------------------------------------------------------------------
// AltEquinQ::AltEquinQ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
AltEquinQ::AltEquinQ(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "AltEquinoctialQ", obj, "AltEquinoctial Q", "", GmatParam::COORD_SYS, ALT_EQ_Q, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}

//------------------------------------------------------------------------------
// AltEquinQ::AltEquinQ(const AltEquinQ &copy)
//------------------------------------------------------------------------------
AltEquinQ::AltEquinQ(const AltEquinQ &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// AltEquinQ& AltEquinQ::operator=(const AltEquinQ&right)
//------------------------------------------------------------------------------
AltEquinQ& AltEquinQ::operator=(const AltEquinQ&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// AltEquinQ::~AltEquinQ()
//------------------------------------------------------------------------------
AltEquinQ::~AltEquinQ()
{
}

//------------------------------------------------------------------------------
// bool AltEquinQ::Evaluate()
//------------------------------------------------------------------------------
bool AltEquinQ::Evaluate()
{
   mRealValue = OrbitData::GetAltEquinReal(ALT_EQ_Q);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* AltEquinQ::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AltEquinQ::Clone(void) const
{
   return new AltEquinQ(*this);
}

//------------------------------------------------------------------------------
// AltEquinState::AltEquinState(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
AltEquinState::AltEquinState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "AltEquinoctial", obj, "AltEquinoctial State Vector",
                "", GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsPlottable = false;
}

//------------------------------------------------------------------------------
// AltEquinState::AltEquinState(const AltEquinState &copy)
//------------------------------------------------------------------------------
AltEquinState::AltEquinState(const AltEquinState &copy)
   : OrbitRvec6(copy)
{
}

//------------------------------------------------------------------------------
// AltEquinState& AltEquinState::operator=(const AltEquinState &right)
//------------------------------------------------------------------------------
AltEquinState& AltEquinState::operator=(const AltEquinState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// AltEquinState::~AltEquinState()
//------------------------------------------------------------------------------
AltEquinState::~AltEquinState()
{
}

//------------------------------------------------------------------------------
// bool AltEquinState::Evaluate()
//------------------------------------------------------------------------------
bool AltEquinState::Evaluate()
{
   mRvec6Value = GetAltEquinState();
   
   #if DEBUG_CARTESIAN_PARAM
   MessageInterface::ShowMessage
      ("EquinState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
}

//------------------------------------------------------------------------------
// GmatBase* AltEquinState::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AltEquinState::Clone(void) const
{
   return new AltEquinState(*this);
}

