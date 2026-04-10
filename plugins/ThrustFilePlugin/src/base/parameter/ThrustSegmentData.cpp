//------------------------------------------------------------------------------
//                                  ThrustSegmentData
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Peter Candell
// Created: 2022.09.06
//
/**
 * Implements ThrustSegment related data class.
 */
 //------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ThrustSegmentData.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"          // ToString()
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"



const std::string
ThrustSegmentData::VALID_OBJECT_TYPE_LIST[ThrustSegmentDataObjectCount] =
{
   "ThrustSegment"
};

const Real ThrustSegmentData::THRUST_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;

//------------------------------------------------------------------------------
// ThrustSegmentData()
//------------------------------------------------------------------------------
ThrustSegmentData::ThrustSegmentData(const std::string &name)
   : RefData(name)
{
   mThrustSegment = NULL;
}


//------------------------------------------------------------------------------
// ThrustSegmentData(const ThrustSegmentData &copy)
//------------------------------------------------------------------------------
ThrustSegmentData::ThrustSegmentData(const ThrustSegmentData &copy)
   : RefData(copy)
{
   mThrustSegment = copy.mThrustSegment;
}


//------------------------------------------------------------------------------
// ThrustSegmentData& operator= (const ThrustSegmentData& right)
//------------------------------------------------------------------------------
ThrustSegmentData& ThrustSegmentData::operator= (const ThrustSegmentData& right)
{
   if (this == &right)
      return *this;

   RefData::operator=(right);

   mThrustSegment = right.mThrustSegment;

   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustSegmentData()
//------------------------------------------------------------------------------
ThrustSegmentData::~ThrustSegmentData()
{
}


//------------------------------------------------------------------------------
// Real GetReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves ThrustSegment or ThrustSegment owned hardware element by integer id.
 */
 //------------------------------------------------------------------------------
Real ThrustSegmentData::GetReal(Integer item)
{
   if (mThrustSegment == NULL)
      InitializeRefObjects();

   // if there was an error initializing the ref objects, do not try to get the value
   if (mThrustSegment == NULL)
      return THRUST_REAL_UNDEFINED;

   switch (item)
   {
      case THRUSTSEGMENTSCALEFACTOR:
         return mThrustSegment->GetRealParameter("ThrustScaleFactor");
      case TSF_SIGMA:
         return mThrustSegment->GetRealParameter("ThrustScaleFactorSigma");
      case MASSFLOWSCALEFACTOR:
         return mThrustSegment->GetRealParameter("MassFlowScaleFactor");
      case TSF_EPSILON:
         return mThrustSegment->GetRealParameter("TSF_Epsilon");
      case START_EPOCH:
         return mThrustSegment->GetRealParameter("StartEpoch");
      case END_EPOCH:
         return mThrustSegment->GetRealParameter("EndEpoch");

      default:
         // otherwise, there is an error   
         throw ParameterException
         ("ThrustSegmentData::GetReal() Not readable or unknown item id: " +
            GmatStringUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real SetReal(Integer item, Real val)
//------------------------------------------------------------------------------
/**
 * Sets ThrustSegment or ThrustSegment owned hardware element by integer id.
 */
 //------------------------------------------------------------------------------
Real ThrustSegmentData::SetReal(Integer item, Real val)
{
#ifdef DEBUG_SETREAL
   MessageInterface::ShowMessage("Setting %s\n", mActualParamName.c_str());
#endif
   if (mThrustSegment == NULL)
      InitializeRefObjects();

   // if there was an error initializing the ref objects, do not try to set the value
   if (mThrustSegment == NULL)
      return THRUST_REAL_UNDEFINED;

   switch (item)
   {
      case THRUSTSEGMENTSCALEFACTOR:
         return mThrustSegment->SetRealParameter("ThrustScaleFactor", val);
      case TSF_SIGMA:
         return mThrustSegment->SetRealParameter("ThrustScaleFactorSigma", val);
      case MASSFLOWSCALEFACTOR:
         return mThrustSegment->SetRealParameter("MassFlowScaleFactor", val);
      case TSF_EPSILON:
         return mThrustSegment->SetRealParameter("TSF_Epsilon", val);
      case START_EPOCH:
         return mThrustSegment->SetRealParameter("StartEpoch", val);
      case END_EPOCH:
         return mThrustSegment->SetRealParameter("EndEpoch", val);

   default:
      // otherwise, there is an error   
      throw ParameterException
      ("ThrustSegmentData::SetReal() Not settable or unknown item id: " +
         GmatStringUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// std::string & ThrustSegmentData::GetString(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves ThrustSegment or ThrustSegment owned hardware element by integer id.
 */
 //------------------------------------------------------------------------------
std::string ThrustSegmentData::GetString(Integer item)
{
   if (mThrustSegment == NULL)
      InitializeRefObjects();

   // if there was an error initializing the ref objects, do not try to set the value
   if (mThrustSegment == NULL)
      return UNDEFINED_RETURN_STRING;

   switch (item)
   {
   default:
      // otherwise, there is an error   
      throw ParameterException
      ("ThrustSegmentData::GetString() Not gettable or unknown item id: " +
         GmatStringUtil::ToString(item));
   }
}

//------------------------------------------------------------------------------
// Real SetString(Integer item, std::string & val)
//------------------------------------------------------------------------------
/**
 * Sets ThrustSegment or ThrustSegment owned hardware element by integer id.
 */
 //------------------------------------------------------------------------------
Real ThrustSegmentData::SetString(Integer item, std::string & val)
{
   if (mThrustSegment == NULL)
      InitializeRefObjects();

   // if there was an error initializing the ref objects, do not try to set the value
   if (mThrustSegment == NULL)
      return THRUST_REAL_UNDEFINED;

   switch (item)
   {
   default:
      // otherwise, there is an error   
      throw ParameterException
      ("ThrustSegmentData::SetString() Not settable or unknown item id: " +
         GmatStringUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Rvector & ThrustSegmentData::GetRvector(Integer item)
//------------------------------------------------------------------------------
/**
 * Gets ThrustSegment or ThrustSegment owned hardware element by integer id.
 */
 //------------------------------------------------------------------------------
Rvector ThrustSegmentData::GetRvector(Integer item)
{
   if (mThrustSegment == NULL)
      InitializeRefObjects();

   // if there was an error initializing the ref objects, do not try to set the value
   if (mThrustSegment == NULL)
      return UNDEFINED_RETURN_RVECTOR;

   switch (item)
   {
      case THRUST_ANGLE_CONSTRAINT:
         return mThrustSegment->GetRvectorParameter("ThrustAngleConstraintVector");
      case THRUST_ANGLE_1:
         return mThrustSegment->GetRvectorParameter("ThrustAngle1");
      case THRUST_ANGLE_2:
         return mThrustSegment->GetRvectorParameter("ThrustAngle2");
      case THRUST_ANGLE_SIGMA_1:
         return mThrustSegment->GetRvectorParameter("ThrustAngle1Sigma");
      case THRUST_ANGLE_SIGMA_2:
         return mThrustSegment->GetRvectorParameter("ThrustAngle2Sigma");
      default:
         // otherwise, there is an error   
         throw ParameterException
         ("ThrustSegmentData::GetRvector() Not gettable or unknown item id: " +
            GmatStringUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Rvector & ThrustSegmentData::SetRvector(Integer item, Rvector & val)
//------------------------------------------------------------------------------
/**
 * Gets ThrustSegment or ThrustSegment owned hardware element by integer id.
 */
 //------------------------------------------------------------------------------
Rvector ThrustSegmentData::SetRvector(Integer item, Rvector & val)
{
   if (mThrustSegment == NULL)
      InitializeRefObjects();

   // if there was an error initializing the ref objects, do not try to set the value
   if (mThrustSegment == NULL)
      return UNDEFINED_RETURN_RVECTOR;

   switch (item)
   {
      case THRUST_ANGLE_CONSTRAINT:
         return mThrustSegment->SetRvectorParameter("ThrustAngleConstraintVector", val);
      case THRUST_ANGLE_1:
         return mThrustSegment->SetRvectorParameter("ThrustAngle1", val);
      case THRUST_ANGLE_2:
         return mThrustSegment->SetRvectorParameter("ThrustAngle2", val);
      case THRUST_ANGLE_SIGMA_1:
         return mThrustSegment->SetRvectorParameter("ThrustAngle1Sigma", val);
      case THRUST_ANGLE_SIGMA_2:
         return mThrustSegment->SetRvectorParameter("ThrustAngle2Sigma", val);
   default:
      // otherwise, there is an error   
      throw ParameterException
      ("ThrustSegmentData::SetRvector() Not settable or unknown item id: " +
         GmatStringUtil::ToString(item));
   }
}

//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* ThrustSegmentData::GetValidObjectList() const
{
   return VALID_OBJECT_TYPE_LIST;
}


//------------------------------------------------------------------------------
// bool ValidateRefObjects(GmatBase *param)
//------------------------------------------------------------------------------
/**
 * Validates reference objects for given parameter
 */
 //------------------------------------------------------------------------------
bool ThrustSegmentData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i = 0; i < ThrustSegmentDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == ThrustSegmentDataObjectCount)
      return true;
   else
      return false;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void ThrustSegmentData::InitializeRefObjects()
{

   mThrustSegment = (ThrustSegment*)FindFirstObject(VALID_OBJECT_TYPE_LIST[THRUSTSEGMENT]);

   if (mThrustSegment == NULL)
   {
      // Just write a message since Parameters in GmatFunction may not have ref. object
      // set until execution
#ifdef DEBUG_ThrustSegmentDATA_INIT
      MessageInterface::ShowMessage
      ("ThrustSegmentData::InitializeRefObjects() Cannot find ThrustSegment object.\n");
#endif

      //throw ParameterException
      //   ("ThrustSegmentData::InitializeRefObjects() Cannot find ThrustSegment object.\n");
   }

#ifdef DEBUG_ThrustSegmentDATA_INIT
   MessageInterface::ShowMessage
   ("ThrustSegmentData::InitializeRefObjects() '%s' leaving, mThrustSegment=<%p>'%s'\n",
      mActualParamName.c_str(), mThrustSegment, mThrustSegment->GetName().c_str());
#endif
}


//------------------------------------------------------------------------------
// virtual bool IsValidObjectType(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 *
 * @return return true if object is valid object, false otherwise
 */
 //------------------------------------------------------------------------------
bool ThrustSegmentData::IsValidObjectType(UnsignedInt type)
{
   
   //if (type == THRUSTSEGMENT)
      //return true;

   //THIS NEEDS TO BE CORRECTLY IMPLEMENTED AT SOME POINT
   for (int i = 0; i < ThrustSegmentDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }

   return false;

}
