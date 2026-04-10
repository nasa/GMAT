//$Id$
//------------------------------------------------------------------------------
//                         FieldOfViewFactory
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
//
// Author: Syeda Kazmi (Based on FieldOfViewFactory)
// Created: 5/20/2019
//
/**
 *  This class is the factory class for FOV.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "FieldOfViewFactory.hpp"
#include "ConicalFOV.hpp"
#include "CustomFOV.hpp"
#include "RectangularFOV.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Returns a GmatBase object pointer to a new object.
 *
 * @param <ofType>   specific type of object to create.
 * @param <withName> name to give to the newly created object.
 *
 * @return pointer to a new object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of the requested type.
 */
//------------------------------------------------------------------------------
GmatBase* FieldOfViewFactory::CreateObject(const std::string &ofType,
                                        const std::string &withName)
{
   return CreateFieldOfView(ofType, withName);
}

//------------------------------------------------------------------------------
//  FieldOfView* CreateFieldOfView(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 *
 * Must be implemented by derived classes that create Field of view objects -
 * in that case, it returns a new  Field of view  object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  Field of view .
 *
 * @param <ofType>   specific type of  Field of view object to create.
 * @param <withName> name to give to the newly created  Field of view object.
 *
 * @return pointer to a new  Field of view  object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type  Field of view.
 */
//------------------------------------------------------------------------------
FieldOfView* FieldOfViewFactory::CreateFieldOfView(const std::string &ofType,
                                                   const std::string &withName)
{
   if (ofType == "ConicalFOV")
       return new ConicalFOV(withName);
   if (ofType == "RectangularFOV")
	   return new RectangularFOV(withName);
   if (ofType == "CustomFOV")
  	   return new CustomFOV(withName);
   return NULL;
}



//------------------------------------------------------------------------------
//  FieldOfViewFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FieldOfViewFactory
 *  (default constructor)
 */
//------------------------------------------------------------------------------
FieldOfViewFactory::FieldOfViewFactory() :
Factory(Gmat::FIELD_OF_VIEW)
{
   if (creatables.empty())
   {
      creatables.push_back("ConicalFOV");
      creatables.push_back("RectangularFOV");
      creatables.push_back("CustomFOV");
   }

   GmatType::RegisterType(Gmat::FIELD_OF_VIEW, "FieldOfView");
   GmatType::RegisterType(Gmat::CONICAL_FOV, "ConicalFOV");
   GmatType::RegisterType(Gmat::RECTANGULAR_FOV, "RectangularFOV");
   GmatType::RegisterType(Gmat::CUSTOM_FOV, "CustomFOV");

}

//------------------------------------------------------------------------------
//  FieldOfViewFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FieldOfViewFactory
 *  (constructor)
 *
 * @param <createList> the initial list of createble objects for this class
 */
//------------------------------------------------------------------------------
FieldOfViewFactory::FieldOfViewFactory(StringArray createList) :
Factory(createList, Gmat::FIELD_OF_VIEW)
{
   if (creatables.empty())
   {
      creatables.push_back("ConicalFOV");
      creatables.push_back("RectangularFOV");
      creatables.push_back("CustomFOV");
   }
}

//------------------------------------------------------------------------------
//  FieldOfViewFactory(const FieldOfViewFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the (base) class FieldOfViewFactory
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
FieldOfViewFactory::FieldOfViewFactory(const FieldOfViewFactory& fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("ConicalFOV");
      creatables.push_back("RectangularFOV");
      creatables.push_back("CustomFOV");
   }
}

//------------------------------------------------------------------------------
//  FieldOfViewFactory& operator= (const FieldOfViewFactory& fact)
//------------------------------------------------------------------------------
/**
   * Assignment operator for the FieldOfViewFactory base class.
   *
   * @param <fact> the FieldOfViewFactory object whose data to assign
   *                 to "this" factory.
   *
   * @return "this" FieldOfViewFactory with data of input factory fact.
   */
//------------------------------------------------------------------------------
FieldOfViewFactory& FieldOfViewFactory::operator= (const FieldOfViewFactory& fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("ConicalFOV");
      creatables.push_back("RectangularFOV");
      creatables.push_back("CustomFOV");
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~FieldOfViewFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the FieldOfViewFactory base class.
   *
   */
//------------------------------------------------------------------------------
FieldOfViewFactory::~FieldOfViewFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
