//$Id: MatlabFunctionFactory.cpp 9513 2011-04-30 21:23:06Z djcinsb $
//------------------------------------------------------------------------------
//                              MatlabFunctionFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2004/9/22
//
/**
 * Implementation code for the MatlabFunctionFactory class, responsible for
 * creating MATLAB Function objects.
 */
//------------------------------------------------------------------------------


#include "MatlabFunctionFactory.hpp"
#include "MatlabFunction.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateFunction(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested MatlabFunction class
 *
 * @param <ofType> the Function object to create and return.
 */
//------------------------------------------------------------------------------
GmatBase* MatlabFunctionFactory::CreateObject(const std::string &ofType,
         const std::string &withName)
{
   return CreateFunction(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateFunction(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested MatlabFunction class
 *
 * @param <ofType> the Function object to create and return.
 */
//------------------------------------------------------------------------------
Function* MatlabFunctionFactory::CreateFunction(const std::string &ofType,
         const std::string &withName)
{
   if (ofType == "MatlabFunction")
      return new MatlabFunction(withName);
   // add more here .......
   else {
      return NULL;   // doesn't match any known type of Function
   }
   
}


//------------------------------------------------------------------------------
//  MatlabFunctionFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MatlabFunctionFactory.
 * (default constructor)
 *
 */
//------------------------------------------------------------------------------
MatlabFunctionFactory::MatlabFunctionFactory() :
    Factory     (Gmat::FUNCTION)
{
   if (creatables.empty())
   {
      creatables.push_back("MatlabFunction");
   }
}

//------------------------------------------------------------------------------
//  MatlabFunctionFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MatlabFunctionFactory.
 *
 * @param <createList> list of creatable Function objects
 *
 */
//------------------------------------------------------------------------------
MatlabFunctionFactory::MatlabFunctionFactory(StringArray createList) :
    Factory     (createList, Gmat::FUNCTION)
{
}

//------------------------------------------------------------------------------
//  MatlabFunctionFactory(const MatlabFunctionFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class MatlabFunctionFactory (called by
   * copy constructors of derived classes).  (copy constructor)
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
MatlabFunctionFactory::MatlabFunctionFactory(const MatlabFunctionFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("MatlabFunction");
   }
}

//------------------------------------------------------------------------------
//  MatlabFunctionFactory& operator= (const MatlabFunctionFactory& fact)
//------------------------------------------------------------------------------
/**
   * Assignment operator for the MatlabFunctionFactory base class.
   *
   * @param <fact> the MatlabFunctionFactory object whose data to assign to "this" factory.
   *
   * @return "this" MatlabFunctionFactory with data of input factory fact.
   */
//------------------------------------------------------------------------------
MatlabFunctionFactory& MatlabFunctionFactory::operator=(const MatlabFunctionFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~MatlabFunctionFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the MatlabFunctionFactory class.
   *
   */
//------------------------------------------------------------------------------
MatlabFunctionFactory::~MatlabFunctionFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

