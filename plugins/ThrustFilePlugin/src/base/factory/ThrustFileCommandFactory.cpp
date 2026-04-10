//------------------------------------------------------------------------------
//                        ThrustFileCommandFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 *
 */
//------------------------------------------------------------------------------


#include "ThrustFileCommandFactory.hpp"
#include "BeginFileThrust.hpp"
#include "EndFileThrust.hpp"


//------------------------------------------------------------------------------
// ThrustFileCommandFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
ThrustFileCommandFactory::ThrustFileCommandFactory() :
   Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("BeginFileThrust");
      creatables.push_back("EndFileThrust");
   }
}


//------------------------------------------------------------------------------
// ~ThrustFileCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThrustFileCommandFactory::~ThrustFileCommandFactory()
{
}


//------------------------------------------------------------------------------
// ThrustFileCommandFactory(const ThrustFileCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
ThrustFileCommandFactory::ThrustFileCommandFactory(const ThrustFileCommandFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("BeginFileThrust");
      creatables.push_back("EndFileThrust");
   }
}


//------------------------------------------------------------------------------
// ThrustFileCommandFactory& operator=(const ThrustFileCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
ThrustFileCommandFactory& ThrustFileCommandFactory::operator=(
      const ThrustFileCommandFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("BeginFileThrust");
         creatables.push_back("EndFileThrust");
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
GmatCommand* ThrustFileCommandFactory::CreateCommand(const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "BeginFileThrust")
      return new BeginFileThrust(withName);
   if (ofType == "EndFileThrust")
      return new EndFileThrust(withName);

   return NULL;
}
