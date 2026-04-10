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

#include "ScriptReadWriter.hpp"
#include "InterpreterException.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include <string.h>
#include <sstream>

ScriptReadWriter* ScriptReadWriter::instance = NULL;
const std::string ScriptReadWriter::sectionDelimiter = "%--------";
const std::string ScriptReadWriter::ellipsis = "...";

//#define DEBUG_SCRIPT_READ
//#define DEBUG_FIRST_BLOCK
//#define DEBUG_ELLIPSIS
//#define DEBUG_COMMENT

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// static Instance()
//------------------------------------------------------------------------------
ScriptReadWriter* ScriptReadWriter::Instance()
{
   if (instance == NULL)
   {
      instance = new ScriptReadWriter();
      instance->Initialize();
   }
   
   return instance;
}


//------------------------------------------------------------------------------
// ~ScriptReadWriter()
//------------------------------------------------------------------------------
ScriptReadWriter::~ScriptReadWriter()
{
}


//------------------------------------------------------------------------------
// void SetInStream(std::istream *is)
//------------------------------------------------------------------------------
void ScriptReadWriter::SetInStream(std::istream *is)
{
   inStream = is; 
   reachedEndOfFile = false;
   readFirstBlock = false;
   if (!isParsingIncludeFile)
       currentLineNumber = 0;
   else
       currentIncludeFileLineNumber = 0;
}

//------------------------------------------------------------------------------
// void SetIsParsingIncludeFile(bool flag);
//------------------------------------------------------------------------------
void ScriptReadWriter::SetIsParsingIncludeFile(bool flag)
{
    isParsingIncludeFile = flag;
}

//------------------------------------------------------------------------------
// bool GetIsParsingIncludeFile(bool flag);
//------------------------------------------------------------------------------
bool ScriptReadWriter::GetIsParsingIncludeFile()
{
    return isParsingIncludeFile;
}

//------------------------------------------------------------------------------
// Integer GetLineWidth()
//------------------------------------------------------------------------------
Integer ScriptReadWriter::GetLineWidth()
{
   return lineWidth;
}

//------------------------------------------------------------------------------
// void SetLineWidth(Integer lineWidth)
//------------------------------------------------------------------------------
void ScriptReadWriter::SetLineWidth(Integer width)
{
   if ((width < 20) && (width != 0))
      throw InterpreterException
         ("Line width must either be unlimited (denoted by 0) or greater "
          "than 19 characters.\n");
   
   lineWidth = width;
}

//------------------------------------------------------------------------------
// Integer GetLineNumber()
//------------------------------------------------------------------------------
Integer ScriptReadWriter::GetLineNumber()
{
   return currentLineNumber;
}

//------------------------------------------------------------------------------
// Integer GetIncludeLineNumber()
//------------------------------------------------------------------------------
Integer ScriptReadWriter::GetIncludeLineNumber()
{
    return currentIncludeFileLineNumber;
}

//------------------------------------------------------------------------------
// void ReadFirstBlock(std::string &header, std::string &firstBlock,
//                     bool skipHeader = false)
//------------------------------------------------------------------------------
/*
 * Reads header and first preface comment and script from the script file.
 * The header block ends when first blank line is read.
 * The first block ends when first non-blank and non-comment line is read.
 * When skipHeader is true, it will read as first block. Usually skipping header
 * will be needed when interpreting ScriptEvent from the GUI.
 *
 * @param  header  Header comment lines read
 * @param  firstBlock  First preface comment and script read
 * @param  skipHeader Flag indicating first comment block is not a header(false)
 */
