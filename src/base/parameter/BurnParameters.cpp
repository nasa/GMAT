//$Id$
//------------------------------------------------------------------------------
//                            File: BurnParameters.cpp
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
// Created: 2005/05/27
//
/**
 * Implements Burn related Parameter classes.
 *   ImpBurnElements, TotalAcceleration, TotalThrust
 */
//------------------------------------------------------------------------------

#include "BurnParameters.hpp"
#include "ColorTypes.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BURN_PARAM 1


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS
//------------------------------------------------------------------------------
// ImpBurnElements(const std::string &type, const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <type> type of the parameter
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ImpBurnElements::ImpBurnElements(const std::string &type, const std::string &name,
                                 GmatBase *obj)
   : BurnReal(name, type, Gmat::IMPULSIVE_BURN, obj, "Impulsive Burn " + type,
              "Km/s", GmatParam::COORD_SYS, true)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
   
   // Write deprecated message for each once per GMAT session
   static bool writeDeprecatedMsg = true;

   if ((type == "V") || (type == "N") || (type == "B"))
   {
      if  (writeDeprecatedMsg)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"V\", \"N\", \"B\" are deprecated and will be "
             "removed from a future build; please use \"Element1\", \"Element2\", \"Element3\" "
             "instead.\n");
         writeDeprecatedMsg = false;
      }
   }

   if (type == "Element1" || type == "V")
      mElementId = ELEMENT1;
   else if (type == "Element2" || type == "N")
      mElementId = ELEMENT2;
   else if (type == "Element3" || type == "B")
      mElementId = ELEMENT3;
   else
      mElementId = -1;
}


//------------------------------------------------------------------------------
// ImpBurnElements(const ImpBurnElements &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ImpBurnElements::ImpBurnElements(const ImpBurnElements &copy)
   : BurnReal(copy)
{
   mElementId = copy.mElementId;
}


