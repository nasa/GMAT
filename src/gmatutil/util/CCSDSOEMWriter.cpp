//------------------------------------------------------------------------------
//                                  CCSDSOEMWriter
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
// Created: 2015.12.14
//
/**
 * Writes a CCSDS Ephemeris Message to a file.
 * This is the base class from which other classes must derive (e.g.
 * a class to handle attitude files)
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "FileUtil.hpp"
#include "FileTypes.hpp"
#include "StringUtil.hpp"

#include "CCSDSOEMWriter.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_META_COMMENT
//#define DEBUG_META_DATA
//#define DEBUG_DATA_COMMENT
//#define DEBUG_DATA_SEGMENT

// -----------------------------------------------------------------------------
// static data
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// default constructor
// -----------------------------------------------------------------------------
CCSDSOEMWriter::CCSDSOEMWriter() :
   CCSDSEMWriter()
{
   versionNumber = "1.0";
}

// -----------------------------------------------------------------------------
// copy constructor
// -----------------------------------------------------------------------------
CCSDSOEMWriter::CCSDSOEMWriter(const CCSDSOEMWriter &copy) :
   CCSDSEMWriter(copy)
{
}

// -----------------------------------------------------------------------------
// operator=
// -----------------------------------------------------------------------------
CCSDSOEMWriter& CCSDSOEMWriter::operator=(const CCSDSOEMWriter &copy)
{
   if (&copy == this)
      return *this;
   
   CCSDSEMWriter::operator=(copy);
   
   return *this;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
CCSDSOEMWriter::~CCSDSOEMWriter()
{
}

//------------------------------------------------------------------------------
// Returns a clone of this object.
//------------------------------------------------------------------------------
CCSDSOEMWriter* CCSDSOEMWriter::Clone() const
{
   return (new CCSDSOEMWriter(*this));
}


//------------------------------------------------------------------------------
// bool WriteMetaData()
//------------------------------------------------------------------------------
/**
 * Writes meta data to a file and then clears meta data.
 */
