//------------------------------------------------------------------------------
//                            EstimationCommandFactory
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
// Author: Wendy Shoan
// Created: 2003/08/03
//
/**
 *  Implementation code for the EstimationCommandFactory class, responsible for
 *  creating Command objects associated with the estimators.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "EstimationCommandFactory.hpp"

#include "RunEstimator.hpp"   // for RunEstimator command
#include "RunSimulator.hpp"   // for RunSimulator command
//#include "SetSeed.hpp"        // ekf mod 12/16


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatCommand* CreateCommand(const std::string &ofType,
//                             const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested command class
 *
 * @param <ofType>   type of command object to create and return.
 * @param <withName> name of the command (currently not used).
 *
 * @return command object
 */
//------------------------------------------------------------------------------
GmatCommand* EstimationCommandFactory::CreateCommand(const std::string &ofType,
                                           const std::string &withName)
{
   if (ofType == "RunSimulator")
       return new RunSimulator;
   if (ofType == "RunEstimator")
       return new RunEstimator;
//   if (ofType == "SetSeed")         // ekf mod 12/16
//       return new SetSeed;          // ekf mod 12/16
   // add more here .......
   else
   {
      return NULL;   // doesn't match any known type of command
   }

}


//------------------------------------------------------------------------------
//  EstimationCommandFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EstimationCommandFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
EstimationCommandFactory::EstimationCommandFactory() :
    Factory(Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("RunEstimator");
      creatables.push_back("RunSimulator");
//      creatables.push_back("SetSeed");          // ekf mod 12/16
   }
}

//------------------------------------------------------------------------------
//  EstimationCommandFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EstimationCommandFactory.
 *
 * @param <createList> list of creatable command objects
 */
//------------------------------------------------------------------------------
EstimationCommandFactory::EstimationCommandFactory(StringArray createList) :
    Factory(createList,Gmat::COMMAND)
{
}

//------------------------------------------------------------------------------
//  EstimationCommandFactory(const EstimationCommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the (base) class EstimationCommandFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
EstimationCommandFactory::EstimationCommandFactory(const EstimationCommandFactory& fact) :
    Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("RunEstimator");
      creatables.push_back("RunSimulator");
//      creatables.push_back("SetSeed");        // ekf mod 12/16
   }
}

//------------------------------------------------------------------------------
//  EstimationCommandFactory& operator= (const EstimationCommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the EstimationCommandFactory base class.
 *
 * @param <fact> the EstimationCommandFactory object whose data to assign to "this" factory.
 *
 * @return "this" EstimationCommandFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
EstimationCommandFactory& EstimationCommandFactory::operator= (const EstimationCommandFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~EstimationCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the EstimationCommandFactory base class.
 */
//------------------------------------------------------------------------------
EstimationCommandFactory::~EstimationCommandFactory()
{
   // deletes handled by Factory destructor
}
