//$Id$
//------------------------------------------------------------------------------
//                              StringTokenizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2004/05/14
//
/**
 * Definition of the StringTokenizer class base
 */
//------------------------------------------------------------------------------

#ifndef StringTokenizer_hpp
#define StringTokenizer_hpp

#include "utildefs.hpp"

class GMATUTIL_API StringTokenizer
{
public:
   StringTokenizer();
   StringTokenizer(const std::string &str, const std::string &delim);
   StringTokenizer(const std::string &str, const std::string &delim,
                   bool insertDelim);
   ~StringTokenizer();
   
   // inline methods
   void SetDelimiters(const std::string &delim) { delimiters = delim; }
   std::string GetDelimiters() { return delimiters; }
   
   Integer CountTokens() const; 
   std::string GetToken(const Integer loc) const;
   const StringArray& GetAllTokens() const;
   
   void Set(const std::string &str, const std::string &delim);
   void Set(const std::string &str, const std::string &delim, bool insertDelim);
   
private:
   
   StringArray  stringTokens;
   std::string  delimiters;
   Integer      countTokens;
   
   void Parse(const std::string &str);
   void Parse(const std::string &str, bool insertDelim);
   
};

#endif // StringTokenizer_hpp
