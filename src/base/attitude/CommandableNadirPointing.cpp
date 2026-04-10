//------------------------------------------------------------------------------
//                     Commandable Nadir Pointing
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
// Author: Mike Stark// Created: 2022.03.10
//
/**
 * Class implementation for the CommandableNadirPointing class.
 * 
 * @note 
 */
//------------------------------------------------------------------------------

//#define DEBUG_CNP         // for sets & gets
//#define DEBUG_CNP_INIT    // for Initialize()
//#define DEBUG_CNP_COMPUTE // for computing DCM

#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "AttitudeConversionUtility.hpp"
#include "AttitudeException.hpp"
#include "SpaceObject.hpp"

#include "CommandableNadirPointing.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Real CommandableNadirPointing::DENOMINATOR_TOLERANCE = 1.0e-15;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  CommandableNadirPointing(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Nadir class (constructor).
 */
//------------------------------------------------------------------------------
CommandableNadirPointing::CommandableNadirPointing(const std::string &itsName) :
   Kinematic(itsName),
   hasComputedFirstDCM  (false)
{
   parameterCount = CommandableNadirPointingParamCount;
   objectTypeNames.push_back("CommandableNadirPointing");
   
   attitudeModelName         = "CommandableNadirPointing";
   setInitialAttitudeAllowed = true; // overrides prohibited "sets" in NadirPointing
}

//------------------------------------------------------------------------------
//  NadirPointing(const NadirPointing &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Nadir class as a copy of the
 * specified Nadir class (copy constructor).
 *
 * @param <att> Nadir object to copy.
 */
//------------------------------------------------------------------------------
CommandableNadirPointing::CommandableNadirPointing(
                    const CommandableNadirPointing& att) :
   Kinematic(att),
   hasComputedFirstDCM  (att.hasComputedFirstDCM)
{
}
 
//------------------------------------------------------------------------------
//  NadirPointing& operator= (const NadirPointing& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the NadirPointing class.
 *
 * @param att the NadirPointing object whose data to assign to "this"
 *            NadirPointing.
 *
 * @return "this" NadirPointing with data of input NadirPointing att.
 */
//------------------------------------------------------------------------------
CommandableNadirPointing&
   CommandableNadirPointing::operator=(const CommandableNadirPointing& att)
{
   Kinematic::operator=(att);
   hasComputedFirstDCM = att.hasComputedFirstDCM;
   return *this;
}


//------------------------------------------------------------------------------
//  ~NadirPointing()
//------------------------------------------------------------------------------
/**
 * Destroys the NadirPointing class (constructor).
 */
//------------------------------------------------------------------------------
CommandableNadirPointing::~CommandableNadirPointing()
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
bool CommandableNadirPointing::Initialize()
{
   
   
#ifdef DEBUG_CNP_INIT
   MessageInterface::ShowMessage(
      "Entering CommandableNadirPointing::Initialize() ...\n");
   MessageInterface::ShowMessage(" ... epoch = %12.5f     attitudeTime = %12.5f\n", epoch, attitudeTime);
   MessageInterface::ShowMessage ("isInitialized=%s\n", (isInitialized? "true": "false"));
   MessageInterface::ShowMessage ("needsReinit=%s  \n", (needsReinit? "true": "false"));
   MessageInterface::ShowMessage ("hasComputedFirstDCM=%s\n",
                                  (hasComputedFirstDCM? "true": "false") );
#endif
   if (!Kinematic::Initialize()) return false;
   // Cannot check refBody value here, as Initialize is called on
   // Attitude when getting any data value, including when the SpacecraftPanel
   // is brought up; this happens before the reference body pointer has been
   // set on the Attitude, so an error message here would be generated
   // each time the panel is opened.
   // We can wait until the attitude is computed to do this check.
   
   // if Kinematic::Initialize()) is successful, return true after writing debug
#ifdef DEBUG_CNP_INIT
   MessageInterface::ShowMessage(
      "CommandableNadirPointing::Initialize() after initializing superclass\n");
   MessageInterface::ShowMessage(" ... epoch = %12.5f     attitudeTime = %12.5f\n", epoch, attitudeTime);
   MessageInterface::ShowMessage ("isInitialized=%s\n", (isInitialized? "true": "false"));
   MessageInterface::ShowMessage ("needsReinit=%s  \n", (needsReinit? "true": "false"));
   MessageInterface::ShowMessage ("hasComputedFirstDCM=%s\n",
                                  (hasComputedFirstDCM? "true": "false") );
#endif
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Nadir.
 *
 * @return clone of the Nadir.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CommandableNadirPointing::Clone() const
{
	return (new CommandableNadirPointing(*this));
}

/* -------------------------
 functions overriding Attitude sets & gets
 @TODO usage guidance
 ---------------------------- */

//---------------------------------------------------------------------------
//  const Rvector&   GetQuaternion(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as a quaternion.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the quaternion representation of the attitude, computed at
 *         time atTime.
 */
//---------------------------------------------------------------------------
const Rvector&   CommandableNadirPointing::GetQuaternion(Real atTime)
{
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage(
      "Entering CommandableNadirPointing::GetQuaternion ...\n");
   MessageInterface::ShowMessage(" ... atTime = %12.5f     attitudeTime = %12.5f\n", atTime, attitudeTime);
   MessageInterface::ShowMessage(" ... attitude state - quaternion: %s\n",
         (quaternion.ToString()).c_str());
   MessageInterface::ShowMessage ("isInitialized=%s, needsReinit=%s\n",
      (isInitialized? "true": "false"), (needsReinit? "true": "false"));
   MessageInterface::ShowMessage ("hasComputedFirstDCM=%s\n",
                                  (hasComputedFirstDCM? "true": "false") );
#endif
   if (!isInitialized || needsReinit) Initialize();
   if (!hasComputedFirstDCM)
   {
      attitudeTime = 0.0; // force computation
      hasComputedFirstDCM = true;
   }
   
   if (GmatMathUtil::Abs(atTime - attitudeTime) > ATTITUDE_TIME_TOLERANCE)
   {
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage(" ... about to compute:  quaternion is %s\n",
         (quaternion.ToString()).c_str());
   MessageInterface::ShowMessage(" ... atTime = %12.5f     attitudeTime = %12.5f\n", atTime, attitudeTime);
#endif
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
      quaternion       = AttitudeConversionUtility::ToQuaternion(dcm);
   }
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage(" ... returning quaternion: %s\n",
         (quaternion.ToString()).c_str());
#endif
   return quaternion;
}

//---------------------------------------------------------------------------
//  const Rmatrix33&   GetCosineMatrix(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as direction cosine matrix.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the direction cosine matrix representation of the attitude,
 *         computed at time atTime.
 */
//---------------------------------------------------------------------------
const Rmatrix33& CommandableNadirPointing::GetCosineMatrix(Real atTime)
{
   #ifdef DEBUG_CNP
      MessageInterface::ShowMessage(
         "Entering CommandableNadirPointing::GetCosineMatrix ...\n");
      MessageInterface::ShowMessage(" ... atTime = %12.5f     attitudeTime = %12.5f\n", atTime, attitudeTime);
      // show flags
      MessageInterface::ShowMessage ("isInitialized=%s, needsReinit=%s\n",
         (isInitialized? "true": "false"), (needsReinit? "true": "false"));
      MessageInterface::ShowMessage ("hasComputedFirstDCM=%s\n",
                                     (hasComputedFirstDCM? "true": "false") );
      
      MessageInterface::ShowMessage(" ... attitude state - cosine matrix: %s\n",
         (dcm.ToString()).c_str());
   #endif
   if (!isInitialized || needsReinit) Initialize();
   if (!hasComputedFirstDCM)
   {
      attitudeTime = 0.0; // force computation
      hasComputedFirstDCM = true;
   }

#ifdef DEBUG_CNP   
   MessageInterface::ShowMessage(" ... about to compute: DCM is %s\n",
      (dcm.ToString()).c_str());
   MessageInterface::ShowMessage(" ... atTime = %12.5f     attitudeTime = %12.5f\n", atTime, attitudeTime);
#endif
   
   if (GmatMathUtil::Abs(atTime - attitudeTime) > ATTITUDE_TIME_TOLERANCE)
   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
      
      // update quaternion in state; GetCosineMatrix is called
      // by propagator & torque models (I think)
      quaternion       = AttitudeConversionUtility::ToQuaternion(dcm);
   }
   #ifdef DEBUG_CNP
      MessageInterface::ShowMessage(" ... returning cosine matrix: %s\n",
            (dcm.ToString()).c_str());
   #endif

   return dcm;
}
 
