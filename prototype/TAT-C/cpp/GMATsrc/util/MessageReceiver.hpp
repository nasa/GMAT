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
 * Defines output operations for messages.  This is the abstract base class for 
 * these operations
 */
//------------------------------------------------------------------------------

#ifndef MESSAGERECEIVER_HPP_
#define MESSAGERECEIVER_HPP_


//#include <iostream>                // for cout, endl
//#include <fstream>
//#include <sstream>
//#include <queue>                   // for queue
#include "BaseException.hpp"
#include "GmatGlobal.hpp"          // for RunBachMode()
#include "utildefs.hpp"            // for MessageTypes


/**
 * MessageReceiver is an abstract class designed to receive messages from GMAT's 
 * components and display them to the user.  Specific implementations of GMAT 
 * derive a class from the MessageReceiver class that implements the abstract
 * methods to present messages to the user using an appropriate venue -- the 
 * console for console applications, the GUI for GUI based applications, and 
 * other methods as deemed appropriate for other implementations.
 * 
 * The derived classes are best implemented as singletons.
 */
class GMATUTIL_API MessageReceiver
{
public:
   // Derived classes should implement a singleton accessor, like this:
   // static MessageReceiver*  Instance();
   
   // Derived classes must implement all of the following methods:
   virtual void ShowMessage(const std::string &msg) = 0;
   virtual void ShowMessage(const char *msg, ...) = 0;

   virtual void PopupMessage(Gmat::MessageType msgType, const std::string &msg) = 0;
   virtual void PopupMessage(Gmat::MessageType msgType, const char *msg, ...) = 0;
   
   virtual std::string GetLogFileName() = 0;
   virtual bool GetLogEnable() = 0;
   virtual void SetLogEnable(bool flag) = 0;
   virtual void SetLogPath(const std::string &pathname, bool append = false) = 0;
   virtual void SetLogFile(const std::string &filename) = 0;

   virtual void LogMessage(const std::string &msg) = 0;
   virtual void LogMessage(const char *msg, ...) = 0;

   virtual void ClearMessage() = 0;

   virtual std::string GetMessage() = 0;
   virtual void PutMessage(const std::string &msg) = 0;
   virtual void ClearMessageQueue() = 0;
   
protected:
      
   MessageReceiver();
   virtual ~MessageReceiver();
   
   // Methods for log file
   bool        IsValidLogFile(const std::string fullLogFilePath);
   std::string GetLogFileText();

};

#endif /*MESSAGERECEIVER_HPP_*/
