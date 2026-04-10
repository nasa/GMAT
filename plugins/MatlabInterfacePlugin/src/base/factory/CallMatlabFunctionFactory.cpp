//$Id: CallMatlabFunctionFactory.cpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                            CallMatlabFunctionFactory
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
// number NNG06CA54C
//
// Author: Linda Jun
// Created: 2010/03/30
//
/**
 *  Implementation code for the CallMatlabFunctionFactory class.
 */
//------------------------------------------------------------------------------

#include "CallMatlabFunctionFactory.hpp"
#include "CallMatlabFunction.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatCommand* CreateCommand(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested GmatCommand class. 
 *
 * @param <ofType> type of GmatCommand object to create and return.
 * @param <withName> the name for the newly-created GmatCommand object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
GmatCommand* CallMatlabFunctionFactory::CreateCommand(const std::string &ofType,
                                                      const std::string &withName)
{
   if (ofType == "CallMatlabFunction")
      return new CallMatlabFunction();
   
   return NULL;
}


//------------------------------------------------------------------------------
//  CallMatlabFunctionFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CallMatlabFunctionFactory. 
 * (default constructor)
 */
//------------------------------------------------------------------------------
CallMatlabFunctionFactory::CallMatlabFunctionFactory() :
   Factory (Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("CallMatlabFunction");
   }
}


//------------------------------------------------------------------------------
//  CallMatlabFunctionFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CallMatlabFunctionFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
CallMatlabFunctionFactory::CallMatlabFunctionFactory(StringArray createList) :
   Factory(createList, Gmat::COMMAND)
{
}


//------------------------------------------------------------------------------
//  CallMatlabFunctionFactory(const CallMatlabFunctionFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CallMatlabFunctionFactory.
 * (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
CallMatlabFunctionFactory::CallMatlabFunctionFactory(const CallMatlabFunctionFactory& fact) :
   Factory (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("CallMatlabFunction");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * CallMatlabFunctionFactory operator for the CallMatlabFunctionFactory base class.
 *
 * @param <fact> the CallMatlabFunctionFactory object that is copied.
 *
 * @return "this" CallMatlabFunctionFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
CallMatlabFunctionFactory& CallMatlabFunctionFactory::operator=(const CallMatlabFunctionFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    

//------------------------------------------------------------------------------
// ~CallMatlabFunctionFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the CallMatlabFunctionFactory base class.
 */
//------------------------------------------------------------------------------
CallMatlabFunctionFactory::~CallMatlabFunctionFactory()
{
}