//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <id>    ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return  Real value of the requested parameter.
 *
 * @note Assumes that these are called before initialization and the execution
 *       of the mission sequence.
 *
 */
//------------------------------------------------------------------------------
Real CommandableNadirPointing::SetRealParameter(const Integer id,
                                const Real    value)
{
   Real retval;
   
   //------------------------------------
   // alignment & constraint parameters
   //------------------------------------
   
   // set up to update attitude
   if ( (id >= BODY_ALIGNMENT_VECTOR_X )   &&
       (id <= BODY_CONSTRAINT_VECTOR_Z)  )
   {
      needsReinit = true;
#ifdef DEBUG_CNP
      MessageInterface::ShowMessage("CommandableNadirPointing::SetRealParameter:\n");
      MessageInterface::ShowMessage ("identified alignment or constraint parameter\n");
#endif
      // @TODO -- add validation to input value
   }
   //else
      // there is no SetRealParameter() defined in Kinematic class,
      // thus calling attitude directly
      //return Attitude::SetRealParameter(id, value);
   
   if (id == BODY_ALIGNMENT_VECTOR_X)
   {
      bodyAlignmentVector(0) = value;
      return bodyAlignmentVector(0);
   }
   if (id == BODY_ALIGNMENT_VECTOR_Y)
   {
      bodyAlignmentVector(1) = value;
      return bodyAlignmentVector(1);
   }
   if (id == BODY_ALIGNMENT_VECTOR_Z)
   {
      bodyAlignmentVector(2) = value;
      return bodyAlignmentVector(2);
   }
   if (id == BODY_CONSTRAINT_VECTOR_X)
   {
      bodyConstraintVector(0) = value;
      return bodyConstraintVector(0);
   }
   if (id == BODY_CONSTRAINT_VECTOR_Y)
   {
      bodyConstraintVector(1) = value;
      return bodyConstraintVector(1);
   }
   if (id == BODY_CONSTRAINT_VECTOR_Z)
   {
      bodyConstraintVector(2) = value;
      return bodyConstraintVector(2);
   }
   
   //------------------------------------
   // cosine matrix parameters
   //------------------------------------
   
   if ((id >= DCM_11) && (id <= DCM_33))
   {
      // check validity of input, throw exception if not
      if ((value < -1.0) || (value > 1.0))
      {
         // @TODO convert id to string & replace "DCMi"
         AttitudeException ae("");
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                       GmatStringUtil::ToString(value, 16).c_str(),
                       "DCMij", "-1.0 <= Real Number <= 1.0");
         throw ae;
      }
      
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      needsReinit = false;
      
      // set the proper array element
      if (id == DCM_11)
         dcm(0,0) = value;
      
      if (id == DCM_12)
         dcm(0,1) = value;
      
      if (id == DCM_13)
         dcm(0,2) = value;
      
      if (id == DCM_21)
         dcm(1,0) = value;
      
      if (id == DCM_22)
         dcm(1,1) = value;
      
      if (id == DCM_23)
         dcm(1,2) = value;
      
      if (id == DCM_31)
         dcm(2,0) = value;
      
      if (id == DCM_32)
         dcm(2,1) = value;
      
      if (id == DCM_33)
         dcm(2,2) = value;
      
      // only compute quaternion if dcm is orthonormal
      if ((id >= DCM_11) && (id <= DCM_33))
      
         if (dcm.IsOrthonormal(GmatAttitudeConstants::DCM_ORTHONORMALITY_TOLERANCE))
         {
            quaternion = AttitudeConversionUtility::ToQuaternion(dcm);
#ifdef DEBUG_CNP
            MessageInterface::ShowMessage
            ("CNP::SetRealParameter, has orthonormal matrix, computed quaternion\n");
            
#endif
         }
      
      return value;
   }

#ifdef DEBUG_CNP
   MessageInterface::ShowMessage
      ("CNP::SetRealParameter, drops through and calls Attitude, id = %d, value = %f\n",
       id, value);
#endif
   return Attitude::SetRealParameter(id,value);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 */
