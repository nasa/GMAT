//$Id: FminconOptimizerFactory.cpp 9461 2011-04-21 22:10:15Z lindajun $
//------------------------------------------------------------------------------
//                            FminconOptimizerFactory
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
 *  Implementation code for the FminconOptimizerFactory class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "FminconOptimizerFactory.hpp"
#include "FminconOptimizer.hpp"

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
GmatBase* FminconOptimizerFactory::CreateObject(const std::string &ofType,
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
Solver* FminconOptimizerFactory::CreateSolver(const std::string &ofType,
                                              const std::string &withName)
{
   if (ofType == "FminconOptimizer")
      return new FminconOptimizer(withName);
    
   return NULL;
}


//------------------------------------------------------------------------------
//  FminconOptimizerFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FminconOptimizerFactory. 
 * (default constructor)
 */
//------------------------------------------------------------------------------
FminconOptimizerFactory::FminconOptimizerFactory() :
   Factory (Gmat::SOLVER)
{
   if (creatables.empty())
   {
      creatables.push_back("FminconOptimizer");
   }
}


//------------------------------------------------------------------------------
//  FminconOptimizerFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FminconOptimizerFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
FminconOptimizerFactory::FminconOptimizerFactory(StringArray createList) :
   Factory(createList, Gmat::SOLVER)
{
}


//------------------------------------------------------------------------------
//  FminconOptimizerFactory(const FminconOptimizerFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FminconOptimizerFactory.
 * (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
FminconOptimizerFactory::FminconOptimizerFactory(const FminconOptimizerFactory& fact) :
   Factory (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("FminconOptimizer");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * FminconOptimizerFactory operator for the FminconOptimizerFactory base class.
 *
 * @param <fact> the FminconOptimizerFactory object that is copied.
 *
 * @return "this" FminconOptimizerFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
FminconOptimizerFactory& FminconOptimizerFactory::operator=(const FminconOptimizerFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    

//------------------------------------------------------------------------------
// ~FminconOptimizerFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the FminconOptimizerFactory base class.
 */
//------------------------------------------------------------------------------
FminconOptimizerFactory::~FminconOptimizerFactory()
{
}

