//$Id$
//------------------------------------------------------------------------------
//                              GmatInterface
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
// Created: 2004/8/30
//
/**
 * Implements class to provide sending scripts from MATLAB to GMAT.
 */
//------------------------------------------------------------------------------

#include "GmatInterface.hpp"
#include "Moderator.hpp"         // for Instance()
#include "MessageInterface.hpp"
#include "InterfaceException.hpp"

GmatInterface* GmatInterface::instance = NULL;
bool GmatInterface::mPassedInterpreter = false;

//#define DEBUG_GMAT_INTERFACE
//#define DEBUG_TEST_CALLBACK

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// static Instance()
//------------------------------------------------------------------------------
GmatInterface* GmatInterface::Instance()
{
   if (instance == NULL)
      instance = new GmatInterface();
   return instance;
}


//------------------------------------------------------------------------------
// void OpenScript()
//------------------------------------------------------------------------------
void GmatInterface::OpenScript()
{
   if (!mInStringStream)
      mInStringStream = new std::istringstream;

}


//------------------------------------------------------------------------------
// void ClearScrip()
//------------------------------------------------------------------------------
void GmatInterface::ClearScript()
{
   mStringStream.str("");
   Moderator::GetUiInterpreter()->CloseCurrentProject();
}


//------------------------------------------------------------------------------
// void PutScript(char *str)
//------------------------------------------------------------------------------
/*
 * Appends script to a string stream.
 *
 * @param <str> string to append
 */
//------------------------------------------------------------------------------
void GmatInterface::PutScript(char *str)
{
   mStringStream << std::string(str) << std::endl;
   #ifdef DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage("GmatInterface::PutScript() str=%s\n", str);
   #endif
}


//------------------------------------------------------------------------------
// void BuildObject()
//------------------------------------------------------------------------------
/*
 * Clears resource and build new objects from a internal string stream.
 */
//------------------------------------------------------------------------------
void GmatInterface::BuildObject()
{
  
   Moderator *moderator = Moderator::Instance();
   mPassedInterpreter = false;
   std::streambuf *streamBuf = mStringStream.rdbuf();

   // redirect mInStringStream into mStringStream
   RedirectBuffer(mInStringStream, streamBuf);
   
   #ifdef DEBUG_GMAT_INTERFACE
   //loj: 8/31/04 Why this causes problem for long scripts? buffer overflow?
   //MessageInterface::ShowMessage
   //   ("GmatInterface::BuildObject() mStringStream.str=\n%s", mStringStream.str().c_str());
   //MessageInterface::ShowMessage
   //   ("GmatInterface::BuildObject() mInStringStream.str=\n%s\n", mInStringStream->str().c_str());
   #endif
   
   // flag to clear objects and mission sequence
   mPassedInterpreter = moderator->InterpretScript(mInStringStream, true);
   Moderator::GetUiInterpreter()->UpdateView(3);
   
   // empty the buffer, once objects are created
   mStringStream.str("");
}


//------------------------------------------------------------------------------
// void UpdateObject()
//------------------------------------------------------------------------------
/*
 * Build and updates objects from a internal string stream without clearing the
 * resource.
 */
//------------------------------------------------------------------------------
void GmatInterface::UpdateObject()
{
  
   Moderator *moderator = Moderator::Instance();
   
   std::streambuf *streamBuf = mStringStream.rdbuf();

   // redirect mInStringStream into mStringStream
   RedirectBuffer(mInStringStream, streamBuf);
   
   #ifdef DEBUG_GMAT_INTERFACE
   //loj: 8/31/04 Why this causes problem for long scripts? buffer overflow?
   //MessageInterface::ShowMessage
   //   ("GmatInterface::UpdateObject() mStringStream.str=\n%s", mStringStream.str().c_str());
   //MessageInterface::ShowMessage
   //   ("GmatInterface::UpdateObject() mInStringStream.str=\n%s\n", mInStringStream->str().c_str());
   #endif
   
   // flag not to clear objects and mission sequence
   moderator->InterpretScript(mInStringStream, false);
   Moderator::GetUiInterpreter()->UpdateView(3);
   
   // empty the buffer, once objects are created
   mStringStream.str("");
}


