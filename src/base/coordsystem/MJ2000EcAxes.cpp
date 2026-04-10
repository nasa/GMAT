//$Id$
//------------------------------------------------------------------------------
//                                  MJ2000EcAxes
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
// Created: 2005/01/06
//
/**
 * Implementation of the MJ2000EcAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "MJ2000EcAxes.hpp"
#include "InertialAxes.hpp"

//---------------------------------
// static data
//---------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MJ2000EcAxes(const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base MJ2000EcAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
MJ2000EcAxes::MJ2000EcAxes(const std::string &itsName) :
InertialAxes("MJ2000Ec",itsName)
{
   objectTypeNames.push_back("MJ2000EcAxes");
   parameterCount = MJ2000EcAxesParamCount;
}

//---------------------------------------------------------------------------
//  MJ2000EcAxes(const MJ2000EcAxes &MJ2000);
//---------------------------------------------------------------------------
/**
 * Constructs base MJ2000EcAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param MJ2000  MJ2000EcAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
MJ2000EcAxes::MJ2000EcAxes(const MJ2000EcAxes &MJ2000) :
InertialAxes(MJ2000)
{
}

//---------------------------------------------------------------------------
//  MJ2000EcAxes& operator=(const MJ2000EcAxes &MJ2000)
//---------------------------------------------------------------------------
/**
 * Assignment operator for MJ2000EcAxes structures.
 *
 * @param MJ2000 The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const MJ2000EcAxes& MJ2000EcAxes::operator=(const MJ2000EcAxes &MJ2000)
{
   if (&MJ2000 == this)
      return *this;
   InertialAxes::operator=(MJ2000);   
   return *this;
}

//---------------------------------------------------------------------------
//  ~MJ2000EcAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
MJ2000EcAxes::~MJ2000EcAxes()
{
}

//---------------------------------------------------------------------------
//  bool MJ2000EcAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this MJ2000EcAxes.
 *
 * @return success flag
 */
//---------------------------------------------------------------------------
bool MJ2000EcAxes::Initialize()
{
   InertialAxes::Initialize();
   // initialize the rotation matrix 
   rotMatrix(0,0) = 1.0;
   rotMatrix(0,1) = 0.0;
   rotMatrix(0,2) = 0.0;
   rotMatrix(1,0) = 0.0;
   rotMatrix(1,1) = 0.917482062076895741;
   rotMatrix(1,2) = -0.397777155914121383;
   rotMatrix(2,0) = 0.0;
   rotMatrix(2,1) = 0.397777155914121383;
   rotMatrix(2,2) = 0.917482062076895741;

   // rotDotMatrix is still the default zero matrix
   
   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MJ2000EcAxes.
 *
 * @return clone of the MJ2000EcAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MJ2000EcAxes::Clone() const
{
   return (new MJ2000EcAxes(*this));
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//---------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000EqAxes system.
 *
 * @param atEpoch          epoch at which to compute the roration matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 */
//---------------------------------------------------------------------------
void MJ2000EcAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                           bool forceComputation)
{
   // already computed in Initialize;
}
