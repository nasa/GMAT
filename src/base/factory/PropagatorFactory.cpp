//$Id$
//------------------------------------------------------------------------------
//                            PropagatorFactory
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
// Created: 2003/10/14
//
/**
 *  Implementation code for the PropagatorFactory class, responsible
 *  for creating Propagator objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "PropagatorFactory.hpp"
#include "RungeKutta89.hpp"
#include "DormandElMikkawyPrince68.hpp" 
#include "RungeKuttaFehlberg56.hpp" 
#include "PrinceDormand45.hpp" 
#include "PrinceDormand78.hpp" 
#include "AdamsBashforthMoulton.hpp"

// Ephemeris propagators
//#ifdef __USE_SPICE__
//#include "SPKPropagator.hpp"
//#endif

// Not yet implemented:
//#include "Cowell.hpp"
/// @todo add others here for future builds

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
GmatBase* PropagatorFactory::CreateObject(const std::string &ofType,
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
Propagator* PropagatorFactory::CreatePropagator(const std::string &ofType,
                                                const std::string &withName)
{
   if (ofType == "RungeKutta89")
      return new RungeKutta89(withName);
   if (ofType == "PrinceDormand78")
      return new PrinceDormand78(withName);
   if (ofType == "PrinceDormand45")
      return new PrinceDormand45(withName);
//   if (ofType == "DormandElMikkawyPrince68")
//      return new DormandElMikkawyPrince68(withName);
   if (ofType == "RungeKutta68")
      return new DormandElMikkawyPrince68(withName);
//   if (ofType == "RungeKuttaFehlberg56")
//      return new RungeKuttaFehlberg56(withName);
   if (ofType == "RungeKutta56")
      return new RungeKuttaFehlberg56(withName);
   if (ofType == "AdamsBashforthMoulton")
      return new AdamsBashforthMoulton(withName);
//   if (ofType == "Cowell")
//      return new Cowell(withName);
   // Add others here as needed
   return NULL;
}


//------------------------------------------------------------------------------
//  PropagatorFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PropagatorFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
PropagatorFactory::PropagatorFactory() 
   :
   Factory(Gmat::PROPAGATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("RungeKutta89");
      creatables.push_back("PrinceDormand78");
      creatables.push_back("PrinceDormand45");
//      creatables.push_back("DormandElMikkawyPrince68");
      creatables.push_back("RungeKutta68");
//      creatables.push_back("RungeKuttaFehlberg56");
      creatables.push_back("RungeKutta56");
      creatables.push_back("AdamsBashforthMoulton");
//      creatables.push_back("Cowell");
   }

   GmatType::RegisterType(Gmat::PROPAGATOR, "Propagator");
   // No factory for this one: putting it here for now
   GmatType::RegisterType(Gmat::INTERPOLATOR, "Interpolator");
}

//------------------------------------------------------------------------------
//  PropagatorFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PropagatorFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects
 *
 */
//------------------------------------------------------------------------------
PropagatorFactory::PropagatorFactory(StringArray createList) :
   Factory(createList, Gmat::PROPAGATOR)
{
}

//------------------------------------------------------------------------------
//  PropagatorFactory(const PropagatorFactory &fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class PropagatorFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
PropagatorFactory::PropagatorFactory(const PropagatorFactory &fact) 
   :
   Factory(fact)
{

}

//------------------------------------------------------------------------------
//  PropagatorFactory& operator= (const PropagatorFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the PropagatorFactory base class.
 *
 * @param <fact> the PropagatorFactory object whose data to assign to "this"
 *  factory.
 *
 * @return "this" PropagatorFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
PropagatorFactory& PropagatorFactory::operator= (const PropagatorFactory &fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~PropagatorFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the PropagatorFactory base class.
   *
   */
//------------------------------------------------------------------------------
PropagatorFactory::~PropagatorFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
