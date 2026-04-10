//------------------------------------------------------------------------------
//                                  CCSDSEMWriter
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
// Created: 2016.01.13
//
/**
 * Writes a CCSDS Ephemeris Message to a file.
 * This is the base class from which other classes must derive (e.g.
 * a class to handle attitude files)
 */
//------------------------------------------------------------------------------

#include "CCSDSEMWriter.hpp"
#include "FileUtil.hpp"
#include "FileTypes.hpp"
#include "StringUtil.hpp"
#include "TimeTypes.hpp"             // for FormatCurrentTime()
#include "UtilityException.hpp"
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_INIT_EM_FILE
//#define DEBUG_EM_FILE

// -----------------------------------------------------------------------------
// static data
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// default constructor
// -----------------------------------------------------------------------------
CCSDSEMWriter::CCSDSEMWriter() :
   versionNumber (""),
   writeAcceleration (false),
   writeCovariance   (false),
   originator    (""),
   creationTime  (""),
   emFileName    ("")
{
   theTimeConverter = TimeSystemConverter::Instance();
}

// -----------------------------------------------------------------------------
// copy constructor
// -----------------------------------------------------------------------------
CCSDSEMWriter::CCSDSEMWriter(const CCSDSEMWriter &copy) :
   versionNumber (copy.versionNumber),
   writeAcceleration (copy.writeAcceleration),
   writeCovariance(copy.writeCovariance),

   originator    (copy.originator),
   creationTime  (copy.creationTime),
   emFileName    (copy.emFileName)
{
   theTimeConverter = TimeSystemConverter::Instance();
}

