//$Id$
//------------------------------------------------------------------------------
//                            CelestialBodyFactory
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
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2004/01/29
//
/**
 *  Implementation code for the CelestialBodyFactory class, responsible for
 *  creating CelestialBody objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "CelestialBodyFactory.hpp"
#include "Star.hpp"
#include "Planet.hpp"
#include "Moon.hpp"
#include "Comet.hpp"
#include "Asteroid.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateObject(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested CelestialBody 
 * class.
 *
 * @param <ofType> the CelestialBody object to create and return.
 * @param <withName> the name to give the newly-created CelestialBody object.
 *
 * @return a new 
 *
 */
//------------------------------------------------------------------------------
CelestialBody* CelestialBodyFactory::CreateObject(const std::string &ofType,
                                                  const std::string &withName)
{
   return CreateCelestialBody(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateCelestialBody(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested CelestialBody 
 * class.
 *
 * @param <ofType> the CelestialBody object to create and return.
 * @param <withName> the name to give the newly-created CelestialBody object.
 *
 * @return a new 
 *
 */
//------------------------------------------------------------------------------
CelestialBody* CelestialBodyFactory::CreateCelestialBody(
                                     const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "Star")
      return new Star(withName);
   else if (ofType == "Planet")
      return new Planet(withName);
   else if (ofType == "Moon")
      return new Moon(withName);
   else if (ofType == "Comet")
      return new Comet(withName);
   else if (ofType == "Asteroid")
      return new Asteroid(withName);
   // add more here ??  KBOs?  .......
   else
   {
      return NULL;   // doesn't match any known type of command
   }
}

//------------------------------------------------------------------------------
//  CelestialBodyFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CelestialBodyFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
CelestialBodyFactory::CelestialBodyFactory() :
Factory(Gmat::CELESTIAL_BODY)
{
   if (creatables.empty())
   {
      creatables.push_back("Star");
      creatables.push_back("Planet");
      creatables.push_back("Moon");
      creatables.push_back("Comet");
      creatables.push_back("Asteroid");
   }

   GmatType::RegisterType(Gmat::SPACE_POINT, "SpacePoint");
   GmatType::RegisterType(Gmat::CELESTIAL_BODY, "CelestialBody");
}

//------------------------------------------------------------------------------
//  CelestialBodyFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CelestialBodyFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
CelestialBodyFactory::CelestialBodyFactory(const StringArray &createList) :
Factory(createList,Gmat::CELESTIAL_BODY)
{
   if (creatables.empty())
   {
      creatables.push_back("Star");
      creatables.push_back("Planet");
      creatables.push_back("Moon");
      creatables.push_back("Comet");
      creatables.push_back("Asteroid");
   }
}

//------------------------------------------------------------------------------
//  CelestialBodyFactory(const CelestialBodyFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CelestialBodyFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
CelestialBodyFactory::CelestialBodyFactory(const CelestialBodyFactory &fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("Star");
      creatables.push_back("Planet");
      creatables.push_back("Moon");
      creatables.push_back("Comet");
      creatables.push_back("Asteroid");
   }
}

//------------------------------------------------------------------------------
//  CelestialBodyFactory& operator= (const CelestialBodyFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the CelestialBodyFactory class.
 *
 * @param <fact> the CelestialBodyFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" CelestialBodyFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
CelestialBodyFactory& CelestialBodyFactory::operator= (
                                             const CelestialBodyFactory &fact)
{
   if (this == &fact) return *this;
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("Star");
      creatables.push_back("Planet");
      creatables.push_back("Moon");
      creatables.push_back("Comet");
      creatables.push_back("Asteroid");
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~CelestialBodyFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the CelestialBodyFactory base class.
 *
 */
//------------------------------------------------------------------------------
CelestialBodyFactory::~CelestialBodyFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------





