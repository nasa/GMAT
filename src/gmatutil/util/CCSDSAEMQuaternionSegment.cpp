//------------------------------------------------------------------------------
//                                  CCSDSAEMQuaternionSegment
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
// Created: 2013.12.03
//
/**
 * Stores, validates, and manages meta data and real Quaternion data read from,
 * or to be written to, a CCSDS Attitude Ephemeris Message file.
 */
//------------------------------------------------------------------------------
#include "CCSDSAEMQuaternionSegment.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"
#include "AttitudeConversionUtility.hpp"

//#define DEBUG_AEM_QUAT_VALIDATE
//#define DEBUG_AEM_QUAT_SET
//#define DEBUG_AEM_QUAT_DATA
//#define DEBUG_AEM_QUAT_GET_STATE

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
CCSDSAEMQuaternionSegment::CCSDSAEMQuaternionSegment(const std::string version,
                                                     const Integer segNum) :
   CCSDSAEMSegment(version, segNum),
   quaternionType (UNSET_STRING)
{
   dataSize = 4;
   dataType = "QUATERNION";
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
CCSDSAEMQuaternionSegment::CCSDSAEMQuaternionSegment(
                           const CCSDSAEMQuaternionSegment &copy) :
   CCSDSAEMSegment(copy),
   quaternionType (copy.quaternionType)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
CCSDSAEMQuaternionSegment& CCSDSAEMQuaternionSegment::operator=(
                           const CCSDSAEMQuaternionSegment &copy)
{
   if (&copy == this)
      return *this;

   CCSDSAEMSegment::operator=(copy);

   quaternionType = copy.quaternionType;

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CCSDSAEMQuaternionSegment::~CCSDSAEMQuaternionSegment()
{
}

//------------------------------------------------------------------------------
// Validates the contents of the handled Quaternion meta data elements.
//------------------------------------------------------------------------------
bool CCSDSAEMQuaternionSegment::Validate(bool checkData)
{
   #ifdef DEBUG_AEM_QUAT_VALIDATE
      MessageInterface::ShowMessage("Entering CCSDSAEMQuaternionSegment::Validate\n");
   #endif

   // Quaternion type
   if (quaternionType == UNSET_STRING)
   {
      std::string errmsg = segError;
      errmsg += "Required field QUATERNION_TYPE is missing.\n";
      throw UtilityException(errmsg);
   }
   if (interpolationMethod != "LINEAR")
   {
      std::string errmsg = segError;
      errmsg += "Interpolation type \"";
      errmsg += interpolationMethod + "\" is not valid for Attitude type ";
      errmsg += "QUATERNION.  The only supported value is \"LINEAR\".\n";
      throw UtilityException(errmsg);
   }

   #ifdef DEBUG_AEM_QUAT_VALIDATE
      MessageInterface::ShowMessage("EXITing CCSDSAEMQuaternionSegment::Validate\n");
   #endif
   return CCSDSAEMSegment::Validate(checkData);
}

//------------------------------------------------------------------------------
// Sets the corresponding meta data for the input field name.
//------------------------------------------------------------------------------
bool CCSDSAEMQuaternionSegment::SetMetaData(const std::string &fieldName,
                                            const std::string &value)
{
   #ifdef DEBUG_AEM_QUAT_SET
      MessageInterface::ShowMessage(
            "Entering AEMQuaternionSegment::SetMetaData with field %s and value %s\n",
            fieldName.c_str(), value.c_str());
   #endif
   if (fieldName == "QUATERNION_TYPE")
   {
      quaternionType = GmatStringUtil::ToUpper(value);
      if ((quaternionType != "FIRST") && (quaternionType != "LAST"))
      {
         std::string errmsg = segError;
         errmsg += "Invalid value for field QUATERNION_TYPE.  Type must be ";
         errmsg += "either FIRST or LAST.\n";
         throw UtilityException(errmsg);
      }
      return true;
   }
   // if it isn't handled here, call the parent class
   return CCSDSAEMSegment::SetMetaData(fieldName, value);
}

//------------------------------------------------------------------------------
// Returns a clone of this object.
//------------------------------------------------------------------------------
CCSDSEMSegment* CCSDSAEMQuaternionSegment::Clone() const
{
   return (new CCSDSAEMQuaternionSegment(*this));
}

//------------------------------------------------------------------------------
// Returns the attitude (DCM) state from inertial-to-body at the specified
// input time.
//------------------------------------------------------------------------------
Rmatrix33 CCSDSAEMQuaternionSegment::GetState(Real atEpoch)
{
   #ifdef DEBUG_AEM_QUAT_GET_STATE
      MessageInterface::ShowMessage("====== Entering QuatSegment::GetState with "
            "epoch = %lf, dataStore size = %d\n",
            atEpoch, (Integer) dataStore.size());
   #endif
   // DetermineState will look for an exact match; if so,
   // return the state at that time; if not, then return the last
   // state (if interpolation degree = 0) or else, interpolate
   // to the requested time
//   Rvector   quaternion(4);
//   quaternion = DetermineState(atEpoch);

   //   Rmatrix33 theDCM = AttitudeConversionUtility::ToCosineMatrix(quaternion);
   Rmatrix33 theDCM = AttitudeConversionUtility::ToCosineMatrix(DetermineState(atEpoch));
   #ifdef DEBUG_AEM_QUAT_GET_STATE
      MessageInterface::ShowMessage("About to Exit QuatSegment::GetState, DCM = %s\n",
            theDCM.ToString().c_str());
   #endif
   if (inertialToBody) return theDCM;
   else                return theDCM.Transpose();
}

//------------------------------------------------------------------------------
// Adds an epoch/data pair to the dataStore.
//------------------------------------------------------------------------------
bool CCSDSAEMQuaternionSegment::AddData(Real epoch, Rvector data,
                                        bool justCheckDataSize)
{
   // First, check for data size and ordering
   CCSDSAEMSegment::AddData(epoch, data, justCheckDataSize);
   
   if (!ValidateQuaternion(data))
   {
      std::string errmsg = segError;
      errmsg += "Data within DATA segment do not represent ";
      errmsg += "a valid quaternion.\n";
      throw UtilityException(errmsg);
   }
   Rvector useData(4, data[0], data[1], data[2], data[3]);
   EpochAndData *newData = new EpochAndData();
   newData->epoch = epoch;
   // We want to store the data in LAST order
   if (quaternionType == "FIRST")
   {
      useData.Set(4,data[1], data[2], data[3], data[0]);
   }
   newData->data  = useData;
   dataStore.push_back(newData);

   return true;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Interpolates the data.
//------------------------------------------------------------------------------
Rvector CCSDSAEMQuaternionSegment::Interpolate(Real atEpoch)
{
   return InterpolateSLERP(atEpoch);
}

//------------------------------------------------------------------------------
//  Validates the quaternion size and magnitude.
//------------------------------------------------------------------------------
bool CCSDSAEMQuaternionSegment::ValidateQuaternion(const Rvector &quat)
{
   if (quat.GetSize() != 4)
   {
      return false;
   }
   if (quat.GetMagnitude() < GmatAttitudeConstants::QUAT_MIN_MAG)
   {
      return false;
   }
   return true;
}
