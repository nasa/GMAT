//$Id$
//------------------------------------------------------------------------------
//                            PropSetupFactory
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
*  Implementation code for the PropSetupFactory class, responsible for
 *  creating PropSetup objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "PropSetupFactory.hpp"
#include "PropSetup.hpp"   // for PropSetup class


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreatePropSetup(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
* This method creates and returns an object of the requested PropSetup class
 *
 * @param <ofType> the PropSetup object to create and return.
 * @param <withName> the name to give the newly-created PropSetup object.
 *
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
GmatBase* PropSetupFactory::CreateObject(const std::string &ofType,
                                         const std::string &withName)
{
   return CreatePropSetup(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreatePropSetup(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
* This method creates and returns an object of the requested PropSetup class
 *
 * @param <ofType> the PropSetup object to create and return.
 * @param <withName> the name to give the newly-created PropSetup object.
 *
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
PropSetup* PropSetupFactory::CreatePropSetup(const std::string &ofType,
                                             const std::string &withName)
{
   return new PropSetup(withName);
}


//------------------------------------------------------------------------------
//  PropSetupFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PropSetupFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
PropSetupFactory::PropSetupFactory() 
   :
   Factory(Gmat::PROP_SETUP)
{
   if (creatables.empty())
   {
      std::string propStr = "PropSetup"; 
      creatables.push_back(propStr);  // default type for this factory
   }

   GmatType::RegisterType(Gmat::PROP_SETUP, "PropSetup");
}

//------------------------------------------------------------------------------
// PropSetupFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PropSetupFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
PropSetupFactory::PropSetupFactory(StringArray createList) 
   :
   Factory(createList,Gmat::PROP_SETUP)
{
   if (creatables.empty())
   {
      std::string propStr = "PropSetup";
      creatables.push_back(propStr);  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  PropSetupFactory(const PropSetupFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PropSetupFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
PropSetupFactory::PropSetupFactory(const PropSetupFactory &fact) 
   :
   Factory(fact)
{
   if (creatables.empty())
   {
      std::string propStr = "PropSetup";
      creatables.push_back(propStr);  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  PropSetupModelFactory& operator= (const PropSetupFactory &fact)
//------------------------------------------------------------------------------
/**
* Assignment operator for the PropSetupFactory class.
 *
 * @param <fact> the PropSetupFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" PropSetupFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
PropSetupFactory& PropSetupFactory::operator= (const PropSetupFactory &fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      std::string propStr = "PropSetup";
      creatables.push_back(propStr);  // default type for this factory
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~PropSetupFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the PropSetupFactory base class.
 *
 */
//------------------------------------------------------------------------------
PropSetupFactory::~PropSetupFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

