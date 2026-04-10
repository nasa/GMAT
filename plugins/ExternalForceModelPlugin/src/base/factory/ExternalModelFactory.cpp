//$Id: ExternalModelFactory.cpp 9461 2012-08-17 16:28:15Z tuannguyen $
//------------------------------------------------------------------------------
//                            ExternalModelFactory
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. 
//
// Author: Alex Campbell
// Created: 07/25/2022
//
/**
 *  Implementation code for the ExternalModelFactory class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ExternalModelFactory.hpp"
#include "PhysicalModel.hpp"
#include "ExternalModel.hpp"

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
GmatBase* ExternalModelFactory::CreateObject(const std::string &ofType,
                                                      const std::string &withName)
{
   return CreatePhysicalModel(ofType, withName);
}

//------------------------------------------------------------------------------
//  Solver* CreatePhysicalModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested ExternalModel class.
 *
 * @param <ofType> type of ExternalModel object to create and return.
 * @param <withName> the name for the newly-created ExternalModel object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
PhysicalModel* ExternalModelFactory::CreatePhysicalModel(const std::string &ofType,
                                              const std::string &withName)
{
   if (ofType == "ExternalModel")
      return ((PhysicalModel*)(new ExternalModel(withName)));
    
   return NULL;
}


//------------------------------------------------------------------------------
//  ExternalModelFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExternalModelFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
ExternalModelFactory::ExternalModelFactory() :
   Factory (Gmat::PHYSICAL_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("ExternalModel");
   }
}


//------------------------------------------------------------------------------
//  ExternalModelFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExternalModelFactory.
 *
 * @param <createList> list of creatable ExternalModel objects
 *
 */
//------------------------------------------------------------------------------
ExternalModelFactory::ExternalModelFactory(StringArray createList) :
   Factory(createList, Gmat::PHYSICAL_MODEL)
{
}


//------------------------------------------------------------------------------
//  ExternalModelFactory(const ExternalModelFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExternalModelFactory.
 * (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
ExternalModelFactory::ExternalModelFactory(const ExternalModelFactory& fact) :
   Factory (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("ExternalModel");
   }
}


//------------------------------------------------------------------------------
//  ExternalModelFactory& operator= (const ExternalModelFactory& fact)
//------------------------------------------------------------------------------
/**
 * ExternalModelFactory operator for the ExternalModelFactory base class.
 *
 * @param <fact> the ExternalModelFactory object that is copied.
 *
 * @return "this" ExternalModelFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
ExternalModelFactory& ExternalModelFactory::operator=(const ExternalModelFactory& fact)
{
   if (&fact != this)
   {
      Factory::operator=(fact);

      if (creatables.empty())
      {
         creatables.push_back("ExternalModel");
      }
   }

   return *this;
}
    

//------------------------------------------------------------------------------
// ~ExternalModelFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the ExternalModelFactory base class.
 */
//------------------------------------------------------------------------------
ExternalModelFactory::~ExternalModelFactory()
{
}

