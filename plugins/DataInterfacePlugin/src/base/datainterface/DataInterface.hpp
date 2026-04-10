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
 * Definition of DataInterfaces
 */
//------------------------------------------------------------------------------

#ifndef DataInterface_hpp
#define DataInterface_hpp

#include "DataInterfaceDefs.hpp"
#include "Interface.hpp"
#include "DataReader.hpp"


/**
 * Base class for Interfaces that retrieve data that is passed into objects
 */
class DATAINTERFACE_API DataInterface: public Interface
{
public:
   DataInterface(const std::string &type, const std::string &name);
   virtual ~DataInterface();
   DataInterface(const DataInterface& di);
   DataInterface& operator=(const DataInterface& di);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
//   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const std::string &label) const;

   const StringArray&   GetSupportedFieldNames() const;
   bool                 WasDataLoaded(const std::string& forField);
   const std::string    GetObjectParameterName(const std::string& forField);
   DataReader::readerDataType                        GetReaderParameterType(const std::string& forField);

   virtual Integer      Open(const std::string &name = "");
   virtual bool         LoadData() = 0;
   virtual Integer      Close(const std::string &name = "");

   // Accessors; these are intentionally NOT virtual, and call virtual protected
   // methods intended for subclass overrides that actually access the data
   Real GetRealValue(const std::string& forField);
   Rvector6 GetReal6Vector(const std::string& forField);
   std::string GetStringValue(const std::string& forField);

   // Methods for fields that have associated items: coordinate systems,
   // time systems, etc
   bool                 UsesCoordinateSystem(const std::string& forField);
   std::string          GetCoordinateSystemName(const std::string& forField);
   bool                 UsesOrigin(const std::string& forField);
   std::string          GetOriginName(const std::string& forField);
   bool                 UsesTimeSystem(const std::string& forField);
   std::string          GetTimeSystemName(const std::string& forField);

protected:
   /// Format identifier for the Reader this interface uses
   std::string readerFormat;
   /// The reader that this interface uses
   DataReader *theReader;
   /// List of data types the interface supports
   StringArray supportedFormats;

   /// Parameter IDs
   enum
   {
      FORMAT = InterfaceParamCount,
      SELECTED_FIELD_NAMES,
      SUPPORTED_FIELD_NAMES,        // Read only list of parms
      DataInterfaceParamCount,
   };

   /// Spacecraft parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[DataInterfaceParamCount - InterfaceParamCount];
   /// Spacecraft parameter labels
   static const std::string PARAMETER_LABEL[DataInterfaceParamCount - InterfaceParamCount];
};

#endif /* DataInterface_hpp */
