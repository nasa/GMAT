//$Id$
//------------------------------------------------------------------------------
//                       GmatFunctionCommandFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 14, 2013
/**
 * Factory for commands associated with GMAT functions
 */
//------------------------------------------------------------------------------

#include "GmatFunctionCommandFactory.hpp"
#include "CallGmatFunction.hpp"
#include "Global.hpp"


//------------------------------------------------------------------------------
// GmatFunctionCommandFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
GmatFunctionCommandFactory::GmatFunctionCommandFactory() :
Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("CallGmatFunction");
      creatables.push_back("Global");
   }
}


//------------------------------------------------------------------------------
// ~GmatFunctionCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GmatFunctionCommandFactory::~GmatFunctionCommandFactory()
{
}


//------------------------------------------------------------------------------
// GmatFunctionCommandFactory(const GmatFunctionCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory that is copied to the new one
 */
//------------------------------------------------------------------------------
GmatFunctionCommandFactory::GmatFunctionCommandFactory(
      const GmatFunctionCommandFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("CallGmatFunction");
      creatables.push_back("Global");
   }
}


//------------------------------------------------------------------------------
// GmatFunctionCommandFactory& operator=(const GmatFunctionCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory that is copied to this one
 *
 * @return This factory, set to look like elf
 */
//------------------------------------------------------------------------------
GmatFunctionCommandFactory& GmatFunctionCommandFactory::operator=(
      const GmatFunctionCommandFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("CallGmatFunction");
         creatables.push_back("Global");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Method used to create the supported commands
 *
 * @param ofType The type of command being created
 * @param withName The command's name
 *
 * @return The new command
 */
//------------------------------------------------------------------------------
GmatCommand* GmatFunctionCommandFactory::CreateCommand(
      const std::string &ofType, const std::string &withName)
{
   if (ofType == "CallGmatFunction")
      return new CallGmatFunction;

   else if (ofType == "Global")
      return new Global();
   // add more here .......

   return NULL;   // doesn't match any type of Command known by this factory
}
