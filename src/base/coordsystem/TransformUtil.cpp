//$Id$
//------------------------------------------------------------------------------
//                               TransformUtil
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
* Defines the TransformUtil class.
*/
//------------------------------------------------------------------------------
#include "TransformUtil.hpp"
#include "StateConversionUtil.hpp"
#include "CoordinateTransformation.hpp"
#include "CoordinateTranslation.hpp"

//------------------------------------------------------------------------------
//Rvector6 TransformOrbitalState(const Rvector6 &stateIn,
//   const Real &timeIn, CoordinateSystem *coordSysIn,
//   const std::string &stateRepIn, CoordinateSystem *coordSysOut,
//   const std::string &stateRepOut, const std::string &ephemType,
//   EphemSmoother *ephemerisSmoother)
//------------------------------------------------------------------------------
/*
* Transform orbital state from one representation/frame/origin to another
*
* @param stateIn Input orbital state
* @param timeIn Input time in MJDA.1
* @param coordSysIn Coordinate system in which input state is defined
*        containing origin and axes information.
* @param stateRepIn State representation of the input state
* @param coordSysOut Coordinate system in which output state is
*        defined containing origin and axes information.
* @param stateRepOut State representation of the output state
* @param ephemType "Spice" or "Spline": how do you look up where planets are?
* @param ephemSmoother ephemSmoother object used to find where planets are
*
* @return stateOut Output orbital state
*/
//------------------------------------------------------------------------------
Rvector6 TransformUtil::TransformOrbitalState(const Rvector6 &stateIn,
   const Real &timeIn, CoordinateSystem *coordSysIn,
   const std::string &stateRepIn, CoordinateSystem *coordSysOut,
   const std::string &stateRepOut, const std::string &ephemType,
   EphemSmoother *ephemerisSmoother)
{
   // Get necessary coordinate system values
   CelestialBody *originIn = (CelestialBody*)coordSysIn->GetOrigin();
   CelestialBody *originOut = (CelestialBody*)coordSysOut->GetOrigin();
   std::string originInName = coordSysIn->GetOriginName();
   std::string originOutName = coordSysOut->GetOriginName();
   AxisSystem* axesIn = coordSysIn->GetAxisSystem();
   AxisSystem* axesOut = coordSysOut->GetAxisSystem();
   std::string axesInName = axesIn->GetTypeName();
   std::string axesOutName = axesOut->GetTypeName();

   Rvector6 stateOut = stateIn;
   StateConversionUtil *stateConvUtil;
   Real emphemTime;

   // Convert the state orbit representation to Cartesian if necessary
   // It IS necessary if any of the following is true:
   // 1. The origins of the input and the output are different AND the input
   //    state rep is not cartesian
   // 2. The axes of the input and the output are different AND the input state
   //    rep is not cartesian
   // 3. The output state rep is Cartesian AND the input state rep is not
   //    Cartesian

   // Check if the input state is not Cartesian--------------------------------
   std::string currStateRepIn; // The current representation of the state
   if (originInName != originOutName || axesInName != axesOutName)
   {
      if (stateRepIn != "Cartesian")
      {
         // Convert phase state to Cartesian
         Real mu, flattening, eqRadius;
         mu = originIn->GetGravitationalConstant();
         flattening = originIn->GetFlattening();
         eqRadius = originIn->GetEquatorialRadius();
         stateOut = stateConvUtil->Convert(stateOut, stateRepIn, "Cartesian",
            mu, flattening, eqRadius);
      }

      // The input state is now converted to Cartesian
      currStateRepIn = "Cartesian";

      // We now have the unscaled cartesian state w.r.t. the origin and
      // axes of the input
   }
   else
   {
      // The current input state repesentation is already Cartesian
      currStateRepIn = stateRepIn;
   }


   // Check whether origins are different--------------------------------------
   if (originInName != originOutName)
   {
      // Translate the input's state to the output's origin(using spice)
      // Convert current time (assumed to be MJDA1) to Spice ephemeris time

      // Translate state
      if (ephemType == "Spice")
      {
         stateOut =
            CoordinateTranslation::TranslateOrigin(timeIn, originIn, stateOut,
               originOut, coordSysIn);
      }
      else if (ephemType == "Spline")
      {
         stateOut = CoordinateTranslation::TranslateOriginSmoothed(timeIn,
            originIn, stateOut, originOut, coordSysIn, ephemerisSmoother);
      }
      
      // Now have unscaled cartesian state w.r.t. the origin of the output in
      // the axes of the input
   }

   // Check whether axes are different-----------------------------------------
   if (axesInName != axesOutName)
   {
      // Rotate the input's state to the output's axes
      stateOut =
         CoordinateTransformation::TransformState(timeIn, axesIn, stateOut,
            axesOut);
      // Now have unscaled cartesian w.r.t. the origin and axes of the output
   }

   // Convert input state to output's representation
   Real mu, flattening, eqRadius;
   mu = originOut->GetGravitationalConstant();
   flattening = originOut->GetFlattening();
   eqRadius = originOut->GetEquatorialRadius();
   stateOut = stateConvUtil->Convert(stateOut, currStateRepIn, stateRepOut,
      mu, flattening, eqRadius);

   return stateOut;
}

