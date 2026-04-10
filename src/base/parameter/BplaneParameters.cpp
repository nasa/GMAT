//$Id$
//------------------------------------------------------------------------------
//                             File: BplaneParameters.cpp
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
// Created: 2005/06/15
//
/**
 * Declares B-Plane related parameter classes.
 *   BdotT, BdotR
 */
//------------------------------------------------------------------------------
#include "BplaneParameters.hpp"
#include "Linear.hpp"           // for GmatRealUtil::ToString()
#include "MessageInterface.hpp"

//==============================================================================
//                              BdotT
//==============================================================================
/**
 * Implements Velocity at Apoapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BdotT(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
BdotT::BdotT(const std::string &name, GmatBase *obj)
   : BplaneReal(name, "BdotT", obj, "B dot T", "", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
}


//------------------------------------------------------------------------------
// BdotT(const BdotT &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BdotT::BdotT(const BdotT &copy)
   : BplaneReal(copy)
{
}


//------------------------------------------------------------------------------
// const BdotT& operator=(const BdotT &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const BdotT&
BdotT::operator=(const BdotT &right)
{
   if (this != &right)
      BplaneReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BdotT()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BdotT::~BdotT()
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
bool BdotT::Evaluate()
{
   mRealValue = BplaneData::GetBplaneReal(B_DOT_T);
   return true;
   
//    try
//    {
//       mRealValue = BplaneData::GetBplaneReal(B_DOT_T);
//    }
//    catch (BaseException &e)
//    {
//       mRealValue = REAL_PARAMETER_UNDEFINED;
//       MessageInterface::ShowMessage
//          (e.GetFullMessage() + ",\n     so setting BdotT parameter value to: " +
//           GmatRealUtil::ToString(REAL_PARAMETER_UNDEFINED) + "\n");
//    }
   
//    if (mRealValue == REAL_PARAMETER_UNDEFINED)
//       return false;
//    else
//       return true;
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
GmatBase* BdotT::Clone(void) const
{
   return new BdotT(*this);
}


//==============================================================================
//                              BdotR
//==============================================================================
/**
 * Implements Velocity at Periapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BdotR(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
BdotR::BdotR(const std::string &name, GmatBase *obj)
   : BplaneReal(name, "BdotR", obj, "B dot R", "", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
}


//------------------------------------------------------------------------------
// BdotR(const BdotR &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BdotR::BdotR(const BdotR &copy)
   : BplaneReal(copy)
{
}


//------------------------------------------------------------------------------
// const BdotR& operator=(const BdotR &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const BdotR&
BdotR::operator=(const BdotR &right)
{
   if (this != &right)
      BplaneReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BdotR()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BdotR::~BdotR()
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
bool BdotR::Evaluate()
{
   try
   {
      mRealValue = BplaneData::GetBplaneReal(B_DOT_R);
   }
   catch (BaseException &e)
   {
      mRealValue = REAL_PARAMETER_UNDEFINED;
      MessageInterface::ShowMessage
         (e.GetFullMessage() + ",\n     so settting BdotR parameter value to: " +
          GmatRealUtil::ToString(REAL_PARAMETER_UNDEFINED) + "\n");
   }
   
   if (mRealValue == REAL_PARAMETER_UNDEFINED)
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
GmatBase* BdotR::Clone(void) const
{
   return new BdotR(*this);
}


//==============================================================================
//                              BVectorMag
//==============================================================================
/**
 * Implements Velocity at Periapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BVectorMag(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
BVectorMag::BVectorMag(const std::string &name, GmatBase *obj)
   : BplaneReal(name, "BVectorMag", obj, "B Vector Magnitude", "", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
}


//------------------------------------------------------------------------------
// BVectorMag(const BVectorMag &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BVectorMag::BVectorMag(const BVectorMag &copy)
   : BplaneReal(copy)
{
}


//------------------------------------------------------------------------------
// const BVectorMag& operator=(const BVectorMag &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const BVectorMag&
BVectorMag::operator=(const BVectorMag &right)
{
   if (this != &right)
      BplaneReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BVectorMag()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BVectorMag::~BVectorMag()
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
bool BVectorMag::Evaluate()
{
   try
   {
      mRealValue = BplaneData::GetBplaneReal(B_VECTOR_MAG);
   }
   catch (BaseException &e)
   {
      mRealValue = REAL_PARAMETER_UNDEFINED;
      MessageInterface::ShowMessage
         (e.GetFullMessage() + ",\n     so settting BVectorMag parameter value to: " +
          GmatRealUtil::ToString(REAL_PARAMETER_UNDEFINED) + "\n");
   }
   
   if (mRealValue == REAL_PARAMETER_UNDEFINED)
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
GmatBase* BVectorMag::Clone(void) const
{
   return new BVectorMag(*this);
}


//==============================================================================
//                              BVectorAngle
//==============================================================================
/**
 * Implements Velocity at Periapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BVectorAngle(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
BVectorAngle::BVectorAngle(const std::string &name, GmatBase *obj)
   : BplaneReal(name, "BVectorAngle", obj, "B Vector Angle", "", GmatParam::COORD_SYS)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   SetRequiresCelestialBodyCSOrigin(true);
}


//------------------------------------------------------------------------------
// BVectorAngle(const BVectorAngle &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BVectorAngle::BVectorAngle(const BVectorAngle &copy)
   : BplaneReal(copy)
{
}


//------------------------------------------------------------------------------
// const BVectorAngle& operator=(const BVectorAngle &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const BVectorAngle&
BVectorAngle::operator=(const BVectorAngle &right)
{
   if (this != &right)
      BplaneReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BVectorAngle()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BVectorAngle::~BVectorAngle()
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
bool BVectorAngle::Evaluate()
{
   try
   {
      mRealValue = BplaneData::GetBplaneReal(B_VECTOR_ANGLE);
   }
   catch (BaseException &e)
   {
      mRealValue = REAL_PARAMETER_UNDEFINED;
      MessageInterface::ShowMessage
         (e.GetFullMessage() + ",\n     so settting BVectorAngle parameter value to: " +
          GmatRealUtil::ToString(REAL_PARAMETER_UNDEFINED) + "\n");
   }
   
   if (mRealValue == REAL_PARAMETER_UNDEFINED)
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
GmatBase* BVectorAngle::Clone(void) const
{
   return new BVectorAngle(*this);
}

