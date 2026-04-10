//$Id$
//------------------------------------------------------------------------------
//                             GuiMessageReceiver
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
// number NNG06CA54C
//
// Author: Darrel Conway, based on code by Linda Jun
// Created: 2008/04/28
//
/**
 * Defines operations on messages.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "ViewTextFrame.hpp"
#include <stdarg.h>                // for va_start(), va_end()
#include <stdio.h>                 // for vsprintf()
#include <string.h>                // for strlen()

#ifdef __WXMSW__                   // for malloc.h if MS Windows only
#include <malloc.h>                
#endif

#include <iostream>                // for cout, endl
#include <fstream>
#include <sstream>
#include <queue>                   // for queue
#include "GuiMessageReceiver.hpp"  // for GuiMessageReceiver functions
#include "BaseException.hpp"
#include "FileManager.hpp"         // for GetFullPathname()
#include "GmatGlobal.hpp"          // for RunBachMode()
#include "FileUtil.hpp"            // for ParsePathName()
//#include "StringUtil.hpp"          // for GmatStringUtil::Replace()

//#define DEBUG_SET_LOG_FILE

//---------------------------------
//  static data
//---------------------------------
GuiMessageReceiver*  GuiMessageReceiver::instance = NULL;

//---------------------------------
//  private functions
//---------------------------------

//------------------------------------------------------------------------------
//  GuiMessageReceiver()
//------------------------------------------------------------------------------
/**
 * Constructor, called from the Instance method to create the singleton.
 */
//------------------------------------------------------------------------------
GuiMessageReceiver::GuiMessageReceiver() :
   MAX_MESSAGE_LENGTH      (10000),
   logFile                 (NULL),
   logEnabled              (false),
   logFileSet              (false)
{
}

//------------------------------------------------------------------------------
//  GuiMessageReceiver()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
GuiMessageReceiver::~GuiMessageReceiver()
{
}

//---------------------------------
//  public functions
//---------------------------------

//------------------------------------------------------------------------------
// GuiMessageReceiver* Instance()
//------------------------------------------------------------------------------
/**
 * Singleton accessor method
 * 
 * This method creates the GuiMessageReceiver singleton if it has not been
 * constructed, and returns the singleton instance.
 * 
 * @return The GuiMessageReceiver instance.
 */
//------------------------------------------------------------------------------
GuiMessageReceiver* GuiMessageReceiver::Instance()
{
   if (instance == NULL)
      instance = new GuiMessageReceiver;
   
   return instance;   
}


//------------------------------------------------------------------------------
//  void ClearMessage()
//------------------------------------------------------------------------------
/**
 * Clears the message window.
 */  
//------------------------------------------------------------------------------
void GuiMessageReceiver::ClearMessage()
{
   GmatAppData *appData = GmatAppData::Instance();
   if (appData->GetMessageTextCtrl() != NULL)
      appData->GetMessageTextCtrl()->Clear();
}


//------------------------------------------------------------------------------
//  int GetNumberOfMessageLines()
//------------------------------------------------------------------------------
/**
 * Returns the number of lines of text in the message window.
 * 
 * @return The number of lines used.
 */
//------------------------------------------------------------------------------
int GuiMessageReceiver::GetNumberOfMessageLines()
{
   GmatAppData *appData = GmatAppData::Instance();
   ViewTextFrame* theMessageWindow = appData->GetMessageWindow();
   if (theMessageWindow != NULL)
   {
      return theMessageWindow->GetNumberOfLines();
   }
   else
   {
      wxLogError("GuiMessageReceiver::GetNumberOfMessageLines(): "
                 "MessageWindow was not created.");
      wxLog::FlushActive();
      return 0;
   }

   return 0;
}


