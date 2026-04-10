//$Id: PlanetodeticParameters.cpp 10206 2013-05-24 Minhee Kim $
//------------------------------------------------------------------------------
//                            PlanetodeticParameters.cpp
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
// Created: 2013/5/24
//
/**
 * Declares Planetodetic related parameter classes.
 *   PlanetodeticRMAG, PlanetodeticLON, PlanetodeticLAT, PlanetodeticVMAG,
 *   PlanetodeticAZI, PlanetodeticHFPA, PlanetodeticState
 */
//------------------------------------------------------------------------------

#include "PlanetodeticParameters.hpp"
#include "ColorTypes.hpp"

//#define DEBUG_CARTESIAN_PARAM 1


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

//==============================================================================
// PldRMAG(const std::string &name, GmatBase *obj)
//==============================================================================
PldRMAG::PldRMAG(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticRMAG", obj, "Planetodetic RMAG", "km",
               GmatParam::COORD_SYS, PLD_RMAG, true)
{ 
   mDepObjectName = "EarthFixed";
   mParameterClassType = "Planetodetic state type";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}

//------------------------------------------------------------------------------
// PldRMAG(const PldRMAG &copy)
//------------------------------------------------------------------------------
PldRMAG::PldRMAG(const PldRMAG &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// PldRMAG& operator=(const PldRMAG&right)
//------------------------------------------------------------------------------
PldRMAG& PldRMAG::operator=(const PldRMAG&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~PldRMAG()
//------------------------------------------------------------------------------
PldRMAG::~PldRMAG()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PldRMAG::Evaluate()
{
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_RMAG);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PldRMAG::Clone(void) const
{
   return new PldRMAG(*this);
}



//==============================================================================
// PldLON(const std::string &name, GmatBase *obj)
//==============================================================================
PldLON::PldLON(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticLON", obj, "Planetodetic LON", "deg",
               GmatParam::COORD_SYS, PLD_LON, true)
{
   mDepObjectName = "EarthFixed";
   mParameterClassType = "Planetodetic state type";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

//------------------------------------------------------------------------------
// PldLON(const PldLON&copy)
//------------------------------------------------------------------------------
PldLON::PldLON(const PldLON&copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// PldLON& operator=(const PldLON&right)
//------------------------------------------------------------------------------
PldLON& PldLON::operator=(const PldLON&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~PldLON()
//------------------------------------------------------------------------------
PldLON::~PldLON()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PldLON::Evaluate()
{
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_LON);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PldLON::Clone(void) const
{
   return new PldLON(*this);
}



//==============================================================================
// PldLAT(const std::string &name, GmatBase *obj)
//==============================================================================
PldLAT::PldLAT(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticLAT", obj, "Planetodetic LAT", "deg",
               GmatParam::COORD_SYS, PLD_LAT, true)
{
   mDepObjectName = "EarthFixed";
   mParameterClassType = "Planetodetic state type";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

//------------------------------------------------------------------------------
// PldLAT(const PldLAT &copy)
//------------------------------------------------------------------------------
PldLAT::PldLAT(const PldLAT &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// PldLAT& operator=(const PldLAT&right)
//------------------------------------------------------------------------------
PldLAT& PldLAT::operator=(const PldLAT&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~PldLAT()
//------------------------------------------------------------------------------
PldLAT::~PldLAT()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PldLAT::Evaluate()
{
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_LAT);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PldLAT::Clone(void) const
{
   return new PldLAT(*this);
}



//==============================================================================
// PldVMAG(const std::string &name, GmatBase *obj)
//==============================================================================
PldVMAG::PldVMAG(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticVMAG", obj, "Planetodetic VMAG", "km/sec",
               GmatParam::COORD_SYS, PLD_VMAG, true)
{
   mDepObjectName = "EarthFixed";
   mParameterClassType = "Planetodetic state type";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW32;
   #endif
}

//------------------------------------------------------------------------------
// PldVMAG(const PldVMAG &copy)
//------------------------------------------------------------------------------
PldVMAG::PldVMAG(const PldVMAG &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// PldVMAG& operator=(const PldVMAG&right)
//------------------------------------------------------------------------------
PldVMAG& PldVMAG::operator=(const PldVMAG&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~PldVMAG()
//------------------------------------------------------------------------------
PldVMAG::~PldVMAG()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PldVMAG::Evaluate()
{
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_VMAG);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PldVMAG::Clone(void) const
{
   return new PldVMAG(*this);
}



//==============================================================================
// PldAZI(const std::string &name, GmatBase *obj)
//==============================================================================
PldAZI::PldAZI(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticAZI", obj, "Planetodetic AZI", "deg",
               GmatParam::COORD_SYS, PLD_AZI, true)
{
   mParameterClassType = "Planetodetic state type";
   mDepObjectName = "EarthFixed";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

//------------------------------------------------------------------------------
// PldAZI(const PldAZI &copy)
//------------------------------------------------------------------------------
PldAZI::PldAZI(const PldAZI &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// PldAZI& operator=(const PldAZI&right)
//------------------------------------------------------------------------------
PldAZI& PldAZI::operator=(const PldAZI&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~PldAZI()
//------------------------------------------------------------------------------
PldAZI::~PldAZI()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PldAZI::Evaluate()
{
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_AZI);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PldAZI::Clone(void) const
{
   return new PldAZI(*this);
}



//==============================================================================
// PldHFPA(const std::string &name, GmatBase *obj)
//==============================================================================
PldHFPA::PldHFPA(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "PlanetodeticHFPA", obj, "Planetodetic HFPA", "deg",
               GmatParam::COORD_SYS, PLD_HFPA, true)
{
   mParameterClassType = "Planetodetic state type";
   mDepObjectName = "EarthFixed";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

//------------------------------------------------------------------------------
// PldHFPA(const PldHFPA &copy)
//------------------------------------------------------------------------------
PldHFPA::PldHFPA(const PldHFPA &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// PldHFPA& operator=(const PldHFPA&right)
//------------------------------------------------------------------------------
PldHFPA& PldHFPA::operator=(const PldHFPA&right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~PldHFPA()
//------------------------------------------------------------------------------
PldHFPA::~PldHFPA()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PldHFPA::Evaluate()
{
   //mRealValue = OrbitData::GetEquinReal("EquinMlong");
   mRealValue = OrbitData::GetPlanetodeticReal(PLD_HFPA);
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PldHFPA::Clone(void) const
{
   return new PldHFPA(*this);
}



//==============================================================================
// PldState(const std::string &name, GmatBase *obj)
//==============================================================================
PldState::PldState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "Planetodetic", obj, "Planetodetic State Vector",
                "", GmatParam::COORD_SYS)
{
   mParameterClassType = "Planetodetic state type";
   mDepObjectName = "EarthFixed";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresBodyFixedCS(true);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsPlottable = false;
}

//------------------------------------------------------------------------------
// PldState(const PldState &copy)
//------------------------------------------------------------------------------
PldState::PldState(const PldState &copy)
   : OrbitRvec6(copy)
{
}

//------------------------------------------------------------------------------
// PldState& operator=(const PldState &right)
//------------------------------------------------------------------------------
PldState& PldState::operator=(const PldState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~PldState()
//------------------------------------------------------------------------------
PldState::~PldState()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PldState::Evaluate()
{
   mRvec6Value = GetPlanetodeticState();
   
   #if DEBUG_CARTESIAN_PARAM
   MessageInterface::ShowMessage
      ("EquinState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
}

//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PldState::Clone(void) const
{
   return new PldState(*this);
}