//------------------------------------------------------------------------------
Real CommandableNadirPointing::SetRealParameter(const std::string &label,
                                const Real        value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets a value on a string parameter given the input parameter ID,
 * and the value.
 *
 * @param id ID for the requested parameter.
 * @param value The new string.
  *
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool CommandableNadirPointing::SetStringParameter(const Integer     id,
                                  const std::string &value)
{
   if (id == ATTITUDE_REFERENCE_BODY)
   {
      refBodyName = value;
      return true;
   }
   if (id == ATTITUDE_CONSTRAINT_TYPE)
   {
      if ((value != "OrbitNormal") && (value != "Velocity"))
      {
         AttitudeException ae;
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
            value.c_str(), GetParameterText(id).c_str(),
            "\"OrbitNormal\" \"Velocity\"");
         throw ae;
      }
      attitudeConstraintType = value;
      return true;
   }
   return Attitude::SetStringParameter(id, value);
   
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets a value on a string parameter given the input parameter
 * label, and the value.
 *
 * @param label label for the requested parameter.
 * @param value The new string.
  *
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool CommandableNadirPointing::SetStringParameter(const std::string &label,
                                  const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

// The next two parameter functions are part of logic for
// setting quaternion during mission sequence

//------------------------------------------------------------------------------
//  const Rvector&  GetRvectorParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * This method gets the Rvector parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  the Rvector parameter (Angles are returned in degrees).
 *
 */
//------------------------------------------------------------------------------
const Rvector& CommandableNadirPointing::SetRvectorParameter(const Integer id,
                                             const Rvector &value)
{
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage (
      "Entering SetRvectorParameter() in CommandableNadirPointing\n");
#endif
   
   if (id == QUATERNION)
   {
#ifdef DEBUG_CNP
      for (int i=0;i<4;i++)
         MessageInterface::ShowMessage(" input value(%d)=%f\n", i, value(i));
#endif
      
      // would want to call Attitude::ValidateQuaternion(value) here;  we
      // can't because the function is private private, not protected
      for (int i=0;i<4;i++)
         quaternion(i) = value(i);
      quaternion.Normalize();
      
      dcm = AttitudeConversionUtility::ToCosineMatrix(quaternion);
      needsReinit = false;
#ifdef DEBUG_CNP
      MessageInterface::ShowMessage ("Quaternion has been set to:\n");
      for (int i=0;i<4;i++)
         MessageInterface::ShowMessage(" q(%d)=%f\n", i, quaternion(i));
      MessageInterface::ShowMessage ("isInitialized=%s, needsReinit=%s\n",
         (isInitialized? "true": "false"), (needsReinit? "true": "false"));
#endif
      return quaternion;
   }
   else
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage (
      "CommandableNadirPointing::SetRvectorParameter() calling Attitude \n");
#endif
      return Attitude::SetRvectorParameter(id,value);
}

//------------------------------------------------------------------------------
//  const Rmatrix&  SetRmatrixParameter(const Integer id, const Rmatrix& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rmatrix parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Rmatrix value for the requested parameter.
 *
 * @return  the Rmatrix parameter value.
 */
//------------------------------------------------------------------------------
const Rmatrix& CommandableNadirPointing::SetRmatrixParameter(
                                             const Integer id, const Rmatrix &value)
{
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage (
      "Entering SetMatrixParameter() in CommandableNadirPointing\n");
#endif
   if (id == DIRECTION_COSINE_MATRIX)
   {
      Integer r,c;
      value.GetSize(r,c);
      if ((r != 3) || (c != 3))
         throw AttitudeException(
         "Incorrectly sized Rmatrix passed in for direction cosine matrix.");
      
      // would want to call ValidateCosineMatrix(value) here,
      // but that function is private, not protected
      
      for (Integer i = 0; i < 3; i++)
         for (Integer j = 0; j < 3; j++)
            dcm(i,j) = value(i,j);
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      quaternion = AttitudeConversionUtility::ToQuaternion(dcm);
      needsReinit = false;
      
      return dcm;
   }
   return Attitude::SetRmatrixParameter(id,value);
}

//---------------------------------------------------------------------------
//  bool IsParameterCommandModeSettable()
//---------------------------------------------------------------------------

bool CommandableNadirPointing::IsParameterCommandModeSettable(const Integer id) const
{
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage (
      "Entering IsCommandModeSettable() in CommandableNadirPointing");
#endif
   
   // later may add to "if" condition to allow DCM and other types to change
   // for now do what's needed for GOES.

   // attitude directly defined
   if (id == QUATERNION || id == DIRECTION_COSINE_MATRIX)
   {
#ifdef DEBUG_CNP
      MessageInterface::ShowMessage ("Quaternion or dcm is commmand mode settable");
#endif
      return true;
   }

   // attitude defined by alignment & constraint vectors
   if ( (id >= BODY_ALIGNMENT_VECTOR_X )   &&
        (id <= BODY_CONSTRAINT_VECTOR_Z)  )
   {
      return true;
   }
// later may add more to allow other parameters to change
// for now do what's needed for GOES.
      
   // kick up the inheritance chain
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
 * It is the rotation matrix from Inertial frame to body frame
 *
 * @param atTime the A1Mjd time at which to compute the attitude.
 *
 * @note This method will update the CosineMatrix parameter of the class.
 */
void CommandableNadirPointing::ComputeAttitudeFromCartesianVectors(Real atTime)
{
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage ("*** Entering ComputeAttitudeFromCartesianVectors\n");
   MessageInterface::ShowMessage ("... atTime = %12.5f\n", atTime);
#endif
   // local variables
   Rvector3 pos, vel, nadir, normal; // computed from orbit parameters
   Rvector3 xhat, yhat, zhat; // body axes expressed in inertial frame
   
   // compute orbit-related parameters
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage("... now calling ComputePostionAndVelocity()\n");
#endif
   ComputePositionAndVelocity(atTime, pos, vel);
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage("... returning from ComputePostionAndVelocity()\n");
#endif
   nadir = - pos / pos.GetMagnitude();
   normal = Cross(pos,vel);
   normal = normal / normal.GetMagnitude();
#ifdef DEBUG_CNP_COMPUTE
   MessageInterface::ShowMessage("Nadir  = %s\n",
                                 nadir.ToString().c_str());
   MessageInterface::ShowMessage("Normal = %s\n",
                                 normal.ToString().c_str());
   MessageInterface::ShowMessage("Align = %s\n",
                                 bodyAlignmentVector.ToString().c_str());
   MessageInterface::ShowMessage("Constraint = %s\n",
                                 bodyConstraintVector.ToString().c_str());
   MessageInterface::ShowMessage("...ready to compute matrix rows\n");
#endif
   
   // compute body axis vectors expressed in inertial frame
   if (abs(bodyAlignmentVector(2)) == 1) // nadir aligns with +z or -z
   {
      if (bodyAlignmentVector(2) == 1)
         zhat = nadir;
      else
         zhat = -nadir;
      
      // now for the constraints
      if (abs(bodyConstraintVector(0)) == 1) //normal aligns with +x or -x
      {
         // not needed for GOES
      }
      
      if (abs(bodyConstraintVector(1)) == 1) //normal aligns with +y or -y
      {
         if (bodyConstraintVector(1) == 1)
            yhat = normal;
         else
            yhat = -normal;
         xhat = Cross(yhat,zhat);
      }
   }
   
   if (abs(bodyAlignmentVector(0)) == 1) // nadir aligns with +x or -x
   {
      // add after testing z-axis alignment
      // needed for pitch +90 or pitch -90 mode
   }
   
   if (abs(bodyAlignmentVector(1)) == 1) // nadir aligns with +x or -x
   {
      // add last, not needed for GOES
   }
   
   // now compute DCM
#ifdef DEBUG_CNP_COMPUTE
   MessageInterface::ShowMessage("... loading dcm rows with xhat, yhat, zhat\n");
   MessageInterface::ShowMessage("xhat  = %s\n",
                                 xhat.ToString().c_str());
   MessageInterface::ShowMessage("yhat = %s\n",
                                 yhat.ToString().c_str());
   MessageInterface::ShowMessage("zhat = %s\n",
                                 zhat.ToString().c_str());
#endif

   for (int j = 0; j < 3; j++)
   {
      dcm (0,j) = xhat(j);
      dcm (1,j) = yhat(j);
      dcm (2,j) = zhat(j);
   }
#ifdef DEBUG_CNP
   MessageInterface::ShowMessage
      ("... Exiting ComputeAttitudeFromCartesianVectors()\n");
#endif
}
//------------------------------------------------------------------------------
//  virtual void ComputePositionAndVelocity(Real atTime, Rvector3& pos, Rvector3& vel)
//------------------------------------------------------------------------------
/**
 * This method computes the current CosineMatrix at the input time atTime.
 * It is the rotation matrix from Inertial frame to body frame
 *
 * @param atTime the A1Mjd time at which to compute the attitude.
 * @param pos        the computed position vector
 * @param vel        the computed velocity vector
 *
 * @note This method will update the CosineMatrix parameter of the class.
 * @TODO -- THIS REALLY SHOULD RETURN Rmatrix33& -- but this is unused by GOES, so do later
 */
void CommandableNadirPointing::ComputePositionAndVelocity
   (Real atTime, Rvector3& pos, Rvector3& vel)
{
   /// using a spacecraft object, *owningSC
   A1Mjd theTime(atTime);

   Rvector6 scState = ((SpaceObject*) owningSC)->GetMJ2000State(theTime);
   Rvector6 refState = refBody->GetMJ2000State(theTime);

   //Rvector3 pos(scState[0] - refState[0], scState[1] - refState[1],  scState[2] - refState[2]);
   //Rvector3 vel(scState[3] - refState[3], scState[4] - refState[4],  scState[5] - refState[5]);
   
   pos(0) = scState[0] - refState[0];
   pos(1) = scState[1] - refState[1];
   pos(2) = scState[2] - refState[2];
   
   vel(0) = scState[3] - refState[3];
   vel(1) = scState[4] - refState[4];
   vel(2) = scState[5] - refState[5];

   #ifdef DEBUG_CNP_COMPUTE
      MessageInterface::ShowMessage("   scState  = %s\n", scState.ToString().c_str());
      MessageInterface::ShowMessage("   refState = %s\n", refState.ToString().c_str());
      MessageInterface::ShowMessage("   pos      = %s\n", pos.ToString().c_str());
      MessageInterface::ShowMessage("   vel      = %s\n", vel.ToString().c_str());
   #endif
}

//------------------------------------------------------
//  virtual void ComputeReferenceAttitude(Real atTime) |
//------------------------------------------------------
/**
 * This method computes the current standard nadir pointing at the input time atTime.
 * It is the rotation matrix from Inertial frame to body frame
 * the body z is aligned with the nadir and the body y with the negative orbit normal
 *
 * @param atTime the A1Mjd time at which to compute the attitude.
 *
 * @note if needed, add notes.
 */

void CommandableNadirPointing::ComputeReferenceAttitude(Real atTime)
{
   // this will ultimately play a role in
   #ifdef DEBUG_CNP_COMPUTE
      MessageInterface::ShowMessage
         ("In CommandableNadirPointing, computing at time %le\n", atTime);
      MessageInterface::ShowMessage("attitudeConstraintType = %s\n", attitudeConstraintType.c_str());
      if (!owningSC) MessageInterface::ShowMessage("--- owningSC is NULL!!!!\n");
      if (!refBody) MessageInterface::ShowMessage("--- refBody is NULL!!!!\n");
   #endif

   if (!isInitialized || needsReinit)
   {
      Initialize();
   }

   // Need to do check for unset reference body here since the needsReinit
   // flag may not have been set
   if (!refBody)
   {
      std::string attEx  = "Reference body ";
      attEx             += refBodyName + " not defined for attitude of type \"";
      attEx             += "CommandableNadirPointing\" on spacecraft \"";
      attEx             += owningSC->GetName() + "\".";
      throw AttitudeException(attEx);
   }

   /// using a spacecraft object, *owningSC
   A1Mjd theTime(atTime);

   Rvector6 scState = ((SpaceObject*) owningSC)->GetMJ2000State(theTime);
   Rvector6 refState = refBody->GetMJ2000State(theTime);

   Rvector3 pos(scState[0] - refState[0], scState[1] - refState[1],  scState[2] - refState[2]);
   Rvector3 vel(scState[3] - refState[3], scState[4] - refState[4],  scState[5] - refState[5]);

   #ifdef DEBUG_CNP_COMPUTE
      MessageInterface::ShowMessage("   scState  = %s\n", scState.ToString().c_str());
      MessageInterface::ShowMessage("   refState = %s\n", refState.ToString().c_str());
      MessageInterface::ShowMessage("   pos      = %s\n", pos.ToString().c_str());
      MessageInterface::ShowMessage("   vel      = %s\n", vel.ToString().c_str());
   #endif

   // Error message
   std::string attErrorMsg = "Nadir Pointing attitude model is singular and/or ";
   attErrorMsg            += "undefined for Spacecraft \"";
   attErrorMsg            += owningSC->GetName() + "\".";
   if ((bodyAlignmentVector.GetMagnitude() < 1.0e-5 )  ||
       (bodyConstraintVector.GetMagnitude() < 1.0e-5)  ||
       (bodyAlignmentVector.GetUnitVector()*
             bodyConstraintVector.GetUnitVector()
             > ( 1.0 - 1.0e-5))                        ||
       (pos.GetMagnitude() < 1.0e-5)                   ||
       (vel.GetMagnitude() < 1.0e-5)                   ||
       (Cross(pos,vel).GetMagnitude() < 1.0e-5)        )
   {
      throw AttitudeException(attErrorMsg);
   }
   
   Rvector3 normal = Cross(pos,vel);

// @TODO -- create z as nadir, y as -normal
   Rvector3 xhat = pos/pos.GetMagnitude();
   Rvector3 yhat = normal/normal.GetMagnitude();
   Rvector3 zhat = Cross(xhat,yhat);

 
   #ifdef DEBUG_CNP_COMPUTE
      MessageInterface::ShowMessage("   refBody = <%s>\n", refBody->GetName().c_str());
      MessageInterface::ShowMessage("   normal  = %s\n", normal.ToString().c_str());
      MessageInterface::ShowMessage("   xhat    = %s\n", xhat.ToString().c_str());
      MessageInterface::ShowMessage("   yhat    = %s\n", yhat.ToString().c_str());
      MessageInterface::ShowMessage("   zhat    = %s\n", zhat.ToString().c_str());
   #endif

   // compute inertial to rotation matrix
   Rmatrix33 R_ri;
   for (int j= 0; j < 3; j++)
   {
      R_ri(1,j) = xhat(j);
      R_ri(2,j) = yhat(j);
      R_ri(1,j) = zhat(j);
   }
   #ifdef DEBUG_CNP_COMPUTE
      MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("   Inertial to reference DCM (R_ri) = \n   %s\n", R_ri.ToString().c_str());
   #endif

   //return R_ri;
   // this function should really return a Rmatrix33& @TODO when it's needed

}
//------------------------------------------------------------------------------
//  virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime)
//------------------------------------------------------------------------------
/**
 * This method computes the current CosineMatrix at the input time atTime.
 * It is the rotation matrix from Inertial frame to body frame
 *
 * @param atTime the A1Mjd time at which to compute the attitude.
 *
 * @note This method will update the CosineMatrix parameter of the class.
 */
/*
 void CommandableNadirPointing::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   // for now, we'll do the direct computation of attitude
   ComputeAttitudeFromCartesianVectors(atTime);
   
   // we will at some point add the option to model decomposition into
   // R_bi = R_br * R_ri
}
 */

//-------------------------------------
//-------------------------------------
//-------------------------------------
//-------------------------------------
//-------------------------------------
//-------------------------------------
//-------------------------------------
//-------------------------------------

//------------------------------------------------------------------------------
//  virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime)
//------------------------------------------------------------------------------
/**
* This method computes the current CosineMatrix at the input time atTime.
* It is the rotation matrix from Inertial frame to body frame
*
* @param atTime the GmatTime time at which to compute the attitude.
*
* @note This method will update the CosineMatrix parameter of the class.
*/
//------------------------------------------------------------------------------
void CommandableNadirPointing::ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime)
{

#ifdef DEBUG_CNP_COMPUTE
   MessageInterface::ShowMessage("In NadirPointing, computing at time %s\n",
      atTime.ToString().c_str());
   MessageInterface::ShowMessage("attitudeConstraintType = %s\n", attitudeConstraintType.c_str());
   if (!owningSC) MessageInterface::ShowMessage("--- owningSC is NULL!!!!\n");
   if (!refBody) MessageInterface::ShowMessage("--- refBody is NULL!!!!\n");
#endif

   if (!isInitialized || needsReinit)
   {
      Initialize();
   }

   // Need to do check for unset reference body here since the needsReinit
   // flag may not have been set
   if (!refBody)
   {
      std::string attEx = "Reference body ";
      attEx += refBodyName + " not defined for attitude of type \"";
      attEx += "NadirPointing\" on spacecraft \"";
      attEx += owningSC->GetName() + "\".";
      throw AttitudeException(attEx);
   }

   /// using a spacecraft object, *owningSC
   GmatTime theTime = atTime;

   Rvector6 scState = ((SpaceObject*)owningSC)->GetMJ2000State(theTime);
   Rvector6 refState = refBody->GetMJ2000State(theTime);

   Rvector3 pos(scState[0] - refState[0], scState[1] - refState[1], scState[2] - refState[2]);
   Rvector3 vel(scState[3] - refState[3], scState[4] - refState[4], scState[5] - refState[5]);

#ifdef DEBUG_CNP_COMPUTE
   MessageInterface::ShowMessage("   scState  = %s\n", scState.ToString().c_str());
   MessageInterface::ShowMessage("   refState = %s\n", refState.ToString().c_str());
   MessageInterface::ShowMessage("   pos      = %s\n", pos.ToString().c_str());
   MessageInterface::ShowMessage("   vel      = %s\n", vel.ToString().c_str());
#endif

   // Error message
   std::string attErrorMsg = "Nadir Pointing attitude model is singular and/or ";
   attErrorMsg += "undefined for Spacecraft \"";
   attErrorMsg += owningSC->GetName() + "\".";
   if ((bodyAlignmentVector.GetMagnitude() < 1.0e-5) ||
      (bodyConstraintVector.GetMagnitude() < 1.0e-5) ||
      (bodyAlignmentVector.GetUnitVector()*
         bodyConstraintVector.GetUnitVector()
      > (1.0 - 1.0e-5)) ||
         (pos.GetMagnitude() < 1.0e-5) ||
      (vel.GetMagnitude() < 1.0e-5) ||
      (Cross(pos, vel).GetMagnitude() < 1.0e-5))
   {
      throw AttitudeException(attErrorMsg);
   }

   Rvector3 normal = Cross(pos, vel);

   Rvector3 xhat = pos / pos.GetMagnitude();
   Rvector3 yhat = normal / normal.GetMagnitude();
   Rvector3 zhat = Cross(xhat, yhat);

   Rvector3 referenceVector;
   Rvector3 constraintVector;


   #ifdef DEBUG_CNP_COMPUTE
   MessageInterface::ShowMessage("   refBody = <%s>\n", refBody->GetName().c_str());
   MessageInterface::ShowMessage("   normal  = %s\n", normal.ToString().c_str());
   MessageInterface::ShowMessage("   xhat    = %s\n", xhat.ToString().c_str());
   MessageInterface::ShowMessage("   yhat    = %s\n", yhat.ToString().c_str());
   MessageInterface::ShowMessage("   zhat    = %s\n", zhat.ToString().c_str());
   #endif

   // RiI calculation (from inertial to LVLH)
   Rmatrix33 RIi;
   RIi(0, 0) = xhat(0);
   RIi(1, 0) = xhat(1);
   RIi(2, 0) = xhat(2);
   RIi(0, 1) = yhat(0);
   RIi(1, 1) = yhat(1);
   RIi(2, 1) = yhat(2);
   RIi(0, 2) = zhat(0);
   RIi(1, 2) = zhat(1);
   RIi(2, 2) = zhat(2);

   // Set alignment/constraint vector in body frame
   if (attitudeConstraintType == "OrbitNormal")
   {
      referenceVector[0] = -1;
      referenceVector[1] = 0;
      referenceVector[2] = 0;

      constraintVector[0] = 0;
      constraintVector[1] = 1;
      constraintVector[2] = 0;
   }
   else if (attitudeConstraintType == "Velocity")
   {
      referenceVector[0] = -1;
      referenceVector[1] = 0;
      referenceVector[2] = 0;

      constraintVector[0] = 0;
      constraintVector[1] = 0;
      constraintVector[2] = -1;
   }

   // RBi calculation using TRIAD (from LVLH to body frame)
   Rmatrix33 RiB = TRIAD(bodyAlignmentVector, bodyConstraintVector, referenceVector, constraintVector);

   // the rotation matrix (from body to inertial)
   dcm = RIi * RiB;
   // the final rotation matrix (from inertial to body)
   dcm = dcm.Transpose();

   // We do NOT calculate angular velocity for Nadir.
   // TODO : Throw AttitudeException?? write warning - but what to return?
   /*
   Rmatrix33 wxIBB = - RBi*RiIDot*dcm.Transpose();
   Rvector3 wIBB;
   wIBB.Set(wxIBB(2,1),wxIBB(0,2),wxIBB(1,0));
   */
}


//-------------------------------------------------------------------------------------------------
// std::vector<Rmatrix33> GetRotationMatrixDerivative(GmatTime &epochGT, CoordinateSystem *j2kCS)
//-------------------------------------------------------------------------------------------------
/**
* Get d[M]T/dX derivative of rotation matrix from spacecraft's attitude frame (B-frame) to inertial
* frame (I-frame) at a given time.
*
* @epochGT   epoch at which rotation matrix is specified.
*
* @return    rotation matrix from spaceraft's attitude frame (B-frame) to inertial frame (I-frame)
*/
//-------------------------------------------------------------------------------------------------
std::vector<Rmatrix33> CommandableNadirPointing::GetRotationMatrixDerivative(GmatTime &atTime, CoordinateSystem *j2kCS)
{
#ifdef DEBUG_ROTATION_MATRIX_DERIVATIVE
   MessageInterface::ShowMessage("In CommandableNadirPointing, computing at time %s\n",
      atTime.ToString());
   MessageInterface::ShowMessage("attitudeConstraintType = %s\n", attitudeConstraintType.c_str());
   if (!owningSC) MessageInterface::ShowMessage("--- owningSC is NULL!!!!\n");
   if (!refBody) MessageInterface::ShowMessage("--- refBody is NULL!!!!\n");
#endif

   if (!isInitialized || needsReinit)
   {
      Initialize();
   }

   // Need to do check for unset reference body here since the needsReinit
   // flag may not have been set
   if (!refBody)
   {
      std::string attEx = "Reference body ";
      attEx += refBodyName + " not defined for attitude of type \"";
      attEx += "NadirPointing\" on spacecraft \"";
      attEx += owningSC->GetName() + "\".";
      throw AttitudeException(attEx);
   }

   /// using a spacecraft object, *owningSC
   GmatTime theTime = atTime;

   Rvector6 scState = ((SpaceObject*)owningSC)->GetMJ2000State(theTime);
   Rvector6 refState = refBody->GetMJ2000State(theTime);

   Rvector3 pos(scState[0] - refState[0], scState[1] - refState[1], scState[2] - refState[2]);
   Rvector3 vel(scState[3] - refState[3], scState[4] - refState[4], scState[5] - refState[5]);
   
#ifdef DEBUG_ROTATION_MATRIX_DERIVATIVE
   MessageInterface::ShowMessage("   scState  = %s\n", scState.ToString().c_str());
   MessageInterface::ShowMessage("   refState = %s\n", refState.ToString().c_str());
   MessageInterface::ShowMessage("   pos      = %s\n", pos.ToString().c_str());
   MessageInterface::ShowMessage("   vel      = %s\n", vel.ToString().c_str());
#endif

   // Error message
   std::string attErrorMsg = "Nadir Pointing attitude model is singular and/or ";
   attErrorMsg += "undefined for Spacecraft \"";
   attErrorMsg += owningSC->GetName() + "\".";
   if ((bodyAlignmentVector.GetMagnitude() < 1.0e-5) ||
      (bodyConstraintVector.GetMagnitude() < 1.0e-5) ||
      (bodyAlignmentVector.GetUnitVector()*
         bodyConstraintVector.GetUnitVector()
      > (1.0 - 1.0e-5)) ||
         (pos.GetMagnitude() < 1.0e-5) ||
      (vel.GetMagnitude() < 1.0e-5) ||
      (Cross(pos, vel).GetMagnitude() < 1.0e-5))
   {
      throw AttitudeException(attErrorMsg);
   }

   Rvector3 normal = Cross(pos, vel);

   Real     posMag = pos.GetMagnitude();
   Rvector3 xhat = pos / posMag;
   Rvector3 yhat = normal / normal.GetMagnitude();
   Rvector3 zhat = Cross(xhat, yhat);

#ifdef DEBUG_ROTATION_MATRIX_DERIVATIVE
   MessageInterface::ShowMessage("   refBody = <%s>\n", refBody->GetName().c_str());
   MessageInterface::ShowMessage("   normal  = %s\n", normal.ToString().c_str());
   MessageInterface::ShowMessage("   xhat    = %s\n", xhat.ToString().c_str());
   MessageInterface::ShowMessage("   yhat    = %s\n", yhat.ToString().c_str());
   MessageInterface::ShowMessage("   zhat    = %s\n", zhat.ToString().c_str());
#endif

   Rmatrix33 Zero(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   Rmatrix33 I(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
   
   // dRiI/dX calculation (from inertial to LVLH)
   /// Calculate dxHat/dr and dxHat/dv
   Rmatrix33 dxHatdr = (I - Outerproduct(xhat, xhat))/ posMag;
   Rmatrix33 dxHatdv = Zero;
   
   /// Calculate dvHat/dv, where: vHat = (v-vref)/|v-vref|
   Real velMag = vel.GetMagnitude();
   Rvector3 vhat = vel / velMag;
   Rmatrix33 dvHatdv = (I - Outerproduct(vhat, vhat)) / velMag;
   
   /// Calculate dyHat/dr = [dyHat/du]*[du/dr]  where: u = xhat x vhat
   ///           dyHat/du = (I-uHat*uHatT)/|u|
   Rvector3 u = Cross(xhat,vhat);
   Real uMag = u.GetMagnitude();
   Rvector uHat = u / uMag;
   Rmatrix33 dyHatdu = (I - Outerproduct(uHat, uHat)) / uMag;
   
   /// du/dr = dxhat/dr x vhat
   Rvector3 vec1, vec2;
   Rmatrix33 dudr;
   for (Integer col = 0; col < 3; ++col)
   {
      for (Integer row = 0; row < 3; ++row)
         vec1[row] = dxHatdr(row, col);
      vec2 = Cross(vec1, vhat);
      for (Integer row = 0; row < 3; ++row)
         dudr(row, col) = vec2[row];
   }
   Rmatrix dyHatdr = dyHatdu * dudr;

   /// Calculate dyHat/dv = [dyHat/du]*[du/dv]  where: u = xhat x vhat
   
   /// du/dv = xhat x dvhat/dv
   Rmatrix33 dudv;
   for (Integer col = 0; col < 3; ++col)
   {
      for (Integer row = 0; row < 3; ++row)
         vec1[row] = dvHatdv(row, col);
      vec2 = Cross(xhat, vec1);
      for (Integer row = 0; row < 3; ++row)
         dudv(row, col) = vec2[row];
   }
   Rmatrix33 dyHatdv = dyHatdu * dudv;

   /// Calculate dzHat/dr = d(xHat x yHat)/dr = [dxHat/dr] x yHat + xHat x [dyHat/dr]
   Rmatrix33 dzHatdr;
   Rvector3 vec3, vec4;
   for (Integer col = 0; col < 3; ++col)
   {
      for (Integer row = 0; row < 3; ++row)
      {
         vec1[row] = dxHatdr(row, col);
         vec2[row] = dyHatdr(row, col);
      }
      vec3 = Cross(vec1, yhat);
      vec4 = Cross(xhat, vec2);

      for (Integer row = 0; row < 3; ++row)
         dzHatdr(row, col) = vec3[row] + vec4[row];
   }
   
   /// Calculate dzHat/dv = d(xHat x yHat)/dv = [dxHat/dv] x yHat + xHat x [dyHat/dv]
   Rmatrix33 dzHatdv;
   for (Integer col = 0; col < 3; ++col)
   {
      for (Integer row = 0; row < 3; ++row)
      {
         vec1[row] = dxHatdv(row, col);
         vec2[row] = dyHatdv(row, col);
      }
      vec3 = Cross(vec1, yhat);
      vec4 = Cross(xhat, vec2);

      for (Integer row = 0; row < 3; ++row)
         dzHatdv(row, col) = vec3[row] + vec4[row];
   }
   
   Rvector3 referenceVector;
   Rvector3 constraintVector;

   // Set alignment/constraint vector in body frame
   if (attitudeConstraintType == "OrbitNormal")
   {
      referenceVector[0] = -1;
      referenceVector[1] = 0;
      referenceVector[2] = 0;

      constraintVector[0] = 0;
      constraintVector[1] = 1;
      constraintVector[2] = 0;
   }
   else if (attitudeConstraintType == "Velocity")
   {
      referenceVector[0] = -1;
      referenceVector[1] = 0;
      referenceVector[2] = 0;

      constraintVector[0] = 0;
      constraintVector[1] = 0;
      constraintVector[2] = -1;
   }
   
   // RBi calculation using TRIAD (from LVLH to body frame)
   Rmatrix33 RiB = TRIAD(bodyAlignmentVector, bodyConstraintVector, referenceVector, constraintVector);

   std::vector<Rmatrix33> MTDeriv;
   MTDeriv.push_back(dxHatdr*RiB);
   MTDeriv.push_back(dyHatdr*RiB);
   MTDeriv.push_back(dzHatdr*RiB);
   MTDeriv.push_back(dxHatdv*RiB);
   MTDeriv.push_back(dyHatdv*RiB);
   MTDeriv.push_back(dzHatdv*RiB);
   
   return MTDeriv;
}

//------------------------------------------------------------------------------
//  virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime)
//------------------------------------------------------------------------------
/**
 * This method computes the current CosineMatrix at the input time atTime.
 * It is the rotation matrix from Inertial frame to body frame
 *
 * @param atTime the A1Mjd time at which to compute the attitude.
 *
 * @note This method will update the CosineMatrix parameter of the class.
 */
//------------------------------------------------------------------------------


 void CommandableNadirPointing::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   #ifdef DEBUG_CNP_COMPUTE
      MessageInterface::ShowMessage
         ("In CommandableNadirPointing, computing at time %le\n", atTime);
      MessageInterface::ShowMessage("attitudeConstraintType = %s\n", attitudeConstraintType.c_str());
      if (!owningSC) MessageInterface::ShowMessage("--- owningSC is NULL!!!!\n");
      if (!refBody) MessageInterface::ShowMessage("--- refBody is NULL!!!!\n");
   #endif

   if (!isInitialized || needsReinit)
   {
      Initialize();
   }

   // Need to do check for unset reference body here since the needsReinit
   // flag may not have been set
   if (!refBody)
   {
      std::string attEx  = "Reference body ";
      attEx             += refBodyName + " not defined for attitude of type \"";
      attEx             += "CommandableNadirPointing\" on spacecraft \"";
      attEx             += owningSC->GetName() + "\".";
      throw AttitudeException(attEx);
   }

   /// using a spacecraft object, *owningSC
   A1Mjd theTime(atTime);

   Rvector6 scState = ((SpaceObject*) owningSC)->GetMJ2000State(theTime);
   Rvector6 refState = refBody->GetMJ2000State(theTime);

   Rvector3 pos(scState[0] - refState[0], scState[1] - refState[1],  scState[2] - refState[2]);
   Rvector3 vel(scState[3] - refState[3], scState[4] - refState[4],  scState[5] - refState[5]);

   #ifdef DEBUG_CNP_COMPUTE
      MessageInterface::ShowMessage("   scState  = %s\n", scState.ToString().c_str());
      MessageInterface::ShowMessage("   refState = %s\n", refState.ToString().c_str());
      MessageInterface::ShowMessage("   pos      = %s\n", pos.ToString().c_str());
      MessageInterface::ShowMessage("   vel      = %s\n", vel.ToString().c_str());
   #endif

   // Error message
   std::string attErrorMsg = "Nadir Pointing attitude model is singular and/or ";
   attErrorMsg            += "undefined for Spacecraft \"";
   attErrorMsg            += owningSC->GetName() + "\".";
   if ((bodyAlignmentVector.GetMagnitude() < 1.0e-5 )  ||
       (bodyConstraintVector.GetMagnitude() < 1.0e-5)  ||
       (bodyAlignmentVector.GetUnitVector()*
             bodyConstraintVector.GetUnitVector()
             > ( 1.0 - 1.0e-5))                        ||
       (pos.GetMagnitude() < 1.0e-5)                   ||
       (vel.GetMagnitude() < 1.0e-5)                   ||
       (Cross(pos,vel).GetMagnitude() < 1.0e-5)        )
   {
      throw AttitudeException(attErrorMsg);
   }
   
 
 ////////// @TODO change to proper +Z nadir & negative orbit normal
   Rvector3 normal = Cross(pos,vel);
   Rvector3 xhat = pos/pos.GetMagnitude();
   Rvector3 yhat = normal/normal.GetMagnitude();
   Rvector3 zhat = Cross(xhat,yhat);
    
    Rvector3 referenceVector, constraintVector;

   #ifdef DEBUG_CNP_COMPUTE
      MessageInterface::ShowMessage("   refBody = <%s>\n", refBody->GetName().c_str());
      MessageInterface::ShowMessage("   normal  = %s\n", normal.ToString().c_str());
      MessageInterface::ShowMessage("   xhat    = %s\n", xhat.ToString().c_str());
      MessageInterface::ShowMessage("   yhat    = %s\n", yhat.ToString().c_str());
      MessageInterface::ShowMessage("   zhat    = %s\n", zhat.ToString().c_str());
   #endif

 ////////////// @TODO xhat, yhat & zhat are ROWS of A_RI
   // RiI calculation (from inertial to LVLH)
   Rmatrix33 RIi;
   RIi(0,0) = xhat(0);
   RIi(1,0) = xhat(1);
   RIi(2,0) = xhat(2);
   RIi(0,1) = yhat(0);
   RIi(1,1) = yhat(1);
   RIi(2,1) = yhat(2);
   RIi(0,2) = zhat(0);
   RIi(1,2) = zhat(1);
   RIi(2,2) = zhat(2);
   
   // Set alignment/constraint vector in body frame
   if ( attitudeConstraintType == "OrbitNormal" )
   {
      referenceVector[0] = -1;
      referenceVector[1] = 0;
      referenceVector[2] = 0;

      constraintVector[0] = 0;
      constraintVector[1] = 1;
      constraintVector[2] = 0;
   }
   else if ( attitudeConstraintType == "Velocity" )
   {
      referenceVector[0] = -1;
      referenceVector[1] = 0;
      referenceVector[2] = 0;

      constraintVector[0] = 0;
      constraintVector[1] = 0;
      constraintVector[2] = -1;
   }

   // RBi calculation using TRIAD (from LVLH to body frame)
   Rmatrix33 RiB = TRIAD( bodyAlignmentVector, bodyConstraintVector, referenceVector, constraintVector );

   // the rotation matrix (from body to inertial)
   dcm = RIi*RiB;
   // the final rotation matrix (from inertial to body)
   dcm =  dcm.Transpose();

   // We do NOT calculate angular velocity for Nadir.
   // TODO : Throw AttitudeException?? write warning - but what to return?
   /*
   Rmatrix33 wxIBB = - RBi*RiIDot*dcm.Transpose();
   Rvector3 wIBB;
   wIBB.Set(wxIBB(2,1),wxIBB(0,2),wxIBB(1,0));
   */
}

//------------------------------------------------------------------------------
//  virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime)
//------------------------------------------------------------------------------
/**
* This method computes the current CosineMatrix at the input time atTime.
* It is the rotation matrix from Inertial frame to body frame
*
* @param atTime the GmatTime time at which to compute the attitude.
*
* @note This method will update the CosineMatrix parameter of the class.
*/
//------------------------------------------------------------------------------

/*
void CommandableNadirPointing::ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime)
{
#ifdef DEBUG_CNP_COMPUTE
   MessageInterface::ShowMessage("In NadirPointing, computing at time %s\n",
      atTime.ToString().c_str());
   MessageInterface::ShowMessage("attitudeConstraintType = %s\n", attitudeConstraintType.c_str());
   if (!owningSC) MessageInterface::ShowMessage("--- owningSC is NULL!!!!\n");
   if (!refBody) MessageInterface::ShowMessage("--- refBody is NULL!!!!\n");
#endif

   if (!isInitialized || needsReinit)
   {
      Initialize();
   }

   // Need to do check for unset reference body here since the needsReinit
   // flag may not have been set
   if (!refBody)
   {
      std::string attEx = "Reference body ";
      attEx += refBodyName + " not defined for attitude of type \"";
      attEx += "NadirPointing\" on spacecraft \"";
      attEx += owningSC->GetName() + "\".";
      throw AttitudeException(attEx);
   }

   /// using a spacecraft object, *owningSC
   GmatTime theTime = atTime;

   Rvector6 scState = ((SpaceObject*)owningSC)->GetMJ2000State(theTime);
   Rvector6 refState = refBody->GetMJ2000State(theTime);

   Rvector3 pos(scState[0] - refState[0], scState[1] - refState[1], scState[2] - refState[2]);
   Rvector3 vel(scState[3] - refState[3], scState[4] - refState[4], scState[5] - refState[5]);

#ifdef DEBUG_CNP_COMPUTE
   MessageInterface::ShowMessage("   scState  = %s\n", scState.ToString().c_str());
   MessageInterface::ShowMessage("   refState = %s\n", refState.ToString().c_str());
   MessageInterface::ShowMessage("   pos      = %s\n", pos.ToString().c_str());
   MessageInterface::ShowMessage("   vel      = %s\n", vel.ToString().c_str());
#endif

   // Error message
   std::string attErrorMsg = "Nadir Pointing attitude model is singular and/or ";
   attErrorMsg += "undefined for Spacecraft \"";
   attErrorMsg += owningSC->GetName() + "\".";
   if ((bodyAlignmentVector.GetMagnitude() < 1.0e-5) ||
      (bodyConstraintVector.GetMagnitude() < 1.0e-5) ||
      (bodyAlignmentVector.GetUnitVector()*
         bodyConstraintVector.GetUnitVector()
      > (1.0 - 1.0e-5)) ||
         (pos.GetMagnitude() < 1.0e-5) ||
      (vel.GetMagnitude() < 1.0e-5) ||
      (Cross(pos, vel).GetMagnitude() < 1.0e-5))
   {
      throw AttitudeException(attErrorMsg);
   }

   Rvector3 normal = Cross(pos, vel);

   Rvector3 xhat = pos / pos.GetMagnitude();
   Rvector3 yhat = normal / normal.GetMagnitude();
   Rvector3 zhat = Cross(xhat, yhat);

   Rvector3 referenceVector;
   Rvector3 constraintVector;


   #ifdef DEBUG_CNP_COMPUTE
   MessageInterface::ShowMessage("   refBody = <%s>\n", refBody->GetName().c_str());
   MessageInterface::ShowMessage("   normal  = %s\n", normal.ToString().c_str());
   MessageInterface::ShowMessage("   xhat    = %s\n", xhat.ToString().c_str());
   MessageInterface::ShowMessage("   yhat    = %s\n", yhat.ToString().c_str());
   MessageInterface::ShowMessage("   zhat    = %s\n", zhat.ToString().c_str());
   #endif

   // RiI calculation (from inertial to LVLH)
   Rmatrix33 RIi;
   RIi(0, 0) = xhat(0);
   RIi(1, 0) = xhat(1);
   RIi(2, 0) = xhat(2);
   RIi(0, 1) = yhat(0);
   RIi(1, 1) = yhat(1);
   RIi(2, 1) = yhat(2);
   RIi(0, 2) = zhat(0);
   RIi(1, 2) = zhat(1);
   RIi(2, 2) = zhat(2);

   // Set alignment/constraint vector in body frame
   if (attitudeConstraintType == "OrbitNormal")
   {
      referenceVector[0] = -1;
      referenceVector[1] = 0;
      referenceVector[2] = 0;

      constraintVector[0] = 0;
      constraintVector[1] = 1;
      constraintVector[2] = 0;
   }
   else if (attitudeConstraintType == "Velocity")
   {
      referenceVector[0] = -1;
      referenceVector[1] = 0;
      referenceVector[2] = 0;

      constraintVector[0] = 0;
      constraintVector[1] = 0;
      constraintVector[2] = -1;
   }

   // RBi calculation using TRIAD (from LVLH to body frame)
   Rmatrix33 RiB = TRIAD(bodyAlignmentVector, bodyConstraintVector, referenceVector, constraintVector);

   // the rotation matrix (from body to inertial)
   dcm = RIi * RiB;
   // the final rotation matrix (from inertial to body)
   dcm = dcm.Transpose();

   // We do NOT calculate angular velocity for Nadir.
   // TODO : Throw AttitudeException?? write warning - but what to return?
   Rmatrix33 wxIBB = - RBi*RiIDot*dcm.Transpose();
   Rvector3 wIBB;
   wIBB.Set(wxIBB(2,1),wxIBB(0,2),wxIBB(1,0));
}
*/


//------------------------------------------------------------------------------
//  Rmatrix33 TRIAD(Rvector3& V1, Rvector3& V2, Rvector3& W1, Rvector3& W2)
//------------------------------------------------------------------------------

Rmatrix33 CommandableNadirPointing::TRIAD(Rvector3& V1, Rvector3& V2, Rvector3& W1, Rvector3& W2)
{
   // V1, V2 : defined in frame A
   // W1, W2 : defined in frame B
   // TRIAD algorithm calculates the rotation matrix from A to B

   Real v1Mag = V1.GetMagnitude();
   if (v1Mag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   Rvector3 r1 = V1 / v1Mag;
   Rvector3 temp = Cross(V1,V2);
   Real tempMag = temp.GetMagnitude();
   if (tempMag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   Rvector3 r2 = temp / tempMag;
   Rvector3 r3 = Cross(V1,temp);
   Real r3Mag = r3.GetMagnitude();
   if (r3Mag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   r3 = r3 / r3Mag;

   Rmatrix33 R(r1[0], r2[0], r3[0],
               r1[1], r2[1], r3[1],
               r1[2], r2[2], r3[2]);



   Real w1Mag = W1.GetMagnitude();
   if (w1Mag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   Rvector3 s1 = W1 / w1Mag;
   temp = Cross(W1,W2);
   Real temp2Mag = temp.GetMagnitude();
   if (temp2Mag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   Rvector3 s2 = temp/temp2Mag;
   Rvector3 s3 = Cross(W1,temp);
   Real s3Mag = s3.GetMagnitude();
   if (s3Mag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   s3 = s3/s3Mag;

   Rmatrix33 S(s1[0], s2[0], s3[0],
               s1[1], s2[1], s3[1],
               s1[2], s2[2], s3[2]);

   // YRL, calculate resultRotMatrix
   Rmatrix33 resultRotMatrix = Outerproduct(s1,r1) + Outerproduct(s2,r2) + Outerproduct(s3,r3) ;
   //Rmatrix33 resultRotMatrix = R*S.Transpose();
   return resultRotMatrix;
}

