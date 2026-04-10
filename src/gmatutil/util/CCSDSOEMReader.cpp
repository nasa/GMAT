//------------------------------------------------------------------------------
//                                  CCSDSOEMReader
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
 * Reads a CCSDS Orbit Ephemeris Message file, and manages segments.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "StringUtil.hpp"
#include "CCSDSOEMReader.hpp"
#include "CCSDSOEMSegment.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"
#include "FileUtil.hpp"
#include "TimeSystemConverter.hpp"

//#define DEBUG_OEM_READER_GET_STATE
//#define DEBUG_INIT_OEM_FILE
//#define DEBUG_PARSE_OEM_FILE

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string CCSDSOEMReader::META_START = "META_START";
const std::string CCSDSOEMReader::META_STOP = "META_STOP";
const std::string CCSDSOEMReader::COVARIANCE_START = "COVARIANCE_START";
const std::string CCSDSOEMReader::COVARIANCE_STOP = "COVARIANCE_STOP";
// DATA_START and DATA_STOP are not used in OEM ephemeris data file

// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// default constructor
// -----------------------------------------------------------------------------
CCSDSOEMReader::CCSDSOEMReader() :
   CCSDSEMReader()
{
   versionFieldName  = "CCSDS_OEM_VERS";
   metaSpecifiesType = false;
   metaDataTypeField = ""; // "POSITION_TYPE";
}

// -----------------------------------------------------------------------------
// copy constructor
// -----------------------------------------------------------------------------
CCSDSOEMReader::CCSDSOEMReader(const CCSDSOEMReader &copy) :
   CCSDSEMReader(copy)
{
}

