//$Id$
//------------------------------------------------------------------------------
//                             File: TimeParameters.cpp
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
 * Implements Time related parameter classes.
 *   CurrA1MJD, A1ModJulian, A1Gregorian, TAIModJulian, TAIGregorian, TTModJulian,
 *   TTGregorian, TDBModJulian, TDBGregorian,
 *   UTCModJulian, UTCGregorian, ElapsedDays, ElapsedDaysFromStart, ElapsedSecs,
 *   ElapsedSecsFromStart
 */
//------------------------------------------------------------------------------
#include "TimeParameters.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_TIME_EVAL

//==============================================================================
//                              CurrA1MJD
//==============================================================================

//------------------------------------------------------------------------------
// CurrA1MJD(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
CurrA1MJD::CurrA1MJD(const std::string &name, GmatBase *obj)
   : TimeReal(name, "CurrA1MJD", obj, "A1 Mod. Julian Days", "day", true,
         Gmat::SPACECRAFT)
{
   // Write deprecated message once per GMAT session
   static bool writeDeprecatedMsg = true;

   if (writeDeprecatedMsg)
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** \"CurrA1MJD\" is deprecated and will be "
          "removed from a future build; please use \"A1ModJulian\" "
          "instead.\n");
      writeDeprecatedMsg = false;
   }
}


//------------------------------------------------------------------------------
// CurrA1MJD::CurrA1MJD(const CurrA1MJD &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CurrA1MJD::CurrA1MJD(const CurrA1MJD &param)
   : TimeReal(param)
{
}

//------------------------------------------------------------------------------
// CurrA1MJD& CurrA1MJD::operator=(const CurrA1MJD &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CurrA1MJD& CurrA1MJD::operator=(const CurrA1MJD &right)
{
   if (this != &right)
      TimeReal::operator=(right);
      
   return *this;
}

