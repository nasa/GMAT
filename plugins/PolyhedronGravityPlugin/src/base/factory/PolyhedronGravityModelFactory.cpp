//------------------------------------------------------------------------------
//                            PolyhedronGravityModelFactory
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
// Author: Tuan Dang Nguyen
// Created: 2012/08/17
//
/**
 *  Implementation code for the PolyhedronGravityModelFactory class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "PolyhedronGravityModelFactory.hpp"
#include "PhysicalModel.hpp"
#include "PolyhedronGravityModel.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * @see CreatePhysicalModel()
 */
//------------------------------------------------------------------------------
GmatBase* PolyhedronGravityModelFactory::CreateObject(const std::string &ofType,
                                                      const std::string &withName)
{
   return CreatePhysicalModel(ofType, withName);
}

//------------------------------------------------------------------------------
//  Solver* CreatePhysicalModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested PolyhedronGravityModel class.
 *
 * @param <ofType> type of PolyhedronGravityModel object to create and return.
 * @param <withName> the name for the newly-created PolyhedronGravityModel object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
PhysicalModel* PolyhedronGravityModelFactory::CreatePhysicalModel(const std::string &ofType,
                                              const std::string &withName)
{
   if (ofType == "PolyhedronGravityModel")
      return ((PhysicalModel*)(new PolyhedronGravityModel(withName)));
    
   return NULL;
}


//------------------------------------------------------------------------------
//  PolyhedronGravityModelFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PolyhedronGravityModelFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
PolyhedronGravityModelFactory::PolyhedronGravityModelFactory() :
   Factory (Gmat::PHYSICAL_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("PolyhedronGravityModel");
   }
}


//------------------------------------------------------------------------------
//  PolyhedronGravityModelFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PolyhedronGravityModelFactory.
 *
 * @param <createList> list of creatable PolyhedronGravityModel objects
 *
 */
//------------------------------------------------------------------------------
PolyhedronGravityModelFactory::PolyhedronGravityModelFactory(StringArray createList) :
   Factory(createList, Gmat::PHYSICAL_MODEL)
{
}


//------------------------------------------------------------------------------
//  PolyhedronGravityModelFactory(const PolyhedronGravityModelFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PolyhedronGravityModelFactory.
 * (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
PolyhedronGravityModelFactory::PolyhedronGravityModelFactory(const PolyhedronGravityModelFactory& fact) :
   Factory (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("PolyhedronGravityModel");
   }
}


//------------------------------------------------------------------------------
//  PolyhedronGravityModelFactory& operator= (const PolyhedronGravityModelFactory& fact)
//------------------------------------------------------------------------------
/**
 * PolyhedronGravityModelFactory operator for the PolyhedronGravityModelFactory base class.
 *
 * @param <fact> the PolyhedronGravityModelFactory object that is copied.
 *
 * @return "this" PolyhedronGravityModelFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
PolyhedronGravityModelFactory& PolyhedronGravityModelFactory::operator=(const PolyhedronGravityModelFactory& fact)
{
   if (&fact != this)
   {
      Factory::operator=(fact);

      if (creatables.empty())
      {
         creatables.push_back("PolyhedronGravityModel");
      }
   }

   return *this;
}
    

//------------------------------------------------------------------------------
// ~PolyhedronGravityModelFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the PolyhedronGravityModelFactory base class.
 */
//------------------------------------------------------------------------------
PolyhedronGravityModelFactory::~PolyhedronGravityModelFactory()
{
}