//------------------------------------------------------------------------------
bool CCSDSOEMWriter::WriteMetaData()
{
   #ifdef DEBUG_META_DATA
   MessageInterface::ShowMessage("CCSDSOEMWriter::WriteMetaData() entered\n");
   #endif
   
   if (!emOutStream.is_open())
      return false;
   
   emOutStream << currentOemSegment.GetMetaDataForWriting();
   emOutStream.flush();
   
   // Note that: only clear meta data when data segment is completed because some
   // information in meta data is still needed in writing data segment.
   // The follwoing line is moved to the end of WriteDataSegment() function.
   //ClearMetaData();
   
   #ifdef DEBUG_META_DATA
   MessageInterface::ShowMessage
      ("CCSDSOEMWriter::WriteMetaData() returning true\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool WriteDataComments()
//------------------------------------------------------------------------------
/**
 * Writes data comments to a file and then clears data comments
 */
//------------------------------------------------------------------------------
bool CCSDSOEMWriter::WriteDataComments()
{
   #ifdef DEBUG_DATA_COMMENTS
   MessageInterface::ShowMessage("CCSDSOEMWriter::WriteDataComments() entered\n");
   #endif
   
   if (!emOutStream.is_open())
      return false;
   
   emOutStream << currentOemSegment.GetDataComments();
   emOutStream.flush();
   
   ClearDataComments();
   
   #ifdef DEBUG_DATA_COMMENTS
   MessageInterface::ShowMessage("CCSDSOEMWriter::WriteDataComments() returning true\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool WriteDataSegment()
//------------------------------------------------------------------------------
/**
 * Writes saved data store to a file and then clears data store
 */
//------------------------------------------------------------------------------
bool CCSDSOEMWriter::WriteDataSegment()
{
   #ifdef DEBUG_DATA_SEGMENT
   MessageInterface::ShowMessage("CCSDSOEMWriter::WriteDataSegment() entered\n");
   #endif
   
   if (!emOutStream.is_open())
      return false;
   
   bool retval = true;
   
   Integer numPoints = currentOemSegment.GetNumberOfDataPoints();

   std::stringstream ss;
   if (writeCovariance)
   {
      ss << "\n";
      ss << "COVARIANCE_START\n";
   }

   std::string covRefFrame = currentOemSegment.GetRefFrame();
   
   Real epoch;
   Rvector data;
   for (Integer i = 0; i < numPoints; i++)
   {
      if (currentOemSegment.GetEpochAndData(i, epoch, data))
      {
         if ((i > 0) && (writeCovariance))
            ss << "\n";
         
         //if ((versionNumber == "1.0")&&(data.GetSize() != 6))
         //{
         //   MessageInterface::ShowMessage 
         //      ("*** INTERNAL ERROR *** CCSDS OEM v1.0 data size is not 6\n");
         //   retval = false;
         //   break;
         //}
         
         const Real *outState = data.GetDataVector();
         std::string epochStr = A1ModJulianToUtcGregorian(epoch, 2);
         char strBuff[300];
         sprintf(strBuff, "%s  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e",
            epochStr.c_str(), outState[0], outState[1], outState[2], outState[3],
            outState[4], outState[5]);
         emOutStream << strBuff;

         if (versionNumber == "2.0")
         {
            // Write out acceleration
            if (writeAcceleration)
            {
               sprintf(strBuff, "  % 1.15e  % 1.15e  % 1.15e", 
                  outState[6], outState[7], outState[8]);
               emOutStream << strBuff;
            }
    
            if (writeCovariance)
            {
               // Write out lower-left covariance matrix
               char strBuff1[200];
               ss << "EPOCH = " << epochStr << "\n";
               ss << "COV_REF_FRAME = " << covRefFrame << "\n";
               std::string lineFormat = "% 1.15e";
               Integer idx = 9;
               for (Integer row = 0; row < 6; ++row)
               {
                  for (Integer col = 0; col < 6; ++col)
                  {
                     if (row < col)
                        continue;

                     sprintf(strBuff1, "% 1.15e", outState[idx]);
                     ++idx;
                     ss << strBuff1;
                     if (row == col)
                        ss << "\n";
                     else
                        ss << "  ";
                  }
               }
            }
         }
         emOutStream << "\n";
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** INTERNAL ERROR *** data index %d out of range\n", i);
         retval = false;
         break;
      }
   }
   
   if (writeCovariance)
      ss << "COVARIANCE_STOP\n";

   if (versionNumber == "2.0")
      emOutStream << ss.str();

   emOutStream.flush();      
   
   // Clears data store
   ClearDataStore();

   // Clear meta data
   ClearMetaData();
   #ifdef DEBUG_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("CCSDSOEMWriter::WriteDataSegment() returning %d\n", retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// bool SetMetaDataForWriting(const std::string &fieldName, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets meta data for writing. Once meta data is written to a file it will be
 * cleared.
 *
 * @param fieldName Metadata field name to be set
 * @param value Value to set to the field
 */
//------------------------------------------------------------------------------
bool CCSDSOEMWriter::SetMetaDataForWriting(const std::string &fieldName,
                                           const std::string &value)
{
   #ifdef DEBUG_META_DATA
   MessageInterface::ShowMessage("CCSDSOEMWriter::SetMetaDataForWriting() entered\n");
   #endif
   
   currentOemSegment.SetMetaDataForWriting(fieldName, value);
   
   #ifdef DEBUG_META_DATA
   MessageInterface::ShowMessage("CCSDSOEMWriter::SetMetaDataForWriting() returning true\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool AddMetaComment(const std::string& comment)
//------------------------------------------------------------------------------
/**
 * Adds meta comment for writing. Once meta data is written to a file it will be
 * cleared.
 *
 * @param comment comment to be added to the metadata comments
 */
//------------------------------------------------------------------------------
bool CCSDSOEMWriter::AddMetaComment(const std::string& comment)
{
   #ifdef DEBUG_META_COMMENT
   MessageInterface::ShowMessage("CCSDSOEMWriter::AddMetaComment() entered\n");
   #endif
   
   currentOemSegment.AddMetaComment(comment);
   
   #ifdef DEBUG_META_COMMENT
   MessageInterface::ShowMessage("CCSDSOEMWriter::AddMetaComment() returning true\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool AddDataComment(const std::string& comment)
//------------------------------------------------------------------------------
/**
 * Adds data comment for writing. Once data segment is written to a file it will be
 * cleared.
 *
 * @param comment comment to be added to the data comments
 */
//------------------------------------------------------------------------------
bool CCSDSOEMWriter::AddDataComment(const std::string& comment)
{
   #ifdef DEBUG_DATA_COMMENT
   MessageInterface::ShowMessage("CCSDSOEMWriter::AddDataComment() entered\n");
   #endif
   
   currentOemSegment.AddDataComment(comment);
   
   #ifdef DEBUG_DATA_COMMENT
   MessageInterface::ShowMessage("CCSDSOEMWriter::AddDataComment() leaving\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool AddDataForWriting(Real epoch, Rvector data)
//------------------------------------------------------------------------------
/**
 * Adds data to a data store for writing. Once data segment is written to a
 * file the data store will be cleared.
 *
 * @epoch Epoch of data to be added to the CCSDS data store
 * @data Data to be added to the CCSDS data store
 */
//------------------------------------------------------------------------------
bool CCSDSOEMWriter::AddDataForWriting(Real epoch, Rvector &data)
{
   #ifdef DEBUG_DATA_SEGMENT
   MessageInterface::ShowMessage("CCSDSOEMWriter::AddDataForWriting() entered\n");
   #endif
   
   currentOemSegment.AddDataForWriting(epoch, data);
   
   #ifdef DEBUG_DATA_SEGMENT
   MessageInterface::ShowMessage("CCSDSOEMWriter::AddDataForWriting() entered\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// void ClearMetaComments()
//------------------------------------------------------------------------------
/**
 * Clears meta data comments
 */
//------------------------------------------------------------------------------
void CCSDSOEMWriter::ClearMetaComments()
{
   currentOemSegment.ClearMetaComments();
}

//------------------------------------------------------------------------------
// void ClearDataComments()
//------------------------------------------------------------------------------
void CCSDSOEMWriter::ClearDataComments()
{
   currentOemSegment.ClearDataComments();
}

//------------------------------------------------------------------------------
// void ClearMetaData()
//------------------------------------------------------------------------------
/**
 * Clears meta comments and data
 */
//------------------------------------------------------------------------------
void CCSDSOEMWriter::ClearMetaData()
{
   #ifdef DEBUG_META_DATA
   MessageInterface::ShowMessage("CCSDSOEMWriter::ClearMetaData() entered\n");
   #endif
   
   // ClearMetaData() also clears meta comments
   currentOemSegment.ClearMetaData();
   
   #ifdef DEBUG_META_DATA
   MessageInterface::ShowMessage("CCSDSOEMWriter::ClearMetaData() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void ClearDataStore()
//------------------------------------------------------------------------------
/**
 * Clears data store and comments.
 */
//------------------------------------------------------------------------------
void CCSDSOEMWriter::ClearDataStore()
{
   #ifdef DEBUG_DATA_SEGMENT
   MessageInterface::ShowMessage("CCSDSOEMWriter::ClearDataStore() entered\n");
   #endif
   
   // ClearDataStore() also clears data comments
   currentOemSegment.ClearDataStore();
   
   #ifdef DEBUG_DATA_SEGMENT
   MessageInterface::ShowMessage("CCSDSOEMWriter::ClearDataStore() leaving\n");
   #endif
}

// -----------------------------------------------------------------------------
// protected methods
// -----------------------------------------------------------------------------

