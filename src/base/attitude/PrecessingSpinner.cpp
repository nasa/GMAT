//------------------------------------------------------------------------------
//                             PrecessingSpinner
//------------------------------------------------------------------------------
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under purchase
// order NNG16LD52P
//
// Author: Yeerang Lim/KAIST
// Created: 2013.05.15
// Updated: 20130.06.26 (Jaehwan Pi/KAIST)
//
/**
 * Class implementation for the PrecessingSpinner class.
 * 
 * @note 
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Attitude.hpp"
#include "AttitudeException.hpp"
#include "MessageInterface.hpp"
#include "AttitudeConversionUtility.hpp"

#include "PrecessingSpinner.hpp"


//#define DEBUG_PRECESSING_SPINNER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  PrecessingSpinner(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the PrecessingSpinner class (constructor).
 */
//------------------------------------------------------------------------------
PrecessingSpinner::PrecessingSpinner(const std::string &itsName) : 
   Kinematic("PrecessingSpinner",itsName)
{
   parameterCount = PrecessingSpinnerParamCount;
   objectTypeNames.push_back("PrecessingSpinner");
   attitudeModelName         = "PrecessingSpinner";
   setInitialAttitudeAllowed = false;
   modifyCoordSysAllowed     = false;
   // Reserve spaces to handle attribute comments for owned object
   // LOJ: 2013.03.01 for GMT-3353 FIX

   FinalizeCreation();
}

//------------------------------------------------------------------------------
//  PrecessingSpinner(const PrecessingSpinner &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the PrecessingSpinner class as a copy of the
 * specified PrecessingSpinner class (copy constructor).
 *
 * @param <att> PrecessingSpinner object to copy.
 */
//------------------------------------------------------------------------------
PrecessingSpinner::PrecessingSpinner(const PrecessingSpinner& att) :
   Kinematic(att)
{
}
 
//------------------------------------------------------------------------------
//  PrecessingSpinner& operator= (const PrecessingSpinner& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the PrecessingSpinner class.
 *
 * @param att the PrecessingSpinner object whose data to assign to "this"
 *            PrecessingSpinner.
 *
 * @return "this" PrecessingSpinner with data of input PrecessingSpinner att.
 */
//------------------------------------------------------------------------------
PrecessingSpinner& PrecessingSpinner::operator=(const PrecessingSpinner& att)
{
   Kinematic::operator=(att);

   return *this;
}

//------------------------------------------------------------------------------
//  ~PrecessingSpinner()
//------------------------------------------------------------------------------
/**
 * Destroys the PrecessingSpinner class (constructor).
 */
//------------------------------------------------------------------------------
PrecessingSpinner::~PrecessingSpinner()
{
   // nothing really to do here ... la la la la la
}


//---------------------------------------------------------------------------
//  bool Initialize() 
//---------------------------------------------------------------------------
 /**
 * Initializes the attitude.
 * 
 * @return Success flag.
 *
  */