//------------------------------------------------------------------------------
// void RunScript()
//------------------------------------------------------------------------------
/*
 * Executues commands from existing objects.
 */
//------------------------------------------------------------------------------
void GmatInterface::RunScript()
{
   #ifdef DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage
      ("GmatInterface::RunScript() entered. mPassedInterpreter=%d\n",
       mPassedInterpreter);
   #endif

   if (mPassedInterpreter)
      Moderator::Instance()->RunScript();
   
   Moderator::GetUiInterpreter()->UpdateView(4);
}


//------------------------------------------------------------------------------
// bool ExecuteCallback()
//------------------------------------------------------------------------------
bool GmatInterface::ExecuteCallback()
{
   // Check periodically for user interrupt (LOJ: 2014.09.08)
   static Integer checkCount = 0;
   static Integer interruptCheckFrequency = 2;
   
   #ifdef DEBUG_TEST_CALLBACK
      MessageInterface::ShowMessage("GmatInterface::ExecuteCallback being called ...\n");
   #endif
   if (callbackObj)
   {
      ++checkCount;
      if (checkCount == interruptCheckFrequency)
      {
         // Request UI to set input focus
         CheckUserInterrupt();
         checkCount = 0;
      }
      callbackObj->ExecuteCallback();
      return true;
   }
   else
   {
      MessageInterface::ShowMessage("call back object is NULL, so returning false\n");
      return false;
   }
}


//------------------------------------------------------------------------------
// bool RegisterCallbackServer(GmatBase *callbackObject)
//------------------------------------------------------------------------------
bool GmatInterface::RegisterCallbackServer(GmatBase *callbackObject)
{
   #ifdef DEBUG_TEST_CALLBACK
      MessageInterface::ShowMessage(
      "GmatInterface::RegisterCallbackServer being called with object %s \"%s\"...\n",
      callbackObject->GetTypeName().c_str(), callbackObject->GetName().c_str());
   #endif
   callbackObj = callbackObject;
   return true;
}


//------------------------------------------------------------------------------
// char* GetCallbackStatus()
//------------------------------------------------------------------------------
/*
 * @return the status of the callback execution ("Executing", "Completed").
 */
//------------------------------------------------------------------------------
char* GmatInterface::GetCallbackStatus()
{
   #ifdef DEBUG_TEST_CALLBACK
      MessageInterface::ShowMessage(
      "GmatInterface::GetCallbackStatus being called ...\n");
   #endif
   static char dataString[MAX_PARAM_VAL_STRING];
   static const char *executingStr = "Executing\0";
   static const char *completedStr = "Completed\0";
   if (!callbackObj) // not running a callback - why are you asking?
   {
      sprintf(dataString, "%s", completedStr);
   }
   else
   {
      if (callbackObj->IsCallbackExecuting())
         sprintf(dataString, "%s", executingStr);
      else
         sprintf(dataString, "%s", completedStr);
   }
   #ifdef DEBUG_TEST_CALLBACK
   MessageInterface::ShowMessage
      ("GmatInterface::GetCallbackStatus() dataString=<%s>\n", dataString);
   #endif
   return dataString;
}

//------------------------------------------------------------------------------
// void PutCallbackData(std::string &data)
//------------------------------------------------------------------------------
void  GmatInterface::PutCallbackData(std::string &data)
{
   #ifdef DEBUG_TEST_CALLBACK
      MessageInterface::ShowMessage(
      "GmatInterface::PutCallbackData being called with data = %s\n", data.c_str());
   #endif
   if (callbackObj)
   {
      if (!(callbackObj->PutCallbackData(data)))
         throw InterfaceException(
         "GmatInterface::Error setting callback data on callback server");
   }
}

