//$Id$
//------------------------------------------------------------------------------
//                             ReaderFactory
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
//
/**
 * Definition of the factory used to create DataReaders
 */
//------------------------------------------------------------------------------


#ifndef ReaderFactory_hpp
#define ReaderFactory_hpp

#include "DataInterfaceDefs.hpp"
#include "Factory.hpp"


/**
 * Factory class that creates one class of your plugin's objects
 *
 * Rename this class from SaveCommandFactory( to something more descriptive of your 
 * plugin object(s).
 */
class DATAINTERFACE_API ReaderFactory : public Factory
{
public:
   ReaderFactory();
   virtual ~ReaderFactory();
   ReaderFactory(const ReaderFactory& elf);
   ReaderFactory& operator=(const ReaderFactory& elf);

   virtual GmatBase* CreateObject(const std::string &ofType,
                                  const std::string &withName);
};

#endif /* ReaderFactory_hpp */
