//$Id: EventLocatorFactory.hpp 1914 2011-11-16 19:06:27Z  $
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
 * Definition of the factory used to create event locators in this plugin
 */
//------------------------------------------------------------------------------


#ifndef EventLocatorFactory_hpp
#define EventLocatorFactory_hpp

#include "Factory.hpp"
#include "EventLocatorDefs.hpp"


/**
 * Factory class that creates EventLocator objects
 */
class LOCATOR_API EventLocatorFactory: public Factory
{
public:
   EventLocatorFactory();
   virtual ~EventLocatorFactory();
   EventLocatorFactory(const EventLocatorFactory& elf);
   EventLocatorFactory& operator=(const EventLocatorFactory& elf);

   virtual GmatBase* CreateObject(const std::string &ofType,
                                  const std::string &withName);
   virtual EventLocator* CreateEventLocator(const std::string &ofType,
                                        const std::string &withName);
};

#endif /* EventLocatorFactory_hpp */
