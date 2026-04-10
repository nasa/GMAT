//$Id$
//------------------------------------------------------------------------------
//                           FileReader
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
// Created: May 2, 2013
/**
 * Implementation for file based data readers
 */
//------------------------------------------------------------------------------

#include "FileReader.hpp"
#include "StringUtil.hpp"
#include "FileUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PARSING


//------------------------------------------------------------------------------
// FileReader(const std::string& theTypeName, const std::string& theName) :
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param theTypeName Sctipted type for the reader
 * @param theName Name of the reader
 */
//------------------------------------------------------------------------------
FileReader::FileReader(const std::string& theTypeName, const std::string& theName) :
   DataReader           (theTypeName, theName),
   filename             ("Not set"),
   theStream            (NULL)
{
   objectTypeNames.push_back("FileReader");
}

//------------------------------------------------------------------------------
// ~FileReader()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
FileReader::~FileReader()
{
}

//------------------------------------------------------------------------------
// FileReader(const FileReader& fr)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fr The object copied to the new one
 */
//------------------------------------------------------------------------------
FileReader::FileReader(const FileReader& fr) :
   DataReader           (fr),
   filename             (fr.filename),
   theStream            (NULL)
{
}

//------------------------------------------------------------------------------
// FileReader& operator=(const FileReader& fr)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fr The object copied to this one
 *
 * @return This reader, set to look like fr
 */