//------------------------------------------------------------------------------
// void CalculateOrbitalJacobian(
//    Rmatrix &jacOrbitStateOutputVarsWrtInputVars,
//    Rmatrix &jacOrbitStateOutputVarsWrtTime, const Rvector6 &stateIn,
//    const Real &timeIn, CoordinateSystem *coordSysIn,
//    const std::string &stateRepIn, CoordinateSystem *coordSysOut,
//    const std::string &stateRepOut, const std::string &ephemType,
//    EphemSmoother *ephemerisSmoother)
//------------------------------------------------------------------------------
/*
* Calculate Jacobian of transformation from one representation/frame/origin to
* another
*
* @param jacOrbitStateOutputVarWrtInputVars Jacobian to be modified to contain
*        the derivatives of the orbital state w.r.t. stateIn
* @param jacOrbitStateOutputVarsWrtTime Jacobian to be modified to contain
*        the derivatives of the orbital state w.r.t. time
* @param stateIn Input orbital state
* @param timeIn Input time in MJDA.1
* @param coordSysIn Coordinate system in which input state is defined
*        containing origin and axes information.
* @param stateRepIn State representation of the input state
* @param coordSysOut Coordinate system in which output state is
*        defined containing origin and axes information.
* @param stateRepOut State representation of the output state
* @param ephemType "Spice" or "Spline": how do you look up where planets are?
* @param ephemSmoother ephemSmoother object used to find where planets are
*/
//------------------------------------------------------------------------------
void TransformUtil::CalculateOrbitalJacobian(
   Rmatrix &jacOrbitStateOutputVarsWrtInputVars,
   Rmatrix &jacOrbitStateOutputVarsWrtTime, const Rvector6 &stateIn,
   const Real &timeIn, CoordinateSystem *coordSysIn,
   const std::string &stateRepIn, CoordinateSystem *coordSysOut,
   const std::string &stateRepOut, const std::string &ephemType,
   EphemSmoother *ephemerisSmoother)
{
   // Get necessary coordinate system values
   CelestialBody *originInObj = (CelestialBody*)coordSysIn->GetOrigin();
   CelestialBody *originOutObj = (CelestialBody*)coordSysOut->GetOrigin();
   std::string originIn = coordSysIn->GetOriginName();
   std::string originOut = coordSysOut->GetOriginName();
   AxisSystem* axesInObj = coordSysIn->GetAxisSystem();
   AxisSystem* axesOutObj = coordSysOut->GetAxisSystem();
   std::string axesIn = axesInObj->GetTypeName();
   std::string axesOut = axesOutObj->GetTypeName();

   Rvector6 stateOut = stateIn;
   StateConversionUtil *stateConvUtil;

   // Derivative of transformation from input representation to cartesian
   // representation in the math spec, this corresponds to getting :
   // [d f^{S_C / S_D} (z^D)] / [d z^D] (for partials w.r.t. state)

   // Before doing anything else, we need to get the derivatives of the
   // input(before being transformed at all) w.r.t.time
   // NH: I *think* the partial and total derivatives w.r.t.time will be
   // zero here ...
   Rmatrix partialOrbitStateInputVarsWrtTime(6, 1);
   Rmatrix jacOrbitStateInputVarsWrtTime = partialOrbitStateInputVarsWrtTime;

   // Convert the state orbit representation to Cartesian if necessary
   // It IS necessary if any of the following is true:
   // 1. The origins of the input and the output are different AND the input
   //    state rep is not cartesian
   // 2. The axes of the input and the output are different AND the input state
   //    rep is not cartesian
   // 3. The output state rep is Cartesian AND the input state rep is not
   //    Cartesian

   // Check if the input state is not Cartesian--------------------------------
   Rmatrix jacTransformToCartesianInputVars(6, 6),
      partialTransformToCartesianWrtTime(6, 1); // This currently just remains
                                                // a matrix of zeros
   std::string currStateRepIn; // The current representation of the state
   if (originIn != originOut || axesIn != axesOut)
   {
      if (stateRepIn == "Cartesian") // State came in Cartesian
      {
         // So, derivative of transformation to cartesian state w.r.t. state
         // is identity
         jacTransformToCartesianInputVars =
            jacTransformToCartesianInputVars.Identity(6);
         // Derivative of transformation w.r.t. time is zero:
         // [\partial z'] / [\partial t]
      }
      else
      {
         // Convert phase state to Cartesian
         Real mu, flattening, eqRadius;
         mu = originInObj->GetGravitationalConstant();
         flattening = originInObj->GetFlattening();
         eqRadius = originInObj->GetEquatorialRadius();
         stateOut = stateConvUtil->Convert(stateOut, stateRepIn, "Cartesian",
            mu, flattening, eqRadius);

         // Partials of transformation to Cartesian
         jacTransformToCartesianInputVars =
            stateConvUtil->StateConvJacobian(stateOut, "Cartesian", stateRepIn,
            "Cartesian", mu, flattening, eqRadius);

         // For now, the time Jacobian of changing the state rep is always 0
         // [\partial z'] / [\partial t]
      }

      // The input state is now converted to Cartesian
      currStateRepIn = "Cartesian";

      // We now have the jacobian of the unscaled state in cartesian 
      // coordinates using the axes and origin of the input with respect
      // to the unscaled state in input coordinates using the axes and origin
      // of the input
   }
   else
   {
      // the origins of the input and output are the same
      // the axes of the input and output are the same
      // so, we don't have to pass through Cartesian necessarily
      jacTransformToCartesianInputVars =
         jacTransformToCartesianInputVars.Identity(6);
      currStateRepIn = stateRepIn;
   }


   // Check whether origins are different--------------------------------------
   Rmatrix jacOriginTransformInputVars(6, 6), 
      partialOriginTransformWrtTime(6, 1);
   if (originIn != originOut)
   {
      // Translate the input's state to the output's origin(using spice)
      // Need to calculate partials w.r.t. state:
      // [d f^{ O_F / O_D } (z')] / [d z'] (derivative of transformation from
      // origin of input frame to origin of output frame)
      
      // This is where we use EphemSmoother, which must be set before we can
      // do this


      // This function call is only valid if the origins of both frames are
      // celestial bodies
      // returns [d f^{ O_F / O_D } (z')] / [d z'] and
      // [\partial f^{ O_F / O_D } (z')] / [\partial t]
      // Also returns the transformed state itself : z''
      CoordinateTranslation::TranslateOriginSmoothedWithDerivatives(timeIn,
         originInObj, stateOut, originOutObj, coordSysIn, ephemerisSmoother,
         stateOut, jacOriginTransformInputVars, partialOriginTransformWrtTime);
   }
   else
   {
      // if the origins are the same, then the time jacobian and the state
      // jacobian are zero
   }

   // We now have the jacobian of the unscaled state in cartesian coordinates
   // using the axes of the input and origin of the output with respect to
   // the unscaled state in cartesian coordinates using the axes and origin
   // of the input

   // Check whether axes are different-----------------------------------------

   // @TODO: Add conversions for differing axes, for now it is assumed they
   // are the same
   Rmatrix jacAxesTransformInputVars(6, 6),
      partialAxesTransformWrtTime(6, 1);

   jacAxesTransformInputVars = jacAxesTransformInputVars.Identity(6);

   // We now have the jacobian of the unscaled state in cartesian coordinates
   // using the axes and origin of the output with respect to the unscaled
   // state in cartesian coordinates using the axes of the input and the origin
   // of the output

   // derivative of transformation from cartesian representation to output
   // representation :
   // [d f^{S_F / S_C}] / [d z'''] and
   // [\partial f^{S_F / S_C}] / [\partial z''']
   Real mu, flattening, eqRadius;
   mu = originOutObj->GetGravitationalConstant();
   flattening = originOutObj->GetFlattening();
   eqRadius = originOutObj->GetEquatorialRadius();

   // Perform a state conversion
   stateOut = stateConvUtil->Convert(stateOut, currStateRepIn, stateRepOut, mu,
      flattening, eqRadius);

   // Now do the partials of the state conversion
   Rmatrix jacTransformToOutputRepInputVars =
      stateConvUtil->StateConvJacobian(stateOut, stateRepOut, currStateRepIn,
         stateRepOut, mu, flattening, eqRadius);

   // Assume the time partials are zero for now
   Rmatrix partialTransformToOutputRepWrtTime(6, 1);

   // Now that we have the final state form(z^F), get the partials of it 
   // w.r.t. time
   Rmatrix partialOrbitStateOutputVarsWrtTime = DecVecTimeDerivatives(stateOut,
      stateRepOut, axesOut, originOut);

   // Combine all pieces together----------------------------------------------

   // State
   jacOrbitStateOutputVarsWrtInputVars = jacTransformToOutputRepInputVars *
      (jacAxesTransformInputVars * (jacOriginTransformInputVars *
      jacTransformToCartesianInputVars + jacTransformToCartesianInputVars));

   // Time

   // Parens temp vars go from inside to outside
   Rmatrix parens1 = jacTransformToCartesianInputVars *
      jacOrbitStateInputVarsWrtTime + partialOrbitStateInputVarsWrtTime;

   // NH: Not totally sure about PartialOriginTransformationWrtTime going here
   // math spec has time partial of z'', i put the time partial of the origin
   // transformation
   Rmatrix parens2 = jacOriginTransformInputVars * parens1 +
      partialTransformToCartesianWrtTime + partialOriginTransformWrtTime;
   Rmatrix parens3 = jacAxesTransformInputVars * parens2 +
      partialAxesTransformWrtTime;
   jacOrbitStateOutputVarsWrtTime = jacTransformToOutputRepInputVars *
      parens3 + partialOrbitStateOutputVarsWrtTime;
}

//------------------------------------------------------------------------------
// Rmatrix DecVecTimeDerivatives(const Rvector6 &stateOut,
//    const std::string &stateRepIn, const std::string &axesIn,
//    const std::string &originIn)
//------------------------------------------------------------------------------
/*
* Calculate time derivatives of decision vector
*
* @param stateIn Input state in inputType
* @param inputType The type of the input state (stateTypes enum)
* @param inputAxes The axes in which stateIn is written (string)
* @param inputOrigin The origin of the axes in which stateIn is written (string)
*
* @return partialDecVecPartialTime Partial derivative of stateIn w.r.t. time
*/
//------------------------------------------------------------------------------
Rmatrix TransformUtil::DecVecTimeDerivatives(const Rvector6 &stateOut,
   const std::string &stateRepIn, const std::string &axesIn,
   const std::string &originIn)
{
   // For now, it is assumed the elements are zero
   Rmatrix partialDecVecWrtTime(6, 1);
   return partialDecVecWrtTime;
}
