//$Id$
//------------------------------------------------------------------------------
//                                  MeanOfDateAxes
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
// Created: 2005/05/03
//
/**
 * Implementation of the MeanOfDateAxes class.  This is the base class for those
 * DynamicAxes classes that implement Mean Of Date axis systems.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "MeanOfDateAxes.hpp"
#include "DynamicAxes.hpp"

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MeanOfDateAxes(const std::string &itsType,
//                 const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base MeanOfDateAxes structures used in derived classes
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for MeanOfDateAxes.  Derived 
 *       classes must pass in the itsType and (optionally) itsName parameters.
 */
//---------------------------------------------------------------------------
MeanOfDateAxes::MeanOfDateAxes(const std::string &itsType,
                               const std::string &itsName) :
DynamicAxes(itsType,itsName)
{
   objectTypeNames.push_back("MeanOfDateAxes");
   parameterCount = MeanOfDateAxesParamCount;
   isInertial = true;
}

//---------------------------------------------------------------------------
//  MeanOfDateAxes(const MeanOfDateAxes &mod);
//---------------------------------------------------------------------------
/**
 * Constructs base MeanOfDateAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param mod  MeanOfDateAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
MeanOfDateAxes::MeanOfDateAxes(const MeanOfDateAxes &mod) :
DynamicAxes(mod)
{
}

//---------------------------------------------------------------------------
//  MeanOfDateAxes& operator=(const MeanOfDateAxes &mod)
//---------------------------------------------------------------------------
/**
 * Assignment operator for MeanOfDateAxes structures.
 *
 * @param mod The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const MeanOfDateAxes& MeanOfDateAxes::operator=(const MeanOfDateAxes &mod)
{
   if (&mod == this)
      return *this;
   DynamicAxes::operator=(mod);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~MeanOfDateAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
MeanOfDateAxes::~MeanOfDateAxes()
{
}


//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this MeanOfDateAxes.
 *
 * @return success flag
 *
 */
//---------------------------------------------------------------------------
bool MeanOfDateAxes::Initialize()
{
   return DynamicAxes::Initialize();
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
// none
