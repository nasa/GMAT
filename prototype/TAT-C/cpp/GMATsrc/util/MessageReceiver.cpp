//$Id$
//------------------------------------------------------------------------------
//                             MessageReceiver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// number NNG06CA54C
//
// Author: Darrel Conway, based on code by Linda Jun
// Created: 2008/05/28
//
/**
 * Implements output operations for messages.  This is the abstract base class 
 * for these operations.
 */
//------------------------------------------------------------------------------

#include "MessageReceiver.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"
#include "StringUtil.hpp"
#include <fstream>                  // for checking GmatFunction declaration
#include <sstream>                  // for checking GmatFunction declaration

//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
// MessageReceiver()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
MessageReceiver::MessageReceiver()
{
}

//------------------------------------------------------------------------------
// ~MessageReceiver()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
MessageReceiver::~MessageReceiver()
{
}

//------------------------------------------------------------------------------
// bool IsValidLogFile(const std::string fullLogFilePath)
//------------------------------------------------------------------------------
bool MessageReceiver::IsValidLogFile(const std::string fullLogFilePath)
{
   if (!GmatFileUtil::DoesFileExist(fullLogFilePath))
      return true;
   
   std::ifstream inStream(fullLogFilePath.c_str());
   std::string line;
   
   std::string logText = GetLogFileText();

   while (!inStream.eof())
   {
      if (!GmatFileUtil::GetLine(&inStream, line))
      {
         return true;  // empty file is OK?
      }
      
      line = GmatStringUtil::Trim(line, GmatStringUtil::BOTH, true, true);
      
      // Skip empty line or comment line
      if (line[0] == '\0' || line[0] == '%')
         continue;
      // first non-empty line
      if (line.find(logText.c_str()) != line.npos)
         return true;
      else
         return false;
   }

   return true;
}

//------------------------------------------------------------------------------
// std::string GetLogFileText()
//------------------------------------------------------------------------------
std::string MessageReceiver::GetLogFileText()
{
   return "GMAT Build Date:";
}


