//$Id: OutgoingAsymptoteParameters.cpp 2013-05-24 Youngkwang Kim $
//------------------------------------------------------------------------------
//                            File: OutgoingAsymptoteParameters.cpp
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
 * Declares Outgoing Asymptote related parameter classes.
 * OutAsymRadPer, OutAsymC3Energy, OutAsym_RHA, OutAsym_DHA, OutAsym_BVAZI, OutAsym_State
 */
//------------------------------------------------------------------------------

#include "OutgoingAsymptoteParameters.hpp"
#include "ColorTypes.hpp"


//==============================================================================
//                              OutAsymRadPer
//==============================================================================
OutAsymRadPer::OutAsymRadPer(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "OutgoingRadPer", obj, "Hyperbolic Outgoing Asymptote Radius at Periapsis", "Km",
               GmatParam::ORIGIN, OUTASYM_RADPER, true)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   SetRequiresCelestialBodyCSOrigin(true);
}

//------------------------------------------------------------------------------
// OutAsymRadPer(const OutAsymRadPer &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OutAsymRadPer::OutAsymRadPer(const OutAsymRadPer &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const OutAsymRadPer& operator=(const OutAsymRadPer &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OutAsymRadPer& OutAsymRadPer::operator=(const OutAsymRadPer &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~OutAsymRadPer()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OutAsymRadPer::~OutAsymRadPer()
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
bool OutAsymRadPer::Evaluate()
{
   mRealValue = OrbitData::GetOutAsymReal(OUTASYM_RADPER);    
   
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
GmatBase* OutAsymRadPer::Clone(void) const
{
   return new OutAsymRadPer(*this);
}



//==============================================================================
//                              OutAsymC3Energy
//==============================================================================
OutAsymC3Energy::OutAsymC3Energy(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "OutgoingC3Energy", obj, "Hyperbolic Outgoing Asymptote C3Energy Energy", "Km^2/s^2",
               GmatParam::ORIGIN, OUTASYM_C3_ENERGY, true)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   SetRequiresCelestialBodyCSOrigin(true);
}

//------------------------------------------------------------------------------
// OutAsymC3Energy(const OutAsymC3Energy &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OutAsymC3Energy::OutAsymC3Energy(const OutAsymC3Energy &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const OutAsymC3Energy& operator=(const OutAsymC3Energy &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OutAsymC3Energy& OutAsymC3Energy::operator=(const OutAsymC3Energy &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~OutAsymC3Energy()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OutAsymC3Energy::~OutAsymC3Energy()
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
bool OutAsymC3Energy::Evaluate()
{
   mRealValue = OrbitData::GetOutAsymReal(OUTASYM_C3_ENERGY);    
   
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
GmatBase* OutAsymC3Energy::Clone(void) const
{
   return new OutAsymC3Energy(*this);
}



//==============================================================================
//                              OutAsymRHA
//==============================================================================
OutAsymRHA::OutAsymRHA(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "OutgoingRHA", obj, "RA of Hyperbolic Outgoing Asymptote", "Deg",
               GmatParam::COORD_SYS, OUTASYM_RHA, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}

//------------------------------------------------------------------------------
// OutAsymRHA(const OutAsymRHA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OutAsymRHA::OutAsymRHA(const OutAsymRHA &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const OutAsymRHA& operator=(const OutAsymRHA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OutAsymRHA& OutAsymRHA::operator=(const OutAsymRHA &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~OutAsymRHA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OutAsymRHA::~OutAsymRHA()
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
bool OutAsymRHA::Evaluate()
{
   mRealValue = OrbitData::GetOutAsymReal(OUTASYM_RHA);    
    
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
GmatBase* OutAsymRHA::Clone(void) const
{
   return new OutAsymRHA(*this);
}



//==============================================================================
//                              OutAsymDHA
//==============================================================================
OutAsymDHA::OutAsymDHA(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "OutgoingDHA", obj, "DEC of Hyperbolic Outgoing Asymptote", "Deg",
               GmatParam::COORD_SYS, OUTASYM_DHA, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}

//------------------------------------------------------------------------------
// OutAsymDHA(const OutAsymDHA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OutAsymDHA::OutAsymDHA(const OutAsymDHA &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const OutAsymDHA& operator=(const OutAsymDHA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OutAsymDHA& OutAsymDHA::operator=(const OutAsymDHA &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~OutAsymDHA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OutAsymDHA::~OutAsymDHA()
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
bool OutAsymDHA::Evaluate()
{
   mRealValue = OrbitData::GetOutAsymReal(OUTASYM_DHA);    
    
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
GmatBase* OutAsymDHA::Clone(void) const
{
   return new OutAsymDHA(*this);
}



//==============================================================================
//                              OutAsymBVAZI
//==============================================================================
OutAsymBVAZI::OutAsymBVAZI(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "OutgoingBVAZI", obj, "B-vector AZI at Infinity of Hyperbolic Outgoing Asymptote", "Deg",
               GmatParam::COORD_SYS, OUTASYM_BVAZI, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}

//------------------------------------------------------------------------------
// OutAsymBVAZI(const OutAsymBVAZI &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OutAsymBVAZI::OutAsymBVAZI(const OutAsymBVAZI &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const OutAsymBVAZI& operator=(const OutAsymBVAZI &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OutAsymBVAZI& OutAsymBVAZI::operator=(const OutAsymBVAZI &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~OutAsymBVAZI()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OutAsymBVAZI::~OutAsymBVAZI()
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
bool OutAsymBVAZI::Evaluate()
{
   mRealValue = OrbitData::GetOutAsymReal(OUTASYM_BVAZI);    
    
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
GmatBase* OutAsymBVAZI::Clone(void) const
{
   return new OutAsymBVAZI(*this);
}



//==============================================================================
//                              OutAsymState
//==============================================================================
//------------------------------------------------------------------------------
// OutAsymState::OutAsymState(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
OutAsymState::OutAsymState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "OutgoingAsymptote", obj, "Hyperbolic Outgoing Asymptote State Vector",
                "", GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsPlottable = false;
}

//------------------------------------------------------------------------------
// OutAsymState::OutAsymState(const OutAsymState &copy)
//------------------------------------------------------------------------------
OutAsymState::OutAsymState(const OutAsymState &copy)
   : OrbitRvec6(copy)
{
}

//------------------------------------------------------------------------------
// OutAsymState& OutAsymState::operator=(const OutAsymState &right)
//------------------------------------------------------------------------------
OutAsymState& OutAsymState::operator=(const OutAsymState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// OutAsymState::~OutAsymState()
//------------------------------------------------------------------------------
OutAsymState::~OutAsymState()
{
}

//------------------------------------------------------------------------------
// bool OutAsymState::Evaluate()
//------------------------------------------------------------------------------
bool OutAsymState::Evaluate()
{
   mRvec6Value = GetOutAsymState();
   
   #if DEBUG_CARTESIAN_PARAM
   MessageInterface::ShowMessage
      ("EquinState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif
   
   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
}

//------------------------------------------------------------------------------
// GmatBase* OutAsymState::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* OutAsymState::Clone(void) const
{
   return new OutAsymState(*this);
}
