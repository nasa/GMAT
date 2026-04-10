//------------------------------------------------------------------------------
//                                  CCSDSAEMReader
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
// Created: 2013.11.19
//
/**
 * Reads a CCSDS Attitude Ephemeris Message file, and manages segments.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "StringUtil.hpp"
#include "CCSDSAEMReader.hpp"
#include "CCSDSAEMQuaternionSegment.hpp"
#include "CCSDSAEMEulerAngleSegment.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_AEM_READER_GET_STATE
//#define DEBUG_INIT_AEM_FILE
//#define DEBUG_PARSE_AEM_FILE
//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string CCSDSAEMReader::META_START = "META_START";
const std::string CCSDSAEMReader::META_STOP = "META_STOP";
const std::string CCSDSAEMReader::DATA_START = "DATA_START";
const std::string CCSDSAEMReader::DATA_STOP  = "DATA_STOP";

// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// default constructor
// -----------------------------------------------------------------------------
CCSDSAEMReader::CCSDSAEMReader() :
   CCSDSEMReader()
{
   versionFieldName  = "CCSDS_AEM_VERS";
   metaSpecifiesType = true;
   metaDataTypeField = "ATTITUDE_TYPE";
}

// -----------------------------------------------------------------------------
// copy constructor
// -----------------------------------------------------------------------------
CCSDSAEMReader::CCSDSAEMReader(const CCSDSAEMReader &copy) :
   CCSDSEMReader(copy)
{
}

// -----------------------------------------------------------------------------
// operator=
// -----------------------------------------------------------------------------
CCSDSAEMReader& CCSDSAEMReader::operator=(const CCSDSAEMReader &copy)
{
   if (&copy == this)
      return *this;

   CCSDSEMReader::operator=(copy);

   return *this;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
CCSDSAEMReader::~CCSDSAEMReader()
{
}

// -----------------------------------------------------------------------------
// CCSDSEMReader* Clone()
// -----------------------------------------------------------------------------
/**
 * This method returns a clone of the CCSDSAEMReader.
 *
 * @return clone of the CCSDSAEMReader.
 */
//------------------------------------------------------------------------------
CCSDSEMReader* CCSDSAEMReader::Clone() const
{
   return (new CCSDSAEMReader(*this));
}

// -----------------------------------------------------------------------------
// void Initialize()
// -----------------------------------------------------------------------------
void CCSDSAEMReader::Initialize()
{
   CCSDSEMReader::Initialize();
   // nothing else to do here at this time
}

// -----------------------------------------------------------------------------
// Rmatrix33 GetState(Real atEpoch)
// -----------------------------------------------------------------------------
Rmatrix33 CCSDSAEMReader::GetState(Real atEpoch)
{
   #ifdef DEBUG_AEM_READER_GET_STATE
      MessageInterface::ShowMessage(
            "Entering AEMReader::GetState, with atEpoch = %lf\n",
            atEpoch);
      MessageInterface::ShowMessage("numSegments = %d\n", numSegments);
      for (Integer ii = 0; ii < numSegments; ii++)
         MessageInterface::ShowMessage("   segment[%d] is at <%p>\n",
               ii, segments.at(ii));
   #endif
   CCSDSEMSegment *theSegment = GetSegment(atEpoch);
   if (!theSegment)
   {
      #ifdef DEBUG_AEM_READER_GET_STATE
         MessageInterface::ShowMessage(
               "In AEMReader::GetState, segment not found!\n");
      #endif
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "Time ";
      errmsg << atEpoch;
      errmsg << " is not contained in any segment present in file " << emFile << ".\n";
      throw UtilityException(errmsg.str());
   }
   #ifdef DEBUG_AEM_READER_GET_STATE
      MessageInterface::ShowMessage(
            "Entering AEMReader::GetState, segmentToUse = %p\n",
            theSegment);
   #endif
   CCSDSAEMSegment *theAEMSegment = (CCSDSAEMSegment*) theSegment;
   #ifdef DEBUG_AEM_READER_GET_STATE
      if (theAEMSegment == NULL) MessageInterface::ShowMessage("the segment to use is NULL!!!!\n");
   #endif
   return theAEMSegment->GetState(atEpoch);
}


