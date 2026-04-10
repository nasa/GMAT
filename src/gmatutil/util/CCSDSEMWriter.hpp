//------------------------------------------------------------------------------
//                                  CCSDSEMWriter
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
// Author: Linda Jun / NASA
// Created: 2016.01.13
//
/**
 * Writes a CCSDS Ephemeris Message to a file.
 * This is the abstract base class from which other classes must derive (e.g.
 * a class to handle attitude files).
 * @todo Add assumptions and general file requirements here
 */
//------------------------------------------------------------------------------
#ifndef CCSDSEMWriter_hpp
#define CCSDSEMWriter_hpp

#include "Rvector.hpp"
#include "utildefs.hpp"
#include <fstream>
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton

class GMATUTIL_API CCSDSEMWriter
{
public:
   /// class methods
   CCSDSEMWriter();
   CCSDSEMWriter(const CCSDSEMWriter &copy);
   CCSDSEMWriter& operator=(const CCSDSEMWriter &copy);
   
   virtual ~CCSDSEMWriter();
   
   virtual CCSDSEMWriter*  Clone() const = 0;
   
   /// Open the EM file for writing
   virtual bool         OpenFile(const std::string &filename);
   virtual bool         SetHeaderForWriting(const std::string &fieldName,
                                            const std::string &value);   
   virtual bool         WriteHeader(const std::string &versionFieldName);
   virtual bool         WriteBlankLine();
   virtual bool         WriteString(const std::string &str);
   virtual void         ClearHeaderComments();
   virtual void         ClearHeader();
   
   /// Methods subclasses should provide
   virtual bool         WriteMetaData() = 0;
   virtual bool         WriteDataComments() = 0;
   virtual bool         WriteDataSegment() = 0;
   
   virtual bool         SetMetaDataForWriting(const std::string &fieldName,
                                              const std::string &value) = 0;
   virtual bool         AddMetaComment(const std::string& comment) = 0;
   virtual bool         AddDataComment(const std::string& comment) = 0;
   virtual bool         AddDataForWriting(Real epoch, Rvector &data) = 0;
   
   virtual void         ClearMetaComments() = 0;
   virtual void         ClearDataComments() = 0;
   virtual void         ClearMetaData() = 0;
   virtual void         ClearDataStore() = 0;

   virtual bool         SetWritingAccelerationOption(const bool writeOption);
   virtual bool         SetWritingCovarianceOption(const bool writeOption);
   
protected:

   /// Required header fields
   std::string versionNumber;
   std::string originator;
   std::string creationTime;
   
   /// Writing options
   bool writeAcceleration;
   bool writeCovariance;

   /// Optional header field
   StringArray headerComments;
   
   /// The file name
   std::string emFileName;
   
   /// output data stream
   std::ofstream emOutStream;
   
   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;

   /// Time conversion
   std::string A1ModJulianToUtcGregorian(Real epochInDays, Integer format);
};

#endif // CCSDSEMWriter_hpp
