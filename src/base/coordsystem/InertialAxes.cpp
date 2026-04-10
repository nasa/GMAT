//$Id$
//------------------------------------------------------------------------------
//                                  InertialAxes
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2004/12/28
//
/**
* Implementation of the InertialAxes class.  This is the base class for those
 * AxisSystem classes that implement inertial systems.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "InertialAxes.hpp"

//---------------------------------
// static data
//---------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  InertialAxes(const std::string &itsType,
//               const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base InertialAxes structures used in derived classes
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for InertialAxes.  Derived 
 *       classes must pass in the itsType and itsName parameters.
 */
//---------------------------------------------------------------------------
InertialAxes::InertialAxes(const std::string &itsType,
                           const std::string &itsName) :
AxisSystem(itsType,itsName)
{
   objectTypeNames.push_back("InertialAxes");
   parameterCount = InertialAxesParamCount;
   isInertial = true;
}

//---------------------------------------------------------------------------
//  InertialAxes(const InertialAxes &inertial);
//---------------------------------------------------------------------------
/**
 * Constructs base InertialAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param inertial  InertialAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
InertialAxes::InertialAxes(const InertialAxes &inertial) :
AxisSystem(inertial)
{
}

//---------------------------------------------------------------------------
//  InertialAxes& operator=(const InertialAxes &inertial)
//---------------------------------------------------------------------------
/**
 * Assignment operator for InertialAxes structures.
 *
 * @param inertial The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const InertialAxes& InertialAxes::operator=(const InertialAxes &inertial)
{
   if (&inertial == this)
      return *this;
   AxisSystem::operator=(inertial);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~InertialAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
InertialAxes::~InertialAxes()
{
}

//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for the InertialAxes classes.
 *
 */
//---------------------------------------------------------------------------
bool InertialAxes::Initialize()
{
   AxisSystem::Initialize();
   return true;
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
// none at this time
