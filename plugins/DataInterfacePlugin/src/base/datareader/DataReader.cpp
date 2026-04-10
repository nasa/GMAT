//$Id$
//------------------------------------------------------------------------------
//                           DataReader
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
 * Implementation of the base reader class used in the DataInterface subsystem
 */
//------------------------------------------------------------------------------

#include "DataReader.hpp"


//------------------------------------------------------------------------------
// DataReader(const std::string& theTypeName, const std::string& theName)
//------------------------------------------------------------------------------
/**
 * DEfault constructor
 *
 * @param theTypeName Type name for the DataReader subtype
 * @param theName The name of the created instance
 */
//------------------------------------------------------------------------------
DataReader::DataReader(const std::string& theTypeName, const std::string& theName) :
   GmatBase                (Gmat::DATAINTERFACE_SOURCE, theTypeName, theName),
   readAllSupportedFields  (true),
   dataReady               (false),
   clearOnRead             (true)
{
   // Engine required fields
   objectTypes.push_back(Gmat::DATAINTERFACE_SOURCE);
   objectTypeNames.push_back("DataReader");
}

//------------------------------------------------------------------------------
// ~DataReader()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DataReader::~DataReader()
{
}

//------------------------------------------------------------------------------
// DataReader(const DataReader& dr)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param dr The DataReader copied to the new instance
 */
//------------------------------------------------------------------------------
DataReader::DataReader(const DataReader& dr) :
   GmatBase                (dr),
   selectedFields          (dr.selectedFields),
   supportedFields         (dr.supportedFields),
   readAllSupportedFields  (dr.readAllSupportedFields),
   dataReady               (false),
   clearOnRead             (dr.clearOnRead),
   fileStringMap           (dr.fileStringMap),
   objectStringMap         (dr.objectStringMap),
   objectIDMap             (dr.objectIDMap),
   dataType                (dr.dataType)
{
}

//------------------------------------------------------------------------------
// DataReader& operator=(const DataReader& dr)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param dr The DataReader used to set the properties of this one.
 *
 * @return This reader set to match dr
 */
//------------------------------------------------------------------------------
DataReader& DataReader::operator=(const DataReader& dr)
{
   if (this != &dr)
   {
      selectedFields         = dr.selectedFields;
      supportedFields        = dr.supportedFields;
      readAllSupportedFields = dr.readAllSupportedFields;
      dataReady              = false;
      clearOnRead            = dr.clearOnRead;
      fileStringMap          = dr.fileStringMap;
      objectStringMap        = dr.objectStringMap;
      objectIDMap            = dr.objectIDMap;
      dataType               = dr.dataType;
   }

   return *this;
}

//------------------------------------------------------------------------------
// const StringArray& GetSelectedFieldNames()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of selected fields
 *
 * @return The list
 */
//------------------------------------------------------------------------------
const StringArray& DataReader::GetSelectedFieldNames()
{
   return selectedFields;
}

//------------------------------------------------------------------------------
// void SetSelectedFieldNames(const StringArray& selections)
//------------------------------------------------------------------------------
/**
 * Sets new selections in the reader
 *
 * @param selections The new selections
 */
//------------------------------------------------------------------------------
void DataReader::SetSelectedFieldNames(const StringArray& selections)
{
   selectedFields = selections;
}

//------------------------------------------------------------------------------
// const StringArray& GetSupportedFieldNames()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of fields the reader supports
 *
 * @return The list
 */
//------------------------------------------------------------------------------
const StringArray& DataReader::GetSupportedFieldNames()
{
   // Leaf classes fill in the field identifiers; we just return that list here
   return supportedFields;
}

//------------------------------------------------------------------------------
// bool WasDataLoaded(const std::string &theField)
//------------------------------------------------------------------------------
/**
 * Checks to see if data was loaded for a specific field
 *
 * @param theField The field identifier
 *
 * @return true if data was loaded, false if not
 */
//------------------------------------------------------------------------------
bool DataReader::WasDataLoaded(const std::string &theField)
{
   return dataLoaded[theField];
}

//------------------------------------------------------------------------------
// void ClearData()
//------------------------------------------------------------------------------
/**
 * Clears loaded data
 *
 * This method does not actually remove loaded data.  The "dataLoaded" flags are
 * cleared and the object level dataReady flag is set to false.
 */
//------------------------------------------------------------------------------
void DataReader::ClearData()
{
   for (std::map<std::string, bool>::iterator i = dataLoaded.begin(); 
        i != dataLoaded.end(); ++i)
      (*i).second = false;
   dataReady = false;
}

//------------------------------------------------------------------------------
// Real GetRealValue(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves a real data value
 *
 * @param forField The field identifier for the data
 *
 * @return The value if data was loaded, or a dummy value if not.
 */
//------------------------------------------------------------------------------
Real DataReader::GetRealValue(const std::string& forField)
{
   Real retval = -999999.999999;

   if (dataReady)
   {
      retval = GetRData(forField);
   }

   return retval;
}

