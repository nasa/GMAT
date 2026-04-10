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
/**
 * Implements operations on messages.
 */
//------------------------------------------------------------------------------
#include "MessageInterface.hpp"
#include <stdarg.h>              // for va_start() and va_end()
#include <cstdlib>               // for malloc() and free() - Required for GCC 4.3
#include <stdio.h>               // for vsprintf(), vsnprintf()

//---------------------------------
//  static data
//---------------------------------
MessageReceiver* MessageInterface::theMessageReceiver = NULL;
//const int MessageInterface::MAX_MESSAGE_LENGTH = 20000;
const int MessageInterface::MAX_MESSAGE_LENGTH = 30000;


//---------------------------------
//  private methods
//---------------------------------


//------------------------------------------------------------------------------
// MessageInterface()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
MessageInterface::MessageInterface()
{
}

//------------------------------------------------------------------------------
// ~MessageInterface()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
MessageInterface::~MessageInterface()
{
}


//---------------------------------
//  public static methods
//---------------------------------


//------------------------------------------------------------------------------
// bool SetMessageReceiver(MessageReceiver *mr)
//------------------------------------------------------------------------------
bool MessageInterface::SetMessageReceiver(MessageReceiver *mr)
{
   theMessageReceiver = mr;
   return true;
}


//------------------------------------------------------------------------------
// MessageReceiver* GetMessageReceiver()
//------------------------------------------------------------------------------
MessageReceiver* MessageInterface::GetMessageReceiver()
{
   return theMessageReceiver;
}


//------------------------------------------------------------------------------
//  void ShowMessage(const std::string &msgString)
//------------------------------------------------------------------------------
/**
 * Passes an std::string message to the MessageReceiver.
 *
 * @param msgString The message that is displayed.
 */
//------------------------------------------------------------------------------
void MessageInterface::ShowMessage(const std::string &msgString)
{
   ShowMessage("%s", msgString.c_str());
}


//------------------------------------------------------------------------------
//  void ShowMessage(const char *format, ...)
//------------------------------------------------------------------------------
/**
 * Passes a variable argument delimited message to the MessageReceiver.
 *
 * @param format The format, possibly including markers for variable argument
 *                  substitution.
 * @param ...    The optional list of parameters that are inserted into the format
 *                  string.
 */
//------------------------------------------------------------------------------
void MessageInterface::ShowMessage(const char *format, ...)
{
   if (theMessageReceiver != NULL)
   {
      int      ret;
      size_t   size;
      va_list  args;
      
      // Try using fixed size buffer and vsnprintf to prevent buffer
      // overrrun (LOJ: 2015.05.21)
      //==============================================================
      #if 1
      //==============================================================
      
      char msgBuffer[MAX_MESSAGE_LENGTH];
      va_start(args, format);
      
      // Used vsnprintf (LOJ: 2015.05.21)
      // int vsnprintf (char * s, size_t n, const char * format, va_list arg );
      
      ret = vsnprintf(msgBuffer, MAX_MESSAGE_LENGTH, format, args);
      
      if (ret < 0) // vsnprintf failed
         theMessageReceiver->ShowMessage("Unable to complete messaging\n");
      else
      {
         va_end(args);
         theMessageReceiver->ShowMessage(std::string(msgBuffer));
      }
      
      //==============================================================
      #else
      //==============================================================
      
      char *msgBuffer = NULL;
      std::string msgStr("*** WARNING *** Cannot allocate enough memory to show the message.\n");
      
      // format is vsprintf format
      // actual max message length is MAX_MESSAGE_LENGTH
      size = strlen(format) + MAX_MESSAGE_LENGTH;
      //LogMessage("strlen(format)=%u, size=%u\n", strlen(format), size);
      
      if( (msgBuffer = (char *)malloc(size)) != NULL )
      {
         for (unsigned int i=0; i<size; i++)
            msgBuffer[i] = '\0';
         va_start(args, format);
         ret = vsprintf(msgBuffer, format, args);
         //LogMessage("return from vsprintf() is %d\n", ret);
         if (ret < 0)
            theMessageReceiver->ShowMessage("Unable to complete messaging");
         else
         {
            va_end(args);
            theMessageReceiver->ShowMessage(std::string(msgBuffer));
         }
      }
      else
      {
         theMessageReceiver->ShowMessage(msgStr);
      }
      free(msgBuffer);
      
      //==============================================================
      #endif
      //==============================================================
   }
} // end ShowMessage()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Passes a popup message to the MessageReceiver.
 *
 * @param msgType The type of message that is displayed, selected from the set
 *                   {ERROR_, WARNING_, INFO_} enumerated in the Gmat namespace.
 * @param msg     The message.
 */
