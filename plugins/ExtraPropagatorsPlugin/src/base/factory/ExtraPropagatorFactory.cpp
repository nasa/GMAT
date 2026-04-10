//------------------------------------------------------------------------------
//                            ExtraPropagatorFactory
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
// Created: May 3 2010
//
/**
 * Implementation code for the ExtraPropagatorFactory class, which creates objects that
 * extend the SRP model for solar sailing.
 *
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ExtraPropagatorFactory.hpp"
#include "BulirschStoer.hpp"

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
GmatBase* ExtraPropagatorFactory::CreateObject(const std::string &ofType,
                                    const std::string &withName)
{
   return CreatePropagator(ofType, withName);
}

//------------------------------------------------------------------------------
// Propagator* CreatePropagator(const std::string &ofType,
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns a PhysicalModel object.
 *
 * @param <ofType> type of PhysicalModel object to create and return.
 * @param <withName> the name for the newly-created PhysicalModel object.
 *
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Propagator* ExtraPropagatorFactory::CreatePropagator(const std::string &ofType,
                                    const std::string &withName)
{
   if (ofType == "BulirschStoer")
      return new BulirschStoer(withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  ExtraPropagatorFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExtraPropagatorFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
ExtraPropagatorFactory::ExtraPropagatorFactory() :
   Factory     (Gmat::PROPAGATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("BulirschStoer");
   }
}

//------------------------------------------------------------------------------
//  ExtraPropagatorFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExtraPropagatorFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
ExtraPropagatorFactory::ExtraPropagatorFactory(StringArray createList) :
   Factory(createList, Gmat::PROPAGATOR)
{
}


//------------------------------------------------------------------------------
//  ExtraPropagatorFactory(const ExtraPropagatorFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExtraPropagatorFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
ExtraPropagatorFactory::ExtraPropagatorFactory(const ExtraPropagatorFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("BulirschStoer");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * ExtraPropagatorFactory operator for the ExtraPropagatorFactory base class.
 *
 * @param <fact> the ExtraPropagatorFactory object that is copied.
 *
 * @return "this" ExtraPropagatorFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
ExtraPropagatorFactory& ExtraPropagatorFactory::operator=(const ExtraPropagatorFactory& fact)
{
   if (this != &fact)
   {
      Factory::operator=(fact);
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~ExtraPropagatorFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the ExtraPropagatorFactory base class.
 */
//------------------------------------------------------------------------------
ExtraPropagatorFactory::~ExtraPropagatorFactory()
{
}
