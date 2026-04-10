//$Id$
//------------------------------------------------------------------------------
//                         ReaderFactory
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


#include "ReaderFactory.hpp"
#include "MessageInterface.hpp"

// Supported subtypes
#include "TcopsVHFAscii.hpp"

//------------------------------------------------------------------------------
// ReaderFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ReaderFactory::ReaderFactory() :
   Factory           (Gmat::INTERFACE)
{
   if (creatables.empty())
   {
      creatables.push_back("TVHF_ASCII");
   }
}


//------------------------------------------------------------------------------
// ~ReaderFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ReaderFactory::~ReaderFactory()
{
}


//------------------------------------------------------------------------------
// ReaderFactory(const ReaderFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
ReaderFactory::ReaderFactory(const ReaderFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("TVHF_ASCII");
   }
}


//------------------------------------------------------------------------------
// ReaderFactory& operator=(const ReaderFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
ReaderFactory& ReaderFactory::operator=(
      const ReaderFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("TVHF_ASCII");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
//GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Returns a GmatBase object pointer to a new object.
 *
 * @param ofType   specific type of Spacecraft object to create.
 * @param withName name to give to the newly created Spacecraft object.
 *
 * @return pointer to a new Spacecraft object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of the requested type.
 */
//------------------------------------------------------------------------------
GmatBase* ReaderFactory::CreateObject(const std::string &ofType,
                                const std::string &withName)
{
   GmatBase *retval = NULL;

   if (ofType == "TVHF_ASCII")
      retval = new TcopsVHFAscii(withName);

   if (retval != NULL)
   {
      if (retval->IsOfType("DataReader") == false)
      {
         delete retval;
         retval = NULL;
         MessageInterface::ShowMessage("The Reader Factory can only create "
               "DataReader subobjects; %s is not a DataReader\n", ofType.c_str());
      }
   }

   return retval;
}

