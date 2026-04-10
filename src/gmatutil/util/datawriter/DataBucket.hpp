//$Id$
//------------------------------------------------------------------------------
//                           DataBucket
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 20, 2016
/**
 * Helper class used to collect data to be written using a DataWriter
 */
//------------------------------------------------------------------------------

#ifndef DataBucket_hpp
#define DataBucket_hpp

#include "utildefs.hpp"


/**
 * A generic container used to store data for use by a DataWriter
 *
 * All members here are public to facilitate fast access
 */
class GMATUTIL_API DataBucket
{
public:
   DataBucket();
   virtual ~DataBucket();
   DataBucket(const DataBucket& db);
   DataBucket& operator=(const DataBucket& db);

   void SetFillToMatch(bool matchFill);
   void SetInitialRealValue(Real val);
   void SetInitialStringValue(std::string val);

   Integer AddRealContainer(const std::string &name, UnsignedInt numElements = 1U);
   Integer FindRealContainer(const std::string &name);
   Integer GetRealContainerSize(UnsignedInt index);
   Integer AddReal3DContainer(const std::string &name);
   Integer FindReal3DContainer(const std::string &name);

   Integer AddReal2DArray(const std::string &name);
   Integer FindReal2DArray(const std::string &name);

   Integer AddStringContainer(const std::string &name, UnsignedInt numElements = 1U);
   Integer FindStringContainer(const std::string &name);
   Integer GetStringContainerSize(UnsignedInt index);
   Integer AddString3DContainer(const std::string &name);
   Integer FindString3DContainer(const std::string &name);

   Integer AddString2DArray(const std::string &name);
   Integer FindString2DArray(const std::string &name);

   Integer AddPoint();
   Integer GetContainerSize();
   void Clear();

   /// Vector to track state for the data, so empty elements can be detected
   RealArray   elementStatus;
   /// Names of the real data collected
   StringArray realNames;
   /// The real data
   std::vector<std::vector<RealArray>> realValues;
   /// The size of each real data
   UnsignedIntArray realValueSize;
   /// Names of the real data collected for the 3D container
   StringArray real3DNames;
   /// The real data for the 3D container
   std::vector<std::vector<std::vector<RealArray>>> real3DValues;
   /// Names of the real 2D array data collected
   StringArray real2DArrayNames;
   /// The real 2D array data
   std::vector<std::vector<RealArray>> real2DArrayValues;
   /// Names of the string data collected
   StringArray stringNames;
   /// The string data
   std::vector<std::vector<StringArray>> stringValues;
   /// The size of each string data
   UnsignedIntArray stringValueSize;
   /// Names of the string data collected for the 3D container
   StringArray string3DNames;
   /// The string data for the 3D container
   std::vector<std::vector<std::vector<StringArray>>> string3DValues;
   /// Names of the string data 2D array collected
   StringArray string2DArrayNames;
   /// The string 2D array data
   std::vector<std::vector<StringArray>> string2DArrayValues;

protected:

   bool fillToMatch;
   Real initialRealValue;
   std::string initialStringValue;
};

#endif /* DataBucket_hpp */