//------------------------------------------------------------------------------
// void PutCallbackRealData(RealArray &data)
//------------------------------------------------------------------------------
void GmatInterface::PutCallbackRealData(RealArray &data)
{
   #ifdef DEBUG_TEST_CALLBACK
   MessageInterface::ShowMessage
      ("GmatInterface::PutCallbackRealData being called with data[0] = %f\n", data[0]);
   #endif
   if (callbackObj)
   {
      if (!(callbackObj->PutCallbackRealData(data)))
         throw InterfaceException(
         "GmatInterface::Error setting callback real data on callback server");
   }
}

//------------------------------------------------------------------------------
// char* GetCallbackResults()
//------------------------------------------------------------------------------
/*
 * @return the status of the callback execution ("Executing", "Completed").
 */
//------------------------------------------------------------------------------
char* GmatInterface::GetCallbackResults()
{
   #ifdef DEBUG_TEST_CALLBACK
      MessageInterface::ShowMessage(
      "GmatInterface::GetCallbackResults being called ...\n");
   #endif
   static char dataString[MAX_CALLBACK_DATA_VAL_STRING];
   static const char *errorStr = "ERROR!!\0";
   if (!callbackObj) // not running a callback - why are you asking?
   {
      sprintf(dataString, "%s", errorStr);
   }
   else
   {
      std::string results = callbackObj->GetCallbackResults();
      sprintf(dataString, "%s", results.c_str());
   }
   #ifdef DEBUG_TEST_CALLBACK
   MessageInterface::ShowMessage
      ("GmatInterface::GetCallbackData() dataString=<%s>\n", dataString);
   #endif
   return dataString;
}

//------------------------------------------------------------------------------
// char* GetRunState()
//------------------------------------------------------------------------------
/*
 * @return the state of system ("RUNNING", "PAUSED", "IDLE").
 */
//------------------------------------------------------------------------------
char* GmatInterface::GetRunState()
{
   static char dataString[MAX_PARAM_VAL_STRING];
   static const char *runningStr = "Running\0";
   static const char *pausedStr = "Paused\0";
   static const char *idleStr = "Idle\0";
   strcpy(dataString, idleStr);
   
   Gmat::RunState state = Moderator::Instance()->GetRunState();
   
   if (state == Gmat::RUNNING)
      sprintf(dataString, "%s", runningStr);
   else if (state == Gmat::PAUSED)
      sprintf(dataString, "%s", pausedStr);
   else if (state == Gmat::IDLE)
      sprintf(dataString, "%s", idleStr);
   else
      sprintf(dataString, "Unknown");
   
   #ifdef DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage
      ("GmatInterface::GetRunState() state=%d, dataString=<%s>\n", state,
       dataString);
   #endif
   
   return dataString;
}


//------------------------------------------------------------------------------
// char* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
/*
 * It retrieves a Parameter pointer from the Sandbox, if it is not found in the
 * Sandbox, it retrieves it from the Configuration.
 *
 * @return string value of the parameter in the Sandbox or in the Configuration.
 */