//---------------------------------------------------------------------------
bool PrecessingSpinner::Initialize()
{
   if (!Kinematic::Initialize()) return false;

   // Cannot check bodySpinAxis or nutationReferenceVector values here,
   // as Initialize is called on Attitude when getting any data value,
   // including when the SpacecraftPanel is brought up; this happens before
   // the reference body pointer has been set on the Attitude, so an error
   // message here would be generated each time the panel is opened.
   // We can wait until the attitude is computed to do this check.


   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the PrecessingSpinner.
 *
 * @return clone of the PrecessingSpinner.
 *
 */
//------------------------------------------------------------------------------
GmatBase* PrecessingSpinner::Clone() const
{
	return (new PrecessingSpinner(*this)); //???(Yeerang Lim)
}

//------------------------------------------------------------------------------
//  void ComputeCosineMatrixAndAngularVelocity(Real atTime)
//------------------------------------------------------------------------------
/**
 * This method computes the attitude matrix and angular velocity
 *
 */
//------------------------------------------------------------------------------
void PrecessingSpinner::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   if (!isInitialized || needsReinit) Initialize();

   Real bsMag = bodySpinAxis.GetMagnitude();
   Real nrMag = nutationReferenceVector.GetMagnitude();

   #ifdef DEBUG_PRECESSING_SPINNER
      MessageInterface::ShowMessage(
      "Entering PrecessingSpinner::ComputeCosineMatrixAndAngularVelocity, BodySpinAxis = [ %.10f %.10f %.10f]\n",
      bodySpinAxis(0), bodySpinAxis(1), bodySpinAxis(2));

      MessageInterface::ShowMessage("NutationReferenceVector = [ %.10f %.10f %.10f]\n",
            nutationReferenceVector(0), nutationReferenceVector(1), nutationReferenceVector(2));
    #endif

   // Error message and vector normalization
   if ( bsMag < 1.0e-5 )
   {
      std::string errmsg = "PrecessingSpinner attitude model is singular ";
      errmsg            += "and/or undefined for Spacecraft ";
      errmsg            += owningSC->GetName() + ".  Magnitude of ";
      errmsg            += "BodySpinVector must be >= 1e-5\n";
      throw AttitudeException(errmsg);
   }
   if ( nrMag < 1.0e-5 )
   {
      std::string errmsg = "PrecessingSpinner attitude model is singular ";
      errmsg            += "and/or undefined for Spacecraft ";
      errmsg            += owningSC->GetName() + ".  Magnitude of ";
      errmsg            += "NutationReferenceVector must be >= 1e-5\n";
      throw AttitudeException(errmsg);
   }

   bodySpinAxisNormalized            = bodySpinAxis / bsMag;
   nutationReferenceVectorNormalized = nutationReferenceVector / nrMag;

   #ifdef DEBUG_PRECESSING_SPINNER
      MessageInterface::ShowMessage("BodySpinAxis = [ %.10f %.10f %.10f]\n",
      bodySpinAxisNormalized(0), bodySpinAxisNormalized(1), bodySpinAxisNormalized(2));

      MessageInterface::ShowMessage("NutationReferenceVector = [ %.10f %.10f %.10f]\n",
            nutationReferenceVectorNormalized(0), nutationReferenceVectorNormalized(1),
            nutationReferenceVectorNormalized(2));

      MessageInterface::ShowMessage("InitialPrecessionAngle = %.10f , PrecessionRate = %.10f\n",
         initialPrecessionAngle*GmatMathConstants::DEG_PER_RAD, precessionRate*GmatMathConstants::DEG_PER_RAD);

      MessageInterface::ShowMessage("NutationAngle = %.10f\n" , nutationAngle*GmatMathConstants::DEG_PER_RAD);

      MessageInterface::ShowMessage("InitialSpinAngle = %.10f , SpinRate = %.10f\n",
         initialSpinAngle*GmatMathConstants::DEG_PER_RAD, spinRate*GmatMathConstants::DEG_PER_RAD);
   #endif

   // Calculate RBIt, where t = atTime
   Real      dt              = (atTime - epoch) * GmatTimeConstants::SECS_PER_DAY;

   #ifdef DEBUG_PRECESSING_SPINNER
      MessageInterface::ShowMessage(
      "In PrecessingSpinner::Compute ... atTime = %.12f and dt = %.12f\n", atTime, dt);
   #endif

   // Calculate current angles   
   Real spinAngle       = ( spinRate * dt ) + initialSpinAngle;
   Real precessionAngle = ( precessionRate * dt ) + initialPrecessionAngle;

   // Rotate to align BodySpinAxis to NutationReferenceVector
   Rvector3 rvectorAlign = Cross(bodySpinAxisNormalized, nutationReferenceVectorNormalized);
   Rmatrix33 rmatrixInit(true);  // identity matrix, by default
   Real alignMag = rvectorAlign.GetMagnitude();
   Real angleAlign = GmatMathUtil::ACos(bodySpinAxisNormalized * nutationReferenceVectorNormalized);  //in radian
   if(angleAlign < 1.0e-16)
   {
	   // Leave RmatrixInit as identity
   }
   else
   {
	   Rvector3 rvectorAlignNormalized = rvectorAlign / alignMag;
	   rmatrixInit = AttitudeConversionUtility::EulerAxisAndAngleToDCM(rvectorAlignNormalized, angleAlign);
   }

   //Set Body 123 axis to perform 3-1-3 rotation
   xAxis.Set(1,0,0);
   yAxis.Set(0,1,0);
   Rvector3 bodyAxis1 = Cross(bodySpinAxisNormalized, xAxis);
   if (bodyAxis1.GetMagnitude() < 1.0e-5)
   {
	   bodyAxis1.Set(0,1,0);
   }
   Rvector3 bodyAxis3 = bodySpinAxisNormalized;
   Rvector3 bodyAxis2 = Cross(bodyAxis3, bodyAxis1);

   bodyAxis1 = bodyAxis1 / bodyAxis1.GetMagnitude();
   bodyAxis2 = bodyAxis2 / bodyAxis2.GetMagnitude();
   bodyAxis3 = bodyAxis3 / bodyAxis3.GetMagnitude();

   //Rotate with PrecessingAngle
   Rmatrix33 rmatrixPrecession = AttitudeConversionUtility::EulerAxisAndAngleToDCM(bodyAxis3,precessionAngle);

   //Rotate with NutationAngle
   Rmatrix33 rmatrixNutation   = AttitudeConversionUtility::EulerAxisAndAngleToDCM(bodyAxis1,nutationAngle);

   //Rotate with SpinAngle
   Rmatrix33 rmatrixSpin       = AttitudeConversionUtility::EulerAxisAndAngleToDCM(bodyAxis3,spinAngle);

   // Final rotation matrix from Inertial to Body
   dcm = rmatrixSpin * rmatrixNutation * rmatrixPrecession * rmatrixInit ;

   // Calculate angular velocity in Body 123 frame
   Real omega1 = precessionRate*GmatMathUtil::Sin(nutationAngle)*GmatMathUtil::Sin(spinAngle);
   Real omega2 = precessionRate*GmatMathUtil::Sin(nutationAngle)*GmatMathUtil::Cos(spinAngle);
   Real omega3 = precessionRate*GmatMathUtil::Cos(nutationAngle) + spinRate;

   Rvector3 omega123(omega1, omega2, omega3);

   // Rotation matrix from Body 123 to Body xyz frame
   Rmatrix33 RBb(bodyAxis1(0), bodyAxis2(0), bodyAxis3(0),
                 bodyAxis1(1), bodyAxis2(1), bodyAxis3(1),
                 bodyAxis1(2), bodyAxis2(2), bodyAxis3(2));

   // Angular velocity of Body w.r.t. Inertial expressed in Body xyz frame
   Rvector3 omega_xyz = RBb * omega123;

   angVel(0) = omega_xyz(0);	//in radians
   angVel(1) = omega_xyz(1);
   angVel(2) = omega_xyz(2);
}


//------------------------------------------------------------------------------
//  void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime)
//------------------------------------------------------------------------------
/**
* This method computes the attitude matrix and angular velocity at a given time
*
* @param atTime   a GmatTime time used to calculate the attitude matrix
*/
//------------------------------------------------------------------------------
void PrecessingSpinner::ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime)
{
   ///@todo: it needs to add code for a high precision of time
   ComputeCosineMatrixAndAngularVelocity(atTime.GetMjd());
}


//------------------------------------------------------------------------------
//  private methods
//------------------------------------------------------------------------------
// none 
