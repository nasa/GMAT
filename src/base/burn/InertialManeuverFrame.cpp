//$Id$
//------------------------------------------------------------------------------
//                           InertialManeuverFrame
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
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the class used for maneuvers defined inertially. 
 */
//------------------------------------------------------------------------------


#include "InertialManeuverFrame.hpp"


//------------------------------------------------------------------------------
// InertialManeuverFrame()
//------------------------------------------------------------------------------
/**
 * Constructs the inertial maneuver frame (default constructor).
 */
//------------------------------------------------------------------------------
InertialManeuverFrame::InertialManeuverFrame()
{
}


//------------------------------------------------------------------------------
// ~InertialManeuverFrame()
//------------------------------------------------------------------------------
/**
 * Destroys the inertial maneuver frame (destructor).
 */
//------------------------------------------------------------------------------
InertialManeuverFrame::~InertialManeuverFrame()
{
}


//------------------------------------------------------------------------------
//  InertialManeuverFrame(const InertialManeuverFrame& imf)
//------------------------------------------------------------------------------
/**
 * Constructs the inertial maneuver frame based on another one (copy 
 * constructor).
 * 
 * @param <imf> The frame that is being copied.
 */
//------------------------------------------------------------------------------
InertialManeuverFrame::InertialManeuverFrame(const InertialManeuverFrame& imf) :
   ManeuverFrame(imf)
{
}


//------------------------------------------------------------------------------
// InertialManeuverFrame& operator=(const InertialManeuverFrame& imf)
//------------------------------------------------------------------------------
/**
 * Sets one inertial maneuver frame object to match another (assignment operator).
 * 
 * @param <imf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
InertialManeuverFrame& InertialManeuverFrame::operator=(
      const InertialManeuverFrame& imf)
{
   if (this == &imf)
      return *this;
        
   ManeuverFrame::operator=(imf);
   
   return *this;
}

    
//------------------------------------------------------------------------------
// void CalculateBasis()
//------------------------------------------------------------------------------
/**
 * Calculates the orthonormal basis vectors used for the maneuver frame.
 * 
 * For the inertial frame, the basis vectors are parallel to the spacecraft 
 * reference frame vectors, so the basis matrix is the identity matrix.
 */
//------------------------------------------------------------------------------
void InertialManeuverFrame::CalculateBasis()
{
   basisMatrix[0][0] = 1.0;
   basisMatrix[1][1] = 1.0;
   basisMatrix[2][2] = 1.0;
   basisMatrix[0][1] = 0.0;
   basisMatrix[0][2] = 0.0;
   basisMatrix[1][0] = 0.0;
   basisMatrix[1][2] = 0.0;
   basisMatrix[2][0] = 0.0;
   basisMatrix[2][1] = 0.0;
}
