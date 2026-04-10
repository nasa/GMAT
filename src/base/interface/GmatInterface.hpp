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
 * Declares class to provide sending scripts from MATLAB to GMAT or
 * requesting GMAT data from MATLAB.
 */
//------------------------------------------------------------------------------
#ifndef GmatInterface_hpp
#define GmatInterface_hpp

#include <sstream>       // for stringstream, istringstream
#include "gmatdefs.hpp"
#include "GmatCommand.hpp"

class GMAT_API GmatInterface
{
public:
   static GmatInterface* Instance();
   ~GmatInterface();
   
   void OpenScript();
   void ClearScript();
   void PutScript(char *str);
   void BuildObject();
   void UpdateObject();
   void RunScript();
   
   // methods to manage execution of GMAT callback
   bool  ExecuteCallback();
   bool  RegisterCallbackServer(GmatBase *callbackObject);
   char* GetCallbackStatus();
   void  PutCallbackData(std::string &data);
   void  PutCallbackRealData(RealArray &data);
   char* GetCallbackResults();
   
   char* GetRunState();
   char* GetGmatObject(const char *name);
   char* GetGmatObject(const std::string &name);
   char* GetParameter(const std::string &name);
   
   void CheckUserInterrupt();
   
private:
   static const int MAX_PARAM_VAL_STRING = 512;
   static const int MAX_OBJECT_VAL_STRING = 4096;
   static const int MAX_CALLBACK_DATA_VAL_STRING = 1024;
   
   void RedirectBuffer(std::ios *stream, std::streambuf* newBuff) 
      { stream->rdbuf(newBuff); }
   
   GmatInterface();
   
   std::stringstream mStringStream;
   std::istringstream *mInStringStream;
   static GmatInterface *instance;
   static bool mPassedInterpreter;
   
   /// class containing callback function
   GmatBase *callbackObj;  
};
#endif
