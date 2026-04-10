//$Id$
//------------------------------------------------------------------------------
//                                  TrueOfDateAxes
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
 * Implementation of the TrueOfDateAxes class.  This is the base class for those
 * DynamicAxes classes that implement True Of Date axis systems.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "TrueOfDateAxes.hpp"
#include "DynamicAxes.hpp"

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  TrueOfDateAxes(const std::string &itsType,
//                 const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base TrueOfDateAxes structures used in derived classes
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for TrueOfDateAxes.  Derived 
 *       classes must pass in the itsType and (optionally) itsName parameters.
 */
//---------------------------------------------------------------------------
TrueOfDateAxes::TrueOfDateAxes(const std::string &itsType,
                               const std::string &itsName) :
DynamicAxes(itsType,itsName)
{
   objectTypeNames.push_back("TrueOfDateAxes");
   parameterCount = TrueOfDateAxesParamCount;
   isInertial = true;
}

//---------------------------------------------------------------------------
//  TrueOfDateAxes(const TrueOfDateAxes &tod);
//---------------------------------------------------------------------------
/**
 * Constructs base TrueOfDateAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param tod  TrueOfDateAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
TrueOfDateAxes::TrueOfDateAxes(const TrueOfDateAxes &tod) :
DynamicAxes(tod)
{
}

//---------------------------------------------------------------------------
//  TrueOfDateAxes& operator=(const TrueOfDateAxes &tod)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TrueOfDateAxes structures.
 *
 * @param tod The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const TrueOfDateAxes& TrueOfDateAxes::operator=(const TrueOfDateAxes &tod)
{
   if (&tod == this)
      return *this;
   DynamicAxes::operator=(tod);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~TrueOfDateAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
TrueOfDateAxes::~TrueOfDateAxes()
{
}


//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this TrueOfDateAxes.
 *
 * @return success flag
 */
//---------------------------------------------------------------------------
bool TrueOfDateAxes::Initialize()
{
   DynamicAxes::Initialize();
   InitializeFK5();
   return true;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesEopFile(
//                                 const std::string &forBaseSystem) const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage TrueOfDateAxes::UsesEopFile(const std::string &forBaseSystem) const
{
   if (forBaseSystem == baseSystem)
      return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesItrfFile() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage TrueOfDateAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage TrueOfDateAxes::UsesNutationUpdateInterval() const
{
   if (originName == GmatSolarSystemDefaults::EARTH_NAME)
      return GmatCoordinate::REQUIRED;
   return DynamicAxes::UsesNutationUpdateInterval();
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
// none
