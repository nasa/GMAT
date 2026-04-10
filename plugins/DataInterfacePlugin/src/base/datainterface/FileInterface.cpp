//$Id$
//------------------------------------------------------------------------------
//                           FileInterface
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
 * Implementation of file based DataInterfaces
 */
//------------------------------------------------------------------------------

#include "FileInterface.hpp"
#include "FileUtil.hpp"
#include "ReaderFactory.hpp"        // Not a registered factory for now, but it
                                    // uses the factory interfaces
#include "MessageInterface.hpp"
#include "InterfaceException.hpp"


//#define DEBUG_FILEINTERFACE_PARAM
//#define DEBUG_READER_CREATION


//-----------------------------------------
// Static data
//-----------------------------------------
const std::string
FileInterface::PARAMETER_LABEL[FileInterfaceParamCount - DataInterfaceParamCount] =
{
   "Filename",
};

const Gmat::ParameterType
FileInterface::PARAMETER_TYPE[FileInterfaceParamCount - DataInterfaceParamCount] =
{
   Gmat::FILENAME_TYPE,
};


//------------------------------------------------------------------------------
// FileInterface(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the constructed object
 */
//------------------------------------------------------------------------------
FileInterface::FileInterface(const std::string &name) :
   DataInterface           ("FileInterface", name),
   filename                (""),
   streamIsBinary          (false)
{
   objectTypeNames.push_back("FileInterface");
   parameterCount = FileInterfaceParamCount;

   ReaderFactory fact;
   supportedFormats = fact.GetListOfCreatableObjects();

   if (supportedFormats.size() > 0)
      readerFormat = supportedFormats[0];
}


//------------------------------------------------------------------------------
// ~FileInterface()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
FileInterface::~FileInterface()
{
   if (theReader != NULL)
      delete theReader;
}

//------------------------------------------------------------------------------
// FileInterface(const FileInterface& fi)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fi The interface that is copied here
 */
//------------------------------------------------------------------------------
FileInterface::FileInterface(const FileInterface& fi) :
   DataInterface           (fi),
   filename                (fi.filename),
   streamIsBinary          (fi.streamIsBinary)
{
}

//------------------------------------------------------------------------------
// FileInterface operator=(const FileInterface& fi)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fi The interface providing data for this one
 *
 * @return This FileInterface, configured to match fi
 */
