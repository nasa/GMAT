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
 * Definition of the base reader class used in the DataInterface subsystem
 */
//------------------------------------------------------------------------------

#ifndef DataReader_hpp
#define DataReader_hpp

#include "DataInterfaceDefs.hpp"
#include "GmatBase.hpp"
#include "Rvector6.hpp"


/**
 * Base reader class used in the DataInterface subsystem
 */
class DATAINTERFACE_API DataReader : public GmatBase
{
public:
   enum readerDataType
   {
      READER_REAL = 30000,
      READER_RVECTOR6,
      READER_STRING,
      READER_TIMESTRING,         // A string that needs special treatment
      READER_SUBTYPE,
      READER_UNKNOWN
   };

   DataReader(const std::string& theTypeName, const std::string& theName);
   virtual ~DataReader();
   DataReader(const DataReader& dr);
   DataReader& operator=(const DataReader& dr);

   // File interfaces, override for FileReaders
   virtual bool SetStream(std::ifstream* aStream, const std::string &fname="");

   virtual const StringArray& GetSelectedFieldNames();
   virtual void SetSelectedFieldNames(const StringArray& selections);
   virtual const StringArray& GetSupportedFieldNames();

   virtual bool ReadData() = 0;
   virtual bool WasDataLoaded(const std::string &theField);
   virtual void ClearData();

   // Accessors; these are intentionally NOT virtual, and call virtual protected
   // methods intended for subclass overrides that actually access the data
   Real GetRealValue(const std::string& forField);
   Rvector6 GetReal6Vector(const std::string& forField);
   std::string GetStringValue(const std::string& forField);

   virtual bool         UsesCoordinateSystem(const std::string& forField);
   virtual std::string  GetCoordinateSystemName(const std::string& forField);
   virtual bool         UsesOrigin(const std::string& forField);
   virtual std::string  GetOriginName(const std::string& forField);
   virtual bool         UsesTimeSystem(const std::string& forField);
   virtual std::string  GetTimeSystemName(const std::string& forField);

   const std::string    GetObjectParameterName(const std::string& forField);
   readerDataType       GetReaderDataType(const std::string& forField);
protected:
   /// Array of the data fields the user has selected
   StringArray selectedFields;
   /// List of field names that the data reader supports
   StringArray supportedFields;
   /// Flag indicating is everything should be read (the default)
   bool readAllSupportedFields;
   /// Flag indicating if the data is available
   bool dataReady;
   /// Flag indicating is a new read clears previous data or overwrites it
   bool clearOnRead;

   /// Map of the user strings to the keys to find in the data source
   std::map<std::string, std::string> fileStringMap;
   /// Map of the user strings to the GMAT object field names
   std::map<std::string, std::string> objectStringMap;
   /// Map of the user strings to the GMAT object field names
   std::map<std::string, Integer> objectIDMap;
   /// Map of the user string to the data type for the data
   std::map<std::string, readerDataType> dataType;
   /// Map indicating if a valid read has happened
   std::map<std::string, bool> dataLoaded;


   /// Mapping for the Real data
   std::map <std::string, Real> realData;
   /// Mapping for the Rvector6 data
   std::map <std::string, Rvector6> rvector6Data;
   /// Mapping for the string data
   std::map <std::string, std::string> stringData;

   // Methods that subclasses may need to override.  Put any additional
   // processing in these methods
   virtual bool ParseRealValue(const Integer i, const std::string& theField) = 0;
   virtual bool ParseRvector6Value(const Integer i, const std::string& theField,
         const StringArray& fieldIdentifiers) = 0;
   virtual bool ParseStringValue(const Integer i, const std::string& theField) = 0;
   virtual Real GetRData(const std::string &forField);
   virtual Rvector6 GetRVectorData(const std::string &forField);
   virtual std::string GetSData(const std::string &forField);

   /// TBD: Go we need the GmatBase parameters in this internal object?
//   /// Parameter IDs
//   enum
//   {
//      SELECTED_FIELD_NAMES = GmatBaseParamCount,
//      SUPPORTED_FIELD_NAMES,
//      DataReaderParamCount,
//   };
//
//   /// Interface parameter types
//   static const Gmat::ParameterType PARAMETER_TYPE[DataReaderParamCount - GmatBaseParamCount];
//   /// Interface parameter labels
//   static const std::string PARAMETER_LABEL[DataReaderParamCount - GmatBaseParamCount];
};

#endif /* DataReader_hpp */
