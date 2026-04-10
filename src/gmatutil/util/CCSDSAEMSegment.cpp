//------------------------------------------------------------------------------
//                                  CCSDSAEMSegment
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
 * Stores, validates, and manages meta data and real data read from, or
 * to be written to, a CCSDS Attitude Ephemeris Message file.
 * This is the abstract base class from which other classes must derive (e.g.
 * a class to handle quaternion segments).
 */
//------------------------------------------------------------------------------
#include "CCSDSAEMSegment.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"

//#define DEBUG_AEM_VALIDATE
//#define DEBUG_AEM_SET_META

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
CCSDSAEMSegment::CCSDSAEMSegment(const std::string version, const Integer segNum) :
   CCSDSEMSegment(version, segNum),
   refFrameA           (UNSET_STRING),
   refFrameB           (UNSET_STRING),
   attitudeDirection   (UNSET_STRING),
   attitudeType        (UNSET_STRING),
   inertialToBody      (true)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
CCSDSAEMSegment::CCSDSAEMSegment(const CCSDSAEMSegment &copy) :
   CCSDSEMSegment(copy),
   refFrameA           (copy.refFrameA),
   refFrameB           (copy.refFrameB),
   attitudeDirection   (copy.attitudeDirection),
   attitudeType        (copy.attitudeType),
   inertialToBody      (copy.inertialToBody)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
CCSDSAEMSegment& CCSDSAEMSegment::operator=(const CCSDSAEMSegment &copy)
{
   if (&copy == this)
      return *this;
   CCSDSEMSegment::operator=(copy);

   refFrameA           = copy.refFrameA;
   refFrameB           = copy.refFrameB;
   attitudeDirection   = copy.attitudeDirection;
   attitudeType        = copy.attitudeType;
   inertialToBody      = copy.inertialToBody;

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CCSDSAEMSegment::~CCSDSAEMSegment()
{
}

//------------------------------------------------------------------------------
// Validates the contents of the handled AEM meta data elements.
//------------------------------------------------------------------------------
bool CCSDSAEMSegment::Validate(bool checkData)
{
   #ifdef DEBUG_AEM_VALIDATE
      MessageInterface::ShowMessage("Entering CCSDSAEMSegment::Validate\n");
   #endif

   // Reference frames and attitude direction
   if ((refFrameA == UNSET_STRING) || (refFrameB == UNSET_STRING))
   {
      std::string errmsg = segError;
      errmsg += "Required field REF_FRAME_A or REF_FRAME_B is missing.\n";
      throw UtilityException(errmsg);
   }
   if (refFrameA == refFrameB)
   {
      std::string errmsg = segError;
      errmsg += "REF_FRAME_A and REF_FRAME_B cannot be the same.\n";
      throw UtilityException(errmsg);
   }

   if (attitudeDirection == UNSET_STRING)
   {
      std::string errmsg = segError;
      errmsg += "Required field ATTITUDE_DIR is missing.\n";
      throw UtilityException(errmsg);
   }
   if (attitudeDirection == "A2B")
   {
      if (refFrameA == "EME2000")  inertialToBody = true;
      else                         inertialToBody = false;
   }
   else   // B2A
   {
      if (refFrameB == "EME2000")  inertialToBody = true;
      else                         inertialToBody = false;
   }

   if (attitudeType == UNSET_STRING)
   {
      std::string errmsg = segError;
      errmsg += "Required field ATTITUDE_TYPE is missing.\n";
      throw UtilityException(errmsg);
   }

   #ifdef DEBUG_AEM_VALIDATE
      MessageInterface::ShowMessage("EXITING CCSDSAEMSegment::Validate\n");
   #endif
   return CCSDSEMSegment::Validate(checkData);
}

//------------------------------------------------------------------------------
// Sets the corresponding meta data for the input field name.
//------------------------------------------------------------------------------
bool CCSDSAEMSegment::SetMetaData(const std::string &fieldName,
                                  const std::string &value)
{
   #ifdef DEBUG_AEM_SET_META
      MessageInterface::ShowMessage(
            "Entering AEMSegment::SetMetaData with field %s and value %s\n",
            fieldName.c_str(), value.c_str());
   #endif
   if (fieldName == "REF_FRAME_A")
   {
      refFrameA = GmatStringUtil::ToUpper(value);
      if ((refFrameA != "EME2000") && (refFrameA != "SC_BODY_1"))
      {
         std::string errmsg = segError;
         errmsg += "Invalid value for field REF_FRAME_A.  Reference frames must be ";
         errmsg += "either EME2000 or SC_BODY_1.\n";
         throw UtilityException(errmsg);
      }
      return true;
   }
   else if (fieldName == "REF_FRAME_B")
   {
      refFrameB = GmatStringUtil::ToUpper(value);
      if ((refFrameB != "EME2000") && (refFrameB != "SC_BODY_1"))
      {
         std::string errmsg = segError;
         errmsg += "Invalid value for field REF_FRAME_B.  Reference frames must be ";
         errmsg += "either EME2000 or SC_BODY_1.\n";
         throw UtilityException(errmsg);
      }
      return true;
   }
   else if (fieldName == "ATTITUDE_DIR")
   {
      attitudeDirection = GmatStringUtil::ToUpper(value);
      if ((attitudeDirection != "A2B") && (attitudeDirection != "B2A"))
      {
         std::string errmsg = segError;
         errmsg += "Invalid value for field ATTITUDE_DIR.  Allowed values are: ";
         errmsg += "either A2B or B2A.\n";
         throw UtilityException(errmsg);
      }
      return true;
   }
   else if (fieldName == "ATTITUDE_TYPE")
   {
      attitudeType = GmatStringUtil::ToUpper(value);
        // this should be caught by the reader ... ideally this segment should
        // not know about its child classes.
//      if ((attitudeType != "QUATERNION") && (attitudeType != "EULER_ANGLE"))
//      {
//         std::string errmsg = segError;
//         errmsg += "Invalid value for field ATTITUDE_TYPE.  ";
//         errmsg+= "GMAT currently supports : QUATERNION and EULER_ANGLE types.\n";
//         throw UtilityException(errmsg);
//      }
      return true;
   }
   // We use Linear (SLERP) or Lagrange interpolation
   else if (fieldName == "INTERPOLATION_METHOD")
   {
      interpolationMethod = GmatStringUtil::ToUpper(value);
      return true;
   }
   // if it isn't handled here, call the parent class
   return CCSDSEMSegment::SetMetaData(fieldName, value);
}

//------------------------------------------------------------------------------
// Returns flag indicating whether or not the rotation specified in the AEM
// represents an inertial-to-body transformation.
//------------------------------------------------------------------------------
bool CCSDSAEMSegment::IsRotationInertialToBody()
{
   return inertialToBody;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
