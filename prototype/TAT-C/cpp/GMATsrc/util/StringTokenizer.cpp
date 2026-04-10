//$Id$
//-------------------------------------------------------------------------------
//                              StringTokenizer
//-------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------

#include "StringTokenizer.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_STRING_TOKENIZER 1
//#define DEBUG_STRING_TOKENIZER_PARSE 1

//-------------------------------------
// public methods
//-------------------------------------

//-------------------------------------------------------------------------------
//  StringTokenizer()
//-------------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 * 
 */
//-------------------------------------------------------------------------------
StringTokenizer::StringTokenizer()
{
   delimiters = " ";
   countTokens = 0;
}


//-------------------------------------------------------------------------------
//  StringTokenizer(const std::string &str, const std::string &delim)
//-------------------------------------------------------------------------------
/**
 * Creates constructor with parameters.
 *
 * @param <str>   Given String
 * @param <delim> Given delimiters
 * 
 */
//-------------------------------------------------------------------------------
StringTokenizer::StringTokenizer(const std::string &str, const std::string &delim)
{
   #if DEBUG_STRING_TOKENIZER
   MessageInterface::ShowMessage
      ("StringTokenizer() str=<%s>, delim=<%s>\n", str.c_str(), delim.c_str());
   #endif
   
   delimiters = delim;
   Parse(str);
}


//-------------------------------------------------------------------------------
//  StringTokenizer(const std::string &str, const std::string &delim,
//                  bool insertDelim)
//-------------------------------------------------------------------------------
/**
 * Creates constructor with parameters.
 *
 * @param <str>   Given String
 * @param <delim> Given delimiters
 * @param <insertDelim> true if inserting delimiter back to tokenized string.
 *                      it will insert if delimiter size is 1
 *
 * @note:
 * I want to have constructor with
 * StringTokenizer(const std::string &str, const std::string &delim,
 *                  bool insertDelim = false)
 * but this causing some numeric differences when routine test is running
 *
 */
//-------------------------------------------------------------------------------
StringTokenizer::StringTokenizer(const std::string &str, const std::string &delim,
                                 bool insertDelim)
{
   #if DEBUG_STRING_TOKENIZER
   MessageInterface::ShowMessage
      ("StringTokenizer() str='%s', delim='%s', insertDelim=%d\n", str.c_str(),
       delim.c_str(), insertDelim);
   #endif
   
   delimiters = delim;
   Parse(str, insertDelim);
}


//-------------------------------------------------------------------------------
//  ~StringTokenizer()
//-------------------------------------------------------------------------------
/**
 * Destructor.
 * 
 */
//-------------------------------------------------------------------------------
StringTokenizer::~StringTokenizer()
{
}

//-------------------------------------------------------------------------------
//  Integer CountTokens() const
//-------------------------------------------------------------------------------
/**
 * Get the number of tokens.
 *
 * @return number of tokens. 
 * 
 */
//-------------------------------------------------------------------------------
Integer StringTokenizer::CountTokens() const
{
   return countTokens;
}

//-------------------------------------------------------------------------------
//  std::string GetToken(const Integer loc) 
//-------------------------------------------------------------------------------
/**
 * Get the string from specifiying the token number.
 *
 * @param <loc>   Specified token number
 * @return        return string in the specified token number 
 * 
 */
//-------------------------------------------------------------------------------
std::string StringTokenizer::GetToken(const Integer loc)  const
{
   if (loc <= countTokens)
      return stringTokens.at(loc);

   return std::string("");
}

//-------------------------------------------------------------------------------
//  std::vector<string> GetAllTokens() const;
//-------------------------------------------------------------------------------
/**
 * Get all token string. 
 *
 * @return       all token strings 
 * 
 */
//-------------------------------------------------------------------------------
const StringArray& StringTokenizer::GetAllTokens() const 
{
   return stringTokens; 
}


//-------------------------------------------------------------------------------
//  void Set(const std::string &str, const std::string &delim) 
//-------------------------------------------------------------------------------
/**
 * Set the string token.
 *
 * @param <str>   given string
 * @param <delim> given delimiters 
 * 
 */
//-------------------------------------------------------------------------------
void StringTokenizer::Set(const std::string &str, const std::string &delim)
{
   #if DEBUG_STRING_TOKENIZER
   MessageInterface::ShowMessage
      ("StringTokenizer::Set() str=<%s>, delim=<%s>\n", str.c_str(), delim.c_str());
   #endif
   
   stringTokens.clear();
   delimiters = delim;
   Parse(str);
}

//-------------------------------------------------------------------------------
//  void Set(const std::string &str, const std::string &delim) 
//-------------------------------------------------------------------------------
/**
 * Set the string token.
 *
 * @param <str>   given string
 * @param <delim> given delimiters 
 * 
 */
