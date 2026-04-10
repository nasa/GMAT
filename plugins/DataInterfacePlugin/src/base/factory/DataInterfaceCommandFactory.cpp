//$Id$
//------------------------------------------------------------------------------
//                         DataInterfaceCommandFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Created: September 20, 2012
//
/**
 * Implementation of a factory used in a GMAT plugin
 */
//------------------------------------------------------------------------------


#include "DataInterfaceCommandFactory.hpp"
#include "Set.hpp"

//------------------------------------------------------------------------------
// DataInterfaceCommandFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
DataInterfaceCommandFactory::DataInterfaceCommandFactory() :
   Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("Set");
   }
}


//------------------------------------------------------------------------------
// ~DataInterfaceCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DataInterfaceCommandFactory::~DataInterfaceCommandFactory()
{
}


//------------------------------------------------------------------------------
// DataInterfaceCommandFactory(const DataInterfaceCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
DataInterfaceCommandFactory::DataInterfaceCommandFactory(const DataInterfaceCommandFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("Set");
   }
}


//------------------------------------------------------------------------------
// DataInterfaceCommandFactory& operator=(const DataInterfaceCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
DataInterfaceCommandFactory& DataInterfaceCommandFactory::operator=(
      const DataInterfaceCommandFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         // Replace the SampleClass string here with your class name.  For multiple 
         // classes of the same type, push back multiple names here
         creatables.push_back("Set");
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
GmatCommand* DataInterfaceCommandFactory::CreateCommand(const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "Set")
      return new Set();
   // add more here .......

   return NULL;   // doesn't match any type of Command known by this factory
}
