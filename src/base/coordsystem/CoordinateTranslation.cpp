//$Id$
//------------------------------------------------------------------------------
//                           CoordinateTranslation
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
* Defines the CoordinateTranslation class.
*/
//------------------------------------------------------------------------------
#include "CoordinateTranslation.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// Rvector6 TranslateOrigin(const A1Mjd &epoch, SpacePoint *oldOriginBody,
//    const Rvector6 &stateWrtOldOrigin, SpacePoint *newOriginBody,
//    AxisSystem *referenceFrame)
//------------------------------------------------------------------------------
/**
* Given position and velocity state w.r.t. the body oldOriginBody, calculate
* position and velocity state w.r.t. the body newOriginBody
*
* @param epoch The time at which to perform the translation
* @param oldOriginBody Body with respect to which the state of the spacecraft
*        is known
* @param stateWrtOldOrigin 6-element list containing position (km) and velocity
*        (km/s) vectors w.r.t. oldOriginBody in referenceFrame frame at time ET
* @param newOriginBody Body with respect to which the state of the spacecraft
*        is desired
* @param referenceFrame The reference frame of stateWrtOldOrigin and the
*        reference frame of the output state
*
* @return The spacecraft state w.r.t. the new origin in the frame referenceFrame
*/
//------------------------------------------------------------------------------
Rvector6 CoordinateTranslation::TranslateOrigin(const A1Mjd &epoch,
   SpacePoint *oldOriginBody, const Rvector6 &stateWrtOldOrigin,
   SpacePoint *newOriginBody, CoordinateSystem *referenceFrame)
{
   // Get difference in body states in FK5
   Rvector6 oldOriginWrtNewOriginFK5 = oldOriginBody->GetMJ2000State(epoch) -
      (newOriginBody->GetMJ2000State(epoch));

   // Convert the difference to the input state axis frame
   Rvector6 oldOriginWrtNewOrigin;
   AxisSystem *refAxisSystem = referenceFrame->GetAxisSystem();
   refAxisSystem->RotateFromBaseSystem(epoch, oldOriginWrtNewOriginFK5,
      oldOriginWrtNewOrigin);

   // Translate the state
   Rvector6 stateWrtNewOrigin = oldOriginWrtNewOrigin + stateWrtOldOrigin;

   return stateWrtNewOrigin;
}

//------------------------------------------------------------------------------
// Rvector6 TranslateOriginSmoothed(const A1Mjd &epoch,
//    SpacePoint *oldOriginBody, const Rvector6 &stateWrtOldOrigin,
//    SpacePoint *newOriginBody, CoordinateSystem *referenceFrame,
//    EphemSmoother *ephemerisSmoother)
//------------------------------------------------------------------------------
/**
* Given position and velocity state w.r.t. the body oldOriginBody, calculate
* position and velocity state w.r.t. the body newOriginBody.  This version uses
* an ephemSmoother object rather than direct spice calls.
*
* @param epoch The time at which to perform the translation
* @param oldOriginBody Body with respect to which the state of the spacecraft
*        is known
* @param stateWrtOldOrigin 6-element list containing position (km) and velocity
*        (km/s) vectors w.r.t. oldOriginBody in referenceFrame frame at time ET
* @param newOriginBody Body with respect to which the state of the spacecraft
*        is desired
* @param referenceFrame The reference frame of stateWrtOldOrigin and the
*        reference frame of the output state
* @param ephemSmoother An ephemSmoother object used to get state values
*
* @return The spacecraft state w.r.t. the new origin in the frame referenceFrame
*/
//------------------------------------------------------------------------------
Rvector6 CoordinateTranslation::TranslateOriginSmoothed(const A1Mjd &epoch,
   SpacePoint *oldOriginBody, const Rvector6 &stateWrtOldOrigin,
   SpacePoint *newOriginBody, CoordinateSystem *referenceFrame,
   EphemSmoother *ephemerisSmoother)
{
   // State of old origin wrt new origin
   Rvector oldOriginWrtNewOrigin, dOldOriginWrtNewOrigin,
      ddOldOriginWrtNewOrigin;
   ephemerisSmoother->GetState(newOriginBody, oldOriginBody, referenceFrame,
      epoch.GetReal(), oldOriginWrtNewOrigin, dOldOriginWrtNewOrigin,
      ddOldOriginWrtNewOrigin);

   Rvector stateWrtNewOrigin = oldOriginWrtNewOrigin + stateWrtOldOrigin;
   
   Rvector6 result;
   for (Integer i = 0; i < 6; ++i)
      result(i) = stateWrtNewOrigin(i);
   return result;
}

