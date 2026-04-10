//$Id$
//------------------------------------------------------------------------------
//                            PlateFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Author: Tuan D. Nguyen
// Created: 2019/05/29
//
/**
 *  Implementation code for the PlateFactory class, responsible for 
 *  creating Plate objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "PlateFactory.hpp"
#include "Plate.hpp" 

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Plate class
 * in generic way.
 *
 * @param <ofType> the Plate object to create and return.
 * @param <withName> the name to give the newly-created Plate object.
 *
 */
//------------------------------------------------------------------------------
GmatBase* PlateFactory::CreateObject(const std::string &ofType,
                                             const std::string &withName)
{
   return CreatePlate(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreatePlate(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Plate class 
 *
 * @param <ofType> the Plate object to create and return.
 * @param <withName> the name for the newly-created Plate object.
 *
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
Plate* PlateFactory::CreatePlate(const std::string &ofType,
                                 const std::string &withName)
{
   if (ofType == "Plate")
      return new Plate(Gmat::PLATE, ofType, withName);
   return NULL;   
}


//------------------------------------------------------------------------------
//  PlateFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PlateFactory
 *  (default constructor)
 */
//------------------------------------------------------------------------------
PlateFactory::PlateFactory() :
   Factory(Gmat::PLATE)
{
   if (creatables.empty())
   {
      // Set NPlate to be able to run in Normal mode
      //Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
      //if (runmode == GmatGlobal::TESTING || runmode == GmatGlobal::TESTING_NO_PLOTS)
      //{
         creatables.push_back("Plate");
      //}
   }

   // Register supported types
   GmatType::RegisterType(Gmat::PLATE, "Plate");
}

//------------------------------------------------------------------------------
//  PlateFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PlateFactory
 *  (constructor)
 *
 * @param <createList> the initial list of createble objects for this class
 */
//------------------------------------------------------------------------------
PlateFactory::PlateFactory(StringArray createList) 
   :
   Factory(createList, Gmat::PLATE)
{
   if (creatables.empty())
   {
      creatables.push_back("Plate");
   }
}

//------------------------------------------------------------------------------
//  PlateFactory(const PlateFactory &fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the (base) class PlateFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
PlateFactory::PlateFactory(const PlateFactory &fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("Plate");
   }
}

//------------------------------------------------------------------------------
//  PlateFactory& operator= (const PlateFactory &fact)
//------------------------------------------------------------------------------
/**
   * Assignment operator for the PlateFactory base class.
   *
   * @param <fact> the PlateFactory object whose data to assign
   *                 to "this" factory.
   *
   * @return "this" PlateFactory with data of input factory fact.
   */
//------------------------------------------------------------------------------
PlateFactory& PlateFactory::operator= (const PlateFactory &fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("Plate");
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~PlateFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the PlateFactory base class.
   *
   */
//------------------------------------------------------------------------------
PlateFactory::~PlateFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
