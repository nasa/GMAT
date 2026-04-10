//$Id$
//------------------------------------------------------------------------------
//                                  MJ2000EqAxes
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
// Created: 2005/01/05
//
/**
 * Implementation of the MJ2000EqAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "MJ2000EqAxes.hpp"
#include "InertialAxes.hpp"

//#define DEBUG_CONSTRUCTION

#ifdef DEBUG_CONSTRUCTION
   #include "MessageInterface.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MJ2000EqAxes(const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base MJ2000EqAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
MJ2000EqAxes::MJ2000EqAxes(const std::string &itsName) :
InertialAxes("MJ2000Eq",itsName)
{
   objectTypeNames.push_back("MJ2000EqAxes");
   parameterCount = MJ2000EqAxesParamCount;
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Now constructing MJ2000Eq with name %s\n",
         itsName.c_str());
   #endif
}

//---------------------------------------------------------------------------
//  MJ2000EqAxes(const MJ2000EqAxes &mj2000);
//---------------------------------------------------------------------------
/**
 * Constructs base MJ2000EqAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param mj2000  MJ2000EqAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
MJ2000EqAxes::MJ2000EqAxes(const MJ2000EqAxes &mj2000) :
InertialAxes(mj2000)
{
}

//---------------------------------------------------------------------------
//  MJ2000EqAxes& operator=(const MJ2000EqAxes &mj2000)
//---------------------------------------------------------------------------
/**
 * Assignment operator for MJ2000EqAxes structures.
 *
 * @param mj2000 The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const MJ2000EqAxes& MJ2000EqAxes::operator=(const MJ2000EqAxes &mj2000)
{
   if (&mj2000 == this)
      return *this;
   InertialAxes::operator=(mj2000);   
   return *this;
}

//---------------------------------------------------------------------------
//  ~MJ2000EqAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
MJ2000EqAxes::~MJ2000EqAxes()
{
}

//---------------------------------------------------------------------------
//  bool MJ2000EqAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this MJ2000EqAxes.
 *
 * @return success flag
 */
//---------------------------------------------------------------------------
bool MJ2000EqAxes::Initialize()
{
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Now entering MJ2000Eq INIT with name %s\n",
         instanceName.c_str());
   #endif
   InertialAxes::Initialize();

   // initialize the rotation matrix to be the identity matrix
   rotMatrix(0,0) = 1.0;
   rotMatrix(1,1) = 1.0;
   rotMatrix(2,2) = 1.0;

   // rotDotMatrix is still the default zero matrix
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Now LEAVING MJ2000Eq INIT with name %s\n",
         instanceName.c_str());
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MJ2000EqAxes.
 *
 * @return clone of the MJ2000EqAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MJ2000EqAxes::Clone() const
{
   return (new MJ2000EqAxes(*this));
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
void MJ2000EqAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                           bool forceComputation)
{
   // already computed in Initialize;
}
