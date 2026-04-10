//$Id$
//------------------------------------------------------------------------------
//                            EKFCommandFactory
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
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2019/07/25
//
/**
 *  This class is the factory class for EKF and smoother commands.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "EKFCommandFactory.hpp"

#include "RunSmoother.hpp"


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
GmatCommand* EKFCommandFactory::CreateCommand(const std::string &ofType,
                                           const std::string &withName)
{
   if (ofType == "RunSmoother")
       return new RunSmoother;
   // add more here .......
   else
   {
      return NULL;   // doesn't match any known type of command
   }

}


//------------------------------------------------------------------------------
//  EKFCommandFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EKFCommandFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
EKFCommandFactory::EKFCommandFactory() :
    Factory(Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("RunSmoother");
   }
}

//------------------------------------------------------------------------------
//  EKFCommandFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EKFCommandFactory.
 *
 * @param <createList> list of creatable command objects
 */
//------------------------------------------------------------------------------
EKFCommandFactory::EKFCommandFactory(StringArray createList) :
    Factory(createList,Gmat::COMMAND)
{
}

//------------------------------------------------------------------------------
//  EKFCommandFactory(const EKFCommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the (base) class EKFCommandFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
EKFCommandFactory::EKFCommandFactory(const EKFCommandFactory& fact) :
    Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("RunSmoother");
   }
}

//------------------------------------------------------------------------------
//  EKFCommandFactory& operator= (const EKFCommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the EKFCommandFactory base class.
 *
 * @param <fact> the EKFCommandFactory object whose data to assign to "this" factory.
 *
 * @return "this" EKFCommandFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
EKFCommandFactory& EKFCommandFactory::operator= (const EKFCommandFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~EKFCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the EKFCommandFactory base class.
 */
//------------------------------------------------------------------------------
EKFCommandFactory::~EKFCommandFactory()
{
   // deletes handled by Factory destructor
}
