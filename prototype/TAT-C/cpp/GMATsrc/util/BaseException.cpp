//$Id$
//------------------------------------------------------------------------------
//                             BaseException
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
// number NNG04CC06P.
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/1/18
//
/**
 * Exception class used by the GmatBase base class.
 */
//------------------------------------------------------------------------------

#include "BaseException.hpp"
#include <stdarg.h>                // for va_start(), va_end()

#include <cstdlib>                 // Required for GCC 4.3
#include <string.h>                // Required for GCC 4.3
#include <stdio.h>                 // Fix for header rearrangement in gcc 4.4


//------------------------------------------------------------------------------
// std::string GetFullMessage() const 
//------------------------------------------------------------------------------
std::string BaseException::GetFullMessage() const 
{
   std::string preface = "";

   if (msgType == Gmat::ERROR_)
      preface = "**** ERROR **** ";
   if (msgType == Gmat::WARNING_)
      preface = "**** WARNING **** ";
   return preface + theMessage + theDetails;
}

//------------------------------------------------------------------------------
// std::string GetDetails() const 
//------------------------------------------------------------------------------
std::string BaseException::GetDetails() const 
{
   return theDetails;
}

//------------------------------------------------------------------------------
// bool IsFatal() const
//------------------------------------------------------------------------------
bool BaseException::IsFatal() const
{
   return isFatal;
}

//------------------------------------------------------------------------------
// void BaseException::SetMessage(const std::string &message)  
//------------------------------------------------------------------------------
void BaseException::SetMessage(const std::string &message)  
{
   theMessage = message;
}

//------------------------------------------------------------------------------
// void SetDetails(const std::string &details)  
//------------------------------------------------------------------------------
void BaseException::SetDetails(const std::string &details)  
{
   theDetails = details;
}

//------------------------------------------------------------------------------
// void SetFatal(bool fatal)
//------------------------------------------------------------------------------
void BaseException::SetFatal(bool fatal)
{
   isFatal = fatal;
}

//------------------------------------------------------------------------------
// Gmat::MessageType GetMessageType()
//------------------------------------------------------------------------------
Gmat::MessageType BaseException::GetMessageType()
{
   return msgType;
}

//------------------------------------------------------------------------------
// void SetMessageType(Gmat::MessageType mt)
//------------------------------------------------------------------------------
void BaseException::SetMessageType(Gmat::MessageType mt)
{
   msgType = mt;
}

//------------------------------------------------------------------------------
// const BaseException& operator=(const std::string &newMessage) 
//------------------------------------------------------------------------------
const BaseException& BaseException::operator=(const std::string &newMessage) 
{
   theMessage = newMessage;
   return *this;
}

//------------------------------------------------------------------------------
// void SetDetails(const char *details, ...)
//------------------------------------------------------------------------------
/**
 * constructor taking variable arguments
 */
//------------------------------------------------------------------------------
void BaseException::SetDetails(const char *details, ...)
{
   short    ret;
   short    size;
   va_list  marker;
   char     *msgBuffer;

   size = strlen(details) + MAX_MESSAGE_LENGTH;

   if ( (msgBuffer = (char *)malloc(size)) != NULL )
   {
      va_start(marker, details);

      #ifdef _MSC_VER  // Microsoft Visual C++
      // _vscprintf doesn't count terminating '\0'
      int len = _vscprintf( details, marker ) + 1;
      ret = vsprintf_s(msgBuffer, len, details, marker);
      #else
      ret = vsprintf(msgBuffer, details, marker);
      #endif

      if (ret < 0)
         ;   // Neg number means the call failed; should we do anything?

      va_end(marker);
   }

   theDetails = std::string(msgBuffer);
   free(msgBuffer);
}

//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// BaseException(const std::string& message = "", const std::string &details = "") 
//------------------------------------------------------------------------------
BaseException::BaseException(const std::string& message,
      const std::string &details, Gmat::MessageType mt)
{
   theMessage = message;
   theDetails = details;
   msgType    = mt;
   isFatal = false;
}

//------------------------------------------------------------------------------
// BaseException(const BaseException& be) 
//------------------------------------------------------------------------------
BaseException::BaseException(const BaseException& be) 
{
   theMessage = be.theMessage;
   theDetails = be.theDetails;
   isFatal = be.isFatal;
}

//------------------------------------------------------------------------------
// virtual ~BaseException() 
//------------------------------------------------------------------------------
BaseException::~BaseException() 
{
}

//------------------------------------------------------------------------------
// const BaseException& operator=(const BaseException& be) 
//------------------------------------------------------------------------------
const BaseException& BaseException::operator=(const BaseException& be) 
{
   theMessage = be.theMessage;
   theDetails = be.theDetails;
   isFatal = be.isFatal;
   return *this;
}