//------------------------------------------------------------------------------
//  void ShowMessage(const std::string &msgString)
//------------------------------------------------------------------------------
/**
 * Displays the message passed in as an std::string.
 * 
 * @param msgString The message that is displayed.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::ShowMessage(const std::string &msgString)
{
   GmatAppData *appData = GmatAppData::Instance();
   if (appData->GetMessageTextCtrl() != NULL)
   {
      appData->GetMessageTextCtrl()->AppendText(wxString(msgString.c_str()));
      // Added since text in the message window are not always scrolled down,
      // such as debug message from the panel or dialog (LOJ: 2009.03.20)
      // wxWidgets-3.0 does not require page down (LOJ: 2014.09.15)
      #ifdef __USE_WX28__
      appData->GetMessageTextCtrl()->PageDown();
      #endif
   }
   LogMessage(msgString);   
}


//------------------------------------------------------------------------------
//  void ShowMessage(const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Displays a message passed in as a char* and a variable argument
 * list.  Throws std::bad_alloc on memory exhaustion.
 * 
 * @param msg The message, possibly including markers for variable argument 
 *            substitution.
 * @param ... The optional list of parameters that are inserted into the msg 
 *            string.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::ShowMessage(const char *msg, ...)
{
   short    ret;
   short    size;
   va_list  marker;
   char     *msgBuffer;
   
   // msg is vsprintf format
   // actual max message length is MAX_MESSAGE_LENGTH
   size = strlen(msg) + MAX_MESSAGE_LENGTH;
   //LogMessage("strlen(msg)=%d, size=%d\n", strlen(msg), size);
   
//   if( (msgBuffer = (char *)malloc(size)) != NULL )
//   {
//      va_start(marker, msg);
//      ret = vsprintf(msgBuffer, msg, marker);
//      va_end(marker);
//      //LogMessage("ret from vsprintf()=%d\n", ret);
//   }
//   else
//   {
//      msgBuffer =
//         "*** WARNING *** Cannot allocate enough memory to show the message.\n";
//   }

   // Note: 'new' throws an exception of type std::bad_alloc on failure.
   // (Note that if an exception is thrown, no memory will have been
   // allocated, so there will be no memory leak.)
   msgBuffer = new char[size];

   // For older C++ compilers, duplicate that behavior by hand.
   if (!msgBuffer)
      throw std::bad_alloc();

   // Process the message
   va_start(marker, msg);
   ret = vsprintf(msgBuffer, msg, marker);
   va_end(marker);
   //LogMessage("ret from vsprintf()=%d\n", ret);
   
   GmatAppData *appData = GmatAppData::Instance();
   if (appData->GetMessageTextCtrl() != NULL)
   {
      appData->GetMessageTextCtrl()->AppendText(wxString(msgBuffer));
      // wxWidgets-3.0 does not require page down (LOJ: 2014.09.15)
      #ifdef __USE_WX28__
      appData->GetMessageTextCtrl()->PageDown();
      #endif
   }
   
   LogMessage(std::string(msgBuffer));

//   free(msgBuffer);
   delete[] msgBuffer;
} // end ShowMessage()


//------------------------------------------------------------------------------
//  void PopupAbortContinue(const std::string abortMsg, ...)
//------------------------------------------------------------------------------
/**
 * Pops up Abort or Continue message box.
 * 
 * @param abortMsg    The abort message
 * @param continueMsg The continue message
 * @param msg         The message
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::PopupAbortContinue(const std::string &abortMsg,
                                          const std::string &continueMsg,
                                          const std::string &msg)
{  
   popupMessage = msg;
   abortMessage = abortMsg;
   continueMessage = continueMsg;
} // end PopupAbortContinue()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Pops up a message in a message box.
 * 
 * This method logs informational messages directed at pop-up message boxes and  
 * shows them in a pop-up.
 * 
 * @param msgType The type of message that is displayed, selected from the set
 *                {ERROR_, WARNING_, INFO_} enumerated in the Gmat namespace.
 * @param msg The message.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::PopupMessage(Gmat::MessageType msgType, const std::string &msg)
{
   popupMessage = msg;
   messageType = msgType;
   
   // always show message
   ShowMessage(msg);
      
   if (GmatGlobal::Instance()->IsBatchMode() != true)
   {
      switch (msgType)
      {
      case Gmat::ERROR_:
         (void)wxMessageBox(wxString(msg.c_str()),
                            wxT("GMAT Error"));
         break;
      case Gmat::WARNING_:
         (void)wxMessageBox(wxString(msg.c_str()),
                            wxT("GMAT Warning"));
         break;
      case Gmat::INFO_:
         (void)wxMessageBox(wxString(msg.c_str()),
                            wxT("Information"));
         break;
      default:
         break;
      };
   }
   
} // end PopupMessage()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Pops up a message in a message box.
 * 
 * This method logs informational messages directed at pop-up message boxes and
 * shows the message as a pop-up.
 *
 * Throws std::bad_alloc in memory exhaustion.
 * 
 * @param msgType The type of message that is displayed, selected from the set
 *                {ERROR_, WARNING_, INFO_} enumerated in the Gmat namespace.
 * @param msg The message, possibly including markers for variable argument 
 *            substitution.
 * @param ... The optional list of parameters that are inserted into the msg 
 *            string.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::PopupMessage(Gmat::MessageType msgType, const char *msg, ...)
{
   short    ret;
   short    size;
   va_list  marker;
   char     *msgBuffer;
   
   // msg is vsprintf format
   // actual max message length is MAX_MESSAGE_LENGTH
   size = strlen(msg) + MAX_MESSAGE_LENGTH;
   
   // Note: 'new' throws an exception of type std::bad_alloc on failure.
   // (Note that if an exception is thrown, no memory will have been
   // allocated, so there will be no memory leak.)
   msgBuffer = new char[size];

   // For older C++ compilers, duplicate that behavior by hand.
   if (!msgBuffer) throw std::bad_alloc();

   // Process the message
   va_start(marker, msg);
   ret = vsprintf(msgBuffer, msg, marker);
   va_end(marker);

   // if no EOL then append it
   if (msgBuffer[strlen(msgBuffer)-1] != '\n')
      msgBuffer[strlen(msgBuffer)] = '\n';

//   if ( (msgBuffer = (char *)malloc(size)) != NULL )
//   {
//      va_start(marker, msg);
//      ret = vsprintf(msgBuffer, msg, marker);
//      va_end(marker);
//
//      // if no EOL then append it
//      if (msgBuffer[strlen(msgBuffer)-1] != '\n')
//         msgBuffer[strlen(msgBuffer)] = '\n';
//   }
//   else
//   {
//      msgBuffer = "*** WARNING *** Cannot allocate enough memory to show the message.\n";
//   }
   
   // always show message
   ShowMessage(msgBuffer);
   
   if (GmatGlobal::Instance()->IsBatchMode() != true)
   {
      switch (msgType)
      {
      case Gmat::ERROR_:
         (void)wxMessageBox(wxString(msgBuffer),
                            wxT("GMAT Error"));
         break;
      case Gmat::WARNING_:
         (void)wxMessageBox(wxString(msgBuffer),
                            wxT("GMAT Warning"));
         break;
      case Gmat::INFO_:
         (void)wxMessageBox(wxString(msgBuffer),
                            wxT("Information"));
         break;
      default:
         break;
      };
   }
   
//   free(msgBuffer);
   delete[] msgBuffer;
} // end PopupMessage()


//------------------------------------------------------------------------------
// std::string GetLogFileName()
//------------------------------------------------------------------------------
/**
 * Retrieves the fully qualified name of the log file.
 * 
 * @return The name of the log file, including path information.
 */
