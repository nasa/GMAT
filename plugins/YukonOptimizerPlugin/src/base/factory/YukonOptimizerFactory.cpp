//$Id$
//------------------------------------------------------------------------------
//                            YukonOptimizerFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task 08
//
// Author: Joshua Raymond, Thinking Systems
// Created: 2017/10/23
//
/**
 *  Implementation code for the YukonOptimizerFactory class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "YukonOptimizerFactory.hpp"
#include "Yukonad.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * @see CreateSolver()
 */
//------------------------------------------------------------------------------
GmatBase* YukonOptimizerFactory::CreateObject(const std::string &ofType,
                                                const std::string &withName)
{
   return CreateSolver(ofType, withName);
}

//------------------------------------------------------------------------------
//  Solver* CreateSolver(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Solver class. 
 *
 * @param <ofType> type of Solver object to create and return.
 * @param <withName> the name for the newly-created Solver object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Solver* YukonOptimizerFactory::CreateSolver(const std::string &ofType,
                                              const std::string &withName)
{
   if (ofType == "Yukon")
      return new Yukonad(withName);
    
   return NULL;
}


//------------------------------------------------------------------------------
//  YukonOptimizerFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class YukonOptimizerFactory. 
 * (default constructor)
 */
//------------------------------------------------------------------------------
YukonOptimizerFactory::YukonOptimizerFactory() :
   Factory (Gmat::SOLVER)
{
   if (creatables.empty())
   {
      creatables.push_back("Yukon");
   }
}


//------------------------------------------------------------------------------
//  YukonOptimizerFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class YukonOptimizerFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
YukonOptimizerFactory::YukonOptimizerFactory(StringArray createList) :
   Factory(createList, Gmat::SOLVER)
{
}


//------------------------------------------------------------------------------
//  YukonOptimizerFactory(const YukonOptimizerFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class YukonOptimizerFactory.
 * (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
YukonOptimizerFactory::YukonOptimizerFactory(const YukonOptimizerFactory& fact) :
   Factory (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("Yukon");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * YukonOptimizerFactory operator for the YukonOptimizerFactory base class.
 *
 * @param <fact> the YukonOptimizerFactory object that is copied.
 *
 * @return "this" YukonOptimizerFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
YukonOptimizerFactory& YukonOptimizerFactory::operator=(const YukonOptimizerFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    

//------------------------------------------------------------------------------
// ~YukonOptimizerFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the YukonOptimizerFactory base class.
 */
//------------------------------------------------------------------------------
YukonOptimizerFactory::~YukonOptimizerFactory()
{
}

//------------------------------------------------------------------------------
// bool DoesObjectTypeMatchSubtype(const std::string &theType,
//                                 const std::string &theSubtype)
//------------------------------------------------------------------------------
/**
* Checks if a creatable solver type matches a subtype.
*
* @param theType The script identifier for the object type
* @param theSubtype The subtype being checked
*
* @return true if the scripted type and subtype match, false if the type does
*         not match the subtype
*/
//------------------------------------------------------------------------------
bool YukonOptimizerFactory::DoesObjectTypeMatchSubtype(const std::string &theType,
   const std::string &theSubtype)
{
   bool retval = false;

   if (theSubtype == "Optimizer")  // If Yukonad is the only object in the factory
      retval = true;

   return retval;
}