//------------------------------------------------------------------------------
FileInterface FileInterface::operator=(const FileInterface& fi)
{
   if (this != &fi)
   {
      DataInterface::operator =(fi);

      filename = fi.filename;
      streamIsBinary = fi.streamIsBinary;
   }
   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new FileInterface tht matches this one
 *
 * @return The new object
 */
//------------------------------------------------------------------------------
GmatBase* FileInterface::Clone() const
{
   return new FileInterface(*this);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the script label of a parameter
 *
 * @param id The ID of the parameter
 *
 * @return The label
 */
//------------------------------------------------------------------------------
std::string FileInterface::GetParameterText(const Integer id) const
{
   if (id >= DataInterfaceParamCount && id < FileInterfaceParamCount)
      return PARAMETER_LABEL[id - DataInterfaceParamCount];
   return DataInterface::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the integer ID of a parameter
 *
 * @param str The label for the parameter
 *
 * @return The parameter's ID
 */
//------------------------------------------------------------------------------
Integer FileInterface::GetParameterID(const std::string& str) const
{
   Integer id = -1;

   for (Integer i = DataInterfaceParamCount; i < FileInterfaceParamCount; i++)
   {
      if (str == PARAMETER_LABEL[i - DataInterfaceParamCount])
      {
         id = i;
         break;
      }
   }

   if (id == -1)
   {
      id = DataInterface::GetParameterID(str);
   }

   #ifdef DEBUG_FILEINTERFACE_PARAM
   MessageInterface::ShowMessage("DataInterface::GetParameterID() returning "
         "%d\n", id);
   #endif
   return id;
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type of a parameter
 *
 * @param id The parameter's ID
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType FileInterface::GetParameterType(const Integer id) const
{
   if (id >= DataInterfaceParamCount && id < FileInterfaceParamCount)
      return PARAMETER_TYPE[id - DataInterfaceParamCount];

   return DataInterface::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description of a parameter's type
 *
 * @param id The parameter's ID
 *
 * @return The description
 */
//------------------------------------------------------------------------------
std::string FileInterface::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Checks to see if the parameter is user settable
 *
 * @param id The parameter's ID
 *
 * @return true for parameters that can be edited, false otherwise
 */
//------------------------------------------------------------------------------
bool FileInterface::IsParameterReadOnly(const Integer id) const
{
   return DataInterface::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Checks to see if the parameter is user settable
 *
 * @param label The parameter's script label
 *
 * @return true for parameters that can be edited, false otherwise
 */
//------------------------------------------------------------------------------
bool FileInterface::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param id The parameter's ID
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string FileInterface::GetStringParameter(const Integer id) const
{
   if (id == FILENAME)
      return filename;

   return DataInterface::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param id The parameter's ID
 * @param value The new parameter value
 *
 * @return true if set successfully, false if not.
 */
//------------------------------------------------------------------------------
bool FileInterface::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == FILENAME)
   {
      filename = value;
      return true;
   }

   return DataInterface::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param id The parameter's ID
 * @param index The index into the array
 *
 * @return The referenced string
 */
//------------------------------------------------------------------------------
std::string FileInterface::GetStringParameter(const Integer id,
      const Integer index) const
{
   return DataInterface::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter from a string array
 *
 * @param id The parameter's ID
 * @param value The new value
 * @param index The index into the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool FileInterface::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   return DataInterface::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param label The parameter's script label
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string FileInterface::GetStringParameter(const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The parameter's script label
 * @param value The new parameter value
 *
 * @return true if set successfully, false if not.
 */
//------------------------------------------------------------------------------
bool FileInterface::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param label The parameter's script label
 * @param index The index into the array
 *
 * @return The referenced string
 */
//------------------------------------------------------------------------------
std::string FileInterface::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter from a string array
 *
 * @param label The parameter's script label
 * @param value The new value
 * @param index The index into the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool FileInterface::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Sets up the reader that the interface uses and prepares for reading
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool FileInterface::Initialize()
{
   bool retval = false;

   #ifdef DEBUG_READER_CREATION
      MessageInterface::ShowMessage("Initializing the %s file interface\n",
            instanceName.c_str());
   #endif

   if (filename == "")
      throw InterfaceException("The FileInterface \"" + instanceName +
            "\" does not identify the file that it needs to read.  Please set "
            "the \"Filename\" field on the object.");

   // Verify that the file exists
   std::string sep = GmatFileUtil::GetPathSeparator();
   std::replace(filename.begin(), filename.end(), '/', sep[0]);
   std::replace(filename.begin(), filename.end(), '\\', sep[0]);

   if (GmatFileUtil::DoesFileExist(filename))
   {
      if (readerFormat == "")
         throw InterfaceException("The FileInterface \"" + instanceName +
               "\" does not identify the file format that it needs to read.  "
               "Please set the \"Format\" field on the object.");

      ReaderFactory rf;

      if (theReader != NULL)
         delete theReader;
      theReader = (DataReader*)rf.CreateObject(readerFormat, "");
      if (theReader != NULL)
      {
         #ifdef DEBUG_READER_CREATION
            MessageInterface::ShowMessage("A %s reader was created\n",
                  readerFormat.c_str());
         #endif
         retval = true;
      }
      else
         throw InterfaceException("The FileInterface \"" + instanceName +
               "\" was unable to create a \"" + readerFormat + 
               "\" reader for the data.");
   }
   else
      throw InterfaceException("The FileInterface \"" + instanceName +
            "\" is set to read the file \"" + filename +
            "\", but the file does not exist.");

   return retval;
}

//------------------------------------------------------------------------------
// Integer Open(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Opens the file stream so data can be read
 *
 * @param name Name for the interface (unused for FoleInterface objects)
 *
 * @return 0 on success, or an error code
 */
//------------------------------------------------------------------------------
Integer FileInterface::Open(const std::string& name)
{
   #ifdef DEBUG_READER_CREATION
      MessageInterface::ShowMessage("FileInterface::Open() called\n"
            "   theReader = <%p>\n", theReader);
   #endif

   Integer retval = -1;

   if (theReader)
   {
      if (theStream.is_open())
         throw InterfaceException("The FileInterface \"" + instanceName +
            "\" attempted to open the file \"" + filename +
            "\", but the file is already open.");

      theStream.open(filename.c_str());
      theReader->SetStream(&theStream, filename);

      retval = 0;
   }
   else
      throw InterfaceException("Cannot open the file interface");

   return retval;
}

//------------------------------------------------------------------------------
// bool LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data into the interface
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool FileInterface::LoadData()
{
   #ifdef DEBUG_READER_CREATION
      MessageInterface::ShowMessage("FileInterface::LoadData() called\n");
   #endif

   bool retval = false;

   if (theReader && theStream.is_open())
   {
      retval = theReader->ReadData();
   }

   return retval;
}

//------------------------------------------------------------------------------
// Integer Close(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Closes the FileInterface
 *
 * @param name Name for the interface (unused for FoleInterface objects)
 *
 * @return 0 on success, or an error code
 */
//------------------------------------------------------------------------------
Integer FileInterface::Close(const std::string& name)
{
   #ifdef DEBUG_READER_CREATION
      MessageInterface::ShowMessage("FileInterface::Close() called\n");
   #endif

   Integer retval = -1;

   if (theReader)
   {
      if (theStream.is_open())
         theStream.close();
      retval = 0;
   }

   return retval;
}
