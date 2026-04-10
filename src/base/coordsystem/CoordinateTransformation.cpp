//$Id$
//------------------------------------------------------------------------------
//                           CoordinateTransformation
//------------------------------------------------------------------------------
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2019.04.10
//
/**
* Original Python prototype:
* Author: N. Hatten
*
* Defines the CoordinateTransformation class.
*/
//------------------------------------------------------------------------------
#include "CoordinateTransformation.hpp"
#include "CoordinateConverter.hpp"

//------------------------------------------------------------------------------
// Rvector6 TransformState(const A1Mjd &epoch, AxisSystem *oldFrame,
//    const Rvector6 &stateWrtOldFrame, AxisSystem *newFrame)
//------------------------------------------------------------------------------
/**
* Given position and velocity state w.r.t. the frame oldFrame, calculate
* position and velocity state w.r.t. the frame newFrame
*
* @param epoch The epoch at which to perform the rotation.
* @param oldFrame Frame with respect to which the state of the spacecraft is
*        known
* @param stateWrtOldFrame 6-element list containing position (km) and
*        velocity (km/s) vectors w.r.t. oldFrame at time epoch
* @param newFrame Frame with respect to which the state of the spacecraft is
*        desired
*
* @return The spacecraft state in the new reference frame
*/
//------------------------------------------------------------------------------
Rvector6 CoordinateTransformation::TransformState(const A1Mjd &epoch,
   AxisSystem *oldFrame, const Rvector6 &stateWrtOldFrame, AxisSystem *newFrame)
{
   Rvector6 stateWrtInBase;
   oldFrame->RotateToBaseSystem(epoch, stateWrtOldFrame, stateWrtInBase);
   std::string baseIn = oldFrame->GetBaseSystem();
   std::string baseOut = newFrame->GetBaseSystem();
   Rvector6 stateWrtOutBase;

   // Rotate to base coordinate system of new frame if needed
   if (baseIn != baseOut)
   {
      CoordinateConverter cc;
      Rmatrix33 rotMat = cc.GetRotationMatrixFromICRFToFK5(epoch);
      if (baseIn == "ICRF" && baseOut == "FK5")
      {
         // Position elements
         stateWrtOutBase(0) = rotMat(0, 0) * stateWrtInBase(0) +
            rotMat(0, 1) * stateWrtInBase(1) + rotMat(0, 2) * stateWrtInBase(2);
         stateWrtOutBase(1) = rotMat(1, 0) * stateWrtInBase(0) +
            rotMat(1, 1) * stateWrtInBase(1) + rotMat(1, 2) * stateWrtInBase(2);
         stateWrtOutBase(2) = rotMat(2, 0) * stateWrtInBase(0) +
            rotMat(2, 1) * stateWrtInBase(1) + rotMat(2, 2) * stateWrtInBase(2);

         // Velocity elements
         stateWrtOutBase(3) = rotMat(0, 0) * stateWrtInBase(3) +
            rotMat(0, 1) * stateWrtInBase(4) + rotMat(0, 2) * stateWrtInBase(5);
         stateWrtOutBase(4) = rotMat(1, 0) * stateWrtInBase(3) +
            rotMat(1, 1) * stateWrtInBase(4) + rotMat(1, 2) * stateWrtInBase(5);
         stateWrtOutBase(5) = rotMat(2, 0) * stateWrtInBase(3) +
            rotMat(2, 1) * stateWrtInBase(4) + rotMat(2, 2) * stateWrtInBase(5);
      }
   }
   else
      stateWrtOutBase = stateWrtInBase;

   Rvector6 stateWrtNewFrame;
   newFrame->RotateFromBaseSystem(epoch, stateWrtOutBase, stateWrtNewFrame);

   return stateWrtNewFrame;
}
