//------------------------------------------------------------------------------
//                           ThrustHistoryFile
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * Code that models thrusts using a time ordered collection of points in a file.
 */
//------------------------------------------------------------------------------


#include "ThrustHistoryFile.hpp"
#include "ThfDataSegment.hpp"
#include "InterfaceException.hpp"
#include "TextParser.hpp"
#include "StringUtil.hpp"
#include <ctype.h>

// Classes/namespaces needed for conversions
#include "TimeSystemConverter.hpp"
#include "GmatConstants.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_FILE_DATA
//#define DEBUG_FILE_READ



const std::string ThrustHistoryFile::
PARAMETER_LABEL[ThrustHistoryFileParamCount - GmatBaseParamCount] =
{
   "FileName",                         // FILENAME
   "AddThrustSegment",                 // SEGMENTS,
};

const Gmat::ParameterType ThrustHistoryFile::
PARAMETER_TYPE[ThrustHistoryFileParamCount - GmatBaseParamCount] =
{
   Gmat::FILENAME_TYPE,
   Gmat::STRINGARRAY_TYPE,
};



//------------------------------------------------------------------------------
// ThrustHistoryFile::ThrustHistoryFile(const std::string& theName) :
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param theName Name of this thrust history file reader
 */
//------------------------------------------------------------------------------
ThrustHistoryFile::ThrustHistoryFile(const std::string& theName) :
   FileReader        ("ThrustHistoryFile", theName),
   thrustFileName    ("")
{
   objectTypes.push_back(Gmat::INTERFACE);
   objectTypeNames.push_back("Interface");
   objectTypeNames.push_back("ThrustHistoryFile");
   parameterCount = ThrustHistoryFileParamCount;

   // "BeginThrust and EndThrust are treated separately
   //"Start_Epoch" and "Thrust_Vector_Coordinate_System" must be 0 and 1. If not, check SetHeaderField();
   keywords.push_back("Start_Epoch");
   keywords.push_back("Thrust_Vector_Coordinate_System");
   keywords.push_back("Thrust_Vector_Interpolation_Method");
   keywords.push_back("Mass_Flow_Rate_Interpolation_Method");

   startEpochFound = false;
   coordSystemFound = false;

   dataStartKeys.push_back("ModelThrustOnly");
   dataStartKeys.push_back("ModelThrustAndMassRate");
   dataStartKeys.push_back("ModelAccelOnly");
   dataStartKeys.push_back("ModelAccelAndMassRate");

   interpolationTypes.push_back("None");
   interpolationTypes.push_back("Linear");
   interpolationTypes.push_back("CubicSpline");
   interpolationTypes.push_back("ThrustVectorMethod");

   theForce.SetName(instanceName + "_FileThrust");
}

//------------------------------------------------------------------------------
// ~ThrustHistoryFile()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThrustHistoryFile::~ThrustHistoryFile()
{
}

//------------------------------------------------------------------------------
// ThrustHistoryFile::ThrustHistoryFile(const ThrustHistoryFile& thf) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param thf The ThrustHistoryFile object used to make this one
 */
//------------------------------------------------------------------------------
ThrustHistoryFile::ThrustHistoryFile(const ThrustHistoryFile& thf) :
   FileReader           (thf),
   thrustFileName       (thf.thrustFileName),
   segments             (thf.segments),
   scriptSegments       (thf.scriptSegments),
   segmentNames         (thf.segmentNames),
   massSources          (thf.massSources),
   keywords             (thf.keywords),
   dataStartKeys        (thf.dataStartKeys),
   interpolationTypes   (thf.interpolationTypes),
   theForce             (thf.theForce),
   startEpochFound      (thf.startEpochFound),
   coordSystemFound     (thf.coordSystemFound)
{
}

//------------------------------------------------------------------------------
// ThrustHistoryFile& operator=(const ThrustHistoryFile& thf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param thf The ThrustHistoryFile object used to set up this one
 *
 * @return This ThrustHistoryFile, set to match thf
 */
