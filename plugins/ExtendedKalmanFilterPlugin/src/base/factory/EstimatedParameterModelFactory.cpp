//$Id$
//------------------------------------------------------------------------------
//                       EstimatedParameterModelFactory
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. under
// contract number NNG15CR67C
//
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2020/02/04
//
/**
 *  Implementation code for the EstimatedParameterModelFactory class, responsible
 *  for creating objects derived from EstimatedParameterModel.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "EstimatedParameterModelFactory.hpp"
#include "EstimatedParameterModel.hpp"

#include "FirstOrderGaussMarkov.hpp"

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
GmatBase* EstimatedParameterModelFactory::CreateObject(const std::string &ofType,
                                          const std::string &withName)
{
   return CreateEstimatedParameterModel(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateEstimatedParameterModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested EstimatedParameterModel class 
 *
 * @param <ofType> the EstimatedParameterModel object to create and return.
 * @param <withName> the name to give the newly-created EstimatedParameterModel object.
 */
//------------------------------------------------------------------------------
EstimatedParameterModel* EstimatedParameterModelFactory::CreateEstimatedParameterModel(const std::string &ofType,
                                                const std::string &withName)
{
   if (ofType == "FirstOrderGaussMarkov")
      return new FirstOrderGaussMarkov(withName);
   // Add others here as needed
   return NULL;
}


//------------------------------------------------------------------------------
//  EstimatedParameterModelFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EstimatedParameterModelFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
EstimatedParameterModelFactory::EstimatedParameterModelFactory() :
   Factory(GmatType::RegisterType("EstimatedParameterModel"))
{
   if (creatables.empty())
   {
      creatables.push_back("FirstOrderGaussMarkov");
   }
}

//------------------------------------------------------------------------------
//  EstimatedParameterModelFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EstimatedParameterModelFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects
 *
 */
//------------------------------------------------------------------------------
EstimatedParameterModelFactory::EstimatedParameterModelFactory(StringArray createList) :
   Factory(createList, GmatType::RegisterType("EstimatedParameterModel"))
{
}

//------------------------------------------------------------------------------
//  EstimatedParameterModelFactory(const EstimatedParameterModelFactory &fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class EstimatedParameterModelFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
EstimatedParameterModelFactory::EstimatedParameterModelFactory(const EstimatedParameterModelFactory &fact) 
   :
   Factory(fact)
{

}

//------------------------------------------------------------------------------
//  EstimatedParameterModelFactory& operator= (const EstimatedParameterModelFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the EstimatedParameterModelFactory base class.
 *
 * @param <fact> the EstimatedParameterModelFactory object whose data to assign to "this"
 *  factory.
 *
 * @return "this" EstimatedParameterModelFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
EstimatedParameterModelFactory& EstimatedParameterModelFactory::operator= (const EstimatedParameterModelFactory &fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~EstimatedParameterModelFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the EstimatedParameterModelFactory base class.
   *
   */
//------------------------------------------------------------------------------
EstimatedParameterModelFactory::~EstimatedParameterModelFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
