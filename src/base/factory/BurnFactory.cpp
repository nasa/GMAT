//$Id$
//------------------------------------------------------------------------------
//                              BurnFactory
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
// number NNG04CC06P.
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2004/1/14
//
/**
 * Implementation code for the BurnFactory class, responsible for creating 
 * Burn objects.
 */
//------------------------------------------------------------------------------


#include "BurnFactory.hpp"
#include "ImpulsiveBurn.hpp"
#include "FiniteBurn.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Burn class 
 * in generic way.
 *
 * @param <ofType>   the burn object to create and return.
 * @param <withName> the name of the new object.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
GmatBase* BurnFactory::CreateObject(const std::string &ofType,
                                    const std::string &withName)
{
   return CreateBurn(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateBurn(const std::string &ofType, const std::string &withName)
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
Burn* BurnFactory::CreateBurn(const std::string &ofType,
                              const std::string &withName)
{
   if (ofType == "ImpulsiveBurn")
      return new ImpulsiveBurn(withName);
   else if (ofType == "FiniteBurn")
      return new FiniteBurn(withName);
   // add more here .......

   return NULL;   // doesn't match any known type of burn
}


//------------------------------------------------------------------------------
//  BurnFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class BurnFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
BurnFactory::BurnFactory() :
   Factory     (Gmat::BURN)
{
   if (creatables.empty())
   {
      creatables.push_back("ImpulsiveBurn");
      creatables.push_back("FiniteBurn"); 
   }

   GmatType::RegisterType(Gmat::BURN, "Burn");
   GmatType::RegisterType(Gmat::IMPULSIVE_BURN, "ImpulsiveBurn");
//   GmatType::RegisterType(Gmat::IMPULSIVE_BURN, "ConstrainedImpulsiveBurn");
   GmatType::RegisterType(Gmat::FINITE_BURN, "FiniteBurn");
   GmatType::RegisterType(Gmat::TRANSIENT_FORCE, "TransientForce");  // Owned force
}

//------------------------------------------------------------------------------
//  BurnFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class BurnFactory.
 *
 * @param <createList> list of creatable burn objects
 */
//------------------------------------------------------------------------------
BurnFactory::BurnFactory(StringArray createList) :
   Factory     (createList, Gmat::BURN)
{
}

//------------------------------------------------------------------------------
//  BurnFactory(const BurnFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class BurnFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
BurnFactory::BurnFactory(const BurnFactory &fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("ImpulsiveBurn");
      creatables.push_back("FiniteBurn");
   }
}

//------------------------------------------------------------------------------
//  BurnFactory& operator= (const BurnFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the BurnFactory base class.
 *
 * @param <fact> the BurnFactory object whose data to assign to "this" factory.
 *
 * @return "this" BurnFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
BurnFactory& BurnFactory::operator=(const BurnFactory &fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~BurnFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the BurnFactory class.
 */
//------------------------------------------------------------------------------
BurnFactory::~BurnFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

