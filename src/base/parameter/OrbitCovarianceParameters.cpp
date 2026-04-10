//$Id$
//------------------------------------------------------------------------------
//                            File: OrbitCovarianceParameters
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
// Author: John McGreevy
// Created: 2020.07.10
//
/**
 * Implements orbit covariance related parameter classes.
 *    OrbitErrorCovariance
 */
//------------------------------------------------------------------------------

#include "OrbitCovarianceParameters.hpp"
#include "ColorTypes.hpp"


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

//==============================================================================
//                              OrbitErrorCovariance
//==============================================================================
/**
 * Implements OrbitErrorCovariance class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// OrbitErrorCovariance(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
OrbitErrorCovariance::OrbitErrorCovariance(const std::string &name, GmatBase *obj)
   : OrbitRmat66(name, "OrbitErrorCovariance", obj, "Orbit Error Covariance", "", GmatParam::COORD_SYS,
         true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// OrbitErrorCovariance(const OrbitErrorCovariance &copy)
//------------------------------------------------------------------------------
OrbitErrorCovariance::OrbitErrorCovariance(const OrbitErrorCovariance &copy)
   : OrbitRmat66(copy)
{
}


//------------------------------------------------------------------------------
// OrbitErrorCovariance& operator=(const OrbitErrorCovariance &right)
//------------------------------------------------------------------------------
OrbitErrorCovariance& OrbitErrorCovariance::operator=(const OrbitErrorCovariance &right)
{
   if (this != &right)
      OrbitRmat66::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitErrorCovariance()
//------------------------------------------------------------------------------
OrbitErrorCovariance::~OrbitErrorCovariance()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool OrbitErrorCovariance::Evaluate()
{
   mRmat66Value = OrbitData::GetCovarianceRmat66();
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* OrbitErrorCovariance::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* OrbitErrorCovariance::Clone(void) const
{
   return new OrbitErrorCovariance(*this);
}

