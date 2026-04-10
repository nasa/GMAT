//------------------------------------------------------------------------------
//                         RampTableType
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
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: 2013/07/02
//
/**
 * RampTableType class used for the GMAT_RampTable data
 */
//------------------------------------------------------------------------------


#include "RampTableType.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "FileManager.hpp"
#include "StringUtil.hpp"
#include "MeasurementException.hpp"
#include <sstream>


//#define DEBUG_RAMPTABLETYPE_CREATION_INITIALIZATION
//#define DEBUG_FILE_WRITE
//#define DEBUG_FILE_READ
//#define DEBUG_FILE_ACCESS

//-----------------------------------------------------------------------------
// RampTableType(const std::string withName)
//-----------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param withName The name of the new object
 */
//-----------------------------------------------------------------------------
RampTableType::RampTableType(const std::string withName) :
   ObType   ("GMAT_RampTable", withName),
   epochPrecision      (16),
   dataPrecision       (6)
{
   #ifdef DEBUG_RAMPTABLETYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Creating a GMAT_RampTable object type\n");
   #endif

}

//-----------------------------------------------------------------------------
// ~RampTableType()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
RampTableType::~RampTableType()
{
}


//-----------------------------------------------------------------------------
// RampTableType(const RampTableType& rtt) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rtt The RampTableType that gets copied to this one
 */
//-----------------------------------------------------------------------------
RampTableType::RampTableType(const RampTableType& rtt) :
   ObType            (rtt),
   epochPrecision    (rtt.epochPrecision),
   dataPrecision     (rtt.dataPrecision)
{
   #ifdef DEBUG_RAMPTABLETYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Copying a GMAT_RampTable object type\n");
   #endif
}


//-----------------------------------------------------------------------------
// RampTableType& operator=(const RampTableType& rtt)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rtt The RampTableType that gets copied to this one
 *
 * @return This RampTableType, configured to match rtt
 */
//-----------------------------------------------------------------------------
RampTableType& RampTableType::operator=(const RampTableType& rtt)
{
   #ifdef DEBUG_RAMPTABLETYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Assigning one GMAT_RampTable object type to another\n");
   #endif

   if (this != &rtt)
   {
      epochPrecision = rtt.epochPrecision;
      dataPrecision  = rtt.dataPrecision;
   }

   return *this;
}


//-----------------------------------------------------------------------------
// GmatBase* Clone() const
//-----------------------------------------------------------------------------
/**
 * Cloning method used to create a RampTableType from a GmatBase pointer
 *
 * @return A new RampTableType object matching this one
 */
//-----------------------------------------------------------------------------
GmatBase* RampTableType::Clone() const
{
   #ifdef DEBUG_RAMPTABLETYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Cloning a GMAT_RampTable object type\n");
   #endif

   return new RampTableType(*this);
}


//-----------------------------------------------------------------------------
// bool Initialize()
//-----------------------------------------------------------------------------
/**
 * Prepares this GmatObType for use
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool RampTableType::Initialize()
{
#ifdef DEBUG_FREQUENCYRAMPTABLE_CREATION_INITIALIZATION
   MessageInterface::ShowMessage("RampTableType::Initialize() Executing\n");
#endif

   ObType::Initialize();

   return true;
}


//-----------------------------------------------------------------------------
// bool Open(bool forRead, bool forWrite, bool append)
//-----------------------------------------------------------------------------
/**
 * Opens a GmatODType stream for processing
 *
 * The method manages RampTableType path and file extension defaults in addition to
 * performing the basic open operations.
 *
 * @param forRead True to open for reading, false otherwise
 * @param forWrite True to open for writing, false otherwise
 * @param append True if data being written should be appended, false if not
 *
 * @return true if the the stream was opened, false if not
 */
