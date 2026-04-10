//------------------------------------------------------------------------------
//                           APIMessageReceiver
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, task order 08.
//
// Author: Darrel Conway, based on the ConsoleMessageReceiver
// Created: 2019/08/08
//
/**
 * Declares operations on messages reported to log files for API users.
 */
//------------------------------------------------------------------------------
#ifndef APIMessageReceiver_hpp
#define APIMessageReceiver_hpp

#include <queue>
#include "MessageReceiver.hpp"

/**
 * APIMessageReceiver implements the methods to present messages to the user
 * on the console.
 * 
 * This class is implemented as a singleton.
 */
class APIMessageReceiver : public MessageReceiver
{
public:
   static APIMessageReceiver*   Instance();

   virtual void ShowMessage(const std::string &msg);
   virtual void ShowMessage(const char *msg, ...);

   virtual void PopupMessage(Gmat::MessageType msgType, const std::string &msg);
   virtual void PopupMessage(Gmat::MessageType msgType, const char *msg, ...);
   
   virtual std::string GetLogFileName();
   virtual bool GetLogEnable();
   virtual void SetLogEnable(bool flag);
   virtual void SetLogPath(const std::string &pathname, bool append = false);
   virtual void SetLogFile(const std::string &filename);

   virtual void LogMessage(const std::string &msg);
   virtual void LogMessage(const char *msg, ...);

   virtual void ClearMessage();

   virtual void OpenLogFile(const std::string &filename, bool append = false);
   virtual void CloseLogFile();
   
   virtual std::string GetMessage();
   virtual void PutMessage(const std::string &msg);
   virtual void ClearMessageQueue();
   virtual void SetEchoMode(bool echo = true);
   
   // Other methods not implemented for the APIMessageReceiver
   //virtual int  GetNumberOfMessageLines();
   //virtual void PopupAbortContinue(const std::string &abortMsg,
   //                               const std::string &continueMsg,
   //                               const std::string &msg);

private:
   static APIMessageReceiver* theInstance;
   const int MAX_MESSAGE_LENGTH;
   
   std::queue<std::string> messageQueue;
   std::string popupMessage;
   std::string abortMessage;
   std::string continueMessage;
   Gmat::MessageType messageType;
   int showIntervalInMilSec;
   short messageExist;
   std::string logFileName;
   FILE *logFile;
   bool logEnabled;
   bool logFileSet;
   std::ostream *msgStream;
   /// Toggle for echoing the log to the console
   bool echoMode;
   
   APIMessageReceiver();
   virtual ~APIMessageReceiver();
};

#endif /*APIMessageReceiver_hpp*/
