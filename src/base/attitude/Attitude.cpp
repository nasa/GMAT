//$Id$
//------------------------------------------------------------------------------
//                               Attitude
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Developed further jointly by NASA/GSFC, Thinking Systems, Inc., and 
// Schafer Corp., under AFRL NOVA Contract #FA945104D03990003
//
// Author:   Wendy C. Shoan/GSFC
// Created:  2006.03.02
// Modified: Dunn Idle/Schafer - Added Modified Rodrigues Parameters
//                               Cleaned up code, added comments, improved
//                               variable names for clarity.
// Date:     2010.08.15 
//
/**
 * Implementation for the Attitude base class.
 * 
 * @note  The attitude is stored and maintained as a direction cosine matrix,
 *        and the angular velocity.  Other representations are computed from 
 *        that and returned when requested.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Attitude.hpp"
#include "AttitudeException.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "GmatConstants.hpp"
#include "AttitudeConversionUtility.hpp"
#include "CoordinateConverter.hpp"


//#define DEBUG_REF_SETTING
//#define DEBUG_ATTITUDE_GEN_STRING
//#define DEBUG_ATTITUDE_GET_REAL
//#define DEBUG_ATTITUDE_GET
//#define DEBUG_ATTITUDE_GET_COSMAT
//#define DEBUG_ATTITUDE_SET_REAL
//#define DEBUG_ATTITUDE_SET
//#define DEBUG_ATTITUDE_READ_ONLY
//#define DEBUG_EULER_ANGLE_RATES
//#define DEBUG_TO_DCM
//#define DEBUG_ATTITUDE_PARAM_TYPE
//#define DEBUG_ATTITUDE_INIT
//#define DEBUG_UPDATE_STATE
//#define DEBUG_SET_RATE
//#define DEBUG_ANG_VEL

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string
Attitude::PARAMETER_TEXT[AttitudeParamCount - GmatBaseParamCount] =
{
   "AttitudeDisplayStateType",
   "AttitudeRateDisplayStateType",
   "AttitudeCoordinateSystem",
   "Epoch",
   "Q1",
   "Q2",
   "Q3",
   "Q4",
   "EulerAngleSequence",
   "EulerAngle1",
   "EulerAngle2",
   "EulerAngle3",
   "DCM11",
   "DCM12",
   "DCM13",
   "DCM21",
   "DCM22",
   "DCM23",
   "DCM31",
   "DCM32",
   "DCM33",
   "MRP1",  // Dunn Added
   "MRP2",  // Dunn Added
   "MRP3",  // Dunn Added
   "EulerAngleRate1",
   "EulerAngleRate2",
   "EulerAngleRate3",
   "AngularVelocityX",
   "AngularVelocityY",
   "AngularVelocityZ",
   // Additional CSFixed field text here
   // none at this time
   // Additional Spinner field text here
   // none at this time
   // Additional SpiceAttitude field text here
   // none at this time
   // Additional PrecessingSpinner field text here
   "NutationReferenceVectorX",
   "NutationReferenceVectorY",
   "NutationReferenceVectorZ",
   "BodySpinAxisX",
   "BodySpinAxisY",
   "BodySpinAxisZ",
   "InitialPrecessionAngle",
   "PrecessionRate",
   "NutationAngle",
   "InitialSpinAngle",
   "SpinRate",
   // Additional NadirPointing field text here
   "AttitudeReferenceBody",
   "AttitudeConstraintType",
   "BodyAlignmentVectorX",
   "BodyAlignmentVectorY",
   "BodyAlignmentVectorZ",
   "BodyConstraintVectorX",
   "BodyConstraintVectorY",
   "BodyConstraintVectorZ",
   // Addtitional CCSDS-AEM field text here
   "AttitudeFileName",
};

const Gmat::ParameterType
Attitude::PARAMETER_TYPE[AttitudeParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,  // Dunn Added
   Gmat::REAL_TYPE,  // Dunn Added
   Gmat::REAL_TYPE,  // Dunn Added
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   // Additional CSFixed field types here
   // none at this time
   // Additional Spinner field types here
   // none at this time
   // Additional SpiceAttitude field types here
   // none at this time
   // Additional PrecessingSpinner field types here
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   // Additional NadirPointing field types here
   Gmat::OBJECT_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   // Additional CCSDS-AEM field types here
   Gmat::STRING_TYPE,
};

const std::string
Attitude::OTHER_REP_TEXT[EndOtherReps - 7000] =
{
   "EulerSequenceList",
   "EulerSequenceArray",     // was "InitialEulerSequence",
   "EulerAngles",            // was "InitialEulerAngles",
   "EulerAngleRates",        // was "InitialEulerAngleRates",
   "Quaternion",             // was "InitialQuaternion",
   "MRPs",                   // Dunn Added
   "DirectionCosineMatrix",  // was "InitialDirectionCosineMatrix",
   "AngularVelocity",        // was "InitialAngularVelocity",
   "BodySpinAxis",           // PrecessingSpinner
   "NutationReferenceVector",// PrecessingSpinner
};

const Gmat::ParameterType
Attitude::OTHER_REP_TYPE[EndOtherReps - 7000] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::UNSIGNED_INTARRAY_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,  // Dunn Added
   Gmat::RMATRIX_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
};

const std::string Attitude::EULER_SEQ_LIST[12] = 
{
   "123",
   "231",
   "312",
   "132",
   "321",
   "213",
   "121",
   "232",
   "313",
   "131",
   "323",
   "212"
};

// Mode of Constraint values for NadirPointing
const std::string Attitude::ATTITUDE_CONSTRAINT_TYPE_LIST[2] =
{
      "OrbitNormal",
      "Velocity",
};

const Real     Attitude::TESTACCURACY                 = 1.19209290E-07;
const Real     Attitude::ATTITUDE_TIME_TOLERANCE      = 1.0E-09;
const Integer  Attitude::OTHER_REPS_OFFSET            = 7000;


//------------------------------------------------------------------------------
// static methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  StringArray GetEulerSequenceStrings()     [static]
//------------------------------------------------------------------------------
/**
 * This method returns the Euler Sequence as an array of strings.
 *
 * @return  array of strings for the Euler Sequence
 */
//------------------------------------------------------------------------------
StringArray Attitude::GetEulerSequenceStrings()
{
   static StringArray eulerStrings;  // compiler didn't like this in the header
   if (eulerStrings.size() == 0)  // set it, if it hasn't been already
   {
      for (Integer i = 0; i < 12; i++)
         eulerStrings.push_back(EULER_SEQ_LIST[i]);
   }
   return eulerStrings;
}

//------------------------------------------------------------------------------
//  StringArray GetAttitudeConstraintTypes()     [static]
//------------------------------------------------------------------------------
/**
 * This method returns the possible values for Mode of Constraint (Nadir
 * Pointing only)
 *
 * @return  array of strings for the Mode of Constraint
 */
//------------------------------------------------------------------------------
StringArray Attitude::GetAttitudeConstraintTypes()
{
   static StringArray constraintTypeStrings;  // compiler doesn't like this in the header
   if (constraintTypeStrings.size() == 0)  // set it, if it hasn't been already
   {
      for (Integer i = 0; i < 2; i++)
         constraintTypeStrings.push_back(ATTITUDE_CONSTRAINT_TYPE_LIST[i]);
   }
   return constraintTypeStrings;
}

//------------------------------------------------------------------------------
//  UnsignedIntArray  ExtractEulerSequence(const std::string &seqStr)   [static]
//------------------------------------------------------------------------------
/**
 * This method sets the state data for the euler sequence array, given
 * the input string representation of the euler sequence.
 *
 * @param <eulerArray>  string representation of euler sequence.
 *
 * @return  array of Unsigned integers representing the Euler Sequence
 *
 */
