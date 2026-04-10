//$Id$
//------------------------------------------------------------------------------
//                           DataInterface
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

#include "DataInterface.hpp"
#include "InterfaceException.hpp"


//-----------------------------------------
// Static data
//-----------------------------------------
const std::string
DataInterface::PARAMETER_LABEL[DataInterfaceParamCount - InterfaceParamCount] =
{
      "Format",
      "SelectedFields",
      "SupportedFields"
};

const Gmat::ParameterType
DataInterface::PARAMETER_TYPE[DataInterfaceParamCount - InterfaceParamCount] =
{
      Gmat::ENUMERATION_TYPE,
      Gmat::STRINGARRAY_TYPE,
      Gmat::STRINGARRAY_TYPE
};



//------------------------------------------------------------------------------
// DataInterface(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The type of the interface
 * @param name The new object's name
 */
//------------------------------------------------------------------------------
DataInterface::DataInterface(const std::string &type, const std::string &name) :
   Interface         (type, name),
   readerFormat      (""),
   theReader         (NULL)
{
   objectTypeNames.push_back("DataInterface");
}

//------------------------------------------------------------------------------
// ~DataInterface()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DataInterface::~DataInterface()
{
}

//------------------------------------------------------------------------------
// DataInterface(const DataInterface& di)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param di The interface copied to the new instance
 */
//------------------------------------------------------------------------------
DataInterface::DataInterface(const DataInterface& di) :
   Interface         (di),
   readerFormat      (di.readerFormat),
   theReader         (NULL),
   supportedFormats    (di.supportedFormats)
{
}

//------------------------------------------------------------------------------
// DataInterface& operator=(const DataInterface& di)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param di The interface copied to this one
 *
 * @return This instance set to match di
 */
