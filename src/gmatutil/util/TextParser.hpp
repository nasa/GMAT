//$Id$
//------------------------------------------------------------------------------
//                              TextParser
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2006/08/10
//
/**
 * Declares the methods to parse text into parts.
 */
//------------------------------------------------------------------------------
#ifndef TextParser_hpp
#define TextParser_hpp

#include "utildefs.hpp"

namespace Gmat
{
   enum BlockType
   {
      COMMENT_BLOCK,
      DEFINITION_BLOCK,
      COMMAND_BLOCK,
      ASSIGNMENT_BLOCK,
      FUNCTION_BLOCK,
      INCLUDE_BLOCK,
      METHOD_BLOCK,
   };
};


class GMATUTIL_API TextParser
{
public:

   TextParser();
   ~TextParser();

   // inline methods
   std::string GetPrefaceComment() { return prefaceComment; }
   std::string GetInlineComment() { return inlineComment; }
   std::string GetInstruction() { return theInstruction; }
   
   void Initialize(const StringArray &commandList);
   StringArray& GetChunks() { return theChunks; }
   bool IsFunctionCall() { return isFunctionCall; }
   
   void Reset();
   
   // For treating Include line as comment
   void PrependIncludeComment(const std::string &incLine);
   
   // for parsing
   Gmat::BlockType EvaluateBlock(const std::string &logicalBlock);
   StringArray DecomposeBlock(const std::string &logicalBlock);
   StringArray ChunkLine();
   
   StringArray Decompose(const std::string &chunk,
                         const std::string &bracketPair,
                         bool checkForArray = true,
                         bool removeOuterBracket = false);
   StringArray SeparateBrackets(const std::string &chunk,
                                const std::string &bracketPair,
                                const std::string &delim,
                                bool checkOuterBracket = true);
   StringArray SeparateAllBrackets(const std::string &chunk,
                                   const std::string &bracketPair);
   StringArray SeparateSpaces(const std::string &chunk);
   StringArray SeparateDots(const std::string &chunk);
   StringArray SeparateBy(const std::string &chunk, const std::string &delim);
   
   
protected:
      
   bool IsCommand(const std::string &str);
   char GetClosingBracket(const char &openBracket);
   std::string TabToSpaceExceptStrings(const std::string& startingBlock);
   
private:
   
   std::string prefaceComment;
   std::string inlineComment;
   std::string theInstruction;
   std::string whiteSpace;
   StringArray theChunks;
   StringArray theCommandList;
   Gmat::BlockType theBlockType;
   bool isFunctionCall;
   char errorMsg[1024];
};


#endif // TextParser_hpp



