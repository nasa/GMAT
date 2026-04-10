//------------------------------------------------------------------------------
//                           GmatType
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under purchase
// order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 2, 2017
/**
 * Implements the run time type system used in GMAT
 */
//------------------------------------------------------------------------------


#include "GmatType.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_TYPE_REGISTRATION


/// Init the singleton
GmatType *GmatType::mapper = NULL;

//------------------------------------------------------------------------------
// static GmatType* Instance()
//------------------------------------------------------------------------------
/**
 * Access method for the singleton
 *
 * @return The GmatType mamager singleton
 */
//------------------------------------------------------------------------------
GmatType* GmatType::Instance()
{
   if (mapper == NULL)
      mapper = new GmatType();
   return mapper;
}

//---------------------------------------------------------------------------
// static void RegisterType(UnsignedInt id, std::string& label)
//---------------------------------------------------------------------------
/**
 * Method used to register types in GMAT when both the id and label are known
 *
 * @param id ObjectType ID for the label
 * @param label Strinc name for objects of type id.
 */
//---------------------------------------------------------------------------
void GmatType::RegisterType(const UnsignedInt id, const std::string& label)
{
   if (mapper == NULL)
      Instance();

   if (mapper->idToString.find(id) == mapper->idToString.end())
   {
      if (id == Gmat::USER_OBJECT_ID_NEEDED)
         RegisterType(label);
      else
      {
         mapper->idToString[id] = label;
         mapper->stringToId[label] = id;
      }
   }
   else
   {
      if (id != mapper->stringToId[label])
         MessageInterface::ShowMessage("Type %s has inconsistent type ids %d "
               "(input) and %d (mapped)\n", id, mapper->stringToId[label]);
   }

   #ifdef DEBUG_TYPE_REGISTRATION
      MessageInterface::ShowMessage("Registering known type %s with id %d\n",
            label.c_str(), id);
   #endif
}

//---------------------------------------------------------------------------
// static UnsignedInt RegisterType(const std::string& label)
//---------------------------------------------------------------------------
/**
 * Method used to register types in GMAT when only a label is known
 *
 * This method is used to register user types in the system
 *
 * @param label String name for the new object type.
 *
 * @return The ID associated with the label
 */
//---------------------------------------------------------------------------
UnsignedInt GmatType::RegisterType(const std::string& label)
{
   if (mapper == NULL)
      Instance();

   UnsignedInt id = Gmat::UNKNOWN_OBJECT;

   if (mapper->stringToId.find(label) == mapper->stringToId.end())
   {
      id = mapper->nextUserId++;

      mapper->idToString[id] = label;
      mapper->stringToId[label] = id;
   }
   else
   {
      id = mapper->stringToId[label];
   }

   #ifdef DEBUG_TYPE_REGISTRATION
      MessageInterface::ShowMessage("Registering user type %s with id %d\n",
            label.c_str(), id);
   #endif

   return id;
}

//---------------------------------------------------------------------------
// static UnsignedInt GetTypeId(const std::string label)
//---------------------------------------------------------------------------
/**
 * Retrieves the ID associated with the type name of an object
 *
 * @param label The type name
 *
 * @return The ID
 */
//---------------------------------------------------------------------------
UnsignedInt GmatType::GetTypeId(const std::string label)
{
   UnsignedInt retval = Gmat::UNKNOWN_OBJECT;

   if (mapper == NULL)
      Instance();
   if (mapper->stringToId.find(label) != mapper->stringToId.end())
      retval = mapper->stringToId[label];

   #ifdef DEBUG_TYPE_REGISTRATION
      MessageInterface::ShowMessage("Returning type id %d for type %s\n",
            retval, label.c_str());
   #endif

   return retval;
}

//---------------------------------------------------------------------------
// static std::string GetTypeName(const UnsignedInt id)
//---------------------------------------------------------------------------
/**
 * Retrieves the string name of an object's type from it's type ID
 *
 * Note that this may not be the leaf class name.
 *
 * @param id The type ID being accessed
 *
 * @return The label for the object type
 */
//---------------------------------------------------------------------------
std::string GmatType::GetTypeName(const UnsignedInt id)
{
   std::string retval = "UnknownObject";

   if (mapper == NULL)
      Instance();

   if (mapper->idToString.find(id) != mapper->idToString.end())
      retval = mapper->idToString[id];

   #ifdef DEBUG_TYPE_REGISTRATION
      MessageInterface::ShowMessage("Returning type name %s for type id %d\n",
            retval.c_str(), id);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// Private Members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// GmatType()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
GmatType::GmatType()
{
}


//------------------------------------------------------------------------------
// ~GmatType()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GmatType::~GmatType()
{
}
