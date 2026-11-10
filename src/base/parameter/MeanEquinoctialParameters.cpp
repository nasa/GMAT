//------------------------------------------------------------------------------
//                            MeanEquinoctialParameters.cpp
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
// Author: Allen Fu
// Created: 2026/08/10
//
#include "MeanEquinoctialParameters.hpp"
#include "ColorTypes.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MEAN_EQUINOCTIAL_PARAM

//#define USE_PREDEFINED_COLORS


//------------------------------------------------------------------------------
// MeanEquinSma(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinSma::MeanEquinSma(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialSMA", obj, "Mean Equinoctial SMA", "km", GmatParam::COORD_SYS, MEQ_SMA, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinSma(const MeanEquinSma &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinSma::MeanEquinSma(const MeanEquinSma &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinSma& operator=(const MeanEquinSma &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinSma& MeanEquinSma::operator=(const MeanEquinSma &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinSma()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinSma::~MeanEquinSma()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinSma::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_SMA);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinSma::Clone() const
{
   return new MeanEquinSma(*this);
}


//------------------------------------------------------------------------------
// MeanEquinMM(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinMM::MeanEquinMM(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialMM", obj, "Mean Equinoctial Mean Motion", "deg/s",
         GmatParam::COORD_SYS, MEQ_MM, false)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinMM(const MeanEquinMM &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinMM::MeanEquinMM(const MeanEquinMM &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinMM& operator=(const MeanEquinMM &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinMM& MeanEquinMM::operator=(const MeanEquinMM &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinMM()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinMM::~MeanEquinMM()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinMM::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_MM);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinMM::Clone() const
{
   return new MeanEquinMM(*this);
}


//------------------------------------------------------------------------------
// MeanEquinH(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinH::MeanEquinH(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialH", obj, "Mean Equinoctial H", "", GmatParam::COORD_SYS, MEQ_H, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinH(const MeanEquinH &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinH::MeanEquinH(const MeanEquinH &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinH& operator=(const MeanEquinH &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinH& MeanEquinH::operator=(const MeanEquinH &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinH()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinH::~MeanEquinH()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinH::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_H);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinH::Clone() const
{
   return new MeanEquinH(*this);
}


//------------------------------------------------------------------------------
// MeanEquinHDot(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinHDot::MeanEquinHDot(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialHDot", obj, "Mean Equinoctial H dot", "1/s",
         GmatParam::COORD_SYS, MEQ_H_DOT, false)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinHDot(const MeanEquinHDot &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinHDot::MeanEquinHDot(const MeanEquinHDot &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinHDot& operator=(const MeanEquinHDot &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinHDot& MeanEquinHDot::operator=(const MeanEquinHDot &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinHDot()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinHDot::~MeanEquinHDot()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinHDot::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_H_DOT);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinHDot::Clone() const
{
   return new MeanEquinHDot(*this);
}


//------------------------------------------------------------------------------
// MeanEquinK(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinK::MeanEquinK(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialK", obj, "Mean Equinoctial K", "", GmatParam::COORD_SYS, MEQ_K, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinK(const MeanEquinK &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinK::MeanEquinK(const MeanEquinK &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinK& operator=(const MeanEquinK &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinK& MeanEquinK::operator=(const MeanEquinK &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinK()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinK::~MeanEquinK()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinK::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_K);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinK::Clone() const
{
   return new MeanEquinK(*this);
}


//------------------------------------------------------------------------------
// MeanEquinKDot(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinKDot::MeanEquinKDot(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialKDot", obj, "Mean Equinoctial K dot", "1/s",
         GmatParam::COORD_SYS, MEQ_K_DOT, false)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinKDot(const MeanEquinKDot &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinKDot::MeanEquinKDot(const MeanEquinKDot &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinKDot& operator=(const MeanEquinKDot &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinKDot& MeanEquinKDot::operator=(const MeanEquinKDot &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinKDot()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinKDot::~MeanEquinKDot()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinKDot::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_K_DOT);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinKDot::Clone() const
{
   return new MeanEquinKDot(*this);
}


//------------------------------------------------------------------------------
// MeanEquinP(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinP::MeanEquinP(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialP", obj, "Mean Equinoctial P", "", GmatParam::COORD_SYS, MEQ_P, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinP(const MeanEquinP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinP::MeanEquinP(const MeanEquinP &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinP& operator=(const MeanEquinP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinP& MeanEquinP::operator=(const MeanEquinP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinP::~MeanEquinP()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinP::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_P);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinP::Clone() const
{
   return new MeanEquinP(*this);
}


//------------------------------------------------------------------------------
// MeanEquinPDot(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinPDot::MeanEquinPDot(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialPDot", obj, "Mean Equinoctial P dot", "1/s",
         GmatParam::COORD_SYS, MEQ_P_DOT, false)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinPDot(const MeanEquinPDot &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinPDot::MeanEquinPDot(const MeanEquinPDot &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinPDot& operator=(const MeanEquinPDot &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinPDot& MeanEquinPDot::operator=(const MeanEquinPDot &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinPDot()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinPDot::~MeanEquinPDot()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinPDot::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_P_DOT);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinPDot::Clone() const
{
   return new MeanEquinPDot(*this);
}


//------------------------------------------------------------------------------
// MeanEquinQ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinQ::MeanEquinQ(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialQ", obj, "Mean Equinoctial Q", "", GmatParam::COORD_SYS, MEQ_Q, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinQ(const MeanEquinQ &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinQ::MeanEquinQ(const MeanEquinQ &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinQ& operator=(const MeanEquinQ &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinQ& MeanEquinQ::operator=(const MeanEquinQ &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinQ()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinQ::~MeanEquinQ()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinQ::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_Q);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinQ::Clone() const
{
   return new MeanEquinQ(*this);
}


//------------------------------------------------------------------------------
// MeanEquinQDot(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinQDot::MeanEquinQDot(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialQDot", obj, "Mean Equinoctial Q dot", "1/s",
         GmatParam::COORD_SYS, MEQ_Q_DOT, false)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinQDot(const MeanEquinQDot &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinQDot::MeanEquinQDot(const MeanEquinQDot &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinQDot& operator=(const MeanEquinQDot &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinQDot& MeanEquinQDot::operator=(const MeanEquinQDot &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinQDot()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinQDot::~MeanEquinQDot()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinQDot::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_Q_DOT);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinQDot::Clone() const
{
   return new MeanEquinQDot(*this);
}


//------------------------------------------------------------------------------
// MeanEquinMlong(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinMlong::MeanEquinMlong(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "MeanEquinoctialMLONG", obj, "Mean Equinoctial Mlong", "deg", GmatParam::COORD_SYS, MEQ_MLONG, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}

//------------------------------------------------------------------------------
// MeanEquinMlong(const MeanEquinMlong &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinMlong::MeanEquinMlong(const MeanEquinMlong &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinMlong& operator=(const MeanEquinMlong &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinMlong& MeanEquinMlong::operator=(const MeanEquinMlong &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinMlong()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinMlong::~MeanEquinMlong()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinMlong::Evaluate()
{
   mRealValue = OrbitData::GetMeanEquinReal(MEQ_MLONG);

   if (mRealValue == GmatOrbitConstants::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinMlong::Clone() const
{
   return new MeanEquinMlong(*this);
}


//------------------------------------------------------------------------------
// MeanEquinState(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
MeanEquinState::MeanEquinState(const std::string &name, GmatBase *obj)
   : OrbitRvec6(name, "MeanEquinoctial", obj, "Mean Equinoctial State Vector",
                "", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
   mIsPlottable = false;
}

//------------------------------------------------------------------------------
// MeanEquinState(const MeanEquinState &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinState::MeanEquinState(const MeanEquinState &copy)
   : OrbitRvec6(copy)
{
}

//------------------------------------------------------------------------------
// MeanEquinState& operator=(const MeanEquinState &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MeanEquinState& MeanEquinState::operator=(const MeanEquinState &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~MeanEquinState()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MeanEquinState::~MeanEquinState()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MeanEquinState::Evaluate()
{
   mRvec6Value = GetMeanEquinState();

   #ifdef DEBUG_MEAN_EQUINOCTIAL_PARAM
   MessageInterface::ShowMessage
      ("MeanEquinState::Evaluate() mRvec6Value =\n%s\n",
       mRvec6Value.ToString().c_str());
   #endif

   return mRvec6Value.IsValid(GmatOrbitConstants::ORBIT_REAL_UNDEFINED);
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MeanEquinState::Clone() const
{
   return new MeanEquinState(*this);
}
