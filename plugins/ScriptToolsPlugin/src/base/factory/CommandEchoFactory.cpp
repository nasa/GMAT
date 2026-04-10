//------------------------------------------------------------------------------
//                         CommandEchoFactory
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
/**
* Implementation of a factory used in the GMAT plugin CommandEcho
*/
//------------------------------------------------------------------------------


#include "CommandEchoFactory.hpp"
#include "CommandEcho.hpp"

//------------------------------------------------------------------------------
// CommandEchoFactory()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
CommandEchoFactory::CommandEchoFactory() :
Factory(Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("CommandEcho");
   }
}


//------------------------------------------------------------------------------
// ~CommandEchoFactory()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
CommandEchoFactory::~CommandEchoFactory()
{
}


//------------------------------------------------------------------------------
// CommandEchoFactory(const CommandEchoFactory& elf)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param elf The factory copied here
*/
//------------------------------------------------------------------------------
CommandEchoFactory::CommandEchoFactory(const CommandEchoFactory& elf) :
Factory(elf)
{
   if (creatables.empty())
   {
      creatables.push_back("CommandEcho");
   }
}


//------------------------------------------------------------------------------
// CommandEchoFactory& operator=(const CommandEchoFactory& elf)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param elf The factory copied to this one
*
* @return this instance, set to match elf
*/
//------------------------------------------------------------------------------
CommandEchoFactory& CommandEchoFactory::operator=(
	const CommandEchoFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         // Replace the SampleClass string here with your class name.  For multiple 
         // classes of the same type, push back multiple names here
         creatables.push_back("CommandEcho");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
* Creation method for GMAT commands
*
* @param ofType The subtype of the command
* @param withName The new command's name
*
* @return A newly created GmatCommand (or NULL if this factory doesn't create
*         the requested type)
*/
//------------------------------------------------------------------------------
GmatCommand* CommandEchoFactory::CreateCommand(const std::string &ofType,
   const std::string &withName)
{
   if (ofType == "CommandEcho")
      return new CommandEcho();

   return NULL;   // doesn't match any type of Command known by this factory
}
