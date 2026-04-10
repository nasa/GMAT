//------------------------------------------------------------------------------
//                          CommandEchoFactory
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
// Free usage granted to all users; this is shell code.  Adapt and enjoy.  
// Attribution is appreciated.
//
// Author: Joshua J. Raymond, Thinking Systems, Inc.
// Created: June 23, 2017
//
//
/**
* Definition of the factory used to create message writing commands in
* CommandEcho plugin
*/
//------------------------------------------------------------------------------


#ifndef CommandEchoFactory_hpp
#define CommandEchoFactory_hpp

#include "ScriptToolsDefs.hpp"
#include "Factory.hpp"


/**
* Factory class that creates one class of your plugin's objects
*/
class ScriptTools_API CommandEchoFactory : public Factory
{
public:
   CommandEchoFactory();
   virtual ~CommandEchoFactory();
   CommandEchoFactory(const CommandEchoFactory& elf);
   CommandEchoFactory& operator=(const CommandEchoFactory& elf);

   virtual GmatCommand*     CreateCommand(const std::string &ofType,
      const std::string &withName = "");
};

#endif /* CommandEchoFactory_hpp */
