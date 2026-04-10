//$Id$
//------------------------------------------------------------------------------
//                              GmatFunctionFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, task order 28.
//
// Author: Darrel Conway (Thinking Systems)
// Created: 2012/09/18
//
/**
 * Implementation code for the GmatFunctionFactory class, responsible for
 * creating GmatFunction objects.
 */
//------------------------------------------------------------------------------


#include "GmatFunctionFactory.hpp"
#include "GmatFunction.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Returns a GmatBase object pointer to a new object. 
 *
 * @param <ofType>   specific type of object to create.
 * @param <withName> name to give to the newly created object.
 *
 * @return pointer to a new object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of the requested type.
 */
//------------------------------------------------------------------------------
GmatBase* GmatFunctionFactory::CreateObject(const std::string &ofType,
                                            const std::string &withName)
{
   return CreateFunction(ofType, withName);
}

//------------------------------------------------------------------------------
// Function* CreateFunction(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Function class
 *
 * @param ofType   The Function object to create and return.
 * @param withName The name of the new object
 *
 * @return The new function
 */
//------------------------------------------------------------------------------
Function* GmatFunctionFactory::CreateFunction(const std::string &ofType,
         const std::string &withName)
{
   if (ofType == "GmatFunction")
      return new GmatFunction(withName);
   // add more here .......
   else {
      return NULL;   // doesn't match any known type of Function
   }
   
}


//------------------------------------------------------------------------------
//  GmatFunctionFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class GmatFunctionFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
GmatFunctionFactory::GmatFunctionFactory() :
    Factory     (Gmat::FUNCTION)
{
   if (creatables.empty())
   {
      creatables.push_back("GmatFunction");
   }
}

//------------------------------------------------------------------------------
//  GmatFunctionFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates a GmatFunctionFactory with list of supported objects.
 *
 * @param createList list of creatable Function objects
 */
//------------------------------------------------------------------------------
GmatFunctionFactory::GmatFunctionFactory(StringArray createList) :
    Factory     (createList, Gmat::FUNCTION)
{
}

//------------------------------------------------------------------------------
//  GmatFunctionFactory(const GmatFunctionFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class GmatFunctionFactory (called by
   * copy constructors of derived classes).  (copy constructor)
   *
   * @param fact the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
GmatFunctionFactory::GmatFunctionFactory(const GmatFunctionFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("GmatFunction");
   }
}

//------------------------------------------------------------------------------
//  GmatFunctionFactory& operator= (const GmatFunctionFactory& fact)
//------------------------------------------------------------------------------
/**
   * Assignment operator for the GmatFunctionFactory base class.
   *
   * @param fact The GmatFunctionFactory object whose data to assign to this
   *             factory.
   *
   * @return "this" GmatFunctionFactory, set to match the input factory fact.
   */
//------------------------------------------------------------------------------
GmatFunctionFactory& GmatFunctionFactory::operator=(const GmatFunctionFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~GmatFunctionFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the GmatFunctionFactory class.
 */
//------------------------------------------------------------------------------
GmatFunctionFactory::~GmatFunctionFactory()
{
   // deletes handled by Factory destructor
}
