//------------------------------------------------------------------------------
//                                  CCSDSOEMSegment
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
// Created: 2016.01.07
//
/**
 * Stores, validates, and manages meta data and cartesion orbit vector read from,
 * or to be written to, a CCSDS Orbit Ephemeris Message file.
 *
 */
//------------------------------------------------------------------------------
#ifndef CCSDSOEMSegment_hpp
#define CCSDSOEMSegment_hpp

#include "utildefs.hpp"
#include "Rvector6.hpp"
#include "CCSDSEMSegment.hpp"

class GMATUTIL_API CCSDSOEMSegment : public CCSDSEMSegment
{
public:
   /// class methods
   CCSDSOEMSegment(const std::string version = "1.0", Integer segNum = 0);
   CCSDSOEMSegment(const CCSDSOEMSegment &copy);
   CCSDSOEMSegment& operator=(const CCSDSOEMSegment &copy);
   
   virtual ~CCSDSOEMSegment();
   
   virtual CCSDSEMSegment*  Clone() const;
   
   virtual bool         Validate(bool checkData = true);
   virtual Rvector6     GetState(Real atEpoch);
   virtual std::string  GetMetaDataForWriting();
   
   virtual bool         SetMetaData(const std::string &fieldName,
                                    const std::string &value);
   
   virtual bool         SetMetaDataForWriting(const std::string &fieldName,
                                              const std::string &value);
   virtual bool         AddData(Real epoch, Rvector data,
                                bool justCheckDataSize = false);
   virtual bool         AddDataForWriting(Real epoch, Rvector &data);
   
   virtual void         ClearMetaData();
   
protected:
   
   virtual Rvector          Interpolate(Real atEpoch);
};

#endif // CCSDSOEMSegment_hpp
