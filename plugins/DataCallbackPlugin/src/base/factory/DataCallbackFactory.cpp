//$Id: DataCallbackFactory.cpp 10067 2011-12-14 18:53:35Z djcinsb $
//------------------------------------------------------------------------------
//                            DataCallbackFactory
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
// number S-67573-G
//
// Author: Ravi Mathur, Emergent Space Technologies, Inc.
// Created: December 15, 2014
//
/**
 *  Implementation code for the DataCallbackFactory class, responsible for
 *  creating DataCallback Subscriber objects.
 */
//------------------------------------------------------------------------------
#include "DataCallbackFactory.hpp"
#include "DataCallback.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Returns a GmatBase object pointer to a new object.
 *
 * @param <ofType>   the Subscriber object to create and return.
 * @param <withName> the name to give the newly-created Subscriber object.
 *
 */
//------------------------------------------------------------------------------
GmatBase* DataCallbackFactory::CreateObject(const std::string &ofType,
                                            const std::string &withName)
{
   return CreateSubscriber(ofType, withName);
}


//------------------------------------------------------------------------------
//  CreateSubscriber(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Subscriber class
 *
 * @param <ofType>   the Subscriber object to create and return.
 * @param <withName> the name to give the newly-created Subscriber object.
 *
 */
//------------------------------------------------------------------------------
Subscriber* DataCallbackFactory::CreateSubscriber(const std::string &ofType,
                                                  const std::string &withName)
{
   if (ofType == "DataCallback")
      return new DataCallback(ofType, withName);
   
   return NULL;
}


//------------------------------------------------------------------------------
//  DataCallbackFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class DataCallbackFactory
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
DataCallbackFactory::DataCallbackFactory() :
Factory(Gmat::SUBSCRIBER)
{
   if (creatables.empty())
   {
      creatables.push_back("DataCallback");
   }
}


//------------------------------------------------------------------------------
// DataCallbackFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class DataCallbackFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
DataCallbackFactory::DataCallbackFactory(StringArray createList) :
Factory(createList,Gmat::SUBSCRIBER)
{
}


//------------------------------------------------------------------------------
//  DataCallbackFactory(const DataCallbackFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class DataCallbackFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
DataCallbackFactory::DataCallbackFactory(const DataCallbackFactory& fact) :
Factory(fact)
{
  if (creatables.empty())
  {
    creatables.push_back("DataCallback");
  }
}


//------------------------------------------------------------------------------
// DataCallbackFactory& operator= (const DataCallbackFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the DataCallbackFactory class.
 *
 * @param <fact> the DataCallbackFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" DataCallbackFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
DataCallbackFactory& DataCallbackFactory::operator= (const DataCallbackFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}


//------------------------------------------------------------------------------
// ~DataCallbackFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the DataCallbackFactory base class.
 *
 */
//------------------------------------------------------------------------------
DataCallbackFactory::~DataCallbackFactory()
{
   // deletes handled by Factory destructor
}