//------------------------------------------------------------------------------
char* GmatInterface::GetParameter(const std::string &name)
{
   #ifdef DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage
      ("GmatInterface::GetParameter() name=%s\n", name.c_str());
   #endif
   
   static char dataString[MAX_PARAM_VAL_STRING];
   static const char *undefindString = "-123456789.123456789\0";
   strcpy(dataString, undefindString);
   Parameter *param = NULL;
   GmatBase *obj = NULL;
   
   try
   {
      obj = Moderator::Instance()->GetInternalObject(name);
   }
   catch (BaseException &)
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** Could not find \"%s\" in the Sandbox. "
          "Trying Configuration...\n", name.c_str());
   }
   
   // if internal object not found, get configured object (loj: 2008.03.04)
   if (obj == NULL)
      param = Moderator::Instance()->GetParameter(name);
   else
      param = (Parameter*)obj;
   
   #ifdef DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage("   internal obj=%p, param=%p\n", obj, param);
   #endif
   
   if (param != NULL)
   {
      #ifdef DEBUG_GMAT_INTERFACE
      MessageInterface::ShowMessage
         ("GmatInterface::GetParameter() evaluate the parameter:%s, type=%s\n",
          param->GetName().c_str(), param->GetTypeName().c_str());
      #endif
      
      //loj: 2/16/05 param->Evaluate() causes system to crash!!
      // so just get the last value without evaluting
      //param->Evaluate(); 
      std::string str = param->ToString(); // returns last value
      str = "[" + str + "]";
      
      #ifdef DEBUG_GMAT_INTERFACE
      MessageInterface::ShowMessage
         ("GmatInterface::GetParameter() str=%s\n", str.c_str());
      #endif
      
      sprintf(dataString, "%s", str.c_str());
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** Could not find \"%s\" in the Configuration\n", name.c_str());
   }
   
   return dataString;
}


char* GmatInterface::GetGmatObject(const char *name)
{
   return GmatInterface::GetGmatObject(std::string(name));
}


//------------------------------------------------------------------------------
// char* GetGmatObject(const std::string &name)
//------------------------------------------------------------------------------
/*
 * It retrieves an object pointer from the Sandbox, if it is not found in the
 * Sandbox, it retrieves it from the Configuration.
 *
 * @return serialized string value of the internal object in the Sandbox or
 *         object in the Configuration
 */
//------------------------------------------------------------------------------
char* GmatInterface::GetGmatObject(const std::string &name)
{
   #ifdef DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage
      ("GmatInterface::GetGmatObject() name=%s\n", name.c_str());
   #endif
   
   static char dataString[MAX_OBJECT_VAL_STRING];
   static const char *undefindString = "-123456789.123456789\0";
   strcpy(dataString, undefindString);
   GmatBase *obj = NULL;
   
   try
   {
      obj = Moderator::Instance()->GetInternalObject(name);
   }
   catch (BaseException &)
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** Could not find \"%s\" in the Sandbox. "
          "Trying Configuration...\n", name.c_str());
      
      // if internal object not found, get configured object (loj: 2008.03.04)
      obj = Moderator::Instance()->GetConfiguredObject(name);
   }
   
   if (obj != NULL)
   {
      #ifdef DEBUG_GMAT_INTERFACE
      MessageInterface::ShowMessage
         ("GmatInterface::GetGmatObject() get serialized string of object name:"
          "%s, type=%s\n", obj->GetName().c_str(), obj->GetTypeName().c_str());
      #endif
      
      std::string str = obj->GetGeneratingString(Gmat::MATLAB_STRUCT);
      
      #ifdef DEBUG_GMAT_INTERFACE
      MessageInterface::ShowMessage("str=%s\n", str.c_str());
      #endif
      
      sprintf(dataString, "%s", str.c_str());
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** Could not find \"%s\" in the Configuration\n", name.c_str());
   }
   
   return dataString;
}


//------------------------------------------------------------------------------
// void CheckUserInterrupt()
//------------------------------------------------------------------------------
/*
 * Calls Moderator::SetUserInputFocus() to check if user interrupted the
 * mission sequence.
 */
//------------------------------------------------------------------------------
void GmatInterface::CheckUserInterrupt()
{
   Moderator::Instance()->GetUserInterrupt();
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// GmatInterface()
//------------------------------------------------------------------------------
GmatInterface::GmatInterface()
{
   mInStringStream = NULL;
   callbackObj     = NULL;
}


//------------------------------------------------------------------------------
// GmatInterface()
//------------------------------------------------------------------------------
GmatInterface::~GmatInterface()
{
   if (mInStringStream)
      delete mInStringStream;
}
  
