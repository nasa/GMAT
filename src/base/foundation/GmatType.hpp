//------------------------------------------------------------------------------
//                           GmatType
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under purchase
// order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 2, 2017
/**
 * Implements the run time type system used in GMAT
 */
//------------------------------------------------------------------------------


#ifndef GmatType_hpp
#define GmatType_hpp

#include "gmatdefs.hpp"
#include <map>

/**
 * GMATTypeMap is a LOW OVERHEAD singleton used to map between type IDs and names
 */
class GMAT_API GmatType
{
public:
   static GmatType* Instance();

   static void RegisterType(const UnsignedInt id, const std::string& label);
   static UnsignedInt RegisterType(const std::string& label);
   static UnsignedInt GetTypeId(const std::string label);
   static std::string GetTypeName(const UnsignedInt id);

private:
   /// The singleton
   static GmatType *mapper;

   // The map from ID to string
   std::map <UnsignedInt, std::string> idToString;
   // The map from string to ID
   std::map <std::string, UnsignedInt> stringToId;
   // The next value for iser registered types
   UnsignedInt nextUserId = Gmat::USER_DEFINED_OBJECT;

   // Bury the constructor and destructor: this is a singleton
   GmatType();
   ~GmatType();
};

#endif /* GmatType_hpp */
