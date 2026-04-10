//------------------------------------------------------------------------------
//                              PlanetographicRegionFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number FDSS III
//
// Author: David E. Washington, Pearl River Technologies, Inc.
// Created: 2022.3.22
//
/**
 * Implementation code for the PlanetographicRegionFactory class, responsible for creating 
 * PlanetographicRegion objects.
 */
//------------------------------------------------------------------------------

#include "PlanetographicRegionFactory.hpp"
#include "PlanetographicRegion.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested PlanetographicRegion class 
 * in generic way.
 *
 * @param <ofType>   the burn object to create and return.
 * @param <withName> the name of the new object.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
GmatBase* PlanetographicRegionFactory::CreateObject(const std::string &ofType,
                                    const std::string &withName)
{
   return CreatePlanetographicRegion(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreatePlanetographicRegion(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Burn class 
 *
 * @param <ofType>   the burn object to create and return.
 * @param <withName> the name of the new object.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
PlanetographicRegion* PlanetographicRegionFactory::CreatePlanetographicRegion(const std::string& ofType,
                              const std::string &withName)
{
   if (ofType == "PlanetographicRegion")
      return new PlanetographicRegion(withName);
   // add more here .......

   return NULL;   // doesn't match any known type of region
}


//------------------------------------------------------------------------------
//  PlanetographicRegionFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PlanetographicRegionFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
PlanetographicRegionFactory::PlanetographicRegionFactory() :
   Factory(Gmat::REGION)
{
   if (creatables.empty())
   {  
      creatables.push_back("PlanetographicRegion");
   }

   GmatType::RegisterType(Gmat::REGION, "PlanetographicRegion");
}

//------------------------------------------------------------------------------
//  PlanetographicRegionFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class BurnFactory.
 *
 * @param <createList> list of creatable burn objects
 */
//------------------------------------------------------------------------------
PlanetographicRegionFactory::PlanetographicRegionFactory(const StringArray& createList) :
   Factory     (createList, Gmat::REGION)
{
}

//------------------------------------------------------------------------------
//  PlanetographicRegionFactory(const PlanetographicRegionFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class BurnFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
PlanetographicRegionFactory::PlanetographicRegionFactory(const PlanetographicRegionFactory &fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("PlanetographicRegion");
   }
}

//------------------------------------------------------------------------------
//  PlanetographicRegionFactory& operator= (const PlanetographicRegionFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the PlanetographicRegionFactory base class.
 *
 * @param <fact> the PlanetographicRegionFactory object whose data to assign to "this" factory.
 *
 * @return "this" PlanetographicRegionFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
PlanetographicRegionFactory& PlanetographicRegionFactory::operator=(const PlanetographicRegionFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~PlanetographicRegionFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the PlanetographicRegionFactory class.
 */
//------------------------------------------------------------------------------
PlanetographicRegionFactory::~PlanetographicRegionFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

