//$Id: ModEquinoctialParameters.cpp 11991 2013-07-16 MHK
//------------------------------------------------------------------------------
//                            ModEquinoctialParameters.cpp
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
// Author: Minhee Kim
// Created: 2013/07/16
//
/**
 * Declares Modified Equinoctial related parameter classes.
 *ModEquinP, ModEquinF, ModEquinG, ModEquinH, ModEquinK, ModEquinTLONG, ModEquinState
 */
//------------------------------------------------------------------------------

#include "ModEquinoctialParameters.hpp"
#include "ColorTypes.hpp"

//#define DEBUG_CARTESIAN_PARAM 1


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

//@note The ModEquinP class is no longer used in the ParameterFactory
//      The SemilausRectum is used as originally designed (LOJ: 2014.01.29)
//------------------------------------------------------------------------------
// ModEquinP::ModEquinP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ModEquinP::ModEquinP(const std::string &name, GmatBase *obj)
   // Changed SemiLatusRectum to SemilatusRectum and ORIGIN dependent (Fix for GMT-4173)
   //: OrbitReal(name, "SemiLatusRectum", obj, "ModEquinoctial P", "km", GmatParam::COORD_SYS, MOD_EQ_P, true)
   //: OrbitReal(name, "SemilatusRectum", obj, "ModEquinoctial P", "km", GmatParam::ORIGIN, MOD_EQ_P, true)
   // Changed to ModEquinoctialP to avoid confusion and changed back to CoordianteSystem dependent (LOJ: 2014.01.29)
   : OrbitReal(name, "ModEquinoctialP", obj, "ModEquinoctial P", "km", GmatParam::COORD_SYS, MOD_EQ_P, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   //mDepObjectName = "Earth";
   //SetRefObjectName(Gmat::SPACE_POINT, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}

//------------------------------------------------------------------------------
// ModEquinP::ModEquinP(const ModEquinP &copy)
//------------------------------------------------------------------------------
ModEquinP::ModEquinP(const ModEquinP &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// ModEquinP& ModEquinP::operator=(const ModEquinP&right)
//------------------------------------------------------------------------------
ModEquinP& ModEquinP::operator=(const ModEquinP&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ModEquinP::~ModEquinP()
//------------------------------------------------------------------------------
ModEquinP::~ModEquinP()
{
}

//------------------------------------------------------------------------------
// bool ModEquinP::Evaluate()
//------------------------------------------------------------------------------
bool ModEquinP::Evaluate()
{
   mRealValue = OrbitData::GetModEquinReal(MOD_EQ_P);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* EquinSma::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ModEquinP::Clone(void) const
{
   return new ModEquinP(*this);
}


//------------------------------------------------------------------------------
// ModEquinF::ModEquinF(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ModEquinF::ModEquinF(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "ModEquinoctialF", obj, "ModEquinoctial F", "?", GmatParam::COORD_SYS, MOD_EQ_F, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

//------------------------------------------------------------------------------
// ModEquinF::ModEquinF(const ModEquinF&copy)
//------------------------------------------------------------------------------
ModEquinF::ModEquinF(const ModEquinF&copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// ModEquinF& ModEquinF::operator=(const ModEquinF&right)
//------------------------------------------------------------------------------
ModEquinF& ModEquinF::operator=(const ModEquinF&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ModEquinF::~ModEquinF()
//------------------------------------------------------------------------------
ModEquinF::~ModEquinF()
{
}

//------------------------------------------------------------------------------
// bool ModEquinF::Evaluate()
//------------------------------------------------------------------------------
bool ModEquinF::Evaluate()
{
   mRealValue = OrbitData::GetModEquinReal(MOD_EQ_F);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* ModEquinF::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ModEquinF::Clone(void) const
{
   return new ModEquinF(*this);
}


//------------------------------------------------------------------------------
// ModEquinG::ModEquinG(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ModEquinG::ModEquinG(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "ModEquinoctialG", obj, "ModEquinoctial G", "?", GmatParam::COORD_SYS, MOD_EQ_G, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}


//------------------------------------------------------------------------------
// ModEquinG::ModEquinG(const EquinEx &copy)
//------------------------------------------------------------------------------
ModEquinG::ModEquinG(const ModEquinG &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// ModEquinG& ModEquinG::operator=(const ModEquinG&right)
//------------------------------------------------------------------------------
ModEquinG& ModEquinG::operator=(const ModEquinG&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ModEquinG::~ModEquinG()
//------------------------------------------------------------------------------
ModEquinG::~ModEquinG()
{
}

//------------------------------------------------------------------------------
// bool ModEquinG::Evaluate()
//------------------------------------------------------------------------------
bool ModEquinG::Evaluate()
{
   mRealValue = OrbitData::GetModEquinReal(MOD_EQ_G);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* ModEquinG::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ModEquinG::Clone(void) const
{
   return new ModEquinG(*this);
}



//------------------------------------------------------------------------------
// ModEquinH::ModEquinH(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ModEquinH::ModEquinH(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "ModEquinoctialH", obj, "ModEquinoctial H", "?", GmatParam::COORD_SYS, MOD_EQ_H, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW32;
   #endif
}

//------------------------------------------------------------------------------
// ModEquinH::ModEquinH(const ModEquinH &copy)
//------------------------------------------------------------------------------
ModEquinH::ModEquinH(const ModEquinH &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// ModEquinH& ModEquinH::operator=(const ModEquinH&right)
//------------------------------------------------------------------------------
ModEquinH& ModEquinH::operator=(const ModEquinH&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ModEquinH::~ModEquinH()
//------------------------------------------------------------------------------
ModEquinH::~ModEquinH()
{
}

//------------------------------------------------------------------------------
// bool ModEquinH::Evaluate()
//------------------------------------------------------------------------------
bool ModEquinH::Evaluate()
{
   mRealValue = OrbitData::GetModEquinReal(MOD_EQ_H);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* ModEquinH::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ModEquinH::Clone(void) const
{
   return new ModEquinH(*this);
}


//------------------------------------------------------------------------------
// ModEquinK::ModEquinK(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ModEquinK::ModEquinK(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "ModEquinoctialK", obj, "ModEquinoctial K", "?", GmatParam::COORD_SYS, MOD_EQ_K, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

//------------------------------------------------------------------------------
// ModEquinK::ModEquinK(const ModEquinK &copy)
//------------------------------------------------------------------------------
ModEquinK::ModEquinK(const ModEquinK &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// ModEquinK& ModEquinK::operator=(const ModEquinK&right)
//------------------------------------------------------------------------------
ModEquinK& ModEquinK::operator=(const ModEquinK&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ModEquinK::~ModEquinK()
//------------------------------------------------------------------------------
ModEquinK::~ModEquinK()
{
}

//------------------------------------------------------------------------------
// bool ModEquinK::Evaluate()
//------------------------------------------------------------------------------
bool ModEquinK::Evaluate()
{
   mRealValue = OrbitData::GetModEquinReal(MOD_EQ_K);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* ModEquinK::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ModEquinK::Clone(void) const
{
   return new ModEquinK(*this);
}



//------------------------------------------------------------------------------
// ModEquinTLONG::ModEquinTLONG(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ModEquinTLONG::ModEquinTLONG(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "TLONG", obj, "ModEquinoctial Tlong", "deg", GmatParam::COORD_SYS, MOD_EQ_TLONG, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

//------------------------------------------------------------------------------
// ModEquinTLONG::ModEquinTLONG(const ModEquinTLONG &copy)
//------------------------------------------------------------------------------
ModEquinTLONG::ModEquinTLONG(const ModEquinTLONG &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// ModEquinTLONG& ModEquinTLONG::operator=(const ModEquinTLONG&right)
//------------------------------------------------------------------------------
ModEquinTLONG& ModEquinTLONG::operator=(const ModEquinTLONG&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ModEquinTLONG::~ModEquinTLONG()
//------------------------------------------------------------------------------
ModEquinTLONG::~ModEquinTLONG()
{
}

//------------------------------------------------------------------------------
// bool ModEquinTLONG::Evaluate()
//------------------------------------------------------------------------------
bool ModEquinTLONG::Evaluate()
{
   mRealValue = OrbitData::GetModEquinReal(MOD_EQ_TLONG);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* ModEquinTLONG::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ModEquinTLONG::Clone(void) const
{
   return new ModEquinTLONG(*this);
}



//------------------------------------------------------------------------------
// ModEquinState::ModEquinState(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ModEquinState::ModEquinState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "ModifiedEquinoctial", obj, "ModEquinoctial State Vector",
                "", GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsPlottable = false;
}

//------------------------------------------------------------------------------
// ModEquinState::ModEquinState(const ModEquinState &copy)
//------------------------------------------------------------------------------
ModEquinState::ModEquinState(const ModEquinState &copy)
   : OrbitRvec6(copy)
{
}

//------------------------------------------------------------------------------
// ModEquinState& ModEquinState::operator=(const ModEquinState &right)
//------------------------------------------------------------------------------
ModEquinState& ModEquinState::operator=(const ModEquinState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ModEquinState::~ModEquinState()
//------------------------------------------------------------------------------
ModEquinState::~ModEquinState()
{
}

//------------------------------------------------------------------------------
// bool ModEquinState::Evaluate()
//------------------------------------------------------------------------------
bool ModEquinState::Evaluate()
{
   mRvec6Value = GetModEquinState();
   
   #if DEBUG_CARTESIAN_PARAM
   MessageInterface::ShowMessage
      ("EquinState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
}

//------------------------------------------------------------------------------
// GmatBase* ModEquinState::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ModEquinState::Clone(void) const
{
   return new ModEquinState(*this);
}