//------------------------------------------------------------------------------
// Rvector6 GetReal6Vector(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves a 6-vector of real data values
 *
 * @param forField The field identifier for the data
 *
 * @return The value if data was loaded, or a dummy vector if not.
 */
//------------------------------------------------------------------------------
Rvector6 DataReader::GetReal6Vector(const std::string& forField)
{
   Rvector6 retval(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

   if (dataReady)
   {
      retval = GetRVectorData(forField);
   }

   return retval;
}

//------------------------------------------------------------------------------
// std::string GetStringValue(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves a string value
 *
 * @param forField The field identifier for the data
 *
 * @return The value if data was loaded, or a dummy string if not.
 */
//------------------------------------------------------------------------------
std::string DataReader::GetStringValue(const std::string& forField)
{
   std::string retval = "No data";

   if (dataReady)
   {
      retval = GetSData(forField);
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool UsesCoordinateSystem(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Checks to see if a field uses a coordinate system
 *
 * @param forField The field identifier for check
 *
 * @return true if a coordinate system is associated with the field, false
 *              if not
 */
//------------------------------------------------------------------------------
bool DataReader::UsesCoordinateSystem(const std::string& forField)
{
   return false;
}

//------------------------------------------------------------------------------
// std::string GetCoordinateSystemName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a used coordinate system
 *
 * @param forField The field identifier for the data that used the coordinate
 *                 system
 *
 * @return The name of the coordinate system
 */
//------------------------------------------------------------------------------
std::string DataReader::GetCoordinateSystemName(const std::string& forField)
{
   return "";
}

//------------------------------------------------------------------------------
// bool UsesOrigin(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Checks to see if a field uses an origin
 *
 * @param forField The field identifier for check
 *
 * @return true if an origin is associated with the field, false if not
 */
//------------------------------------------------------------------------------
bool DataReader::UsesOrigin(const std::string& forField)
{
   return false;
}

//------------------------------------------------------------------------------
// std::string GetOriginName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a used origin
 *
 * @param forField The field identifier for the data that used the origin
 *
 * @return The name of the origin
 */
//------------------------------------------------------------------------------
std::string DataReader::GetOriginName(const std::string& forField)
{
   return "";
}

//------------------------------------------------------------------------------
// bool UsesTimeSystem(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Checks to see if a field uses a time system
 *
 * @param forField The field identifier for check
 *
 * @return true if a time system is associated with the field, false if not
 */
//------------------------------------------------------------------------------
bool DataReader::UsesTimeSystem(const std::string& forField)
{
   return false;
}

//------------------------------------------------------------------------------
// std::string GetTimeSystemName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a used time system
 *
 * @param forField The field identifier for the data that used the time system
 *
 * @return The name of the time system
 */
//------------------------------------------------------------------------------
std::string DataReader::GetTimeSystemName(const std::string& forField)
{
   return "";
}

bool DataReader::SetStream(std::ifstream* aStream, const std::string &fname)
{
   return false;
}

//------------------------------------------------------------------------------
// const std::string GetObjectParameterName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Gets the object parameter name for a data field
 *
 * @param forField The field name
 *
 * @return The scripted parameter name
 */
//------------------------------------------------------------------------------
const std::string DataReader::GetObjectParameterName(
      const std::string& forField)
{
   if (objectStringMap.find(forField) != objectStringMap.end())
      return objectStringMap[forField];
   return "";
}

//------------------------------------------------------------------------------
// const readerDataType GetReaderDataType(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Returns the data type as seen in the DataReader object.
 *
 * @param forField The interface identifier for the parameter.
 *
 * @return The readerDataType for the field.
 */
//------------------------------------------------------------------------------
DataReader::readerDataType DataReader::GetReaderDataType(      const std::string& forField)
{
   readerDataType theType = DataReader::READER_UNKNOWN;

   if (dataType.find(forField) != dataType.end())
      theType = dataType[forField];

   return theType;
}

//------------------------------------------------------------------------------
// Real GetRData(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves a real data value
 *
 * @param forField The interface identifier for the parameter.
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real DataReader::GetRData(const std::string& forField)
{
   Real retval = -999999.999999;

   if (realData.find(forField) != realData.end())
      retval = realData[forField];

   return retval;
}

//------------------------------------------------------------------------------
// Rvector6 GetRVectorData(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves a 6-vector of real data
 *
 * @param forField The interface identifier for the parameter.
 *
 * @return The data vector
 */
//------------------------------------------------------------------------------
Rvector6 DataReader::GetRVectorData(const std::string& forField)
{
   Rvector6 retval(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

   if (rvector6Data.find(forField) != rvector6Data.end())
      retval = rvector6Data[forField];

   return retval;
}

//------------------------------------------------------------------------------
// std::string GetSData(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves string data
 *
 * @param forField The field that references teh string
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string DataReader::GetSData(const std::string& forField)
{
   std::string retval = "No data";

   if (stringData.find(forField) != stringData.end())
      retval = stringData[forField];

   return retval;
}
