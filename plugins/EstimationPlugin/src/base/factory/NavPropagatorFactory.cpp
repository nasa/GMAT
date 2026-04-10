//------------------------------------------------------------------------------
//                            NavPropagatorFactory
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
// Author: Darrel J. Conway (Thinking Systems, Inc.)
// Created: 2015/12/15
//
/**
 *  Implementation code for the NavPropagatorFactory class, responsible
 *  for creating Propagator objects exclusive to the Nav code.
 */
//------------------------------------------------------------------------------
#include "NavPropagatorFactory.hpp"

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "RungeKutta4.hpp"

#include "MessageInterface.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateObject(const std::string &ofType, const std::string &withName)
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
GmatBase* NavPropagatorFactory::CreateObject(const std::string &ofType,
                                             const std::string &withName)
{
   return CreatePropagator(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreatePropagator(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Propagator class 
 *
 * @param <ofType> the Propagator object to create and return.
 * @param <withName> the name to give the newly-created Propagator object.
 */
//------------------------------------------------------------------------------
Propagator* NavPropagatorFactory::CreatePropagator(const std::string &ofType,
                                                const std::string &withName)
{
   // RK4 added for fast fixed step propagation in the Nav code
   if (ofType == "RungeKutta4")
      return new RungeKutta4(withName);

   // Add others here as needed
   return NULL;
}


//------------------------------------------------------------------------------
//  NavPropagatorFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class NavPropagatorFactory
 * (default constructor).
 */
//------------------------------------------------------------------------------
NavPropagatorFactory::NavPropagatorFactory() :
   Factory(Gmat::PROPAGATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("RungeKutta4");
   }
}

//------------------------------------------------------------------------------
//  NavPropagatorFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class NavPropagatorFactory
 * (constructor).
 *
 * @param createList Initial list of creatable objects
 *
 */
//------------------------------------------------------------------------------
NavPropagatorFactory::NavPropagatorFactory(StringArray createList) :
   Factory(createList, Gmat::PROPAGATOR)
{
}

//------------------------------------------------------------------------------
//  NavPropagatorFactory(const NavPropagatorFactory &fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class NavPropagatorFactory
   * (copy constructor).
   *
   * @param fact the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
NavPropagatorFactory::NavPropagatorFactory(const NavPropagatorFactory &fact) :
   Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("RungeKutta4");
   }
}

//------------------------------------------------------------------------------
//  NavPropagatorFactory& operator= (const NavPropagatorFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the NavPropagatorFactory base class.
 *
 * @param fact the NavPropagatorFactory object whose data to assign to "this"
 *  factory.
 *
 * @return "this" NavPropagatorFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
NavPropagatorFactory& NavPropagatorFactory::operator= (const NavPropagatorFactory &fact)
{
   if (this != &fact)
   {
      Factory::operator=(fact);
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~NavPropagatorFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the NavPropagatorFactory base class.
   */
//------------------------------------------------------------------------------
NavPropagatorFactory::~NavPropagatorFactory()
{
}
