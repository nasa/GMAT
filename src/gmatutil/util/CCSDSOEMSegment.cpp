//------------------------------------------------------------------------------
//                                  CCSDSOEMSegment
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
// Author: Linda Jun / NASA
// Created: 2016.01.07
//
/**
 * Stores, validates, and manages meta data and cartesian orbit vector read from,
 * or to be written to, a CCSDS Orbit Ephemeris Message file.
 */
//------------------------------------------------------------------------------
#include "CCSDSOEMSegment.hpp"
#include "MessageInterface.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include "TimeSystemConverter.hpp"
#include <sstream>

//#define DEBUG_OEM_VALIDATE
//#define DEBUG_OEM_SET
//#define DEBUG_OEM_DATA
//#define DEBUG_OEM_GET_STATE
//#define DEBUG_ADD_DATA_FOR_WRITING

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
CCSDSOEMSegment::CCSDSOEMSegment(const std::string version, Integer segNum) : 
   CCSDSEMSegment(version, segNum)
{
   if (formatVersion == "1.0")
      dataSize = 6;
   else if (formatVersion == "2.0")
      dataSize = 9;
   else
      throw UtilityException("Error: CCSDS OEM version "
         + version + 
         " is not available in the current GMAT build.\n");
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
CCSDSOEMSegment::CCSDSOEMSegment(const CCSDSOEMSegment &copy) :
   CCSDSEMSegment(copy)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
CCSDSOEMSegment& CCSDSOEMSegment::operator=(const CCSDSOEMSegment &copy)
{
   if (&copy == this)
      return *this;
   
   CCSDSEMSegment::operator=(copy);

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CCSDSOEMSegment::~CCSDSOEMSegment()
{
}

//------------------------------------------------------------------------------
// Returns a clone of this object.
//------------------------------------------------------------------------------
CCSDSEMSegment* CCSDSOEMSegment::Clone() const
{
   return (new CCSDSOEMSegment(*this));
}

//------------------------------------------------------------------------------
// Validates the contents of the handled Cartesian meta data elements.
//------------------------------------------------------------------------------
bool CCSDSOEMSegment::Validate(bool checkData)
{
   #ifdef DEBUG_OEM_VALIDATE
   MessageInterface::ShowMessage
      ("CCSDSOEMSegment::Validate() entered, checkData=%d\n", checkData);
   #endif
   
   // Cartesian type
   if (GmatStringUtil::ToUpper(interpolationMethod) != "LAGRANGE")
   {
      std::string errmsg = segError;
      errmsg += "Interpolation type \"";
      errmsg += interpolationMethod + "\" is not valid for Orbit type. ";
      errmsg += "The only supported value is \"LAGRANGE\".\n";
      throw UtilityException(errmsg);
   }
   
   #ifdef DEBUG_OEM_VALIDATE
   MessageInterface::ShowMessage
      ("CCSDSOEMSegment::Validate() returning CCSDSEMSegment::Validate()\n");
   #endif
   return CCSDSEMSegment::Validate(checkData);
}


//------------------------------------------------------------------------------
// Returns the cartesin orbit vecot at the specified input time.
//------------------------------------------------------------------------------
Rvector6 CCSDSOEMSegment::GetState(Real atEpoch)
{
   #ifdef DEBUG_OEM_GET_STATE
   MessageInterface::ShowMessage
      ("CCSDSOEMSegment::GetState() entered, epoch = %lf, dataStore size = %d\n",
       atEpoch, (Integer) dataStore.size());
   #endif
   
   Rvector rvec = DetermineState(atEpoch);
   Rvector6 cart(rvec[0], rvec[1], rvec[2], rvec[3], rvec[4], rvec[5]);
   
   #ifdef DEBUG_OEM_GET_STATE
   MessageInterface::ShowMessage
      ("CCSDSOEMSegment::GetState() returning\n%s\n", cart.ToString().c_str());
   #endif
   
   return cart;
}

//------------------------------------------------------------------------------
// std::string GetMetaDataForWriting()
//------------------------------------------------------------------------------
std::string CCSDSOEMSegment::GetMetaDataForWriting()
{
   std::stringstream ss("");
   ss << std::endl << "META_START" << std::endl;
   for (unsigned int i = 0; i < metaComments.size(); i++)
      ss << "COMMENT  " << metaComments[i] << std::endl;
   ss << "OBJECT_NAME          = " << objectName << std::endl;
   ss << "OBJECT_ID            = " << objectID << std::endl;
   ss << "CENTER_NAME          = " << centerName << std::endl;
   ss << "REF_FRAME            = " << refFrame << std::endl;
   ss << "TIME_SYSTEM          = " << timeSystem << std::endl;
   ss << "START_TIME           = " << startTimeStr << std::endl;
   ss << "USEABLE_START_TIME   = " << usableStartTimeStr << std::endl;
   ss << "USEABLE_STOP_TIME    = " << usableStopTimeStr << std::endl;
   ss << "STOP_TIME            = " << stopTimeStr << std::endl;
   ss << "INTERPOLATION        = " << interpolationMethod << std::endl;
   ss << "INTERPOLATION_DEGREE = " << interpolationDegreeStr << std::endl;
   ss << "META_STOP" << std::endl << std::endl;
   return ss.str();
}

//------------------------------------------------------------------------------
// Sets the corresponding meta data for the input field name.
//------------------------------------------------------------------------------
bool CCSDSOEMSegment::SetMetaData(const std::string &fieldName,
                                  const std::string &value)
{
   return CCSDSEMSegment::SetMetaData(fieldName, value);
}

//------------------------------------------------------------------------------
// bool SetMetaDataForWriting(const std::string &fieldName,
//                            const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets OEM meta data for writing.
 */
//------------------------------------------------------------------------------
bool CCSDSOEMSegment::SetMetaDataForWriting(const std::string &fieldName,
                                            const std::string &value)
{
   if (fieldName == "OBJECT_NAME" || fieldName == "OBJECT_ID" ||
       fieldName == "CENTER_NAME" || fieldName == "REF_FRAME" ||
       fieldName == "TIME_SYSTEM" || fieldName == "START_TIME" ||
       fieldName == "USEABLE_START_TIME" || fieldName == "USEABLE_STOP_TIME" ||
       fieldName == "STOP_TIME" || fieldName == "INTERPOLATION_DEGREE")
      CCSDSEMSegment::SetMetaDataForWriting(fieldName, value);
   else if (fieldName == "INTERPOLATION")
      interpolationMethod = GmatStringUtil::ToUpper(value);
   else
      throw UtilityException
         ("The field \"" + fieldName + "\" is not valid OEM metadata field.\n"
          "Valid fields are: OBJECT_NAME, OBJECT_ID, CENTER_NAME, REF_FRAME, "
          "TIME_SYSTEM, START_TIME, USEABLE_START_TIME, USEABLE_STOP_TIME, "
          "STOP_TIME, INTERPOLATION, INTERPOLATION_DEGREE");
   
   return true;
}


//------------------------------------------------------------------------------
// Adds an epoch/data pair to the dataStore.
// At this level, it just checks for errors in data size or epoch order.
//------------------------------------------------------------------------------
bool CCSDSOEMSegment::AddData(Real epoch, Rvector data, bool justCheckDataSize)
{
#ifdef DEBUG_ADD_DATA
   MessageInterface::ShowMessage
   ("CCSDSOEMSegment::AddData() entered, epoch=%f, justCheckDataSize=%d, "
      "dataStore.size()=%d\n", epoch, justCheckDataSize, dataStore.size());
#endif
   Rvector useData(6, data[0], data[1], data[2], data[3], data[4], data[5]);
   EpochAndData *newData = new EpochAndData();
   newData->epoch = epoch;
   newData->data = useData;
   
   bool recordIsValid = true;
   
   if (!dataStore.empty())
   {
      Real lastEpoch = dataStore.at(dataStore.size() - 1)->epoch;
      if (epoch == lastEpoch)
      {
         if (newData->data == dataStore.at(dataStore.size() - 1)->data)
         {
            recordIsValid = false;
            std::string warnmsg = "WARNING: A repeat epoch was found in an Ephemeris file at epoch: " + std::to_string(epoch) +
               ". The state data is identical so the second record will be ignored.\n";
            //Message disabled as it prints too many times. Could be added back if desired. This was never enabled. GMT-7376
            //MessageInterface::ShowMessage(warnmsg);
         }
         else
         {
            recordIsValid = false;
            TimeSystemConverter *theTimeConverter;
            theTimeConverter = TimeSystemConverter::Instance();
            std::string errmsg = "";

            std::string time = "";
            Real temp;
            theTimeConverter->Convert("A1ModJulian", epoch, "", "UTCGregorian", temp, time, 1);

            errmsg += "Duplicate epochs with different position/velocity data were found within a segment at ";
            errmsg += time;
            errmsg += ". Ephemeris data can only be discontinuous at segment boundaries.";
            throw UtilityException(errmsg);
         }
      }
   }
   
   if(recordIsValid)
      dataStore.push_back(newData);

#ifdef DEBUG_ADD_DATA
   MessageInterface::ShowMessage("CCSDSOEMSegment::AddData() returning true\n");
#endif
   return true;
}


//------------------------------------------------------------------------------
// Adds an epoch/data pair to the dataStore for writing.
//------------------------------------------------------------------------------
bool CCSDSOEMSegment::AddDataForWriting(Real epoch, Rvector &data)
{
   // Note that: useData may contain not only position and velocity but it may
   // contains some other parameters else.
   //Rvector useData(6, data[0], data[1], data[2], data[3], data[4], data[5]);
   EpochAndData *newData = new EpochAndData();
   newData->epoch = epoch;
   //newData->data  = useData;
   newData->data = data;
   dataStore.push_back(newData);
   
   #ifdef DEBUG_ADD_DATA_FOR_WRITING
   for (Integer i = 0; i < newData->data.GetSize(); ++i)
      MessageInterface::ShowMessage("data [%d] = %.15le\n", i, newData->data[i]);
   #endif

   return true;
}

//------------------------------------------------------------------------------
// void ClearMetaData()
//------------------------------------------------------------------------------
/**
 * Clears meta comments and data
 */
//------------------------------------------------------------------------------
void CCSDSOEMSegment::ClearMetaData()
{
   CCSDSEMSegment::ClearMetaData();
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Interpolates the data.
//------------------------------------------------------------------------------
Rvector CCSDSOEMSegment::Interpolate(Real atEpoch)
{
   return InterpolateLagrange(atEpoch);
}