//------------------------------------------------------------------------------
std::string GuiMessageReceiver::GetLogFileName()
{
   FileManager *fm = FileManager::Instance();
   std::string fname;
   try
   {
      if (logFileName == "")
      {
         fname = fm->GetFullPathname("LOG_FILE");
      }
      else
      {
         std::string outputPath = fm->GetPathname(FileManager::LOG_FILE);
         
         // add output path if there is no path
         if (logFileName.find("/") == logFileName.npos &&
             logFileName.find("\\") == logFileName.npos)
         {
            fname = outputPath + logFileName;
         }
      }
   }
   catch (BaseException &e)
   {
      GuiMessageReceiver::ShowMessage
         ("**** ERROR **** " + e.GetFullMessage() + 
          "So setting log file name to GmatLog.txt");
      
      fname = "GmatLog.txt";
   }
   
   return fname;
}


//------------------------------------------------------------------------------
// void LogMessage(const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Logs the message to the log file.
 * 
 * This method displays the input message on the console and writes it to the 
 * log file.
 * 
 * @param msg The message.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::LogMessage(const std::string &msg)
{
   if (!logEnabled) return;

   std::cout << msg;
   
   if (logEnabled)
   {
      if (logFile == NULL)
      {
         SetLogFile(GetLogFileName());
      }
   }
   else if (!logFileSet)
   {
      OpenLogFile(logFileName);
   }
   
   if (logFile)
   {
      //std::string tempStr = GmatStringUtil::Replace(msg, "%", "%%");
      //fprintf(logFile, "%s", tempStr.c_str());
      fprintf(logFile, "%s", msg.c_str());
      fflush(logFile);
   }
}


