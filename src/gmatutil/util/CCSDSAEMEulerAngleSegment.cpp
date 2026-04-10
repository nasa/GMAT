//------------------------------------------------------------------------------
//                                  CCSDSAEMEulerAngleSegment
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
// Author: Wendy Shoan / NASA
// Created: 2013.12.11
//
/**
 * Stores, validates, and manages meta data and real Euler Angle data read from,
 * or to be written to, a CCSDS Attitude Ephemeris Message file.
 */
//------------------------------------------------------------------------------
#include "CCSDSAEMEulerAngleSegment.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"
#include "AttitudeConversionUtility.hpp"

//#define DEBUG_AEM_EULER_VALIDATE
//#define DEBUG_AEM_EULER_SET
//#define DEBUG_AEM_EULER_DATA
//#define DEBUG_AEM_EULER_GET_STATE

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// static methods
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructor (default)
//------------------------------------------------------------------------------
CCSDSAEMEulerAngleSegment::CCSDSAEMEulerAngleSegment(const std::string version,
                                                     const Integer segNum) :
   CCSDSAEMSegment(version, segNum),
   eulerRotSeq (UNSET_STRING),
   euler1      (3),
   euler2      (2),
   euler3      (1)
{
   dataSize           = 3;
   dataType           = "EULER_ANGLE";
   checkLagrangeOrder = true;
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
CCSDSAEMEulerAngleSegment::CCSDSAEMEulerAngleSegment(
                           const CCSDSAEMEulerAngleSegment &copy) :
   CCSDSAEMSegment(copy),
   eulerRotSeq (copy.eulerRotSeq),
   euler1      (copy.euler1),
   euler2      (copy.euler2),
   euler3      (copy.euler3)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
CCSDSAEMEulerAngleSegment& CCSDSAEMEulerAngleSegment::operator=(
                           const CCSDSAEMEulerAngleSegment &copy)
{
   if (&copy == this)
      return *this;

   CCSDSAEMSegment::operator=(copy);

   eulerRotSeq = copy.eulerRotSeq;
   euler1      = copy.euler1;
   euler2      = copy.euler2;
   euler3      = copy.euler3;

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CCSDSAEMEulerAngleSegment::~CCSDSAEMEulerAngleSegment()
{
}

//------------------------------------------------------------------------------
// Validates the contents of the handled Euler Angle meta data elements.
//------------------------------------------------------------------------------
bool CCSDSAEMEulerAngleSegment::Validate(bool checkData)
{
   #ifdef DEBUG_AEM_EULER_VALIDATE
      MessageInterface::ShowMessage("Entering CCSDSAEMEulerAngleSegment::Validate\n");
   #endif

   // Rotation Sequence
   if (eulerRotSeq == UNSET_STRING)
   {
      std::string errmsg = segError;
      errmsg += "Required field EULER_ROT_SEQ is missing.\n";
      throw UtilityException(errmsg);
   }
   if (interpolationMethod != "LAGRANGE")
   {
      std::string errmsg = segError;
      errmsg += "Interpolation type \"";
      errmsg += interpolationMethod + "\" is not valid for Attitude type ";
      errmsg += "EULER_ANGLE.  The only supported value is \"LAGRANGE\".\n";
      throw UtilityException(errmsg);
   }

   return CCSDSAEMSegment::Validate(checkData);
}

//------------------------------------------------------------------------------
// Sets the corresponding meta data for the input field name.
//------------------------------------------------------------------------------
bool CCSDSAEMEulerAngleSegment::SetMetaData(const std::string &fieldName,
                                const std::string &value)
{
   #ifdef DEBUG_AEM_EULER_SET
      MessageInterface::ShowMessage(
            "Entering AEMEulerAngleSegment::SetMetaData with field %s and value %s\n",
            fieldName.c_str(), value.c_str());
   #endif
   if (fieldName == "EULER_ROT_SEQ")
   {
      // Checking here for valid Euler sequence; however, the standard states
      // that, while any sequence is allowed, symmetric ones are not
      // recommended.  We are not checking for that here at this time.
      if (!AttitudeConversionUtility::IsValidEulerSequence(value))
      {
         std::string errmsg = segError;
         errmsg += "Invalid value for field EULER_ROT_SEQ.\n";
         throw UtilityException(errmsg);
      }
      eulerRotSeq = value;
      GmatStringUtil::ToInteger(eulerRotSeq.substr(0,1), euler1);
      GmatStringUtil::ToInteger(eulerRotSeq.substr(1,1), euler2);
      GmatStringUtil::ToInteger(eulerRotSeq.substr(2,1), euler3);
      return true;
   }
   // if it isn't handled here, call the parent class
   return CCSDSAEMSegment::SetMetaData(fieldName, value);
}

//------------------------------------------------------------------------------
// Returns a clone of this object.
//------------------------------------------------------------------------------
CCSDSEMSegment* CCSDSAEMEulerAngleSegment::Clone() const
{
   return (new CCSDSAEMEulerAngleSegment(*this));
}

//------------------------------------------------------------------------------
// Returns the attitude (DCM) state from inertial-to-body at the specified
// input time.
//------------------------------------------------------------------------------
Rmatrix33 CCSDSAEMEulerAngleSegment::GetState(Real atEpoch)
{
   #ifdef DEBUG_AEM_EULER_GET_STATE
      MessageInterface::ShowMessage("====== Entering EulerSegment::GetState with epoch = %lf, dataStore size = %d\n",
            atEpoch, (Integer) dataStore.size());
   #endif
   // DetermineState will look for an exact match; if so,
   // return the state at that time; if not, then return the last
   // state (if interpolation degree = 0) or else, interpolate
   // to the requested time
   Rvector  eulerAngles(3);
   eulerAngles = DetermineState(atEpoch);
   #ifdef DEBUG_AEM_EULER_GET_STATE
      MessageInterface::ShowMessage("          eulerAngles (deg) from DetermineState are: %12.10f  %12.10f  %12.10f\n",
            eulerAngles[0] * GmatMathConstants::DEG_PER_RAD,
            eulerAngles[1] * GmatMathConstants::DEG_PER_RAD,
            eulerAngles[2] * GmatMathConstants::DEG_PER_RAD);
      MessageInterface::ShowMessage("          and euler sequence is: %d   %d   %d\n", euler1, euler2, euler3);
   #endif

   // Conversion method has to have an Rvector3
   Rvector3  theEulerAngles(eulerAngles(0), eulerAngles(1), eulerAngles(2));
   Rmatrix33 theDCM = AttitudeConversionUtility::ToCosineMatrix(
                      theEulerAngles, euler1, euler2, euler3);
   #ifdef DEBUG_AEM_EULER_GET_STATE
      MessageInterface::ShowMessage("About to Exit EulerSegment::GetState, DCM = %s\n",
            theDCM.ToString().c_str());
   #endif
   if (inertialToBody) return theDCM;
   else                return theDCM.Transpose();
}

//------------------------------------------------------------------------------
// Adds an epoch/data pair to the dataStore.
//------------------------------------------------------------------------------
bool CCSDSAEMEulerAngleSegment::AddData(Real epoch, Rvector data,
                                        bool justCheckDataSize)
{
   #ifdef DEBUG_AEM_EULER_DATA
   MessageInterface::ShowMessage
      ("CCSDSAEMEulerAngleSegment::AddData() entered, epoch=%f, dataStore.size()=%d\n",
       epoch, dataStore.size());
   #endif
   // First, check for data size and ordering
   CCSDSAEMSegment::AddData(epoch, data, justCheckDataSize);
   // We need to store the angles in radians
   Rvector useData(3, data[0] * GmatMathConstants::RAD_PER_DEG,
                      data[1] * GmatMathConstants::RAD_PER_DEG,
                      data[2] * GmatMathConstants::RAD_PER_DEG);

   if (!ValidateEulerAngles(useData))
   {
      std::string errmsg = segError;
      errmsg += "Data within DATA segment are not valid Euler Angles.\n";
      throw UtilityException(errmsg);
   }
   EpochAndData *newData = new EpochAndData();
   newData->epoch = epoch;
   newData->data  = useData;
   dataStore.push_back(newData);

   #ifdef DEBUG_AEM_EULER_DATA
      MessageInterface::ShowMessage("   dataStore.size() = %d\n", dataStore.size());
      MessageInterface::ShowMessage("----> For (Euler Angle) segment number %d, added epoch = "
            "%12.10f and data = %12.10f  %12.10f  %12.10f\n",
            segmentNumber, epoch, useData[0], useData[1], useData[2]);
   #endif

   return true;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Interpolates the data.
//------------------------------------------------------------------------------
Rvector CCSDSAEMEulerAngleSegment::Interpolate(Real atEpoch)
{
   return InterpolateLagrange(atEpoch);
}

//------------------------------------------------------------------------------
//  Validates the Euler Angle size and checks for singularities.
//------------------------------------------------------------------------------
bool CCSDSAEMEulerAngleSegment::ValidateEulerAngles(const Rvector &eAngles)
{
   if (eAngles.GetSize() != 3)
   {
      return false;
   }
   Real angle2 = eAngles[1];
   // check for a nearly singular attitude, for symmetric sequences
   if ((euler1 == euler3) &&
       (GmatMathUtil::Abs(GmatMathUtil::Sin(angle2)) < GmatAttitudeConstants::EULER_ANGLE_TOLERANCE))
   {
//      std::ostringstream errmsg;
//      errmsg << "The attitude defined by the input euler angles (";
//      errmsg << (eAngles(0) * GmatMathConstants::DEG_PER_RAD) << ", "
//             << (eAngles(1) * GmatMathConstants::DEG_PER_RAD) << ", "
//             << (eAngles(2) * GmatMathConstants::DEG_PER_RAD);
//      errmsg << ") is near a singularity.  The allowed values are:\n";
//      errmsg << "For a symmetric sequence EulerAngle2 != 0. ";
//      errmsg << "For a non-symmetric sequence EulerAngle2 != 90.  ";
//      errmsg << "The tolerance on EulerAngle2 singularity is ";
//      errmsg << GmatAttitudeConstants::EULER_ANGLE_TOLERANCE << ".\n";
//      throw UtilityException(errmsg.str());
      return false;
   }
   // check for a nearly singular attitude, for non-symmetric sequences
   else if ((euler1 != euler3) &&
            (GmatMathUtil::Abs(GmatMathUtil::Cos(angle2)) < GmatAttitudeConstants::EULER_ANGLE_TOLERANCE))
   {
//      std::ostringstream errmsg;
//      errmsg << "The attitude defined by the input euler angles (";
//      errmsg << (eAngles(0) * GmatMathConstants::DEG_PER_RAD) << ", "
//             << (eAngles(1) * GmatMathConstants::DEG_PER_RAD) << ", "
//             << (eAngles(2) * GmatMathConstants::DEG_PER_RAD);
//      errmsg << ") is near a singularity.  The allowed values are:\n";
//      errmsg << "For a symmetric sequence EulerAngle2 != 0. ";
//      errmsg << "For a non-symmetric sequence EulerAngle2 != 90.  ";
//      errmsg << "The tolerance on EulerAngle2 singularity is ";
//      errmsg << GmatAttitudeConstants::EULER_ANGLE_TOLERANCE << ".\n";
//      throw UtilityException(errmsg.str());
      return false;
   }

   return true;
}

