//$Id$
//------------------------------------------------------------------------------
//                         ProcessNoiseModelFactory
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
// Created: 2018/09/19
//
/**
*  Implementation code for the ProcessNoiseModelFactory class, responsible for
 *  creating ProcessNoiseModel objects.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ProcessNoiseModelFactory.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
* This method creates and returns an object of the requested class
 *
 * @param <ofType> the ProcessNoiseModel object to create and return.
 * @param <withName> the name to give the newly-created ProcessNoiseModel object.
 *
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
GmatBase* ProcessNoiseModelFactory::CreateObject(const std::string &ofType,
                                         const std::string &withName)
{
   return CreateProcessNoiseModel(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateProcessNoiseModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
* This method creates and returns an object of the requested ProcessNoiseModel class
 *
 * @param <ofType> the ProcessNoiseModel object to create and return.
 * @param <withName> the name to give the newly-created ProcessNoiseModel object.
 *
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
ProcessNoiseModel* ProcessNoiseModelFactory::CreateProcessNoiseModel(const std::string &ofType,
                                             const std::string &withName)
{
   return new ProcessNoiseModel(withName);
}


//------------------------------------------------------------------------------
//  ProcessNoiseModelFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProcessNoiseModelFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
ProcessNoiseModelFactory::ProcessNoiseModelFactory() :
   Factory(GmatType::RegisterType("ProcessNoiseModel"))
{
   if (creatables.empty())
   {
      creatables.push_back("ProcessNoiseModel");
   }
}

//------------------------------------------------------------------------------
// ProcessNoiseModelFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProcessNoiseModelFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
ProcessNoiseModelFactory::ProcessNoiseModelFactory(StringArray createList) :
   Factory(createList,GmatType::RegisterType("ProcessNoiseModel"))
{
}

//------------------------------------------------------------------------------
//  ProcessNoiseModelFactory(const ProcessNoiseModelFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProcessNoiseModelFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
ProcessNoiseModelFactory::ProcessNoiseModelFactory(const ProcessNoiseModelFactory &fact) 
   :
   Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("ProcessNoiseModel");
   }
}

//------------------------------------------------------------------------------
//  ProcessNoiseModelFactory& operator= (const ProcessNoiseModelFactory &fact)
//------------------------------------------------------------------------------
/**
* Assignment operator for the ProcessNoiseModelFactory class.
 *
 * @param <fact> the ProcessNoiseModelFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" ProcessNoiseModelFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
ProcessNoiseModelFactory& ProcessNoiseModelFactory::operator= (const ProcessNoiseModelFactory &fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("ProcessNoiseModel");
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~ProcessNoiseModelFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the ProcessNoiseModelFactory base class.
 *
 */
//------------------------------------------------------------------------------
ProcessNoiseModelFactory::~ProcessNoiseModelFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

