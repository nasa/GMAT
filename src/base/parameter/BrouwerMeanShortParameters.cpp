//$Id: BrouwerMeanShortParameters.cpp 2013-05-24 Youngkwang Kim $
//------------------------------------------------------------------------------
//                            File: BrouwerMeanShortParameters.cpp
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
// Author: Youngkwang Kim
// Created: 2013/09/15
//
/**
 * Declares Brouwer-Lyddane mean short related parameter classes.
 * BLshort_SMAP, BLshort_ECCP, BLshort_INCP, BLshort_RAANP, BLshort_AOPP, BLshort_MAP, BLshort_State
 */
//------------------------------------------------------------------------------

#include "BrouwerMeanShortParameters.hpp"
#include "ColorTypes.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// BLshortSMAP::BLshortSMAP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLshortSMAP::BLshortSMAP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerShortSMA", obj, "Short-term averaged mean SMA", "Km",
               GmatParam::COORD_SYS, BLS_SMA, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
}

//------------------------------------------------------------------------------
// BLshortSMAP::BLshortSMAP(const BLshortSMAP &copy)
//------------------------------------------------------------------------------
BLshortSMAP::BLshortSMAP(const BLshortSMAP &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// BLshortSMAP& BLshortSMAP::operator=(const BLshortSMAP&right)
//------------------------------------------------------------------------------
BLshortSMAP& BLshortSMAP::operator=(const BLshortSMAP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// BLshortSMAP::~BLshortSMAP()
//------------------------------------------------------------------------------
BLshortSMAP::~BLshortSMAP()
{
}

//------------------------------------------------------------------------------
// bool BLshortSMAP::Evaluate()
//------------------------------------------------------------------------------
bool BLshortSMAP::Evaluate()
{
   mRealValue = OrbitData::GetBLshortReal(BLS_SMA);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* BLshortSMAP::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BLshortSMAP::Clone(void) const
{
   return new BLshortSMAP(*this);
}


//==============================================================================
//                              BLshortECCP
//==============================================================================
//------------------------------------------------------------------------------
// BLshortECCP::BLshortECCP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLshortECCP::BLshortECCP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerShortECC", obj, "Short-term averaged mean ECC", " ",
               GmatParam::COORD_SYS, BLS_ECC, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
}

//------------------------------------------------------------------------------
// BLshortECCP::BLshortECCP(const BLshortECCP &copy)
//------------------------------------------------------------------------------
BLshortECCP::BLshortECCP(const BLshortECCP &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// BLshortECCP& BLshortECCP::operator=(const BLshortECCP&right)
//------------------------------------------------------------------------------
BLshortECCP& BLshortECCP::operator=(const BLshortECCP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// BLshortECCP::~BLshortECCP()
//------------------------------------------------------------------------------
BLshortECCP::~BLshortECCP()
{
}

//------------------------------------------------------------------------------
// bool BLshortECCP::Evaluate()
//------------------------------------------------------------------------------
bool BLshortECCP::Evaluate()
{
   mRealValue = OrbitData::GetBLshortReal(BLS_ECC);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* BLshortECCP::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BLshortECCP::Clone(void) const
{
   return new BLshortECCP(*this);
}

//==============================================================================
//                              BLshortINCP
//==============================================================================
BLshortINCP::BLshortINCP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerShortINC", obj, "Short-term averaged mean INC", "Deg",
               GmatParam::COORD_SYS, BLS_INC, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
}

//------------------------------------------------------------------------------
// BLshortINCP(const BLshortINCP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLshortINCP::BLshortINCP(const BLshortINCP &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const BLshortINCP& operator=(const BLshortINCP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLshortINCP& BLshortINCP::operator=(const BLshortINCP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BLshortINCP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BLshortINCP::~BLshortINCP()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool BLshortINCP::Evaluate()
{
   mRealValue = OrbitData::GetBLshortReal(BLS_INC);    
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* BLshortINCP::Clone(void) const
{
   return new BLshortINCP(*this);
}


//==============================================================================
//                              BLshortRAANP
//==============================================================================
//------------------------------------------------------------------------------
// BLshortRAANP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLshortRAANP::BLshortRAANP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerShortRAAN", obj, "Short-term averaged mean RAAN", "Deg",
               GmatParam::COORD_SYS, BLS_RAAN, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// BLshortRAANP(const BLshortRAANP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLshortRAANP::BLshortRAANP(const BLshortRAANP &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const BLshortRAANP& operator=(const BLshortRAANP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLshortRAANP& BLshortRAANP::operator=(const BLshortRAANP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BLshortRAANP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BLshortRAANP::~BLshortRAANP()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool BLshortRAANP::Evaluate()
{
   mRealValue = OrbitData::GetBLshortReal(BLS_RAAN);    
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* BLshortRAANP::Clone(void) const
{
   return new BLshortRAANP(*this);
}


//==============================================================================
//                              BLshortAOPP
//==============================================================================
//------------------------------------------------------------------------------
// BLshortAOPP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLshortAOPP::BLshortAOPP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerShortAOP", obj, "Short-term averaged mean AOP", "Deg",
               GmatParam::COORD_SYS, BLS_AOP, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// BLshortAOPP(const BLshortAOPP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLshortAOPP::BLshortAOPP(const BLshortAOPP &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const BLshortAOPP& operator=(const BLshortAOPP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLshortAOPP& BLshortAOPP::operator=(const BLshortAOPP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BLshortAOPP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BLshortAOPP::~BLshortAOPP()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool BLshortAOPP::Evaluate()
{
   mRealValue = OrbitData::GetBLshortReal(BLS_AOP);    
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* BLshortAOPP::Clone(void) const
{
   return new BLshortAOPP(*this);
}


//==============================================================================
//                              BLshortMAP
//==============================================================================
//------------------------------------------------------------------------------
// BLshortMAP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLshortMAP::BLshortMAP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerShortMA", obj, "Short-term averaged mean MA", "Deg",
               GmatParam::COORD_SYS, BLS_MA, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// BLshortMAP(const BLshortMAP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLshortMAP::BLshortMAP(const BLshortMAP &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const BLshortMAP& operator=(const BLshortMAP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLshortMAP& BLshortMAP::operator=(const BLshortMAP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BLshortMAP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BLshortMAP::~BLshortMAP()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool BLshortMAP::Evaluate()
{
   mRealValue = OrbitData::GetBLshortReal(BLS_MA);    
    
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* BLshortMAP::Clone(void) const
{
   return new BLshortMAP(*this);
}


//==============================================================================
//                              BLshortState
//==============================================================================
//------------------------------------------------------------------------------
// BLshortState::BLshortState(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLshortState::BLshortState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "BrouwerMeanShort", obj, "Brouwer Mean Short State Vector",
                "", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsPlottable = false;
}

//------------------------------------------------------------------------------
// BLshortState::BLshortState(const BLshortState &copy)
//------------------------------------------------------------------------------
BLshortState::BLshortState(const BLshortState &copy)
   : OrbitRvec6(copy)
{
}

//------------------------------------------------------------------------------
// BLshortState& BLshortState::operator=(const BLshortState &right)
//------------------------------------------------------------------------------
BLshortState& BLshortState::operator=(const BLshortState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// BLshortState::~BLshortState()
//------------------------------------------------------------------------------
BLshortState::~BLshortState()
{
}

//------------------------------------------------------------------------------
// bool BLshortState::Evaluate()
//------------------------------------------------------------------------------
bool BLshortState::Evaluate()
{
   mRvec6Value = GetBLshortState();
   
   #if DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("BLshortState::Evaluate() mRvec6Value =\n%s\n", mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
}

//------------------------------------------------------------------------------
// GmatBase* BLshortState::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BLshortState::Clone(void) const
{
   return new BLshortState(*this);
}