//------------------------------------------------------------------------------
// void TranslateOriginSmoothedWithDerivatives(const A1Mjd &epoch,
//    SpacePoint *oldOriginBody, const Rvector6 &stateWrtOldOrigin,
//    SpacePoint *newOriginBody, CoordinateSystem *referenceFrame,
//    EphemSmoother *ephemerisSmoother, Rvector6 &stateWrtNewOrigin,
//    Rmatrix66 &dOldOriginWrtNewOriginDState,
//    Rvector6 &dOldOriginWrtNewOriginDt)
//------------------------------------------------------------------------------
/**
* Given position and velocity state w.r.t. the body oldOriginBody, calculate
* position and velocity state w.r.t. the body newOriginBody.  This version uses
* an ephemSmoother object rather than direct spice calls.  Also returns first
* derivative of the transformation w.r.t. the spacecraft state and time.  Note
* that the derivative is just the derivative of the function f^{O_F/O_D} (z')
* in the math spec, meaning it does not differentiate z' itself.
*
* @param epoch The time at which to perform the translation
* @param oldOriginBody Body with respect to which the state of the spacecraft
*        is known
* @param stateWrtOldOrigin 6-element list containing position (km) and velocity
*        (km/s) vectors w.r.t. oldOriginBody in referenceFrame frame at time ET
* @param newOriginBody Body with respect to which the state of the spacecraft
*        is desired
* @param referenceFrame The reference frame of stateWrtOldOrigin and the
*        reference frame of the output state
* @param ephemSmoother An ephemSmoother object used to get state values
* @param stateWrtNewOrigin The spacecraft state w.r.t. the new origin in the
*         frame referenceFrame
* @param dOldOriginWrtNewOriginDState 6x6 array containing state derivatives
*        of oldOriginWrtNewOrigin. All elements are zero because the states
*        of the origins only depend on time
* @param dOldOriginWrtNewOriginDt 6-element array containing time derivatives
*        of oldOriginWrtNewOrigin
*/
//------------------------------------------------------------------------------
void CoordinateTranslation::TranslateOriginSmoothedWithDerivatives(
   const A1Mjd &epoch, SpacePoint *oldOriginBody,
   const Rvector6 &stateWrtOldOrigin, SpacePoint *newOriginBody,
   CoordinateSystem *referenceFrame, EphemSmoother *ephemerisSmoother,
   Rvector6 &stateWrtNewOrigin, Rmatrix &dOldOriginWrtNewOriginDState,
   Rmatrix &dOldOriginWrtNewOriginDt)
{
   // State of old origin wrt new origin
   Rvector oldOriginState, dOldOriginState, ddOldOriginState;
   ephemerisSmoother->GetState(newOriginBody, oldOriginBody, referenceFrame,
      epoch.GetReal(), oldOriginState, dOldOriginState,
      ddOldOriginState);

   Rvector oldOriginWrtNewOrigin = oldOriginState;
   for (Integer i = 0; i < 6; ++i)
   {
      stateWrtNewOrigin(i) = oldOriginWrtNewOrigin(i) + stateWrtOldOrigin(i);
      dOldOriginWrtNewOriginDt(i, 0) = dOldOriginState(i);
   }

   Rmatrix matrixOfZeros(6, 6);
   dOldOriginWrtNewOriginDState = matrixOfZeros;
}