//------------------------------------------------------------------------------
FileReader& FileReader::operator=(const FileReader& fr)
{
   if (this != &fr)
   {
      DataReader::operator=(fr);

      filename  = fr.filename;
      theStream = NULL;
      dataBuffer.clear();
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool SetStream(std::ifstream* aStream, const std::string &fname)
//------------------------------------------------------------------------------
/**
 * Sets up the stream used to read in the data
 *
 * @param aStream The stream for the data
 * @param fname The name of the file associated with the stream
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool FileReader::SetStream(std::ifstream* aStream, const std::string &fname)
{
   #ifdef DEBUG_SETUP
      MessageInterface::ShowMessage("Setting stream to <%p> and filename to "
            "\"%s\"\n", aStream, fname.c_str());
   #endif

   if (fname != "")
      filename = fname;
   theStream = aStream;
   return true;
}

//------------------------------------------------------------------------------
// bool ReadLine(std::string& theLine)
//------------------------------------------------------------------------------
/**
 * Reads in a line from the file.
 *
 * @param theLine The container for the test line when it is read.
 *
 * @return true on success, false if at EOF or another failure occurs
 */
//------------------------------------------------------------------------------
bool FileReader::ReadLine(std::string& theLine)
{
   if (theStream->eof())
      return false;
   return GmatFileUtil::GetLine(theStream, theLine);
}

//------------------------------------------------------------------------------
// bool FileReader::ParseRealValue(const Integer i, const std::string& theField)
//------------------------------------------------------------------------------
/**
 * Reads a Real value from the data buffer.
 *
 * @param i Index of the line that contains (or starts) the data
 * @param theField The field label for the data
 *
 * @return true if a valid element was found, false if not
 */
//------------------------------------------------------------------------------
bool FileReader::ParseRealValue(const Integer i, const std::string& theField)
{
   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("FileReader::ParseRealValue(%d, %s) "
            "entered\n", i, theField.c_str());
   #endif
   bool retval = false;

   std::string theLine = dataBuffer[i];

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Looking for the string \"%s\"\n",
            fileStringMap[theField].c_str());
   #endif

   size_t start = 0, end;
   if (theField != "")
      start = theLine.find(fileStringMap[theField]);

   if (start != std::string::npos)
   {
      start += fileStringMap[theField].length() + 1;

      while (theLine[start] == ' ')
         ++start;
      end = start;
      do
      {
         ++end;
      } while ((end < theLine.length()) && (theLine[end] != ' '));

      std::string theData = theLine.substr(start, end - start);

      #ifdef DEBUG_PARSING
         MessageInterface::ShowMessage("   Details: \"%s\" is the string built "
               "from [%d, %d] of \"%s\"\n", theData.c_str(), start, end,
               theLine.c_str());
      #endif

      // Change occurrences of "D" or "d" to "e" to account for FORTRAN doubles
      for (UnsignedInt i = 0; i < theData.length(); ++i)
         if ((theData[i] == 'd') || (theData[i] == 'D'))
            theData[i] = 'e';

      #ifdef DEBUG_PARSING
         MessageInterface::ShowMessage("The data for field %s is set to "
               "\"%s\"\n", theField.c_str(), theData.c_str());
      #endif

      Real value;
      Integer retcode;
      if (GmatStringUtil::IsValidReal(theData, value, retcode))
      {
         realData[theField] = value;
         dataLoaded[theField] = true;
         retval = true;
         #ifdef DEBUG_PARSING
            MessageInterface::ShowMessage("Current realData mapping:\n");
            for (std::map<std::string,Real>::iterator i = realData.begin();
                  i != realData.end(); ++i)
               MessageInterface::ShowMessage("   %s = %16.12lf\n",
                     i->first.c_str(), i->second);
         #endif
      }
      else
         MessageInterface::ShowMessage("*** Warning *** The field %s does not "
               "contain a valid real number value in the file %s\n", 
               theField.c_str(), filename.c_str());
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool ParseRvector6Value(const Integer i, const std::string& theField,
//                         const StringArray& fieldIdentifiers)
//------------------------------------------------------------------------------
/**
 * Reads a 6 dimensional Real vector from the data buffer.
 *
 * The Rvector parsing assumes that the data to be parsed from the file falls
 * in the line indicated by the index i or one of the next 6 lines.
 *
 * @param i Index of the line that contains (or starts) the data
 * @param theField The field label for the data
 * @param fieldIdentifiers Strings used to identify the 6 elements.  Use empty
 *                 strings if there are no text identifiers.  This array should
 *                 be 6 elements long.  Data past element [5] is ignored.  If
 *                 fewer that 6 elements are specified, the data in the
 *                 remainder of the vector is set to 0.0.
 *
 * @return true if at least one element of valid vector was found, false if not
 *
 * @note The empty string functionality is not yet implemented.  When/if we
 *       encounter a case that needs it, this method will need to be adapted to
 *       handle it.
 */
//------------------------------------------------------------------------------
bool FileReader::ParseRvector6Value(const Integer i,
      const std::string& theField, const StringArray& fieldIdentifiers)
{
   bool retval = false;

   Rvector6 theVector(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   Integer validFieldCount = 0;

   for (UnsignedInt j = 0; j < fieldIdentifiers.size(); ++j)
   {
      for (Integer k = 0; k < 7; ++k)
      {
         if (ParseRealValue(i+k, fieldIdentifiers[j]))
         {
            theVector[j] = realData[fieldIdentifiers[j]];
            realData.erase(fieldIdentifiers[j]);
            ++validFieldCount;
            break;
         }
      }
   }

   // Check that all components loaded
   if (validFieldCount == 6)
      retval = true;

   if (retval)
   {
      rvector6Data[theField] = theVector;
      dataLoaded[theField] = true;
   }

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Current rvector6Data mapping:\n");
      for (std::map<std::string,Rvector6>::iterator i = rvector6Data.begin();
            i != rvector6Data.end(); ++i)
      {
         MessageInterface::ShowMessage("   %s = [", i->first.c_str());
         for (UnsignedInt j = 0; j < 6; ++j)
         {
            MessageInterface::ShowMessage("%16.12lf", i->second[j]);
            if (j < 5)
               MessageInterface::ShowMessage(", ");
         }
         MessageInterface::ShowMessage("]\n");
      }
   #endif

   return retval;
}

//------------------------------------------------------------------------------
// bool FileReader::ParseStringValue(const Integer i, const std::string& theField)
//------------------------------------------------------------------------------
/**
 * Reads a data string from the data buffer.
 *
 * @param i Index of the line that contains (or starts) the data
 * @param theField The field label for the data
 *
 * @return true if a valid string was found, false if not
 */
//------------------------------------------------------------------------------
bool FileReader::ParseStringValue(const Integer i, const std::string& theField)
{
   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("FileReader::ParseStringValue(%d, %s) "
            "entered\n", i, theField.c_str());
   #endif
   bool retval = false;

   std::string theLine = dataBuffer[i];

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Looking for the string \"%s\"\n",
            fileStringMap[theField].c_str());
   #endif

   size_t start = 0, end;
   if (theField != "")
      start = theLine.find(fileStringMap[theField]);

   if (start != std::string::npos)
   {
      #ifdef DEBUG_PARSING
         MessageInterface::ShowMessage("String \"%s\" found at pos %d in line \"%s\"\n",
               fileStringMap[theField].c_str(), start, theLine.c_str());
      #endif

      start += fileStringMap[theField].length() + 1;

      while (theLine[start] == ' ')
         ++start;

      end = theLine.length() - 1;
      while (theLine[end] == ' ')
         --end;

      std::string theData = theLine.substr(start, end + 1 - start);

      #ifdef DEBUG_PARSING
         MessageInterface::ShowMessage("The data for field %s is set to "
               "\"%s\" from start = %d, end = %d\n", theField.c_str(),
               theData.c_str(), start, end);
      #endif
      
      // To be valid the string must contain some type of data
      if (theData.length() > 0)
      {
         stringData[theField] = theData;
         dataLoaded[theField] = true;
         retval = true;
      }
      else
         MessageInterface::ShowMessage("*** Warning *** The field %s does not "
               "contain a valid string value in the file %s\n", 
               theField.c_str(), filename.c_str());

      #ifdef DEBUG_PARSING
         MessageInterface::ShowMessage("Current stringData mapping:\n");
         for (std::map<std::string,std::string>::iterator i=stringData.begin();
               i != stringData.end(); ++i)
            MessageInterface::ShowMessage("   %s = %s\n",
                  i->first.c_str(), i->second.c_str());
      #endif
   }

   return retval;
}
