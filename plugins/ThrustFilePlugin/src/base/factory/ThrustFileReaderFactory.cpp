//------------------------------------------------------------------------------
//                           ThrustFileReaderFactory
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
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 14, 2016
/**
 * Factory used to create Thrust History File components
 */
//------------------------------------------------------------------------------

#include "ThrustFileReaderFactory.hpp"
#include "ThrustHistoryFile.hpp"
#include "ThrustSegment.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// ThrustFileReaderFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ThrustFileReaderFactory::ThrustFileReaderFactory() :
   Factory           (Gmat::INTERFACE)
{
   creatables.push_back("ThrustHistoryFile");
   creatables.push_back("ThrustSegment");
}

//------------------------------------------------------------------------------
// ThrustFileReaderFactory::~ThrustFileReaderFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThrustFileReaderFactory::~ThrustFileReaderFactory()
{
}

//------------------------------------------------------------------------------
// ThrustFileReaderFactory(const ThrustFileReaderFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
ThrustFileReaderFactory::ThrustFileReaderFactory(
      const ThrustFileReaderFactory& elf) :
   Factory        (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("ThrustHistoryFile");
      creatables.push_back("ThrustSegment");
   }
}

//------------------------------------------------------------------------------
// ThrustFileReaderFactory& operator=(const ThrustFileReaderFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment opertor
 *
 * @param elf The factory copied here
 *
 * @return This factory, matching elf
 */
//------------------------------------------------------------------------------
ThrustFileReaderFactory& ThrustFileReaderFactory::operator=(
      const ThrustFileReaderFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("ThrustHistoryFile");
         creatables.push_back("ThrustSegment");
      }
   }
   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string& ofType, const std::string& withName)
//------------------------------------------------------------------------------
/**
 * Object instantiator
 *
 * @param ofType The object type requested
 * @param withName The name of the new object
 *
 * @return A new object of the specified type
 */
//------------------------------------------------------------------------------
GmatBase* ThrustFileReaderFactory::CreateObject(const std::string& ofType,
      const std::string& withName)
{
   GmatBase *retval = NULL;

   if (ofType == "ThrustHistoryFile")
      retval = new ThrustHistoryFile(withName);
   if (ofType == "ThrustSegment")
      retval = new ThrustSegment(withName);
   // TBD - where should this call go?
   retval->SetPrecisionTimeFlag(true);

   return retval;
}