//------------------------------------------------------------------------------
UnsignedIntArray Attitude::ExtractEulerSequence(const std::string &seqStr)
{
   UnsignedIntArray intSeq;
   const char *tmpStr = seqStr.c_str();
   if (tmpStr[0] == '1')      intSeq.push_back(1);
   else if (tmpStr[0] == '2') intSeq.push_back(2);
   else if (tmpStr[0] == '3') intSeq.push_back(3);
   else
   throw AttitudeException("Invalid character in euler sequence string.");
   if (tmpStr[1] == '1')      intSeq.push_back(1);
   else if (tmpStr[1] == '2') intSeq.push_back(2);
   else if (tmpStr[1] == '3') intSeq.push_back(3);
   else
   throw AttitudeException("Invalid character in euler sequence string.");
   if (tmpStr[2] == '1')      intSeq.push_back(1);
   else if (tmpStr[2] == '2') intSeq.push_back(2);
   else if (tmpStr[2] == '3') intSeq.push_back(3);
   else
   throw AttitudeException("Invalid character in euler sequence string.");
   return intSeq;
}

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Attitude(const std::string &typeStr, const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Attitude class (Constructor).
 * The default value is the (0,0,0,1) quaternion.
 */
//------------------------------------------------------------------------------
Attitude::Attitude(const std::string &typeStr, const std::string &itsName) :
   GmatBase(Gmat::ATTITUDE, typeStr, itsName),
   inputAttitudeType       (GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE),
   inputAttitudeRateType   (GmatAttitude::ANGULAR_VELOCITY_TYPE),
   attitudeDisplayType     ("Quaternion"),
   attitudeRateDisplayType ("AngularVelocity"),
   isInitialized           (false),
   needsReinit             (false),
   epoch                   (0.0),
   epochGT                 (0.0),
   owningSC                (NULL),
   refCSName               ("EarthMJ2000Eq"),
   refCS                   (NULL),
   eulerSequence           ("321"),  // Dunn Changed from 312 to 321
   RBi                     (Rmatrix33(true)),
   dcm                     (Rmatrix33(true)),
   attitudeTime            (0.0),
   attitudeTimeGT          (0.0),
   quaternion              (Rvector(4,0.0,0.0,0.0,1.0)),
   attitudeModelName       (""),
   modifyCoordSysAllowed   (true),
   setInitialAttitudeAllowed (true),
   warnNoCSWritten         (false),
   warnNoAttitudeWritten   (false),
   modelComputesRates      (true),
   warnNoRatesWritten      (false),
   // Additional CSFixed fields here
   // none at this time
   // Additional Spinner fields here
   // none at this time
   // Additional SpiceAttitude fields here
   // none at this time
   // Additional PrecessingSpinner fields here
   initialPrecessionAngle  (0.0),
   precessionRate          (5.0 * GmatMathConstants::RAD_PER_DEG),
   nutationAngle           (15.0 * GmatMathConstants::RAD_PER_DEG),
   initialSpinAngle        (0.0),
   spinRate                (10.0 * GmatMathConstants::RAD_PER_DEG),
   // Additional NadirPointing fields here
   refBodyName             ("Earth"),
   refBody                 (NULL),
   attitudeConstraintType  ("OrbitNormal"),
   // Additional CCSDS-AEM fields here
   aemFile                 (""),
   aemFileFullPath         ("")
{
   parameterCount = AttitudeParamCount;
   objectTypes.push_back(Gmat::ATTITUDE);
   objectTypeNames.push_back("Attitude");

   // push the default sequence to the array
   unsigned int defSeq[3] = {3, 2, 1};  // Dunn Changed from 312 to 321
   eulerSequenceArray.push_back(defSeq[0]);
   eulerSequenceArray.push_back(defSeq[1]);
   eulerSequenceArray.push_back(defSeq[2]);
   // create the list of all the possible euler sequences
   for (Integer i = 0; i < 12; i++)
      eulerSequenceList.push_back(EULER_SEQ_LIST[i]);

   // Additional CSFixed fields here
   // none at this time
   // Additional Spinner fields here
   // none at this time
   // Additional SpiceAttitude fields here
   // none at this time
   // Additional PrecessingSpinner fields here
   nutationReferenceVector.Set(0.0,0.0,1.0);
   bodySpinAxis.Set(0.0,0.0,1.0);
   // Additional NadirPointing fields here
   bodyAlignmentVector.Set(1.0, 0.0, 0.0);
   bodyConstraintVector.Set(0.0, 0.0, 1.0);
 }
 
//------------------------------------------------------------------------------
//  Attitude(const Attitude &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Attitude class as a copy of the
 * specified Attitude class (copy constructor).
 *
 * @param <att> Attitude object to copy.
 */
//------------------------------------------------------------------------------
Attitude::Attitude(const Attitude& att) :
   GmatBase(att),
   inputAttitudeType       (att.inputAttitudeType),
   inputAttitudeRateType   (att.inputAttitudeRateType),
   attitudeDisplayType     (att.attitudeDisplayType),
   attitudeRateDisplayType (att.attitudeRateDisplayType),
   isInitialized           (att.isInitialized),
   needsReinit             (att.needsReinit),
   eulerSequenceList       (att.eulerSequenceList),
   epoch                   (att.epoch),
   epochGT                 (att.epochGT),
   owningSC                (NULL),
   refCSName               (att.refCSName),
   refCS                   (att.refCS),   // shouldn't this be NULL?
   eulerSequence           (att.eulerSequence),
   eulerSequenceArray      (att.eulerSequenceArray),
   RBi                     (att.RBi),
   wIBi                    (att.wIBi),
   dcm                     (att.dcm),
   angVel                  (att.angVel),
   attitudeTime            (att.attitudeTime),
   attitudeTimeGT          (att.attitudeTimeGT),
   quaternion              (att.quaternion),
   mrps                    (att.mrps),		   // Dunn Added
   eulerAngles             (att.eulerAngles),
   eulerAngleRates         (att.eulerAngleRates),
   attitudeModelName       (att.attitudeModelName),
   modifyCoordSysAllowed   (att.modifyCoordSysAllowed),
   setInitialAttitudeAllowed (att.setInitialAttitudeAllowed),
   warnNoCSWritten         (att.warnNoCSWritten),
   warnNoAttitudeWritten   (att.warnNoAttitudeWritten),
   modelComputesRates      (att.modelComputesRates),
   warnNoRatesWritten      (att.warnNoRatesWritten),
   // Additional CSFixed fields here
   // none at this time
   // Additional Spinner fields here
   // none at this time
   // Additional SpiceAttitude fields here
   // none at this time
   // Additional PrecessingSpinner fields here
   nutationReferenceVector (att.nutationReferenceVector),
   bodySpinAxis            (att.bodySpinAxis),
   initialPrecessionAngle  (att.initialPrecessionAngle),
   precessionRate          (att.precessionRate),
   nutationAngle           (att.nutationAngle),
   initialSpinAngle        (att.initialSpinAngle),
   spinRate                (att.spinRate),
   // Additional NadirPointing fields here
   refBodyName             (att.refBodyName),
   refBody                 (NULL),
   attitudeConstraintType  (att.attitudeConstraintType),
   bodyAlignmentVector     (att.bodyAlignmentVector),
   bodyConstraintVector    (att.bodyConstraintVector),
   // Additional CCSDS-AEM fields here
   aemFile                 (att.aemFile),
   aemFileFullPath         (att.aemFileFullPath)
{
#ifdef DEBUG_ATTITUDE_INIT
   MessageInterface::ShowMessage("New attitude created by copying attitude <%p> of type %s\n",
         &att, (att.typeName).c_str()); // **********************
   MessageInterface::ShowMessage("eulerAngleRates = %s\n", eulerAngleRates.ToString().c_str());
   MessageInterface::ShowMessage("att.eulerAngleRates = %s\n", (att.eulerAngleRates).ToString().c_str());
#endif
}
 
//------------------------------------------------------------------------------
//  Attitude& operator= (const Attitude& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Attitude class.
 *
 * @param att the Attitude object whose data to assign to "this"
 *            Attitude.
 *
 * @return "this" Attitude with data of input Attitude att.
 */
//------------------------------------------------------------------------------
Attitude& Attitude::operator=(const Attitude& att)
{
   GmatBase::operator=(att);
   inputAttitudeType       = att.inputAttitudeType;
   inputAttitudeRateType   = att.inputAttitudeRateType;
   attitudeDisplayType     = att.attitudeDisplayType;
   attitudeRateDisplayType = att.attitudeRateDisplayType;
   isInitialized           = false;
   needsReinit             = false;
   eulerSequenceList       = att.eulerSequenceList;
   epoch                   = att.epoch;
   epochGT                 = att.epochGT;
   owningSC                = att.owningSC;
   refCSName               = att.refCSName;
   refCS                   = att.refCS;
   eulerSequence           = att.eulerSequence;
   eulerSequenceArray      = att.eulerSequenceArray;
   RBi                     = att.RBi;
   wIBi                    = att.wIBi;
   dcm                     = att.dcm;
   angVel                  = att.angVel;
   attitudeTime            = att.attitudeTime;
   attitudeTimeGT          = att.attitudeTimeGT;
   quaternion              = att.quaternion;
   mrps                    = att.mrps;			// Dunn Added
   eulerAngles             = att.eulerAngles;
   eulerAngleRates         = att.eulerAngleRates;
   attitudeModelName       = att.attitudeModelName;  // need this to stay what it was
   modifyCoordSysAllowed   = att.modifyCoordSysAllowed;
   setInitialAttitudeAllowed = att.setInitialAttitudeAllowed;
   warnNoCSWritten         = att.warnNoCSWritten;
   warnNoAttitudeWritten   = att.warnNoAttitudeWritten;
   modelComputesRates      = att.modelComputesRates;
   warnNoRatesWritten      = att.warnNoRatesWritten;
   // Additional CSFixed fields here
   // none at this time
   // Additional Spinner fields here
   // none at this time
   // Additional SpiceAttitude fields here
   // none at this time
   // Additional PrecessingSpinner fields here
   nutationReferenceVector = att.nutationReferenceVector;
   bodySpinAxis            = att.bodySpinAxis;
   initialPrecessionAngle  = att.initialPrecessionAngle;
   precessionRate          = att.precessionRate;
   nutationAngle           = att.nutationAngle;
   initialSpinAngle        = att.initialSpinAngle;
   spinRate                = att.spinRate;
   // Additional NadirPointing fields here
   refBodyName             = att.refBodyName;
   refBody                 = att.refBody;
   attitudeConstraintType  = att.attitudeConstraintType;
   bodyAlignmentVector     = att.bodyAlignmentVector;
   bodyConstraintVector    = att.bodyConstraintVector;
   // Additional CCSDS-AEM fields here
   aemFile                 = att.aemFile;
   aemFileFullPath         = att.aemFileFullPath;

   return *this;
}


//------------------------------------------------------------------------------
//  ~Attitude()
//------------------------------------------------------------------------------
/**
 * Destructor for the Attitude class.
 */
//------------------------------------------------------------------------------
Attitude::~Attitude()
{
   eulerSequenceArray.clear();
   // nothing else to do here ... la la la la la
}

//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
/**
 * Checks to be sure that initial attitude settings do not lead to a
 * singularity and are not disallowed
 *
 * @return true if the settings are correct, throws an AttitudeException if not.
 */
//------------------------------------------------------------------------------
bool Attitude::Validate()
{
   // only need to check if setting the initial attitude is allowed for this
   // attitude model type
   if (setInitialAttitudeAllowed)
   {
      switch (inputAttitudeType)
      {
         case GmatAttitude::QUATERNION_TYPE:
            ValidateQuaternion(quaternion);
            break;
         case GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE:
            ValidateCosineMatrix(dcm);
            break;
         case GmatAttitude::MODIFIED_RODRIGUES_PARAMETERS_TYPE:
            ValidateMRPs(mrps);
            break;
         case GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE:
            ValidateEulerSequence(eulerSequence);
            ValidateEulerAngles(eulerAngles, eulerSequenceArray);
            break;
         default:
            break;
      };

      switch (inputAttitudeRateType)
      {
         case GmatAttitude::ANGULAR_VELOCITY_TYPE:
            break;
         case GmatAttitude::EULER_ANGLE_RATES_TYPE:
            (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
            ValidateEulerSequence(eulerSequence);
            ValidateEulerAngles(eulerAngles, eulerSequenceArray);
            angVel = AttitudeConversionUtility::ToAngularVelocity(eulerAngleRates,
                               eulerAngles,
                               (Integer) eulerSequenceArray.at(0),
                               (Integer) eulerSequenceArray.at(1),
                               (Integer) eulerSequenceArray.at(2));
         default:
            break;
      }
   }

   return true;
}



//---------------------------------------------------------------------------
//  bool Initialize() 
//---------------------------------------------------------------------------
 /**
 * Initializes the attitude, including computing the initial RBi and
 * wIBi data members, as needed.
 * 
 * @return Success flag.
 *
 */
//---------------------------------------------------------------------------
bool Attitude::Initialize()
{
   #ifdef DEBUG_ATTITUDE_INIT
   MessageInterface::ShowMessage
      ("Attitude::Initialize() this=<%p>'%s' entered, refCS=<%p>\n",
       this, GetName().c_str(), refCS);
   MessageInterface::ShowMessage
      ("Attitude::Initialize() isInitialized=%s, needsReinit=%s\n",
       (isInitialized? "true": "false"), (needsReinit? "true": "false"));
   #endif
   
   if (isInitialized && !needsReinit) return true;
   GmatBase::Initialize();
   if (modifyCoordSysAllowed && (!refCS))
   {
      std::string attEx  = "Reference coordinate system ";
      attEx             += refCSName + " not defined for attitude of type \"";
      attEx             += typeName + "\"";
      throw AttitudeException(attEx);
   }
    
   // compute cosine matrix and angular velocity from inputs and synchronize all 
   // the values at the epoch time; dcm and angVel are kept up-to-date
   // Validate the input values, if this is an attitude model that uses those
   // inputs
   if (setInitialAttitudeAllowed)
   {
      switch (inputAttitudeType)
      {
         case GmatAttitude::QUATERNION_TYPE:
            quaternion.Normalize();
            dcm = AttitudeConversionUtility::ToCosineMatrix(quaternion);
            break;
         case GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE:
            ValidateCosineMatrix(dcm);
            break;
         // Dunn added case below
         case GmatAttitude::MODIFIED_RODRIGUES_PARAMETERS_TYPE:
            ValidateMRPs(mrps);
            quaternion = AttitudeConversionUtility::ToQuaternion(mrps);
            dcm = AttitudeConversionUtility::ToCosineMatrix(quaternion);
            break;
         case GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE:
            ValidateEulerSequence(eulerSequence);
            ValidateEulerAngles(eulerAngles, eulerSequenceArray);
            dcm = AttitudeConversionUtility::ToCosineMatrix(eulerAngles,
                            (Integer) eulerSequenceArray.at(0),
                            (Integer) eulerSequenceArray.at(1),
                            (Integer) eulerSequenceArray.at(2));
            break;
         default:
            break;
      };

      switch (inputAttitudeRateType)
      {
         case GmatAttitude::ANGULAR_VELOCITY_TYPE:
            break;
         case GmatAttitude::EULER_ANGLE_RATES_TYPE:
            (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
            ValidateEulerSequence(eulerSequence);
            angVel = AttitudeConversionUtility::ToAngularVelocity(eulerAngleRates,
                               eulerAngles,
                               (Integer) eulerSequenceArray.at(0),
                               (Integer) eulerSequenceArray.at(1),
                               (Integer) eulerSequenceArray.at(2));
         default:
            break;
      }
   }

   RBi          = dcm;
   wIBi         = angVel;
   attitudeTime = epoch;
   attitudeTimeGT = epochGT;

   // Set flags and values to indicate that the initialization has taken place, and that the
   // DCM and angVel are the current representations (the ones kept up-to-date)
   isInitialized         = true;
   needsReinit           = false;
   inputAttitudeType     = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
   inputAttitudeRateType = GmatAttitude::ANGULAR_VELOCITY_TYPE;

   #ifdef DEBUG_ATTITUDE_INIT
   MessageInterface::ShowMessage
      ("Attitude::Initialize() EXITing\n");
   #endif
   return true;
}

//---------------------------------------------------------------------------
//  const Real GetEpoch() const
//---------------------------------------------------------------------------
 /**
 * Returns the epoch for the attitude.
 * 
 * @return the epoch for this attitude.
 *
  */
//---------------------------------------------------------------------------
Real Attitude::GetEpoch() const
{
   return epoch;
}


GmatTime Attitude::GetEpochGT() const
{
   return epochGT;
}

//---------------------------------------------------------------------------
//  void SetEpoch(Real toEpoch)
//---------------------------------------------------------------------------
 /**
 * Sets the epoch for the attitude.
 * 
 * @param toEpoch the epoch to use for this attitude.
 *
 */
//---------------------------------------------------------------------------
void Attitude::SetEpoch(Real toEpoch)
{
   epoch         = toEpoch; // need to reinitialize
   needsReinit   = true;
}


void Attitude::SetEpochGT(GmatTime toEpoch)
{
   epochGT = toEpoch; // need to reinitialize
   needsReinit = true;
}

//---------------------------------------------------------------------------
//  void NeedsReinitialization()
//---------------------------------------------------------------------------
 /**
 * Sets the flag indicating that the Attitude needs to be re-initialized.
 *
 *
 */
//---------------------------------------------------------------------------
void Attitude::NeedsReinitialization()
{
   needsReinit = true;
}

void Attitude::SetOwningSpacecraft(GmatBase *theSC)
{
   if (theSC->IsOfType("Spacecraft"))
      owningSC = theSC;
   else
   {
      throw AttitudeException(
            "ERROR setting the owning spacecraft on attitude\n");
   }
}


////---------------------------------------------------------------------------
////  bool    SetReferenceCoordinateSystemName(
////          const std::string &refName) const
////---------------------------------------------------------------------------
// /**
// * sets the name of the reference coordinate system for the Attitude object.
// *
// * @param refName name of the reference coordinate system.
// *
// * @return success flag.
// */
////---------------------------------------------------------------------------
//bool Attitude::SetReferenceCoordinateSystemName(const std::string &refName)
//{
//   refCSName     = refName;
//   needsReinit   = true;
//   return true;
//}

//---------------------------------------------------------------------------
//  const StringArray&   GetEulerSequenceList() const
//---------------------------------------------------------------------------
 /**
 * Returns the list of valid euler sequences, as a vector of strings.
 *
 * @return the list of valid euler sequences.  
 */
//---------------------------------------------------------------------------
const StringArray& Attitude::GetEulerSequenceList() const
{
   return eulerSequenceList;
}

 
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
const Rvector&   Attitude::GetQuaternion(Real atTime)
{
   if (!isInitialized || needsReinit) Initialize();
//   if (GmatMathUtil::Abs(atTime - attitudeTime) > ATTITUDE_TIME_TOLERANCE)
//   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
//   }
   quaternion       = AttitudeConversionUtility::ToQuaternion(dcm);
   return quaternion;
}

//---------------------------------------------------------------------------
//  const Rvector3&   GetEulerAngles(Real tTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as an array of Euler angles.  This
 * method assumes the previously set Euler sequence.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the euler angle representation of the attitude, computed at
 *         time atTime (radians).  
 */
//---------------------------------------------------------------------------
const Rvector3&  Attitude::GetEulerAngles(Real atTime)
{
   if (!isInitialized || needsReinit)
   {
      #ifdef DEBUG_ATT_GET_EULER_ANGLES
         MessageInterface::ShowMessage("In GetEulerAngles, about to call Initialize\n");
      #endif
      Initialize();
   }
   #ifdef DEBUG_ATT_GET_EULER_ANGLES
   else
   {
      MessageInterface::ShowMessage("In GetEulerAngles, NOT about to call Initialize\n");
   }
   #endif

//   if (GmatMathUtil::Abs(atTime - attitudeTime) >
//       ATTITUDE_TIME_TOLERANCE)
//   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
//   }

   eulerAngles = AttitudeConversionUtility::ToEulerAngles(dcm,
                           (Integer) eulerSequenceArray.at(0),
                           (Integer) eulerSequenceArray.at(1),
                           (Integer) eulerSequenceArray.at(2));
   return eulerAngles;
}

//---------------------------------------------------------------------------
//  const Rvector3&   GetEulerAngles(Real atTime,  
//                                   Integer seq1, Integer seq2, 
//                                   Integer seq3)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as an array of Euler angles.  This
 * method uses the Euler sequence passed in here.
 *
 * @param atTime  time at which to compute the attitude.
 * @param seq1    euler Sequence 1
 * @param seq2    euler sequence 2
 * @param seq3    euler sequence 3
 *
 * @return the euler angle representation of the attitude, computed at
 *         time atTime (radians).  
 * @todo - figure out what to do if the time is the same, but the 
 *         Euler sequence is different that the last time it was computed.
 */
//---------------------------------------------------------------------------
const Rvector3&  Attitude::GetEulerAngles(Real atTime,  Integer seq1, 
                                          Integer seq2, Integer seq3)
{
   if (!isInitialized || needsReinit) Initialize();
//   if (GmatMathUtil::Abs(atTime - attitudeTime) >
//       ATTITUDE_TIME_TOLERANCE)
//   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
//   }
   eulerAngles = AttitudeConversionUtility::ToEulerAngles(dcm, seq1, seq2, seq3);
   return eulerAngles;
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
const Rmatrix33& Attitude::GetCosineMatrix(Real atTime)
{
   #ifdef DEBUG_ATTITUDE_GET_COSMAT
      MessageInterface::ShowMessage("Entering Attitude::GetCosineMatrix ...\n");
      MessageInterface::ShowMessage(" ... atTime = %12.10f     attitudeTime = %12.10f\n",
            atTime, attitudeTime);
      MessageInterface::ShowMessage(" ... ATTITUDE_TIME_TOLERANCE = %12.10f     Abs(atTime - attitudeTime)  = %12.10f\n",
            ATTITUDE_TIME_TOLERANCE, GmatMathUtil::Abs(atTime - attitudeTime));
   #endif
   if (!isInitialized || needsReinit)
   {
      #ifdef DEBUG_ATTITUDE_GET_COSMAT
         MessageInterface::ShowMessage("In Attitude::GetCosineMatrix, Initializing ...\n");
      #endif
      Initialize();
   }
//   if (GmatMathUtil::Abs(atTime - attitudeTime) >
//       ATTITUDE_TIME_TOLERANCE)
//   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
//   }
   #ifdef DEBUG_ATTITUDE_GET_COSMAT
      MessageInterface::ShowMessage(" ... returning cosine matrix: %s\n",
            (dcm.ToString()).c_str());
   #endif

   return dcm;
}
 
//---------------------------------------------------------------------------
//  const Rvector3&   GetAngularVelocity(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude rates at time atTime as an angular velocity.
 *
 * @param atTime  time at which to compute the attitude rate.
 *
 * @return the angular velocity representation of the attitude rates, 
 *         computed at time atTime (radians/second).  
 */
//---------------------------------------------------------------------------
const Rvector3& Attitude::GetAngularVelocity(Real atTime)
{
   #ifdef DEBUG_ANG_VEL
      MessageInterface::ShowMessage("Entering GetAngVel for attitude of type %s\n",
            attitudeModelName.c_str());
      MessageInterface::ShowMessage("modelComputesRates = %s\n",
            (modelComputesRates? "true" : "false"));
   #endif
   if (!modelComputesRates)
   {
      std::string errMsg = "Attitude rates for spacecraft ";
      errMsg += owningSC->GetName() + " were requested, but attitude rates ";
      errMsg += "are not computed when attitude mode is " + attitudeModelName;
      errMsg += ".\n";
      throw AttitudeException(errMsg);
   }
   if (!isInitialized || needsReinit) Initialize();
//   if (GmatMathUtil::Abs(atTime - attitudeTime) >
//       ATTITUDE_TIME_TOLERANCE)
//   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
//   }
   return angVel;
}

//---------------------------------------------------------------------------
//  const Rvector3&   GetEulerAngleRates(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude rates at time atTime as an array of euler 
 * angle rates.
 *
 * @param atTime  time at which to compute the attitude rate.
 *
 * @return the euler angle rates representation of the attitude rates, 
 *         computed at time atTime (radians/second).  
 */
//---------------------------------------------------------------------------
const Rvector3& Attitude::GetEulerAngleRates(Real atTime)
{
   #ifdef DEBUG_EULER_ANGLE_RATES
   MessageInterface::ShowMessage("Entering Attitude::GetEulerAngleRates ...\n");
   MessageInterface::ShowMessage(
   "   with atTime = %.12f, and attitudeTime = %.12f\n",
   atTime, attitudeTime);
   #endif

   if (!modelComputesRates)
   {
      std::string errMsg = "Attitude rates for spacecraft ";
      errMsg += owningSC->GetName() + " were requested, but attitude rates ";
      errMsg += "are not computed when attitude mode is " + attitudeModelName;
      errMsg += ".\n";
      throw AttitudeException(errMsg);
   }

   if (!isInitialized || needsReinit) Initialize();
//   if (GmatMathUtil::Abs(atTime - attitudeTime) >
//       ATTITUDE_TIME_TOLERANCE)
//   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
//   }
   eulerAngles       = GetEulerAngles(atTime);
   eulerAngleRates = AttitudeConversionUtility::ToEulerAngleRates(angVel,
                               eulerAngles,
                               (Integer) eulerSequenceArray.at(0),
                               (Integer) eulerSequenceArray.at(1),
                               (Integer) eulerSequenceArray.at(2));
   return eulerAngleRates;
}


//---------------------------------------------------------------------------
//  std::string   GetAttitudeModelName()
//---------------------------------------------------------------------------
 /**
 * Returns the name of the attitude model.
 *
 * @return the name of the attitude model (e.g. "Spinner").  
 */
//---------------------------------------------------------------------------
std::string Attitude::GetAttitudeModelName() const
{
   return attitudeModelName;
}

//---------------------------------------------------------------------------
//  bool   CSModifyAllowed() const
//---------------------------------------------------------------------------
 /**
 * Returns a flag indicating whether or not modification of the reference
 * coordinate system is allowed for this attitude model.
 *
 * @return is modification of the CS allowed?
 */
//---------------------------------------------------------------------------
bool Attitude::CSModifyAllowed() const
{
   return modifyCoordSysAllowed;
}

//---------------------------------------------------------------------------
//  bool   SetInitialAttitudeAllowed() const
//---------------------------------------------------------------------------
 /**
 * Returns a flag indicating whether or not setting of the initial
 * attitude conditions is allowed for this attitude model.
 *
 * @return is setting of the attitude allowed?
 */
//---------------------------------------------------------------------------
bool Attitude::SetInitialAttitudeAllowed() const
{
   return setInitialAttitudeAllowed;
}

//---------------------------------------------------------------------------
//  bool   ModelComputesRates() const
//---------------------------------------------------------------------------
 /**
 * Returns a flag indicating whether or not this model computes attitude
 * rates.
 *
 * @return true if the model computes rates; false otherwise
 */
//---------------------------------------------------------------------------
bool Attitude::ModelComputesRates() const
{
   return modelComputesRates;
}

//------------------------------------------------------------------------------
//   std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Returns the name of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.  
 *
 * @return The name of the reference object.
 */
//------------------------------------------------------------------------------
std::string Attitude::GetRefObjectName(const UnsignedInt type) const
{
   #ifdef DEBUG_REF_SETTING
      MessageInterface::ShowMessage("Attitude::GetRefObjectName with type = %d\n",
            type);
   #endif

   if (type == Gmat::CELESTIAL_BODY)
   {
      return refBodyName;
   }

   if ((type == Gmat::UNKNOWN_OBJECT) ||
       (type == Gmat::COORDINATE_SYSTEM))
   {
       return refCSName;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObjectName(type);
}

const StringArray& Attitude::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();

   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::CELESTIAL_BODY)
   {
      refObjectNames.push_back(refBodyName);
   }

   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::COORDINATE_SYSTEM)
   {
      refObjectNames.push_back(refCSName);
   }
   return refObjectNames;
   //return refObjectNames; // previous ver
   return GmatBase::GetRefObjectNameArray(type);
}



//------------------------------------------------------------------------------
//   bool SetRefObjectName(const UnsignedInt type,
//                         const std::string &name) 
//------------------------------------------------------------------------------
/**
 * Sets the name of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param type   reference object type.  
 * @param name   name to use for the object
 *
 * @return Success flag.
 */
//------------------------------------------------------------------------------
bool Attitude::SetRefObjectName(const UnsignedInt type,
                                const std::string &name)
{
   if (type == Gmat::CELESTIAL_BODY)
   {
      #ifdef DEBUG_ATTITUDE_SET
         MessageInterface::ShowMessage("Attitude: Setting refBodyName to %s\n",
                                       name.c_str());
      #endif
      refBodyName = name;
      return true;
   }

   if ((type == Gmat::UNKNOWN_OBJECT) ||
       (type == Gmat::COORDINATE_SYSTEM))
   {
      #ifdef DEBUG_ATTITUDE_SET
         MessageInterface::ShowMessage("Attitude: Setting refCSName to %s\n",
                                       name.c_str());
      #endif
      refCSName = name;
      return true;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return GmatBase::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName,
//                       const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames a reference object. 
 *
 * @param <type>    reference object type.   
 * @param <oldName> the old name for the object.   
 * @param <newName> the new name for the object.  
 *
 * @return success flag
 */
//------------------------------------------------------------------------------
bool Attitude::RenameRefObject(const UnsignedInt type,
                               const std::string &oldName,
                               const std::string &newName)
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (refCSName == oldName)
      {
         refCSName = newName;
      }
   }
   if (type == Gmat::CELESTIAL_BODY)
   {
      if (refBodyName == oldName)
         refBodyName = newName;
   }

   return true;
}
                               
//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * Returns a pointer to a reference object with the input name. 
 *
 * @param <type>    reference object type.   
 * @param <name>    the name for the object.   
  *
 * @return Pointer to the object specified.
 */
//------------------------------------------------------------------------------
GmatBase* Attitude::GetRefObject(const UnsignedInt type,
                                 const std::string      &name)
{
   switch (type)
   {
      case Gmat::COORDINATE_SYSTEM:
         if (name == refCSName)           return refCS;
         break;
      case Gmat::CELESTIAL_BODY:
         if (name == refBodyName)         return refBody;
         break;
      default:
         break;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObject(type, name);
}
                                    
//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * Returns a pointer to a reference object with the input name. 
 *
 * @param obj     the object
 * @param type    reference object type.   
 * @param name    the name for the object.   
  *
 * @return Success flag.
 */
//------------------------------------------------------------------------------
bool Attitude::SetRefObject(GmatBase *obj,
                            const UnsignedInt type,
                            const std::string &name)
{
   #ifdef DEBUG_REF_SETTING
   MessageInterface::ShowMessage
      ("Attitude::SetRefObject() this=<%p>'%s' entered, refCSName='%s', name='%s', "
       "obj=<%p><%s>'%s'\n", this, GetName().c_str(), refCSName.c_str(),
       name.c_str(), obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL");
   #endif
   
   if (obj == NULL)
      return false;
   
   if (obj->IsOfType("CoordinateSystem"))
   {
      if (name == refCSName)
      {
         #ifdef DEBUG_REF_SETTING
         if (refCS != NULL)
            MessageInterface::ShowMessage(
               "   The current reference coordinate system for attitude is %s\n",
               (refCS->GetName()).c_str());
         else
            MessageInterface::ShowMessage("   refCS == NULL!\n");
         #endif
         if (refCS != (CoordinateSystem*) obj)
         {
            #ifdef DEBUG_REF_SETTING
               MessageInterface::ShowMessage(
                  "   Setting <%p>'%s' as reference coordinate system for attitude '%s'\n",
                  obj, name.c_str(), instanceName.c_str());
            #endif
            // Make sure we don't have a circular dependency
            CoordinateSystem *tmpCS = (CoordinateSystem*) obj;
            if (tmpCS->IsOfType("BodyFixed") &&
                tmpCS->UsesSpacecraft(owningSC->GetName()))
            {
               std::string errmsg = "Cannot set attitude coordinate system \"";
               errmsg += tmpCS->GetName() + "\" on spacecraft \"";
               errmsg += owningSC->GetName() + "\".  Body Fixed coordinate ";
               errmsg += "system contains ";
               errmsg += "circular reference to the spacecraft.\n";
               throw AttitudeException(errmsg);
            }
            refCS = (CoordinateSystem*) obj;
            needsReinit = true;  // need to reinitialize, since CS has changed
         }
      }
      #ifdef DEBUG_REF_SETTING
      MessageInterface::ShowMessage
         ("Attitude::SetRefObject() this=<%p>'%s' returning true\n", this,
          GetName().c_str());
      #endif
      return true;
   }
   if (obj->IsOfType("CelestialBody"))
   {
      if (name == refBodyName)
      {
         if (refBody != (CelestialBody*) obj)
         {
            refBody = (CelestialBody*) obj;
            #ifdef DEBUG_REF_SETTING
               MessageInterface::ShowMessage(
                     "Entering Attitude::SetRefObject setting refBody pointer to %s<%p>\n",
                     refBodyName.c_str(), obj);
            #endif
            needsReinit = true;  // need to reinitialize, since Reference Body has changed
         }
      }
      return true;
   }

   // Not handled here -- invoke the next higher SetRefObject call
   return GmatBase::SetRefObject(obj, type, name);
}

bool Attitude::HasRefObjectTypeArray()
{
   return true;
}

const ObjectTypeArray& Attitude::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::COORDINATE_SYSTEM);
   refObjectTypes.push_back(Gmat::CELESTIAL_BODY);

   return refObjectTypes;
}


// methods to get/set parameter values
//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Attitude::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < AttitudeParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else if (id >= OTHER_REPS_OFFSET && id < EndOtherReps)
      return OTHER_REP_TEXT[id - OTHER_REPS_OFFSET];
   return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer Attitude::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < AttitudeParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   // otherwise, check for other reps
   for (Integer i = OTHER_REPS_OFFSET; i < EndOtherReps; i++)
   {
      if (str == OTHER_REP_TEXT[i - OTHER_REPS_OFFSET])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Attitude::GetParameterType(const Integer id) const
{
   #ifdef DEBUG_ATTITUDE_PARAM_TYPE
      MessageInterface::ShowMessage("Asking for Attitude parameter type for parameter %d (%s)\n",
          id, (GetParameterText(id)).c_str());
      MessageInterface::ShowMessage("GmatBaseParamCount = %d, AttitudeParamCount = %d\n",
            GmatBaseParamCount, AttitudeParamCount);
      MessageInterface::ShowMessage("OTHER_REPS_OFFSET = %d, EndOtherReps = %d\n",
            OTHER_REPS_OFFSET, EndOtherReps);
   #endif
   if (id >= GmatBaseParamCount && id < AttitudeParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else if (id >= OTHER_REPS_OFFSET && id < EndOtherReps)
   {
      #ifdef DEBUG_ATTITUDE_PARAM_TYPE
         MessageInterface::ShowMessage("About to get type for ID = %d\n", (id - OTHER_REPS_OFFSET));
         MessageInterface::ShowMessage("   So returning type ID = %d\n",
               OTHER_REP_TYPE[id - OTHER_REPS_OFFSET]);
      #endif
      return OTHER_REP_TYPE[id - OTHER_REPS_OFFSET];
   }
   #ifdef DEBUG_ATTITUDE_PARAM_TYPE
      MessageInterface::ShowMessage("SHOULD NOT get to this code for a valid attitude ID!!!\n");
   #endif
      
   return GmatBase::GetParameterType(id);
}
            
//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Attitude::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
UnsignedInt Attitude::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case REFERENCE_COORDINATE_SYSTEM:
      return Gmat::COORDINATE_SYSTEM;
   case ATTITUDE_REFERENCE_BODY:
      return Gmat::CELESTIAL_BODY;
   default:
      return GmatBase::GetPropertyObjectType(id);
   }
}

//---------------------------------------------------------------------------
// bool CanAssignStringToObjectProperty(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not a string can be assigned to the
 * specified object property.
 *
 * @param <id> ID for the property.
 *
 * @return true, if a string can be assigned; false otherwise
 */
//---------------------------------------------------------------------------
bool Attitude::CanAssignStringToObjectProperty(const Integer id) const
{
   if (id == REFERENCE_COORDINATE_SYSTEM)
      return false;
   if (id == ATTITUDE_REFERENCE_BODY)
      return false;

   return GmatBase::CanAssignStringToObjectProperty(id);
}



//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not;
 *         throws an exception if the parameter is out of the valid range
 *         of values.
 */
//---------------------------------------------------------------------------
bool Attitude::IsParameterReadOnly(const Integer id) const
{
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   "Entering Attitude::ReadOnly (%s) with id = %d (%s)\n",
   attitudeModelName.c_str(), id,
   GetParameterText(id).c_str());
   #endif
   if ((!modifyCoordSysAllowed) && (id == REFERENCE_COORDINATE_SYSTEM))
      return true;

   if (!setInitialAttitudeAllowed && IsInitialAttitudeParameter(id,"Any"))
      return true;

   if (id == EULER_SEQUENCE_LIST)
      return true;
   if (id == EPOCH)
      return true;

   if (attitudeDisplayType == "Quaternion")
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly in quaternion section\n");
   #endif
      if ((id == EULER_ANGLE_1) || 
          (id == EULER_ANGLE_2) ||
          (id == EULER_ANGLE_3) || 
          (id == MRP_1)         || (id == MRP_2)    || (id == MRP_3)  || // Dunn Added
          (id == DCM_11)        || (id == DCM_12)   || (id == DCM_13) ||
          (id == DCM_21)        || (id == DCM_22)   || (id == DCM_23) || 
          (id == DCM_31)        || (id == DCM_32)   || (id == DCM_33))
         return true;
   }
   else if (attitudeDisplayType == "DirectionCosineMatrix")
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly in dcm section\n");
   #endif
      if ((id == Q_1) || (id == Q_2) || (id == Q_3) || (id == Q_4)    ||
          (id == MRP_1)         || (id == MRP_2)    || (id == MRP_3)  || // Dunn Added
          (id == EULER_ANGLE_1) || (id == EULER_ANGLE_2) ||
          (id == EULER_ANGLE_3))
         return true;
   }
   else if (attitudeDisplayType == "MRPs") // Dunn Added
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
         MessageInterface::ShowMessage(
            " ....... Attitude::ReadOnly in MRPs section\n");
   #endif
      if ((id == EULER_ANGLE_1) || 
          (id == EULER_ANGLE_2) ||
          (id == EULER_ANGLE_3) || 
          (id == Q_1) || (id == Q_2) || (id == Q_3) || (id == Q_4)    ||
          (id == DCM_11)        || (id == DCM_12)   || (id == DCM_13) ||
          (id == DCM_21)        || (id == DCM_22)   || (id == DCM_23) || 
          (id == DCM_31)        || (id == DCM_32)   || (id == DCM_33))
         return true;
   }
   else  // "EulerAngles
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly in euler angles section\n");
   #endif
      if ((id == Q_1) || (id == Q_2) || (id == Q_3) || (id == Q_4)    ||
          (id == MRP_1)         || (id == MRP_2)    || (id == MRP_3)  || // Dunn Added
          (id == DCM_11)        || (id == DCM_12)   || (id == DCM_13) || 
          (id == DCM_21)        || (id == DCM_22)   || (id == DCM_23) || 
          (id == DCM_31)        || (id == DCM_32)   || (id == DCM_33))
         return true;
   }
   

   if (attitudeRateDisplayType == "AngularVelocity")
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly in angular velocity section\n");
   #endif
      if ((id == EULER_ANGLE_RATE_1) || (id == EULER_ANGLE_RATE_2) ||
          (id == EULER_ANGLE_RATE_3))
         return true;
   }
   else // EulerAngleRates
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly in euler angle rates section\n");
   #endif
      if ((id == ANGULAR_VELOCITY_X) || (id == ANGULAR_VELOCITY_Y) ||
          (id == ANGULAR_VELOCITY_Z))
         return true;
   }

   if (attitudeModelName != "PrecessingSpinner")
   {
      if ((id >= NUTATION_REFERENCE_VECTOR_X) && (id <= SPIN_RATE))
         return true;
   }
   if (attitudeModelName != "NadirPointing")
   {
      if ((id >= ATTITUDE_REFERENCE_BODY) && (id <= BODY_CONSTRAINT_VECTOR_Z))
         return true;
   }
   if (attitudeModelName != "CCSDS-AEM")
   {
      if (id == AEM_FILE_NAME)
         return true;
   }

   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly calling GmatBase::IsParameterReadOnly\n");
   #endif

   return GmatBase::IsParameterReadOnly(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool Attitude::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


GmatTime Attitude::GetGmatTimeParameter(const Integer id) const
{
   // re-initialize, if some input has changed
   if (!isInitialized || needsReinit) (const_cast<Attitude*>(this))->Initialize();

   if (id == EPOCH)
      return epochGT;

   return GmatBase::GetGmatTimeParameter(id);
}


GmatTime Attitude::SetGmatTimeParameter(const Integer id, const GmatTime value)
{
   if (id == EPOCH)  // this should be an A1Mjd time
   {
      if (epochGT != value)
      {
         if (isInitialized) needsReinit = true;
         epochGT = value;
      }
      return epochGT;
   }

   return GmatBase::SetGmatTimeParameter(id, value);
}


GmatTime Attitude::GetGmatTimeParameter(const std::string &label) const
{
   return GetGmatTimeParameter(GetParameterID(label));
}


GmatTime Attitude::SetGmatTimeParameter(const std::string &label, const GmatTime value)
{
   return SetGmatTimeParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 * 
 * @note I am using const_cast here to be able to make changes in the 
 *       object data.  (Just like in the Spacecraft).  Ew.
 *
 */
//------------------------------------------------------------------------------
Real Attitude::GetRealParameter(const Integer id) const
{
   #ifdef DEBUG_ATTITUDE_GET_REAL
   MessageInterface::ShowMessage(
   "Entering Attitude::GetReal with id = %d (%s)\n", id,
   (GetParameterText(id)).c_str());
   MessageInterface::ShowMessage("--- and isInitialized = %s, and needsReinit = %s\n",
         (isInitialized? "true" : "false"), (needsReinit? "true" : "false"));
   #endif
   // re-initialize, if some input has changed
   if (!isInitialized || needsReinit) (const_cast<Attitude*>(this))->Initialize();

   if (id == EPOCH)      return epoch;

   if (id == Q_1)
   {
      (const_cast<Attitude*>(this))->UpdateState("Quaternion");
      return quaternion(0);
   }
   if (id == Q_2)
   {   
      (const_cast<Attitude*>(this))->UpdateState("Quaternion");
      return quaternion(1);
   }   
   if (id == Q_3)
   {
      (const_cast<Attitude*>(this))->UpdateState("Quaternion");
      return quaternion(2);
   }
   if (id == Q_4)
   {
      (const_cast<Attitude*>(this))->UpdateState("Quaternion");
      return quaternion(3);
   }
   if (id == EULER_ANGLE_1)  
   {              
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      return eulerAngles(0) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_2)
   {              
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      return eulerAngles(1) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_3)
   {              
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      return eulerAngles(2) * GmatMathConstants::DEG_PER_RAD;
   }
   // Dunn Added MRPs Here
   if (id == MRP_1)
   {              
      (const_cast<Attitude*>(this))->UpdateState("MRPs");
      return mrps(0);
   }
   if (id == MRP_2)
   {              
      (const_cast<Attitude*>(this))->UpdateState("MRPs");
      return mrps(1);
   }
   if (id == MRP_3)
   {              
      (const_cast<Attitude*>(this))->UpdateState("MRPs");
      return mrps(2);
   }

   // dcm is always kept up-to-date, after initialization
   if (id == DCM_11)             return dcm(0,0);
   if (id == DCM_12)             return dcm(0,1);
   if (id == DCM_13)             return dcm(0,2);
   if (id == DCM_21)             return dcm(1,0);
   if (id == DCM_22)             return dcm(1,1);
   if (id == DCM_23)             return dcm(1,2);
   if (id == DCM_31)             return dcm(2,0);
   if (id == DCM_32)             return dcm(2,1);
   if (id == DCM_33)             return dcm(2,2);
   if (id == EULER_ANGLE_RATE_1)
   {
      #ifdef DEBUG_ATTITUDE_GET_REAL
         MessageInterface::ShowMessage("In GetRealP (Attitude <%p>), before UpdateState for ea1, rates = %s\n",
               this, eulerAngleRates.ToString().c_str());
      #endif
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      #ifdef DEBUG_ATTITUDE_GET_REAL
         MessageInterface::ShowMessage("In GetRealP (Attitude <%p>), AFTER UpdateState ea1, rates = %s\n",
               this, eulerAngleRates.ToString().c_str());
      #endif
      return eulerAngleRates(0) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_RATE_2)
   {
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      return eulerAngleRates(1) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_RATE_3)
   {
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      return eulerAngleRates(2) * GmatMathConstants::DEG_PER_RAD;
   }
   // angVel is always kept up-to-date, after initialization
   if (id == ANGULAR_VELOCITY_X) return angVel[0] * GmatMathConstants::DEG_PER_RAD;
   if (id == ANGULAR_VELOCITY_Y) return angVel[1] * GmatMathConstants::DEG_PER_RAD;
   if (id == ANGULAR_VELOCITY_Z) return angVel[2] * GmatMathConstants::DEG_PER_RAD;

   #ifdef DEBUG_ATTITUDE_GET_REAL
   MessageInterface::ShowMessage(
   "Exiting Attitude::GetReal - calling parent ...\n");
   #endif

   // Additional Real data for CSFixed
   // none at this time
   // Additional Real data for Spinner
   // none at this time
   // Additional Real data for SpiceAttitude
   // none at this time
   // Additional Real data for PrecessingSpinner
   if ((id >= NUTATION_REFERENCE_VECTOR_X) && (id <= SPIN_RATE))
   {
      Real precessingReal = GetPrecessingSpinnerRealParameter(id);
      if (precessingReal != REAL_PARAMETER_UNDEFINED)
         return precessingReal;
   }
   // Additional Real data for NadirPointing
   // none at this time
   if ((id >= BODY_ALIGNMENT_VECTOR_X) && (id <= BODY_CONSTRAINT_VECTOR_Z))
   {
      Real nadirReal = GetNadirPointingRealParameter(id);
      if (nadirReal != REAL_PARAMETER_UNDEFINED)
         return nadirReal;
   }

   return GmatBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real Attitude::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
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
Real Attitude::SetRealParameter(const Integer id,
                                const Real    value)
{
   #ifdef DEBUG_ATTITUDE_SET_REAL
   MessageInterface::ShowMessage(
   "ENTERING Att::SetReal with id = %d (%s) and value = %.12f\n", id,
   GetParameterText(id).c_str(), value);
   #endif
   if ((!setInitialAttitudeAllowed) && IsInitialAttitudeParameter(id,"Real"))
   {
      if (!warnNoAttitudeWritten)
      {
         std::string warnMsg = "*** WARNING *** Setting attitude initial ";
         warnMsg += "conditions has no effect when attitude mode is ";
         warnMsg += attitudeModelName + "\n";
         MessageInterface::ShowMessage(warnMsg);
         warnNoAttitudeWritten = true;
      }
      return true;
   }
   if (id == EPOCH)  // this should be an A1Mjd time
   {
      if (epoch != value)
      {
         if (isInitialized) needsReinit = true;
         epoch         = value;
      }
      return epoch;
   }
   else if (id == Q_1)
   {
      if (isInitialized)
      {
         std::string errmsg = "Error: Quaternion must be input as entire vector ";
         errmsg += "inside the mission sequence to avoid ";
         errmsg += "normalization and/or conversion errors.\n";
         throw AttitudeException(errmsg);
      }
      // Set the value, but quaternion is not verified or normalized until initialization
      if (inputAttitudeType == GmatAttitude::QUATERNION_TYPE)
      {
         quaternion[0] = value;
      }
      else
      {
         (const_cast<Attitude*>(this))->UpdateState("Quaternion");
         quaternion[0] = value;
         ValidateQuaternion(quaternion);
      }
      inputAttitudeType  = GmatAttitude::QUATERNION_TYPE;
      if (isInitialized) needsReinit = true;
      return quaternion(0);
   }
   if (id == Q_2)
   {
      if (isInitialized)
      {
         std::string errmsg = "Error: Quaternion must be input as entire vector ";
         errmsg += "inside the mission sequence to avoid ";
         errmsg += "normalization and/or conversion errors.\n";
         throw AttitudeException(errmsg);
      }
      // Set the value, but quaternion is not verified or normalized until initialization
      if (inputAttitudeType == GmatAttitude::QUATERNION_TYPE)
      {
         quaternion[1] = value;
      }
      else
      {
         (const_cast<Attitude*>(this))->UpdateState("Quaternion");
         quaternion[1] = value;
         ValidateQuaternion(quaternion);
      }
      inputAttitudeType  = GmatAttitude::QUATERNION_TYPE;
      if (isInitialized) needsReinit = true;
      return quaternion(1);
   }
   if (id == Q_3)
   {
      if (isInitialized)
      {
         std::string errmsg = "Error: Quaternion must be input as entire vector ";
         errmsg += "inside the mission sequence to avoid ";
         errmsg += "normalization and/or conversion errors.\n";
         throw AttitudeException(errmsg);
      }
      // Set the value, but quaternion is not verified or normalized until initialization
      if (inputAttitudeType == GmatAttitude::QUATERNION_TYPE)
      {
         quaternion[2] = value;
      }
      else
      {
         (const_cast<Attitude*>(this))->UpdateState("Quaternion");
         quaternion[2] = value;
         ValidateQuaternion(quaternion);
      }
      inputAttitudeType  = GmatAttitude::QUATERNION_TYPE;
      if (isInitialized) needsReinit = true;
      return quaternion(2);
   }
   if (id == Q_4)
   {
      if (isInitialized)
      {
         std::string errmsg = "Error: Quaternion must be input as entire vector ";
                     errmsg += "inside the mission sequence to avoid ";
                     errmsg += "normalization and/or conversion errors.\n";
         throw AttitudeException(errmsg);
      }
      // Set the value, but quaternion is not verified or normalized until initialization
      if (inputAttitudeType == GmatAttitude::QUATERNION_TYPE)
      {
         quaternion[3] = value;
      }
      else
      {
         (const_cast<Attitude*>(this))->UpdateState("Quaternion");
         quaternion[3] = value;
         ValidateQuaternion(quaternion);
      }
      inputAttitudeType  = GmatAttitude::QUATERNION_TYPE;
      if (isInitialized) needsReinit = true;
      return quaternion(3);
   }
   if (id == EULER_ANGLE_1)
   {
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      eulerAngles(0) = value * GmatMathConstants::RAD_PER_DEG;
      dcm = AttitudeConversionUtility::ToCosineMatrix(eulerAngles,
                      eulerSequenceArray.at(0),
                      eulerSequenceArray.at(1),
                      eulerSequenceArray.at(2));
      inputAttitudeType = GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE;
      if (isInitialized) needsReinit = true;
      #ifdef DEBUG_ATTITUDE_SET_REAL
      MessageInterface::ShowMessage(
      "   and now euler angles = %.12f   %.12f   %.12f\n", eulerAngles[0], eulerAngles[1], eulerAngles[2]);
      #endif
      return eulerAngles(0) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_2)
   {
      Real angle2 = value * GmatMathConstants::RAD_PER_DEG;
      // check for a nearly singular attitude, for symmetric sequences
      if ((eulerSequenceArray.at(0) == eulerSequenceArray.at(2)) &&
          (GmatMathUtil::Abs(GmatMathUtil::Sin(angle2)) < GmatAttitudeConstants::EULER_ANGLE_TOLERANCE))
      {
         std::ostringstream errmsg;
         errmsg << "The attitude defined by the input euler angles (";
         errmsg << (eulerAngles(0) * GmatMathConstants::DEG_PER_RAD) << ", "
                << value << ", " << (eulerAngles(2) * GmatMathConstants::DEG_PER_RAD);
         errmsg << ") is near a singularity.  The allowed values are:\n";
         errmsg << "For a symmetric sequence EulerAngle2 != 0. ";
         errmsg << "For a non-symmetric sequence EulerAngle2 != 90.  ";
         errmsg << "The tolerance on EulerAngle2 singularity is ";
         errmsg << GmatAttitudeConstants::EULER_ANGLE_TOLERANCE << ".\n";
         throw AttitudeException(errmsg.str());
      }
      // check for a nearly singular attitude, for non-symmetric sequences
      else if ((eulerSequenceArray.at(0) != eulerSequenceArray.at(2)) &&
               (GmatMathUtil::Abs(GmatMathUtil::Cos(angle2)) < GmatAttitudeConstants::EULER_ANGLE_TOLERANCE))
      {
         std::ostringstream errmsg;
         errmsg << "The attitude defined by the input euler angles (";
         errmsg << (eulerAngles(0) * GmatMathConstants::DEG_PER_RAD) << ", "
                << value << ", " << (eulerAngles(2) * GmatMathConstants::DEG_PER_RAD);
         errmsg << ") is near a singularity.  The allowed values are:\n";
         errmsg << "For a symmetric sequence EulerAngle2 != 0. ";
         errmsg << "For a non-symmetric sequence EulerAngle2 != 90.  ";
         errmsg << "The tolerance on EulerAngle2 singularity is ";
         errmsg << GmatAttitudeConstants::EULER_ANGLE_TOLERANCE << ".\n";
         throw AttitudeException(errmsg.str());
      }
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      eulerAngles(1) = value * GmatMathConstants::RAD_PER_DEG;
      dcm = AttitudeConversionUtility::ToCosineMatrix(eulerAngles,
                      eulerSequenceArray.at(0),
                      eulerSequenceArray.at(1),
                      eulerSequenceArray.at(2));
      inputAttitudeType = GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE;
      if (isInitialized) needsReinit = true;
      #ifdef DEBUG_ATTITUDE_SET_REAL
      MessageInterface::ShowMessage(
      "   and now euler angles = %.12f   %.12f   %.12f\n", eulerAngles[0], eulerAngles[1], eulerAngles[2]);
      #endif
      return eulerAngles(1) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_3)
   {
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      eulerAngles(2) = value * GmatMathConstants::RAD_PER_DEG;
      dcm = AttitudeConversionUtility::ToCosineMatrix(eulerAngles,
                      eulerSequenceArray.at(0),
                      eulerSequenceArray.at(1),
                      eulerSequenceArray.at(2));
      inputAttitudeType = GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE;
      if (isInitialized) needsReinit = true;
      #ifdef DEBUG_ATTITUDE_SET_REAL
      MessageInterface::ShowMessage(
      "   and now euler angles = %.12f   %.12f   %.12f\n", eulerAngles[0], eulerAngles[1], eulerAngles[2]);
      #endif
      return eulerAngles(2) * GmatMathConstants::DEG_PER_RAD;
   }
   // Dunn Added MRPs
   if (id == MRP_1)
   {
      (const_cast<Attitude*>(this))->UpdateState("MRPs");
      mrps(0) = value;
      inputAttitudeType = GmatAttitude::MODIFIED_RODRIGUES_PARAMETERS_TYPE;
      if (isInitialized) needsReinit = true;
      return mrps(0);
   }
   if (id == MRP_2)
   {
      (const_cast<Attitude*>(this))->UpdateState("MRPs");
      mrps(1) = value;
      inputAttitudeType = GmatAttitude::MODIFIED_RODRIGUES_PARAMETERS_TYPE;
      if (isInitialized) needsReinit = true;
      return mrps(1);
   }
   if (id == MRP_3)
   {
      (const_cast<Attitude*>(this))->UpdateState("MRPs");
      mrps(2) = value;
      inputAttitudeType = GmatAttitude::MODIFIED_RODRIGUES_PARAMETERS_TYPE;
      if (isInitialized) needsReinit = true;
      return mrps(2);
   }
   if (id == DCM_11)
   {
      if ((value < -1.0) || (value > 1.0))
      {
         AttitudeException ae("");
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                       GmatStringUtil::ToString(value, 16).c_str(),
                       "DCM11", "-1.0 <= Real Number <= 1.0");
         throw ae;
      }
      (const_cast<Attitude*>(this))->UpdateState("DirectionCosineMatrix");
      dcm(0,0)          = value;
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      if (isInitialized) needsReinit = true;
      return dcm(0,0);
   }
   if (id == DCM_12)
   {
      if ((value < -1.0) || (value > 1.0))
      {
         AttitudeException ae("");
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                       GmatStringUtil::ToString(value, 16).c_str(),
                       "DCM12", "-1.0 <= Real Number <= 1.0");
         throw ae;
      }
      (const_cast<Attitude*>(this))->UpdateState("DirectionCosineMatrix");
      dcm(0,1) = value;
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      if (isInitialized) needsReinit = true;
      return dcm(0,1);
   }
   if (id == DCM_13)
   {
      if ((value < -1.0) || (value > 1.0))
      {
         AttitudeException ae("");
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                       GmatStringUtil::ToString(value, 16).c_str(),
                       "DCM13", "-1.0 <= Real Number <= 1.0");
         throw ae;
      }
      (const_cast<Attitude*>(this))->UpdateState("DirectionCosineMatrix");
      dcm(0,2) = value;
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      if (isInitialized) needsReinit = true;
      return dcm(0,2);
   }
   if (id == DCM_21)
   {
      if ((value < -1.0) || (value > 1.0))
      {
         AttitudeException ae("");
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                       GmatStringUtil::ToString(value, 16).c_str(),
                       "DCM21", "-1.0 <= Real Number <= 1.0");
         throw ae;
      }
      (const_cast<Attitude*>(this))->UpdateState("DirectionCosineMatrix");
      dcm(1,0) = value;
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      if (isInitialized) needsReinit = true;
      return dcm(1,0);
   }
   if (id == DCM_22)
   {
      if ((value < -1.0) || (value > 1.0))
      {
         AttitudeException ae("");
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                       GmatStringUtil::ToString(value, 16).c_str(),
                       "DCM22", "-1.0 <= Real Number <= 1.0");
         throw ae;
      }
      (const_cast<Attitude*>(this))->UpdateState("DirectionCosineMatrix");
      dcm(1,1) = value;
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      if (isInitialized) needsReinit = true;
      return dcm(1,1);
   }
   if (id == DCM_23)
   {
      if ((value < -1.0) || (value > 1.0))
      {
         AttitudeException ae("");
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                       GmatStringUtil::ToString(value, 16).c_str(),
                       "DCM23", "-1.0 <= Real Number <= 1.0");
         throw ae;
      }
      (const_cast<Attitude*>(this))->UpdateState("DirectionCosineMatrix");
      dcm(1,2) = value;
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      if (isInitialized) needsReinit = true;
      return dcm(1,2);
   }
   if (id == DCM_31)
   {
      if ((value < -1.0) || (value > 1.0))
      {
         AttitudeException ae("");
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                       GmatStringUtil::ToString(value, 16).c_str(),
                       "DCM31", "-1.0 <= Real Number <= 1.0");
         throw ae;
      }
      (const_cast<Attitude*>(this))->UpdateState("DirectionCosineMatrix");
      dcm(2,0) = value;
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      if (isInitialized) needsReinit = true;
      return dcm(2,0);
   }
   if (id == DCM_32)
   {
      if ((value < -1.0) || (value > 1.0))
      {
         AttitudeException ae("");
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                       GmatStringUtil::ToString(value, 16).c_str(),
                       "DCM32", "-1.0 <= Real Number <= 1.0");
         throw ae;
      }
      (const_cast<Attitude*>(this))->UpdateState("DirectionCosineMatrix");
      dcm(2,1) = value;
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      if (isInitialized) needsReinit = true;
      return dcm(2,1);
   }
   if (id == DCM_33)
   {
      if ((value < -1.0) || (value > 1.0))
      {
         AttitudeException ae("");
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                       GmatStringUtil::ToString(value, 16).c_str(),
                       "DCM33", "-1.0 <= Real Number <= 1.0");
         throw ae;
      }
      (const_cast<Attitude*>(this))->UpdateState("DirectionCosineMatrix");
      dcm(2,2) = value;
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      if (isInitialized) needsReinit = true;
      return dcm(2,2);
   }
   if (id == EULER_ANGLE_RATE_1)
   {
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      eulerAngleRates(0) = value * GmatMathConstants::RAD_PER_DEG;
      angVel = AttitudeConversionUtility::ToAngularVelocity(eulerAngleRates,eulerAngles,
                        eulerSequenceArray.at(0),
                        eulerSequenceArray.at(1),
                        eulerSequenceArray.at(2));
      inputAttitudeRateType = GmatAttitude::EULER_ANGLE_RATES_TYPE;
      if (isInitialized) needsReinit = true;
      return eulerAngleRates(0) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_RATE_2)
   {
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      eulerAngleRates(1) = value * GmatMathConstants::RAD_PER_DEG;
      angVel = AttitudeConversionUtility::ToAngularVelocity(eulerAngleRates,eulerAngles,
                        eulerSequenceArray.at(0),
                        eulerSequenceArray.at(1),
                        eulerSequenceArray.at(2));
      inputAttitudeRateType = GmatAttitude::EULER_ANGLE_RATES_TYPE;
      if (isInitialized) needsReinit = true;
      return eulerAngleRates(1) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_RATE_3)
   {
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      eulerAngleRates(2) = value * GmatMathConstants::RAD_PER_DEG;
      angVel = AttitudeConversionUtility::ToAngularVelocity(eulerAngleRates,eulerAngles,
                        eulerSequenceArray.at(0),
                        eulerSequenceArray.at(1),
                        eulerSequenceArray.at(2));
      inputAttitudeRateType = GmatAttitude::EULER_ANGLE_RATES_TYPE;
      if (isInitialized) needsReinit = true;
      return eulerAngleRates(2) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == ANGULAR_VELOCITY_X)
   {
      (const_cast<Attitude*>(this))->UpdateState("AngularVelocity");
      angVel(0) = value * GmatMathConstants::RAD_PER_DEG;
      inputAttitudeRateType = GmatAttitude::ANGULAR_VELOCITY_TYPE;
      if (isInitialized) needsReinit = true;
      return angVel(0) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == ANGULAR_VELOCITY_Y)
   {
      UpdateState("AngularVelocity");
      angVel(1) = value * GmatMathConstants::RAD_PER_DEG;
      inputAttitudeRateType = GmatAttitude::ANGULAR_VELOCITY_TYPE;
      if (isInitialized) needsReinit = true;
      return angVel(1) * GmatMathConstants::DEG_PER_RAD;
   }
   if (id == ANGULAR_VELOCITY_Z)
   {
      (const_cast<Attitude*>(this))->UpdateState("AngularVelocity");
      angVel(2) = value * GmatMathConstants::RAD_PER_DEG;
      inputAttitudeRateType = GmatAttitude::ANGULAR_VELOCITY_TYPE;
      if (isInitialized) needsReinit = true;
      return angVel(2) * GmatMathConstants::DEG_PER_RAD;
   }

   // Additional Real data for CSFixed
   // none at this time
   // Additional Real data for Spinner
   // none at this time
   // Additional Real data for SpiceAttitude
   // none at this time
   // Additional Real data for PrecessingSpinner
   if ((id >= NUTATION_REFERENCE_VECTOR_X) && (id <= SPIN_RATE))
   {
      Real precessingReal = SetPrecessingSpinnerRealParameter(id, value);
      if (precessingReal != REAL_PARAMETER_UNDEFINED)
         return precessingReal;
   }
   // Additional Real data for NadirPointing
   // none at this time
   if ((id >= BODY_ALIGNMENT_VECTOR_X) && (id <= BODY_CONSTRAINT_VECTOR_Z))
   {
      Real nadirReal = SetNadirPointingRealParameter(id, value);
      if (nadirReal != REAL_PARAMETER_UNDEFINED)
         return nadirReal;
   }

   return GmatBase::SetRealParameter(id, value);
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
Real Attitude::SetRealParameter(const std::string &label,
                                const Real        value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value, Integer index)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id    The integer ID for the parameter.
 * @param value The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real Attitude::SetRealParameter(const Integer id, const Real value,
                                const Integer index)
{
   #ifdef DEBUG_ATTITUDE_SET_REAL
      MessageInterface::ShowMessage("Entering SetReal with id = %d (%s), value = %12.10f, index= %d\n",
            id, GetParameterText(id).c_str(), value, index);
   #endif
   if ((!setInitialAttitudeAllowed) && IsInitialAttitudeParameter(id,"Rvector"))
   {
      if (!warnNoAttitudeWritten)
      {
         std::string warnMsg = "*** WARNING *** Setting attitude initial ";
         warnMsg += "conditions has no effect when attitude mode is ";
         warnMsg += attitudeModelName + "\n";
         MessageInterface::ShowMessage(warnMsg);
         warnNoAttitudeWritten = true;
      }
      return true;
   }
   if (id == EULER_ANGLES)
   {
      if ((index < 0) || (index > 2))
      {
         throw AttitudeException("Error: the Euler Angle index is out-of-range\n");
      }
      if (index == 1) // second angle
      {
         Real angle2 = value * GmatMathConstants::RAD_PER_DEG;
         // check for a nearly singular attitude, for symmetric sequences
         if ((eulerSequenceArray.at(0) == eulerSequenceArray.at(2)) &&
             (GmatMathUtil::Abs(GmatMathUtil::Sin(angle2)) < GmatAttitudeConstants::EULER_ANGLE_TOLERANCE))
         {
            std::ostringstream errmsg;
            errmsg << "The attitude defined by the input euler angles (";
            errmsg << (eulerAngles(0)) << ", "
                   << value << ", " << (eulerAngles(2));
            errmsg << ") is near a singularity.  The allowed values are:\n";
            errmsg << "For a symmetric sequence EulerAngle2 != 0. ";
            errmsg << "For a non-symmetric sequence EulerAngle2 != 90.  ";
            errmsg << "The tolerance on EulerAngle2 singularity is ";
            errmsg << GmatAttitudeConstants::EULER_ANGLE_TOLERANCE << ".\n";
            throw AttitudeException(errmsg.str());
         }
         // check for a nearly singular attitude, for non-symmetric sequences
         else if ((eulerSequenceArray.at(0) != eulerSequenceArray.at(2)) &&
                  (GmatMathUtil::Abs(GmatMathUtil::Cos(angle2)) < GmatAttitudeConstants::EULER_ANGLE_TOLERANCE))
         {
            std::ostringstream errmsg;
            errmsg << "The attitude defined by the input euler angles (";
            errmsg << (eulerAngles(0)) << ", "
                   << value << ", " << (eulerAngles(2));
            errmsg << ") is near a singularity.  The allowed values are:\n";
            errmsg << "For a symmetric sequence EulerAngle2 != 0. ";
            errmsg << "For a non-symmetric sequence EulerAngle2 != 90.  ";
            errmsg << "The tolerance on EulerAngle2 singularity is ";
            errmsg << GmatAttitudeConstants::EULER_ANGLE_TOLERANCE << ".\n";
            throw AttitudeException(errmsg.str());
         }
      }
      eulerAngles(index) = value * GmatMathConstants::RAD_PER_DEG;
      dcm = AttitudeConversionUtility::ToCosineMatrix(eulerAngles,
                      eulerSequenceArray.at(0),
                      eulerSequenceArray.at(1),
                      eulerSequenceArray.at(2));
      inputAttitudeType = GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE;
      if (isInitialized) needsReinit = true;
      return eulerAngles(index);
   }
   if (id == QUATERNION)
   {
      if ((index < 0) || (index > 3))
      {
         throw AttitudeException("Error: the Quaternion index is out-of-range\n");
      }
      quaternion(index) = value;
      inputAttitudeType = GmatAttitude::QUATERNION_TYPE;
      #ifdef DEBUG_ATTITUDE_SET_REAL
         MessageInterface::ShowMessage(" ... set quaternion[%d] = %12.10f\n",
               index, value);
         MessageInterface::ShowMessage(" ...     quaternion = %s\n",
               quaternion.ToString().c_str());
      #endif
      if (isInitialized) needsReinit = true;
      return quaternion(index);
   }

   // Dunn added MRPs
   if (id == MRPS)
   {
      if ((index < 0) || (index > 2))
      {
         throw AttitudeException("Error: the MRPs index is out-of-range\n");
      }
      mrps(index) = value;
      inputAttitudeType = GmatAttitude::MODIFIED_RODRIGUES_PARAMETERS_TYPE;
      #ifdef DEBUG_ATTITUDE_SET_REAL
            MessageInterface::ShowMessage(" ... set MRP[%d] = %12.10f\n",
               index, value);
            MessageInterface::ShowMessage(" ...     MRP = %s\n",
               quaternion.ToString().c_str());
      #endif
      if (isInitialized) needsReinit = true;
      return mrps(index);
   }

   if (id == EULER_ANGLE_RATES)
   {
      if ((index < 0) || (index > 2))
      {
         throw AttitudeException("Error: the Euler Angle Rate index is out-of-range\n");
      }
      eulerAngleRates(index) = value * GmatMathConstants::RAD_PER_DEG;
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      angVel = AttitudeConversionUtility::ToAngularVelocity(eulerAngleRates, eulerAngles,
                         eulerSequenceArray.at(0),
                         eulerSequenceArray.at(1),
                         eulerSequenceArray.at(2));
      inputAttitudeRateType = GmatAttitude::EULER_ANGLE_RATES_TYPE;
      if (isInitialized) needsReinit = true;
      return eulerAngleRates(index);
   }
   if (id == ANGULAR_VELOCITY)
   {
      if ((index < 0) || (index > 2))
      {
         throw AttitudeException("Error: the Angular Velocity index is out-of-range\n");
      }
      angVel(index) = value * GmatMathConstants::RAD_PER_DEG;
      inputAttitudeRateType = GmatAttitude::ANGULAR_VELOCITY_TYPE;
      if (isInitialized) needsReinit = true;
      return angVel(index);
   }
   return GmatBase::SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  const UnsignedIntArray&  GetUnsignedIntArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the UnsignedIntArray parameter value, 
 * given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return  UnsignedIntArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const UnsignedIntArray& 
            Attitude::GetUnsignedIntArrayParameter(const Integer id) const
{
   if (id == EULER_SEQUENCE) return eulerSequenceArray;
   return GmatBase::GetUnsignedIntArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const UnsignedIntArray&
//        GetUnsignedIntArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the UnsignedIntArray parameter value, 
 * given the input parameter label.
 *
 * @param <label> label for the requested parameter value.
 *
 * @return  UnsignedIntArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const UnsignedIntArray&
            Attitude::GetUnsignedIntArrayParameter(
            const std::string &label) const
{
   return GetUnsignedIntArrayParameter(GetParameterID(label));
}


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
const Rvector& Attitude::GetRvectorParameter(const Integer id) const
{
   static Rvector3 vec3;
   static Rvector  vec4(4);
   if (!isInitialized || needsReinit) (const_cast<Attitude*>(this))->Initialize();

   if (id == QUATERNION)
   {          
      (const_cast<Attitude*>(this))->UpdateState("Quaternion");
      vec4 = quaternion;
      return vec4;
   }
   if (id == MRPS) // Dunn Added
   {     
      (const_cast<Attitude*>(this))->UpdateState("MRPs");
      vec3 = mrps;
      return vec3;
   }
   if (id == EULER_ANGLES)   
   {     
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      vec3 =  (eulerAngles * GmatMathConstants::DEG_PER_RAD);
      return vec3;
   }
   if (id == ANGULAR_VELOCITY) 
   {  
      (const_cast<Attitude*>(this))->UpdateState("AngularVelocity");
      vec3 = angVel * GmatMathConstants::DEG_PER_RAD;
      return vec3;
   }
   if (id == EULER_ANGLE_RATES) 
   {  
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      vec3 = eulerAngleRates * GmatMathConstants::DEG_PER_RAD;
      return vec3;
      //return (eulerAngleRates * GmatMathConstants::DEG_PER_RAD);
   }

   return GmatBase::GetRvectorParameter(id);
}

//------------------------------------------------------------------------------
//  const Rvector&  GetRvectorParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * This method gets the Rvector parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  the Rvector parameter.
 *
 */
//------------------------------------------------------------------------------
const Rvector& Attitude::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  const Rvector&  SetRvectorParameter(const Integer id, const Rvector& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rvector parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Rvector value for the requested parameter (angles are
 *                expected in degrees).
 *
 * @return  Rvector parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rvector& Attitude::SetRvectorParameter(const Integer id,
                                             const Rvector &value)
{
   #ifdef DEBUG_ATTITUDE_SET
      MessageInterface::ShowMessage("Entering Attitude::SetRvector with id = %d and value =  %s\n",
            id, (value.ToString()).c_str());
      MessageInterface::ShowMessage("  and Euler sequence is %d %d %d\n", eulerSequenceArray.at(0),
            eulerSequenceArray.at(1), eulerSequenceArray.at(2));
   #endif
   Integer sz = value.GetSize();
   Integer i;
   
   if ((!setInitialAttitudeAllowed) && IsInitialAttitudeParameter(id,"Rvector"))
   {
      if (!warnNoAttitudeWritten)
      {
         std::string warnMsg = "*** WARNING *** Setting attitude initial ";
         warnMsg += "conditions has no effect when attitude mode is ";
         warnMsg += attitudeModelName + "\n";
         MessageInterface::ShowMessage(warnMsg);
         warnNoAttitudeWritten = true;
      }
      return angVel;
   }
   if (id == EULER_ANGLES)
   {
      if (sz != 3) throw AttitudeException(
                  "Incorrectly sized Rvector passed in for euler angles.");
      ValidateEulerAngles(value * GmatMathConstants::RAD_PER_DEG, eulerSequenceArray);
      for (i=0;i<3;i++) 
         eulerAngles(i) = value(i) * GmatMathConstants::RAD_PER_DEG;
      dcm = AttitudeConversionUtility::ToCosineMatrix(eulerAngles,
                      eulerSequenceArray.at(0),
                      eulerSequenceArray.at(1),
                      eulerSequenceArray.at(2));
      inputAttitudeType = GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE;
      if (isInitialized) needsReinit = true;
      return eulerAngles;
   }
   if (id == QUATERNION)
   {
      #ifdef DEBUG_ATTITUDE_SET
         MessageInterface::ShowMessage("In Attitude::SetRvector, setting quaternion\n");
      #endif
      ValidateQuaternion(value);
      for (i=0;i<4;i++) 
         quaternion(i) = value(i);
      quaternion.Normalize();
      #ifdef DEBUG_ATTITUDE_SET
         MessageInterface::ShowMessage("In Attitude::SetRvector, normalized quaternion = %s\n",
               (quaternion.ToString()).c_str());
      #endif
      dcm = AttitudeConversionUtility::ToCosineMatrix(quaternion);
      inputAttitudeType = GmatAttitude::QUATERNION_TYPE;
      if (isInitialized) needsReinit = true;
      return quaternion;
   }
   if (id == MRPS) // Dunn Added
   {
      #ifdef DEBUG_ATTITUDE_SET
            MessageInterface::ShowMessage("In Attitude::SetRvector, setting MRPs\n");
      #endif
      if (sz != 3) throw AttitudeException(
         "Incorrectly sized Rvector passed in for MRPs.");
      ValidateMRPs(value);
      for (i=0;i<3;i++) 
         mrps(i) = value(i);
      quaternion = AttitudeConversionUtility::ToQuaternion(mrps);
      dcm = AttitudeConversionUtility::ToCosineMatrix(quaternion);
      inputAttitudeType = GmatAttitude::MODIFIED_RODRIGUES_PARAMETERS_TYPE;
      if (isInitialized) needsReinit = true;
      return mrps;
   }
   if (id == EULER_ANGLE_RATES)
   {
      #ifdef DEBUG_SET_RATE
         MessageInterface::ShowMessage("In SetRvector with value = %s\n", value.ToString().c_str());
      #endif
      if (sz != 3) throw AttitudeException(
                  "Incorrectly sized Rvector passed in for euler angle rates.");
      for (i=0;i<3;i++) 
         eulerAngleRates(i) = value(i) * GmatMathConstants::RAD_PER_DEG;
      #ifdef DEBUG_SET_RATE
      MessageInterface::ShowMessage("      and eulerAngleRates set to %s\n", eulerAngleRates.ToString().c_str());
      MessageInterface::ShowMessage("      and eulerAngles are currently set to %s\n", eulerAngles.ToString().c_str());
      #endif
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      angVel = AttitudeConversionUtility::ToAngularVelocity(eulerAngleRates,
                         eulerAngles,
                         eulerSequenceArray.at(0),
                         eulerSequenceArray.at(1),
                         eulerSequenceArray.at(2));                         
      #ifdef DEBUG_SET_RATE
         MessageInterface::ShowMessage(">>>>>> and angVel computed as %le %le %le\n",
               angVel[0] * GmatMathConstants::DEG_PER_RAD,
               angVel[1] * GmatMathConstants::DEG_PER_RAD,
               angVel[2] * GmatMathConstants::DEG_PER_RAD);
      #endif
      inputAttitudeRateType = GmatAttitude::EULER_ANGLE_RATES_TYPE;
      if (isInitialized) needsReinit = true;
      return eulerAngleRates;
   }
   if (id == ANGULAR_VELOCITY)
   {
      if (sz != 3) throw AttitudeException(
                  "Incorrectly sized Rvector passed in for angular velocity.");
      for (i=0;i<3;i++) 
         angVel(i) = value(i) * GmatMathConstants::RAD_PER_DEG;
      inputAttitudeRateType = GmatAttitude::ANGULAR_VELOCITY_TYPE;
      if (isInitialized) needsReinit = true;
      return angVel;
   }
   if (id == BODY_SPIN_AXIS)
   {
      if (sz != 3) throw AttitudeException(
                  "Incorrectly sized Rvector passed in for Body Spin Axis.");
      for (i=0;i<3;i++)   bodySpinAxis(i) = value(i);
      Real bsMag = bodySpinAxis.GetMagnitude();
      // Error message and vector normalization
      if ( bsMag < 1.0e-5 )
      {
         std::string errmsg = "PrecessingSpinner attitude model is singular ";
         errmsg            += "and/or undefined for the Spacecraft.  ";
         errmsg            += "Magnitude of ";
         errmsg            += "BodySpinVector must be >= 1e-5\n";
         throw AttitudeException(errmsg);
      }
      if (isInitialized) needsReinit = true;
      return bodySpinAxis;
   }
   if (id == NUTATION_REFERENCE_VECTOR)
   {
      if (sz != 3) throw AttitudeException(
                  "Incorrectly sized Rvector passed in for Nutation Reference Vector.");
      for (i=0;i<3;i++)   nutationReferenceVector(i) = value(i);
      Real nrMag = nutationReferenceVector.GetMagnitude();
      if ( nrMag < 1.0e-5 )
      {
         std::string errmsg = "PrecessingSpinner attitude model is singular ";
         errmsg            += "and/or undefined for the Spacecraft.  ";
         errmsg            += "Magnitude of ";
         errmsg            += "NutationReferenceVector must be >= 1e-5\n";
         throw AttitudeException(errmsg);
      }
      if (isInitialized) needsReinit = true;
      return nutationReferenceVector;
   }

   return GmatBase::SetRvectorParameter(id,value);
}

                                                 
//------------------------------------------------------------------------------
//  const Rvector&  SetRvectorParameter(const std::string &label, 
//                                      const Rvector& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rvector parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 * @param <value> Rvector value for the requested parameter.
 *
 * @return  Rvector parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rvector& Attitude::SetRvectorParameter(const std::string &label,
                                             const Rvector     &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}
                                    
//------------------------------------------------------------------------------
//  const Rmatrix&  GetRmatrixParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * This method gets the Rmatrix parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  the Rmatrix parameter.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& Attitude::GetRmatrixParameter(const Integer id) const
{
   if (id == DIRECTION_COSINE_MATRIX)    return dcm;
   return GmatBase::GetRmatrixParameter(id);
}

//------------------------------------------------------------------------------
//  const Rmatrix&  GetRmatrixParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * This method gets the Rmatrix parameter value, given the input
 * parameter ID.
 *
 * @param <label> string ID for the requested parameter.
 *
 * @return  the Rmatrix parameter.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& Attitude::GetRmatrixParameter(const std::string &label) const
{
   return GetRmatrixParameter(GetParameterID(label));
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
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& Attitude::SetRmatrixParameter(const Integer id,
                                             const Rmatrix &value)
{
   if ((!setInitialAttitudeAllowed) && IsInitialAttitudeParameter(id,"Rmatrix"))
   {
      if (!warnNoAttitudeWritten)
      {
         std::string warnMsg = "*** WARNING *** Setting attitude initial ";
         warnMsg += "conditions has no effect when attitude mode is ";
         warnMsg += attitudeModelName + "\n";
         MessageInterface::ShowMessage(warnMsg);
         warnNoAttitudeWritten = true;
      }
      return dcm;
   }
   if (id == DIRECTION_COSINE_MATRIX) 
   {
      Integer r,c;
      value.GetSize(r,c);
      if ((r != 3) || (c != 3))
         throw AttitudeException(
         "Incorrectly sized Rmatrix passed in for direction cosine matrix.");
      Rmatrix33 inValue;
      for (Integer i = 0; i < 3; i++) // compiler didn't like op=here ???
         for (Integer j = 0; j < 3; j++)
            inValue(i,j) = value(i,j);
      ValidateCosineMatrix(inValue);
      dcm               = inValue;
      inputAttitudeType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      if (isInitialized) needsReinit = true;
      return dcm;
   }

   return GmatBase::SetRmatrixParameter(id,value);
}
                                                 
                                                 
//------------------------------------------------------------------------------
//  const Rmatrix&  SetRmatrixParameter(const std::string &label,
//                                      const Rmatrix& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rmatrix parameter value, given the input
 * parameter label.
 *
 * @param <label> string ID for the requested parameter.
 * @param <value> Rmatrix value for the requested parameter.
 *
 * @return  the Rmatrix parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix&    Attitude::SetRmatrixParameter(const std::string &label,
                                                 const Rmatrix &value)
{
   return SetRmatrixParameter(GetParameterID(label), value);
}

                                                 
//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter given the input parameter ID. 
 *
 * @param id ID for the requested parameter.
 * 
 * @return The requested string.
 */
//------------------------------------------------------------------------------
std::string Attitude::GetStringParameter(const Integer id) const
{
   if (id == ATTITUDE_DISPLAY_STATE_TYPE)      return attitudeDisplayType;
   if (id == ATTITUDE_RATE_DISPLAY_STATE_TYPE) return attitudeRateDisplayType;
   if (id == REFERENCE_COORDINATE_SYSTEM)      return refCSName;
   if (id == EULER_ANGLE_SEQUENCE)             return eulerSequence;
   if (id == ATTITUDE_CONSTRAINT_TYPE)
   {
      if (attitudeModelName != "NadirPointing")
      {
//         std::string warnMsg = "*** WARNING *** Requesting  ";
//         warnMsg += GetParameterText(id);
//         warnMsg += " on a non-NadirPointing Attitude model\n";
//         MessageInterface::ShowMessage(warnMsg);
      }
      return attitudeConstraintType;
   }
   if (id == AEM_FILE_NAME)
   {
      return aemFile;
   }
   if (id <= ATTITUDE_REFERENCE_BODY)
   {
      if (attitudeModelName != "NadirPointing")
      {
//         std::string warnMsg = "*** WARNING *** Requesting  ";
//         warnMsg += GetParameterText(id);
//         warnMsg += " on a non-NadirPointing Attitude model\n";
//         MessageInterface::ShowMessage(warnMsg);
      }
      return refBodyName;
   }
   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter given the input parameter label. 
 *
 * @param label label for the requested parameter.
 * 
 * @return The requested string.
 */
//------------------------------------------------------------------------------
std::string Attitude::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
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
bool Attitude::SetStringParameter(const Integer     id,
                                  const std::string &value)
{
   #ifdef DEBUG_ATTITUDE_SET
   MessageInterface::ShowMessage(
   "ENTERING Att::SetString with id = %d (%s) and value = %s\n", id,
   GetParameterText(id).c_str(), value.c_str());
   #endif
   if (id == ATTITUDE_DISPLAY_STATE_TYPE)
   {
      if ((value != "Quaternion") && (value != "DirectionCosineMatrix") &&
          (value != "EulerAngles") && (value != "MRPs")) // Dunn Added MRPs
      {
         AttitudeException ae;
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
            value.c_str(), GetParameterText(id).c_str(),
            "\"Quaternion\" \"DirectionCosineMatrix\" \"EulerAngles\" \"MRPs\""); // Dunn Added MRPs
         throw ae;
      }
      attitudeDisplayType = value;
      return true;
   }
   if (id == ATTITUDE_RATE_DISPLAY_STATE_TYPE)
   {
      if ((value != "AngularVelocity") && (value != "EulerAngleRates"))
      {
         AttitudeException ae;
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
            value.c_str(), GetParameterText(id).c_str(),
            "\"AngularVelocity\"  \"EulerAngleRates\"");
         throw ae;
      }
      attitudeRateDisplayType = value;
      return true;
   }
   if (id == REFERENCE_COORDINATE_SYSTEM)
   {
      if (!modifyCoordSysAllowed)
      {
         if (!warnNoCSWritten)
         {
            std::string warnMsg = "*** WARNING *** Setting coordinate ";
            warnMsg += "system has no effect when attitude mode is ";
            warnMsg += attitudeModelName + "\n";
            MessageInterface::ShowMessage(warnMsg);
            warnNoCSWritten = true;
         }
         return true;
      }
      refCSName = value;
      return true;
   }
   if (id == EULER_ANGLE_SEQUENCE)
   {
      #ifdef DEBUG_ATTITUDE_SET
         MessageInterface::ShowMessage("****** SETTING Euler angle sequence to %s\n",
               value.c_str());
      #endif
      ValidateEulerSequence(value);
      UnsignedIntArray newSeq = Attitude::ExtractEulerSequence(value);

      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates"); // updates euler angles and euler angle rates
      #ifdef DEBUG_ATTITUDE_SET
         MessageInterface::ShowMessage("*** Current euler angles are : %12.10f  %12.10f  %12.10f\n",
               eulerAngles[0], eulerAngles[1], eulerAngles[2]);
         MessageInterface::ShowMessage("*** Now setting sequence to %d  %d  %d\n",
               (Integer) newSeq[0], (Integer) newSeq[1], (Integer) newSeq[2]);
      #endif
      eulerSequence         = value;
      eulerSequenceArray.clear();
      eulerSequenceArray    = newSeq;
      // update the DCM and angVel
      (const_cast<Attitude*>(this))->UpdateState("DirectionCosineMatrix");
      (const_cast<Attitude*>(this))->UpdateState("AngularVelocity");
      if (isInitialized) needsReinit = true;
      #ifdef DEBUG_ATTITUDE_SET
         MessageInterface::ShowMessage("****** After updating, euler angles are : %12.10f  %12.10f  %12.10f\n",
               eulerAngles[0], eulerAngles[1], eulerAngles[2]);
      #endif

      return true;
   }

   // now, handle the array values - Dunn added MRPs
   if ((id == QUATERNION) || (id == EULER_ANGLES)     || (id == DIRECTION_COSINE_MATRIX) ||
       (id == MRPS)       || (id == ANGULAR_VELOCITY) || (id == EULER_ANGLE_RATES))
   {
      if ((!setInitialAttitudeAllowed) &&
          (IsInitialAttitudeParameter(id,"Rvector") || (IsInitialAttitudeParameter(id,"Rmatrix"))))
      {
         if (!warnNoAttitudeWritten)
         {
            std::string warnMsg = "*** WARNING *** Setting attitude initial ";
            warnMsg += "conditions has no effect when attitude mode is ";
            warnMsg += attitudeModelName + "\n";
            MessageInterface::ShowMessage(warnMsg);
            warnNoAttitudeWritten = true;
         }
         return true;
      }
      SetRealArrayFromString(id, value);
      return true;
   }

   if (id == ATTITUDE_REFERENCE_BODY)
   {
      if (attitudeModelName != "NadirPointing")
      {
         std::string warnMsg = "*** WARNING *** Setting  ";
         warnMsg += GetParameterText(id);
         warnMsg += " on a non-NadirPointing Attitude model has no effect\n";
         MessageInterface::ShowMessage(warnMsg);
      }
      refBodyName = value;
      return true;
   }
   if (id == ATTITUDE_CONSTRAINT_TYPE)
   {
      if (attitudeModelName != "NadirPointing")
      {
         std::string warnMsg = "*** WARNING *** Setting  ";
         warnMsg += GetParameterText(id);
         warnMsg += " on a non-NadirPointing Attitude model has no effect\n";
         MessageInterface::ShowMessage(warnMsg);
      }
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
   if (id == AEM_FILE_NAME)
   {
      if (value != aemFile)
      {
         // Get full path from the static GmatBase::GetFullPathFileName() (LOJ: 2014.06.26)
         std::string aemFileNoPath;
         aemFileFullPath =
            GmatBase::GetFullPathFileName(aemFileNoPath, GetName(), value,
                         "VEHICLE_EPHEM_CCSDS_PATH", true, "", true, true);
         aemFile = value;
      }
      return true;
   }
   
   return GmatBase::SetStringParameter(id, value);
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
bool Attitude::SetStringParameter(const std::string &label, 
                                  const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns the specified string array value at the given index.
 *
 * @param id    ID for the requested parameter.
 * @param index index into the array
 *
 * @return The requested string.
 */
//------------------------------------------------------------------------------
std::string  Attitude::GetStringParameter(const Integer id,
                                          const Integer index) const
{
   return GmatBase::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id,const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the specified string array value at the given index.
 *
 * @param id    ID for the requested parameter.
 * @param value string value for the parameter
 * @param index index into the array
 *
 * @return flag indicating success or failure
 */
//------------------------------------------------------------------------------
bool Attitude::SetStringParameter(const Integer     id,
                                  const std::string &value,
                                  const Integer     index)
{
   return GmatBase::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string array given the input parameter ID. 
 *
 * @param id ID for the requested parameter.
 * 
 * @return The requested string array.
 */
//------------------------------------------------------------------------------
const StringArray& 
            Attitude::GetStringArrayParameter(const Integer id) const
{
   if (id == EULER_SEQUENCE_LIST)  return eulerSequenceList;
   return GmatBase::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the string array given the input parameter label. 
 *
 * @param label label for the requested parameter.
 * 
 * @return The requested string array.
 */
//------------------------------------------------------------------------------
const StringArray& 
            Attitude::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// StringArray GetGeneratingString(Gmat::WriteMode mode,
//             const std::string &prefix, const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Produces a string, possibly multi-line, containing the text that produces an
 * object.
 *
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 *
 * @return A string containing the text.
 *
 */
//------------------------------------------------------------------------------
const std::string& Attitude::GetGeneratingString(Gmat::WriteMode   mode,
                                                 const std::string &prefix,
                                                 const std::string &useName)
{
   #ifdef DEBUG_ATTITUDE_GEN_STRING
   MessageInterface::ShowMessage
      ("Attitude::GetGeneratingString() this=%p, mode=%d, prefix=%s, "
       "useName=%s\n", this, mode, prefix.c_str(), useName.c_str());
   #endif
   
   std::stringstream data;

   data.precision(18);   // Crank up data precision so we don't lose anything
   std::string preface = "", nomme;

   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::OWNED_OBJECT) ||
       (mode == Gmat::SHOW_SCRIPT))
      inMatlabMode = false;
   if (mode == Gmat::MATLAB_STRUCT || mode == Gmat::EPHEM_HEADER)
      inMatlabMode = true;

   if (useName != "")
      nomme = useName;
   else
      nomme = instanceName;
   nomme += ".";

   if (mode == Gmat::OWNED_OBJECT)
   {
      preface = prefix;
      nomme = "";
   }

   preface += nomme;
   #ifdef DEBUG_ATTITUDE_GEN_STRING
   MessageInterface::ShowMessage
      ("Attitude::GetGeneratingString() about to call WriteParameters ...\n");
   #endif
   WriteParameters(mode, preface, data);
   #ifdef DEBUG_ATTITUDE_GEN_STRING
   MessageInterface::ShowMessage
      ("Attitude::GetGeneratingString() DONE calling WriteParameters ...\n");
   #endif

   generatingString = data.str();
   return generatingString;
}
 
 
 //------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  bool  ValidateCosineMatrix(const Rmatrix33 &mat)
//------------------------------------------------------------------------------
/**
 * This method validates the direction cosine matrix.
 *
 * @param <rMat>  direction cosine matrix to validate
 *
 * @return  flag indicating whether or not the input is a valid DCM
 *
 */
//------------------------------------------------------------------------------
bool Attitude::ValidateCosineMatrix(const Rmatrix33 &mat)
{
   // Check to see that all values meet range checks first
   bool elementOutOfRange = false;
   bool notOrthonormal    = false;

   // check for an element out of range
   for (Integer ii = 0; ii < 3; ii++)
      for (Integer jj = 0; jj < 3; jj++)
         if ((mat(ii,jj) < -1.0 || mat(ii,jj) > 1.0)) elementOutOfRange = true;

   // check for orthonormality
   if (!mat.IsOrthonormal(GmatAttitudeConstants::DCM_ORTHONORMALITY_TOLERANCE)) notOrthonormal = true;

   if (elementOutOfRange || notOrthonormal)
   {
      AttitudeException attex;
      std::ostringstream matS;
      matS.str("");
      matS << "[";
      for (Integer ii = 0; ii < 3; ii++)
         for (Integer jj = 0; jj < 3; jj++)
         {
            matS << mat(ii,jj) ;
            if (!(ii ==3 && jj==3))  matS << " ";
         }
      matS << "]";
      std::string errMsg = "The value of \"" + matS.str();
      errMsg += "\" for field \"" + OTHER_REP_TEXT[DIRECTION_COSINE_MATRIX - OTHER_REPS_OFFSET];
      errMsg += "\" on an object of type \"" + typeName;
      errMsg += "\" is not an allowed value.\n";
      if (elementOutOfRange)
      {
         std::stringstream outofrange;
         outofrange << "The allowed values are: [-1.0 <= each element <= 1.0].";
         errMsg += outofrange.str();
      }
      else // not orthonormal
      {
         std::stringstream ortho;
         ortho << "The allowed values are: [orthogonal matrix]. The tolerance on orthonormality is "
               << GmatAttitudeConstants::DCM_ORTHONORMALITY_TOLERANCE << ".";
         errMsg += ortho.str();
      }
      attex.SetDetails(errMsg);
      throw attex;
   }
   return true;
}

//------------------------------------------------------------------------------
//  bool  ValidateEulerAngles(const Rvector &eAngles,
//                            const UnsignedIntArray &eulSeq)
//------------------------------------------------------------------------------
/**
 * This method validates the euler angles.
 *
 * @param <eAngles>  euler angles to validate
 * @param <eulSeq>   euler sequence
 *
 * @return  flag indicating whether or not the input is a valid set of
 *          euler angles
 *
 */
//------------------------------------------------------------------------------
bool Attitude::ValidateEulerAngles(const Rvector &eAngles,
                                   const UnsignedIntArray &eulSeq)
{
   if (eAngles.GetSize() != 3)
   {
      throw AttitudeException(
            "The Euler Angles must have 3 elements.\n");
   }
   Real angle2 = eAngles[1];
   // check for a nearly singular attitude, for symmetric sequences
   if ((eulSeq.at(0) == eulSeq.at(2)) &&
       (GmatMathUtil::Abs(GmatMathUtil::Sin(angle2)) < GmatAttitudeConstants::EULER_ANGLE_TOLERANCE))
   {
      std::ostringstream errmsg;
      errmsg << "The attitude defined by the input euler angles (";
      errmsg << (eAngles(0) * GmatMathConstants::DEG_PER_RAD) << ", "
             << (eAngles(1) * GmatMathConstants::DEG_PER_RAD) << ", "
             << (eAngles(2) * GmatMathConstants::DEG_PER_RAD);
      errmsg << ") is near a singularity.  The allowed values are:\n";
      errmsg << "For a symmetric sequence EulerAngle2 != 0. ";
      errmsg << "For a non-symmetric sequence EulerAngle2 != 90.  ";
      errmsg << "The tolerance on EulerAngle2 singularity is ";
      errmsg << GmatAttitudeConstants::EULER_ANGLE_TOLERANCE << ".\n";
      throw AttitudeException(errmsg.str());
   }
   // check for a nearly singular attitude, for non-symmetric sequences
   else if ((eulSeq.at(0) != eulSeq.at(2)) &&
            (GmatMathUtil::Abs(GmatMathUtil::Cos(angle2)) < GmatAttitudeConstants::EULER_ANGLE_TOLERANCE))
   {
      std::ostringstream errmsg;
      errmsg << "The attitude defined by the input euler angles (";
      errmsg << (eAngles(0) * GmatMathConstants::DEG_PER_RAD) << ", "
             << (eAngles(1) * GmatMathConstants::DEG_PER_RAD) << ", "
             << (eAngles(2) * GmatMathConstants::DEG_PER_RAD);
      errmsg << ") is near a singularity.  The allowed values are:\n";
      errmsg << "For a symmetric sequence EulerAngle2 != 0. ";
      errmsg << "For a non-symmetric sequence EulerAngle2 != 90.  ";
      errmsg << "The tolerance on EulerAngle2 singularity is ";
      errmsg << GmatAttitudeConstants::EULER_ANGLE_TOLERANCE << ".\n";
      throw AttitudeException(errmsg.str());
   }

   return true;
}

//------------------------------------------------------------------------------
//  bool  ValidateEulerSequence(const std::string &seq)
//------------------------------------------------------------------------------
/**
 * This method validates the euler angle sequence.
 *
 * @param <seq>  euler sequence to validate
 *
 * @return  flag indicating whether or not the input is a valid euler sequence
 *
 */
//------------------------------------------------------------------------------
bool Attitude::ValidateEulerSequence(const std::string &seq)
{
   for (Integer i=0; i<12; i++)
      if (seq == EULER_SEQ_LIST[i])  return true;
   AttitudeException ae;
   std::ostringstream eulSeqs;
   eulSeqs << "One of ";
   for (Integer i=0; i<12; i++)
      eulSeqs << " " << EULER_SEQ_LIST[i];
   ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                 seq.c_str(),
                 "EulerAngleSequence", 
                 (eulSeqs.str()).c_str());
   throw ae;
}

//------------------------------------------------------------------------------
//  bool  ValidateEulerSequence(const UnsignedIntArray &eulAng)
//------------------------------------------------------------------------------
/**
 * This method validates the euler angle sequence.
 *
 * @param <eulAng>  euler sequence to validate
 *
 * @return  flag indicating whether or not the input is a valid euler sequence
 *
 */
//------------------------------------------------------------------------------
bool Attitude::ValidateEulerSequence(const UnsignedIntArray &eulAng)
{
   if (eulAng.size()  != 3)
      throw AttitudeException(
         "Euler Sequence contains too few/many components - cannot convert input\n");
   std::string        eulStr;
   std::ostringstream eulS;
   eulS.str(eulStr); 
   eulS << eulAng[0] << eulAng[1] << eulAng[2];
   return ValidateEulerSequence(eulStr);
}

//------------------------------------------------------------------------------
//  bool  ValidateQuaternion(const Rvector &quat)
//------------------------------------------------------------------------------
/**
 * This method validates the quaternion.
 *
 * @param <quat>  quaternion to validate
 *
 * @return  flag indicating whether or not the input is a valid quaternion
 *
 */
//------------------------------------------------------------------------------
bool Attitude::ValidateQuaternion(const Rvector &quat)
{
   if (quat.GetSize() != 4)
   {
      throw AttitudeException(
            "The quaternion must have 4 elements.\n");
   }
   if (quat.GetMagnitude() < GmatAttitudeConstants::QUAT_MIN_MAG)
   {
      std::string errmsg = "The quaternion magnitude must be greater than 1e-10.\n";
      throw AttitudeException(errmsg);
   }
   return true;
}

//------------------------------------------------------------------------------
//  bool  ValidateMRPs(const Rvector &mrps)
//------------------------------------------------------------------------------
/**
* This method validates the MRPs.
*
* @param <quat>  quaternion to validate
*
* @return  flag indicating whether or not the input is a valid quaternion
*
*/
//------------------------------------------------------------------------------
bool Attitude::ValidateMRPs(const Rvector &mrps)
{
   // Note from Dunn.  There may be some more qualities of MRPs we can check
   // here.
   if (mrps.GetSize() != 3)
   {
      throw AttitudeException(
         "The MRP vector must have 3 elements.\n");
   }
   return true;
}


//------------------------------------------------------------------------------
//  void  UpdateState(const std::string &rep)
//------------------------------------------------------------------------------
/**
 * This method updates the attitude in the representation specified.
 *
 * @param <rep>  representation in which to update the attitude
 *
 */
//------------------------------------------------------------------------------
void Attitude::UpdateState(const std::string &rep)
{
   if (rep == "Quaternion")
   {
      if (inputAttitudeType == GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE)
         quaternion       = AttitudeConversionUtility::ToQuaternion(dcm);
      else if (inputAttitudeType == GmatAttitude::MODIFIED_RODRIGUES_PARAMETERS_TYPE)
         quaternion       = AttitudeConversionUtility::ToQuaternion(mrps);  // Dunn Added
      else if (inputAttitudeType == GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE)
         quaternion       = AttitudeConversionUtility::ToQuaternion(eulerAngles,
                            (Integer) eulerSequenceArray.at(0),
                            (Integer) eulerSequenceArray.at(1),
                            (Integer) eulerSequenceArray.at(2));
      // else it's already the up-to-date quaternion (as input)
   }
   else if (rep == "EulerAngles")
   {
      if (inputAttitudeType == GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE)
         eulerAngles = AttitudeConversionUtility::ToEulerAngles(dcm,
                                 (Integer) eulerSequenceArray.at(0),
                                 (Integer) eulerSequenceArray.at(1),
                                 (Integer) eulerSequenceArray.at(2));
      else if (inputAttitudeType == GmatAttitude::MODIFIED_RODRIGUES_PARAMETERS_TYPE)
      {
         quaternion       = AttitudeConversionUtility::ToQuaternion(mrps);  // Dunn Added
         eulerAngles = AttitudeConversionUtility::ToEulerAngles(quaternion,
                                               (Integer) eulerSequenceArray.at(0),
                                               (Integer) eulerSequenceArray.at(1),
                                               (Integer) eulerSequenceArray.at(2));
      }
      else if (inputAttitudeType == GmatAttitude::QUATERNION_TYPE)
         eulerAngles = AttitudeConversionUtility::ToEulerAngles(quaternion,
                       (Integer) eulerSequenceArray.at(0),
                       (Integer) eulerSequenceArray.at(1),
                       (Integer) eulerSequenceArray.at(2));
      // else it is already the up-to-date euler angles (as input)
   }
   else if (rep == "DirectionCosineMatrix")
   {
      if (inputAttitudeType == GmatAttitude::QUATERNION_TYPE)
         dcm = AttitudeConversionUtility::ToCosineMatrix(quaternion);
      else if (inputAttitudeType == GmatAttitude::MODIFIED_RODRIGUES_PARAMETERS_TYPE)
      {
         quaternion = AttitudeConversionUtility::ToQuaternion(mrps);  // Dunn Added
         dcm        = AttitudeConversionUtility::ToCosineMatrix(quaternion);
      }
      else if (inputAttitudeType == GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE)
         dcm = AttitudeConversionUtility::ToCosineMatrix(eulerAngles,
                             (Integer) eulerSequenceArray.at(0),
                             (Integer) eulerSequenceArray.at(1),
                             (Integer) eulerSequenceArray.at(2));
      // else it is already the up-to-date DCM (as input OR as kept up-to-date internally)
   }
   else if (rep == "MRPs") // Dunn Added
   {
      if (inputAttitudeType == GmatAttitude::QUATERNION_TYPE)
         mrps = AttitudeConversionUtility::ToMRPs(quaternion);
      else if (inputAttitudeType == GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE)
      {
         quaternion = AttitudeConversionUtility::ToQuaternion(dcm);
         mrps       = AttitudeConversionUtility::ToMRPs(quaternion);
      }
      else if (inputAttitudeType == GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE)
      {
         dcm = AttitudeConversionUtility::ToCosineMatrix(eulerAngles,
                             (Integer) eulerSequenceArray.at(0),
                             (Integer) eulerSequenceArray.at(1),
                             (Integer) eulerSequenceArray.at(2));
         quaternion = AttitudeConversionUtility::ToQuaternion(dcm);
         mrps = AttitudeConversionUtility::ToMRPs(quaternion);
      }

   }
   else if (rep == "EulerAngleRates")
   {
      #ifdef DEBUG_UPDATE_STATE
         MessageInterface::ShowMessage("UpdateState for EAR ... \n");
         MessageInterface::ShowMessage(" ... eulerAngles = %s\n", eulerAngles.ToString().c_str());
         MessageInterface::ShowMessage(" ... mrps        = %s\n", mrps.ToString().c_str());
         MessageInterface::ShowMessage(" ... euler sequence array = %d %d %d\n",
               (Integer) eulerSequenceArray.at(0),
               (Integer) eulerSequenceArray.at(1),
               (Integer) eulerSequenceArray.at(2));
      #endif
      if (inputAttitudeRateType == GmatAttitude::ANGULAR_VELOCITY_TYPE)
      {
         #ifdef DEBUG_UPDATE_STATE
            MessageInterface::ShowMessage("UpdateState for EAR ... type is Angular Velocity\n");
         #endif
         (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
         eulerAngleRates   = AttitudeConversionUtility::ToEulerAngleRates(angVel,
                             eulerAngles,
                             (Integer) eulerSequenceArray.at(0),
                             (Integer) eulerSequenceArray.at(1),
                             (Integer) eulerSequenceArray.at(2));
         #ifdef DEBUG_UPDATE_STATE
            MessageInterface::ShowMessage("UpdateState for EAR after conversion, eulerAngleRates = %s\n",
                  eulerAngleRates.ToString().c_str());
         #endif
      }
      // else it is already the up-to-date euler angle rates (as input)
   }
   else if (rep == "AngularVelocity")
   {
      if (inputAttitudeRateType == GmatAttitude::EULER_ANGLE_RATES_TYPE)
      {
         (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
         angVel            = AttitudeConversionUtility::ToAngularVelocity(eulerAngleRates, eulerAngles,
                             (Integer) eulerSequenceArray.at(0),
                             (Integer) eulerSequenceArray.at(1),
                             (Integer) eulerSequenceArray.at(2));
      }
      // else it is already the up-to-date angular velocity (as input OR as kept up-to-date internally)
   }
//   else
//      ; // do nothing - dcm and angVel are kept up-to-date
}

//------------------------------------------------------------------------------
//  void  SetRealArrayFromString(Integer id, const std::string &sval)
//------------------------------------------------------------------------------
/**
 * This method sets the real array specified by the input id using the
 * input string.
 *
 * @param id      Id of the real array to set
 * @param sval    String to use to set real array
 *
 */
//------------------------------------------------------------------------------
void Attitude::SetRealArrayFromString(Integer id, const std::string &sval)
{
   #ifdef DEBUG_ATTITUDE_SET_REAL
      MessageInterface::ShowMessage("Entering SetRealArrayFromString with id = %d (%s) and string = %s\n",
            id, GetParameterText(id).c_str(), sval.c_str());
   #endif
   if (id == DIRECTION_COSINE_MATRIX)
   {
      std::string errmsg = "ERROR - setting DCM from string not yet implemented \n";
      throw AttitudeException(errmsg);
   }
   try
   {
      RealArray vals = GmatStringUtil::ToRealArray(sval);
      if (vals.empty())
      {
         std::string errmsg = "ERROR attempting to set Attitude or Angular Velocity from string \"";
         errmsg += sval + "\"\n";
         throw AttitudeException(errmsg);
      }
      //Changed to call SetRvectorParameter() - LOJ: 2012.03.30
      //for (UnsignedInt i=0; i<vals.size(); i++)
      //   SetRealParameter(id, vals[i], i);
      Rvector rvec = Rvector(vals.size());
      for (UnsignedInt i=0; i<vals.size(); i++)
         rvec[i] = vals[i];
      SetRvectorParameter(id, rvec);
   }
   catch (BaseException &)
   {
      std::string errmsg = "ERROR attempting to set Attitude or Angular Velocity from string \"";
      errmsg += sval + "\"\n";
      throw AttitudeException(errmsg);
   }
}

//------------------------------------------------------------------------------
// GetPrecessingSpinnerParameter
//------------------------------------------------------------------------------
Real Attitude::GetPrecessingSpinnerRealParameter(const Integer id) const
{
   if (attitudeModelName != "PrecessingSpinner")
   {
//      std::string warnMsg = "*** WARNING *** Requesting  ";
//      warnMsg += GetParameterText(id);
//      warnMsg += " on a non-PrecessingSpinner Attitude model\n";
//      MessageInterface::ShowMessage(warnMsg);
   }
   if (id == NUTATION_REFERENCE_VECTOR_X)
   {
      return nutationReferenceVector(0);
   }
   else if (id == NUTATION_REFERENCE_VECTOR_Y)
   {
      return nutationReferenceVector(1);
   }
   else if (id == NUTATION_REFERENCE_VECTOR_Z)
   {
      return nutationReferenceVector(2);
   }
   else if (id == BODY_SPIN_AXIS_X)
   {
      return bodySpinAxis(0);
   }
   else if (id == BODY_SPIN_AXIS_Y)
   {
      return bodySpinAxis(1);
   }
   else if (id == BODY_SPIN_AXIS_Z)
   {
      return bodySpinAxis(2);
   }
   else if (id == INITIAL_PRECESSION_ANGLE)
   {
      return initialPrecessionAngle * GmatMathConstants::DEG_PER_RAD;
   }
   else if (id == PRECESSION_RATE)
   {
      return precessionRate * GmatMathConstants::DEG_PER_RAD;
   }
   else if (id == NUTATION_ANGLE)
   {
      return nutationAngle * GmatMathConstants::DEG_PER_RAD;
   }
   else if (id == INITIAL_SPIN_ANGLE)
   {
      return initialSpinAngle * GmatMathConstants::DEG_PER_RAD;
   }
   else if (id == SPIN_RATE)
   {
      return spinRate * GmatMathConstants::DEG_PER_RAD;
   }
   // Something weird happened
   return REAL_PARAMETER_UNDEFINED;
}

//------------------------------------------------------------------------------
// SetPrecessingSpinnerParameter
//------------------------------------------------------------------------------
Real Attitude::SetPrecessingSpinnerRealParameter(const Integer id, const Real value)
{
   if (attitudeModelName != "PrecessingSpinner")
   {
      std::string warnMsg = "*** WARNING *** Setting  ";
      warnMsg += GetParameterText(id);
      warnMsg += " on a non-PrecessingSpinner Attitude model has no effect\n";
      MessageInterface::ShowMessage(warnMsg);
   }
   if (id == NUTATION_REFERENCE_VECTOR_X)
   {
      nutationReferenceVector(0) = value;
      return nutationReferenceVector(0);
   }
   if (id == NUTATION_REFERENCE_VECTOR_Y)
   {
      nutationReferenceVector(1) = value;
      return nutationReferenceVector(1);
   }
   if (id == NUTATION_REFERENCE_VECTOR_Z)
   {
      nutationReferenceVector(2) = value;
      return nutationReferenceVector(2);
   }
   if (id == BODY_SPIN_AXIS_X)
   {
      bodySpinAxis(0) = value;
      return bodySpinAxis(0);
   }
   if (id == BODY_SPIN_AXIS_Y)
   {
      bodySpinAxis(1) = value;
      return bodySpinAxis(1);
   }
   if (id == BODY_SPIN_AXIS_Z)
   {
      bodySpinAxis(2) = value;
      return bodySpinAxis(2);
   }
   if (id == INITIAL_PRECESSION_ANGLE)
   {
      initialPrecessionAngle = value * GmatMathConstants::RAD_PER_DEG;
      return initialPrecessionAngle;
   }
   if (id == PRECESSION_RATE)
   {
      precessionRate = value * GmatMathConstants::RAD_PER_DEG;
      return precessionRate;
   }
   if (id == NUTATION_ANGLE)
   {
      nutationAngle = value * GmatMathConstants::RAD_PER_DEG;
      return nutationAngle;
   }
   if (id == INITIAL_SPIN_ANGLE)
   {
      initialSpinAngle = value * GmatMathConstants::RAD_PER_DEG;
      return initialSpinAngle;
   }
   if (id == SPIN_RATE)
   {
      spinRate = value * GmatMathConstants::RAD_PER_DEG;
      return spinRate;
   }
   // Something weird happened
   return REAL_PARAMETER_UNDEFINED;
}

//------------------------------------------------------------------------------
// GetNadirPointingParameter
//------------------------------------------------------------------------------
Real Attitude::GetNadirPointingRealParameter(const Integer id) const
{
   if (attitudeModelName != "NadirPointing")
   {
//      std::string warnMsg = "*** WARNING *** Requesting  ";
//      warnMsg += GetParameterText(id);
//      warnMsg += " on a non-NadirPointing Attitude model\n";
//      MessageInterface::ShowMessage(warnMsg);
   }
   if (id == BODY_ALIGNMENT_VECTOR_X)
   {
      return bodyAlignmentVector(0);
   }
   if (id == BODY_ALIGNMENT_VECTOR_Y)
   {
      return bodyAlignmentVector(1);
   }
   if (id == BODY_ALIGNMENT_VECTOR_Z)
   {
      return bodyAlignmentVector(2);
   }
   if (id == BODY_CONSTRAINT_VECTOR_X)
   {
      return bodyConstraintVector(0);
   }
   if (id == BODY_CONSTRAINT_VECTOR_Y)
   {
      return bodyConstraintVector(1);
   }
   if (id == BODY_CONSTRAINT_VECTOR_Z)
   {
      return bodyConstraintVector(2);
   }
   // Something weird happened
   return REAL_PARAMETER_UNDEFINED;
}

//------------------------------------------------------------------------------
// SetNadirPointingParameter
//------------------------------------------------------------------------------
Real Attitude::SetNadirPointingRealParameter(const Integer id, const Real value)
{
   if (attitudeModelName != "NadirPointing")
   {
      std::string warnMsg = "*** WARNING *** Setting  ";
      warnMsg += GetParameterText(id);
      warnMsg += " on a non-NadirPointing Attitude model has no effect\n";
      MessageInterface::ShowMessage(warnMsg);
   }
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
   // Something weird happened
   return REAL_PARAMETER_UNDEFINED;
}


//------------------------------------------------------------------------------
//  bool  IsInitialAttitudeParameter(Integer id, std::string ofType = "Any") const
//------------------------------------------------------------------------------
/**
 * Returns true if this parameter ID is the ID for an initial attitude parameter.
 *
 * @param id      Id of the real array to set
 * @param ofType  type of parameter
 *
 * @return true if it is an initial attitude parameter; false otherwise
 */
//------------------------------------------------------------------------------
bool Attitude::IsInitialAttitudeParameter(Integer id, std::string ofType) const
{
   if ((ofType == "Real") || (ofType == "Any"))
   {
      if ((id == Q_1) || (id == Q_2) || (id == Q_3) || (id == Q_4) ||
          (id == EULER_ANGLE_1) || (id == EULER_ANGLE_2) ||
          (id == EULER_ANGLE_3) ||
          (id == MRP_1)  || (id == MRP_2)  || (id == MRP_3)  ||
          (id == DCM_11) || (id == DCM_12) || (id == DCM_13) ||
          (id == DCM_21) || (id == DCM_22) || (id == DCM_23) ||
          (id == DCM_31) || (id == DCM_32) || (id == DCM_33) ||
          (id == EULER_ANGLE_RATE_1) || (id == EULER_ANGLE_RATE_2) ||
          (id == EULER_ANGLE_RATE_3) ||
          (id == ANGULAR_VELOCITY_X) || (id == ANGULAR_VELOCITY_Y) ||
          (id == ANGULAR_VELOCITY_Z))
      {
         return true;
      }
   }
   if ((ofType == "Rvector") || (ofType == "Any"))
   {
      if ((id == QUATERNION)              || (id == EULER_ANGLES)     ||
          (id == DIRECTION_COSINE_MATRIX) || (id == MRPS)             ||
          (id == ANGULAR_VELOCITY)        || (id == EULER_ANGLE_RATES))
      {
         return true;
      }
   }
   if ((ofType == "Rmatrix") || (ofType == "Any"))
   {
      if (id == DIRECTION_COSINE_MATRIX)
      {
         return true;
      }
   }
   return false;
}


//-------------------------------------------------------------------------------------------------
// Rmatrix33 GetRotationMatrix(GmatTime &epochGT)
//-------------------------------------------------------------------------------------------------
/**
* Get rotation matrix from spacecraft's attitude frame (B-frame) to inertial frame (I-frame) at 
* a given time.
*
* @epochGT   epoch at which rotation matrix is specified.
*
* @return    rotation matrix from spaceraft's attitude frame (B-frame) to inertial frame (I-frame)
*/
//-------------------------------------------------------------------------------------------------
Rmatrix33 Attitude::GetRotationMatrix(const GmatTime &epochGT)
{
   // Compute dcm matrix and angles' velocity w.r.t. MJ2000Eq
   ComputeCosineMatrixAndAngularVelocity(epochGT.GetMjd());

   // dcm.Transpose() is rotation matrix from spacecraft's attitude frame (B-frame) to inertial frame (I-frame)
   Rmatrix33 MT = dcm.Transpose();

	return MT;
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
std::vector<Rmatrix33> Attitude::GetRotationMatrixDerivative(GmatTime &epochGT, CoordinateSystem *j2kCS)
{
	// 1. Specify spacecraft attitude's coordinate system
	CoordinateSystem *attCS = NULL;
   std::string attCSName = GetStringParameter("AttitudeCoordinateSystem");
   attCS = (CoordinateSystem *)(GetRefObject(Gmat::COORDINATE_SYSTEM, attCSName));
	
   if (attCS == NULL)
	{
		throw GmatBaseException("Error: ********************!!!!!!!!!!!!!!!\n");
	}

	// 2. Specify rotation matrix [M]^T from spacecraft's attitude frame to spacecraft's inertial frame 
	Rvector inState(6, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
	Rvector outState(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	CoordinateConverter cv;

	cv.SetToCalculateRotMatrixDeriv(true);                  // turn flag to calculate rotation matrix derivative on
	cv.Convert(epochGT, inState, attCS, outState, j2kCS);
	cv.SetToCalculateRotMatrixDeriv(false);                 // reset to default flag

	std::vector<Rmatrix33> MTDeriv = cv.GetLastRotationMatrixDerivative();

	return MTDeriv;
}