//------------------------------------------------------------------------------
void MessageInterface::PopupMessage(Gmat::MessageType msgType, const std::string &msg)
{
   PopupMessage(msgType, "%s", msg.c_str());
}


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const char *format, ...)
//------------------------------------------------------------------------------
/**
 * Passes a variable argument delimited popup message to the MessageReceiver.
 *
 * @param msgType The type of message that is displayed, selected from the set
 *                   {ERROR_, WARNING_, INFO_} enumerated in the Gmat namespace.
 * @param format  The format, possibly including markers for variable argument
 *                    substitution.
 * @param ...     The optional list of parameters that are inserted into the format
 *                   string.
 */
//------------------------------------------------------------------------------
void MessageInterface::PopupMessage(Gmat::MessageType msgType, const char *format,
      ...)
{
   if (theMessageReceiver != NULL)
   {
      int          ret;
      unsigned int size;
      va_list      args;
      
      // Try using fixed size buffer and vsnprintf to prevent buffer
      // overrrun (LOJ: 2015.05.21)
      //==============================================================
      #if 1
      //==============================================================
      
      char msgBuffer[MAX_MESSAGE_LENGTH];
      va_start(args, format);
      
      // Used vsnprintf (LOJ: 2015.05.21)
      // int vsnprintf (char * s, size_t n, const char * format, va_list arg );      
      ret = vsnprintf(msgBuffer, MAX_MESSAGE_LENGTH, format, args);
      
      if (ret < 0) // vsnprintf failed
         theMessageReceiver->PopupMessage(msgType, "Unable to complete messaging\n");
      else
      {
         va_end(args);
         
         // if no EOL then append it
         if (msgBuffer[ret-1] != '\n')
         {
            if ((ret+1) < MAX_MESSAGE_LENGTH)
            {
               msgBuffer[ret] = '\n';
               msgBuffer[ret+1] = '\0';
            }
         }
         theMessageReceiver->PopupMessage(msgType, std::string(msgBuffer));
      }
      
      //==============================================================
      #else
      //==============================================================
      
      char *msgBuffer = NULL;
      std::string msgStr("*** WARNING *** Cannot allocate enough memory to show the message.\n");
      
      // format is vsprintf format
      // actual max message length is MAX_MESSAGE_LENGTH
      size = strlen(format) + MAX_MESSAGE_LENGTH;
      
      if ( (msgBuffer = (char *)malloc(size)) != NULL )
      {
         for (unsigned int i=0; i<size; i++)
            msgBuffer[i] = '\0';
         va_start(args, format);
         ret = vsprintf(msgBuffer, format, args);
         if (ret < 0)
            theMessageReceiver->PopupMessage(msgType,
                  "Unable to complete messaging");
         else
         {
            va_end(args);

            // if no EOL then append it
            if (msgBuffer[strlen(msgBuffer)-1] != '\n')
               msgBuffer[strlen(msgBuffer)] = '\n';
            theMessageReceiver->PopupMessage(msgType, std::string(msgBuffer));
         }
      }
      else
      {
         theMessageReceiver->PopupMessage(msgType, msgStr);
      }
      
      free(msgBuffer);
      
      //==============================================================
      #endif
      //==============================================================
   }
} // end PopupMessage()

