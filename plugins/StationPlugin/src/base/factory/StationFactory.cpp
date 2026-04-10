//$Id$
//------------------------------------------------------------------------------
//                         StationFactory
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
// contract, Task Order 28.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: January 10, 2013
//
/**
 * Implementation of a factory used to create (ground)stations
 */
//------------------------------------------------------------------------------


#include "StationFactory.hpp"

// Here you add include statements for each class that can be instantiated in
// this factory.  The example here is a SampleClass object.
#include "GroundStation.hpp"

//------------------------------------------------------------------------------
// StationFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
StationFactory::StationFactory() :
   Factory           (Gmat::SPACE_POINT)
{
   if (creatables.empty())
   {
      creatables.push_back("GroundStation");
   }

   GmatType::RegisterType(Gmat::GROUND_STATION, "GroundStation");
   GmatType::RegisterType(Gmat::BODY_FIXED_POINT, "BodyFixedPoint");
}


//------------------------------------------------------------------------------
// ~StationFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
StationFactory::~StationFactory()
{
}


//------------------------------------------------------------------------------
// StationFactory(const StationFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
StationFactory::StationFactory(const StationFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("GroundStation");
   }
}


//------------------------------------------------------------------------------
// StationFactory& operator=(const StationFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
StationFactory& StationFactory::operator=(
      const StationFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("GroundStation");
      }
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for GMAT SpacePoint
 *
 * @param ofType The subtype of the SpacePoint
 * @param withName The name of type to be created
 *
 * @return A newly created SpacePoint (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
GmatBase* StationFactory::CreateObject(const std::string &ofType,
                                         const std::string &withName)
{
   return CreateSpacePoint(ofType, withName);
}

//------------------------------------------------------------------------------
// SpacePoint* CreateSpacePoint(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for GMAT SpacePoint
 *
 * @param ofType The subtype of the SpacePoint
 * @param withName The name of type to be created
 *
 * @return A newly created SpacePoint (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
SpacePoint* StationFactory::CreateSpacePoint(const std::string &ofType,
                                             const std::string &withName)
{
   if (ofType == "GroundStation")
      return new GroundStation(withName);
   // add more here .......

   return NULL;   // doesn't match any type of Command known by this factory
}