//------------------------------------------------------------------------------
DataInterface& DataInterface::operator=(const DataInterface& di)
{
   if (this != &di)
   {
      readerFormat = di.readerFormat;
      if (theReader)
         delete theReader;
      theReader = NULL;
      supportedFormats = di.supportedFormats;
   }

   return *this;
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 *Retrieves the script label for a parameter
 *
 * @param id The parameter's ID
 *
 * @return The label
 */
//------------------------------------------------------------------------------
std::string DataInterface::GetParameterText(const Integer id) const
{
   if (id >= InterfaceParamCount && id < DataInterfaceParamCount)
      return PARAMETER_LABEL[id - InterfaceParamCount];
   return Interface::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID of a parameter
 *
 * @param str The label for the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer DataInterface::GetParameterID(const std::string& str) const
{
   Integer id = -1;

   for (Integer i = InterfaceParamCount; i < DataInterfaceParamCount; i++)
   {
      if (str == PARAMETER_LABEL[i - InterfaceParamCount])
      {
         id = i;
         break;
      }
   }

   if (id != -1)
   {
      #ifdef DEBUG_DATAINTERFACE_PARAM
      MessageInterface::ShowMessage("DataInterface::GetParameterID() returning "
            "%d\n", id);
      #endif

      return id;
   }

   return Interface::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a parameter
 *
 * @param id The parameter's ID
 *
 * @return The type of the parameter
 */
//------------------------------------------------------------------------------
Gmat::ParameterType DataInterface::GetParameterType(const Integer id) const
{
   if (id >= InterfaceParamCount && id < DataInterfaceParamCount)
      return PARAMETER_TYPE[id - InterfaceParamCount];

   return Interface::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a descriptive label for a parameter's type
 *
 * @param id The parameter's ID
 *
 * @return The label
 */
//------------------------------------------------------------------------------
std::string DataInterface::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Checks to see if a parameter is user settable
 *
 * @param id The parameter's ID
 *
 * @return true for user settable parameters, false if not settable
 */
//------------------------------------------------------------------------------
bool DataInterface::IsParameterReadOnly(const Integer id) const
{
   if ((id == SUPPORTED_FIELD_NAMES) || (id == SELECTED_FIELD_NAMES))
      return true;
   return Interface::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Checks to see if a parameter is user settable
 *
 * @param label The parameter's script label
 *
 * @return true for user settable parameters, false if not settable
 */
//------------------------------------------------------------------------------
bool DataInterface::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a string parameter
 *
 * @param id The parameter's ID
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string DataInterface::GetStringParameter(const Integer id) const
{
   if (id == FORMAT)
      return readerFormat;

   return Interface::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the data for a string parameter
 *
 * @param id The parameter's ID
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataInterface::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == FORMAT)
   {
      if (find(supportedFormats.begin(), supportedFormats.end(), value) !=
            supportedFormats.end())
      {
         readerFormat = value;
         return true;
      }
      else
      {
         std::string theList;
         for (UnsignedInt i = 0; i < supportedFormats.size(); ++i)
         {
            if (i > 0)
               theList += ", ";
            theList += supportedFormats[i];
         }
         InterfaceException ex("");
         ex.SetDetails(errorMessageFormat.c_str(), value.c_str(),
               GetParameterText(id).c_str(), theList.c_str());
         throw ex;
      }
   }

   return Interface::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param id The parameter ID
 * @param index The index into the array
 *
 * @return The value
 */
//------------------------------------------------------------------------------
std::string DataInterface::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == SELECTED_FIELD_NAMES)
   {
      if (theReader)
      {
         StringArray selections = theReader->GetSelectedFieldNames();
         if ((index >= 0) && (index < (Integer)selections.size()))
            return selections[index];
         throw InterfaceException("The selection index is out of bounds in the "
               "DataReader \"" + instanceName + "\"");
      }
      throw InterfaceException("The DataReader has not been set in the "
            "DataInterface \"" + instanceName + "\"");
   }

   if (id == SUPPORTED_FIELD_NAMES)
   {
      if (theReader)
      {
         StringArray selections = theReader->GetSupportedFieldNames();
         if ((index >= 0) && (index < (Integer)selections.size()))
            return selections[index];
         throw InterfaceException("The supported field index is out of bounds "
               "in the DataReader \"" + instanceName + "\"");
      }
      throw InterfaceException("The DataReader has not been set in the "
            "DataInterface \"" + instanceName + "\"");
   }

   return Interface::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string parameter in a string array
 *
 * @param id The parameter ID
 * @param value The new parameter value
 * @param index The index of the parameter in the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataInterface::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   if (id == SELECTED_FIELD_NAMES)
   {
      if (theReader)
         return theReader->SetStringParameter("SelectedFieldNames", value,
               index);
      throw InterfaceException("The DataReader has not been set in the "
            "DataInterface \"" + instanceName + "\"");
   }

   if (id == SUPPORTED_FIELD_NAMES)
   {
      if (theReader)
         return theReader->SetStringParameter("SupportedFieldNames", value,
               index);
      throw InterfaceException("The DataReader has not been set in the "
            "DataInterface \"" + instanceName + "\"");
   }

   return Interface::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a string parameter
 *
 * @param label The parameter's script label
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string DataInterface::GetStringParameter(const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the data for a string parameter
 *
 * @param label The parameter's script label
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataInterface::SetStringParameter(const std::string& label,
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
 * @return The value
 */
//------------------------------------------------------------------------------
std::string DataInterface::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string parameter in a string array
 *
 * @param label The parameter's script label
 * @param value The new parameter value
 * @param index The index of the parameter in the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataInterface::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param id The ID of the parameter
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& DataInterface::GetStringArrayParameter(
      const Integer id) const
{
   if (id == SELECTED_FIELD_NAMES)
   {
      if (theReader)
         return theReader->GetSelectedFieldNames();
      throw InterfaceException("The DataReader has not been set in the "
            "DataInterface \"" + instanceName + "\"");
   }

   if (id == SUPPORTED_FIELD_NAMES)
   {
      return GetSupportedFieldNames();
   }

   return Interface::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter from a vector of StringArrays
 *
 * @param id The parameter ID
 * @param index The index into the vector
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& DataInterface::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   return Interface::GetStringArrayParameter(id, index);
}

//------------------------------------------------------------------------------
// const StringArray& DataInterface::GetStringArrayParameter(
//       const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param label The script label for the parameter
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& DataInterface::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// const StringArray& DataInterface::GetStringArrayParameter(
//       const std::string& label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter from a vector of StringArrays
 *
 * @param label The script label for the parameter
 * @param index The index into the vector
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& DataInterface::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the list of field values accepted for an enumerated field
 *
 * @param id The ID of the field
 *
 * @return The list of accepted values
 */
//------------------------------------------------------------------------------
const StringArray& DataInterface::GetPropertyEnumStrings(const Integer id) const
{
   if (id == FORMAT)
      return supportedFormats;

   return Interface::GetPropertyEnumStrings(id);
}

//------------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the list of field values accepted for an enumerated field
 *
 * @param label The scripted string for the field
 *
 * @return The list of accepted values
 */
//------------------------------------------------------------------------------
const StringArray& DataInterface::GetPropertyEnumStrings(
      const std::string &label) const
{
   return GetPropertyEnumStrings(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetSupportedFieldNames() const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of fields that the interface supports
 *
 * @return The list, in a StringArray
 */
//------------------------------------------------------------------------------
const StringArray& DataInterface::GetSupportedFieldNames() const
{
   if (theReader)
      return theReader->GetSupportedFieldNames();
   throw InterfaceException("The DataReader has not been set in the "
         "DataInterface \"" + instanceName + "\"");
}

//------------------------------------------------------------------------------
// Integer Open(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Opens the interface stream
 *
 * @param name A name used when opening the interface
 *
 * @return 0 on success, or an error code
 */
//------------------------------------------------------------------------------
Integer DataInterface::Open(const std::string& name)
{
   Integer retval = -1;
   return retval;
}

//------------------------------------------------------------------------------
// bool DataInterface::WasDataLoaded(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Checks to see if data was loaded in the interface's reader
 *
 * @param forField The field descriptor being checked
 *
 * @return true is the data was loaded, false if not
 */
//------------------------------------------------------------------------------
bool DataInterface::WasDataLoaded(const std::string& forField)
{
   return theReader->WasDataLoaded(forField);
}

//------------------------------------------------------------------------------
// const std::string GetObjectParameterName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter string for the target object
 *
 * @param forField The interface identifier for the parameter.  This is likely
 *                 to be the parameter name in many cases
 *
 * @return The parameter name
 */
//------------------------------------------------------------------------------
const std::string DataInterface::GetObjectParameterName(
      const std::string& forField)
{
   return theReader->GetObjectParameterName(forField);
}

//------------------------------------------------------------------------------
// const DataReader::readerDataType GetReaderParameterType(
//       const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Returns the data type as seen in the DataReader object.
 *
 * @param forField The interface identifier for the parameter.
 *
 * @return The readerDataType for the field.
 */
//------------------------------------------------------------------------------
DataReader::readerDataType DataInterface::GetReaderParameterType(      const std::string& forField)
{
   DataReader::readerDataType theType = DataReader::READER_UNKNOWN;

   if (theReader)
      theType = theReader->GetReaderDataType(forField);

   return theType;
}

//------------------------------------------------------------------------------
// Integer Close(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Closes the reader
 *
 * @param name Name of the reader to close.  This parameter is ignored for
 *             DataReader objects
 *
 * @return 0 in success, or an error code
 */
//------------------------------------------------------------------------------
Integer DataInterface::Close(const std::string& name)
{
   Integer retval = -1;
   return retval;
}

//------------------------------------------------------------------------------
// Real GetRealValue(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Pass-through function to access a read Real number
 *
 * @param forField The field identifier for the data
 *
 * @return The number
 */
//------------------------------------------------------------------------------
Real DataInterface::GetRealValue(const std::string& forField)
{
   if (!theReader)
      throw InterfaceException("The data reader has not been set");
   return theReader->GetRealValue(forField);
}

//------------------------------------------------------------------------------
// Rvector6 GetReal6Vector(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Pass-through function to access a read 6-vector of Real numbers
 *
 * @param forField The field identifier for the data
 *
 * @return The number
 */
//------------------------------------------------------------------------------
Rvector6 DataInterface::GetReal6Vector(const std::string& forField)
{
   if (!theReader)
      throw InterfaceException("The data reader has not been set");
   return theReader->GetReal6Vector(forField);
}

//------------------------------------------------------------------------------
// std::string GetStringValue(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Pass-through function to access a read string
 *
 * @param forField The field identifier for the data
 *
 * @return The number
 */
//------------------------------------------------------------------------------
std::string DataInterface::GetStringValue(const std::string& forField)
{
   if (!theReader)
      throw InterfaceException("The data reader has not been set");
   return theReader->GetStringValue(forField);
}

//------------------------------------------------------------------------------
// bool UsesCoordinateSystem(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Pass-through function to see if a field is coordinate system dependent
 *
 * @param forField The field identifier for the data
 *
 * @return true if the field is coordinate system dependent, false if not
 */
//------------------------------------------------------------------------------
bool DataInterface::UsesCoordinateSystem(const std::string& forField)
{
   bool retval = false;
   if (theReader)
      retval = theReader->UsesCoordinateSystem(forField);
   return retval;
}

//------------------------------------------------------------------------------
// std::string GetCoordinateSystemName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves the coordinate system name for a coordinate system dependent field
 *
 * @param forField The field label
 *
 * @return The coordinate system name
 */
//------------------------------------------------------------------------------
std::string DataInterface::GetCoordinateSystemName(const std::string& forField)
{
   std::string retval = "";
   if (theReader)
      retval = theReader->GetCoordinateSystemName(forField);
   return retval;
}

//------------------------------------------------------------------------------
// bool UsesOrigin(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Checks to see if a field uses an origin
 *
 * @param forField The field label
 *
 * @return true if an origin is used, false if not
 */
//------------------------------------------------------------------------------
bool DataInterface::UsesOrigin(const std::string& forField)
{
   bool retval = false;
   if (theReader)
      retval = theReader->UsesOrigin(forField);
   return retval;
}

//------------------------------------------------------------------------------
// std::string GetOriginName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a field's origin
 *
 * @param forField The field label
 *
 * @return The name of the origin
 */
//------------------------------------------------------------------------------
std::string DataInterface::GetOriginName(const std::string& forField)
{
   std::string retval = "";
   if (theReader)
      retval = theReader->GetOriginName(forField);
   return retval;
}

//------------------------------------------------------------------------------
// bool UsesTimeSystem(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Checks to see if a field uses a time system
 *
 * @param forField The field label
 *
 * @return true is a time system is used, false if not
 */
//------------------------------------------------------------------------------
bool DataInterface::UsesTimeSystem(const std::string& forField)
{
   bool retval = false;
   if (theReader)
      retval = theReader->UsesTimeSystem(forField);
   return retval;
}

//------------------------------------------------------------------------------
// std::string GetTimeSystemName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves teh name of a used time system
 *
 * @param forField The field label
 *
 * @return The time system name
 */
//------------------------------------------------------------------------------
std::string DataInterface::GetTimeSystemName(const std::string& forField)
{
   std::string retval = "";
   if (theReader)
      retval = theReader->GetTimeSystemName(forField);
   return retval;
}
