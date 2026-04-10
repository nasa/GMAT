//------------------------------------------------------------------------------
//                         FormationFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: January 10, 2013
//
/**
 * Implementation of a factory used to create Formations
 */
//------------------------------------------------------------------------------


#include "FormationFactory.hpp"
#include "Formation.hpp"

//------------------------------------------------------------------------------
// FormationFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
FormationFactory::FormationFactory() :
   Factory           (Gmat::FORMATION)
{
   if (creatables.empty())
   {
      creatables.push_back("Formation");
   }

   // Register supported types
   GmatType::RegisterType(Gmat::FORMATION, "Formation");
}


//------------------------------------------------------------------------------
// ~FormationFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
FormationFactory::~FormationFactory()
{
}


//------------------------------------------------------------------------------
// FormationFactory(const FormationFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
FormationFactory::FormationFactory(const FormationFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("Formation");
   }
}


//------------------------------------------------------------------------------
// FormationFactory& operator=(const FormationFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
FormationFactory& FormationFactory::operator=(
      const FormationFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("Formation");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for GMAT SpaceObjects
 *
 * @param ofType The subtype of the space object
 * @param withName The new command's name
 *
 * @return A newly created GmatCommand (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
GmatBase* FormationFactory::CreateObject(const std::string &ofType,
                                         const std::string &withName)
{
   return CreateFormation(ofType, withName);
}

//------------------------------------------------------------------------------
// SpaceObject* CreateFormation(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for GMAT Formation
 *
 * @param ofType The type name of the Formation
 * @param withName The new Formation name
 *
 * @return A newly created Formation (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
SpaceObject* FormationFactory::CreateFormation(const std::string &ofType,
                                               const std::string &withName)
{
   if (ofType == "Formation")
      return new Formation(Gmat::FORMATION, ofType, withName);
   
   return NULL;   // doesn't match any type of Command known by this factory
}
