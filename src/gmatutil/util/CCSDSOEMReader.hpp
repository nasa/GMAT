//------------------------------------------------------------------------------
//                                  CCSDSOEMReader
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
// Author: Tuan D. Nguyen / NASA
// Created: July 7, 2021
//
/**
 * Reads a CCSDS Orbit Ephemeris Message file, and manages segments.
 */
//------------------------------------------------------------------------------
#ifndef CCSDSOEMReader_hpp
#define CCSDSOEMReader_hpp

#include "utildefs.hpp"
#include "CCSDSEMReader.hpp"
#include "CCSDSOEMSegment.hpp"
#include "Rvector6.hpp"

class GMATUTIL_API CCSDSOEMReader : public CCSDSEMReader
{
public:
   /// class methods
   CCSDSOEMReader();
   CCSDSOEMReader(const CCSDSOEMReader &copy);
   CCSDSOEMReader& operator=(const CCSDSOEMReader &copy);

   virtual ~CCSDSOEMReader();

   virtual CCSDSEMReader*  Clone() const;

   /// Initialize will parse the file, initialize and validate each
   /// segment, and check to make sure the segments are ordered
   /// correctly by increasing time
   virtual void            Initialize();
   /// Returns the DCM from inertial-to-body given the input epoch
   //virtual Rmatrix33       GetState(Real atEpoch);
   virtual Rvector6        GetState(Real atEpoch);

protected:
   static const std::string META_START;
   static const std::string META_STOP;
   static const std::string COVARIANCE_START;
   static const std::string COVARIANCE_STOP;

   /// Required header fields
   /// none

   /// Other data
   /// none

   /// Create and return a new segment of the correct type
   virtual CCSDSOEMSegment* CreateNewSegment(const std::string version,
                                             const Integer segNum, 
                                             const std::string &ofType = "ANY");

   /// Check to see if the version number on the file is valid for this type of
   // ephemeris message file
   virtual bool            IsValidVersion(const std::string &versionValue) const;

   virtual bool            ParseFile();
};

#endif // CCSDSOEMReader_hpp