//------------------------------------------------------------------------------
//  void LogMessage(const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Logs a variable argument formatted message to the log file.
 * 
 * This method calls the std::string vrrersion of LogMessage to do the actual
 * logging.
 *
 * Throws std::bad_alloc on memory exhaustion.
 * 
 * @param msg The message, possibly including markers for variable argument 
 *            substitution.
 * @param ... The optional list of parameters that are inserted into the msg 
 *            string.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::LogMessage(const char *msg, ...)
{
   if (!logEnabled) return;

   short    ret;
   short    size;
   va_list  marker;
   char     *msgBuffer;
   
   // msg is vsprintf format
   // actual max message length is MAX_MESSAGE_LENGTH
   size = strlen(msg) + MAX_MESSAGE_LENGTH;
   
//   if ( (msgBuffer = (char *)malloc(size)) != NULL )
//   {
//      va_start(marker, msg);
//      ret = vsprintf(msgBuffer, msg, marker);
//      va_end(marker);
//   }
//   else
//   {
//      msgBuffer = "*** WARNING *** Cannot allocate enough memory to log the message.\n";
//   }
//

   // Note: 'new' throws an exception of type std::bad_alloc on failure.
   // (Note that if an exception is thrown, no memory will have been
   // allocated, so there will be no memory leak.)
   msgBuffer = new char[size];

   // For older C++ compilers, duplicate that behavior by hand.
   if (!msgBuffer) throw std::bad_alloc();

   // Process the message
   va_start(marker, msg);
   ret = vsprintf(msgBuffer, msg, marker);
   va_end(marker);
   
   LogMessage(std::string(msgBuffer));
   
//   free(msgBuffer);
   delete[] msgBuffer;
} // end LogMessage()


//------------------------------------------------------------------------------
// void SetLogEnable(bool flag)
//------------------------------------------------------------------------------
/**
 * Turns logging on or off.
 * 
 * @param flag The new logging state -- true enables logging, and false disables 
 *             it.  The logging state is idempotent.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::SetLogEnable(bool flag)
{
   logEnabled = flag;
}


//------------------------------------------------------------------------------
// bool GetLogEnable()
//------------------------------------------------------------------------------
/**
* returns if logging is on or off.
*
*/
//------------------------------------------------------------------------------
bool GuiMessageReceiver::GetLogEnable()
{
   return logEnabled;
}


