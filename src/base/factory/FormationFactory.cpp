//$Id$
//------------------------------------------------------------------------------
//                            FormationFactory
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
// Author: Linda Jun
// Created: 2016.09.21
//
/**
 *  Implementation code for the FormationFactory class, responsible for 
 *  creating Formation objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "FormationFactory.hpp"
#include "Spacecraft.hpp" 

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Formation class
 * in generic way.
 *
 * @param <ofType> the Formation object to create and return.
 * @param <withName> the name to give the newly-created Formation object.
 *
 */
//------------------------------------------------------------------------------
GmatBase* FormationFactory::CreateObject(const std::string &ofType,
                                         const std::string &withName)
{
   return CreateFormation(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateFormation(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Formation class 
 *
 * @param <ofType> the Formation object to create and return.
 * @param <withName> the name for the newly-created Formation object.
 *
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
SpaceObject* FormationFactory::CreateFormation(const std::string &ofType,
                                               const std::string &withName)
{
   if (ofType == "Formation")
      return new Spacecraft(withName);
   return NULL;   
}


//------------------------------------------------------------------------------
//  FormationFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FormationFactory
 *  (default constructor)
 */
//------------------------------------------------------------------------------
FormationFactory::FormationFactory() 
   : Factory(Gmat::FORMATION)
{
   if (creatables.empty())
   {
      creatables.push_back("Formation");
   }
}

//------------------------------------------------------------------------------
//  FormationFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FormationFactory
 *  (constructor)
 *
 * @param <createList> the initial list of createble objects for this class
 */
//------------------------------------------------------------------------------
FormationFactory::FormationFactory(StringArray createList) 
   : Factory(createList, Gmat::FORMATION)
{
   if (creatables.empty())
   {
      creatables.push_back("Formation");
   }
}

//------------------------------------------------------------------------------
//  FormationFactory(const FormationFactory &fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the (base) class FormationFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
FormationFactory::FormationFactory(const FormationFactory &fact)
  : Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("Formation");
   }
}

//------------------------------------------------------------------------------
//  FormationFactory& operator= (const FormationFactory &fact)
//------------------------------------------------------------------------------
/**
   * Assignment operator for the FormationFactory base class.
   *
   * @param <fact> the FormationFactory object whose data to assign
   *                 to "this" factory.
   *
   * @return "this" FormationFactory with data of input factory fact.
   */
//------------------------------------------------------------------------------
FormationFactory& FormationFactory::operator= (const FormationFactory &fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("Formation");
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~FormationFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the FormationFactory base class.
   *
   */
//------------------------------------------------------------------------------
FormationFactory::~FormationFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