//------------------------------------------------------------------------------
// std::string GetLogFileName()
//------------------------------------------------------------------------------
/**
 * Retrieves the fully qualified name of the log file from the MessageReceiver.
 *
 * @return The name of the log file, including path information.
 */
//------------------------------------------------------------------------------
std::string MessageInterface::GetLogFileName()
{
   if (theMessageReceiver == NULL)
      return "";
   return theMessageReceiver->GetLogFileName();
}

//------------------------------------------------------------------------------
// void SetLogEnable(bool flag)
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to turn logging on or off.
 *
 * @param flag The new logging state -- true enables logging, and false disables
 *             it.  The logging state is idempotent.
 */
//------------------------------------------------------------------------------
void MessageInterface::SetLogEnable(bool flag)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->SetLogEnable(flag);
}

//------------------------------------------------------------------------------
// bool GetLogEnable()
//------------------------------------------------------------------------------
/**
* Is logging on or off.
*
*/
//------------------------------------------------------------------------------
bool MessageInterface::GetLogEnable()
{
   if (theMessageReceiver != NULL)
      return theMessageReceiver->GetLogEnable();
   else
      return false;
}

//------------------------------------------------------------------------------
// void SetLogPath(const char *pathname, bool append = false)
//------------------------------------------------------------------------------
/*
 * Sends log file path and append state to the MessageReceiver.
 *
 * @param  pathname  log file path name, such as "/newpath/test1/"
 * @param  append  true if appending log message (false)
 */
//------------------------------------------------------------------------------
void MessageInterface::SetLogPath(const char *pathname, bool append)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->SetLogPath(std::string(pathname), append);
}

//------------------------------------------------------------------------------
// void SetLogPath(const std::string &pathname, bool append = false)
//------------------------------------------------------------------------------
/*
 * Sends log file path and append state to the MessageReceiver.
 *
 * @param  pathname  log file path name, such as "/newpath/test1/"
 * @param  append  true if appending log message (false)
 */
//------------------------------------------------------------------------------
void MessageInterface::SetLogPath(const std::string &pathname, bool append)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->SetLogPath(pathname, append);
}

//------------------------------------------------------------------------------
// void SetLogFile(const std::string &filename)
//------------------------------------------------------------------------------
/*
 * Sends the log file path and name to the MessageReceiver.
 *
 * @param  filename  log file name, such as "/newpath/test1/GmatLog.txt"
 */
//------------------------------------------------------------------------------
void MessageInterface::SetLogFile(const std::string &filename)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->SetLogFile(filename);
}

//------------------------------------------------------------------------------
//  void LogMessage(const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Sends a message to the MessageReceiver for logging.
 *
 * @param msg The message.
 */
//------------------------------------------------------------------------------
void MessageInterface::LogMessage(const std::string &msg)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->LogMessage(msg);
}

//------------------------------------------------------------------------------
//  void LogMessage(const char *format, ...)
//------------------------------------------------------------------------------
/**
 * Sends a variable argument message to the MessageReceiver for logging.
 *
 * @param format The format, possibly including markers for variable argument
 *            substitution.
 * @param ... The optional list of parameters that are inserted into the format
 *            string.
 */
