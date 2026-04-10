//------------------------------------------------------------------------------
//                                  CCSDSEMReader
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
// Created: 2013.11.22
//
/**
 * Reads a CCSDS Ephemeris Message file, and manages segments.
 * This is the base class from which other classes must derive (e.g.
 * a class to handle attitude files)
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "FileUtil.hpp"
#include "FileTypes.hpp"
#include "StringUtil.hpp"

#include "CCSDSEMReader.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PARSE_EM_FILE
//#define DEBUG_INIT_EM_FILE
//#define DEBUG_EM_FILE

// -----------------------------------------------------------------------------
// static data
// -----------------------------------------------------------------------------
//const std::string CCSDSEMReader::META_START = "META_START";
//const std::string CCSDSEMReader::META_STOP  = "META_STOP";
//const std::string CCSDSEMReader::DATA_START = "DATA_START";
//const std::string CCSDSEMReader::DATA_STOP = "DATA_STOP";
// Values of META_START, META_STOP, DATA_START, and DATA_STOP should be set to 
// different values for 2 different types of EM files due to their difference 
// of syntax.

// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// default constructor
// -----------------------------------------------------------------------------
CCSDSEMReader::CCSDSEMReader() :
   versionNumber         ("0.0"),
   originator            (""),
   creationDate          (""),
   emFile                (""),
   dataSize              (0),
   isInitialized         (false),
   versionFound          (false),
   nonCommentFound       (false),
   versionFieldName      ("Version"),
   metaSpecifiesType     (false),
   metaDataTypeField     ("ANY"),
   dataType              (""),
   currentSegment        (NULL),
   numSegments           (0)
{
   comments.clear();
   segments.clear();
   metaMap.clear();

   covarianceMetaMap.clear();
}

// -----------------------------------------------------------------------------
// copy constructor
// -----------------------------------------------------------------------------
CCSDSEMReader::CCSDSEMReader(const CCSDSEMReader &copy) :
   versionNumber         (copy.versionNumber),
   originator            (copy.originator),
   creationDate          (copy.creationDate),
   emFile                (copy.emFile),
   dataSize              (copy.dataSize),
   isInitialized         (false),
   versionFound          (false),
   nonCommentFound       (false),
   versionFieldName      (copy.versionFieldName),
   metaSpecifiesType     (copy.metaSpecifiesType),
   metaDataTypeField     (copy.metaDataTypeField),
   dataType              (copy.dataType),
   currentSegment        (NULL)
{
   segments.clear();
   numSegments = 0;

   comments.clear();
   metaMap.clear();
   covarianceMetaMap.clear();

   comments = copy.comments;
   metaMap  = copy.metaMap;
   covarianceMetaMap = copy.covarianceMetaMap;
}

// -----------------------------------------------------------------------------
// operator=
// -----------------------------------------------------------------------------
CCSDSEMReader& CCSDSEMReader::operator=(const CCSDSEMReader &copy)
{
   if (&copy == this)
      return *this;

   versionNumber           = copy.versionNumber;
   originator              = copy.originator;
   creationDate            = copy.creationDate;
   emFile                  = copy.emFile;
   dataSize                = copy.dataSize;
   isInitialized           = copy.isInitialized;
   versionFound            = copy.versionFound;
   nonCommentFound         = copy.nonCommentFound;
   versionFieldName        = copy.versionFieldName;
   metaSpecifiesType       = copy.metaSpecifiesType;
   metaDataTypeField       = copy.metaDataTypeField;
   dataType                = copy.dataType;
   currentSegment          = NULL;   // not sure if this is right
   numSegments             = copy.numSegments;

   for (unsigned int ii = 0; ii < segments.size(); ii++)
   {
      delete segments[ii];
   }
   segments.clear();
   // numSegments = 0;           // It is incorrect. Its value is defined in line: "numSegments  = copy.numSegments;" above 
   for (unsigned int ii = 0; ii < copy.segments.size(); ii++)
   {
      segments.push_back((copy.segments[ii])->Clone());
   }

   comments.clear();
   metaMap.clear();
   covarianceMetaMap.clear(); 

   comments = copy.comments;
   metaMap  = copy.metaMap;
   covarianceMetaMap = copy.covarianceMetaMap;

   return *this;

}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
CCSDSEMReader::~CCSDSEMReader()
{
   for (unsigned int ii = 0; ii < segments.size(); ii++)
   {
      delete segments[ii];
   }
   segments.clear();
   numSegments = 0;

   metaMap.clear();
   comments.clear();
   covarianceMetaMap.clear();

   #ifdef DEBUG_INIT_EM_FILE
      MessageInterface::ShowMessage("In CCSDSEMReader destructor, closing the ephFile\n");
   #endif
   if (ephFile.is_open())  ephFile.close();
}

// -----------------------------------------------------------------------------
// void Initialize()
// -----------------------------------------------------------------------------
void CCSDSEMReader::Initialize()
{
   if (isInitialized) return;

   #ifdef DEBUG_INIT_EM_FILE
      MessageInterface::ShowMessage("Entering CCSDSEMReader::Initialize, with emFile = %s\n",
            emFile.c_str());
   #endif
   // Check for the existence of the file
   if (!GmatFileUtil::DoesFileExist(emFile))
   {
      #ifdef DEBUG_INIT_EM_FILE
         MessageInterface::ShowMessage("In CCSDSEMReader::Initialize, file %s does NOT exist!\n",
               emFile.c_str());
      #endif
      std::string errmsg = "Specified ephemeris message file \" ";
      errmsg += emFile + "\" does not exist.\n";
      throw UtilityException(errmsg);
   }

   // Parse the file
   bool fileParsed = ParseFile();
   
   if (!fileParsed)
   {
      #ifdef DEBUG_INIT_EM_FILE
         MessageInterface::ShowMessage("In CCSDSEMReader::Initialize, ERROR parsing the file!\n");
      #endif
      std::string errmsg = "There is an error opening or reading the ";
      errmsg += "ephemeris message file \"" + emFile + "\"\n";
      throw UtilityException(errmsg);
   }
   #ifdef DEBUG_INIT_EM_FILE
      MessageInterface::ShowMessage("      in CCSDSEMReader::Initialize, file has been parsed.\n");
   #endif

   // Validate the header data first
   if (versionNumber == "0.0")
   {
      std::string errmsg = "Error reading ephemeris message file \"";
      errmsg += emFile + "\".  ";
      errmsg += "Version number is missing or invalid.\n";
      throw UtilityException(errmsg);
   }
   // These we ignore, so we won't write error messages for this, for now
//   if (originator == "")
//   {
//      std::string errmsg = "Error reading ephemeris message file \"";
//      errmsg += emFile + "\".  ";
//      errmsg += "ORIGINATOR is missing or invalid.\n";
//      throw UtilityException(errmsg);
//   }
//   if (creationDate == "")
//   {
//      std::string errmsg = "Error reading ephemeris message file \"";
//      errmsg += emFile + "\".  ";
//      errmsg += "CREATION_DATE is missing or invalid.\n";
//      throw UtilityException(errmsg);
//   }

   if (numSegments == 0)
   {
      std::string errmsg = "Error reading ephemeris message file \"";
      errmsg += emFile + "\".  ";
      errmsg += "No segments found.\n";
      throw UtilityException(errmsg);
   }
   #ifdef DEBUG_INIT_EM_FILE
      MessageInterface::ShowMessage("      in CCSDSEMReader::Initialize, about to validate segments.\n");
   #endif
   // Validate all of the segments
   bool segmentOK = true;
   for (Integer ii = 0; ii < numSegments; ii++)
   {
      #ifdef DEBUG_INIT_EM_FILE
         MessageInterface::ShowMessage(
               "      in CCSDSEMReader::Initialize, about to validate segment %d <%p>.\n",
               ii, segments.at(ii));
      #endif
      segmentOK = (segments.at(ii))->Validate();
      if (!segmentOK)
      {
         std::stringstream errmsg("");
         errmsg << "There is an error validating segment ";
         errmsg << ii << ", contained in file " << emFile << ".\n";
         throw UtilityException(errmsg.str());
      }
   }
   #ifdef DEBUG_INIT_EM_FILE
      MessageInterface::ShowMessage("      in CCSDSEMReader::Initialize, segments have been validated.\n");
   #endif
   // Validate that the segments' start and stop times are in the correct order
   Real currentStop  = (segments.at(0))->GetStopTime();
   Real segStart, segStop;
   for (Integer jj = 1; jj < numSegments; jj++)
   {
      segStart   = (segments.at(jj))->GetStartTime();
      segStop    = (segments.at(jj))->GetStopTime();
      #ifdef DEBUG_INIT_EM_FILE
         MessageInterface::ShowMessage("--- last segment stop time = %12.10f, segment start = %12.10f\n",
               currentStop, segStart);
      #endif
      if (segStart < currentStop)
      {
         std::string errmsg = "Error reading ephemeris message file \"";
         errmsg += emFile + "\".  ";
         errmsg += "Segment start and stop times are not properly ordered.\n";
         throw UtilityException(errmsg);
         currentStop = segStop;
      }
   }
   #ifdef DEBUG_INIT_EM_FILE
      MessageInterface::ShowMessage("      in CCSDSEMReader::Initialize, start and stop times have been checked.\n");
   #endif

   isInitialized = true;
}


bool CCSDSEMReader::IsInitialized()
{
   return isInitialized;
}


// -----------------------------------------------------------------------------
// bool SetFile(const std::string &theEMFile)
// -----------------------------------------------------------------------------
bool CCSDSEMReader::SetFile(const std::string &theEMFile)
{
   #ifdef DEBUG_EM_FILE
      MessageInterface::ShowMessage("In SetFile, current file = %s, new file = %s\n",
            emFile.c_str(), theEMFile.c_str());
      MessageInterface::ShowMessage("     and number of segments = %d\n",
            numSegments);
   #endif
   if (theEMFile != emFile)
   {
      emFile        = theEMFile;
      isInitialized = false;
      for (unsigned int ii = 0; ii < segments.size(); ii++)
      {
         delete segments[ii];
      }
      segments.clear();
      numSegments = 0;

      Initialize();
   }
   return true;
}


std::string CCSDSEMReader::GetVersion()
{
   if (IsValidVersion(versionNumber))
      return versionNumber;
   else
      throw UtilityException("Error: GMAT is not implemented the reader for version " + versionNumber + " of CCSDS ephemeris file.");

}


std::string CCSDSEMReader::GetOriginator()
{
   return originator;
}


std::string CCSDSEMReader::GetCreationDate()
{
   return creationDate;
}


Integer CCSDSEMReader::GetNumberOfSegments()
{
   return numSegments;
}


// -----------------------------------------------------------------------------
// protected methods
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CCSDSEMSegment* GetSegment()
// Returns the current segment
// -----------------------------------------------------------------------------
CCSDSEMSegment* CCSDSEMReader::GetSegment()
{
   return currentSegment;
}

// -----------------------------------------------------------------------------
// CCSDSEMSegment* GetSegment(Integer num)
// Returns the segment number requested
// -----------------------------------------------------------------------------
CCSDSEMSegment* CCSDSEMReader::GetSegment(Integer num)
{
   //if ((num < 0) || (num > numSegments))
   if ((num < 0) || (num >= numSegments))          // segment index has to be in range from 0 to numSegments-1 
   {
      throw UtilityException(
            "EphemerisMessage:: segment number requested is out-of-range.");
   }
   return segments.at(num);
}

// -----------------------------------------------------------------------------
// Integer GetSegmentNumber(Real epoch)
// Returns the segment number for the segment containing the
// requested time (using usable start/stop time if they exist,
// otherwise using start and stop time)
// -----------------------------------------------------------------------------
Integer CCSDSEMReader::GetSegmentNumber(Real epoch)
{
   for (Integer ii = 0; ii < numSegments; ii++)
   {
      if ((segments.at(ii))->CoversEpoch(epoch))  return ii;
   }
   return -1;
}

// -----------------------------------------------------------------------------
// CCSDSEMSegment* GetSegment(Real epoch)
// Returns the segment that contains the epoch specified
// (using usable start/stop time if they exist; otherwise,
// using start and stop time)
// -----------------------------------------------------------------------------
CCSDSEMSegment* CCSDSEMReader::GetSegment(Real epoch)
{
   for (Integer ii = 0; ii < numSegments; ii++)
   {
      if ((segments.at(ii))->CoversEpoch(epoch)) return segments.at(ii);
   }
   return NULL;
}

// -----------------------------------------------------------------------------
// bool ParseFile()
// Parse the file, validating where possible
// -----------------------------------------------------------------------------
std::string CCSDSEMReader::LeftTrim(std::string str)
{
   Integer i = 0;
   for (; i < str.size(); ++i)
   {
      char c = str.at(i);
      if ((c != ' ') && (c != '\t') && (c != '\r'))
         break;
   }

   if (i < str.size())
      return str.substr(i);
   else
      return "";
}

void CCSDSEMReader::ReadLine(std::ifstream& ephFile, std::string&line)
{
   std::string s;
   line = "";
   while (!ephFile.eof())
   {
      getline(ephFile, s);
      line = LeftTrim(s);
      if (line.size() != 0)
         break;
   }
}



// Different types of EM file have different syntax. Therefore, they have different way of parsing 
// data file. Therefore ParseFile() has to be defined as an abstract function here. This function 
// will be implemented in each child class with their own way to parse the data file.

//bool CCSDSEMReader::ParseFile()
//{
//   #ifdef DEBUG_INIT_EM_FILE
//      MessageInterface::ShowMessage("Entering CCSDSEMReader::ParseFile and isInitialized = %s...\n",
//            (isInitialized? "true" : "false"));
//   #endif
//   if (isInitialized) return true;
//
//   #ifdef DEBUG_INIT_EM_FILE
//      MessageInterface::ShowMessage("In CCSDSEMReader, about to open %s for reading\n",
//            emFile.c_str());
//   #endif
//
//   // Open the file for reading
//   std::string   line;
//   ephFile.open(emFile.c_str(), std::ios_base::in);
//   if (!ephFile.is_open())
//   {
//      #ifdef DEBUG_INIT_EM_FILE
//         MessageInterface::ShowMessage("In CCSDSEMReader::ParseFile, file cannot be opened!\n");
//      #endif
//      return false;
//   }
//   #ifdef DEBUG_INIT_EM_FILE
//      MessageInterface::ShowMessage("In CCSDSEMReader, about to ski white space\n");
//   #endif
//
//   // Ignore leading white space
//   ephFile.setf(std::ios::skipws);
//   // check for an empty file
//   if (ephFile.eof())
//   {
//      #ifdef DEBUG_INIT_EM_FILE
//         MessageInterface::ShowMessage("In CCSDSEMReader::ParseFile, file appears to be empty!\n");
//      #endif
//      ephFile.close();
//      std::string errmsg = "Error reading ephemeris message file \"";
//      errmsg += emFile + "\".  ";
//      errmsg += "File appears to be empty.\n";
//      throw UtilityException(errmsg);
//   }
//
//   // Read the header data first - version number must be
//   // on the first non-blank line
//   std::string   firstWord, firstAllCaps;
//   std::string   eqSign;
//
//   #ifdef DEBUG_PARSE_EM_FILE
//      MessageInterface::ShowMessage("In CCSDSEMReader, about to read the first line\n");
//   #endif
//   // read the first line for the version number
//   getline(ephFile, line);
//   #ifdef DEBUG_PARSE_EM_FILE
//      MessageInterface::ShowMessage("In CCSDSEMReader, line= %s\n", line.c_str());
//
//   #endif
//   std::istringstream lineStr;
//   lineStr.str(line);
//   lineStr >> firstWord;
//   firstAllCaps = GmatStringUtil::ToUpper(firstWord);
//   if (firstAllCaps == versionFieldName)
//   {
//      lineStr >> eqSign;
//      if (eqSign != "=")
//      {
//         std::string errmsg = "Error reading ephemeris message file \"";
//         errmsg += emFile + "\".  ";
//         errmsg += "Equal sign missing or incorrect.\n";
//         throw UtilityException(errmsg);
//      }
//      std::string versionValue;
//      lineStr >> versionValue;
//      if (!GmatStringUtil::IsNumber(versionValue))
//      {
//         std::string errmsg = "Error reading ephemeris message file \"";
//         errmsg += emFile + "\".  ";
//         errmsg += "Version number is not a valid real number.\n";
//         throw UtilityException(errmsg);
//      }
//      if (!IsValidVersion(versionValue))
//      {
//         std::string errmsg = "Error reading ephemeris message file \"";
//         errmsg += emFile + "\".  ";
//         errmsg += "Version number is not valid.\n";
//         throw UtilityException(errmsg);
//      }
//      versionNumber = versionValue;
//      versionFound = true;
//   }
//   else
//   {
//      std::string errmsg = "Error reading ephemeris message file \"";
//      errmsg += emFile + "\".  Field \"" + versionFieldName;
//      errmsg += "\", specifying version number, must appear in first non-blank line.\n";
//      throw UtilityException(errmsg);
//   }
//
//   // Read the rest of the header
//   std::string   keyWord, keyAllCaps, sVal, sVal2;
//   bool          readingMeta = false;
//   bool          readingData = false;
//   std::string   lastRead = "none";
//   Real          epochVal = 0.0;
//
//   // 1. Read header
//   while (!readingMeta && (!ephFile.eof()))
//   {
//      getline(ephFile, line);
//      // ignore blank lines
//      if (GmatStringUtil::IsBlank(line, true))  continue;
//      #ifdef DEBUG_PARSE_EM_FILE
//         MessageInterface::ShowMessage("In CCSDSEMReader, line= %s\n", line.c_str());
//      #endif
//      std::istringstream lineStr;
//      lineStr.str(line);
//      lineStr >> keyWord;
//      keyAllCaps = GmatStringUtil::ToUpper(keyWord);
//
//      if (keyAllCaps == META_START)
//      {
//         readingMeta = true;
//      }
//      else if (keyAllCaps == "COMMENT")
//      {
//         if (nonCommentFound)
//         {
//            std::string errmsg = "Error reading ephemeris message file \"";
//            errmsg += emFile + "\".  ";
//            errmsg += "Header comment lines must appear directly after ";
//            errmsg += "version number.\n";
//            throw UtilityException(errmsg);
//         }
//         // get the rest of the line for the comments value
//         getline(lineStr, sVal);
//         comments.push_back(sVal);
//         #ifdef DEBUG_PARSE_EM_FILE
//            MessageInterface::ShowMessage("In CCSDSEMReader, Header Comment added: \"%s\"\n",
//                  sVal.c_str());
//         #endif
//      }
//      else
//      {
//         lineStr >> eqSign;
//         if (eqSign != "=")
//         {
//            std::string errmsg = "Error reading ephemeris message file \"";
//            errmsg += emFile + "\".  ";
//            errmsg += "Equal sign missing or incorrect.\n";
//            throw UtilityException(errmsg);
//         }
//         if (keyAllCaps == "CREATION_DATE")
//         {
//            // get the rest of the line for the creationDate value
//            getline(lineStr, sVal);
//            std::string sVal2 = GmatStringUtil::Trim(sVal, GmatStringUtil::BOTH, true, true);
//            try
//            {
//               CCSDSEMSegment::ParseEpoch(sVal2);
//            }
//            catch (UtilityException &ue)
//            {
//               std::string errmsg = "Error reading ephemeris message file \"";
//               errmsg += emFile + "\".  ";
//               errmsg += "CREATION_DATE is invalid.\n";
//               throw UtilityException(errmsg);
//            }
//            creationDate = sVal2;
//            nonCommentFound = true;
//         }
//         else if (keyAllCaps == "ORIGINATOR")
//         {
//            getline(lineStr, sVal);
//            std::string sVal2 = GmatStringUtil::Trim(sVal, GmatStringUtil::BOTH, true, true);
//            originator = sVal2;
//            nonCommentFound = true;
//         }
//         else
//         {
//            std::string errmsg = "Error reading ephemeris message file \"";
//            errmsg += emFile + "\".  ";
//            errmsg += "Field " + keyWord;
//            errmsg += " is not allowed in the header.\n";
//            throw UtilityException(errmsg);
//         }
//      }
//   }
//
//   // 2. Read meta data
//   if (!readingMeta)
//   {
//      std::string errmsg = "Error reading ephemeris message file \"";
//      errmsg += emFile + "\".  ";
//      errmsg += "\"META_START\" not found.\n";
//      throw UtilityException(errmsg);
//   }
//   // Reset the non-comment flag
//   nonCommentFound = false;
//
//   bool typeFound = false;
//   bool getRestOfLine = true;
//   if (!metaSpecifiesType)
//   {
//      typeFound = true;
//      dataType = "ANY";
//   }
//
//   // 2.1. Now read meta data and ephemeris data
//   // At this point, we've found the first META_START line
//   while (!ephFile.eof())
//   {
//      getline(ephFile, line);
//      // ignore blank lines
//      if (GmatStringUtil::IsBlank(line, true))  continue;
//      #ifdef DEBUG_PARSE_EM_FILE
//         MessageInterface::ShowMessage("In CCSDSEMReader, line= %s\n", line.c_str());
//      #endif
//      std::istringstream lineStr;
//      lineStr.str(line);
//      lineStr >> keyWord;
//      keyAllCaps = GmatStringUtil::ToUpper(keyWord);
//
//      if (readingMeta)
//      {
//         if (keyAllCaps == META_STOP)
//         {
//            readingData = true;       // Reading data is started after META_STOP
//            readingMeta = false;
//            lastRead = "meta";
//            nonCommentFound = false;
//            #ifdef DEBUG_PARSE_EM_FILE
//               MessageInterface::ShowMessage("In CCSDSEMReader, META_STOP detected\n");
//            #endif
//            if (!typeFound)
//            {
//               std::string errmsg = "Error reading ephemeris message file \"";
//               errmsg += emFile + "\".  Meta data must contain a value for ";
//               errmsg += "field \"" + metaDataTypeField + "\"\n.";
//               throw UtilityException(errmsg);
//            }
//            // Create a new segment of the appropriate type
//            currentSegment = CreateNewSegment(++numSegments, dataType);
//            dataSize = currentSegment->GetDataSize();
//            segments.push_back(currentSegment);
//            #ifdef DEBUG_PARSE_EM_FILE
//               MessageInterface::ShowMessage(
//                     "In CCSDSEMReader, new segment <%p> created, numSegments = %d\n",
//                     currentSegment, numSegments);
//            #endif
//            // send all meta data to the new segment
//            bool setOK = true;
//            std::map<std::string, std::string>::iterator pos;
//            for (pos = metaMap.begin(); pos != metaMap.end(); ++pos)
//            {
//               setOK = currentSegment->SetMetaData(pos->first, pos->second);
//               if (!setOK)
//               {
//                  std::string errmsg = "Error reading ephemeris message file \"";
//                  errmsg += emFile + "\"  ";
//                  errmsg += "Invalid keyword = value pair (";
//                  errmsg += pos->first + "," + pos->second;
//                  errmsg += ").\n";
//                  throw UtilityException(errmsg);
//               }
//            }
//            currentSegment->Validate(false);
//            metaMap.clear();
//         }
//         else if (keyAllCaps == META_START)
//         {
//            std::string errmsg = "Error reading ephemeris message file \"";
//            errmsg += emFile + "\"  ";
//            errmsg += "Missing META_STOP.\n";
//            throw UtilityException(errmsg);
//         }
//         else
//         {
//            // Read command "Key = Value" or "COMMENT ..."
//            getRestOfLine = true;
//            if (keyAllCaps == "COMMENT")
//            {
//               if (nonCommentFound)
//               {
//                  std::string errmsg = "Error reading ephemeris message file \"";
//                  errmsg += emFile + "\".  ";
//                  errmsg += "Meta data comment lines must appear directly after ";
//                  errmsg += "META_START.\n";
//                  throw UtilityException(errmsg);
//               }
//               getline(lineStr, sVal);
//               currentSegment->AddMetaComment(sVal);
//            }
//            else
//            {
//               lineStr >> eqSign;
//               if (eqSign != "=")
//               {
//                  std::string errmsg = "Error reading ephemeris message file \"";
//                  errmsg += emFile + "\".  ";
//                  errmsg += "Equal sign missing or incorrect.\n";
//                  throw UtilityException(errmsg);
//               }
//               if (metaSpecifiesType && (keyAllCaps == metaDataTypeField))
//               {
//                  lineStr >> sVal;
//                  dataType = GmatStringUtil::ToUpper(sVal);
//                  #ifdef DEBUG_PARSE_EM_FILE
//                     MessageInterface::ShowMessage("In CCSDSEMReader, FOUND dataType = %s\n",
//                           dataType.c_str());
//                  #endif
//                  typeFound = true;
//                  getRestOfLine = false;  // we already put the value into sVal!!
//               }
//               nonCommentFound = true;
//            }
//            // get the rest of the line for the comments or string value
//            #ifdef DEBUG_PARSE_EM_FILE
//               MessageInterface::ShowMessage("In CCSDSEMReader, getting rest of line (%s) '%s'\n",
//                     (getRestOfLine? "true" : "false"), lineStr.str().c_str());
//            #endif
//            if (getRestOfLine) getline(lineStr, sVal);
//            // put the string into the proper form
//            sVal2 = GmatStringUtil::Trim(sVal, GmatStringUtil::BOTH, true, true);
//            #ifdef DEBUG_PARSE_EM_FILE
//               MessageInterface::ShowMessage("In CCSDSEMReader, trimmed result '%s' to '%s'\n",
//                     sVal.c_str(), sVal2.c_str());
//            #endif
//            metaMap.insert(std::make_pair(keyAllCaps, sVal2));
//         }
//      }
//      else if (readingData)
//      {
//         if (keyAllCaps == META_START)
//         {
//            readingData = false;
//            readingMeta = true;
//            lastRead = "data";
//            nonCommentFound = false;
//         }
//         else
//         {
//            if (keyAllCaps == "COMMENT")
//            {
//               if (nonCommentFound)
//               {
//                  std::string errmsg = "Error reading ephemeris message file \"";
//                  errmsg += emFile + "\".  ";
//                  errmsg += "Ephemeris data comment lines must appear directly after ";
//                  errmsg += "DATA_START.\n";
//                  throw UtilityException(errmsg);
//               }
//               getline(lineStr, sVal);
//               currentSegment->AddDataComment(sVal);
//            }
//            else
//            {
//               nonCommentFound = true;
//            }
//            
//            epochVal = CCSDSEMSegment::ParseEpoch(keyWord);
//            
//            Rvector dataVec(dataSize);
//            std::string strDataVal;
//            Real    dataVal;
//            for (Integer ii = 0; ii < dataSize; ii++)
//            {
//               if (lineStr >> dataVal)
//               {
//                  dataVec[ii] = dataVal;
//               }
//               else
//               {
//                  std::string errmsg = "Error reading ephemeris message file \"";
//                  errmsg += emFile + "\"  ";
//                  errmsg += "Missing data.\n";
//                  throw UtilityException(errmsg);
//               }
//            }
//
//            currentSegment->AddData(epochVal, dataVec);
//         }
//      }
//   }
//
//   if (readingMeta)
//   {
//      std::string errmsg = "Error reading ephemeris message file \"";
//      errmsg += emFile + "\".  META_STOP is missing ";
//      errmsg += "from the file.\n";
//      throw UtilityException(errmsg);
//   }
//   if ((readingData)&&(!nonCommentFound))
//   {
//      std::string errmsg = "Error reading ephemeris message file \"";
//      errmsg += emFile + "\".  DATA_STOP is missing ";
//      errmsg += "from the file.\n";
//      throw UtilityException(errmsg);
//   }
//   //if (lastRead != "data")
//   //{
//   //   std::string errmsg = "Error reading ephemeris message file \"";
//   //   errmsg += emFile + "\".  Meta data may have been read, ";
//   //   errmsg += "but file is missing corresponding data.\n";
//   //   throw UtilityException(errmsg);
//   //}
//
//   #ifdef DEBUG_INIT_EM_FILE
//      MessageInterface::ShowMessage("In CCSDSEMReader::ParseFile, closing the ephFile\n");
//   #endif
//   if (ephFile.is_open())  ephFile.close();
//
//   return true;
//}
//
