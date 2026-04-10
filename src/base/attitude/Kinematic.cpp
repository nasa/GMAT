//$Id$
//------------------------------------------------------------------------------
//                               Kinematic
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Wendy C. Shoan/GSFC
// Created: 2006.03.24
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Implementation for the Kinematic base class.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Attitude.hpp"
#include "AttitudeException.hpp"
#include "Kinematic.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none 

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Kinematic(const std::string &typeStr, const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Kinematic class (constructor).
 * The default value is the (0,0,0,1) quaternion.
 */
//------------------------------------------------------------------------------
Kinematic::Kinematic(const std::string &typeStr, const std::string &itsName) : 
   Attitude(typeStr, itsName)
{
   parameterCount = KinematicParamCount;
   objectTypeNames.push_back("Kinematic");
 }
 
 //------------------------------------------------------------------------------
//  Kinematic(const Kinematic &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Kinematic class as a copy of the
 * specified Kinematic class (copy constructor).
 *
 * @param <att> Kinematic object to copy.
 */
//------------------------------------------------------------------------------
Kinematic::Kinematic(const Kinematic& att) :
   Attitude(att)
{
}
 
//------------------------------------------------------------------------------
//  Kinematic& operator= (const Kinematic& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Kinematic class.
 *
 * @param att the Kinematic object whose data to assign to "this"
 *            Kinematic.
 *
 * @return "this" Kinematic with data of input Kinematic att.
 */
//------------------------------------------------------------------------------
Kinematic& Kinematic::operator=(const Kinematic& att)
{
   Attitude::operator=(att);
   return *this;
}

//------------------------------------------------------------------------------
//  ~Kinematic()
//------------------------------------------------------------------------------
/**
 * Destroys the Kinematic class (destructor).
 */
//------------------------------------------------------------------------------
Kinematic::~Kinematic()
{
   // nothing really to do here ... la la la la la
}

//------------------------------------------------------------------------------
//  protected methods
//------------------------------------------------------------------------------
// none 

//------------------------------------------------------------------------------
//  private methods
//------------------------------------------------------------------------------
// none 