// -----------------------------------------------------------------------------
// protected methods
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CCSDSEMSegment* CreateNewSegment(const std::string version,
//                                  const Integer segNum,
//                                  const std::string &ofType = "ANY")
// -----------------------------------------------------------------------------
CCSDSEMSegment* CCSDSAEMReader::CreateNewSegment(const std::string version,
                                                 const Integer segNum,
                                                 const std::string &ofType)
{
   if (ofType == "QUATERNION")
   {
      return new CCSDSAEMQuaternionSegment(version, segNum);
   }
   else if (ofType == "EULER_ANGLE")
   {
      return new CCSDSAEMEulerAngleSegment(version, segNum);
   }
   else
   {
      std::string errmsg = "Error reading ephemeris message file \"";
      errmsg += emFile + "\".  ";
      errmsg += "Attitude type \"" + ofType;
      errmsg += "\" is invalid or not supported.\n";
      throw UtilityException(errmsg);
   }
   return NULL;  // for testing
}

// -----------------------------------------------------------------------------
// bool IsValidVersion(const std::string &versionValue) const
// -----------------------------------------------------------------------------
bool CCSDSAEMReader::IsValidVersion(const std::string &versionValue) const
{
   // version 1.0 is allowed right now for CCSDS-AEM
   if (versionValue == "1.0")
      return true;

   return false;
}


bool CCSDSAEMReader::ParseFile()
{
#ifdef DEBUG_INIT_AEM_FILE
   MessageInterface::ShowMessage("Entering CCSDSAEMReader::ParseFile and isInitialized = %s...\n",
      (isInitialized ? "true" : "false"));
#endif
   if (isInitialized) return true;

#ifdef DEBUG_INIT_AEM_FILE
   MessageInterface::ShowMessage("In CCSDSAEMReader, about to open %s for reading\n",
      emFile.c_str());
#endif

   // Open the file for reading
   std::string   line;
   ephFile.open(emFile.c_str(), std::ios_base::in);
   if (!ephFile.is_open())
   {
#ifdef DEBUG_INIT_AEM_FILE
      MessageInterface::ShowMessage("In CCSDSAEMReader::ParseFile, file cannot be opened!\n");
#endif
      return false;
   }
#ifdef DEBUG_INIT_AEM_FILE
   MessageInterface::ShowMessage("In CCSDSAEMReader, about to ski white space\n");
#endif

   // Ignore leading white space
   ephFile.setf(std::ios::skipws);
   // check for an empty file
   if (ephFile.eof())
   {
#ifdef DEBUG_INIT_AEM_FILE
      MessageInterface::ShowMessage("In CCSDSAEMReader::ParseFile, file appears to be empty!\n");
#endif
      ephFile.close();
      std::string errmsg = "Error reading ephemeris message file \"";
      errmsg += emFile + "\".  ";
      errmsg += "File appears to be empty.\n";
      throw UtilityException(errmsg);
   }

   // Read the header data first - version number must be
   // on the first non-blank line
   std::string   firstWord, firstAllCaps;
   std::string   eqSign;

#ifdef DEBUG_PARSE_AEM_FILE
   MessageInterface::ShowMessage("In CCSDSAEMReader, about to read the first line\n");
#endif
   // read the first line for the version number
   getline(ephFile, line);
#ifdef DEBUG_PARSE_AEM_FILE
   MessageInterface::ShowMessage("In CCSDSAEMReader, line= %s\n", line.c_str());

#endif
   std::istringstream lineStr;
   lineStr.str(line);
   lineStr >> firstWord;
   firstAllCaps = GmatStringUtil::ToUpper(firstWord);
   if (firstAllCaps == versionFieldName)
   {
      lineStr >> eqSign;
      if (eqSign != "=")
      {
         std::string errmsg = "Error reading ephemeris message file \"";
         errmsg += emFile + "\".  ";
         errmsg += "Equal sign missing or incorrect.\n";
         throw UtilityException(errmsg);
      }
      std::string versionValue;
      lineStr >> versionValue;

      // This code is omitted due to version value was verified by IsValidVersion() function
      //if (!GmatStringUtil::IsNumber(versionValue))
      //{
      //   std::string errmsg = "Error reading ephemeris message file \"";
      //   errmsg += emFile + "\".  "; 
      //   errmsg += "Version number is not a valid real number.\n";
      //   throw UtilityException(errmsg); 
      //}
      if (!IsValidVersion(versionValue))
      {
         std::string errmsg = "Error reading ephemeris message file \"";
         errmsg += emFile + "\".  ";
         errmsg += "Version number \"" + versionValue + "\" is not valid.\n";
         throw UtilityException(errmsg);
      }
      versionNumber = versionValue;
      versionFound = true;
   }
   else
   {
      std::string errmsg = "Error reading ephemeris message file \"";
      errmsg += emFile + "\".  Field \"" + versionFieldName;
      errmsg += "\", specifying version number, must appear in first non-blank line.\n";
      throw UtilityException(errmsg);
   }

   // Read the rest of the header
   std::string   keyWord, keyAllCaps, sVal, sVal2, headerComment, metaComment, dataComment;
   bool          readingMeta = false;
   bool          readingData = false;
   std::string   lastRead = "none";
   Real          epochVal = 0.0;

   // 1. Read header
   while (!readingMeta && (!ephFile.eof()))
   {
      getline(ephFile, line);
      // ignore blank lines
      if (GmatStringUtil::IsBlank(line, true))  continue;


#ifdef DEBUG_PARSE_AEM_FILE
      MessageInterface::ShowMessage("In CCSDSAEMReader, line= %s\n", line.c_str());
#endif
      std::istringstream lineStr;
      lineStr.str(line);
      lineStr >> keyWord;
      keyAllCaps = GmatStringUtil::ToUpper(keyWord);

      if (keyAllCaps == META_START)
      {
         readingMeta = true;
         readingData = false;
         nonCommentFound = false;
      }
      else if (keyAllCaps == "COMMENT")
      {
         if (nonCommentFound)
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\".  ";
            //errmsg += "Header comment lines must appear directly after ";
            //errmsg += "version number.\n";
            errmsg += "Comment lines must not appear within any block of ";
            errmsg += "ephemeris lines or covariance matrix lines.\n";
            throw UtilityException(errmsg);
         }
         // get the rest of the line for the comments value
         getline(lineStr, headerComment);
         comments.push_back(headerComment);
#ifdef DEBUG_PARSE_AEM_FILE
         MessageInterface::ShowMessage("In CCSDSAEMReader, Header Comment added: \"%s\"\n",
            headerComment.c_str());
#endif
      }
      else
      {
         lineStr >> eqSign;
         if (eqSign != "=")
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\".  ";
            errmsg += "Equal sign missing or incorrect.\n";
            throw UtilityException(errmsg);
         }

         if (keyAllCaps == "CREATION_DATE")
         {
            // get the rest of the line for the creationDate value
            getline(lineStr, sVal);
            std::string sVal2 = GmatStringUtil::Trim(sVal, GmatStringUtil::BOTH, true, true);
            try
            {
               CCSDSEMSegment::ParseEpoch(sVal2);
            }
            catch (UtilityException &ue)
            {
               std::string errmsg = "Error reading ephemeris message file \"";
               errmsg += emFile + "\".  ";
               errmsg += "CREATION_DATE is invalid.\n";
               throw UtilityException(errmsg);
            }
            creationDate = sVal2;
            nonCommentFound = true;
         }
         else if (keyAllCaps == "ORIGINATOR")
         {
            getline(lineStr, sVal);
            std::string sVal2 = GmatStringUtil::Trim(sVal, GmatStringUtil::BOTH, true, true);
            originator = sVal2;
            nonCommentFound = true;
         }
         else
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\".  ";
            errmsg += "Field " + keyWord;
            errmsg += " is not allowed in the header.\n";
            throw UtilityException(errmsg);
         }
      }
   }

   // 2. Read meta data and ephemeris data
   if (!readingMeta)
   {
      std::string errmsg = "Error reading ephemeris message file \"";
      errmsg += emFile + "\".  ";
      errmsg += "\"META_START\" not found.\n";
      throw UtilityException(errmsg);
   }
   // Reset the non-comment flag
   nonCommentFound = false;

   bool typeFound = false;
   bool getRestOfLine = true;
   if (!metaSpecifiesType)
   {
      typeFound = true;
      dataType = "ANY";
   }

   // 2.1. Now read meta data and ephemeris data
   // At this point, we've found the first META_START line
   metaComment = "";
   dataComment = "";
   while (!ephFile.eof())
   {
      getline(ephFile, line);
      // ignore blank lines
      if (GmatStringUtil::IsBlank(line, true))  continue;

#ifdef DEBUG_PARSE_AEM_FILE
      MessageInterface::ShowMessage("In CCSDSAEMReader, line= %s\n", line.c_str());
#endif
      std::istringstream lineStr;
      lineStr.str(line);
      lineStr >> keyWord;
      keyAllCaps = GmatStringUtil::ToUpper(keyWord);

      if (readingMeta)
      {
         // Read meta data block
         if (keyAllCaps == META_STOP)
         {
            // Complete meta data block
            readingData = false;         // Reading data is only started after DATA_START. But after META_STOP is not started yet.
            readingMeta = false;         // Reading meta data is stop now
            lastRead = "meta";
            nonCommentFound = false;
#ifdef DEBUG_PARSE_AEM_FILE
            MessageInterface::ShowMessage("In CCSDSAEMReader, META_STOP detected\n");
#endif
            if (!typeFound)
            {
               std::string errmsg = "Error reading ephemeris message file \"";
               errmsg += emFile + "\".  Meta data must contain a value for ";
               errmsg += "field \"" + metaDataTypeField + "\"\n.";
               throw UtilityException(errmsg);
            }

            // Create a new segment of the appropriate type
            //currentSegment = CreateNewSegment(versionNumber, ++numSegments, dataType);
            currentSegment = CreateNewSegment(versionNumber, numSegments, dataType);
            ++numSegments;

            dataSize = currentSegment->GetDataSize();
            segments.push_back(currentSegment);
            #ifdef DEBUG_PARSE_AEM_FILE
               MessageInterface::ShowMessage(
                     "In CCSDSEMReader, new segment <%p> created, numSegments = %d\n",
                     currentSegment, numSegments);
            #endif
            
            // send all meta data to the new segment
            bool setOK = true;
            std::map<std::string, std::string>::iterator pos;
            for (pos = metaMap.begin(); pos != metaMap.end(); ++pos)
            {
               setOK = currentSegment->SetMetaData(pos->first, pos->second);
               if (!setOK)
               {
                  std::string errmsg = "Error reading ephemeris message file \"";
                  errmsg += emFile + "\"  ";
                  errmsg += "Invalid keyword = value pair (";
                  errmsg += pos->first + "," + pos->second;
                  errmsg += ").\n";
                  throw UtilityException(errmsg);
               }
            }
            currentSegment->Validate(false);
            metaMap.clear();

            // comment in meta data block is added here
            currentSegment->AddMetaComment(metaComment);
            metaComment = "";
         }
         else if ((keyAllCaps == META_START) || (keyAllCaps == DATA_START) || (keyAllCaps == DATA_STOP))
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\"  ";
            errmsg += "Missing META_STOP.\n";
            throw UtilityException(errmsg);
         }
         else
         {
            // Read all meta data here
            // Read command "Key = Value" or "COMMENT ..."
            getRestOfLine = true;
            if (keyAllCaps == "COMMENT")
            {
               if (nonCommentFound)
               {
                  std::string errmsg = "Error reading ephemeris message file \"";
                  errmsg += emFile + "\".  ";
                  //errmsg += "Meta data comment lines must appear directly after ";
                  //errmsg += "META_START.\n";
                  errmsg += "Comment lines must not appear within any block of ";
                  errmsg += "ephemeris lines or covariance matrix lines.\n";
                  throw UtilityException(errmsg);
               }
               
               getline(lineStr, metaComment);
               // Meta data block's comment will be added when META_STOP is read
               //currentSegment->AddMetaComment(metaComment);
            }
            else
            {
               lineStr >> eqSign;
               if (eqSign != "=")
               {
                  std::string errmsg = "Error reading ephemeris message file \"";
                  errmsg += emFile + "\".  ";
                  errmsg += "Equal sign missing or incorrect.\n";
                  throw UtilityException(errmsg);
               }

               if (metaSpecifiesType && (keyAllCaps == metaDataTypeField))
               {
                  lineStr >> sVal;
                  dataType = GmatStringUtil::ToUpper(sVal);
                  #ifdef DEBUG_PARSE_AEM_FILE
                  MessageInterface::ShowMessage("In CCSDSAEMReader, FOUND dataType = %s\n",
                     dataType.c_str());
                  #endif
                  typeFound = true;
                  getRestOfLine = false;  // we already put the value into sVal!!
               }
               nonCommentFound = true;
            }
            // get the rest of the line for the comments or string value
            #ifdef DEBUG_PARSE_AEM_FILE
            MessageInterface::ShowMessage("In CCSDSAEMReader, getting rest of line (%s) '%s'\n",
               (getRestOfLine ? "true" : "false"), lineStr.str().c_str());
            #endif
            if (getRestOfLine) getline(lineStr, sVal);

            // put the string into the proper form
            sVal2 = GmatStringUtil::Trim(sVal, GmatStringUtil::BOTH, true, true);
            #ifdef DEBUG_PARSE_AEM_FILE
            MessageInterface::ShowMessage("In CCSDSAEMReader, trimmed result '%s' to '%s'\n",
               sVal.c_str(), sVal2.c_str());
            #endif
            metaMap.insert(std::make_pair(keyAllCaps, sVal2));
         }
      }
      else if (readingData)
      {
         // Read ephemeris data section
         if ((keyAllCaps == DATA_START)|| (keyAllCaps == META_START)|| (keyAllCaps == META_STOP))
         {
            // Generate error message when incorrect syntax occurs inside ephemeris data block 
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\"  ";
            errmsg += "missing DATA_STOP.\n";
            throw UtilityException(errmsg);
         }
         else if (keyAllCaps == DATA_STOP)
         {
            // Complete an ephemeris data block
            readingData = false;
            readingMeta = false;
            lastRead = "data";
            nonCommentFound = false;
         }
         else if (keyAllCaps == "COMMENT")
         {
            // Read comment in ephemeris data block
            if (nonCommentFound)
            {
               std::string errmsg = "Error reading ephemeris message file \"";
               errmsg += emFile + "\".  ";
               //errmsg += "Ephemeris data comment lines must appear directly after ";
               //errmsg += "DATA_START.\n";
               errmsg += "Comment lines must not appear within any block of ";
               errmsg += "ephemeris lines or covariance matrix lines.\n";
               throw UtilityException(errmsg);
            }
            getline(lineStr, dataComment);
            currentSegment->AddDataComment(dataComment);
            dataComment = "";
         }
         else
         {
            // Read an ephemeris data record
            epochVal = CCSDSEMSegment::ParseEpoch(keyWord);

            Rvector dataVec(dataSize);
            std::string strDataVal;
            Real    dataVal;
            for (Integer ii = 0; ii < dataSize; ii++)
            {
               if (lineStr >> dataVal)
               {
                  dataVec[ii] = dataVal;
               }
               else
               {
                  std::string errmsg = "Error reading ephemeris message file \"";
                  errmsg += emFile + "\"  ";
                  errmsg += "Missing data.\n";
                  throw UtilityException(errmsg);
               }
            }

            currentSegment->AddData(epochVal, dataVec);
         }
      }
      else
      {
         // This is case for readingData = false and readingMeta = false
         if (keyAllCaps == META_STOP)
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\"  ";
            errmsg += "missing META_START.\n";
            throw UtilityException(errmsg);
         }
         else if (keyAllCaps == DATA_STOP)
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\"  ";
            errmsg += "missing DATA_START.\n";
            throw UtilityException(errmsg);
         }
         else if (keyAllCaps == META_START)
         {
            if (lastRead == "meta")
            {
               std::string errmsg = "Error reading ephemeris message file \"";
               errmsg += emFile + "\"  ";
               errmsg += "Two meta blocks are back to back.\n";
               throw UtilityException(errmsg);
            }
            else
            {
               // Repeat another segment
               readingData = false;
               readingMeta = true;
               nonCommentFound = false;

               //// Create a new segment of the appropriate type to store data
               //currentSegment = CreateNewSegment(++numSegments, dataType);
               //dataSize = currentSegment->GetDataSize();
               //segments.push_back(currentSegment);

               //#ifdef DEBUG_PARSE_AEM_FILE
               //MessageInterface::ShowMessage(
               //   "In CCSDSAEMReader, new segment <%p> created, numSegments = %d\n",
               //   currentSegment, numSegments);
               //#endif
            }
         }
         else if (keyAllCaps == DATA_START)
         {
            // Read all ephemeris data 
            if (lastRead == "data")
            {
               std::string errmsg = "Error reading ephemeris message file \"";
               errmsg += emFile + "\"  ";
               errmsg += "Two data blocks are back to back.\n";
               throw UtilityException(errmsg);
            }
            else
            {
               // Repeat another emphemeris data block
               readingData = true;
               readingMeta = false;
               nonCommentFound = false;
            }
         }
         else
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\"  ";
            errmsg += "missing META_START or DATA_START.\n";
            throw UtilityException(errmsg);
         }
      }
   }

   if (readingMeta)
   {
      std::string errmsg = "Error reading ephemeris message file \"";
      errmsg += emFile + "\".  META_STOP is missing ";
      errmsg += "from the file.\n";
      throw UtilityException(errmsg);
   }
   if (readingData)
   {
      std::string errmsg = "Error reading ephemeris message file \"";
      errmsg += emFile + "\".  DATA_STOP is missing ";
      errmsg += "from the file.\n";
      throw UtilityException(errmsg);
   }
   //if (lastRead != "data")
   //{
   //   std::string errmsg = "Error reading ephemeris message file \"";
   //   errmsg += emFile + "\".  Meta data may have been read, ";
   //   errmsg += "but file is missing corresponding data.\n";
   //   throw UtilityException(errmsg);
   //}

#ifdef DEBUG_INIT_AEM_FILE
   MessageInterface::ShowMessage("In CCSDSEMReader::ParseFile, closing the ephFile\n");
#endif
   if (ephFile.is_open())  ephFile.close();

   return true;
}