//------------------------------------------------------------------------------
// ImpBurnElements& operator=(const ImpBurnElements &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ImpBurnElements& ImpBurnElements::operator=(const ImpBurnElements &right)
{
   if (this != &right)
   {
      BurnReal::operator=(right);
      mElementId = right.mElementId;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~ImpBurnElements()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ImpBurnElements::~ImpBurnElements()
{
}


//-------------------------------------
// methods inherited from Parameter
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
bool ImpBurnElements::Evaluate()
{
   mRealValue = BurnData::GetReal(mElementId);

   #ifdef DEBUG_BURN_EVAL
   MessageInterface::ShowMessage
      ("ImpBurnElements::Evaluate() mRealValue=%f\n", mRealValue);
   #endif
   
   if (mRealValue == BurnData::BURN_REAL_UNDEFINED)
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
void ImpBurnElements::SetReal(Real val)
{
   BurnData::SetReal(mElementId, val);
   RealVar::SetReal(val);
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
GmatBase* ImpBurnElements::Clone(void) const
{
   return new ImpBurnElements(*this);
}


//==============================================================================
//                              TotalMassFlowRate
//==============================================================================
/**
 * Implements Finiteburn total mass flow rate
 *    TotalMassFlowRate
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TotalMassFlowRate(const std::string &type, const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <type> type of the parameter
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
TotalMassFlowRate::TotalMassFlowRate(const std::string &type, const std::string &name,
                                 GmatBase *obj)
   : BurnReal(name, type, Gmat::FINITE_BURN, obj, "Finite Burn " + type,
              "Km/s", GmatParam::NO_DEP, true)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}


//------------------------------------------------------------------------------
// TotalMassFlowRate(const TotalMassFlowRate &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TotalMassFlowRate::TotalMassFlowRate(const TotalMassFlowRate &copy)
   : BurnReal(copy)
{
}


//------------------------------------------------------------------------------
// TotalMassFlowRate& operator=(const TotalMassFlowRate &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TotalMassFlowRate& TotalMassFlowRate::operator=(const TotalMassFlowRate &right)
{
   if (this != &right)
      BurnReal::operator=(right);
   
   return *this;
}


//------------------------------------------------------------------------------
// ~TotalMassFlowRate()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
TotalMassFlowRate::~TotalMassFlowRate()
{
}

//-------------------------------------
// methods inherited from Parameter
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
bool TotalMassFlowRate::Evaluate()
{
   mRealValue = BurnData::GetReal(TOTAL_MASS_FLOW_RATE);
   
   #ifdef DEBUG_BURN_EVAL
   MessageInterface::ShowMessage
      ("TotalMassFlowRate::Evaluate() mRealValue=%f\n", mRealValue);
   #endif
   
   if (mRealValue == BurnData::BURN_REAL_UNDEFINED)
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
void TotalMassFlowRate::SetReal(Real val)
{
   BurnData::SetReal(TOTAL_MASS_FLOW_RATE, val);
   RealVar::SetReal(val);
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
GmatBase* TotalMassFlowRate::Clone(void) const
{
   return new TotalMassFlowRate(*this);
}


//==============================================================================
//                              TotalAcceleration
//==============================================================================
/**
 * Implements Finiteburn total thrust Parameters.
 *    TotalAcceleration1, TotalAcceleration2, TotalAcceleration3
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TotalAcceleration(const std::string &type, const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <type> type of the parameter
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
TotalAcceleration::TotalAcceleration(const std::string &type, const std::string &name,
                                     GmatBase *obj)
   : BurnReal(name, type, Gmat::FINITE_BURN, obj, "Finite Burn " + type,
              "Km/s", GmatParam::NO_DEP, true)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
   
   // Write deprecated message for each once per GMAT session
   static bool writeDeprecatedMsg = true;
   
   if (type == "TotalAcceleration1")
      mTotalAccelerationId = TOTAL_ACCEL1;
   else if (type == "TotalAcceleration2")
      mTotalAccelerationId = TOTAL_ACCEL2;
   else if (type == "TotalAcceleration3")
      mTotalAccelerationId = TOTAL_ACCEL3;
   else
      mTotalAccelerationId = -1;
}


//------------------------------------------------------------------------------
// TotalAcceleration(const TotalAcceleration &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TotalAcceleration::TotalAcceleration(const TotalAcceleration &copy)
   : BurnReal(copy)
{
   mTotalAccelerationId = copy.mTotalAccelerationId;
}


//------------------------------------------------------------------------------
// TotalAcceleration& operator=(const TotalAcceleration &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TotalAcceleration& TotalAcceleration::operator=(const TotalAcceleration &right)
{
   if (this != &right)
   {
      BurnReal::operator=(right);
      mTotalAccelerationId = right.mTotalAccelerationId;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~TotalAcceleration()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
TotalAcceleration::~TotalAcceleration()
{
}


//-------------------------------------
// methods inherited from Parameter
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
bool TotalAcceleration::Evaluate()
{
   mRealValue = BurnData::GetReal(mTotalAccelerationId);

   #ifdef DEBUG_BURN_EVAL
   MessageInterface::ShowMessage
      ("TotalAcceleration::Evaluate() mRealValue=%f\n", mRealValue);
   #endif
   
   if (mRealValue == BurnData::BURN_REAL_UNDEFINED)
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
void TotalAcceleration::SetReal(Real val)
{
   BurnData::SetReal(mTotalAccelerationId, val);
   RealVar::SetReal(val);
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
GmatBase* TotalAcceleration::Clone(void) const
{
   return new TotalAcceleration(*this);
}


//==============================================================================
//                              TotalThrust
//==============================================================================
/**
 * Implements Finiteburn total thrust Parameters.
 *    TotalThrust1, TotalThrust2, TotalThrust3
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TotalThrust(const std::string &type, const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <type> type of the parameter
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
TotalThrust::TotalThrust(const std::string &type, const std::string &name,
                         GmatBase *obj)
   : BurnReal(name, type, Gmat::FINITE_BURN, obj, "Finite Burn " + type,
              "Km/s", GmatParam::NO_DEP, true)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
   
   // Write deprecated message for each once per GMAT session
   static bool writeDeprecatedMsg = true;
   
   if (type == "TotalThrust1")
      mTotalThrustId = TOTAL_THRUST1;
   else if (type == "TotalThrust2")
      mTotalThrustId = TOTAL_THRUST2;
   else if (type == "TotalThrust3")
      mTotalThrustId = TOTAL_THRUST3;
   else
      mTotalThrustId = -1;
}


//------------------------------------------------------------------------------
// TotalThrust(const TotalThrust &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TotalThrust::TotalThrust(const TotalThrust &copy)
   : BurnReal(copy)
{
   mTotalThrustId = copy.mTotalThrustId;
}


//------------------------------------------------------------------------------
// TotalThrust& operator=(const TotalThrust &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TotalThrust& TotalThrust::operator=(const TotalThrust &right)
{
   if (this != &right)
   {
      BurnReal::operator=(right);
      mTotalThrustId = right.mTotalThrustId;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~TotalThrust()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
TotalThrust::~TotalThrust()
{
}


//-------------------------------------
// methods inherited from Parameter
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
bool TotalThrust::Evaluate()
{
   mRealValue = BurnData::GetReal(mTotalThrustId);

   #ifdef DEBUG_BURN_EVAL
   MessageInterface::ShowMessage
      ("TotalThrust::Evaluate() mRealValue=%f\n", mRealValue);
   #endif
   
   if (mRealValue == BurnData::BURN_REAL_UNDEFINED)
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
void TotalThrust::SetReal(Real val)
{
   BurnData::SetReal(mTotalThrustId, val);
   RealVar::SetReal(val);
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
GmatBase* TotalThrust::Clone(void) const
{
   return new TotalThrust(*this);
}


