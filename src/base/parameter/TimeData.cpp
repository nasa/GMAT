//$Id$
//------------------------------------------------------------------------------
//                                  TimeData
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/12
//
/**
 * Implements Time related data class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "TimeData.hpp"
#include "ParameterException.hpp"
#include "TimeTypes.hpp"
#include "Linear.hpp"               // for GmatRealUtil::ToString()
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_TIMEDATA
//#define DEBUG_TIMEDATA_OBJNAME
//#define DEBUG_TIMEDATA_GET
//#define DEBUG_CLEAR

//---------------------------------
// static data
//---------------------------------

//const Real TimeData::MJD_OFFSET = GmatTimeConstants::JD_JAN_5_1941;
const Real TimeData::TIME_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;
const std::string TimeData::TIME_STRING_UNDEFINED = "INVALID_TIME";

const std::string
TimeData::VALID_OBJECT_TYPE_LIST[TimeDataObjectCount] =
{
   "Spacecraft", //loj: use spacecraft to get current time?
   "SpacePoint"  // can use times for non-Spacecraft SpacePoints
}; 


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// TimeData(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
TimeData::TimeData(const std::string &name, const std::string &typeName,
                   UnsignedInt paramOwnerType)
   : RefData(name, typeName, paramOwnerType)
{
   #ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("TimeData::TimeData(default) <%p>'%s' entered\n", this, mActualParamName.c_str());
   #endif
   mInitialEpoch = 0.0;
   mIsInitialEpochSet = false;
   mSpacecraft = NULL;
   mSpacePoint = NULL;
   
   handleLeapSecond = false;
   theTimeConverter = TimeSystemConverter::Instance();

   #ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("TimeData::TimeData(default) <%p>'%s' leaving, mInitialEpoch=%f, "
       "mIsInitialEpochSet=%d\n", this, mActualParamName.c_str(),
       mInitialEpoch, mIsInitialEpochSet);
   #endif
}


//------------------------------------------------------------------------------
// TimeData(const TimeData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
TimeData::TimeData(const TimeData &copy)
   : RefData(copy)
{
   #ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("TimeData::TimeData(copy) <%p>'%s' entered, copy.mInitialEpoch=%f, "
       "copoy.mIsInitialEpochSet=%d\n", this, mActualParamName.c_str(),
       copy.mInitialEpoch, copy.mIsInitialEpochSet);
   #endif
   mInitialEpoch      = copy.mInitialEpoch;
   mIsInitialEpochSet = copy.mIsInitialEpochSet;
   mSpacecraft        = copy.mSpacecraft;
   mSpacePoint        = copy.mSpacePoint;
   
   handleLeapSecond   = copy.handleLeapSecond;
   theTimeConverter   = TimeSystemConverter::Instance();
   
   #ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("TimeData::TimeData(copy) <%p>'%s' leaving, mInitialEpoch=%f, "
       "mIsInitialEpochSet=%d\n", this, mActualParamName.c_str(),
       mInitialEpoch, mIsInitialEpochSet);
   #endif
}


//------------------------------------------------------------------------------
// TimeData& operator= (const TimeData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
TimeData& TimeData::operator= (const TimeData &right)
{
   if (this != &right)
   {
      RefData::operator=(right);
      
      mInitialEpoch      = right.mInitialEpoch;
      mIsInitialEpochSet = right.mIsInitialEpochSet;
      mSpacecraft        = right.mSpacecraft;
      mSpacePoint        = right.mSpacePoint;
      handleLeapSecond   = right.handleLeapSecond;
      
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~TimeData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TimeData::~TimeData()
{
}


//------------------------------------------------------------------------------
// bool IsInitialEpochSet()
//------------------------------------------------------------------------------
/**
 * @return true if initial epoch is set
 */
//------------------------------------------------------------------------------
bool TimeData::IsInitialEpochSet()
{
   return mIsInitialEpochSet;
}

//------------------------------------------------------------------------------
// void ClearIsInitialEpochSet()
//------------------------------------------------------------------------------
/**
 * Clears the flag indicating if initial epoch is set
 */
//------------------------------------------------------------------------------
void TimeData::ClearIsInitialEpochSet()
{
   #ifdef DEBUG_CLEAR
   MessageInterface::ShowMessage
      ("TimeData::ClearIsInitialEpochSet() <%p>'%s' Setting mIsInitialEpochSet to false\n",
       this, mActualParamName.c_str());
   #endif
   
   mIsInitialEpochSet = false;
}

