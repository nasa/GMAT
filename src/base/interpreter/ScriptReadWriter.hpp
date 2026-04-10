//$Id$
//------------------------------------------------------------------------------
//                           ScriptReadWriter
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
// Author: Allison Greene
// Created: 2006/07/10
//
/**
 * Implements reading and writing a script file.
 */
//------------------------------------------------------------------------------

#ifndef SCRIPTREADWRITER_HPP_
#define SCRIPTREADWRITER_HPP_

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include <iostream>

class GMAT_API ScriptReadWriter 
{
public:
   static ScriptReadWriter* Instance();
   ~ScriptReadWriter();
   
   void SetInStream(std::istream *is);
   void SetOutStream(std::ostream *os) { outStream = os; }
   void SetIsParsingIncludeFile(bool flag);
   bool GetIsParsingIncludeFile();
   
   Integer GetLineWidth();
   void SetLineWidth(Integer width);
   
   Integer GetLineNumber();
   Integer GetIncludeLineNumber();
   std::string GetCurrentLine() { return currentLine; }
   
   void ReadFirstBlock(std::string &header, std::string &firstBlock,
                       bool skipHeader = false);
   std::string ReadLogicalBlock();
   bool WriteText(const std::string &textToWrite);
      
protected:

private:

   // These data are not created here
   std::istream *inStream;
   std::ostream *outStream;
   
   std::string currentLine;
   
   Integer lineWidth;
   Integer currentLineNumber;
   bool writeGmatKeyword;
   bool reachedEndOfFile;
   bool readFirstBlock;
   bool isParsingIncludeFile;
   Integer currentIncludeFileLineNumber;
   
   bool Initialize();
   std::string CrossPlatformGetLine();
   bool IsComment(const std::string &text);
   bool IsBlank(const std::string &text);
   bool HasEllipse(const std::string &text);
   std::string HandleEllipsis(const std::string &text);
   //std::string HandleComments(const std::string &text);
   
   static ScriptReadWriter *instance;   
   static const std::string sectionDelimiter;
   static const std::string ellipsis;
};

#endif /*SCRIPTREADWRITER_HPP_*/