//------------------------------------------------------------------------------
ThrustHistoryFile& ThrustHistoryFile::operator =(const ThrustHistoryFile& thf)
{
   if (this != &thf)
   {
      FileReader::operator =(thf);

      thrustFileName = thf.thrustFileName;
      segments = thf.segments;
      scriptSegments = thf.scriptSegments;
      segmentNames = thf.segmentNames;
      massSources = thf.massSources;
      keywords = thf.keywords;
      dataStartKeys = thf.dataStartKeys;
      interpolationTypes = thf.interpolationTypes;
      theForce = thf.theForce;
      startEpochFound = thf.startEpochFound;
      coordSystemFound = thf.coordSystemFound;
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new ThrustHistoryFile that looks like this one
 *
 * @return The new object
 */
//------------------------------------------------------------------------------
GmatBase* ThrustHistoryFile::Clone() const
{
   return new ThrustHistoryFile(*this);
}

//------------------------------------------------------------------------------
// bool ThrustHistoryFile::RenameRefObject(UnsignedInt type,
//       const std::string& oldname, const std::string& newname)
//------------------------------------------------------------------------------
/**
 * Resets the name for a referenced object
 *
 * @param type The type of the object being renamed
 * @param oldname The previous name of the reference
 * @param newname The new name for the object
 *
 * @return true if a name was changed
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::RenameRefObject(UnsignedInt type,
      const std::string& oldname, const std::string& newname)
{
   bool retval = false;

   for (UnsignedInt i = 0; i < segmentNames.size(); ++i)
   {
      if (segmentNames[i] == oldname)
      {
         segmentNames[i] = newname;
         retval = true;
      }
   }

   for (UnsignedInt i = 0; i < segments.size(); ++i)
   {
      if (segments[i].GetName() == oldname)
      {
         segments[i].SetName(newname);
         retval = true;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the script label for a parameter
 *
 * @param id The parameter's ID
 *
 * @return The script string
 */
//------------------------------------------------------------------------------
std::string ThrustHistoryFile::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ThrustHistoryFileParamCount)
      return PARAMETER_LABEL[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieved the parameter ID for a parameter label
 *
 * @param str The scripting used for the parameter
 *
 * @return The ID
 */
//------------------------------------------------------------------------------
Integer ThrustHistoryFile::GetParameterID(const std::string& str) const
{
   Integer id = -1;

   for (Integer i = GmatBaseParamCount; i < ThrustHistoryFileParamCount; i++)
    {
       if (str == PARAMETER_LABEL[i - GmatBaseParamCount])
       {
          id = i;
          break;
       }
    }

    if (id != -1)
    {
       #ifdef DEBUG_BURN_PARAM
       MessageInterface::ShowMessage("Burn::GetParameterID() returning %d\n", id);
       #endif

       return id;
    }

    return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type of a parameter
 *
 * @param id The ID of the parameter
 *
 * @return The type of the parameter
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ThrustHistoryFile::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ThrustHistoryFileParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing the type of a parameter
 *
 * @param id The ID of the parameter
 *
 * @return The type of the parameter
 */
//------------------------------------------------------------------------------
std::string ThrustHistoryFile::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or throw ab=n exception if
 *         there is no string association.
 */
//------------------------------------------------------------------------------
std::string ThrustHistoryFile::GetStringParameter(const Integer id) const
{
   if (id == FILENAME)
      return thrustFileName;

   return FileReader::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 *
 * @return true if the string is stored, throw if the parameter is not stored.
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == FILENAME)
   {
      // verify a valid file name
      Integer error;
      if (!GmatStringUtil::IsValidFullFileName(value, error))
         throw InterfaceException("Error: '" + value + "' set to " + GetName() + ".FileName is an invalid file name.\n");

      thrustFileName = value;
      return true;
   }

   if (id == SEGMENTS)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Block string %s\n", value.c_str());
      #endif

      // trim off braces:
      if (GmatStringUtil::IsEnclosedWithBraces(value))
      {
         std::string value1 = GmatStringUtil::RemoveEnclosingString(value, "{}");
         if (GmatStringUtil::Trim(value1) == "")
         {
            segmentNames.clear();               // empty list of solvefors.
            return true;
         }
      }

      // verify input value:
      if (GmatStringUtil::IsValidIdentity(value) == false)
      {
         throw InterfaceException("Error: The value \"" + value + "\" cannot be accepted for " + GetName() + ".AddThrustSegment ");
         return false;
      }

      if (find(segmentNames.begin(), segmentNames.end(), value) ==
            segmentNames.end())
         segmentNames.push_back(value);
      return true;
   }

   return FileReader::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string ThrustHistoryFile::GetStringParameter(const Integer id,
      const Integer index) const
{
   return FileReader::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const char *value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   return FileReader::SetStringParameter(id, value, index);
}

//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& ThrustHistoryFile::GetStringArrayParameter(
      const Integer id) const
{
   if (id == SEGMENTS)
      return segmentNames;

   return FileReader::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id,
//                                             const Integer index) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 * @param index The index when multiple StringArrays are supported.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& ThrustHistoryFile::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   return FileReader::GetStringArrayParameter(id, index);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string ThrustHistoryFile::GetStringParameter(
      const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label,
//                                 const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string ThrustHistoryFile::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& ThrustHistoryFile::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const std::string &label,
//                                             const Integer index) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param label The (string) label for the parameter.
 * @param index Which string array if more than one is supported.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& ThrustHistoryFile::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Returns flag indicating whether GetRefObjectTypeArray() is implemented.
 *
 * @return true if an array will be returned
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::HasRefObjectTypeArray()
{
   return true;
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Returns the types of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @return The types of the reference object.
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& ThrustHistoryFile::GetRefObjectTypeArray()
{
   refObjectTypes.clear();

   // Get ref. object types from the parent class
   FileReader::GetRefObjectTypeArray();

   // Add ref. object types from this class if not already added
   if (find(refObjectTypes.begin(), refObjectTypes.end(), Gmat::INTERFACE) ==
       refObjectTypes.end())
      refObjectTypes.push_back(Gmat::INTERFACE);

   return refObjectTypes;
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param type reference object type.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& ThrustHistoryFile::GetRefObjectNameArray(const UnsignedInt type)
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::INTERFACE)
   {
      FileReader::GetRefObjectNameArray(type);
      refObjectNames.insert(refObjectNames.begin(), segmentNames.begin(),
            segmentNames.end());
      return refObjectNames;
   }

   return FileReader::GetRefObjectNameArray(type);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                                   const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets the name of the reference object.
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                  const std::string &name)
{
   switch (type)
   {
   case Gmat::INTERFACE:
      {
         if (!obj->IsOfType("ThrustSegment"))
            throw InterfaceException("ThrustHistoryFile object segment "
                  "references must be ThrustSegment objects, but " +
                  obj->GetName() + "is not a ThrustSegment.");

         // Look through the Segments for the passed in object
         Integer index = -1;
         for (UnsignedInt i = 0; i < scriptSegments.size(); ++i)
         {
            if (scriptSegments[i]->GetName() == name)
               index = i;
         }
         if (index == -1)
         {
            scriptSegments.push_back((ThrustSegment*)obj);
         }
         else
         {
            scriptSegments[index] = (ThrustSegment*) obj;
         }
         return true;
      }

   default:
      return FileReader::SetRefObject(obj, type, name);
   }
}


//------------------------------------------------------------------------------
// bool ReadData()
//------------------------------------------------------------------------------
/**
 * Parser for the Thrust History File
 *
 * @return true if the file parsed correctly
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::ReadData()
{
   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage("Entered ThrustHistoryFile::ReadData()\n");
   #endif

   bool retval = false;
   TextParser parser;

   bool beginThrustFound = false;
   theStream = new std::ifstream();

   // Check for the file.  If it is not found on the raw file name, use the
   // file manager to build an alternative path and check there.
   std::string theFile = GmatFileUtil::FindFile(thrustFileName);
   if (theFile != "")
   {
      theStream->open(theFile.c_str());
   }

   if (theStream->is_open())
   {
      #ifdef DEBUG_FILE_READ
         MessageInterface::ShowMessage("File opened successfully\n");
      #endif

      // Keep track of segment names
      StringArray currSegmentNames;

      std::string theLine;
      char filedata[2048];

      #ifdef DEBUG_FILE_DATA
         Integer lineNo = 1;
      #endif

      while (!theStream->eof())
      {
         theStream->getline(filedata, 2048);
         theLine = filedata;
         /// Strip off trailing new line chars
         size_t nlChar = theLine.find("\n");
         if (nlChar != std::string::npos)
            theLine = theLine.substr(0, nlChar);
         nlChar = theLine.find("\r");
         if (nlChar != std::string::npos)
            theLine = theLine.substr(0, nlChar);

         #ifdef DEBUG_FILE_DATA
            MessageInterface::ShowMessage("%3d: %s\n", lineNo, theLine.c_str());
            ++lineNo;
         #endif

         // Each THF contains one or more blocks of data, called segments.
         // Segments start with a "BeginThrust" keyword, followed by the segment
         // name.
         if (theLine.find("BeginThrust") != std::string::npos)
         {
            
            beginThrustFound = true;
            startEpochFound = false;
            coordSystemFound = false;

            ThfDataSegment theSegment;
            theSegment.SetPrecisionTimeFlag(HasPrecisionTime());
            #ifdef DEBUG_FILE_DATA
               MessageInterface::ShowMessage("Reading Thrust History File "
                     "header information\n");
            #endif

            // Find the segment name
            StringArray chunks = parser.Decompose(theLine, "{}", false, true);
            if (chunks.size() == 2)
            {
               Integer start = chunks[1].find("{");
               Integer end = chunks[1].find("}");
               if ((start != std::string::npos) && (end != std::string::npos))
                  theSegment.segmentName = GmatStringUtil::Trim(
                        chunks[1].substr(start+1, (end - start - 1)));
            }

            #ifdef DEBUG_FILE_DATA
               MessageInterface::ShowMessage("   Segment name: \"%s\"\n",
                     theSegment.segmentName.c_str());
            #endif

            // Following the name come the fields that describe the profile
            bool readingHeader = true;
            bool dataKeywordFound = false;
            do
            {
               theStream->getline(filedata, 2048);
               theLine = filedata;
               /// Strip off trailing new line chars
               size_t nlChar = theLine.find("\n");
               if (nlChar != std::string::npos)
                  theLine = theLine.substr(0, nlChar);
               nlChar = theLine.find("\r");
               if (nlChar != std::string::npos)
                  theLine = theLine.substr(0, nlChar);

               dataKeywordFound = CheckDataStart(theLine, theSegment);
               if (!dataKeywordFound)
                  readingHeader = SetHeaderField(theLine, theSegment);
            } while (readingHeader && !dataKeywordFound);

            //Check for required keywords
            if(!startEpochFound)
               throw InterfaceException("Failed to parse the thrust history file " +
                  thrustFileName + "; Unable to find \"Start_Epoch\" keyword for segment \"" + theSegment.segmentName + "\".");
            if (!coordSystemFound)
               throw InterfaceException("Failed to parse the thrust history file " +
                  thrustFileName + "; Unable to find \"Thrust_Vector_Coordinate_System\" keyword for segment \"" + theSegment.segmentName + "\".");

            // Finally comes the thrust profile.
            if (dataKeywordFound)
            {
               #ifdef DEBUG_FILE_DATA
                  MessageInterface::ShowMessage("   Reading the Thrust profile\n");
               #endif
               bool dataEndFound = ReadThrustProfile(theSegment);
            }
            else
            {
               throw InterfaceException("Failed to parse the thrust history file " +
                  thrustFileName + "; Unable to find header value keyword for segment \"" + theSegment.segmentName + "\".");
            }

            // Check if segment name is repeated
            for (UnsignedInt ii = 0; ii < currSegmentNames.size(); ii++)
               if (theSegment.segmentName == currSegmentNames[ii])
                  throw InterfaceException("In ThrustHistoryFile \"" + instanceName + "\", "
                     "the ThrustSegment name \"" + theSegment.segmentName + "\" "
                     "is used more than once");

            currSegmentNames.push_back(theSegment.segmentName);

            ValidateSegment(theSegment);
            SetSegmentData(theSegment);
         }
         else if(theLine != "")
            throw InterfaceException("ThrustHistoryFile \"" + instanceName + "\" has an invalid syntax. Ensure all thrust segments start with \"BeginThrust\" and end in \"EndThrust\". ");
      }
      theStream->close();
      retval = true;
   }
   else
      throw InterfaceException("Failed to open the thrust history file " +
            thrustFileName + "; is the file in the search path?");


   if(!beginThrustFound)
      throw InterfaceException("Failed to parse the thrust history file " +
         thrustFileName + "; \"BeginThrust\" keyword not found.");
   return retval;
}


//------------------------------------------------------------------------------
// bool AllDataSegmentsLoaded()
//------------------------------------------------------------------------------
/**
 * Check that all ThrustSegments for the thrust history file have their data loaded
 *
 * @param segsNotLoaded list of thrust segments whose data hasn't been loaded
 *
 * @return true if all thrust segments have their data loaded
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::AllDataSegmentsLoaded(StringArray &segsNotLoaded)
{
   bool allLoaded = true;
   segsNotLoaded.clear();

   for (std::vector<ThrustSegment>::iterator it = segments.begin(); it != segments.end(); it++)
      if (!it->segData.isDataLoaded) {
         segsNotLoaded.push_back(it->GetName());
         allLoaded = false;
      }

   return allLoaded;
}


//------------------------------------------------------------------------------
// void ActivateSegments()
//------------------------------------------------------------------------------
/**
 * Marks the segments that have been set in AddThrustSegment as active
 */
 //------------------------------------------------------------------------------
void ThrustHistoryFile::ActivateSegments()
{

   //If a segment added to the THF is not in the THF, throw an error. 
   for (UnsignedInt jj = 0; jj < segmentNames.size(); jj++)
   {
      bool found = false;

      for (UnsignedInt ii = 0; ii < segments.size(); ii++)
         if (segmentNames[jj] == segments[ii].segData.segmentName)
            found = true;

      if (!found)
      {
         throw InterfaceException("ThrustSegment \"" + segmentNames[jj] + "\" is not specified in ThrustHistoryFile \"" + GetName() + 
                             "\". ThrustSegments must be specified in every ThrustHistoryFile they are added to.\n");
      }
   }

   //Activate thrust segments
   static bool inactiveSegmentFirstWarning = true;
 
   for (UnsignedInt ii = 0; ii < segments.size(); ii++)
   {
      std::string segName = segments[ii].segData.segmentName;

      segments[ii].segData.isActive =
         std::find(segmentNames.begin(), segmentNames.end(), segName) != segmentNames.end();

      // Warn if not all segments are active
      if (inactiveSegmentFirstWarning && !segments[ii].segData.isActive)
      {
         MessageInterface::ShowMessage ("*** WARNING *** Not all thrust segments in "
            "ThrustHistoryFile \"" + GetName() + "\" are active\n");
         inactiveSegmentFirstWarning = false;
      }
   }

   
}


//------------------------------------------------------------------------------
// void DeactivateSegments()
//------------------------------------------------------------------------------
/**
 * Marks all the segments as inactive
 */
 //------------------------------------------------------------------------------
void ThrustHistoryFile::DeactivateSegments()
{
   for (UnsignedInt ii = 0; ii < segments.size(); ii++)
      segments[ii].segData.isActive = false;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Reads the thrust history file and prepares the file thrust for use
 *
 * @return true if the object initialized successfully
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::Initialize()
{
   bool retval = false;

   retval = ReadData();
   if (retval) {
      for (std::vector<ThrustSegment>::iterator it = segments.begin(); it != segments.end(); it++)
         if (!it->segData.isDataLoaded)
            MessageInterface::ShowMessage("Warning - data not loaded for ThrustHistoryFile '" + this->GetName() +
               "' ThrustSegment '" + it->GetName() + "'\n");

      theForce.SetSegmentList(&segments, &scriptSegments);
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool CheckDataStart(std::string theLine, ThfDataSegment &theSegment)
//------------------------------------------------------------------------------
/**
 * Checks for teh data start keywords
 *
 * @param theLine The line from the thrust history file thta is being checked
 * @param theSegment The segment that received the data
 *
 * @return true if a keyword was found, false if not
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::CheckDataStart(std::string theLine,
      ThfDataSegment &theSegment)
{
   bool retval = false;
   for (UnsignedInt i = 0; i < dataStartKeys.size(); ++i)
   {
      if (theLine.find(dataStartKeys[i]) != std::string::npos)
      {
         #ifdef DEBUG_FILE_READ
            MessageInterface::ShowMessage("   Data Start Keyword %s in line "
                  "\"%s\"\n", dataStartKeys[i].c_str(), theLine.c_str());
         #endif
         // @todo  Sanity check theLine to be sure it's not gorp
         theSegment.modelFlag = dataStartKeys[i];

         retval = true;
      }
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool ThrustHistoryFile::SetHeaderField(std::string theLine,
//       ThfDataSegment &theSegment)
//------------------------------------------------------------------------------
/**
 * Stores segment header data
 *
 * @param theLine The line from the file under analysis
 * @param theSegment The segment that receives the setting
 *
 * @return true if a field keyword was found
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::SetHeaderField(std::string theLine,
      ThfDataSegment &theSegment)
{
   bool retval = false;
   //"Start_Epoch" and "Thrust_Vector_Coordinate_System" must be 0 and 1 in keywords array. 
   StringArray chunks = GmatStringUtil::SeparateBy(theLine, "=");
   if (chunks.size() == 2)
   {
      std::string temp = GmatStringUtil::Trim(chunks[0]);
      for (UnsignedInt i = 0; i < keywords.size(); ++i)
      {
         if (temp == keywords[i])
         {
            //"Start_Epoch" and "Thrust_Vector_Coordinate_System" must be 0 and 1 in keywords array.
            if (i == 0)
               startEpochFound = true;
            if (i == 1)
               coordSystemFound = true;
            #ifdef DEBUG_FILE_READ
               MessageInterface::ShowMessage("      Header Keyword %s in "
                     "line \"%s\" is setting the field to \"%s\"\n",
                     keywords[i].c_str(), theLine.c_str(),
                     GmatStringUtil::Trim(chunks[1]).c_str());
            #endif

            MapField(temp, GmatStringUtil::Trim(chunks[1]), theSegment);
            retval = true;
         }
      }
      if (!retval)   // Check to be sure there is no block end
      {
         if (theLine.find("EndThrust") != std::string::npos)
            throw InterfaceException("An EndSegment was found for the segment \"" +
                        theSegment.segmentName + "\" before any data was read "
                        "for the segment.");
      }
   }
   return retval;
}

//------------------------------------------------------------------------------
// void MapField(const std::string &key, const std::string &datum,
//          ThfDataSegment &theSegment)
//------------------------------------------------------------------------------
/**
 * Sets the values for the header fields
 *
 * @param key The keyword for the field being set
 * @param datum The value for the field
 * @param theSegment The segment storing the field
 */
//------------------------------------------------------------------------------
void ThrustHistoryFile::MapField(const std::string &key,
      const std::string &datum, ThfDataSegment &theSegment)
{
   if (key == "Start_Epoch")
      theSegment.startEpochString = datum;

   if (key == "Thrust_Vector_Coordinate_System")
      theSegment.csName = datum;

   if (key == "Thrust_Vector_Interpolation_Method")
   {
      if ((find(interpolationTypes.begin(), interpolationTypes.end(), datum) !=
            interpolationTypes.end()) && (datum != "ThrustVectorMethod"))
         theSegment.interpolationMethod = datum;
   }

   if (key == "Mass_Flow_Rate_Interpolation_Method")
   {
      if (find(interpolationTypes.begin(), interpolationTypes.end(), datum) !=
            interpolationTypes.end())
         theSegment.massFlowInterpolationMethod = datum;
   }
}


//------------------------------------------------------------------------------
// FileThrust* GetForce()
//------------------------------------------------------------------------------
/**
 * Returns the transient force pointer for the thrust associated with the file
 *
 * @return The force pointer. Ownership for the force remains with this object.
 */
//------------------------------------------------------------------------------
FileThrust* ThrustHistoryFile::GetForce()
{
   // Make sure that the force has access to the data
   theForce.SetSegmentList(&segments, &scriptSegments);
   return &theForce;
}


//------------------------------------------------------------------------------
// void SetSegmentData(ThfDataSegment seg)
//------------------------------------------------------------------------------
/**
 * Passes the data segment from a file to a ThrustSegment
 *
 * @param seg The segment data
 */
//------------------------------------------------------------------------------
void ThrustHistoryFile::SetSegmentData(ThfDataSegment seg)
{
   bool segmentFound = false;

   for (UnsignedInt i = 0; i < segments.size(); ++i)
      if (segments[i].GetName() == seg.segmentName)
      {
         for (UnsignedInt j = 0; j < scriptSegments.size(); ++j)
         {
            if (scriptSegments[j]->GetName() == seg.segmentName)
            {
               // Update segment with any changes in scriptSegment
               ThrustSegment newSegment(*(scriptSegments[j]));
               (*(scriptSegments[i])).SetPrecisionTimeFlag(this->HasPrecisionTime());
               newSegment.SetPrecisionTimeFlag(this->HasPrecisionTime());
               segments[i] = newSegment;
               break;
            }
         }


         segments[i].SetDataSegment(seg);
         segmentFound = true;
      }

   // Segment was not found, so add it
   if (!segmentFound)
   {
      bool scriptSegmentFound = false;

      // First try finding a ThrustSegment from the script
      for (UnsignedInt i = 0; i < scriptSegments.size(); ++i)
      {
         if (scriptSegments[i]->GetName() == seg.segmentName)
         {
            ThrustSegment newSegment(*(scriptSegments[i]));
            (*(scriptSegments[i])).SetPrecisionTimeFlag(this->HasPrecisionTime());
            (*(scriptSegments[i])).SetDataSegment(seg);
            newSegment.SetPrecisionTimeFlag(this->HasPrecisionTime());
            newSegment.SetDataSegment(seg);
            segments.push_back(newSegment);
            scriptSegmentFound = true;
         }
      }

      // If none in the script, create a new one
      if (!scriptSegmentFound)
      {
         ThrustSegment newSegment(seg.segmentName);
         newSegment.SetPrecisionTimeFlag(this->HasPrecisionTime());
         newSegment.SetDataSegment(seg);
         segments.push_back(newSegment);
      }
   }
}


//------------------------------------------------------------------------------
// bool ReadThrustProfile(ThfDataSegment& theSegment)
//------------------------------------------------------------------------------
/**
 * Fills the thrust profile data in the segment
 *
 * @param theSegment The segment that received the data
 *
 * @return true if at least one profile point was parsed
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::ReadThrustProfile(ThfDataSegment& theSegment)
{
   bool retval = true; //false;

   // If mass flow is modeled, the modelFlag will contain the string "MassRate"
   bool includeMass =
         !(theSegment.modelFlag.find("MassRate") == std::string::npos);

   Integer dataCount = (includeMass ? 5 : 4);
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("ModelFlag %s, Include Mass = %s, "
            "dataCount = %d\n", theSegment.modelFlag.c_str(),
            (includeMass ? "true" : "false"), dataCount);
   #endif
   TextParser parser;
   std::stringstream theLine;
   char filedata[2048];

   bool endFound = false;
   while (!theStream->eof() && !endFound)
   {
      theLine.str("");
      theLine.clear();
      theStream->getline(filedata, 2048);
      theLine.str(filedata);

      if (theLine.str().find("EndThrust") != std::string::npos)
      {
         // Find the segment name
         StringArray chunks = parser.Decompose(theLine.str(), "{}", false, true);
         std::string endName;
         if (chunks.size() == 2)
         {
            Integer start = chunks[1].find("{");
            Integer end = chunks[1].find("}");
            if ((start != std::string::npos) && (end != std::string::npos))
               endName = GmatStringUtil::Trim(
                     chunks[1].substr(start+1, (end - start - 1)));

            if (endName != theSegment.segmentName)
               throw InterfaceException("The EndSegment identifies a different "
                     "segment (" + endName +
                     ") than the name name of the current segment, \"" +
                     theSegment.segmentName + "\"");

            #ifdef DEBUG_FILE_READ
               MessageInterface::ShowMessage("Data end found on line '%s'\n",
                     theLine.str().c_str());
            #endif
            endFound = true;
         }
      }
      // String has to be >= 7 characters long, "0 0 0 0", to have valid data
      else if (theLine.str().length() > 6)
      {

         Real theValues[5];
         StringArray vals; 
         ThfDataSegment::ThrustPoint point;
         
         const char space = ' ';
         std::string end = "\n";
         size_t pos = 0;
         std::string sub;

         while (theLine >> sub)
         {
            if(sub != "")
               vals.push_back(sub);
         }

         if (vals.size() != dataCount)
            throw InterfaceException("Failed to parse the thrust history file " +
               thrustFileName + "; Incorrectly sized data found for segment \"" + theSegment.segmentName + "\". \"" +
               theSegment.modelFlag + "\" requires exactly " + std::to_string(dataCount) + " data points per line. Found " + std::to_string(vals.size()) + " data points in: \"" + theLine.str() + "\".\n");
         
         for (int i = 0; i < dataCount; i++)
         {
               if(!GmatStringUtil::ToReal(vals[i], theValues[i], false, false))
                  throw InterfaceException("Failed to parse the thrust history file " +
                     thrustFileName + "; Invalid data found for segment \"" + theSegment.segmentName + "\". " +
                     "Data must be a real number or you must add \"EndThrust\" at the end of your data. Found: \"" + theLine.str() + "\".\n");
         }




         point.time      = theValues[0];
         point.vector[0] = theValues[1];
         point.vector[1] = theValues[2];
         point.vector[2] = theValues[3];
         if (includeMass)
            point.mdot   = theValues[4];

         theSegment.profile.push_back(point);
         retval = true;
      }
   }

   if(!endFound)
      throw InterfaceException("Failed to parse the thrust history file " +
         thrustFileName + "; \"EndThrust\" keyword not found.");

   return retval;
}


//------------------------------------------------------------------------------
// void ValidateSegment(ThfDataSegment& theSegment)
//------------------------------------------------------------------------------
/**
 * Sanity checks the segment, and throws if there is an issue
 *
 * @param theSegment The segment that is being validated
 */
//------------------------------------------------------------------------------
void ThrustHistoryFile::ValidateSegment(ThfDataSegment& theSegment)
{
   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage("Validating data segment\n");
      MessageInterface::ShowMessage("   Segment Name:  %s\n",
            theSegment.segmentName.c_str());
      MessageInterface::ShowMessage("   Start Epoch:   %s\n",
            theSegment.startEpochString.c_str());
      MessageInterface::ShowMessage("   >>>        :   %.12lf\n",
            theSegment.startEpoch);
      MessageInterface::ShowMessage("   >>>        :   %s\n",
         theSegment.startEpochGT.ToString());
      MessageInterface::ShowMessage("   Coord System:  %s\n",
            theSegment.csName.c_str());
      MessageInterface::ShowMessage("   Interpolator:  %s\n",
            theSegment.interpolationMethod.c_str());
      MessageInterface::ShowMessage("   Mass Flow:     %s\n",
            theSegment.massFlowInterpolationMethod.c_str());
      MessageInterface::ShowMessage("   Mode:          %s\n",
            theSegment.modelFlag.c_str());

      MessageInterface::ShowMessage("   Data:\n");
      for (UnsignedInt i = 0; i < theSegment.profile.size(); ++i)
      {
         ThfDataSegment::ThrustPoint point = theSegment.profile[i];
         MessageInterface::ShowMessage("      %lf    [%lf %lf %lf]   %lf\n",
               point.time, point.vector[0], point.vector[1], point.vector[2],
               point.mdot);
      }
   #endif

   // Perform conversions: epochs and vectors
   if (theSegment.startEpochString != "")
   {
      TimeSystemConverter *theTimeConverter = TimeSystemConverter::Instance();

      Real utcEpoch =
            theTimeConverter->ConvertGregorianToMjd(theSegment.startEpochString);
      theSegment.startEpoch = theTimeConverter->Convert(utcEpoch,
            TimeSystemConverter::UTCMJD, TimeSystemConverter::A1MJD);

      GmatTime utcEpochGT =
         theTimeConverter->ConvertGregorianToMjdGT(theSegment.startEpochString);
      theSegment.startEpochGT = theTimeConverter->Convert(utcEpochGT,
         TimeSystemConverter::UTCMJD, TimeSystemConverter::A1MJD);
   }

   for (UnsignedInt i = 0; i < theSegment.profile.size(); ++i)
   {
      // Convert time in sec to epoch offsets in days
      theSegment.profile[i].time /= GmatTimeConstants::SECS_PER_DAY;
   }

   if (theSegment.profile.size() < 2)
   {
      std::stringstream msg;
      msg << "The data segment " << theSegment.segmentName << " contains "
          << theSegment.profile.size() << " data point(s), but file thrusts "
          << "require at least 2 points on the thrust profile.\nThis error can "
          << "occur is the segment data is not started with one of the "
          << "following keywords:\n";
      for (UnsignedInt i = 0; i < dataStartKeys.size(); ++i)
         msg << "   \"" << dataStartKeys[i] << "\"\n";
      throw InterfaceException(msg.str());
   }

   theSegment.endEpoch = theSegment.startEpoch +
      theSegment.profile[theSegment.profile.size() - 1].time;
   theSegment.endEpochGT = theSegment.startEpochGT +
      theSegment.profile[theSegment.profile.size() - 1].time;

   // Set the interpolation method types
   if (theSegment.interpolationMethod == "None")
      theSegment.accelIntType = ThfDataSegment::NONE;
   else if (theSegment.interpolationMethod == "Linear")
      theSegment.accelIntType = ThfDataSegment::LINEAR;
   else if (theSegment.interpolationMethod == "CubicSpline")
      theSegment.accelIntType = ThfDataSegment::SPLINE;
   else
      throw InterfaceException("The interpolation method " +
            theSegment.interpolationMethod + " is not a recognized method of "
                  "interpolation in the Thrust History File segment named " +
                  theSegment.segmentName);

   // Set the thrust or acceleration flag
   if ((theSegment.modelFlag == "ModelThrustOnly") ||
       (theSegment.modelFlag == "ModelThrustAndMassRate"))
      theSegment.modelThrust = true;
   else
      theSegment.modelThrust = false;


   // Set the interpolation method types
   if (theSegment.massFlowInterpolationMethod == "None")
      theSegment.massIntType = ThfDataSegment::NONE;
   else if (theSegment.massFlowInterpolationMethod == "Linear")
      theSegment.massIntType = ThfDataSegment::LINEAR;
   else if (theSegment.massFlowInterpolationMethod == "CubicSpline")
      theSegment.massIntType = ThfDataSegment::SPLINE;
   else
      throw InterfaceException("The interpolation method " +
            theSegment.interpolationMethod + " is not a recognized method of "
                  "interpolation in the Thrust History File segment named " +
                  theSegment.segmentName);

   // Error if epochs overlap with another segment
   bool checkOverlap = true;
   for (UnsignedInt ii = 0; ii < segments.size(); ii++)
   {
      if (theSegment.segmentName == segments[ii].segData.segmentName)
      {
         // Don't write warning message more than once
         // (if segment is already loaded, the warning has already been written
         checkOverlap = false;
         break;
      }
   }

   if (checkOverlap)
   {
      for (UnsignedInt ii = 0; ii < segments.size(); ii++)
      {
         bool overlap = false;

         if (hasPrecisionTime)
         {
            // Start epoch is inside another segment
            overlap = overlap || (theSegment.startEpochGT >= segments[ii].segData.startEpochGT &&
               theSegment.startEpochGT < segments[ii].segData.endEpochGT);
            // End epoch is inside another segment
            overlap = overlap || (theSegment.endEpochGT > segments[ii].segData.startEpochGT &&
               theSegment.endEpochGT <= segments[ii].segData.endEpochGT);
            // This segment envelops another segment
            overlap = overlap || (theSegment.startEpochGT <= segments[ii].segData.startEpochGT &&
               theSegment.endEpochGT >= segments[ii].segData.endEpochGT);
         }
         else
         {
            // Start epoch is inside another segment
            overlap = overlap || (theSegment.startEpoch >= segments[ii].segData.startEpoch &&
               theSegment.startEpoch < segments[ii].segData.endEpoch);
            // End epoch is inside another segment
            overlap = overlap || (theSegment.endEpoch > segments[ii].segData.startEpoch &&
               theSegment.endEpoch <= segments[ii].segData.endEpoch);
            // This segment envelops another segment
            overlap = overlap || (theSegment.startEpoch <= segments[ii].segData.startEpoch &&
               theSegment.endEpoch >= segments[ii].segData.endEpoch);
         }

         if (overlap)
         {
            throw InterfaceException("In ThrustHistoryFile \"" + instanceName + "\", "
               "ThrustSegments \"" + segments[ii].segData.segmentName + "\" "
               "and \"" + theSegment.segmentName + "\" have epochs that overlap. "
               "Currently, GMAT can only apply one ThrustSegment at a given epoch.");
         }
      }
   }

   theSegment.isDataLoaded = true;

   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage("Segment data after processing\n");
      MessageInterface::ShowMessage("   Segment Name:      %s\n",
            theSegment.segmentName.c_str());
      MessageInterface::ShowMessage("   Start Epoch:       %s\n",
            theSegment.startEpochString.c_str());
      MessageInterface::ShowMessage("   >>>        :       %.12lf\n",
            theSegment.startEpoch);
      MessageInterface::ShowMessage("   >>>        :       %s\n",
            theSegment.startEpochGT.ToString());
      MessageInterface::ShowMessage("   Coord System:      %s\n",
            theSegment.csName.c_str());
      MessageInterface::ShowMessage("   Interpolator:      %s\n",
            theSegment.interpolationMethod.c_str());
      MessageInterface::ShowMessage("   Mdot Interpolator: %s\n",
            theSegment.massFlowInterpolationMethod.c_str());
      MessageInterface::ShowMessage("   Mode:              %s\n",
            theSegment.modelFlag.c_str());

      MessageInterface::ShowMessage("   Data:\n");
      for (UnsignedInt i = 0; i < theSegment.profile.size(); ++i)
      {
         ThfDataSegment::ThrustPoint point = theSegment.profile[i];
         MessageInterface::ShowMessage("      %lf    [%lf %lf %lf]   %lf\n",
               point.time, point.vector[0], point.vector[1], point.vector[2],
               point.mdot);
      }
   #endif
}


//------------------------------------------------------------------------------
// bool SetPrecisionTimeFlag(bool onOff)
//------------------------------------------------------------------------------
/**
* Set whether the thrust segment is using precision time or not
*
* @param onOff Flag indicating whether the thrust segment uses precision time
*
* @return Returns the value of the onOff flag passed in
*/
//------------------------------------------------------------------------------
bool ThrustHistoryFile::SetPrecisionTimeFlag(bool onOff)
{
   hasPrecisionTime = onOff;

   for (ThrustSegment seg : segments)
      seg.SetPrecisionTimeFlag(onOff);

   theForce.SetPrecisionTimeFlag(onOff);

   return hasPrecisionTime;
}
