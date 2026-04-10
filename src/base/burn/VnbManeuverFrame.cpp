//$Id$
//------------------------------------------------------------------------------
//                              VnbManeuverFrame 
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
 * Defines the class used for maneuvers defined in VNB coordinates. 
 */
//------------------------------------------------------------------------------


#include "VnbManeuverFrame.hpp"
#include <math.h>


//------------------------------------------------------------------------------
//  VnbManeuverFrame()
//------------------------------------------------------------------------------
/**
 * Constructs the VNB maneuver frame (default constructor).
 */
//------------------------------------------------------------------------------
VnbManeuverFrame::VnbManeuverFrame()
{
}


//------------------------------------------------------------------------------
//  ~VnbManeuverFrame()
//------------------------------------------------------------------------------
/**
 * Destroys the VNB maneuver frame (destructor).
 */
//------------------------------------------------------------------------------
VnbManeuverFrame::~VnbManeuverFrame()
{
}


//------------------------------------------------------------------------------
//  VnbManeuverFrame(const InertialManeuverFrame& vmf)
//------------------------------------------------------------------------------
/**
 * Constructs the VNB maneuver frame based on another one (copy constructor).
 * 
 * @param <vmf> The frame that is being copied.
 */
//------------------------------------------------------------------------------
VnbManeuverFrame::VnbManeuverFrame(const VnbManeuverFrame& vmf) :
   ManeuverFrame       (vmf)
{
}


//------------------------------------------------------------------------------
//  VnbManeuverFrame& operator=(const InertialManeuverFrame& vmf)
//------------------------------------------------------------------------------
/**
 * Sets one VNB maneuver frame object to match another (assignment operator).
 * 
 * @param <vmf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
VnbManeuverFrame& VnbManeuverFrame::operator=(const VnbManeuverFrame& vmf)
{
   if (this == &vmf)
      return *this;
        
   ManeuverFrame::operator=(vmf);
   return *this;
}


//------------------------------------------------------------------------------
//  void CalculateBasis()
//------------------------------------------------------------------------------
/**
 * Calculates the orthonormal basis vectors used for the maneuver frame.
 * 
 * For the VNB frame, the basis vectors are calculated from the spacecraft 
 * position and velocity.  This method should be generalized so that the VNB
 * calculations can use any central body in the calculations.
 * 
 * @todo Generalize the basis vector calculations for the VNB maneuver frame.
 */
//------------------------------------------------------------------------------
void VnbManeuverFrame::CalculateBasis()
{
   if (!position || !velocity)
      throw BurnException("VNB frame undefined -- no position/velocity data.");
        
   // "X" direction is towards velocity
   Real temp[3], mag;
   memcpy(temp, velocity, 3*sizeof(Real));
   mag = sqrt(temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2]);
   if (mag == 0.0)
      throw BurnException("VNB frame undefined for velocity = 0.0.");
   basisMatrix[0][0] = temp[0] / mag;
   basisMatrix[1][0] = temp[1] / mag;
   basisMatrix[2][0] = temp[2] / mag;
    
   // Normal direction defined by r cross v
   temp[0] = position[1]*velocity[2] - position[2]*velocity[1];
   temp[1] = position[2]*velocity[0] - position[0]*velocity[2];
   temp[2] = position[0]*velocity[1] - position[1]*velocity[0];
   mag = sqrt(temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2]);
   if (mag == 0.0)
      throw BurnException("VNB frame undefined for |r x v| = 0.0.");
   basisMatrix[0][1] = temp[0] / mag;
   basisMatrix[1][1] = temp[1] / mag;
   basisMatrix[2][1] = temp[2] / mag;

   // Binormal direction defined by v cross n
   temp[0] = basisMatrix[1][0]*basisMatrix[2][1] -
             basisMatrix[2][0]*basisMatrix[1][1];
   temp[1] = basisMatrix[2][0]*basisMatrix[0][1] -
             basisMatrix[0][0]*basisMatrix[2][1];
   temp[2] = basisMatrix[0][0]*basisMatrix[1][1] -
             basisMatrix[1][0]*basisMatrix[0][1];
   mag = sqrt(temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2]);
   if (mag == 0.0)
      throw BurnException("VNB frame undefined for |v x n| = 0.0.");
   basisMatrix[0][2] = temp[0] / mag;
   basisMatrix[1][2] = temp[1] / mag;
   basisMatrix[2][2] = temp[2] / mag;
}


//------------------------------------------------------------------------------
//  std::string GetFrameLabel(Integer id)
//------------------------------------------------------------------------------
/**
 * Gets the label for the maneuver frame principal axes.
 * 
 * @param <id> Integer ID of the axis -- 1, 2, or 3.
 * 
 * @return The label for the corresponding reference vector.
 */
//------------------------------------------------------------------------------
std::string VnbManeuverFrame::GetFrameLabel(Integer id)
{
   switch (id)
   {
      case 1:
         return "V";
      case 2:
         return "N";
      case 3:
         return "B";
      default:
         ;
   }
   return "Undefined";
}