// -----------------------------------------------------------------------------
// operator=
// -----------------------------------------------------------------------------
CCSDSOEMReader& CCSDSOEMReader::operator=(const CCSDSOEMReader &copy)
{
   if (&copy == this)
      return *this;

   CCSDSEMReader::operator=(copy);

   return *this;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
CCSDSOEMReader::~CCSDSOEMReader()
{
}

// -----------------------------------------------------------------------------
// CCSDSOEMReader* Clone()
// -----------------------------------------------------------------------------
/**
 * This method returns a clone of the CCSDSOEMReader.
 *
 * @return clone of the CCSDSOEMReader.
 */
//------------------------------------------------------------------------------
CCSDSEMReader* CCSDSOEMReader::Clone() const
{
   return (new CCSDSOEMReader(*this));
}

// -----------------------------------------------------------------------------
// void Initialize()
// -----------------------------------------------------------------------------
void CCSDSOEMReader::Initialize()
{
   if (!IsInitialized())
      CCSDSEMReader::Initialize();
   // nothing else to do here at this time
}

// -----------------------------------------------------------------------------
// Rvector6 GetState(Real atEpoch)
// -----------------------------------------------------------------------------
Rvector6 CCSDSOEMReader::GetState(Real atEpoch)
{
   #ifdef DEBUG_OEM_READER_GET_STATE
      MessageInterface::ShowMessage(
            "Entering CCSDSOEMReader::GetState, with atEpoch = %lf\n",
            atEpoch);
      MessageInterface::ShowMessage("numSegments = %d\n", numSegments);
      for (Integer ii = 0; ii < numSegments; ii++)
         MessageInterface::ShowMessage("   segment[%d] is at <%p>\n",
               ii, segments.at(ii));
   #endif
   CCSDSEMSegment *theSegment = GetSegment(atEpoch);
   if (!theSegment)
   {
      #ifdef DEBUG_OEM_READER_GET_STATE
         MessageInterface::ShowMessage(
               "In CCSDSOEMReader::GetState, segment not found!\n");
      #endif
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "Time ";
      errmsg << atEpoch;
      errmsg << " is not contained in any segment present in file " << emFile << ".\n";
      throw UtilityException(errmsg.str());
   }
   #ifdef DEBUG_OEM_READER_GET_STATE
      MessageInterface::ShowMessage(
            "Entering CCSDSOEMReader::GetState, segmentToUse = %p\n",
            theSegment);
   #endif
   CCSDSOEMSegment *theOEMSegment = (CCSDSOEMSegment*) theSegment;
   #ifdef DEBUG_OEM_READER_GET_STATE
      if (theOEMSegment == NULL) MessageInterface::ShowMessage("the segment to use is NULL!!!!\n");
   #endif
   return theOEMSegment->GetState(atEpoch);
}

// -----------------------------------------------------------------------------
// protected methods
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CCSDSEMSegment* CreateNewSegment(const std::string version, 
//                                  const Integer segNum,
//                                  const std::string &ofType = "ANY")
// -----------------------------------------------------------------------------
CCSDSOEMSegment* CCSDSOEMReader::CreateNewSegment(const std::string version,
                                                  const Integer segNum,
                                                  const std::string &ofType)
{
   return new CCSDSOEMSegment(version, segNum);
}

// -----------------------------------------------------------------------------
// bool IsValidVersion(const std::string &versionValue) const
// -----------------------------------------------------------------------------
bool CCSDSOEMReader::IsValidVersion(const std::string &versionValue) const
{
   // Only version allowed right now are 1.0 and 2.0 
   if (versionValue == "1.0")
      return true;

   if (versionValue == "2.0")
   {
      //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)          // made changes by TUAN NGUYEN
      if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING)     // made changes by TUAN NGUYEN
         return true;
      else
      {
         throw UtilityException("Error: Usage of CCSDS OEM v2 ephemeris data file is only allowed for running in TESTING mode.\n");
         return false;
      }
   }

   return false;
}


bool CCSDSOEMReader::ParseFile()
{
#ifdef DEBUG_INIT_OEM_FILE
   MessageInterface::ShowMessage("Entering CCSDSOEMReader::ParseFile and isInitialized = %s...\n",
      (isInitialized ? "true" : "false"));
#endif
   if (isInitialized) return true;

#ifdef DEBUG_INIT_OEM_FILE
   MessageInterface::ShowMessage("In CCSDSOEMReader, about to open %s for reading\n",
      emFile.c_str());
#endif

   // Open the file for reading
   std::string   line;
   ephFile.open(emFile.c_str(), std::ios_base::in);
   if (ephFile)
   {
      if (!ephFile.is_open())
      {
#ifdef DEBUG_INIT_OEM_FILE
         MessageInterface::ShowMessage("In CCSDSOEMReader::ParseFile, file cannot be opened!\n");
#endif
         return false; 
      }
   }
   else
   {
      return false;
   }
#ifdef DEBUG_INIT_OEM_FILE
   MessageInterface::ShowMessage("In CCSDSOEMReader, about to ski white space\n");
#endif

   // Ignore leading white space
   ephFile.setf(std::ios::skipws);
   // check for an empty file
   if (ephFile.eof())
   {
#ifdef DEBUG_INIT_OEM_FILE
      MessageInterface::ShowMessage("In CCSDSOEMReader::ParseFile, file appears to be empty!\n");
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

#ifdef DEBUG_PARSE_OEM_FILE
   MessageInterface::ShowMessage("In CCSDSOEMReader, about to read the first line\n");
#endif
   // read the first line for the version number
   GmatFileUtil::GetLine(&ephFile, line);
#ifdef DEBUG_PARSE_OEM_FILE
   MessageInterface::ShowMessage("In CCSDSOEMReader, line= %s\n", line.c_str());

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

      // This verify of value is not needed. The version is verified by IsValidVersion() function
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
   std::string   keyWord, keyAllCaps, sVal, sVal2, metaComment, dataComment, covComment;
   bool          readingMeta = false;
   bool          readingData = false;
   bool          readingCovariance = false;
   bool          readingCovarianceMeta = false;
   bool          readingCovarianceData = false;

   std::string   lastRead = "none";
   Real          epochVal = 0.0;

   // 1. Read header
   while (!readingMeta && (!ephFile.eof()))
   {
	  GmatFileUtil::GetLine(&ephFile, line);
      // ignore blank lines
      if (GmatStringUtil::IsBlank(line, true))  continue;
#ifdef DEBUG_PARSE_OEM_FILE
      MessageInterface::ShowMessage("In CCSDSOEMReader, line= %s\n", line.c_str());
#endif
      std::istringstream lineStr;
      lineStr.str(line);
      lineStr >> keyWord;
      keyAllCaps = GmatStringUtil::ToUpper(keyWord);

      if (keyAllCaps == META_START)
      {
         readingMeta = true;
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
		 GmatFileUtil::GetLine(&lineStr, sVal);
         comments.push_back(sVal);
#ifdef DEBUG_PARSE_OEM_FILE
         MessageInterface::ShowMessage("In CCSDSOEMReader, Header Comment added: \"%s\"\n",
            sVal.c_str());
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
			GmatFileUtil::GetLine(&lineStr, sVal);
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
			GmatFileUtil::GetLine(&lineStr, sVal);
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

   // 2. Read meta data
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

   covComment  = "";
   Integer covRowIndex = 6;     // valid range from 0 to 6
   Integer idx = 0;
   bool skipReadingALine = false;
   
   while (!ephFile.eof())
   {
      #ifdef DEBUG_PARSE_OEM_FILE
      MessageInterface::ShowMessage("In CCSDSOEMReader, start a loop: skip a line = <%s>\n", (skipReadingALine?"true":"false"));
      #endif
      // Read a line from ephemeris file with option to skip a line is request
      // After skip a line was done, it removes the "skip a line" request.
      if (skipReadingALine == false)
      {
         GmatFileUtil::GetLine(&ephFile, line);
         // ignore blank lines
         if (GmatStringUtil::IsBlank(line, true))  continue;
      }
      else
         skipReadingALine = false;

      #ifdef DEBUG_PARSE_OEM_FILE
      MessageInterface::ShowMessage("In CCSDSOEMReader, line= %s\n", line.c_str());
      #endif

      std::istringstream lineStr;
      lineStr.str(line);
      lineStr >> keyWord;
      keyAllCaps = GmatStringUtil::ToUpper(keyWord);

      #ifdef DEBUG_PARSE_OEM_FILE
      MessageInterface::ShowMessage("In CCSDSOEMReader, keyWord= <%s>   lineStr = <%s>\n", keyWord.c_str(), lineStr.str().c_str());
      #endif

      if (readingMeta)
      {
         if (keyAllCaps == META_STOP)
         {
            readingData = true;       // Reading data is started after META_STOP
            readingMeta = false;
            lastRead = "meta";
            nonCommentFound = false;

            #ifdef DEBUG_PARSE_OEM_FILE
            MessageInterface::ShowMessage("In CCSDSOEMReader, META_STOP detected\n");
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
            #ifdef DEBUG_PARSE_OEM_FILE
            MessageInterface::ShowMessage(
               "In CCSDSOEMReader, new segment <%p> created, numSegments = %d\n",
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

            #ifdef DEBUG_PARSE_OEM_FILE
            MessageInterface::ShowMessage("In CCSDSOEMReader, META_STOP processing end\n");
            #endif
         }
         else if (keyAllCaps == META_START)
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\"  ";
            errmsg += "Missing META_STOP.\n";
            throw UtilityException(errmsg);
         }
         else if ((keyAllCaps == COVARIANCE_START) || (keyAllCaps == COVARIANCE_STOP))
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\". ";
            errmsg += keyAllCaps + " key word is not allowed to appear between META_START and META_STOP.\n";
            throw UtilityException(errmsg);
         }
         else
         {
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

			      GmatFileUtil::GetLine(&lineStr, metaComment);
               currentSegment->AddMetaComment(metaComment);
               metaComment = "";
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
                  #ifdef DEBUG_PARSE_OEM_FILE
                  MessageInterface::ShowMessage("In CCSDSOEMReader, FOUND dataType = %s\n",
                     dataType.c_str());
                  #endif
                  typeFound = true;
                  getRestOfLine = false;  // we already put the value into sVal!!
               }
               nonCommentFound = true;
            }
            
            // get the rest of the line for the comments or string value
            #ifdef DEBUG_PARSE_OEM_FILE
            MessageInterface::ShowMessage("In CCSDSOEMReader, getting rest of line (%s) '%s'\n",
               (getRestOfLine ? "true" : "false"), lineStr.str().c_str());
            #endif
            if (getRestOfLine) GmatFileUtil::GetLine(&lineStr, sVal);
            
            // put the string into the proper form
            sVal2 = GmatStringUtil::Trim(sVal, GmatStringUtil::BOTH, true, true);
            #ifdef DEBUG_PARSE_OEM_FILE
            MessageInterface::ShowMessage("In CCSDSOEMReader, trimmed result '%s' to '%s'\n",
               sVal.c_str(), sVal2.c_str());
            #endif
            metaMap.insert(std::make_pair(keyAllCaps, sVal2));
         }
      } // reading meta block
      else if (readingData)
      {
         if (keyAllCaps == META_STOP)
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\"  ";
            errmsg += "missing META_START.\n";
            throw UtilityException(errmsg);

         }
         else if (keyAllCaps == COVARIANCE_STOP)
         {
            std::string errmsg = "Error reading ephemeris message file \"";
            errmsg += emFile + "\"  ";
            errmsg += "missing COVARIANCE_START.\n";
            throw UtilityException(errmsg);
         }
         else if (keyAllCaps == META_START)
         {
            readingData = false;
            readingMeta = true;
            lastRead = "data";
            nonCommentFound = false;
         }
         else if (keyAllCaps == COVARIANCE_START)
         {
            readingData = false;
            readingCovariance  = true;
            readingCovarianceMeta = true;
            readingCovarianceData = false;
            lastRead = "data";
            nonCommentFound = false;
         }
         else if (keyAllCaps == "COMMENT")
         {
            if (nonCommentFound)
            {
               std::string errmsg = "Error reading ephemeris message file \"";
               errmsg += emFile + "\".  ";
               //errmsg += "Ephemeris data comment lines must appear directly after ";
               //errmsg += "META_STOP.\n";
               errmsg += "Comment lines must not appear within any block of ";
               errmsg += "ephemeris lines or covariance matrix lines.\n";
               throw UtilityException(errmsg);
            }
			   GmatFileUtil::GetLine(&lineStr, dataComment);
            currentSegment->AddDataComment(dataComment);
            dataComment = "";
         }
         else
         {
            nonCommentFound = true;
            epochVal = CCSDSEMSegment::ParseEpoch(keyWord);

            Rvector dataVec(dataSize);
            std::string strDataVal;
            Real    dataVal;

            for (Integer ii = 0; ii < dataSize; ii++)
            {
               if (lineStr >> dataVal)
               {
                  dataVec[ii] = dataVal;
                  //MessageInterface::ShowMessage("dataVec[%d] = %.15le\n", ii, dataVec[ii]);
               }
               else
               {
                  if (ii == 6)
                  {
                     // This is for CCSDS-OEM v2 with data block containing only position and velocity (without acceleartion)
                     dataSize = 6;
                     //dataVec.Resize(6);
                     //currentSegment->SetDataSize(dataSize);
                  }
                  else
                  {
                     // This is for CCSDS-OEM v2 with data block containing only position, velocicty, and acceleartion but it has less than 9 data
                     std::string errmsg = "Error reading ephemeris message file \"";
                     errmsg += emFile + "\"  ";
                     errmsg += "Missing data found at epoch: ";
                     errmsg += keyWord + "\n";
                     throw UtilityException(errmsg);
                  }
               }
            }

            try
            {
               if (dataSize != dataVec.GetSize())
               {
                  Rvector dataRec;
                  dataRec.SetSize(dataSize);
                  for (Integer i = 0; i < dataSize; ++i)
                     dataRec[i] = dataVec[i];
                  currentSegment->AddData(epochVal, dataRec);
               }
               else
                  currentSegment->AddData(epochVal, dataVec);
            }
            catch (UtilityException e)
            {
               std::string base_filename = emFile.substr(emFile.find_last_of("/\\") + 1);
               std::string errmsg = "ERROR reading data in ephemeris file: \"" + base_filename + "\"\n"  + e.GetFullMessage().substr(e.GetFullMessage().find_first_of(":") + 2) + "\n";
               throw UtilityException(errmsg);
            }
            /// @todo: add code to read acceleration here when needs
         }
      } // reading data block
      else if (readingCovariance)
      {
         if (readingCovarianceMeta)
         {
            // Read command "Key = Value" or "COMMENT ..."
            if (keyAllCaps == "COMMENT")
            {
               if (nonCommentFound)
               {
                  std::string errmsg = "Error reading ephemeris message file \"";
                  errmsg += emFile + "\".  ";
                  errmsg += "Meta data comment lines must appear directly after ";
                  errmsg += "COVARIANCE_START.\n";
                  throw UtilityException(errmsg);
               }

               // Get comment for the covariance record
               GmatFileUtil::GetLine(&lineStr, covComment);
            }
            else
            {
               if (!(lineStr >> eqSign))
                  eqSign = "notAnEqualSign";

               #ifdef DEBUG_PARSE_OEM_FILE
               MessageInterface::ShowMessage("eqSign = <%s>\n", eqSign.c_str());
               #endif

               if (eqSign == "=")
               {
                  // Processing <KeyWord> = <Value>:
                  nonCommentFound = true;

                  // Read a covariance meta
                  // get the rest of the line for the string value
                  #ifdef DEBUG_PARSE_OEM_FILE
                  MessageInterface::ShowMessage("In CCSDSOEMReader: Processing covariance meta section: <KeyWord> = <Value>: reading <Value>. Getting the rest of line (%s) '%s'\n",
                     (getRestOfLine ? "true" : "false"), lineStr.str().c_str());
                  #endif
                  GmatFileUtil::GetLine(&lineStr, sVal);

                  // put the string into the proper form
                  sVal2 = GmatStringUtil::Trim(sVal, GmatStringUtil::BOTH, true, true);
                  #ifdef DEBUG_PARSE_OEM_FILE
                  MessageInterface::ShowMessage("In CCSDSOEMReader: Processing covariance meta section: <KeyWord> = <Value>:  trimmed result '%s' to '%s'\n",
                     sVal.c_str(), sVal2.c_str());
                  #endif

                  covarianceMetaMap[keyAllCaps] = sVal2;

                  lastRead = "covMeta";
               }
               else
               {
                  // Setting flags for the next state. The next state is "reading covariance matrix"
                  readingCovarianceMeta = false;
                  readingCovarianceData = true;

                  // Start reading covariance matrix, it needs to set row index and element index to 0 and to reuse the current line
                  covRowIndex = 0;
                  idx = 0;
                  skipReadingALine = true;
               }
            }
         } // reading covariance meta section
         else if (readingCovarianceData)
         {
            if (keyAllCaps == COVARIANCE_STOP)
            {
               readingCovarianceData = false;
               readingCovariance = false;
               lastRead = "covData";
               nonCommentFound = false;

               ///@todo: add code to complete a segment
               /// Note that: a segment is ended by "End Of File" or by COVARIANCE_STOP key word 
            }
            else
            {
               ///@todo: Read lower-left covariance matrix
               Rvector covValue;
               covValue.SetSize(21);

               std::istringstream rowStream;
               rowStream.str(line);

               #ifdef DEBUG_PARSE_OEM_FILE
               MessageInterface::ShowMessage("  covRowIndex = %d\n", covRowIndex);
               MessageInterface::ShowMessage("  rowStream = <%s>\n", rowStream.str());
               #endif
               
               for (Integer col = 0; col <= covRowIndex; ++col)
               {
                  std::string strVal;
                  rowStream >> strVal;
                  Real val;
                  Integer errNum;
                  if (GmatStringUtil::IsValidReal(strVal, val, errNum))
                  {
                     covValue[idx] = val;
                     
                     #ifdef DEBUG_PARSE_OEM_FILE
                     MessageInterface::ShowMessage("  covValue[%d] = %.15le\n", idx, covValue[idx]);
                     #endif
                  }
                  else
                  {
                     std::string errmsg = "Error reading ephemeris message file \"";
                     errmsg += emFile + "\".  ";
                     errmsg += "A non real number: " + strVal + " is in the lower-left covariance matrix\n";
                     throw UtilityException(errmsg);
                  }
                  ++idx;
               }
               ++covRowIndex;
               lastRead = "covData";

               if (covRowIndex == 6)
               {
                  // Create a new covariance record and store its values.
                  //CCSDSEMSegment::CovarianceRecord *covarianceRec = new CCSDSEMSegment::CovarianceRecord;
                  CovarianceRecord *covarianceRec = new CovarianceRecord;

                  std::string frame = covarianceMetaMap["COV_REF_FRAME"];
                  std::string epochStr = covarianceMetaMap["EPOCH"];
                  Real ep = CCSDSEMSegment::ParseEpoch(epochStr);
                  covarianceRec->SetValue(ep, frame, covComment, covValue);
                  covComment = "";
                  currentSegment->AddCovarianceRecord(covarianceRec);

                  // Setting flags for the next state. The next state is "reading covariance meta"
                  readingCovarianceData = false;
                  readingCovarianceMeta = true;
                  lastRead = "covData";
                  covRowIndex = 0;
                  idx = 0;
               }
            }
         }  // reading covariance data
         else
         {
            if (keyAllCaps == META_START)
            {
               // Setting flags to move to meta block
               readingMeta = true;
               readingCovariance = false;
               lastRead = "covBlock";
               nonCommentFound = false;
            }
            else
            {
               readingCovariance = false;
               skipReadingALine = true;
               break;
            }
         }  // read COVARIANCE_STOP

      } // reading covariance block
   }

   // Make sure nothing after the end of a segment in ephemeris file
   while (!ephFile.eof())
   {
      if (!skipReadingALine)
      {
         GmatFileUtil::GetLine(&ephFile, line);
         // ignore blank lines
         if (GmatStringUtil::IsBlank(line, true))  continue;
      }
      else
         skipReadingALine = false;
      if (GmatStringUtil::Trim(line) != "")
      {
         std::string errmsg = "Error reading ephemeris message file \"";
         errmsg += emFile + "\". This line \"" + line + "\" is not allowed in the file.\n";
         throw UtilityException(errmsg);
      }
   }
   
   //if (readingMeta)
   //{
   //   std::string errmsg = "Error reading ephemeris message file \"";
   //   errmsg += emFile + "\".  META_STOP is missing ";
   //   errmsg += "from the file.\n";
   //   throw UtilityException(errmsg);
   //}
   //if ((readingData) && (!nonCommentFound))
   //{
   //   std::string errmsg = "Error reading ephemeris message file \"";
   //   errmsg += emFile + "\".  No ephemeris data is in data block ";
   //   errmsg += "from the file.\n";
   //   throw UtilityException(errmsg);
   //}
   //if (lastRead != "data")
   //{
   //   std::string errmsg = "Error reading ephemeris message file \"";
   //   errmsg += emFile + "\".  Meta data may have been read, ";
   //   errmsg += "but file is missing corresponding data.\n";
   //   throw UtilityException(errmsg);
   //}

#ifdef DEBUG_INIT_OEM_FILE
   MessageInterface::ShowMessage("In CCSDSOEMReader::ParseFile, closing the ephFile\n");
#endif
   if (ephFile.is_open())  ephFile.close();

   return true;
}

