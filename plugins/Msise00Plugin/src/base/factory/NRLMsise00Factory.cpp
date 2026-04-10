//$Id: NRLMsise00Factory.cpp 9490 2011-08-24 16:28:40Z tdnguye2 $
//------------------------------------------------------------------------------
//                            NRLMsise00Factory
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
// Author: Tuan Nguyen
// Created: 2011/8/24
//
/**
 *  Implementation code for the NRLMsise00Factory class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "NRLMsise00Factory.hpp"
#include "NRLMsise00Atmosphere.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * @see CreateAtmosphereModel()
 */
//------------------------------------------------------------------------------
GmatBase* NRLMsise00Factory::CreateObject(const std::string &ofType,
                                          const std::string &withName)
{
   return CreateAtmosphereModel(ofType, withName);
}

//------------------------------------------------------------------------------
//  AtmosphereModel* CreateAtmosphereModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested AtmosphereModel class.
 *
 * @param <ofType> type of AtmosphereModel object to create and return.
 * @param <withName> the name for the newly-created AtmosphereModel object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
AtmosphereModel* NRLMsise00Factory::CreateAtmosphereModel(
                const std::string &ofType,
                const std::string &withName)
{
   if (ofType == "NRLMSISE00")
   {
       AtmosphereModel* atm_model = (AtmosphereModel*)(new NRLMsise00Atmosphere(ofType, withName));
       return atm_model;
   }
   else
   {
       // no other NRLMSISE00 was found:
       return NULL;
   }

}


//------------------------------------------------------------------------------
//  NRLMsise00Factory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class NRLMsise00Factory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
NRLMsise00Factory::NRLMsise00Factory() :
   Factory(Gmat::ATMOSPHERE)
{
   if (creatables.empty())
   {
      creatables.push_back("NRLMSISE00");
   }
}


//------------------------------------------------------------------------------
//  NRLMsise00Factory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class NRLMsise00Factory.
 *
 * @param <createList> list of creatable AtmosphereModel objects
 *
 */
//------------------------------------------------------------------------------
NRLMsise00Factory::NRLMsise00Factory(StringArray createList) :
   Factory(createList, Gmat::ATMOSPHERE)
{
}


//------------------------------------------------------------------------------
//  NRLMsise00Factory(const NRLMsise00Factory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class NRLMsise00Factory.
 * (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
NRLMsise00Factory::NRLMsise00Factory(const NRLMsise00Factory& fact) :
   Factory (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("NRLMSISE00");
   }
}


//------------------------------------------------------------------------------
//  NRLMsise00Factory& operator= (const NRLMsise00Factory& fact)
//------------------------------------------------------------------------------
/**
 * NRLMsise00Factory operator for the NRLMsise00Factory base class.
 *
 * @param <fact> the NRLMsise00Factory object that is copied.
 *
 * @return "this" NRLMsise00Factory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
NRLMsise00Factory& NRLMsise00Factory::operator=(const NRLMsise00Factory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    

//------------------------------------------------------------------------------
// ~NRLMsise00Factory()
//------------------------------------------------------------------------------
/**
 * Destructor for the NRLMsise00Factory base class.
 */
//------------------------------------------------------------------------------
NRLMsise00Factory::~NRLMsise00Factory()
{
}


//------------------------------------------------------------------------------
// StringArray GetListOfCreatableObjects(const std::string& qualifier) const
//------------------------------------------------------------------------------
/**
 * Finds the list of models for a given body.
 *
 * @param qualifier The name of the body that has the atmosphere.
 *
 * @return The list of supported atmosphere models.  The current code only has
 *          Earth models, so only "Earth" is supported, and the complete list is
 *          returned for Earth.
 */
//------------------------------------------------------------------------------
StringArray NRLMsise00Factory::GetListOfCreatableObjects(
      const std::string& qualifier)
{
   if (qualifier == "")
      return creatables;

   qualifiedCreatables.clear();

   if (qualifier == "Earth")
   {
      return creatables;
   }

   // Return an empty list if not Earth
   return qualifiedCreatables;
}
