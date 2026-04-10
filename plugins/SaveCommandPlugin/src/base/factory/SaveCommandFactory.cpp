//------------------------------------------------------------------------------
//                         SaveCommandFactory
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: September 20, 2012
//
/**
 * Implementation of a factory used in a GMAT plugin
 *
 * Replace code in the indicated places for your Factory
 */
//------------------------------------------------------------------------------


#include "SaveCommandFactory.hpp"
#include "Save.hpp"

//------------------------------------------------------------------------------
// SaveCommandFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
SaveCommandFactory::SaveCommandFactory() :
   Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("Save");
   }
}


//------------------------------------------------------------------------------
// ~SaveCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SaveCommandFactory::~SaveCommandFactory()
{
}


//------------------------------------------------------------------------------
// SaveCommandFactory(const SaveCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
SaveCommandFactory::SaveCommandFactory(const SaveCommandFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("Save");
   }
}


//------------------------------------------------------------------------------
// SaveCommandFactory& operator=(const SaveCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
SaveCommandFactory& SaveCommandFactory::operator=(
      const SaveCommandFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         // Replace the SampleClass string here with your class name.  For multiple 
         // classes of the same type, push back multiple names here
         creatables.push_back("Save");
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
GmatCommand* SaveCommandFactory::CreateCommand(const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "Save")
      return new Save();
   // add more here .......

   return NULL;   // doesn't match any type of Command known by this factory
}