// -----------------------------------------------------------------------------
// operator=
// -----------------------------------------------------------------------------
CCSDSEMWriter& CCSDSEMWriter::operator=(const CCSDSEMWriter &copy)
{
   if (&copy == this)
      return *this;

   versionNumber = copy.versionNumber;
   writeAcceleration = copy.writeAcceleration;
   writeCovariance = copy.writeCovariance;

   originator    = copy.originator;
   creationTime  = copy.creationTime;
   emFileName    = copy.emFileName;
   
   return *this;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
CCSDSEMWriter::~CCSDSEMWriter()
{
   emOutStream.flush();
   emOutStream.close();
}

// -----------------------------------------------------------------------------
// bool OpenFile(const std::string &filename)
// -----------------------------------------------------------------------------
/**
 * Opens CCSDS output file for writing.
 *
 * @filename File name to open
 */
// -----------------------------------------------------------------------------
bool CCSDSEMWriter::OpenFile(const std::string &filename)
{
   #ifdef DEBUG_EM_FILE
   MessageInterface::ShowMessage
      ("CCSDSEMWriter::OpenFile() entered, filename='%s'\n", filename.c_str());
   #endif
   
   bool retval = false;
   
   if (emOutStream.is_open())
      emOutStream.close();
   
   emFileName = filename;
   emOutStream.open(emFileName.c_str());
   
   if (emOutStream.is_open())
   {
      retval = true;
      #ifdef DEBUG_EM_FILE
      MessageInterface::ShowMessage
         ("   Successfully opened '%s' for writing, originator='%s'\n",
          emFileName.c_str(), originator.c_str());
      #endif
   }
   else
   {
      retval = false;
      #ifdef DEBUG_EM_FILE
      MessageInterface::ShowMessage
         ("   Failed open '%s' for writing\n", emFileName.c_str());
      #endif
   }
   
   #ifdef DEBUG_EM_FILE
   MessageInterface::ShowMessage
      ("CCSDSEMWriter::OpenFile() returning %d\n", retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// bool SetHeaderForWriting(const std::string &fieldName,
//                          const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets EM header data for writing. It does not validate the input value.
 *
 * @param fieldName Header field name to be set
 * @param value Value to set to the field
 */
//------------------------------------------------------------------------------
bool CCSDSEMWriter::SetHeaderForWriting(const std::string &fieldName,
                                        const std::string &value)
{
   if (fieldName == "VERSION_NUMBER")
      versionNumber = value;
   else if (fieldName == "COMMENT")
      headerComments.push_back(value);
   else if (fieldName == "CREATION_DATE")
      creationTime = value;
   else if (fieldName == "ORIGINATOR")
      originator = value;
   else
      throw UtilityException
         ("The field \"" + fieldName + "\" is not valid EM header field.\n"
          "Valid fields are: VERSION_NUMBER, COMMENT, CREATION_DATE, ORIGINATOR");
   
   return true;
}

//------------------------------------------------------------------------------
// bool WriteHeader(const std::string &versionFieldName)
//------------------------------------------------------------------------------
/**
 * Formats and writes header to a file. If output stream is not opened,
 * it just returns false.
 *
 * @versionFieldName The version keyword to be written out
 *                   For OEM, it should be "CCSDS_OEM_VERS" but it
 *                   doesn't validate the input
 */
//------------------------------------------------------------------------------
bool CCSDSEMWriter::WriteHeader(const std::string &versionFieldName)
{
   if (!emOutStream.is_open())
      return false;
   
   creationTime = GmatTimeUtil::FormatCurrentTime(2);
   
   std::stringstream ss("");
   ss << versionFieldName << " = " << versionNumber << std::endl;
   //ss << "CCSDS_OEM_VERS = " << versionNumber << std::endl;
   for (unsigned int i = 0; i < headerComments.size(); i++)
      ss << "COMMENT  " << headerComments[i] << std::endl;
   ss << "CREATION_DATE  = " << creationTime << std::endl;
   ss << "ORIGINATOR     = " << originator << std::endl;
   
   emOutStream << ss.str();
   emOutStream.flush();
   
   // It is incorrect to call ClearHeader() function here bacause it clears
   // value of versionNumber is used in writing a segment of ephem file
   //ClearHeader();

   return true;
}

//------------------------------------------------------------------------------
// bool WriteBlankLine()
//------------------------------------------------------------------------------
/**
 * Writes blank line to a file.
 */
//------------------------------------------------------------------------------
bool CCSDSEMWriter::WriteBlankLine()
{
   if (!emOutStream.is_open())
      return false;
   
   emOutStream << std::endl;
   emOutStream.flush();
   return true;
}

//------------------------------------------------------------------------------
// bool WriteString(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Writes input string to a file.
 */
//------------------------------------------------------------------------------
bool CCSDSEMWriter::WriteString(const std::string &str)
{
   #ifdef DEBUG_WRITE_STRING
   MessageInterface::ShowMessage
      ("CCSDSEMWriter::WriteString() entered, str='%s'\n", str.c_str());
   #endif
   
   if (!emOutStream.is_open())
      return false;
   
   emOutStream << str << std::endl;
   emOutStream.flush();
   
   #ifdef DEBUG_WRITE_STRING
   MessageInterface::ShowMessage
      ("CCSDSEMWriter::WriteString() returning true\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// void ClearHeaderComments()
//------------------------------------------------------------------------------
/**
 * Clears header comments.
 */
//------------------------------------------------------------------------------
void CCSDSEMWriter::ClearHeaderComments()
{
   headerComments.clear();
}


//------------------------------------------------------------------------------
// void ClearHeader()
//------------------------------------------------------------------------------
/**
 * Clears header information.
 */
//------------------------------------------------------------------------------
void CCSDSEMWriter::ClearHeader()
{
   ClearHeaderComments();
   versionNumber = "";
   originator    = "";
   creationTime  = "";
   emFileName    = "";
}

// -----------------------------------------------------------------------------
// protected methods
// -----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// std::string A1ModJulianToUtcGregorian(Real epochInDays, Integer format = 1)
//------------------------------------------------------------------------------
/**
 * Formats epoch in either in days or seconds to desired format.
 *
 * @param  epoch   Epoch in A1 days
 * @param  format  Desired output format [1]
 *                 1 = "01 Jan 2000 11:59:28.000"
 *                 2 = "2000-01-01T11:59:28.000"
 */
//------------------------------------------------------------------------------
std::string CCSDSEMWriter::A1ModJulianToUtcGregorian(Real epochInDays, Integer format)
{
   if (epochInDays == -999.999)
      return "-999.999";
   
   Real toMjd;
   std::string epochStr;
   std::string outFormat = "UTCGregorian";
   
   // Convert current epoch to specified format
   theTimeConverter->Convert("A1ModJulian", epochInDays, "", outFormat,
                              toMjd, epochStr, format);
   
   if (epochStr == "")
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** EphemerisWriter::ToUtcGregorian() Cannot convert epoch %.10f %s "
          "to UTCGregorian\n", epochInDays, "days");
      
      epochStr = "EpochError";
   }
   
   return epochStr;
}



bool CCSDSEMWriter::SetWritingAccelerationOption(const bool writeOption)
{
   writeAcceleration = writeOption;
   return writeAcceleration;
};

                                                   

bool CCSDSEMWriter::SetWritingCovarianceOption(const bool writeOption)
{
   writeCovariance = writeOption;
   return writeCovariance;
};