//-----------------------------------------------------------------------------
bool RampTableType::Open(bool forRead, bool forWrite, bool append)
{
   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("RampTableType::Open(%s, %s, %s) Executing\n",
            (forRead ? "true" : "false"),
            (forWrite ? "true" : "false"),
            (append ? "true" : "false") );
   #endif

   // when theStream open twice the content in theStream is always empty string. Therefore, it needs this verification.
   if (theStream.is_open())
      return true;

   bool retval = false;

   // temporary
   retval = true;
   std::ios_base::openmode mode = std::fstream::in;

   if (forRead && forWrite)
      mode = std::fstream::in | std::fstream::out;

   else
   {
      if (forRead)
         mode = std::fstream::in;
      if (forWrite)
         mode = std::fstream::out;
   }

   if (append)
      mode = mode | std::fstream::app;

   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("   Opening the stream %s, mode = %d\n",
            streamName.c_str(), mode);
   #endif

   std::string fullPath = "";
   if (streamName != "")
   {
      // If no path designation slash character is found, add the default path
      if ((streamName.find('/') == std::string::npos) &&
          (streamName.find('\\') == std::string::npos))
      {
         FileManager *fm = FileManager::Instance();
         fullPath = fm->GetPathname(FileManager::MEASUREMENT_PATH);
      }
      fullPath += streamName;

      // Add the .gmd extension if there is no extension in the file
      size_t dotLoc = fullPath.find_last_of('.');                         // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
      size_t slashLoc = fullPath.find_last_of('/');                       // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
      if (slashLoc == std::string::npos)
         slashLoc = fullPath.find_last_of('\\');

      if ((dotLoc == std::string::npos) ||
          (dotLoc < slashLoc))
      {
         fullPath += ".rmp";
      }

      // Change '\' to '/' in fullPath
      for (Integer i = 0; i < fullPath.size(); ++i)
      {
         if (fullPath.at(i) == '\\')
            fullPath.at(i) = '/';
      }

      #ifdef DEBUG_FILE_ACCESS
         MessageInterface::ShowMessage("   Full path <%s>, mode = %d\n", 
               fullPath.c_str(), mode);
         MessageInterface::ShowMessage("   open theStream = <%p>\n", theStream);
      #endif
      try
      {
         theStream.open(fullPath.c_str(), mode);
      } catch(...)
      {
         throw MeasurementException("Error: GMAT can't open ramp table file '" + fullPath +"'.\n");
      }
   }
   
   retval = theStream.is_open();

   if (retval == false)
      throw MeasurementException("Error: GMAT can't open ramp table file '"+ fullPath +"'.\n");
   
   return retval;
}

//-----------------------------------------------------------------------------
// bool IsOpen()
//-----------------------------------------------------------------------------
/**
 * Tests to see if the RampTableType file has been opened
 *
 * @return true if the file is open, false if not.
 */
//-----------------------------------------------------------------------------
bool RampTableType::IsOpen()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("RampTableType::IsOpen() Executing\n");
   #endif
   return theStream.is_open();
}



//-----------------------------------------------------------------------------
// RampTableData* ReadRampTableData()
//-----------------------------------------------------------------------------
/**
 * Retrieves a record from ramp table
 *
 * This method reads a record from a Gmat_RampTable data stream and
 * returns the data to the caller.
 *
 * @return The ramp table record from the stream.  If there is no more data in
 * the stream, a NULL pointer is returned.
 */
//-----------------------------------------------------------------------------
RampTableData* RampTableType::ReadRampTableData()
{
   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage("RampTableType::ReadRampTableData() Executing\n");
   #endif

   std::string s1;
   std::stringstream theLine;
   std::stringstream theLine1;

   Integer participantSize;

   // Do nothing when it is at the end of file
   if (theStream.eof())
      return NULL;
   
   // Read a line when it is not end of file
   std::getline(theStream, s1);
   
   // Skip header and comment lines or empty lines
   while ((s1[0] == '%') || (GmatStringUtil::RemoveAllBlanks(s1) == "") ||
          (s1.length() < 2))
   {
      std::getline(theStream, s1);
      
      // Do nothing when it is at the end of file
      if (theStream.eof())
         return NULL;
   }
   

   // Processing data in the line
   theLine << s1;
   theLine1 << s1;
   currentRecord.Clear();
   currentRecord.dataFormat = "GMAT_RampTable";

   // Record format of frequency ramp table: 
   // Epoch   StationID   SpacecraftID    Uplink Band       Ramp Type     Ramp Frequency     Ramp Rate
   // Real    string      string          Integer           Integer      Real               Real
   Real value; 

   std::string taiEpochStr;
   theLine1 >> taiEpochStr;

   GmatTime taiEpochGT;
   taiEpochGT.SetMjdString(taiEpochStr);
   currentRecord.epochGT = (currentRecord.epochSystem == TimeSystemConverter::TAIMJD ?
              taiEpochGT :
              theTimeConverter->ConvertFromTaiMjd(currentRecord.epochSystem, taiEpochGT,
              GmatTimeConstants::JD_NOV_17_1858));


   GmatEpoch taiEpoch;
   theLine >> taiEpoch;

   currentRecord.epoch = (currentRecord.epochSystem == TimeSystemConverter::TAIMJD ?
         taiEpoch :
         theTimeConverter->ConvertFromTaiMjd(currentRecord.epochSystem, taiEpoch,
               GmatTimeConstants::JD_NOV_17_1858));
/*
   switch (currentRecord.type)
   {
      case Gmat::GEOMETRIC_RANGE:
      case Gmat::GEOMETRIC_RANGE_RATE:
      case Gmat::USN_TWOWAYRANGE:
      case Gmat::USN_TWOWAYRANGERATE:
      case Gmat::DSN_TWOWAYRANGE:
      case Gmat::DSN_TWOWAYDOPPLER:
         participantSize = 2;
         break;

      case Gmat::TDRSS_TWOWAYRANGE:
      case Gmat::TDRSS_TWOWAYRANGERATE:
         participantSize = 3;
         break;

      case Gmat::GEOMETRIC_AZ_EL:
      case Gmat::GEOMETRIC_RA_DEC:
      case Gmat::OPTICAL_AZEL:
      case Gmat::OPTICAL_RADEC:
         participantSize = 2;
         break;

      default:
         participantSize = 0;
         break;
   }
*/
   participantSize = 2;
   for (Integer i = 0; i < participantSize; ++i)
   {
      theLine >> s1;
      currentRecord.participantIDs.push_back(s1);
   }

   theLine >> currentRecord.uplinkBand;
   theLine >> currentRecord.rampType;
   theLine >> currentRecord.rampFrequency;
   theLine >> currentRecord.rampRate;

   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage(" %.12lf    %s    %d    ", currentRecord.epoch, currentRecord.typeName.c_str(), currentRecord.type);
      for (Integer i = 0; i < participantSize; ++i)
         MessageInterface::ShowMessage("%s    ", currentRecord.participantIDs.at(i).c_str());

      MessageInterface::ShowMessage(" %d    %d    %.12le    %.12le\n",currentRecord.uplinkBand, currentRecord.rampType, currentRecord.rampFrequency, currentRecord.rampRate);
      MessageInterface::ShowMessage("RampTableType::ReadRampTableData() End\n");
   #endif

   return &currentRecord;
}


