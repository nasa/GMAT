//$Id$
//------------------------------------------------------------------------------
//                            PhysicalModelFactory
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
// Created: 2003/10/22
//
/**
 *  Implementation code for the PhysicalModelFactory class, responsible for
 *  creating PhysicalModel objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "PhysicalModelFactory.hpp"
#include "PointMassForce.hpp"
#include "SolarRadiationPressure.hpp"
#include "DragForce.hpp" 
#include "GravityField.hpp" 
#include "FiniteThrust.hpp"
#include "RelativisticCorrection.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreatePhysicalModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested PhysicalModel 
 * class.
 *
 * @param <ofType> the PhysicalModel object to create and return.
 * @param <withName> the name to give the newly-created PhysicalModel object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
GmatBase* PhysicalModelFactory::CreateObject(const std::string &ofType,
                                             const std::string &withName)
{
   return CreatePhysicalModel(ofType, withName);
}


//------------------------------------------------------------------------------
//  CreatePhysicalModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested PhysicalModel 
 * class.
 *
 * @param <ofType> the PhysicalModel object to create and return.
 * @param <withName> the name to give the newly-created PhysicalModel object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
PhysicalModel* PhysicalModelFactory::CreatePhysicalModel(
                                     const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "PointMassForce")
       return new PointMassForce(withName);
   else if (ofType == "SolarRadiationPressure")
       return new SolarRadiationPressure(withName);
   else if (ofType == "DragForce")
       return new DragForce(withName);
   else if (ofType == "GravityField")
       return new GravityField(withName, "Earth");
   else if (ofType == "FiniteThrust")
       return new FiniteThrust(withName);
   else if (ofType == "RelativisticCorrection")
       return new RelativisticCorrection(withName, "Earth");  
   return NULL;
}


//------------------------------------------------------------------------------
//  PhysicalModelFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PhysicalModelFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
PhysicalModelFactory::PhysicalModelFactory() 
   :
   Factory(Gmat::PHYSICAL_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("PointMassForce");
      creatables.push_back("GravityField");
      creatables.push_back("SolarRadiationPressure");
      creatables.push_back("DragForce");
      creatables.push_back("FiniteThrust");
      creatables.push_back("RelativisticCorrection");
   }
   GmatType::RegisterType(Gmat::ODE_MODEL, "ODEModel");
   GmatType::RegisterType(Gmat::PHYSICAL_MODEL, "PhysicalModel");
}

//------------------------------------------------------------------------------
//  PhysicalModelFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PhysicalModelFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
PhysicalModelFactory::PhysicalModelFactory(StringArray createList) 
   :
   Factory(createList,Gmat::PHYSICAL_MODEL)
{
}

//------------------------------------------------------------------------------
//  PhysicalModelFactory(const PhysicalModelFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PhysicalModelFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
PhysicalModelFactory::PhysicalModelFactory(const PhysicalModelFactory &fact) 
   :
   Factory(fact)
{
}

//------------------------------------------------------------------------------
//  PhysicalModelFactory& operator= (const PhysicalModelFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the PhysicalModelFactory class.
 *
 * @param <fact> the PhysicalModelFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" PhysicalModelFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
PhysicalModelFactory& PhysicalModelFactory::operator= (
                                             const PhysicalModelFactory &fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~PhysicalModelFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the PhysicalModelFactory base class.
 *
 */
//------------------------------------------------------------------------------
PhysicalModelFactory::~PhysicalModelFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

