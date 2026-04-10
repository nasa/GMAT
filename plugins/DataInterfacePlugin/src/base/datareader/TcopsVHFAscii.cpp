//$Id$
//------------------------------------------------------------------------------
//                           TcopsVHFAscii
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: May 3, 2013
/**
 * Implementation of the TCOPS Vector Hold File ASCII reader
 */
//------------------------------------------------------------------------------

#include "TcopsVHFAscii.hpp"
#include "InterfaceException.hpp"
#include "StringUtil.hpp"
#include "DateUtil.hpp"             // To get the ModJulian date
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_FILEREAD

//------------------------------------------------------------------------------
// TcopsVHFAscii(const std::string& theName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param theName Name of this reader
 */
//------------------------------------------------------------------------------
TcopsVHFAscii::TcopsVHFAscii(const std::string& theName) :
   TcopsVHFData         ("TVHF_ASCII", theName),
   isDumpFile           (false),
   utcEpoch             (0.0)
{
   // Set up the engine accessor fields
   objectTypeNames.push_back("TVHF_ASCII");
   objectTypeNames.push_back("TcopsVHFAscii");

   for (Integer i = 0; i < 7; ++i)
      startVector[i] = 0.0;
}

//------------------------------------------------------------------------------
// ~TcopsVHFAscii()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TcopsVHFAscii::~TcopsVHFAscii()
{
}

//------------------------------------------------------------------------------
// TcopsVHFAscii(const TcopsVHFAscii& vhf) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param vhf The reader copied to make the new one
 */
//------------------------------------------------------------------------------
TcopsVHFAscii::TcopsVHFAscii(const TcopsVHFAscii& vhf) :
   TcopsVHFData         (vhf),
   isDumpFile           (vhf.isDumpFile),
   utcEpoch             (0.0)
{
   for (Integer i = 0; i < 7; ++i)
      startVector[i] = 0.0;
}

//------------------------------------------------------------------------------
// TcopsVHFAscii& operator=(const TcopsVHFAscii& vhf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param vhf The reader copied into this one
 *
 * @return This reader, set to match vhf
 */
