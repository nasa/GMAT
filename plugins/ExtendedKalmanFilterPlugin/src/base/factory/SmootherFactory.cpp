//$Id$
//------------------------------------------------------------------------------
//                            SmootherFactory
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
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2019/07/12
//
/**
 *  Implementation code for the SmootherFactory class, responsible
 *  for creating Smoother objects.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SmootherFactory.hpp"
#include "Smoother.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested ODEModel class
 * in generic way.
 *
 * @param <ofType> the ODEModel object to create and return.
 * @param <withName> the name to give the newly-created ODEModel object.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SmootherFactory::CreateObject(const std::string &ofType,
                                          const std::string &withName)
{
   return CreateSmoother(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateSmoother(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Smoother class 
 *
 * @param <ofType> the Smoother object to create and return.
 * @param <withName> the name to give the newly-created Smoother object.
 */
//------------------------------------------------------------------------------
Smoother* SmootherFactory::CreateSmoother(const std::string &ofType,
                                          const std::string &withName)
{
   if (ofType == "Smoother")
      return new Smoother(withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  SmootherFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SmootherFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
SmootherFactory::SmootherFactory() :
   Factory(GmatType::RegisterType("Smoother"))
{
   if (creatables.empty())
   {
      creatables.push_back("Smoother");
   }
}

//------------------------------------------------------------------------------
//  SmootherFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SmootherFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects
 *
 */
//------------------------------------------------------------------------------
SmootherFactory::SmootherFactory(StringArray createList) :
   Factory(createList, GmatType::RegisterType("Smoother"))
{
}

//------------------------------------------------------------------------------
//  SmootherFactory(const SmootherFactory &fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class SmootherFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
SmootherFactory::SmootherFactory(const SmootherFactory &fact) 
   :
   Factory(fact)
{

}

//------------------------------------------------------------------------------
//  SmootherFactory& operator= (const SmootherFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SmootherFactory class.
 *
 * @param <fact> the SmootherFactory object whose data to assign to "this"
 *  factory.
 *
 * @return "this" SmootherFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
SmootherFactory& SmootherFactory::operator= (const SmootherFactory &fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~SmootherFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the SmootherFactory class.
   *
   */
//------------------------------------------------------------------------------
SmootherFactory::~SmootherFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
