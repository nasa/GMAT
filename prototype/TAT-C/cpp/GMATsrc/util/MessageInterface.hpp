//$Id$
//------------------------------------------------------------------------------
//                             MessageInterface
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
// Created: 2008/04/29
//
/**
 * Defines operations on messages.
 */
//------------------------------------------------------------------------------


#ifndef MessageInterface_hpp
#define MessageInterface_hpp

#include <string>
#include "MessageReceiver.hpp"

/**
 * The MessageInterface class defines static methods that are called to send
 * messages to the user.  MessageInterface passes these messages to an 
 * implementation specific class rerived from teh abstract  MessageReceiver 
 * class.  Display to the user is handled in the derived MessageReceiver.
 */
class GMATUTIL_API MessageInterface
{
public:
   static const int MAX_MESSAGE_LENGTH;
   
   static bool SetMessageReceiver(MessageReceiver *mr);
   static MessageReceiver* GetMessageReceiver();

   static void ShowMessage(const std::string &msg);
   static void ShowMessage(const char *format, ...);

   static void PopupMessage(Gmat::MessageType msgType, const std::string &msg);
   static void PopupMessage(Gmat::MessageType msgType, const char *format, ...);
   
   static std::string GetLogFileName();
   static bool GetLogEnable();
   static void SetLogEnable(bool flag);
   static void SetLogPath(const char *pathname, bool append = false);
   static void SetLogPath(const std::string &pathname, bool append = false);
   static void SetLogFile(const std::string &filename);

   static void LogMessage(const std::string &msg);
   static void LogMessage(const char *format, ...);

   static void ClearMessage();
   
   static std::string GetQueuedMessage();
   static void PutMessage(const std::string &msg);
   static void PutMessage(const char *format, ...);
   static void ClearMessageQueue();
   
private:
   static MessageReceiver  *theMessageReceiver;
   
   MessageInterface();
   virtual ~MessageInterface();
};

#endif /* MessageInterface_hpp */
