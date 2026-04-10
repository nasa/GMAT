//$Id$
//------------------------------------------------------------------------------
//                         EstimatedParameterFactory
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
*  Implementation code for the EstimatedParameterFactory class, responsible for
 *  creating EstimatedParameter objects.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "EstimatedParameterFactory.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
* This method creates and returns an object of the requested class
 *
 * @param <ofType> the EstimatedParameter object to create and return.
 * @param <withName> the name to give the newly-created EstimatedParameter object.
 *
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
GmatBase* EstimatedParameterFactory::CreateObject(const std::string &ofType,
                                         const std::string &withName)
{
   return CreateEstimatedParameter(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateEstimatedParameter(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
* This method creates and returns an object of the requested EstimatedParameter class
 *
 * @param <ofType> the EstimatedParameter object to create and return.
 * @param <withName> the name to give the newly-created EstimatedParameter object.
 *
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
EstimatedParameter* EstimatedParameterFactory::CreateEstimatedParameter(const std::string &ofType,
                                             const std::string &withName)
{
   return new EstimatedParameter(withName);
}


//------------------------------------------------------------------------------
//  EstimatedParameterFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EstimatedParameterFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
EstimatedParameterFactory::EstimatedParameterFactory() :
   Factory(GmatType::RegisterType("EstimatedParameter"))
{
   if (creatables.empty())
   {
      creatables.push_back("EstimatedParameter");
   }
}

//------------------------------------------------------------------------------
// EstimatedParameterFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EstimatedParameterFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
EstimatedParameterFactory::EstimatedParameterFactory(StringArray createList) :
   Factory(createList,GmatType::RegisterType("EstimatedParameter"))
{
}

//------------------------------------------------------------------------------
//  EstimatedParameterFactory(const EstimatedParameterFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EstimatedParameterFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
EstimatedParameterFactory::EstimatedParameterFactory(const EstimatedParameterFactory &fact) 
   :
   Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("EstimatedParameter");
   }
}

//------------------------------------------------------------------------------
//  EstimatedParameterFactory& operator= (const EstimatedParameterFactory &fact)
//------------------------------------------------------------------------------
/**
* Assignment operator for the EstimatedParameterFactory class.
 *
 * @param <fact> the EstimatedParameterFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" EstimatedParameterFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
EstimatedParameterFactory& EstimatedParameterFactory::operator= (const EstimatedParameterFactory &fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("EstimatedParameter");
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~EstimatedParameterFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the EstimatedParameterFactory base class.
 *
 */
//------------------------------------------------------------------------------
EstimatedParameterFactory::~EstimatedParameterFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

