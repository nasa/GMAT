//$Id: MatlabWorkspaceFactory.cpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                            MatlabWorkspaceFactory
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
 *  Implementation code for the MatlabWorkspaceFactory class. With this class user can
 *  write data to MATLAB workspace as data is published from the Publisher.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "MatlabWorkspaceFactory.hpp"
#include "MatlabWorkspace.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * @see CreateSubscriber()
 */
//------------------------------------------------------------------------------
GmatBase* MatlabWorkspaceFactory::CreateObject(const std::string &ofType,
                                               const std::string &withName)
{
   return CreateSubscriber(ofType, withName);
}

//------------------------------------------------------------------------------
//  Subscriber* CreateSubscriber(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Subscriber class
 *
 * @param <ofType>   the Subscriber object to create and return.
 * @param <withName> the name to give the newly-created Subscriber object. [""]
 * @param <fileName> the file name if subscriber is ReportFile, ignored otherwise [""]
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Subscriber* MatlabWorkspaceFactory::CreateSubscriber(const std::string &ofType,
                                                     const std::string &withName)
{
   if (ofType == "MatlabWorkspace")
      return new MatlabWorkspace(withName);
   
   return NULL;
}


//------------------------------------------------------------------------------
//  MatlabWorkspaceFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MatlabWorkspaceFactory. 
 * (default constructor)
 */
//------------------------------------------------------------------------------
MatlabWorkspaceFactory::MatlabWorkspaceFactory() :
   Factory (Gmat::SUBSCRIBER)
{
   if (creatables.empty())
   {
      creatables.push_back("MatlabWorkspace");
   }
}

//------------------------------------------------------------------------------
//  MatlabWorkspaceFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MatlabWorkspaceFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
MatlabWorkspaceFactory::MatlabWorkspaceFactory(StringArray createList) :
   Factory(createList, Gmat::SUBSCRIBER)
{
}


//------------------------------------------------------------------------------
//  MatlabWorkspaceFactory(const MatlabWorkspaceFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MatlabWorkspaceFactory.
 * (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
MatlabWorkspaceFactory::MatlabWorkspaceFactory(const MatlabWorkspaceFactory& fact) :
   Factory (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("MatlabWorkspace");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * MatlabWorkspaceFactory operator for the MatlabWorkspaceFactory base class.
 *
 * @param <fact> the MatlabWorkspaceFactory object that is copied.
 *
 * @return "this" MatlabWorkspaceFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
MatlabWorkspaceFactory&
MatlabWorkspaceFactory::operator=(const MatlabWorkspaceFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    

//------------------------------------------------------------------------------
// ~MatlabWorkspaceFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the MatlabWorkspaceFactory base class.
 */
//------------------------------------------------------------------------------
MatlabWorkspaceFactory::~MatlabWorkspaceFactory()
{
}