//------------------------------------------------------------------------------
// CurrA1MJD::~CurrA1MJD()
//------------------------------------------------------------------------------
CurrA1MJD::~CurrA1MJD()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool CurrA1MJD::Evaluate()
{
   mRealValue = TimeData::GetTimeReal(A1);
   
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void CurrA1MJD::SetReal(Real val)
{
   TimeData::SetTimeReal(A1, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* CurrA1MJD::Clone(void) const
{
   return new CurrA1MJD(*this);
}


//==============================================================================
//                              A1ModJulian
//==============================================================================

//------------------------------------------------------------------------------
// A1ModJulian(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
A1ModJulian::A1ModJulian(const std::string &name, GmatBase *obj)
   : TimeReal(name, "A1ModJulian", obj, "A1 Mod. Julian Days", "day", true,
         Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
// A1ModJulian::A1ModJulian(const A1ModJulian &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
A1ModJulian::A1ModJulian(const A1ModJulian &param)
   : TimeReal(param)
{
}

//------------------------------------------------------------------------------
// A1ModJulian& A1ModJulian::operator=(const A1ModJulian &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
A1ModJulian& A1ModJulian::operator=(const A1ModJulian &right)
{
   if (this != &right)
      TimeReal::operator=(right);
      
   return *this;
}

//------------------------------------------------------------------------------
// A1ModJulian::~A1ModJulian()
//------------------------------------------------------------------------------
A1ModJulian::~A1ModJulian()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool A1ModJulian::Evaluate()
{
   mRealValue = TimeData::GetTimeReal(A1);

   #ifdef DEBUG_TIMEPARAM_EVAL
   MessageInterface::ShowMessage
      ("A1ModJulian::Evaluate() <%p>'%s' evaluated to %f, spacecraft=<%p>'%s'\n",
       this, GetName().c_str(), mRealValue, GetSpacecraft(),
       GetSpacecraft()->GetName().c_str());
   #endif
   
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void A1ModJulian::SetReal(Real val)
{
   TimeData::SetTimeReal(A1, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* A1ModJulian::Clone(void) const
{
   return new A1ModJulian(*this);
}


//==============================================================================
//                              A1Gregorian
//==============================================================================

//------------------------------------------------------------------------------
// A1Gregorian(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
A1Gregorian::A1Gregorian(const std::string &name, GmatBase *obj)
   : TimeString(name, "A1Gregorian", obj, "A1 Gregorian Date", "", true,
         Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
// A1Gregorian::A1Gregorian(const A1Gregorian &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
A1Gregorian::A1Gregorian(const A1Gregorian &param)
   : TimeString(param)
{
}

//------------------------------------------------------------------------------
// A1Gregorian& A1Gregorian::operator=(const A1Gregorian &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
A1Gregorian& A1Gregorian::operator=(const A1Gregorian &right)
{
   if (this != &right)
      TimeString::operator=(right);
      
   return *this;
}

//------------------------------------------------------------------------------
// A1Gregorian::~A1Gregorian()
//------------------------------------------------------------------------------
A1Gregorian::~A1Gregorian()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool A1Gregorian::Evaluate()
{
   mStringValue = TimeData::GetTimeString(A1);
   
   if (mStringValue == TIME_STRING_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetString(const std::string &val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void A1Gregorian::SetString(const std::string &val)
{
   TimeData::SetTimeString(A1, val);
   StringVar::SetString(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* A1Gregorian::Clone(void) const
{
   return new A1Gregorian(*this);
}


//==============================================================================
//                              TAIModJulian
//==============================================================================

//------------------------------------------------------------------------------
// TAIModJulian(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
TAIModJulian::TAIModJulian(const std::string &name, GmatBase *obj)
   : TimeReal(name, "TAIModJulian", obj, "TAI Mod. Julian Days", "day", true,
         Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
// TAIModJulian::TAIModJulian(const TAIModJulian &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TAIModJulian::TAIModJulian(const TAIModJulian &param)
   : TimeReal(param)
{
}

//------------------------------------------------------------------------------
// TAIModJulian& TAIModJulian::operator=(const TAIModJulian &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TAIModJulian& TAIModJulian::operator=(const TAIModJulian &right)
{
   if (this != &right)
      TimeReal::operator=(right);
      
   return *this;
}

//------------------------------------------------------------------------------
// TAIModJulian::~TAIModJulian()
//------------------------------------------------------------------------------
TAIModJulian::~TAIModJulian()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool TAIModJulian::Evaluate()
{
   mRealValue = TimeData::GetTimeReal(TAI);
   
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void TAIModJulian::SetReal(Real val)
{
   TimeData::SetTimeReal(TAI, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* TAIModJulian::Clone(void) const
{
   return new TAIModJulian(*this);
}


//==============================================================================
//                              TAIGregorian
//==============================================================================

//------------------------------------------------------------------------------
// TAIGregorian(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
TAIGregorian::TAIGregorian(const std::string &name, GmatBase *obj)
   : TimeString(name, "TAIGregorian", obj, "TAI Gregorian Date", "", true,
         Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
// TAIGregorian::TAIGregorian(const TAIGregorian &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TAIGregorian::TAIGregorian(const TAIGregorian &param)
   : TimeString(param)
{
}

//------------------------------------------------------------------------------
// TAIGregorian& TAIGregorian::operator=(const TAIGregorian &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TAIGregorian& TAIGregorian::operator=(const TAIGregorian &right)
{
   if (this != &right)
      TimeString::operator=(right);
      
   return *this;
}

//------------------------------------------------------------------------------
// TAIGregorian::~TAIGregorian()
//------------------------------------------------------------------------------
TAIGregorian::~TAIGregorian()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool TAIGregorian::Evaluate()
{
   mStringValue = TimeData::GetTimeString(TAI);
   
   if (mStringValue == TIME_STRING_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetString(const std::string &val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void TAIGregorian::SetString(const std::string &val)
{
   TimeData::SetTimeString(TAI, val);
   StringVar::SetString(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* TAIGregorian::Clone(void) const
{
   return new TAIGregorian(*this);
}


//==============================================================================
//                              TTModJulian
//==============================================================================

//------------------------------------------------------------------------------
// TTModJulian(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
TTModJulian::TTModJulian(const std::string &name, GmatBase *obj)
   : TimeReal(name, "TTModJulian", obj, "TT Mod. Julian Days", "day", true,
         Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
// TTModJulian::TTModJulian(const TTModJulian &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TTModJulian::TTModJulian(const TTModJulian &param)
   : TimeReal(param)
{
}

//------------------------------------------------------------------------------
// TTModJulian& TTModJulian::operator=(const TTModJulian &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TTModJulian& TTModJulian::operator=(const TTModJulian &right)
{
   if (this != &right)
      TimeReal::operator=(right);
      
   return *this;
}

//------------------------------------------------------------------------------
// TTModJulian::~TTModJulian()
//------------------------------------------------------------------------------
TTModJulian::~TTModJulian()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool TTModJulian::Evaluate()
{
   mRealValue = TimeData::GetTimeReal(TT);
   
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void TTModJulian::SetReal(Real val)
{
   TimeData::SetTimeReal(TT, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* TTModJulian::Clone(void) const
{
   return new TTModJulian(*this);
}


//==============================================================================
//                              TTGregorian
//==============================================================================

//------------------------------------------------------------------------------
// TTGregorian(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
TTGregorian::TTGregorian(const std::string &name, GmatBase *obj)
   : TimeString(name, "TTGregorian", obj, "TT Gregorian Date", "", true,
         Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
// TTGregorian::TTGregorian(const TTGregorian &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TTGregorian::TTGregorian(const TTGregorian &param)
   : TimeString(param)
{
}

//------------------------------------------------------------------------------
// TTGregorian& TTGregorian::operator=(const TTGregorian &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TTGregorian& TTGregorian::operator=(const TTGregorian &right)
{
   if (this != &right)
      TimeString::operator=(right);
      
   return *this;
}

//------------------------------------------------------------------------------
// TTGregorian::~TTGregorian()
//------------------------------------------------------------------------------
TTGregorian::~TTGregorian()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool TTGregorian::Evaluate()
{
   mStringValue = TimeData::GetTimeString(TT);
   
   if (mStringValue == TIME_STRING_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetString(const std::string &val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void TTGregorian::SetString(const std::string &val)
{
   TimeData::SetTimeString(TT, val);
   StringVar::SetString(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* TTGregorian::Clone(void) const
{
   return new TTGregorian(*this);
}


//==============================================================================
//                              TDBModJulian
//==============================================================================

//------------------------------------------------------------------------------
// TDBModJulian(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
TDBModJulian::TDBModJulian(const std::string &name, GmatBase *obj)
   : TimeReal(name, "TDBModJulian", obj, "TDB Mod. Julian Days", "day", true,
         Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
// TDBModJulian::TDBModJulian(const TDBModJulian &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TDBModJulian::TDBModJulian(const TDBModJulian &param)
   : TimeReal(param)
{
}


//------------------------------------------------------------------------------
// TDBModJulian& TDBModJulian::operator=(const TDBModJulian &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TDBModJulian& TDBModJulian::operator=(const TDBModJulian &right)
{
   if (this != &right)
      TimeReal::operator=(right);
      
   return *this;
}


//------------------------------------------------------------------------------
// TDBModJulian::~TDBModJulian()
//------------------------------------------------------------------------------
TDBModJulian::~TDBModJulian()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool TDBModJulian::Evaluate()
{
   mRealValue = TimeData::GetTimeReal(TDB);
   
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void TDBModJulian::SetReal(Real val)
{
   TimeData::SetTimeReal(TDB, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* TDBModJulian::Clone(void) const
{
   return new TDBModJulian(*this);
}


//==============================================================================
//                              TDBGregorian
//==============================================================================

//------------------------------------------------------------------------------
// TDBGregorian(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
TDBGregorian::TDBGregorian(const std::string &name, GmatBase *obj)
   : TimeString(name, "TDBGregorian", obj, "TDB Gregorian Date", "", true,
         Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
// TDBGregorian::TDBGregorian(const TDBGregorian &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TDBGregorian::TDBGregorian(const TDBGregorian &param)
   : TimeString(param)
{
}


//------------------------------------------------------------------------------
// TDBGregorian& TDBGregorian::operator=(const TDBGregorian &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TDBGregorian& TDBGregorian::operator=(const TDBGregorian &right)
{
   if (this != &right)
      TimeString::operator=(right);
      
   return *this;
}


//------------------------------------------------------------------------------
// TDBGregorian::~TDBGregorian()
//------------------------------------------------------------------------------
TDBGregorian::~TDBGregorian()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool TDBGregorian::Evaluate()
{
   mStringValue = TimeData::GetTimeString(TDB);
   
   if (mStringValue == TIME_STRING_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetString(const std::string &val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void TDBGregorian::SetString(const std::string &val)
{
   TimeData::SetTimeString(TDB, val);
   StringVar::SetString(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* TDBGregorian::Clone(void) const
{
   return new TDBGregorian(*this);
}


//==============================================================================
//                              UTCModJulian
//==============================================================================

//------------------------------------------------------------------------------
// UTCModJulian(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
UTCModJulian::UTCModJulian(const std::string &name, GmatBase *obj)
   : TimeReal(name, "UTCModJulian", obj, "UTC Mod. Julian Days", "day", true,
         Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
// UTCModJulian::UTCModJulian(const UTCModJulian &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
UTCModJulian::UTCModJulian(const UTCModJulian &param)
   : TimeReal(param)
{
}

//------------------------------------------------------------------------------
// UTCModJulian& UTCModJulian::operator=(const UTCModJulian &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
UTCModJulian& UTCModJulian::operator=(const UTCModJulian &right)
{
   if (this != &right)
      TimeReal::operator=(right);
      
   return *this;
}

//------------------------------------------------------------------------------
// UTCModJulian::~UTCModJulian()
//------------------------------------------------------------------------------
UTCModJulian::~UTCModJulian()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool UTCModJulian::Evaluate()
{
   mRealValue = TimeData::GetTimeReal(UTC);
   
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void UTCModJulian::SetReal(Real val)
{
   TimeData::SetTimeReal(UTC, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* UTCModJulian::Clone(void) const
{
   return new UTCModJulian(*this);
}


//==============================================================================
//                              UTCGregorian
//==============================================================================

//------------------------------------------------------------------------------
// UTCGregorian(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
UTCGregorian::UTCGregorian(const std::string &name, GmatBase *obj)
   : TimeString(name, "UTCGregorian", obj, "UTC Gregorian Date", "", true,
         Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
// UTCGregorian::UTCGregorian(const UTCGregorian &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
UTCGregorian::UTCGregorian(const UTCGregorian &param)
   : TimeString(param)
{
}

//------------------------------------------------------------------------------
// UTCGregorian& UTCGregorian::operator=(const UTCGregorian &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
UTCGregorian& UTCGregorian::operator=(const UTCGregorian &right)
{
   if (this != &right)
      TimeString::operator=(right);
      
   return *this;
}

//------------------------------------------------------------------------------
// UTCGregorian::~UTCGregorian()
//------------------------------------------------------------------------------
UTCGregorian::~UTCGregorian()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool UTCGregorian::Evaluate()
{
   mStringValue = TimeData::GetTimeString(UTC);
   
   if (mStringValue == TIME_STRING_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetString(const std::string &val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 *
 */
//------------------------------------------------------------------------------
void UTCGregorian::SetString(const std::string &val)
{
   TimeData::SetTimeString(UTC, val);
   StringVar::SetString(val);
}


//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* UTCGregorian::Clone(void) const
{
   return new UTCGregorian(*this);
}


//==============================================================================
//                              ElapsedDays
//==============================================================================

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedDays::PARAMETER_TEXT[ElapsedDaysParamCount - ParameterParamCount] =
{
   "InitialEpoch"
};

const Gmat::ParameterType
ElapsedDays::PARAMETER_TYPE[ElapsedDaysParamCount - ParameterParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedDays(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ElapsedDays::ElapsedDays(const std::string &name, GmatBase *obj)
   : TimeReal(name, "ElapsedDays", obj, "Elapsed Days", "Day", false,
         Gmat::SPACECRAFT)
{
   // GmatBase data
   parameterCount = ElapsedDaysParamCount;
}


//------------------------------------------------------------------------------
// ElapsedDays(const ElapsedDays &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ElapsedDays::ElapsedDays(const ElapsedDays &copy)
    : TimeReal(copy)
{
}


//------------------------------------------------------------------------------
// const ElapsedDays& operator=(const ElapsedDays &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ElapsedDays& ElapsedDays::operator=(const ElapsedDays &right)
{
   if (this != &right)
   {
      TimeReal::operator=(right);
   }
   return *this;
}


//------------------------------------------------------------------------------
// ~ElapsedDays()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ElapsedDays::~ElapsedDays()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool ElapsedDays::Evaluate()
{
   #ifdef DEBUG_TIME_EVAL
   MessageInterface::ShowMessage
      ("ElapsedDays::Evaluate() <%p>'%s' entered, IsGlobal:%s\n", this, GetName().c_str(),
       IsGlobal() ? "Yes" : "No");
   #endif
   
   mRealValue = GetElapsedTimeReal(DAYS);
   bool retval = true;
   if (mRealValue == TIME_REAL_UNDEFINED)
      retval = false;
   
   #ifdef DEBUG_TIME_EVAL
   MessageInterface::ShowMessage
      ("ElapsedDays::Evaluate() <%p>'%s' returning %d, mRealValue=%f\n",
       this, GetName().c_str(),retval,  mRealValue);
   #endif
   return retval;
}


//--------------------------------------
// Inherited methods from GmatBase
//--------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* ElapsedDays::Clone(void) const
{
   return new ElapsedDays(*this);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElapsedDays::GetParameterType(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedDaysParamCount)
      return PARAMETER_TYPE[id - ParameterParamCount];
   else
      return TimeReal::GetParameterType(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedDays::GetParameterTypeString(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedDaysParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - ParameterParamCount)];
   else
      return TimeReal::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedDays::GetParameterText(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedDaysParamCount)
      return PARAMETER_TEXT[id - ParameterParamCount];
   else
      return TimeReal::GetParameterText(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> parameter index.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool ElapsedDays::IsParameterReadOnly(const Integer id) const
{
   if (id == INITIAL_EPOCH)
      return true;
   
   return TimeReal::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ElapsedDays::GetParameterID(const std::string &str) const
{
   for (int i = ParameterParamCount; i < ElapsedDaysParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return TimeReal::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDays::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case INITIAL_EPOCH:
       return GetInitialEpoch();
   default:
       return TimeReal::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDays::GetRealParameter(const std::string &label) const
{
   if (label == "InitialEpoch")
      return GetInitialEpoch();
   else
      return TimeReal::GetRealParameter(label);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDays::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case INITIAL_EPOCH:
      SetInitialEpoch(value);
      return value;
   default:
      return TimeReal::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDays::SetRealParameter(const std::string &label, const Real value)
{
   if (label == "InitialEpoch")
   {
      SetInitialEpoch(value);
      return value;
   }
   else
   {
      return TimeReal::SetRealParameter(label, value);
   }
}


//==============================================================================
//                              ElapsedDaysFromStart
//==============================================================================

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedDaysFromStart::PARAMETER_TEXT[ElapsedDaysFromStartParamCount - ParameterParamCount] =
{
   "InitialEpoch"
};

const Gmat::ParameterType
ElapsedDaysFromStart::PARAMETER_TYPE[ElapsedDaysFromStartParamCount - ParameterParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedDaysFromStart(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ElapsedDaysFromStart::ElapsedDaysFromStart(const std::string &name, GmatBase *obj)
   : TimeReal(name, "ElapsedDaysFromStart", obj, "Elapsed Days", "Day", false,
         Gmat::SPACECRAFT)
{
   // GmatBase data
   parameterCount = ElapsedDaysFromStartParamCount;
}


//------------------------------------------------------------------------------
// ElapsedDaysFromStart(const ElapsedDaysFromStart &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ElapsedDaysFromStart::ElapsedDaysFromStart(const ElapsedDaysFromStart &copy)
    : TimeReal(copy)
{
}


//------------------------------------------------------------------------------
// const ElapsedDaysFromStart& operator=(const ElapsedDaysFromStart &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ElapsedDaysFromStart& ElapsedDaysFromStart::operator=(const ElapsedDaysFromStart &right)
{
   if (this != &right)
   {
      TimeReal::operator=(right);
   }
   return *this;
}


//------------------------------------------------------------------------------
// ~ElapsedDaysFromStart()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ElapsedDaysFromStart::~ElapsedDaysFromStart()
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
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool ElapsedDaysFromStart::Evaluate()
{
   mRealValue = GetElapsedTimeReal(DAYS);
   
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//--------------------------------------
// Inherited methods from GmatBase
//--------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* ElapsedDaysFromStart::Clone(void) const
{
   return new ElapsedDaysFromStart(*this);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElapsedDaysFromStart::GetParameterType(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedDaysFromStartParamCount)
      return PARAMETER_TYPE[id - ParameterParamCount];
   else
      return TimeReal::GetParameterType(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedDaysFromStart::GetParameterTypeString(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedDaysFromStartParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - ParameterParamCount)];
   else
      return TimeReal::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedDaysFromStart::GetParameterText(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedDaysFromStartParamCount)
      return PARAMETER_TEXT[id - ParameterParamCount];
   else
      return TimeReal::GetParameterText(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> parameter index.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool ElapsedDaysFromStart::IsParameterReadOnly(const Integer id) const
{
   if (id == INITIAL_EPOCH)
      return true;
   
   return TimeReal::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ElapsedDaysFromStart::GetParameterID(const std::string &str) const
{
   for (int i = ParameterParamCount; i < ElapsedDaysFromStartParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return TimeReal::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDaysFromStart::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case INITIAL_EPOCH:
       return GetInitialEpoch();
   default:
       return TimeReal::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDaysFromStart::GetRealParameter(const std::string &label) const
{
   if (label == "InitialEpoch")
      return GetInitialEpoch();
   else
      return TimeReal::GetRealParameter(label);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDaysFromStart::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case INITIAL_EPOCH:
      SetInitialEpoch(value);
      return value;
   default:
      return TimeReal::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDaysFromStart::SetRealParameter(const std::string &label, const Real value)
{
   if (label == "InitialEpoch")
   {
      SetInitialEpoch(value);
      return value;
   }
   else
   {
      return TimeReal::SetRealParameter(label, value);
   }
}


//==============================================================================
//                              ElapsedSecs
//==============================================================================

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedSecs::PARAMETER_TEXT[ElapsedSecsParamCount - ParameterParamCount] =
{
   "InitialEpoch"
};

const Gmat::ParameterType
ElapsedSecs::PARAMETER_TYPE[ElapsedSecsParamCount - ParameterParamCount] =
{
   Gmat::REAL_TYPE
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedSecs(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ElapsedSecs::ElapsedSecs(const std::string &name, GmatBase *obj)
   : TimeReal(name, "ElapsedSecs", obj, "Elapsed Seconds", "s", false,
         Gmat::SPACECRAFT)
{
   // GmatBase data
   parameterCount = ElapsedSecsParamCount;
}


//------------------------------------------------------------------------------
// ElapsedSecs(const ElapsedSecs &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ElapsedSecs::ElapsedSecs(const ElapsedSecs &copy)
   : TimeReal(copy)
{
}


//------------------------------------------------------------------------------
// const ElapsedSecs& operator=(const ElapsedSecs &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ElapsedSecs& ElapsedSecs::operator=(const ElapsedSecs &right)
{
   if (this != &right)
   {
      TimeReal::operator=(right);
   }
   return *this;
}


//------------------------------------------------------------------------------
// ~ElapsedSecs()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ElapsedSecs::~ElapsedSecs()
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
bool ElapsedSecs::Evaluate()
{
   mRealValue = GetElapsedTimeReal(SECS);
   
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//--------------------------------------
// Inherited methods from GmatBase
//--------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* ElapsedSecs::Clone(void) const
{
   return new ElapsedSecs(*this);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElapsedSecs::GetParameterType(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedSecsParamCount)
      return PARAMETER_TYPE[id - ParameterParamCount];
   else
      return TimeReal::GetParameterType(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedSecs::GetParameterTypeString(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedSecsParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - ParameterParamCount)];
   else
      return TimeReal::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedSecs::GetParameterText(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedSecsParamCount)
      return PARAMETER_TEXT[id - ParameterParamCount];
   else
      return TimeReal::GetParameterText(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> parameter index.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool ElapsedSecs::IsParameterReadOnly(const Integer id) const
{
   if (id == INITIAL_EPOCH)
      return true;
   
   return TimeReal::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ElapsedSecs::GetParameterID(const std::string &str) const
{
   for (int i = ParameterParamCount; i < ElapsedSecsParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return TimeReal::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case INITIAL_EPOCH:
      return GetInitialEpoch();
   default:
      return TimeReal::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::GetRealParameter(const std::string &label) const
{
   if (label == "InitialEpoch")
      return GetInitialEpoch();
   else
      return TimeReal::GetRealParameter(label);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case INITIAL_EPOCH:
      SetInitialEpoch(value);
      return value;
   default:
      return TimeReal::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::SetRealParameter(const std::string &label, const Real value)
{
   if (label == "InitialEpoch")
   {
      SetInitialEpoch(value);
      return value;
   }
   else
   {
      return TimeReal::SetRealParameter(label, value);
   }
}


//==============================================================================
//                              ElapsedSecsFromStart
//==============================================================================

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedSecsFromStart::PARAMETER_TEXT[ElapsedSecsFromStartParamCount - ParameterParamCount] =
{
   "InitialEpoch"
};

const Gmat::ParameterType
ElapsedSecsFromStart::PARAMETER_TYPE[ElapsedSecsFromStartParamCount - ParameterParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedSecsFromStart(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ElapsedSecsFromStart::ElapsedSecsFromStart(const std::string &name, GmatBase *obj)
   : TimeReal(name, "ElapsedSecsFromStart", obj, "Elapsed Seconds", "s", false,
         Gmat::SPACECRAFT)
{
   // GmatBase data
   parameterCount = ElapsedSecsFromStartParamCount;
}


//------------------------------------------------------------------------------
// ElapsedSecsFromStart(const ElapsedSecsFromStart &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ElapsedSecsFromStart::ElapsedSecsFromStart(const ElapsedSecsFromStart &copy)
   : TimeReal(copy)
{
}


//------------------------------------------------------------------------------
// const ElapsedSecsFromStart& operator=(const ElapsedSecsFromStart &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ElapsedSecsFromStart& ElapsedSecsFromStart::operator=(const ElapsedSecsFromStart &right)
{
   if (this != &right)
   {
      TimeReal::operator=(right);
   }
   return *this;
}


//------------------------------------------------------------------------------
// ~ElapsedSecsFromStart()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ElapsedSecsFromStart::~ElapsedSecsFromStart()
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
bool ElapsedSecsFromStart::Evaluate()
{
   mRealValue = GetElapsedTimeReal(SECS);
   
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//--------------------------------------
// Inherited methods from GmatBase
//--------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* ElapsedSecsFromStart::Clone(void) const
{
   return new ElapsedSecsFromStart(*this);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElapsedSecsFromStart::GetParameterType(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedSecsFromStartParamCount)
      return PARAMETER_TYPE[id - ParameterParamCount];
   else
      return TimeReal::GetParameterType(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedSecsFromStart::GetParameterTypeString(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedSecsFromStartParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - ParameterParamCount)];
   else
      return TimeReal::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedSecsFromStart::GetParameterText(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedSecsFromStartParamCount)
      return PARAMETER_TEXT[id - ParameterParamCount];
   else
      return TimeReal::GetParameterText(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> parameter index.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool ElapsedSecsFromStart::IsParameterReadOnly(const Integer id) const
{
   if (id == INITIAL_EPOCH)
      return true;
   
   return TimeReal::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ElapsedSecsFromStart::GetParameterID(const std::string &str) const
{
   for (int i = ParameterParamCount; i < ElapsedSecsFromStartParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return TimeReal::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecsFromStart::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case INITIAL_EPOCH:
      return GetInitialEpoch();
   default:
      return TimeReal::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecsFromStart::GetRealParameter(const std::string &label) const
{
   if (label == "InitialEpoch")
      return GetInitialEpoch();
   else
      return TimeReal::GetRealParameter(label);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecsFromStart::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case INITIAL_EPOCH:
      SetInitialEpoch(value);
      return value;
   default:
      return TimeReal::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecsFromStart::SetRealParameter(const std::string &label, const Real value)
{
   if (label == "InitialEpoch")
   {
      SetInitialEpoch(value);
      return value;
   }
   else
   {
      return TimeReal::SetRealParameter(label, value);
   }
}



