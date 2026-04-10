//$Id$
//------------------------------------------------------------------------------
//                              FMAcceleration
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/29
//
/**
 * Implements the Acceleration Parameter classes.
 */
//------------------------------------------------------------------------------

#include "FMAcceleration.hpp"

//==============================================================================
//                              FMAcceleration
//==============================================================================
/**
 * Implements FMAcceleration parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FMAcceleration(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
FMAcceleration::FMAcceleration(const std::string &name, GmatBase *obj)
   : OdeReal(name, "Acceleration", obj, "Acceleration", "Km/s^2",
             Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = ""; //"DefaultFM";
//   SetRefObjectName(Gmat::ODE_MODEL, "DefaultFM"); //loj: 4/7/05 Added
}


//------------------------------------------------------------------------------
// FMAcceleration(const FMAcceleration &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
FMAcceleration::FMAcceleration(const FMAcceleration &copy)
   : OdeReal(copy)
{
}


//------------------------------------------------------------------------------
// const FMAcceleration& operator=(const FMAcceleration &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const FMAcceleration&
FMAcceleration::operator=(const FMAcceleration &right)
{
   if (this != &right)
   {
      OdeReal::operator=(right);
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~FMAcceleration()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
FMAcceleration::~FMAcceleration()
{
   //MessageInterface::ShowMessage("==> FMAcceleration::~FMAcceleration()\n");
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
bool FMAcceleration::Evaluate()
{
   mRealValue = OdeData::GetOdeReal("FMAcceleration");
   
   if (mRealValue == OdeData::ODE_REAL_UNDEFINED)
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
GmatBase* FMAcceleration::Clone() const
{
   return new FMAcceleration(*this);
}




//==============================================================================
//                              FMAccelerationX
//==============================================================================
/**
 * Implements FMAccelerationX parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FMAccelerationX(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
FMAccelerationX::FMAccelerationX(const std::string &name, GmatBase *obj)
   : OdeReal(name, "AccelerationX", obj, "AccelerationX", "Km/s^2",
             Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}


//------------------------------------------------------------------------------
// FMAccelerationX(const FMAccelerationX &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
FMAccelerationX::FMAccelerationX(const FMAccelerationX &copy)
   : OdeReal(copy)
{
}


//------------------------------------------------------------------------------
// const FMAccelerationX& operator=(const FMAccelerationX &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param right the parameter to make copy of
 */
//------------------------------------------------------------------------------
const FMAccelerationX&
FMAccelerationX::operator=(const FMAccelerationX &right)
{
   if (this != &right)
   {
      OdeReal::operator=(right);
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~FMAccelerationX()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
FMAccelerationX::~FMAccelerationX()
{
   //MessageInterface::ShowMessage("==> FMAccelerationX::~FMAccelerationX()\n");
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
bool FMAccelerationX::Evaluate()
{
   mRealValue = OdeData::GetOdeReal("FMAccelerationX");

   if (mRealValue == OdeData::ODE_REAL_UNDEFINED)
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
GmatBase* FMAccelerationX::Clone() const
{
   return new FMAccelerationX(*this);
}




//==============================================================================
//                              FMAccelerationY
//==============================================================================
/**
 * Implements FMAccelerationY parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FMAccelerationY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
FMAccelerationY::FMAccelerationY(const std::string &name, GmatBase *obj)
   : OdeReal(name, "AccelerationY", obj, "AccelerationY", "Km/s^2",
             Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = ""; //"DefaultFM";
//   SetRefObjectName(Gmat::ODE_MODEL, "DefaultFM"); //loj: 4/7/05 Added
}


//------------------------------------------------------------------------------
// FMAccelerationX(const FMAccelerationX &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
FMAccelerationY::FMAccelerationY(const FMAccelerationY &copy)
   : OdeReal(copy)
{
}


//------------------------------------------------------------------------------
// const FMAccelerationY& operator=(const FMAccelerationY &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const FMAccelerationY&
FMAccelerationY::operator=(const FMAccelerationY &right)
{
   if (this != &right)
   {
      OdeReal::operator=(right);
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~FMAccelerationY()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
FMAccelerationY::~FMAccelerationY()
{
   //MessageInterface::ShowMessage("==> FMAccelerationY::~FMAccelerationY()\n");
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
bool FMAccelerationY::Evaluate()
{
   mRealValue = OdeData::GetOdeReal("FMAccelerationY");

   if (mRealValue == OdeData::ODE_REAL_UNDEFINED)
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
GmatBase* FMAccelerationY::Clone() const
{
   return new FMAccelerationY(*this);
}




//==============================================================================
//                              FMAccelerationZ
//==============================================================================
/**
 * Implements FMAccelerationZ parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FMAccelerationZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
FMAccelerationZ::FMAccelerationZ(const std::string &name, GmatBase *obj)
   : OdeReal(name, "AccelerationZ", obj, "AccelerationZ", "Km/s^2",
             Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = ""; //"DefaultFM";
//   SetRefObjectName(Gmat::ODE_MODEL, "DefaultFM"); //loj: 4/7/05 Added
}


//------------------------------------------------------------------------------
// FMAccelerationZ(const FMAccelerationZ &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
FMAccelerationZ::FMAccelerationZ(const FMAccelerationZ &copy)
   : OdeReal(copy)
{
}


//------------------------------------------------------------------------------
// const FMAccelerationZ& operator=(const FMAccelerationZ &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const FMAccelerationZ&
FMAccelerationZ::operator=(const FMAccelerationZ &right)
{
   if (this != &right)
   {
      OdeReal::operator=(right);
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~FMAccelerationZ()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
FMAccelerationZ::~FMAccelerationZ()
{
   //MessageInterface::ShowMessage("==> FMAccelerationZ::~FMAccelerationZ()\n");
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
bool FMAccelerationZ::Evaluate()
{
   mRealValue = OdeData::GetOdeReal("FMAccelerationZ");

   if (mRealValue == OdeData::ODE_REAL_UNDEFINED)
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
GmatBase* FMAccelerationZ::Clone() const
{
   return new FMAccelerationZ(*this);
}
