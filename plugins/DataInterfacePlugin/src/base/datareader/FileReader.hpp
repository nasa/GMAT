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
 * Definition for file based data readers
 */
//------------------------------------------------------------------------------

#ifndef FileReader_hpp
#define FileReader_hpp

#include "DataInterfaceDefs.hpp"
#include "DataReader.hpp"

/**
 * Defines the base class for DataReaders that retrieve data from a file
 */
class DATAINTERFACE_API FileReader: public DataReader
{
public:
   FileReader(const std::string& theTypeName, const std::string& theName);
   virtual ~FileReader();
   FileReader(const FileReader& fr);
   FileReader& operator=(const FileReader& fr);

   virtual bool SetStream(std::ifstream* aStream, const std::string &fname="");

protected:
   /// Name of the file, used for error reporting
   std::string filename;
   /// The file stream used to access the data
   std::ifstream *theStream;
   /// Buffer of lines read
   StringArray dataBuffer;

   bool ReadLine(std::string &theLine);
   virtual bool ParseRealValue(const Integer i, const std::string& theField);
   virtual bool ParseRvector6Value(const Integer i, const std::string& theField,
                                   const StringArray& fieldIdentifiers);
   virtual bool ParseStringValue(const Integer i, const std::string& theField);


   /// TBD: Go we need the GmatBase parameters in this internal object?
   /// Parameter IDs
//   enum
//   {
//      FILENAME = DataReaderParamCount,
//      FileReaderParamCount,
//   };
//
//   /// Spacecraft parameter types
//   static const Gmat::ParameterType PARAMETER_TYPE[FileReaderParamCount - DataReaderParamCount];
//   /// Spacecraft parameter labels
//   static const std::string PARAMETER_LABEL[FileReaderParamCount - DataReaderParamCount];
};

#endif /* FileReader_hpp */