//------------------------------------------------------------------------------
void MessageInterface::LogMessage(const char *format, ...)
{
   if (theMessageReceiver != NULL)
   {
      int     ret;
      size_t  size;
      va_list args;
      
      // Try using fixed size buffer and vsnprintf to prevent buffer
      // overrrun (LOJ: 2015.05.21)
      //==============================================================
      #if 1
      //==============================================================
      
      char msgBuffer[MAX_MESSAGE_LENGTH];
      va_start(args, format);
      
      // Used vsnprintf (LOJ: 2015.05.21)
      // int vsnprintf (char * s, size_t n, const char * format, va_list arg );
      // Composes a string with the same text that would be printed if format was
      // used on printf, but using the elements in the variable argument list
      // identified by arg instead of additional function arguments and storing
      // the resulting content as a C string in the buffer pointed by s
      // (taking n as the maximum buffer capacity to fill).
      // If the resulting string would be longer than n-1 characters, the remaining
      // characters are discarded and not stored, but counted for the value returned
      // by the function.
      
      ret = vsnprintf(msgBuffer, MAX_MESSAGE_LENGTH, format, args);
      
      if (ret < 0) // vsnprintf failed
         theMessageReceiver->LogMessage("Unable to complete messaging\n");
      else
      {
         va_end(args);
         theMessageReceiver->LogMessage(std::string(msgBuffer));
      }

      //==============================================================
      #else
      //==============================================================
      
      char *msgBuffer = NULL;
      std::string msgStr("*** WARNING *** Cannot allocate enough memory to log the message.\n");

      // format is vsprintf format
      // actual max message length is MAX_MESSAGE_LENGTH
      size = strlen(format) + MAX_MESSAGE_LENGTH;
      
      // If allocation failes, try with smaller size
      if ( (msgBuffer = (char *)malloc(size)) == NULL)
         msgBuffer = (char *)malloc(10000);
      
      //if ( (msgBuffer = (char *)malloc(size)) != NULL )
      if (msgBuffer != NULL )
      {
         for (size_t i=0; i<size; i++)
            msgBuffer[i] = '\0';
         va_start(args, format);
         ret = vsprintf(msgBuffer, format, args);
         if (ret < 0) // vsprintf failed
            theMessageReceiver->LogMessage("Unable to complete messaging\n");
         else
         {
            va_end(args);
            theMessageReceiver->LogMessage(std::string(msgBuffer));
         }
      }
      else
      {
         theMessageReceiver->LogMessage(msgStr);
      }
      
      free(msgBuffer);
      
      //==============================================================
      #endif
      //==============================================================
   }
}

//------------------------------------------------------------------------------
//  void ClearMessage()
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to clear the message window.
 */
//------------------------------------------------------------------------------
void MessageInterface::ClearMessage()
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->ClearMessage();
}

//------------------------------------------------------------------------------
// std::string GetQueuedMessage()
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to retrieve all message from the queue.
 */
//------------------------------------------------------------------------------
std::string MessageInterface::GetQueuedMessage()
{
   if (theMessageReceiver != NULL)
      return theMessageReceiver->GetMessage();
   else
      return "";
}

//------------------------------------------------------------------------------
// void PutMessage(const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to push the message into queue
 */
//------------------------------------------------------------------------------
void MessageInterface::PutMessage(const std::string &msg)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->PutMessage(msg);
}

//------------------------------------------------------------------------------
// void PutMessage(const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to push the message into queue
 */
//------------------------------------------------------------------------------
void MessageInterface::PutMessage(const char *format, ...)
{
   if (theMessageReceiver != NULL)
   {
      int     ret;
      size_t  size;
      va_list args;
      char    *msgBuffer = NULL;
      std::string msgStr("*** WARNING *** Cannot allocate enough memory to show the message.\n");
      
      // msg is vsprintf format
      // actual max message length is MAX_MESSAGE_LENGTH
      size = strlen(format) + MAX_MESSAGE_LENGTH;
      //LogMessage("strlen(format)=%d, size=%d\n", strlen(format), size);
      
      if( (msgBuffer = (char *)malloc(size)) != NULL )
      {
         for (size_t i=0; i<size; i++)
            msgBuffer[i] = '\0';
         va_start(args, format);
         ret = vsprintf(msgBuffer, format, args);
         if (ret < 0) // vsprintf failed
            theMessageReceiver->PutMessage("Unable to complete messaging\n");
         else
         {
            va_end(args);
            theMessageReceiver->PutMessage(std::string(msgBuffer));
         }
      }
      else
      {
         theMessageReceiver->PutMessage(msgStr);
      }
      
//      theMessageReceiver->LogMessage(std::string(msgBuffer));
      free(msgBuffer);
   }
}

//------------------------------------------------------------------------------
// void ClearMessageQueue()
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to clear the message queue.
 */
//------------------------------------------------------------------------------
void MessageInterface::ClearMessageQueue()
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->ClearMessageQueue();
}