//-------------------------------------------------------------------------------
void StringTokenizer::Set(const std::string &str, const std::string &delim,
                          bool insertDelim)
{
   #if DEBUG_STRING_TOKENIZER
   MessageInterface::ShowMessage
      ("StringTokenizer::Set() str='%s', delim='%s', insertDelim=%d\n", str.c_str(),
       delim.c_str(), insertDelim);
   #endif
   
   stringTokens.clear();
   delimiters = delim;
   Parse(str, insertDelim);
}


//-------------------------------------
// private methods
//-------------------------------------

//-------------------------------------------------------------------------------
//  void Parse(const std::string &str)
//-------------------------------------------------------------------------------
/**
 * Parse out the string for token
 *
 * @param <str>   Given string
 *
 */
//-------------------------------------------------------------------------------
void StringTokenizer::Parse(const std::string &str)
{
   #if DEBUG_STRING_TOKENIZER_PARSE
   MessageInterface::ShowMessage
      ("StringTokenizer::Parse() str=%s, delimiters=<%s>, insertDelim=%d\n",
       str.c_str(), delimiters.c_str(), insertDelim);
   #endif
   
   std::string::size_type pos, lastPos;
   std::string substr;
   
   // Skip delimiters at beginning.
   lastPos = str.find_first_not_of(delimiters, 0);
   
   // Find first delimiter.
   pos = str.find_first_of(delimiters, lastPos);
   
   #if DEBUG_STRING_TOKENIZER_PARSE > 1
   MessageInterface::ShowMessage("   lastPos=%u, pos=%u\n", lastPos, pos);
   #endif
   
   if (pos == str.npos)
   {
      stringTokens.push_back(str);
   }
   else
   {
      while (pos != str.npos || lastPos != str.npos)
      {
         #if DEBUG_STRING_TOKENIZER_PARSE > 1
         MessageInterface::ShowMessage
            ("   lastPos=%u, pos=%u\n", lastPos, pos);
         #endif
         
         // Found a token, add it to the vector.
         substr = str.substr(lastPos, pos - lastPos);
         
         #if DEBUG_STRING_TOKENIZER_PARSE > 1
         MessageInterface::ShowMessage("   substr=<%s>\n", substr.c_str());
         #endif

         
         stringTokens.push_back(str.substr(lastPos, pos - lastPos));
         
         // Skip delimiters.  Note the "not_of"
         lastPos = str.find_first_not_of(delimiters, pos);
         
         // Find next "delimiter"
         pos = str.find_first_of(delimiters, lastPos);
      }
   }
   
   // Set the number of token found
   countTokens = stringTokens.size();
   
   #if DEBUG_STRING_TOKENIZER_PARSE
   MessageInterface::ShowMessage("   countTokens=%d\n", countTokens);
   #endif
   
}


//-------------------------------------------------------------------------------
//  void Parse(const std::string &str)
//-------------------------------------------------------------------------------
/**
 * Parse out the string for token
 *
 * @param <str>   Given string
 *
 */
//-------------------------------------------------------------------------------
void StringTokenizer::Parse(const std::string &str, bool insertDelim)
{
   #if DEBUG_STRING_TOKENIZER_PARSE
   MessageInterface::ShowMessage
      ("StringTokenizer::Parse() str=%s, delimiters=<%s>, insertDelim=%d\n",
       str.c_str(), delimiters.c_str(), insertDelim);
   #endif
   
   std::string::size_type pos, lastPos;
   std::string substr;
   
   // Skip delimiters at beginning.
   lastPos = str.find_first_not_of(delimiters, 0);
   
   // Find first delimiter.
   pos = str.find_first_of(delimiters, lastPos);
   
   #if DEBUG_STRING_TOKENIZER_PARSE > 1
   MessageInterface::ShowMessage("   lastPos=%u, pos=%u\n", lastPos, pos);
   #endif
   
   if (pos == str.npos)
   {
      stringTokens.push_back(str);
   }
   else
   {
      while (pos != str.npos || lastPos != str.npos)
      {
         #if DEBUG_STRING_TOKENIZER_PARSE > 1
         MessageInterface::ShowMessage
            ("   lastPos=%u, pos=%u\n", lastPos, pos);
         #endif
         
         // Found a token, add it to the vector.
         substr = str.substr(lastPos, pos - lastPos);
         
         #if DEBUG_STRING_TOKENIZER_PARSE > 1
         MessageInterface::ShowMessage("   substr=<%s>\n", substr.c_str());
         #endif

         
         ////stringTokens.push_back(str.substr(lastPos, pos - lastPos));
         stringTokens.push_back(str.substr(lastPos, pos - lastPos + 1));
         
         // Skip delimiters.  Note the "not_of"
         lastPos = str.find_first_not_of(delimiters, pos);
         
         // Find next "delimiter"
         pos = str.find_first_of(delimiters, lastPos);
      }
   }
   
   // Set the number of token found
   countTokens = stringTokens.size();
   
   #if DEBUG_STRING_TOKENIZER_PARSE
   MessageInterface::ShowMessage("   countTokens=%d\n", countTokens);
   #endif
   
}
