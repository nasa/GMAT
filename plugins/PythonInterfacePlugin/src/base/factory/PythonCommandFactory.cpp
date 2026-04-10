//$Id$
//------------------------------------------------------------------------------
//                                 PythonCommandFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II .
//
// Author: Farideh Farahnak
// Created: 2015/02/23
//
/**
 * Factory used to create scripted Python commands
 */
//------------------------------------------------------------------------------

#include "PythonCommandFactory.hpp"
#include "CallPythonFunction.hpp"

//------------------------------------------------------------------------------
// PythonCommandFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
PythonCommandFactory::PythonCommandFactory() :
   Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("CallPythonFunction");
   }
}


//------------------------------------------------------------------------------
// ~PythonCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PythonCommandFactory::~PythonCommandFactory()
{
}


//------------------------------------------------------------------------------
// PythonCommandFactory(const PythonCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
PythonCommandFactory::PythonCommandFactory(const PythonCommandFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("CallPythonFunction");
   }
}


//------------------------------------------------------------------------------
// PythonCommandFactory& operator=(const PythonCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
PythonCommandFactory& PythonCommandFactory::operator=(
      const PythonCommandFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("CallPythonFunction");
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
GmatCommand* PythonCommandFactory::CreateCommand(const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "CallPythonFunction")
      return new CallPythonFunction();

   return NULL;   // doesn't match any type of Command known by this factory
}