//------------------------------------------------------------------------------
// Real GetInitialEpoch() const
//------------------------------------------------------------------------------
/**
 * Retrives initial epoch.
 */
//------------------------------------------------------------------------------
Real TimeData::GetInitialEpoch() const
{
   #ifdef DEBUG_TIMEDATA
   MessageInterface::ShowMessage
      ("TimeData::GetInitialEpoch() returning %f\n", mInitialEpoch);
   #endif
   
   return mInitialEpoch;
}


//------------------------------------------------------------------------------
// void SetInitialEpoch(const Real &initialEpoch)
//------------------------------------------------------------------------------
/**
 * Sets initial epoch.
 */
//------------------------------------------------------------------------------
void TimeData::SetInitialEpoch(const Real &initialEpoch)
{
   mInitialEpoch      = initialEpoch;
   mIsInitialEpochSet = true;
   
   #ifdef DEBUG_TIMEDATA
   MessageInterface::ShowMessage
      ("TimeData::SetInitialEpoch() mInitialEpoch is set to %f\n", mInitialEpoch);
   #endif
}


//------------------------------------------------------------------------------
// Real GetTimeReal(Integer id)
//------------------------------------------------------------------------------
/**
 * Retrives current time.
 */
//------------------------------------------------------------------------------
Real TimeData::GetTimeReal(Integer id)
{
   // Get current time from Spacecraft
//   if (mSpacecraft == NULL)
   if (mSpacePoint == NULL)
      InitializeRefObjects();
   
//   Real a1Mjd = mSpacecraft->GetEpoch();
   Real a1Mjd = mSpacePoint->GetEpoch();
   
   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("TimeData::GetTimeReal() <%p>'%s' entered, id=%d, mSpacecraft=<%p>'%s', "
       "a1Mjd=%f, mSpacecraft->IsGlobal=%d, mSpacecraft->IsLocal=%d\n", this,
       mActualParamName.c_str(), id, mSpacecraft, mSpacecraft->GetName().c_str(),
       a1Mjd, mSpacecraft->IsGlobal(),
       mSpacecraft->IsLocal());
   #endif
   
   Real time = -999.999;
   
   handleLeapSecond = false;
   
   switch (id)
   {
   case A1:
      time = a1Mjd;
      break;
   case TAI:
      time = theTimeConverter->Convert(a1Mjd, TimeSystemConverter::A1MJD,
                                        TimeSystemConverter::TAIMJD,
                                        GmatTimeConstants::JD_JAN_5_1941);
      break;
   case TT:
      time = theTimeConverter->Convert(a1Mjd, TimeSystemConverter::A1MJD,
                                        TimeSystemConverter::TTMJD,
                                        GmatTimeConstants::JD_JAN_5_1941);
      break;
   case TDB:
      time = theTimeConverter->Convert(a1Mjd, TimeSystemConverter::A1MJD,
                                        TimeSystemConverter::TDBMJD,
                                        GmatTimeConstants::JD_JAN_5_1941);
      break;
   case UTC:
      time = theTimeConverter->Convert(a1Mjd, TimeSystemConverter::A1MJD,
                                        TimeSystemConverter::UTCMJD,
                                        GmatTimeConstants::JD_JAN_5_1941,
                                        &handleLeapSecond);
      break;
   default:
      throw ParameterException("TimeData::GetTimeReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
   
   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("TimeData::GetTimeReal() <%p>'%s' id=%d, a1Mjd=%.10f, returning time=%.10f\n",
       this, mActualParamName.c_str(), id, a1Mjd, time);
   #endif
   
   return time;
}


//------------------------------------------------------------------------------
// void SetTimeReal(Integer id, Real value)
//------------------------------------------------------------------------------
/**
 * Sets time of real value to Spacecraft  << SpacePoint
 */
//------------------------------------------------------------------------------
void TimeData::SetTimeReal(Integer id, Real value)
{
//   // Set input time to Spacecraft
//   if (mSpacecraft == NULL)
   // Set input time to Spacecraft
   if (mSpacePoint == NULL)
      InitializeRefObjects();
   
   #ifdef DEBUG_TIMEDATA_SET
   MessageInterface::ShowMessage
      ("TimeData::SetTimeReal() entered, mSpacecraft=<%p>'%s', value=%f\n",
       mSpacecraft, mSpacecraft->GetName().c_str(), value);
   #endif
   
   Real a1Mjd = -9999.999;
//   Integer epochId = mSpacecraft->GetParameterID("A1Epoch");
   Integer epochId = mSpacePoint->GetParameterID("A1Epoch");
   
   switch (id)
   {
   case A1:
//      mSpacecraft->SetRealParameter(epochId, value);
      mSpacePoint->SetRealParameter(epochId, value);
      break;
   case TAI:
      a1Mjd = theTimeConverter->Convert(value, TimeSystemConverter::TAIMJD,
                                         TimeSystemConverter::A1MJD,
                                         GmatTimeConstants::JD_JAN_5_1941);
      mSpacePoint->SetRealParameter(epochId, a1Mjd);
      break;
   case TT:
      a1Mjd = theTimeConverter->Convert(value, TimeSystemConverter::TTMJD,
                                         TimeSystemConverter::A1MJD,
                                         GmatTimeConstants::JD_JAN_5_1941);
      mSpacePoint->SetRealParameter(epochId, a1Mjd);
      break;
   case TDB:
      a1Mjd = theTimeConverter->Convert(value, TimeSystemConverter::TDBMJD,
                                         TimeSystemConverter::A1MJD,
                                         GmatTimeConstants::JD_JAN_5_1941);
      mSpacePoint->SetRealParameter(epochId, a1Mjd);
      break;
   case UTC:
      a1Mjd = theTimeConverter->Convert(value, TimeSystemConverter::UTCMJD,
                                         TimeSystemConverter::A1MJD,
                                         GmatTimeConstants::JD_JAN_5_1941);
      mSpacePoint->SetRealParameter(epochId, a1Mjd);
     break;
   default:
      throw ParameterException("TimeData::GetTimeReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
   
   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("TimeData::SetTimeReal() leaving, id=%d, a1Mjd=%f.10\n", id, a1Mjd);
   #endif
}


//------------------------------------------------------------------------------
// std::string GetTimeString(Integer id)
//------------------------------------------------------------------------------
/**
 * Retrives current time string.
 */
//------------------------------------------------------------------------------
std::string TimeData::GetTimeString(Integer id)
{
//   // Get current time from Spacecraft
//   if (mSpacecraft == NULL)
   // Get current time from Spacecraft
   if (mSpacePoint == NULL)
      InitializeRefObjects();
   
   Real time  = GetTimeReal(id);
   bool isUTC = (id == UTC);
   
   switch (id)
   {
   case A1:
   case TAI:
   case TT:
   case TDB:
   case UTC:
      #ifdef DEBUG_TIMEDATA
      MessageInterface::ShowMessage
         ("TimeData::GetTimeString() id=%d, timeStr = %s\n", id,
          TimeSystemConverter::ConvertMjdToGregorian(time, isUTC).c_str());
      #endif
      return theTimeConverter->ConvertMjdToGregorian(time, (isUTC && handleLeapSecond));// need toUTC
   default:
      throw ParameterException("TimeData::GetTimeString() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
}


//------------------------------------------------------------------------------
// void SetTimeString(Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets time of string value in gregorian to Spacecraft
 */
//------------------------------------------------------------------------------
void TimeData::SetTimeString(Integer id, const std::string &value)
{
//   // Set input time to Spacecraft
//   if (mSpacecraft == NULL)
   // Set input time to Spacecraft
   if (mSpacePoint == NULL)
      InitializeRefObjects();
   
   #ifdef DEBUG_TIMEDATA_SET
   MessageInterface::ShowMessage
      ("TimeData::SetTimeString() entered, mSpacecraft=<%p>'%s', value=%s\n",
       mSpacecraft, mSpacecraft->GetName().c_str(), value.c_str());
   #endif
   
//   Integer epochId = mSpacecraft->GetParameterID("A1Epoch");
   Integer epochId = mSpacePoint->GetParameterID("A1Epoch");
   Real fromMjd = -999.999;
   Real a1Mjd = -999.999;
   std::string a1MjdString;
   switch (id)
   {
   case A1:
      theTimeConverter->Convert("A1Gregorian", fromMjd, value,
                                 "A1ModJulian", a1Mjd, a1MjdString);
      mSpacePoint->SetRealParameter(epochId, a1Mjd);
      break;
   case TAI:
      theTimeConverter->Convert("TAIGregorian", fromMjd, value,
                                 "A1ModJulian", a1Mjd, a1MjdString);
      mSpacePoint->SetRealParameter(epochId, a1Mjd);
      break;
   case TT:
      theTimeConverter->Convert("TTGregorian", fromMjd, value,
                                 "A1ModJulian", a1Mjd, a1MjdString);
      mSpacePoint->SetRealParameter(epochId, a1Mjd);
      break;
   case TDB:
      theTimeConverter->Convert("TDBGregorian", fromMjd, value,
                                 "A1ModJulian", a1Mjd, a1MjdString);
      mSpacePoint->SetRealParameter(epochId, a1Mjd);
      break;
   case UTC:
      theTimeConverter->Convert("UTCGregorian", fromMjd, value,
                                 "A1ModJulian", a1Mjd, a1MjdString);
      mSpacePoint->SetRealParameter(epochId, a1Mjd);
     break;
   default:
      throw ParameterException("TimeData::SetTimeString() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
   
   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("TimeData::SetTimeString() leaving, id=%d, a1Mjd=%f.10\n", id, a1Mjd);
   #endif
}


//------------------------------------------------------------------------------
// Real GetElapsedTimeReal(Integer id)
//------------------------------------------------------------------------------
/**
 * Retrives elapsed time from Spacecraft (loj: for now)
 */
//------------------------------------------------------------------------------
Real TimeData::GetElapsedTimeReal(Integer id)
{
   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("TimeData::GetElapsedTimeReal() <%p>'%s' entered, id=%d\n", this,
       mActualParamName.c_str(), id);
   #endif
   
   Real a1mjd = GetTimeReal(A1);
   Real retVal = GmatBase::REAL_PARAMETER_UNDEFINED;
   
   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("   a1mjd=%f, mInitialEpoch=%f, mIsInitialEpochSet=%d\n", a1mjd,
       mInitialEpoch, mIsInitialEpochSet);
   #endif
   
   if (!mIsInitialEpochSet)
   {
      mInitialEpoch = a1mjd;
      mIsInitialEpochSet = true;
      #ifdef DEBUG_TIMEDATA
      MessageInterface::ShowMessage
         ("   mInitialEpoch is set to %f and mIsInitialEpochSet to true \n",
          mInitialEpoch);
      #endif
   }
   
   switch (id)
   {
   //case YEARS:
   //case MONTHS:
   case DAYS:
      retVal = a1mjd - mInitialEpoch;
      break;
   //case HOURS:
   //case MINS:
   case SECS:
      retVal =  (a1mjd - mInitialEpoch)* GmatTimeConstants::SECS_PER_DAY;
      break;
   default:
      throw ParameterException("TimeData::GetElapsedTimeReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("TimeData::GetElapsedTimeReal() <%p>'%s' id=%d, returning %f\n", this,
       mActualParamName.c_str(), id, retVal);
   #endif
   return retVal;
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* TimeData::GetValidObjectList() const
{
   return VALID_OBJECT_TYPE_LIST;
}

// The inherited methods from RefData
//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
std::string TimeData::GetRefObjectName(const UnsignedInt type) const
{
   #ifdef DEBUG_TIMEDATA_OBJNAME
   MessageInterface::ShowMessage
      ("TimeData::GetRefObjectName() entered, type=%d\n", type);
   #endif
   
   try
   {
      return RefData::GetRefObjectName(type);
   }
   catch (ParameterException &pe)
   {
      // if the type was SpacePoint, we may need to look in the Spacecraft
      // list (or vice versa?), since we are looking for a Spacecraft
      // and a Spacecraft is a SpacePoint
      UnsignedInt altType = type;
      if (type == Gmat::SPACE_POINT)
         altType = Gmat::SPACECRAFT;
      else if (type == Gmat::SPACECRAFT)
         altType = Gmat::SPACE_POINT;
      
      #ifdef DEBUG_TIMEDATA_OBJNAME
         MessageInterface::ShowMessage(
               "TimeData::GetRefObjectName -> couldn't find type %d, so look for type %d\n",
               type, altType);
      #endif
      for (int i=0; i<mNumRefObjects; i++)
      {
         if (mRefObjList[i].objType == altType)
         {
            //Notes: will return first object name.
            #ifdef DEBUG_TIMEDATA_OBJNAME
            MessageInterface::ShowMessage
               ("TimeData::GetRefObjectName() altType=%d returning: %s\n", altType,
                mRefObjList[i].objName.c_str());
            #endif
            return mRefObjList[i].objName;
         }
      }

      throw;
   }
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& TimeData::GetRefObjectNameArray(const UnsignedInt type)
{
   RefData::GetRefObjectNameArray(type);

   if (mAllRefObjectNames.empty() && type == Gmat::SPACE_POINT)
   {
      // if the type was SpacePoint, we may need to look in the Spacecraft
      // list (or vice versa?), since we are looking for a Spacecraft
      // and a Spacecraft is a SpacePoint
      return RefData::GetRefObjectNameArray(Gmat::SPACECRAFT);
   }
   else
      return mAllRefObjectNames;
}

//------------------------------------------------------------------------------
// bool SetRefObjectName(UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Adds type and name to reference object list.
 *
 * @param <type> reference object type
 * @param <name> reference object name
 *
 * @return true if type and name has successfully added to the list
 *
 */
//------------------------------------------------------------------------------
bool TimeData::SetRefObjectName(UnsignedInt type, const std::string &name)
{
   // We need to be able to handle SpacePoints, not just Spacecraft
   UnsignedInt useType = type;
   if ((type == Gmat::GROUND_STATION)   || (type == Gmat::BODY_FIXED_POINT) ||
       (type == Gmat::CALCULATED_POINT) || (type == Gmat::LIBRATION_POINT)  ||
       (type == Gmat::BARYCENTER)       || (type == Gmat::CELESTIAL_BODY))
      useType = Gmat::SPACE_POINT;

   return RefData::SetRefObjectName(useType, name);

}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* TimeData::GetRefObject(const UnsignedInt type,
                                 const std::string &name)
{
   try
   {
      GmatBase* theObj = RefData::GetRefObject(type, name);
      return theObj;
   }
   catch (ParameterException &pe)
   {
      // if the type was SpacePoint, we may need to look in the Spacecraft
      // list (or vice versa?), since we are looking for a Spacecraft
      // and a Spacecraft is a SpacePoint
      UnsignedInt altType = type;
      if (type == Gmat::SPACE_POINT) altType = Gmat::SPACECRAFT;
      for (int i=0; i<mNumRefObjects; i++)
      {
         if (mRefObjList[i].objType == altType)
         {
            if (name == "") //if name is "", return first object
               return mRefObjList[i].obj;

            if ((name == "" || mRefObjList[i].objName == name) &&
                (mRefObjList[i].obj)->IsOfType("Spacecraft"))
            {
               //Notes: will return first object name.
               #ifdef DEBUG_TIMEDATA_OBJNAME
               MessageInterface::ShowMessage
                  ("TimeData::GetRefObject() altType=%d returning: %s\n", altType,
                   mRefObjList[i].objName.c_str());
               #endif
               return mRefObjList[i].obj;
            }
         }
      }

      throw;
   }
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets object which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool TimeData::SetRefObject(GmatBase *obj, const UnsignedInt type,
                            const std::string &name)
{
   // We need to be able to handle SpacePoints, not just Spacecraft
   UnsignedInt useType = type;
   if ((type == Gmat::GROUND_STATION)   || (type == Gmat::BODY_FIXED_POINT) ||
       (type == Gmat::CALCULATED_POINT) || (type == Gmat::LIBRATION_POINT)  ||
       (type == Gmat::BARYCENTER)       || (type == Gmat::CELESTIAL_BODY))
      useType = Gmat::SPACE_POINT;

   return RefData::SetRefObject(obj, useType, name);
}


//------------------------------------------------------------------------------
// bool ValidateRefObjects(GmatBase *param)
//------------------------------------------------------------------------------
/**
 * Validates reference objects for given parameter
 */
//------------------------------------------------------------------------------
bool TimeData::ValidateRefObjects(GmatBase *param)
{
   #ifdef DEBUG_TIMEDATA
   MessageInterface::ShowMessage
      ("TimeData::ValidateRefObjects() entered, param=<%p>'%s'\n", param,
       param ? param->GetName().c_str() : "NULL");
   #endif
   
   //loj: 3/23/04 removed checking for type
   bool status = false;
    
//   if (HasObjectType(VALID_OBJECT_TYPE_LIST[SPACECRAFT]))
   if ((HasObjectType(VALID_OBJECT_TYPE_LIST[SPACE_POINT])) ||
       (HasObjectType(VALID_OBJECT_TYPE_LIST[SPACECRAFT])))
   {
      if (mIsInitialEpochSet)
      {
         status = true;
      }
      else
      {
//         Spacecraft *sc = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
         SpacePoint *sp = (SpacePoint*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACE_POINT]);
         if (!sp)
         {
            sp = (SpacePoint*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
         }
         Real rval = sp->GetRealParameter("A1Epoch");

         if (rval != GmatBase::REAL_PARAMETER_UNDEFINED)
         {
            mInitialEpoch = sp->GetRealParameter("A1Epoch");
            mIsInitialEpochSet = true;
            status = true;
         }
      }
   }
   
   #ifdef DEBUG_TIMEDATA
   MessageInterface::ShowMessage
      ("TimeData::ValidateRefObjects() returning %d\n", status);
   #endif
   return status;
}


//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void TimeData::InitializeRefObjects()
{
   #ifdef DEBUG_TIMEDATA
   MessageInterface::ShowMessage
      ("TimeData::InitializeRefObjects() <%p>'%s' entered, mIsInitialEpochSet=%d\n",
       this, mActualParamName.c_str(), mIsInitialEpochSet);
   MessageInterface::ShowMessage
      ("   IsParameterGlobal=%d\n", GetParameter() ? GetParameter()->IsGlobal() : -123.123);
   #endif
   
//   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   mSpacePoint = (SpacePoint*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACE_POINT]);
   if (!mSpacePoint)
      mSpacePoint = (SpacePoint*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   
   #ifdef DEBUG_TIMEDATA
   MessageInterface::ShowMessage
      ("   mSpacePoint=<%p>'%s', mSpacePoint->IsGlobal=%d\n", mSpacePoint,
       mSpacePoint ? mSpacePoint->GetName().c_str() : "NULL",
       mSpacePoint ? mSpacePoint->IsGlobal() : -123.123);
   #endif
   
//   if (mSpacecraft == NULL)
   if (mSpacePoint == NULL)
   {
//      std::string scName = GetRefObjectName(Gmat::SPACECRAFT);
      std::string scName = GetRefObjectName(Gmat::SPACE_POINT);
      #ifdef DEBUG_TIMEDATA
      MessageInterface::ShowMessage
//         ("TimeData::InitializeRefObjects() Cannot find Spacecraft object named "
         ("TimeData::InitializeRefObjects() Cannot find SpacePoint object named "
          "\"%s\"\n", scName.c_str());
      #endif
      
      throw ParameterException
//         ("TimeData::InitializeRefObjects() Cannot find Spacecraft object named \"" +
         ("TimeData::InitializeRefObjects() Cannot find SpacePoint object named \"" +
          scName + "\"\n");
   }
   else
   {
      if (!mIsInitialEpochSet)
      {
//         mInitialEpoch = mSpacecraft->GetEpoch();
         mInitialEpoch = mSpacePoint->GetEpoch();
         mIsInitialEpochSet = true;
         
         #ifdef DEBUG_TIMEDATA
         MessageInterface::ShowMessage
            ("TimeData::InitializeRefObjects() set mInitialEpoch to %f\n", mInitialEpoch);
         #endif
      }
   }
   if (mSpacePoint->IsOfType("Spacecraft"))
   {
      mSpacecraft = (Spacecraft*)mSpacePoint;
   }
   
   #ifdef DEBUG_TIMEDATA
   MessageInterface::ShowMessage
      ("TimeData::InitializeRefObjects() leaving, mSpacecraft=<%p>\n", mSpacecraft);
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
bool TimeData::IsValidObjectType(UnsignedInt type)
{
   for (int i=0; i<TimeDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}

bool TimeData::AddRefObject(const UnsignedInt type,
                            const std::string &name, GmatBase *obj,
                            bool replaceName)
{
   #ifdef DEBUG_TIME_STRING_ADD
      MessageInterface::ShowMessage("In TimeData::AddRefObject - obj is %s\n",
            (obj? " NOT NULL" : "NULL!"));
      MessageInterface::ShowMessage("... replaceName = %s\n",
            (replaceName? "true" : "false"));
      if (obj)
      {
         MessageInterface::ShowMessage("... obj is %s\n", obj->GetName().c_str());
         MessageInterface::ShowMessage("... type is %d\n", obj->GetType());
      }
   #endif
   // We need to be able to handle SpacePoints, not just Spacecraft
   UnsignedInt useType = type;
   if ((type == Gmat::GROUND_STATION)    || (type == Gmat::BODY_FIXED_POINT)  ||
       (type == Gmat::CALCULATED_POINT)  || (type == Gmat::LIBRATION_POINT) ||
       (type == Gmat::BARYCENTER))
      useType = Gmat::SPACE_POINT;

   return RefData::AddRefObject(useType, name, obj, replaceName);
}