//RampTableData* RampTableType::ReadRampTableData()
//{
//#ifdef DEBUG_FILE_READ
//   MessageInterface::ShowMessage("RampTableType::ReadRampTableData() Executing\n");
//#endif
//
//   std::string str;
//   std::stringstream theLine;
//   Integer participantSize;
//
//   // Do nothing when it is at the end of file
//   if (theStream.eof())
//      return NULL;
//
//   // Read a line when it is not end of file
//   std::getline(theStream, str);
//
//   // Skip header and comment lines or empty lines
//   while ((str[0] == '%') || (GmatStringUtil::RemoveAllBlanks(str) == "") ||
//      (str.length() < 2))
//   {
//      std::getline(theStream, str);
//
//      // Do nothing when it is at the end of file
//      if (theStream.eof())
//         return NULL;
//   }
//
//
//   // Processing data in the line
//   theLine << str;
//   currentRecord.Clear();
//   currentRecord.dataFormat = "GMAT_RampTable";
//
//   // Record format of frequency ramp table: 
//   // Epoch   StationID   SpacecraftID    Uplink Band       Ramp Type     Ramp Frequency     Ramp Rate
//   // Real    string      string          Integer           Integer      Real               Real
//   Real value;
//
//   GmatEpoch taiEpoch;
//   theLine >> taiEpoch;
//
//   currentRecord.epoch = (currentRecord.epochSystem == TimeSystemConverter::TAIMJD ?
//   taiEpoch :
//            theTimeConverter->ConvertFromTaiMjd(currentRecord.epochSystem, taiEpoch,
//            GmatTimeConstants::JD_NOV_17_1858));
//
//   participantSize = 2;
//   for (Integer i = 0; i < participantSize; ++i)
//   {
//      theLine >> str;
//      currentRecord.participantIDs.push_back(str);
//   }
//
//   theLine >> currentRecord.uplinkBand;
//   theLine >> currentRecord.rampType;
//   theLine >> currentRecord.rampFrequency;
//   theLine >> currentRecord.rampRate;
//
//#ifdef DEBUG_FILE_READ
//   MessageInterface::ShowMessage(" %.12lf    %s    %d    ", currentRecord.epoch, currentRecord.typeName.c_str(), currentRecord.type);
//   for (Integer i = 0; i < participantSize; ++i)
//      MessageInterface::ShowMessage("%s    ", currentRecord.participantIDs.at(i).c_str());
//
//   MessageInterface::ShowMessage(" %d    %d    %.12le    %.12le\n", currentRecord.uplinkBand, currentRecord.rampType, currentRecord.rampFrequency, currentRecord.rampRate);
//   MessageInterface::ShowMessage("RampTableType::ReadRampTableData() End\n");
//#endif
//
//   return &currentRecord;
//}


//-----------------------------------------------------------------------------
// bool Close()
//-----------------------------------------------------------------------------
/**
 * Closes the data stream
 *
 * This method flushes the data stream, and then closes it.
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool RampTableType::Close()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("RampTableType::Close() Executing\n");
   #endif
   bool retval = false;

   if (theStream.is_open())
   {
      theStream.flush();
      theStream.close();
      retval = !(theStream.is_open());
   }

   return retval;
}


//-----------------------------------------------------------------------------
// bool RampTableType::Finalize()
//-----------------------------------------------------------------------------
/**
 * Completes operations on this RampTableType.
 *
 * @return true always -- there is no RampTableType specific finalization needed.
 */
//-----------------------------------------------------------------------------
bool RampTableType::Finalize()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("RampTableType::Finalize() Executing\n");
   #endif
   bool retval = true;

   return retval;
}
