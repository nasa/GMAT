//$Id$
//------------------------------------------------------------------------------
//                             File: KeplerianParameters.cpp
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
// Author: Linda Jun
// Created: 2004/03/12
//
/**
 * Implements Keplerian related parameter classes.
 *   KepSMA, KepEcc, KepInc, KepAOP, KepRAAN, KepTA, KepMA, KepEA, KepHA,
 *   KepMM, KepElem, ModKepRadApo, ModKepRadPer, ModKepElem
 */
//------------------------------------------------------------------------------

#include "KeplerianParameters.hpp"

//==============================================================================
//                              KepSMA
//==============================================================================
/**
 * Implements Keplerian Semimajor Axis class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepSMA(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepSMA::KepSMA(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "SMA", obj, "Orbit Semi-Major Axis", "Km", GmatParam::ORIGIN, KEP_SMA, true)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// KepSMA(const KepSMA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepSMA::KepSMA(const KepSMA &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepSMA& operator=(const KepSMA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepSMA& KepSMA::operator=(const KepSMA &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~KepSMA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepSMA::~KepSMA()
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
bool KepSMA::Evaluate()
{
   mRealValue = OrbitData::GetKepReal(KEP_SMA);    
   
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
GmatBase* KepSMA::Clone(void) const
{
   return new KepSMA(*this);
}


//==============================================================================
//                              KepEcc
//==============================================================================
/**
 * Implements Keplerian Eccentricity class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepEcc(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepEcc::KepEcc(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "ECC", obj, "Eccentricity", " ", GmatParam::ORIGIN, KEP_ECC, true)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// KepEcc(const KepEcc &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepEcc::KepEcc(const KepEcc &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepEcc& operator=(const KepEcc &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepEcc& KepEcc::operator=(const KepEcc &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~KepEcc()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepEcc::~KepEcc()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool KepEcc::Evaluate()
{
   mRealValue = OrbitData::GetKepReal(KEP_ECC);
   
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
GmatBase* KepEcc::Clone(void) const
{
   return new KepEcc(*this);
}


//==============================================================================
//                              KepInc
//==============================================================================
/**
 * Implements Keplerian Inclinatin class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepInc(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepInc::KepInc(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "INC", obj, "Orbit Inclination", "Deg", GmatParam::COORD_SYS, KEP_INC, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_180;
}


//------------------------------------------------------------------------------
// KepInc(const KepInc &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepInc::KepInc(const KepInc &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepInc& operator=(const KepInc &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepInc& KepInc::operator=(const KepInc &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~KepInc()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepInc::~KepInc()
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
bool KepInc::Evaluate()
{
   mRealValue = OrbitData::GetKepReal(KEP_INC);    
    
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
GmatBase* KepInc::Clone(void) const
{
   return new KepInc(*this);
}


//==============================================================================
//                              KepAOP
//==============================================================================
/**
 * Implements Keplerian Argument of Periapsis class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepAOP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepAOP::KepAOP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "AOP", obj, "Argument of Periapsis", "Deg",
               GmatParam::COORD_SYS, KEP_AOP, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// KepAOP(const KepAOP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepAOP::KepAOP(const KepAOP &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepAOP& operator=(const KepAOP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepAOP& KepAOP::operator=(const KepAOP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~KepAOP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepAOP::~KepAOP()
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
bool KepAOP::Evaluate()
{
   mRealValue = OrbitData::GetKepReal(KEP_AOP);    
    
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
GmatBase* KepAOP::Clone(void) const
{
   return new KepAOP(*this);
}


//==============================================================================
//                              KepRAAN
//==============================================================================
/**
 * Implements Keplerian Right Ascention of Ascending Node class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepRAAN(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepRAAN::KepRAAN(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "RAAN", obj, "RA of Asscending Node", "Deg",
               GmatParam::COORD_SYS, KEP_RAAN, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// KepRAAN(const KepRAAN &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepRAAN::KepRAAN(const KepRAAN &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepRAAN& operator=(const KepRAAN &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepRAAN& KepRAAN::operator=(const KepRAAN &right)
{
   if (this != &right)
      OrbitReal::operator=(right);
    
   return *this;
}


//------------------------------------------------------------------------------
// ~KepRAAN()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepRAAN::~KepRAAN()
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
bool KepRAAN::Evaluate()
{
   mRealValue = OrbitData::GetKepReal(KEP_RAAN);    
    
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
GmatBase* KepRAAN::Clone(void) const
{
   return new KepRAAN(*this);
}


//==============================================================================
//                              KepRADN
//==============================================================================
/**
 * Implements Keplerian Right Ascention of Ascending Node class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepRADN(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepRADN::KepRADN(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "RADN", obj, "RA of Asscending Node", "Deg", GmatParam::COORD_SYS,
               KEP_RADN, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
}


//------------------------------------------------------------------------------
// KepRADN(const KepRADN &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepRADN::KepRADN(const KepRADN &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepRADN& operator=(const KepRADN &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepRADN& KepRADN::operator=(const KepRADN &right)
{
   if (this != &right)
      OrbitReal::operator=(right);
    
   return *this;
}


//------------------------------------------------------------------------------
// ~KepRADN()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepRADN::~KepRADN()
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
bool KepRADN::Evaluate()
{
   mRealValue = OrbitData::GetKepReal(KEP_RADN);    
   
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
GmatBase* KepRADN::Clone(void) const
{
   return new KepRADN(*this);
}


//==============================================================================
//                              KepTA
//==============================================================================
/**
 * Implements Keplerian True Anomaly class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepTA(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepTA::KepTA(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "TA", obj, "True Anomaly", "Deg", GmatParam::ORIGIN, KEP_TA, true)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// KepTA(const KepTA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepTA::KepTA(const KepTA &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepTA& operator=(const KepTA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepTA& KepTA::operator=(const KepTA &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~KepTA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepTA::~KepTA()
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
bool KepTA::Evaluate()
{
   mRealValue = OrbitData::GetKepReal(KEP_TA);    
    
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
GmatBase* KepTA::Clone(void) const
{
   return new KepTA(*this);
}


//==============================================================================
//                              KepMA
//==============================================================================
/**
 * Implements Keplerian Mean Anomaly class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepMA(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepMA::KepMA(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MA", obj, "Mean Anomaly", "Deg", GmatParam::ORIGIN, KEP_MA, false)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// KepMA(const KepMA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepMA::KepMA(const KepMA &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepMA& operator=(const KepMA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepMA& KepMA::operator=(const KepMA &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~KepMA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepMA::~KepMA()
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
bool KepMA::Evaluate()
{
   mRealValue = OrbitData::GetKepReal(KEP_MA);    
    
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
GmatBase* KepMA::Clone(void) const
{
   return new KepMA(*this);
}


//==============================================================================
//                              KepEA
//==============================================================================
/**
 * Implements Keplerian Eccentric Anomaly class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepEA(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepEA::KepEA(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "EA", obj, "Eccentric Anomaly", "Deg", GmatParam::ORIGIN, KEP_EA, false)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// KepEA(const KepEA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepEA::KepEA(const KepEA &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepEA& operator=(const KepEA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepEA& KepEA::operator=(const KepEA &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~KepEA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepEA::~KepEA()
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
bool KepEA::Evaluate()
{
   mRealValue = OrbitData::GetKepReal(KEP_EA);    
    
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
GmatBase* KepEA::Clone(void) const
{
   return new KepEA(*this);
}


//==============================================================================
//                              KepHA
//==============================================================================
/**
 * Implements Keplerian Hyperbolic Anomaly class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepHA(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepHA::KepHA(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "HA", obj, "Hyperbolic Anomaly", "Deg", GmatParam::ORIGIN, KEP_HA, false)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   mIsAngleParam = true;
   mCycleType = GmatParam::ZERO_360;
}


//------------------------------------------------------------------------------
// KepHA(const KepHA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepHA::KepHA(const KepHA &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepHA& operator=(const KepHA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepHA& KepHA::operator=(const KepHA &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~KepHA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepHA::~KepHA()
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
bool KepHA::Evaluate()
{
   mRealValue = OrbitData::GetKepReal(KEP_HA);    
   
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
GmatBase* KepHA::Clone(void) const
{
   return new KepHA(*this);
}


//==============================================================================
//                              KepMM
//==============================================================================
/**
 * Implements Keplerian Mean Motion class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepMM(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepMM::KepMM(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MM", obj, "Mean Motion", "Deg", GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// KepMM(const KepMM &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepMM::KepMM(const KepMM &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const KepMM& operator=(const KepMM &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepMM& KepMM::operator=(const KepMM &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~KepMM()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepMM::~KepMM()
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
bool KepMM::Evaluate()
{
   mRealValue = OrbitData::GetOtherKepReal(MM);
    
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
GmatBase* KepMM::Clone(void) const
{
   return new KepMM(*this);
}


//==============================================================================
//                              KepElem
//==============================================================================
/**
 * Implements Keplerian Elements class.
 *   6 elements: KepSMA, KepEcc, KepInc, KepRAAN, KepAOP, KepTA
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepElem(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
KepElem::KepElem(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "Keplerian", obj, "Keplerian Elements", " ", GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mIsPlottable = false;
}


//------------------------------------------------------------------------------
// KepElem(const KepElem &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepElem::KepElem(const KepElem &copy)
   : OrbitRvec6(copy)
{
}


//------------------------------------------------------------------------------
// const KepElem& operator=(const KepElem &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepElem& KepElem::operator=(const KepElem &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~KepElem()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepElem::~KepElem()
{
}


//--------------------------------------
// Inherited methods from Parameter
//--------------------------------------

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool KepElem::Evaluate()
{
   mRvec6Value = OrbitData::GetKepState();

   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
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
GmatBase* KepElem::Clone(void) const
{
   return new KepElem(*this);
}


//==============================================================================
//                              ModKepRadApo
//==============================================================================
/**
 * Implements radius of apoapsis.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ModKepRadApo(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ModKepRadApo::ModKepRadApo(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "RadApo", obj, "Radius at Apoapsis", "Km",
               GmatParam::ORIGIN, MODKEP_RADAPO, true)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// ModKepRadApo(const ModKepRadApo &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ModKepRadApo::ModKepRadApo(const ModKepRadApo &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const ModKepRadApo& operator=(const ModKepRadApo &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ModKepRadApo&
ModKepRadApo::operator=(const ModKepRadApo &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ModKepRadApo()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ModKepRadApo::~ModKepRadApo()
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
bool ModKepRadApo::Evaluate()
{
   if (mOrigin == NULL)
      OrbitData::InitializeRefObjects();
   
   mRealValue = OrbitData::GetModKepReal(MODKEP_RADAPO);    
   
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
GmatBase* ModKepRadApo::Clone(void) const
{
   return new ModKepRadApo(*this);
}


//==============================================================================
//                              ModKepRadPer
//==============================================================================
/**
 * Implements radius of periapsis.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ModKepRadPer(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ModKepRadPer::ModKepRadPer(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "RadPer", obj, "Radius at Periapsis", "Km",
               GmatParam::ORIGIN, MODKEP_RADPER, true)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// ModKepRadPer(const ModKepRadPer &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ModKepRadPer::ModKepRadPer(const ModKepRadPer &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const ModKepRadPer& operator=(const ModKepRadPer &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ModKepRadPer&
ModKepRadPer::operator=(const ModKepRadPer &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ModKepRadPer()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ModKepRadPer::~ModKepRadPer()
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
bool ModKepRadPer::Evaluate()
{
   if (mOrigin == NULL)
      OrbitData::InitializeRefObjects();
   
   mRealValue = OrbitData::GetModKepReal(MODKEP_RADPER);    
   
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
GmatBase* ModKepRadPer::Clone(void) const
{
   return new ModKepRadPer(*this);
}


//==============================================================================
//                              ModKepElem
//==============================================================================
/**
 * Implements Keplerian Elements class.
 *   6 elements: ModKepRadPer, ModKepRadApo, KepInc, KepRAAN, KepAOP, KepTA
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ModKepElem(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ModKepElem::ModKepElem(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "ModKeplerian", obj, "Keplerian Elements", " ",
                GmatParam::COORD_SYS)
{
   // Parameter member data
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   mIsPlottable = false;
}


//------------------------------------------------------------------------------
// ModKepElem(const ModKepElem &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ModKepElem::ModKepElem(const ModKepElem &copy)
   : OrbitRvec6(copy)
{
}


//------------------------------------------------------------------------------
// const ModKepElem& operator=(const ModKepElem &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ModKepElem& ModKepElem::operator=(const ModKepElem &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ModKepElem()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ModKepElem::~ModKepElem()
{
}


//--------------------------------------
// Inherited methods from Parameter
//--------------------------------------

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool ModKepElem::Evaluate()
{
   mRvec6Value = OrbitData::GetModKepState();

   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
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
GmatBase* ModKepElem::Clone(void) const
{
   return new ModKepElem(*this);
}
