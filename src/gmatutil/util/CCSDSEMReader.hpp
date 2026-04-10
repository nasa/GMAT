//------------------------------------------------------------------------------
//                                  CCSDSEMReader
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
 * Reads a CCSDS Ephemeris Message file, and manages segments.
 * This is the abstract base class from which other classes must derive (e.g.
 * a class to handle attitude files).
 * @todo Add assumptions and general file requirements here
 */
//------------------------------------------------------------------------------
#ifndef CCSDSEMReader_hpp
#define CCSDSEMReader_hpp

#include <fstream>
#include "CCSDSEMSegment.hpp"
#include "Rvector.hpp"
#include "utildefs.hpp"

class GMATUTIL_API CCSDSEMReader
{
public:
   /// class methods
   CCSDSEMReader();
   CCSDSEMReader(const CCSDSEMReader &copy);
   CCSDSEMReader& operator=(const CCSDSEMReader &copy);

   virtual ~CCSDSEMReader();

   virtual CCSDSEMReader*  Clone() const = 0;

   /// Initialize will parse the file, initialize and validate each
   /// segment, and check to make sure the segments are ordered
   /// correctly by increasing time
   virtual void            Initialize();
   virtual bool            IsInitialized();

   /// Set the EM file name
   virtual bool            SetFile(const std::string &theEMFile);

   /// GetState method should be added to child class, to return
   /// data in needed form
   virtual std::string     GetVersion();
   virtual std::string     GetOriginator();
   virtual std::string     GetCreationDate();
   virtual Integer         GetNumberOfSegments();

   /// Returns the current segment
   virtual CCSDSEMSegment* GetSegment();
   /// Returns the segment number requested
   virtual CCSDSEMSegment* GetSegment(Integer num);
   /// Returns the segment number for the segment containing the
   /// requested time (using usable start/stop time if they exist,
   /// otherwise using start and stop time)
   virtual Integer         GetSegmentNumber(Real epoch);
   /// Returns the segment that contains the epoch specified
   /// (using usable start/stop time if they exist; otherwise,
   /// using start and stop time)
   virtual CCSDSEMSegment* GetSegment(Real epoch);
   /// Parse the file, validating where possible, and creating the appropriate
   /// segments to hold the meta data and ephemeris data

protected:

   // These static constants were moved to child class because different EM 
   // type of data files have different syntax. As a result,it has 
   // different parsing process. 
   //static const std::string META_START;
   //static const std::string META_STOP;
   //static const std::string DATA_START;
   //static const std::string DATA_STOP;

   // Required header fields
   /// The actual version number field name will vary depending on which type
   /// of Ephemeris Message we are reading. The expected field name is declared
   /// separately and this data field is set in the appropriate base class.
   std::string versionNumber;

   // GMAT stores input for the following three fields, but does not currently
   // use the data.  NOTE: GMAT stores the creation date as a string - if
   // this is used later, it must be parsed to get the Real date
   std::string originator;
   std::string creationDate;
   StringArray comments;

   // The file name
   std::string emFile;
   // The size of the data needed
   Integer     dataSize;

   /// Has the file been read and the segments initialized and validated?
   bool        isInitialized;
   /// Has the version number been found?
   bool        versionFound;
   /// Has there been a non-comment line after the version number?
   bool        nonCommentFound;

   // Child classes must set the next three fields:
   /// What is the expected field name for the file version number, which
   /// is required to be defined in the first non-blank line of the file
   std::string versionFieldName;
   /// Does this type of Ephemeris Message file specify a type of data in the
   /// meta data
   bool        metaSpecifiesType;
   /// The meta data field (all CAPS) that specifies the type of data
   std::string metaDataTypeField;

   /// The type of data specified in the meta data
   std::string dataType;
   /// The current segment that we are using/setting/querying
   CCSDSEMSegment *currentSegment;

   /// the number of segments
   Integer        numSegments;
   /// in stream
   std::ifstream ephFile;

   // Store a vector of segment pointers
   std::vector<CCSDSEMSegment*>   segments;

   // Buffer meta data when we read it in, since we don't know what type of
   /// segment to create until we see the type specified in the meta data
   /// (if metaSpecifiesType is true)
   std::map<std::string, std::string> metaMap;

   std::map<std::string, std::string> covarianceMetaMap;

   /// Create and return a new segment of the correct type
   virtual CCSDSEMSegment* CreateNewSegment(const std::string versionValue, 
                                            const Integer segNum,
                                            const std::string &ofType = "ANY") = 0;
   /// Check to see if the version number on the file is valid for this type of
   // ephemeris message file
   virtual bool            IsValidVersion(const std::string &versionValue) const = 0;

   virtual bool            ParseFile() = 0;


private:
   void ReadLine(std::ifstream& ephFile, std::string&line);
   std::string LeftTrim(std::string str);
};

#endif // CCSDSEMReader_hpp