//------------------------------------------------------------------------------
void ScriptReadWriter::ReadFirstBlock(std::string &header, std::string &firstBlock,
                                      bool skipHeader)
{
   #ifdef DEBUG_FIRST_BLOCK
   MessageInterface::ShowMessage
      ("ScriptReadWriter::ReadFirstBlock() entered, skipHeader=%d\n", skipHeader);
   #endif
   
   std::string newLine = "";
   header = "";
   firstBlock = "";
   bool doneWithHeader = false;
   
   if (reachedEndOfFile)
   {
      #ifdef DEBUG_FIRST_BLOCK
      MessageInterface::ShowMessage
         ("ScriptReadWriter::ReadFirstBlock() leaving, end-of-file reached\n");
      #endif
      return;
   }
   
   // get 1 line of text
   newLine = CrossPlatformGetLine();
   
   #ifdef DEBUG_FIRST_BLOCK
   MessageInterface::ShowMessage
      ("ReadFirstBlock() firstLine=<<<%s>>>\n", newLine.c_str());
   #endif
   
   if (reachedEndOfFile && IsBlank(newLine))
   {
      #ifdef DEBUG_FIRST_BLOCK
      MessageInterface::ShowMessage
         ("ScriptReadWriter::ReadFirstBlock() leaving, end-of-file reached\n");
      #endif
      return;
   }
   
   //if line is not blank and is not comment line, return this line
   if (!IsBlank(newLine) && (!IsComment(newLine)))
   {
      if (HasEllipse(newLine))
      {
         // overwrite result with multiple lines separated by ellipsis
         newLine = HandleEllipsis(newLine);
         #ifdef DEBUG_FIRST_BLOCK
            MessageInterface::ShowMessage("After handling ellipses, newLine "
                  "= <<%s>>\n", newLine.c_str());
         #endif
      }
      firstBlock = newLine;
      #ifdef DEBUG_FIRST_BLOCK
      MessageInterface::ShowMessage
         ("ReadFirstBlock() header=<<<%s>>>\nfirstBlock=<<<%s>>>\n", header.c_str(),
          firstBlock.c_str());
      MessageInterface::ShowMessage
         ("ReadFirstBlock() newLine=<<<%s>>>\n", newLine.c_str());
      MessageInterface::ShowMessage("ScriptReadWriter::ReadFirstBlock() leaving\n");
      #endif
      return;
   }
   
   header = newLine + "\n";
   
   if (IsBlank(newLine))
      doneWithHeader = true;
   
   //-----------------------------------------------------------------
   // Read header comments
   // keep looping and append till we find blank line or end of file
   //-----------------------------------------------------------------
   if (!doneWithHeader)
   {
      while (!reachedEndOfFile)
      {
         newLine = CrossPlatformGetLine();
         
         #ifdef DEBUG_FIRST_BLOCK
         MessageInterface::ShowMessage
            ("   header newLine=<<<%s>>>\n", newLine.c_str());
         #endif
         
         // If non-blank and non-comment line found, return
         if (!IsBlank(newLine) && (!IsComment(newLine)))
         {
            if (HasEllipse(newLine))
            {
               // overwrite result with multiple lines separated  by ellipsis
               newLine = HandleEllipsis(newLine);
               #ifdef DEBUG_FIRST_BLOCK
                  MessageInterface::ShowMessage("After handling ellipses, newLine "
                        "= <<%s>>\n", newLine.c_str());
               #endif
            }

            firstBlock = newLine + "\n";
            
            if (skipHeader)
            {
               firstBlock = header + firstBlock;
               header = "";
            }
            
            #ifdef DEBUG_FIRST_BLOCK
            MessageInterface::ShowMessage
               ("ReadFirstBlock() non-blank and non-comment found\n"
                "header=<<<%s>>>\nfirstBlock=<<<%s>>>\n", header.c_str(),
                firstBlock.c_str());
            MessageInterface::ShowMessage
               ("ScriptReadWriter::ReadFirstBlock() leaving\n");
            #endif
            
            return;
         }
         
         // If blank line found, break
         if (IsBlank(newLine))
         {
            header = header + newLine + "\n";
            doneWithHeader = true;
            break;
         }
         
         header = header + newLine + "\n";
      }
   }
   
   
   //-----------------------------------------------------------------
   // Read first script
   // Keep looping and append till we find non-blank/non-comment line
   // or end of file
   //-----------------------------------------------------------------
   while (!reachedEndOfFile)
   {
      newLine = CrossPlatformGetLine();
      
      #ifdef DEBUG_FIRST_BLOCK
      MessageInterface::ShowMessage("   1stblk newLine=<<<%s>>>\n", newLine.c_str());
      #endif
      
      // If non-blank and non-comment line found, break
      if (!IsBlank(newLine) && (!IsComment(newLine)))
      {
         if (HasEllipse(newLine))
         {
            // overwrite result with multiple lines separated  by ellipsis
            newLine = HandleEllipsis(newLine);
            #ifdef DEBUG_FIRST_BLOCK
               MessageInterface::ShowMessage("After handling ellipses, newLine "
                     "= <<%s>>\n", newLine.c_str());
            #endif
         }

         firstBlock = firstBlock + newLine + "\n";
         break;
      }
      
      firstBlock = firstBlock + newLine + "\n";
   }
   
   if (skipHeader)
   {
      firstBlock = header + firstBlock;
      header = "";
   }
   
   #ifdef DEBUG_FIRST_BLOCK
   MessageInterface::ShowMessage
      ("ReadFirstBlock() header=<<<%s>>>\nfirstBlock=<<<%s>>>\n", header.c_str(),
       firstBlock.c_str());
   MessageInterface::ShowMessage
      ("ReadFirstBlock() newLine=<<<%s>>>\n", newLine.c_str());
   MessageInterface::ShowMessage("ScriptReadWriter::ReadFirstBlock() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// std::string ReadLogicalBlock()
//------------------------------------------------------------------------------
/*
 * Reads lines until non-blank and non-comment line from the input stream
 */
//------------------------------------------------------------------------------
std::string ScriptReadWriter::ReadLogicalBlock()
{
   #ifdef DEBUG_SCRIPT_READ
   MessageInterface::ShowMessage("ScriptReadWriter::ReadLogicalBlock() entered\n");
   #endif
   
   std::string result = "";
   std::string oneLine = ""; 
   std::string block = "";
   
   if (reachedEndOfFile)
      return "\0";
   
   // get 1 line of text
   oneLine = CrossPlatformGetLine();
   
   if (reachedEndOfFile && IsBlank(oneLine))
      return "\0";
   
   #ifdef DEBUG_SCRIPT_READ
   MessageInterface::ShowMessage
      ("ReadLogicalBlock() oneLine=\n<<<%s>>>\n", oneLine.c_str());
   #endif
   
   // keep looping till we find non-blank or non-comment line
   while ((!reachedEndOfFile) && (IsBlank(oneLine) || IsComment(oneLine)))
   {      
      block = block + oneLine + "\n";
      oneLine = CrossPlatformGetLine();
      
      #ifdef DEBUG_SCRIPT_READ
      MessageInterface::ShowMessage
         ("ReadLogicalBlock() oneLine=\n<<<%s>>>\n", oneLine.c_str());
      #endif
   }
   
   block = block + oneLine + "\n";
   
   #ifdef DEBUG_SCRIPT_READ
   MessageInterface::ShowMessage
      ("ReadLogicalBlock() block=\n<<<%s>>>\n", block.c_str());
   #endif
   
   result = block;
   
   if (HasEllipse(oneLine))
   {
      // overwrite result with multiple lines separated  by ellipsis
//      result = block + HandleEllipsis(oneLine);  <-- Resulted in first line repeating
      result = HandleEllipsis(oneLine);
   }
   
   readFirstBlock = true;
   
   #ifdef DEBUG_SCRIPT_READ
   MessageInterface::ShowMessage
      ("ScriptReadWriter::ReadLogicalBlock() returning:\n<<<%s>>>\n", result.c_str());
   #endif
   return result;
}


//------------------------------------------------------------------------------
// bool WriteText(const std::string &textToWrite)
//------------------------------------------------------------------------------
bool ScriptReadWriter::WriteText(const std::string &textToWrite)
{
   *outStream << textToWrite;
   outStream->flush();
   return true;
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool ScriptReadWriter::Initialize()
{
   lineWidth = 80;
   currentLineNumber = 0;
   currentIncludeFileLineNumber = 0;
   writeGmatKeyword = true;
   reachedEndOfFile = false;
   readFirstBlock = false;
   isParsingIncludeFile = false;
   
   return true;  // need to change so if something wasn't set to return false
}


//------------------------------------------------------------------------------
// std::string CrossPlatformGetLine()
//------------------------------------------------------------------------------
std::string ScriptReadWriter::CrossPlatformGetLine()
{
   char ch = '\0';
   std::string result;
   
   while (inStream->get(ch) && ch != '\r' && ch != '\n' && ch != '\0' &&
          !inStream->eof()) 
   {
      if (result.length() < 3)
      {
         // Test 1st 3 bytes for non-ANSI encoding -- anything with the top bit set
         if (ch < 0)
         {
            throw InterpreterException("Non-standard characters were "
                  "encountered in the script file; please check the file to "
                  "be sure it is saved as an ASCII file, and not formatted "
                  "for Unicode or UTF.");
         }
      }
      result += ch;
   }
   
   if ((ch == '\r') && (inStream->peek() == '\n'))
      inStream->get(ch);

   if ((ch == '\0') || (inStream->eof()))
   {
      reachedEndOfFile = true;
   }
   
   // Increment line counter for main body of script
   if (!isParsingIncludeFile)
       ++currentLineNumber;
   else
       ++currentIncludeFileLineNumber;

   currentLine = result;
   
   return result;
}


//------------------------------------------------------------------------------
// bool IsComment(const std::string &text)
//------------------------------------------------------------------------------
bool ScriptReadWriter::IsComment(const std::string &text)
{
   #ifdef DEBUG_COMMENT
   MessageInterface::ShowMessage
      ("ScriptReadWriter::IsComment() entered, text=\n<%s>\n", text.c_str());
   #endif
   std::string str = GmatStringUtil::Trim(text, GmatStringUtil::BOTH);
   bool isComment = GmatStringUtil::StartsWith(str, "%");
   // // Treat # sign as comment to preserve #Include position
   // bool hasSharpSign = GmatStringUtil::StartsWith(str, "#");
   // isComment = isComment || hasSharpSign;
   
   #ifdef DEBUG_COMMENT
   MessageInterface::ShowMessage
      ("ScriptReadWriter::IsComment() returnig %d\n", isComment);
   #endif
   return (isComment);
   //return GmatStringUtil::StartsWith(str, "%");
}


//------------------------------------------------------------------------------
// bool IsBlank(const std::string &text)
//------------------------------------------------------------------------------
bool ScriptReadWriter::IsBlank(const std::string &text)
{
   std::string str = GmatStringUtil::Trim(text, GmatStringUtil::BOTH);
   
   if (str == "")
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool HasEllipse(const std::string &text)
//------------------------------------------------------------------------------
bool ScriptReadWriter::HasEllipse(const std::string &text)
{
   #ifdef DEBUG_ELLIPSE
   MessageInterface::ShowMessage
      ("ScriptReadWriter::HasEllipse() entered,\n   text = <%s>\n", text.c_str());
   #endif
   
   std::string ellipsis = "...";

   int pos = text.find(ellipsis,0);
   bool ellipsisFound = false;
   
   if (pos < 0)
   {
      ellipsisFound = false;
   }
   else
   {
      std::string newStr = GmatStringUtil::Trim(text, GmatStringUtil::TRAILING, true);
      #ifdef DEBUG_ELLIPSE
      MessageInterface::ShowMessage("   newStr = <%s>\n", newStr.c_str());
      #endif
      if (GmatStringUtil::EndsWith(newStr, ellipsis))
         ellipsisFound = true;
      else
         ellipsisFound = false;
   }
   
   #ifdef DEBUG_ELLIPSE
   MessageInterface::ShowMessage
      ("ScriptReadWriter::HasEllipse() <%s> returning %d\n", text.c_str(), ellipsisFound);
   #endif
   return ellipsisFound;
}

//------------------------------------------------------------------------------
// std::string HandleEllipsis(const std::string &text)
//------------------------------------------------------------------------------
std::string ScriptReadWriter::HandleEllipsis(const std::string &text)
{
   #ifdef DEBUG_ELLIPSIS
   MessageInterface::ShowMessage
      ("ScriptReadWriter::HandleEllipsis() entered, text = <%s>\n", text.c_str());
   #endif
   
   std::string str = GmatStringUtil::Trim(text, GmatStringUtil::TRAILING);
   int pos = str.find(ellipsis,0);
   
   if (pos < 0)      // no ellipsis
   {
      #ifdef DEBUG_ELLIPSIS
      MessageInterface::ShowMessage
         ("ScriptReadWriter::HandleEllipsis() returning <%s>, no ellipsis found\n",
          str.c_str());
      #endif
      return str;
   }
   
   // make sure ellipsis is at the end of the line
   if ((int)(str.size())-3 != pos)
   {
      std::stringstream buffer;
      if (!isParsingIncludeFile)
          buffer << currentLineNumber;
      else
          buffer << currentIncludeFileLineNumber;
      throw InterpreterException("Script Line " + buffer.str() +
                                 "-->Ellipses must be at the end of the line\n" );
   }
   
   std::string result = "";
   
   while (pos >= 0)
   {
      if (pos == 0)     // ellipsis were on a line by themselves
        result += " ";
      else
      {
         result += str.substr(0, pos);  // add substring to first set
         result += " ";
      }
      
      // reset str string and position
      str = "";
      pos = -1;
      
      // read a line
      str = CrossPlatformGetLine();
      
      while (IsBlank(str) && !reachedEndOfFile)
         str = CrossPlatformGetLine();
      
      if (IsBlank(str) && reachedEndOfFile)
      {
         std::stringstream buffer;
         if (!isParsingIncludeFile)
             buffer << currentLineNumber;
         else
             buffer << currentIncludeFileLineNumber;
         throw InterpreterException("Script Line " + buffer.str() +
             "-->Prematurely reached the end of file.\n");
      }
      
      if (IsComment(str))
      {
         std::stringstream buffer;
         if (!isParsingIncludeFile)
             buffer << currentLineNumber;
         else
             buffer << currentIncludeFileLineNumber;
         throw InterpreterException("Script Line " + buffer.str() +
            "-->Comments are not allowed in the middle of a block\n");
      }
      
      str = GmatStringUtil::Trim(str, GmatStringUtil::TRAILING);      
      pos = str.find(ellipsis, 0);
   }
   
   // add the last line on to result
   if (IsComment(str))
   {
      std::stringstream buffer;
      if (!isParsingIncludeFile)
          buffer << currentLineNumber;
      else
          buffer << currentIncludeFileLineNumber;
      throw InterpreterException("Script Line " + buffer.str() +
         "-->Comments are not allowed in the middle of a block\n");
   }
   
   result += str;
   
   #ifdef DEBUG_ELLIPSIS
   MessageInterface::ShowMessage
      ("ScriptReadWriter::HandleEllipsis() returning <%s>\n", result.c_str());
   #endif
   
   return result;
}


// This method is not used
// //------------------------------------------------------------------------------
// // std::string HandleComments(const std::string &text)
// //------------------------------------------------------------------------------
// std::string ScriptReadWriter::HandleComments(const std::string &text)
// {
//    MessageInterface::ShowMessage
//       ("==> ScriptReadWriter::HandleComments() entered, text=<%s>\n", text.c_str());
   
//    std::string result = text + "\n";
   
//    std::string newLine = CrossPlatformGetLine();

//    // keep adding to comment if line is blank or comment
//    while (((IsComment(newLine)) || (IsBlank(newLine))) && (!reachedEndOfFile))
//    {
//       result += (newLine + "\n");
//       newLine = CrossPlatformGetLine();
//    }
   
//    if (HasEllipse(newLine))
//       newLine = HandleEllipsis(newLine);
   
//    result += newLine;
   
//    MessageInterface::ShowMessage
//       ("==> ScriptReadWriter::HandleComments() returning\n   <%s>\n", result.c_str());
   
//    return result;
// }

