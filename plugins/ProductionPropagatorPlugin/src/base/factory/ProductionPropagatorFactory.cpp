//------------------------------------------------------------------------------
//                            ProductionPropagatorFactory
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
// Author: Darrel Conway
// Created: Nov 6, 2013
//
/**
 * Implementation code for the ProductionPropagatorFactory class, which creates objects that
 * extend the SRP model for solar sailing.
 *
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ProductionPropagatorFactory.hpp"
#include "PrinceDormand853.hpp"

#include "MessageInterface.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * @see CreatePropagator()
 */
//------------------------------------------------------------------------------
GmatBase* ProductionPropagatorFactory::CreateObject(const std::string &ofType,
                                    const std::string &withName)
{
   return CreatePropagator(ofType, withName);
}

//------------------------------------------------------------------------------
//  Propagator* CreatePropagator(const std::string &ofType,
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns a Propagator object.
 *
 * @param <ofType> type of Propagator object to create and return.
 * @param <withName> the name for the newly-created Propagator object.
 *
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Propagator* ProductionPropagatorFactory::CreatePropagator(const std::string &ofType,
                                    const std::string &withName)
{
   if (ofType == "PrinceDormand853")
      return new PrinceDormand853(withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  ProductionPropagatorFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProductionPropagatorFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
ProductionPropagatorFactory::ProductionPropagatorFactory() :
   Factory     (Gmat::PROPAGATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("PrinceDormand853");
   }
}

//------------------------------------------------------------------------------
//  ProductionPropagatorFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProductionPropagatorFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
ProductionPropagatorFactory::ProductionPropagatorFactory(StringArray createList) :
   Factory(createList, Gmat::PROPAGATOR)
{
}


//------------------------------------------------------------------------------
//  ProductionPropagatorFactory(const ProductionPropagatorFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProductionPropagatorFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
ProductionPropagatorFactory::ProductionPropagatorFactory(const ProductionPropagatorFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("PrinceDormand853");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * ProductionPropagatorFactory operator for the ProductionPropagatorFactory base class.
 *
 * @param <fact> the ProductionPropagatorFactory object that is copied.
 *
 * @return "this" ProductionPropagatorFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
ProductionPropagatorFactory& ProductionPropagatorFactory::operator=(const ProductionPropagatorFactory& fact)
{
   if (this != &fact)
   {
      Factory::operator=(fact);
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~ProductionPropagatorFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the ProductionPropagatorFactory base class.
 */
//------------------------------------------------------------------------------
ProductionPropagatorFactory::~ProductionPropagatorFactory()
{
}
