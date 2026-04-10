//$Id: EventLocatorFactory.cpp 1914 2011-11-16 19:06:27Z  $
//------------------------------------------------------------------------------
//                           EventLocatorFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 6, 2011
//
/**
 * Implementation of the factory used to create event locators in this plugin
 */
//------------------------------------------------------------------------------


#include "EventLocatorFactory.hpp"
#include "ContactLocator.hpp"
#include "EclipseLocator.hpp"
#include "IntrusionLocator.hpp"

#define INCLUDE_CONTACT

//------------------------------------------------------------------------------
// EventLocatorFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
EventLocatorFactory::EventLocatorFactory() :
   Factory           (Gmat::EVENT_LOCATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("EclipseLocator");
#ifdef INCLUDE_CONTACT
      creatables.push_back("ContactLocator");
#endif
      creatables.push_back("IntrusionLocator");
   }

   GmatType::RegisterType(Gmat::EVENT_LOCATOR, "EventLocator");
}


//------------------------------------------------------------------------------
// ~EventLocatorFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EventLocatorFactory::~EventLocatorFactory()
{
}


//------------------------------------------------------------------------------
// EventLocatorFactory(const EventLocatorFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
EventLocatorFactory::EventLocatorFactory(const EventLocatorFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("EclipseLocator");
#ifdef INCLUDE_CONTACT
      creatables.push_back("ContactLocator");
#endif
      creatables.push_back("IntrusionLocator");
   }
}


//------------------------------------------------------------------------------
// EventLocatorFactory& operator=(const EventLocatorFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
EventLocatorFactory& EventLocatorFactory::operator=(
      const EventLocatorFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("EclipseLocator");
#ifdef INCLUDE_CONTACT
         creatables.push_back("ContactLocator");
#endif
         creatables.push_back("IntrusionLocator");
      }
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for EventLocators
 *
 * @param ofType The subtype of the event locator
 * @param withName The new locator's name
 *
 * @return A newly created EventLocator (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
GmatBase* EventLocatorFactory::CreateObject(const std::string &ofType,
                                            const std::string &withName)
{
   return CreateEventLocator(ofType, withName);
}

//------------------------------------------------------------------------------
// EventLocator* CreateEventLocator(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for EventLocators
 *
 * @param ofType The subtype of the event locator
 * @param withName The new locator's name
 *
 * @return A newly created EventLocator (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
EventLocator* EventLocatorFactory::CreateEventLocator(const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "EclipseLocator")
      return new EclipseLocator(withName);
#ifdef INCLUDE_CONTACT
   else if (ofType == "ContactLocator")
      return new ContactLocator(withName);
#endif
   else if (ofType == "IntrusionLocator")
      return new IntrusionLocator(withName);
   // add more here .......

   return NULL;   // doesn't match any known type of EventLocator
}