//------------------------------------------------------------------------------
TcopsVHFAscii& TcopsVHFAscii::operator=(const TcopsVHFAscii& vhf)
{
   if (this == &vhf)
   {
      TcopsVHFData::operator=(vhf);

      isDumpFile = vhf.isDumpFile;
      for (Integer i = 0; i < 7; ++i)
         startVector[i] = 0.0;
      utcEpoch = 0.0;
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method to make a publicly identiccal copy of this reader
 *
 * @return The copy
 */
//------------------------------------------------------------------------------
GmatBase* TcopsVHFAscii::Clone() const
{
   return new TcopsVHFAscii(*this);
}

//------------------------------------------------------------------------------
// bool ReadData()
//------------------------------------------------------------------------------
/**
 * The method that actually reads in all of the data
 *
 * @return true if the read succeeds, meaning that the file is a TVHF and it
 *              contained at least one data element that we requested
 */
//------------------------------------------------------------------------------
bool TcopsVHFAscii::ReadData()
{
   #ifdef DEBUG_FILEREAD
      MessageInterface::ShowMessage("TcopsVHFAscii::ReadData() entered\n");
   #endif

   bool retval = false;

   if (clearOnRead)
   {
      realData.clear();
      rvector6Data.clear();
      stringData.clear();

      for (Integer i = 0; i < 7; ++i)
         startVector[i] = 0.0;
   }

   if (theStream)
   {
      std::string theLine;
      Integer count = 0, blockCount = 0;
      bool finished = false;
      bool headerFound = false;

      dataBuffer.clear();

      while (!finished)
      {
         if (ReadLine(theLine))
         {
            ++count;
            #ifdef DEBUG_FILEREAD
               MessageInterface::ShowMessage("%d: \"%s\"\n", count,
                     theLine.c_str());
            #endif

            // Skip blank lines
            if (theLine != "")
            {
               if (!headerFound)
               {
                  // Don't do any extra processing
                  headerFound = CheckForHeader(theLine);
                  if (headerFound)
                     ManageStartData();
                  else
                     dataBuffer.push_back(theLine);
               }
               else
               {
                  if (!CheckForBlockBoundary(theLine))
                  {
                     dataBuffer.push_back(theLine);
                  }
                  else
                  {
                     ++blockCount;
                     if (blockCount > 1)
                     {
                        // Put the logic here to see if we are at the requested
                        // block; for now, we just keep the first block
                        finished = true;
                     }
                  }
               }
            }
         }
         else
            // ReadLine returns false on EOF
            finished = true;

         // The header line must occur in the first 100 lines of the file
         if ((count > 100) && !headerFound)
            finished = true;
      }

      // Now that the file has been read, parse the data for the desired fields
      retval = ParseDataBlock();

      BuildOriginName();
      BuildCSName();

      dataReady = retval;
   }
   #ifdef DEBUG_FILEREAD
      else
         MessageInterface::ShowMessage("TcopsVHFAscii::ReadData(): The data "
               "stream was not set\n");
   #endif

   return retval;
}

//------------------------------------------------------------------------------
// bool CheckForHeader(const std::string& theLine)
//------------------------------------------------------------------------------
/**
 * Checks the header to see if the file is a VHF dump
 *
 * VHF dumps are identified by a specific string in the text od the file.  This
 * method checks the current line to see if it contains that string.
 * In addition, it uses the detected string to identify if the file is an
 * unedited dump file or a Task 9 edited file, and sets the isDumpFile flag true
 * for the unedited version.
 *
 * For task 9 files, the data at the top of the file is loaded into the
 * startVector array.  Those data are compared to the parsed out data in
 * ParseDataBlock() and, if different, post a warning message.
 *
 * @param theLine The line that is being checked to see if the file is a VHF
 *
 * @return true if a header line was detected, false if not
 */
//------------------------------------------------------------------------------
bool TcopsVHFAscii::CheckForHeader(const std::string& theLine)
{
   bool isHeader = false;

   std::string upperLine = theLine;
   transform(upperLine.begin(), upperLine.end(), upperLine.begin(), toupper);

   // Unedited dump file
   if (upperLine.find("TCOPS VECTOR HOLD FILE DUMP PROGRAM") !=
         std::string::npos)
   {
      isDumpFile = true;
      isHeader = true;
   }

   // Task 9 version
   if (upperLine.find("TVHF ELEMENT SET SUMMARY") != std::string::npos)
      isHeader = true;

   return isHeader;
}

//------------------------------------------------------------------------------
// bool TcopsVHFAscii::CheckForBlockBoundary(const std::string& theLine)
//------------------------------------------------------------------------------
/**
 * Identifies block boundaries for the unedited TVHF.
 *
 * @param theLine The current line from the file
 *
 * @return true if the block header was found, false if not
 */
//------------------------------------------------------------------------------
bool TcopsVHFAscii::CheckForBlockBoundary(const std::string& theLine)
{
   bool retval = false;

   // The unedited dump file starts each block with a block header; other
   // formats just read to the end
   if (isDumpFile)
   {
      std::string upperLine = theLine;
      transform(upperLine.begin(), upperLine.end(), upperLine.begin(), toupper);
      if (upperLine.find("LONG REPORT OF THE TCOPS VECTOR HOLD FILE") !=
            std::string::npos)
         retval = true;
   }

   return retval;
}

//------------------------------------------------------------------------------
// void ManageStartData()
//------------------------------------------------------------------------------
/**
 * Processes data found before the line that identified this file as a TVFH
 *
 * The data parsed here is the initial data vector in a Task 9 formatted file.
 * At his time there is no other pre-header information processed.
 */
//------------------------------------------------------------------------------
void TcopsVHFAscii::ManageStartData()
{
   // Task 9 files have the isDumpFile flag set false
   if (!isDumpFile)
   {
      // Fill in the starting vector, assumed to be first 7 lines of the file
      Real value;
      Integer errorCode;
      if (dataBuffer.size() >= 7)
      {
         for (Integer i = 0; i < 7; ++i)
         if (GmatStringUtil::IsValidReal(dataBuffer[i], value, errorCode))
            startVector[i] = value;
         else
         {
            throw InterfaceException("The Task 9 TVHF \"" + filename +
                  "\" initial vector contains invalid data in the element \"" +
                  dataBuffer[i] + "\"");
         }
      }
      else
         throw InterfaceException("The Task 9 TVHF \"" + filename +
               "\" does not contain an initial vector");

      #ifdef DEBUG_FILEREAD
         MessageInterface::ShowMessage("Code 9 file detected with initial "
               "data:\n");
         for (Integer i = 0; i < 7; ++i)
            MessageInterface::ShowMessage("   %16lf\n", startVector[i]);
      #endif
   }
}

//------------------------------------------------------------------------------
// bool ParseDataBlock()
//------------------------------------------------------------------------------
/**
 * Pulls data from a loaded data block
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TcopsVHFAscii::ParseDataBlock()
{
   bool retval = false;

   for (UnsignedInt i = 0; i < supportedFields.size(); ++i)
   {
      std::string theField = supportedFields[i];
      if (readAllSupportedFields || (find(selectedFields.begin(),
            selectedFields.end(), theField) != selectedFields.end()))
      {
         // Read the data block looking for a file string match
         for (UnsignedInt i = 0; i < dataBuffer.size(); ++i)
         {
            if (dataBuffer[i].find(fileStringMap[theField])!=std::string::npos)
            {
               switch (dataType[theField])
               {
               case READER_REAL:
                  if (ParseRealValue(i, theField))
                     retval = true;;
                  break;

               case READER_RVECTOR6:
                  {
                     // Set up for the Cartesian State scan
                     StringArray theIds;
                     theIds.push_back("X ");   // ' ' to distinguish from XDOT,
                     theIds.push_back("Y ");   // YDOT, and ZDOT
                     theIds.push_back("Z ");
                     theIds.push_back("XDOT");
                     theIds.push_back("YDOT");
                     theIds.push_back("ZDOT");
                     // And find it
                     if (ParseRvector6Value(i, theField, theIds))
                        retval = true;
                  }
                  break;

               case READER_STRING:
               case READER_TIMESTRING:
                  if (ParseStringValue(i, theField))
                  {
                     if (dataType[theField] == READER_TIMESTRING)
                        ParseTime(theField);
                     retval = true;
                  }
                  break;

               default:
                  // Skip the other types (basically the subtypes)
                  break;
               }
            }
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void ParseTime(std::string& theField)
//------------------------------------------------------------------------------
/**
 * Special handler for the time data in the TVHF
 *
 * @param theField The field identifier for the string containing epoch data
 */
//------------------------------------------------------------------------------
void TcopsVHFAscii::ParseTime(std::string& theField)
{
   // Only process if the field is in the string map
   if (stringData.find(theField) != stringData.end())
   {
      std::stringstream theData;
      theData << stringData[theField];

      #ifdef DEBUG_FILEREAD
         MessageInterface::ShowMessage("      Raw data: \"%s\" stream data: "
               "\"%s\"\n", stringData[theField].c_str(), theData.str().c_str());
      #endif

      Integer year, month, day, hour, minute;
      Real second;

      theData >> year >> month >> day >> hour >> minute >> second;

      if (year < 50)
         year += 2000;
      else if (year < 100)
         year += 1900;

      // Validate the ranges
      std::stringstream errstream;
      if (year < 1950)
         errstream << "   The specified year, " << year << ", is not valid.\n";
      if ((month < 1) || (month > 12))
         errstream << "   The specified month, " << month 
                   << ", is not valid; it must be between 1 and 12.\n";
      if ((day < 1) || (day > 31))
         errstream <<    "   The specified day of month, " << day << ", is not valid.\n";
      else
      {
         if (month == 2)
         {
            if (day > 29)
               errstream << "   The specified day of month, " << day << ", is not "
                  "valid for month " << month << ".\n";
            else if (day == 29)
            {
               if (year % 4 != 0)
                  errstream << "   The specified day of month, " << day 
                            << ", is not valid for month " << month 
                            << "in the year " << year << ".\n";
            }
         }
         if ((month == 4) || (month == 6) || (month == 9) || (month == 11))
            if (day > 30)
               errstream << "   The specified day of month, " << day 
                         << ", is not valid for month " << month << ".\n";
      }

      if ((hour < 0) || (hour > 24))
         errstream << "   The specified hour of day, " << hour 
                     << ", is not valid[ it must be between 0 and 24.\n";
      else
         if (((minute > 0) || (second > 0.0)) && (hour == 24))
            errstream << "   The specified hour of day, " << hour 
                        << ", is not valid with non-zero minutes "
                           "or seconds.\n";

      if ((minute < 0) || (minute > 60))
         errstream << "   The specified number of minutes, " << minute 
                     << ", is not valid; it must be between 0 and 60.\n";
      else
         if ((minute == 60) && (second > 0.0))
            errstream << "   The specified number of minutes, " << minute 
                        << ", is not valid with non-zero seconds.\n";

      if ((second < 0.0) || (second > 60.5))
         errstream << "   The specified number of seconds, " << second 
                   << ", is not valid; it must be between 0 and 60.5\n";

      if (errstream.str().length() > 0)
         throw InterfaceException("Error parsing the epoch data from the data file " + filename +
         ":\n" + errstream.str());

      utcEpoch = ModifiedJulianDate(year,month,day,hour,minute,second);
      realData[theField] = utcEpoch;
      dataLoaded[theField] = true;

      #ifdef DEBUG_FILEREAD
         MessageInterface::ShowMessage("   %s is at [%d %d %d %d %d %lf] = "
               "%17.12lf\n", theField.c_str(), year, month, day, hour,
               minute, second, utcEpoch);
      #endif
   }
}
