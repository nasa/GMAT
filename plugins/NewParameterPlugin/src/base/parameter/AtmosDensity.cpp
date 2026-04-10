//$Id$
//------------------------------------------------------------------------------
//                              AtmosDensity
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/12/10
// Adapted to use ForceModel: DJC, 5/24/2013
//
/**
 * Implements the Atmospheric Density Parameter class.
 */
//------------------------------------------------------------------------------

#include "AtmosDensity.hpp"

//==============================================================================
//                              AtmosDensity
//==============================================================================
/**
 * Implements AtmosDensity parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// AtmosDensity(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
AtmosDensity::AtmosDensity(const std::string &name, GmatBase *obj)
   : EnvReal(name, "AtmosDensity", obj, "Atmospheric Density", "Kg/m^3",
             Gmat::SPACECRAFT, GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth"); //loj: 4/7/05 Added
}


//------------------------------------------------------------------------------
// AtmosDensity(const AtmosDensity &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
AtmosDensity::AtmosDensity(const AtmosDensity &copy)
   : EnvReal(copy)
{
}


//------------------------------------------------------------------------------
// const AtmosDensity& operator=(const AtmosDensity &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const AtmosDensity&
AtmosDensity::operator=(const AtmosDensity &right)
{
   if (this != &right)
      EnvReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~AtmosDensity()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
AtmosDensity::~AtmosDensity()
{
   //MessageInterface::ShowMessage("==> AtmosDensity::~AtmosDensity()\n");
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
bool AtmosDensity::Evaluate()
{
   mRealValue = EnvData::GetEnvReal("AtmosDensity");    
   
   if (mRealValue == EnvData::ENV_REAL_UNDEFINED)
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
GmatBase* AtmosDensity::Clone(void) const
{
   return new AtmosDensity(*this);
}
