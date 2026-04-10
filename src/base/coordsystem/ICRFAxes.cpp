//$Id: $
//------------------------------------------------------------------------------
//                                  ICRFAxes
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Wendy C. Shoan/GSFC/GSSB
//         Tuan Dang Nguyen/GSFC
// Created: 2012.02.23
//
/**
 * Implementation of the ICRFAxes class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RealUtilities.hpp"
#include "ICRFAxes.hpp"
#include "InertialAxes.hpp"
#include "MessageInterface.hpp"

using namespace GmatMathUtil;      // for trig functions, etc.

//#define DEBUG_ICRFAXES_CONSTRUCTION
//#define DEBUG_ICRFAXES_CLONE
//#define DEBUG_ICRFAXES_INITIALIZE
//#define DEBUG_ICRFAXES_FIRST_CALL
//#define DEBUG_ICRFAXES_ROT_MATRIX
//#define DEBUG_ICRFAXES_RECOMPUTE
//#define DEBUG_ICRFAXES_EPOCHS

#ifdef DEBUG_ICRFAXES_FIRST_CALL
   static bool firstCallFired = false;
#endif

//---------------------------------
// static data
//---------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ICRFAxes(const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base ICRFAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
ICRFAxes::ICRFAxes(const std::string &itsName) :
   InertialAxes   ("ICRF",itsName)
{
   objectTypeNames.push_back("ICRFAxes");
   baseSystem     = "ICRF";
   parameterCount = ICRFAxesParamCount;
   isInertial = true;
   #ifdef DEBUG_ICRFAXES_CONSTRUCTION
      MessageInterface::ShowMessage("Now constructing ICRFAxes with name '%s'\n",
         itsName.c_str());
   #endif
}

//---------------------------------------------------------------------------
//  ICRFAxes(const ICRFAxes &icrf);
//---------------------------------------------------------------------------
/**
 * Constructs base ICRFAxes structures used in derived classes, by copying
 * the input instance (copy constructor).
 *
 * @param icrf  ICRFAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
ICRFAxes::ICRFAxes(const ICRFAxes &icrf) :
   InertialAxes(icrf)
{
#ifdef DEBUG_ICRFAXES_CONSTRUCTION
   MessageInterface::ShowMessage("Now copy-constructing ICRFAxes from object (%p) with name '%s'\n", &icrf,
     icrf.GetName().c_str());
#endif
}

//---------------------------------------------------------------------------
//  ICRFAxes& operator=(const ICRFAxes &icrf)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ICRFAxes structures.
 *
 * @param icrf The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ICRFAxes& ICRFAxes::operator=(const ICRFAxes &icrf)
{
   if (&icrf == this)
      return *this;

   InertialAxes::operator=(icrf);

   return *this;
}

//---------------------------------------------------------------------------
//  ~ICRFAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ICRFAxes::~ICRFAxes()
{
}

//---------------------------------------------------------------------------
//  bool ICRFAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this ICRFAxes.
 *
 * @result success flag
 */
//---------------------------------------------------------------------------
bool ICRFAxes::Initialize()
{
   #ifdef DEBUG_ICRFAXES_INITIALIZE
      MessageInterface::ShowMessage("Initialize ICRFAxes: with name '%s'\n",
         instanceName.c_str());
   #endif

   InertialAxes::Initialize();
   isInitialized = true;

   #ifdef DEBUG_ICRFAXES_INITIALIZE
   MessageInterface::ShowMessage("End initialize ICRFAxes: with name '%s'\n",
         instanceName.c_str());
   #endif

   return true;
}

//-----------------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem) const
//-----------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ICRFAxes::UsesEopFile(const std::string &forBaseSystem) const
{
   if (forBaseSystem == baseSystem)
      return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}


//----------------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesItrfFile() const
//----------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ICRFAxes::UsesItrfFile() const
{
   return GmatCoordinate::NOT_USED;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ICRFAxes.
 *
 * @return clone of the ICRFAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ICRFAxes::Clone() const
{
   #ifdef DEBUG_ICRFAXES_CONSTRUCTION
   MessageInterface::ShowMessage("Now clone ICRFAxes from object (%p) with name '%s'\n", this,
        this->GetName().c_str());
   #endif
   return (new ICRFAxes(*this));
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the ICRFAxes system
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 *
 */
//------------------------------------------------------------------------------
void ICRFAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                            bool forceComputation)
{
	rotMatrix.Set(1.0, 0.0, 0.0,
		          0.0, 1.0, 0.0,
			      0.0, 0.0, 1.0);

	rotDotMatrix.Set(0.0, 0.0, 0.0,
		             0.0, 0.0, 0.0,
			         0.0, 0.0, 0.0);
}



//------------------------------------------------------------------------------
//  Rmatrix33 GetRotationMatrix(const A1Mjd &atEpoch,
//                              bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the ICRFAxes system
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 *
 * @return rotation matrix
 */
//------------------------------------------------------------------------------
Rmatrix33 ICRFAxes::GetRotationMatrix(const A1Mjd &atEpoch, bool forceComputation)
{
	CalculateRotationMatrix(atEpoch, forceComputation);
	return rotMatrix;
}
