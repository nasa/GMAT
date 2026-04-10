//$Id$
//------------------------------------------------------------------------------
//                            StopConditionFactory
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
 *  Implementation code for the StopConditionFactory class, responsible for
 *  creating StopCondition objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "StopConditionFactory.hpp"
#include "StopCondition.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested StopCondition class
 *
 * @param <ofType> the StopCondition object to create and return.
 * @param <withName> the name to give the newly-created StopCondition object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
GmatBase* StopConditionFactory::CreateObject(const std::string &ofType,
                                             const std::string &withName)
{
   return CreateStopCondition(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateStopCondition(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested StopCondition class
 *
 * @param <ofType> the StopCondition object to create and return.
 * @param <withName> the name to give the newly-created StopCondition object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
StopCondition* StopConditionFactory::CreateStopCondition(const std::string &ofType,
                                                         const std::string &withName)
{
    if (ofType == "StopCondition")
        return new StopCondition(withName);
    return NULL;
}


//------------------------------------------------------------------------------
//  StopConditionFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class StopConditionFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
StopConditionFactory::StopConditionFactory() :
Factory(Gmat::STOP_CONDITION)
{
   if (creatables.empty())
   {
      creatables.push_back("StopCondition");
   }
   GmatType::RegisterType(Gmat::STOP_CONDITION, "StopCondition");
}

//------------------------------------------------------------------------------
//  StopConditionFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class StopConditionFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
StopConditionFactory::StopConditionFactory(StringArray createList) :
Factory(createList,Gmat::STOP_CONDITION)
{
}

//------------------------------------------------------------------------------
//  StopConditionFactory(const StopConditionFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class StopConditionFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
StopConditionFactory::StopConditionFactory(const StopConditionFactory& fact) :
Factory(fact)
{
}

//------------------------------------------------------------------------------
//  StopConditionFactory& operator= (const StopConditionFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the StopConditionFactory class.
 *
 * @param <fact> the StopConditionFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" StopConditionFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
StopConditionFactory& StopConditionFactory::operator= (
                                             const StopConditionFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~StopConditionFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the StopConditionFactory base class.
 *
 */
//------------------------------------------------------------------------------
StopConditionFactory::~StopConditionFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------





