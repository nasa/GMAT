//------------------------------------------------------------------------------
//                            ThreeAxisKinematic
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
//
// Author: Michael Stark / GSFC
// Created: 2019.05.16
//
/**
 * Class implementation for the ThreeAxisKinematic class.
 * 
 * @note 
 */
//------------------------------------------------------------------------------


//#define DEBUG_THREE_AXIS_INIT // used in constructors & Initialize()
//#define DEBUG_THREE_AXIS // use in main DCM and angular velocity computation

#include "ThreeAxisKinematic.hpp"

#include "AttitudeException.hpp"
#include "MessageInterface.hpp"

#include "GmatConstants.hpp"             // for seconds per day
#include "Rvector3.hpp"                  // for vector magnitude
#include "RealUtilities.hpp"             // for trig functions
#include "AttitudeConversionUtility.hpp" // convert q to dcm
#include "AttitudeUtil.hpp"              // normalize quaternion
#include "Rvector.hpp"                   // for debug output string

using namespace GmatTimeConstants;
using namespace GmatMathConstants;
//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

// none


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  ThreeAxisKinematic(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the ThreeAxisKinematic class (constructor).
 *
 * @param <itsname> Text identifier for this object.
 */
//------------------------------------------------------------------------------
ThreeAxisKinematic::ThreeAxisKinematic(const std::string &itsName) :
	Kinematic("ThreeAxisKinematic",itsName),
   Omega (Rmatrix(4,4)),
   wMag (0.0)
{
   #ifdef DEBUG_THREE_AXIS_INIT
   MessageInterface::ShowMessage(
      "*** Entering ThreeAxisKinematic constructor ***\n");
   MessageInterface::ShowMessage(
      "     for object %s, parent constructor has already returned\n",
      itsName.c_str() );
   #endif
   
   parameterCount = ThreeAxisParamCount;
   objectTypeNames.push_back("ThreeAxisKinematic");
   attitudeModelName         = "ThreeAxisKinematic";
   
   // set flags governing computations
   //modifyCoordSysAllowed     = false;
   //modelComputesRates = false;
   
   // initialize 4 x 4 identity for propagation of angular velocity
   I44 = Rmatrix::Identity(4);

   FinalizeCreation();
   #ifdef DEBUG_THREE_AXIS_INIT
   MessageInterface::ShowMessage("Leaving ThreeAxisKinematic constructor\n");
   #endif

}

//------------------------------------------------------------------------------
//  ThreeAxisKinematic(const ThreeAxisKinematic &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the ThreeAxisKinematic class as a copy of the
 * specified ThreeAxisKinematic class (copy constructor).
 *
 * @param <att> ThreeAxisKinematic object to copy.
 */
//------------------------------------------------------------------------------
ThreeAxisKinematic::ThreeAxisKinematic(const ThreeAxisKinematic& att) :
	Kinematic(att),
   I44      (att.I44),
   Omega    (att.Omega),
   wMag     (att.wMag)
{
   #ifdef DEBUG_THREE_AXIS_INIT
//   MessageInterface::ShowMessage(
//      "***  ThreeAxisKinematic copy constructor has copied member data " +
//      "for object %s ***\n", instanceName.c_str());
   #endif
   
}
 
//------------------------------------------------------------------------------
//  ThreeAxisKinematic& operator= (const ThreeAxisKinematic& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ThreeAxisKinematic class.
 *
 * @param att the ThreeAxisKinematic object whose data to assign to "this"
 *            ThreeAxisKinematic object.
 *
 * @return "this" ThreeAxisKinematic with data of input ThreeAxisKinematic att.
 */
//------------------------------------------------------------------------------
ThreeAxisKinematic& ThreeAxisKinematic::operator=(const ThreeAxisKinematic& att)
{
   #ifdef DEBUG_THREE_AXIS_INIT
   MessageInterface::ShowMessage(
      "*** Entering ThreeAxisKinematic operator=()  for object %s ***\n",
                                 instanceName.c_str());
   #endif
   
   if (&att == this)
      // assigning to same object, no action needed
      return *this;
   
   // superclass assignment
   Kinematic::operator=(att);
      
   I44   = att.I44;
   Omega = att.Omega;
   wMag  = att.wMag;
   return *this;

   #ifdef DEBUG_THREE_AXIS_INIT
   MessageInterface::ShowMessage("Leaving ThreeAxisKinematic operator=()\n");
   #endif
}


//------------------------------------------------------------------------------
//  ~ThreeAxisKinematic()
//------------------------------------------------------------------------------
/**
 * Destroys the ThreeAxisKinematic class (constructor).
 */
//------------------------------------------------------------------------------
ThreeAxisKinematic::~ThreeAxisKinematic()
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
bool ThreeAxisKinematic::Initialize()
{
   #ifdef DEBUG_THREE_AXIS_INIT
   MessageInterface::ShowMessage(
      "*** Entering ThreeAxisKinematic::Initialize()  for object %s ***\n",
      instanceName.c_str());
   #endif
   
   // initialize parent
   bool success = Kinematic::Initialize();
   if (!success)
   {
      #ifdef DEBUG_THREE_AXIS_INIT
      MessageInterface::ShowMessage(
         "failed assignment in call to superclass Kinematic, returning false\n");
      #endif      
      return false;
   }
   // convert dcm inputs
   if (inputAttitudeType != GmatAttitude::QUATERNION_TYPE)
       quaternion = AttitudeConversionUtility::ToQuaternion(dcm);
   wMag = angVel.GetMagnitude();
   Real w1 = angVel[0];
   Real w2 = angVel[1];
   Real w3 = angVel[2];
   Omega = Rmatrix(4,4,
             0.0,  w3, -w2, w1,
             -w3, 0.0,  w1, w2,
              w2, -w1, 0.0, w3,
             -w1, -w2, -w3, 0.0  );
 
   #ifdef DEBUG_THREE_AXIS_INIT
   // write initial values
      MessageInterface::ShowMessage("I44 =\n");
      for (int i=0; i < 4; i++)
      {
         // need precision for string conversion
         MessageInterface::ShowMessage(I44.ToRowString(i,3) );
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\nAngular Velocity =");
      MessageInterface::ShowMessage(angVel.ToString(3) ); // need precision
      MessageInterface::ShowMessage("\nOmega =\n");
      for (int i=0; i < 4; i++)
      {
         // need precision here, too
         MessageInterface::ShowMessage(Omega.ToRowString(i,3) );
         MessageInterface::ShowMessage("\n");
      }
   #endif
      
   #ifdef DEBUG_THREE_AXIS_INIT
      MessageInterface::ShowMessage(
         "successful initialization, returning true\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ThreeAxisKinematic.
 *
 * @return clone of the ThreeAxisKinematic.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ThreeAxisKinematic::Clone() const
{
	return (new ThreeAxisKinematic(*this));
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
bool ThreeAxisKinematic::IsParameterCommandModeSettable(const Integer id) const
{
   // allow commanding of angular velocity to simulate maneuvers
   if ((id == ANGULAR_VELOCITY)   ||
       (id == ANGULAR_VELOCITY_X) ||
       (id == ANGULAR_VELOCITY_Y) ||
       (id == ANGULAR_VELOCITY_Z) )
      return true;
      // may add code later to allow epoch or attitude to change
   
   // kick Boolean test up the inheritance chain
   return Kinematic::IsParameterCommandModeSettable(id);
}

   
//------------------------------------------------------------------------------
//  protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime)
//------------------------------------------------------------------------------
/**
 * This method computes the current CosineMatrix at the input time atTime.
 *
 * @param atTime the A1Mjd time at which to compute the attitude.
 *
 * @note This method will update the CosineMatrix parameter dcm of the class.
 */
//------------------------------------------------------------------------------

void ThreeAxisKinematic::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   #ifdef DEBUG_THREE_AXIS
      MessageInterface::ShowMessage(
         "*** ThreeAxisKinematic object %s, computing DCM at time %13.7f, epoch = %13.7f\n",
         instanceName.c_str(), atTime, epoch);
   #endif
   
   if (atTime != epoch)
   {

      if (!isInitialized || needsReinit)
      {
         #ifdef DEBUG_THREE_AXIS
            MessageInterface::ShowMessage(
               "need to initialize or reinitialize attitude\n" );
         #endif

         Initialize();

         #ifdef DEBUG_THREE_AXIS
            MessageInterface::ShowMessage("Attitude re/initialized successfully\n");
         #endif
      }
      #ifdef DEBUG_THREE_AXIS
         MessageInterface::ShowMessage("Current quaternion = %s\n",
                                 quaternion.ToString().c_str());
      #endif

      Real angle = wMag * (SECS_PER_DAY * (atTime - epoch)) / 2;
   
      #ifdef DEBUG_THREE_AXIS
         MessageInterface::ShowMessage("wMag = %12.7f, angle = %12.7f\n",
                                    wMag,angle);
      #endif
      if (wMag != 0.0)
      {
         quaternion = (cos(angle) * I44 + (1.0/wMag) *
                       sin(angle) * Omega) * quaternion;
         quaternion = quaternion.Normalize();
      }
 
      // update state; remember angular velocity is constant unless changed
      // by script or user
      dcm   = AttitudeConversionUtility::ToCosineMatrix(quaternion);
      epoch = atTime;

      #ifdef DEBUG_THREE_AXIS
         // need precision for quaternion string
         MessageInterface::ShowMessage("Newly computed quaternion = %s\n",
                                       quaternion.ToString().c_str());
         MessageInterface::ShowMessage("New epoch is %13.7f\n",epoch);
         MessageInterface::ShowMessage(
            "Exiting ThreeAxisKinematic DCM computations at time %le\n", atTime);
      #endif
   }  // if atTime = epoch
}

void ThreeAxisKinematic::ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime)
{
   ///@todo: it needs to add code for high precision of time
   ComputeCosineMatrixAndAngularVelocity(atTime.GetMjd());
}


//------------------------------------------------------------------------------
//  private methods
//------------------------------------------------------------------------------
// none 
