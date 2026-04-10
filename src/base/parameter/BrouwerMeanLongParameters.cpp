//$Id: BrouwerMeanLongParameters.cpp 2013-05-24 Youngkwang Kim $
//------------------------------------------------------------------------------
//                            File: BrouwerMeanLongParameters.cpp
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
 * Declares Brouwer-Lyddane mean long related parameter classes.
 * BLlong_SMADP, BLlong_ECCDP, BLlong_INCDP, BLlong_RAANDP, BLlong_AOPDP, BLlong_MADP, BLlong_State
 */
//------------------------------------------------------------------------------

#include "BrouwerMeanLongParameters.hpp"
#include "ColorTypes.hpp"


//==============================================================================
//                              BLlongSMADP
//==============================================================================
//------------------------------------------------------------------------------
// BLlongSMADP::BLlongSMADP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLlongSMADP::BLlongSMADP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerLongSMA", obj, "Long-term averaged mean SMA", "Km",
               GmatParam::COORD_SYS, BLL_SMA, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
}

//------------------------------------------------------------------------------
// BLlongSMADP::BLlongSMADP(const BLlongSMADP &copy)
//------------------------------------------------------------------------------
BLlongSMADP::BLlongSMADP(const BLlongSMADP &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// BLlongSMADP& BLlongSMADP::operator=(const BLlongSMADP&right)
//------------------------------------------------------------------------------
BLlongSMADP& BLlongSMADP::operator=(const BLlongSMADP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// BLlongSMADP::~BLlongSMADP()
//------------------------------------------------------------------------------
BLlongSMADP::~BLlongSMADP()
{
}

//------------------------------------------------------------------------------
// bool BLlongSMADP::Evaluate()
//------------------------------------------------------------------------------
bool BLlongSMADP::Evaluate()
{

   mRealValue = OrbitData::GetBLlongReal(BLL_SMA);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* BLlongSMADP::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BLlongSMADP::Clone(void) const
{
   return new BLlongSMADP(*this);
}

//==============================================================================
//                              BLlongECCDP
//==============================================================================
//------------------------------------------------------------------------------
// BLlongECCDP::BLlongECCDP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLlongECCDP::BLlongECCDP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerLongECC", obj, "Long-term averaged mean ECC", " ",
               GmatParam::COORD_SYS, BLL_ECC, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
}

//------------------------------------------------------------------------------
// BLlongECCDP::BLlongECCDP(const BLlongECCDP &copy)
//------------------------------------------------------------------------------
BLlongECCDP::BLlongECCDP(const BLlongECCDP &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// BLlongECCDP& BLlongECCDP::operator=(const BLlongECCDP&right)
//------------------------------------------------------------------------------
BLlongECCDP& BLlongECCDP::operator=(const BLlongECCDP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// BLlongECCDP::~BLlongECCDP()
//------------------------------------------------------------------------------
BLlongECCDP::~BLlongECCDP()
{
}

//------------------------------------------------------------------------------
// bool BLlongECCDP::Evaluate()
//------------------------------------------------------------------------------
bool BLlongECCDP::Evaluate()
{

   mRealValue = OrbitData::GetBLlongReal(BLL_ECC);
   
   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* BLlongECCDP::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BLlongECCDP::Clone(void) const
{
   return new BLlongECCDP(*this);
}

//==============================================================================
//                              BLlongINCDP
//==============================================================================
BLlongINCDP::BLlongINCDP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerLongINC", obj, "Long-term averaged mean INC", "Deg",
               GmatParam::COORD_SYS, BLL_INC, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
}

//------------------------------------------------------------------------------
// BLlongINCDP(const BLlongINCDP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLlongINCDP::BLlongINCDP(const BLlongINCDP &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const BLlongINCDP& operator=(const BLlongINCDP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLlongINCDP& BLlongINCDP::operator=(const BLlongINCDP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BLlongINCDP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BLlongINCDP::~BLlongINCDP()
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
bool BLlongINCDP::Evaluate()
{
   mRealValue = OrbitData::GetBLlongReal(BLL_INC);    
    
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
GmatBase* BLlongINCDP::Clone(void) const
{
   return new BLlongINCDP(*this);
}



//==============================================================================
//                              BLlongRAANDP
//==============================================================================
//------------------------------------------------------------------------------
// BLlongRAANDP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLlongRAANDP::BLlongRAANDP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerLongRAAN", obj, "Long-term averaged mean RAAN", "Deg",
               GmatParam::COORD_SYS, BLL_RAAN, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// BLlongRAANDP(const BLlongRAANDP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLlongRAANDP::BLlongRAANDP(const BLlongRAANDP &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const BLlongRAANDP& operator=(const BLlongRAANDP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLlongRAANDP& BLlongRAANDP::operator=(const BLlongRAANDP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BLlongRAANDP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BLlongRAANDP::~BLlongRAANDP()
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
bool BLlongRAANDP::Evaluate()
{
   mRealValue = OrbitData::GetBLlongReal(BLL_RAAN);    
    
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
GmatBase* BLlongRAANDP::Clone(void) const
{
   return new BLlongRAANDP(*this);
}


//==============================================================================
//                              BLlongAOPDP
//==============================================================================
//------------------------------------------------------------------------------
// BLlongAOPDP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLlongAOPDP::BLlongAOPDP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerLongAOP", obj, "Long-term averaged mean AOP", "Deg",
               GmatParam::COORD_SYS, BLL_AOP, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// BLlongAOPDP(const BLlongAOPDP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLlongAOPDP::BLlongAOPDP(const BLlongAOPDP &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const BLlongAOPDP& operator=(const BLlongAOPDP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLlongAOPDP& BLlongAOPDP::operator=(const BLlongAOPDP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BLlongAOPDP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BLlongAOPDP::~BLlongAOPDP()
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
bool BLlongAOPDP::Evaluate()
{
   mRealValue = OrbitData::GetBLlongReal(BLL_AOP);    
    
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
GmatBase* BLlongAOPDP::Clone(void) const
{
   return new BLlongAOPDP(*this);
}


//==============================================================================
//                              BLlongMADP
//==============================================================================
//------------------------------------------------------------------------------
// BLlongMADP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLlongMADP::BLlongMADP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "BrouwerLongMA", obj, "Long-term averaged mean MA", "Deg",
               GmatParam::COORD_SYS, BLL_MA, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// BLlongMADP(const BLlongMADP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLlongMADP::BLlongMADP(const BLlongMADP &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const BLlongMADP& operator=(const BLlongMADP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BLlongMADP& BLlongMADP::operator=(const BLlongMADP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BLlongMADP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BLlongMADP::~BLlongMADP()
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
bool BLlongMADP::Evaluate()
{
   mRealValue = OrbitData::GetBLlongReal(BLL_MA);    
    
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
GmatBase* BLlongMADP::Clone(void) const
{
   return new BLlongMADP(*this);
}


//==============================================================================
//                              BLlongState
//==============================================================================
//------------------------------------------------------------------------------
// BLlongState::BLlongState(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
BLlongState::BLlongState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "BrouwerMeanLong", obj, "Brouwer Mean Long State Vector",
                "", GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsPlottable = false;
}

//------------------------------------------------------------------------------
// BLlongState::BLlongState(const BLlongState &copy)
//------------------------------------------------------------------------------
BLlongState::BLlongState(const BLlongState &copy)
   : OrbitRvec6(copy)
{
}

//------------------------------------------------------------------------------
// BLlongState& BLlongState::operator=(const BLlongState &right)
//------------------------------------------------------------------------------
BLlongState& BLlongState::operator=(const BLlongState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// BLlongState::~BLlongState()
//------------------------------------------------------------------------------
BLlongState::~BLlongState()
{
}

//------------------------------------------------------------------------------
// bool BLlongState::Evaluate()
//------------------------------------------------------------------------------
bool BLlongState::Evaluate()
{
   mRvec6Value = GetBLlongState();
   
   #if DEBUG_CARTESIAN_PARAM
   MessageInterface::ShowMessage
      ("EquinState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
}

//------------------------------------------------------------------------------
// GmatBase* BLlongState::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BLlongState::Clone(void) const
{
   return new BLlongState(*this);
}