//------------------------------------------------------------------------------
// void SetLogPath(const std::string &pathname, bool append = false)
//------------------------------------------------------------------------------
/*
 * Sets log file path with keeping log file name as is.
 *
 * @param  pathname  log file path name, such as "/newpath/test1/"
 * @param  append  true if appending log message (false)
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::SetLogPath(const std::string &pathname, bool append)
{
   FileManager *fm = FileManager::Instance();
   std::string fname;
   try
   {
      std::string filename = fm->GetFilename(FileManager::LOG_FILE);
      fname = pathname + filename;
   }
   catch (BaseException &e)
   {
      GuiMessageReceiver::ShowMessage
         ("**** ERROR **** " + e.GetFullMessage() + 
          "So setting log file name to GmatLog.txt");
      
      fname = "GmatLog.txt";
   }
   
   OpenLogFile(fname, append);
   
}


//------------------------------------------------------------------------------
// void SetLogFile(const std::string &filename)
//------------------------------------------------------------------------------
/*
 * Calls OpenLogFile() to set the log file path and name and then open the log.
 *
 * @param  filename  log file name, such as "/newpath/test1/GmatLog.txt"
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::SetLogFile(const std::string &filename)
{
#ifdef DEBUG_SET_LOG_FILE
   ShowMessage("Log file being set to: %s\n", filename.c_str());
#endif
   std::string fname = filename;
   FileManager *fm   = FileManager::Instance();
   
   if (GmatFileUtil::ParsePathName(fname) == "")
   {
      std::string outPath = fm->GetFullPathname(FileManager::OUTPUT_PATH);
      fname = outPath + fname;
   }
   
   OpenLogFile(fname);
}


//------------------------------------------------------------------------------
// void OpenLogFile(const std::string &filename, bool append = false)
//------------------------------------------------------------------------------
/*
 * Sets the log file name and opens the log file.
 *
 * @param filename  log file name, such as "/newpath/test1/GmatLog.txt"
 * @param append  true if appending log message
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::OpenLogFile(const std::string &filename, bool append)
{
   // Make sure we aren't stomping on an already existing non-log file
   // Make sure we aren't stomping on an already existing non-log file
   if (logFileName != filename)
   {
      if (!IsValidLogFile(filename))
      {
         throw GmatBaseException(
               "ERROR - specified log file is not a valid log file.\n");
      }
   }

   logFileName = filename;
   
   if (logFile)
      fclose(logFile);
   
   if (append)
      logFile = fopen(logFileName.c_str(), "a");
   else
      logFile = fopen(logFileName.c_str(), "w");
   
   if (!logFile)
   {
      std::cout << "**** ERROR **** Error setting the log file to " << logFileName
                << "\nSo setting it to \"GmatLog.txt\" in the executable directory\n";
      
      logFileName = "GmatLog.txt";
      
      if (append)
         logFile = fopen(logFileName.c_str(), "a");
      else
         logFile = fopen(logFileName.c_str(), "w");
   }
   
   if (logFile)
   {
      FileManager *fm          = FileManager::Instance();
      std::string logFileStart = GetLogFileText();

      // Fix bug GMT-7399
      // fprintf(logFile, "%s %s %s\n\n",  logFileStart.c_str(), __DATE__, __TIME__);
      fprintf(logFile, "%s %s %s\n\n", logFileStart.c_str(),
         GmatGlobal::Instance()->GetGMATBuildDate().c_str(),
         GmatGlobal::Instance()->GetGMATBuildTime().c_str());

      fprintf(logFile, "GMAT Log file set to %s\n", logFileName.c_str());
      
      logFileSet = true;
   }
}


//------------------------------------------------------------------------------
// void CloseLogFile()
//------------------------------------------------------------------------------
/**
 * Closes the log file.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::CloseLogFile()
{
   if (logFile)
      fclose(logFile);
   
   logFile = NULL;
   logFileSet = false;
}


//------------------------------------------------------------------------------
//  std::string GetMessage()
//------------------------------------------------------------------------------
/**
 * Pops the messages off the message queue and concatenates them together.
 * 
 * @return The concatenated messages.
 */
//------------------------------------------------------------------------------
std::string GuiMessageReceiver::GetMessage()
{
   std::string msg;
   
   while (!GuiMessageReceiver::messageQueue.empty())
   {
      msg = msg + messageQueue.front().c_str();
      messageQueue.pop();
   }
   GuiMessageReceiver::messageExist = 0;
   
   return msg;
}

//------------------------------------------------------------------------------
// void PutMessage(const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Push the message into queue
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::PutMessage(const std::string &msg)
{
   messageQueue.push(msg);
}

//------------------------------------------------------------------------------
// void ClearMessageQueue()
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to clear the message queue.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::ClearMessageQueue()
{
   while (!GuiMessageReceiver::messageQueue.empty())
      messageQueue.pop();
   
   GuiMessageReceiver::messageExist = 0;
}


