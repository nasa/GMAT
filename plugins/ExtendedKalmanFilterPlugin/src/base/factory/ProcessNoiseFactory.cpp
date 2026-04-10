//$Id$
//------------------------------------------------------------------------------
//                            ProcessNoiseFactory
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. under
// contract number NNG15CR67C
//
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2018/09/19
//
/**
 *  Implementation code for the ProcessNoiseFactory class, responsible
 *  for creating ProcessNoiseModel objects.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ProcessNoiseFactory.hpp"
#include "ProcessNoiseBase.hpp"

#include "LinearProcessNoise.hpp"
#include "SNCProcessNoise.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested ODEModel class
 * in generic way.
 *
 * @param <ofType> the ODEModel object to create and return.
 * @param <withName> the name to give the newly-created ODEModel object.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ProcessNoiseFactory::CreateObject(const std::string &ofType,
                                          const std::string &withName)
{
   return CreateProcessNoise(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateProcessNoise(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested ProcessNoiseBase class 
 *
 * @param <ofType> the ProcessNoiseBase object to create and return.
 * @param <withName> the name to give the newly-created ProcessNoiseBase object.
 */
//------------------------------------------------------------------------------
ProcessNoiseBase* ProcessNoiseFactory::CreateProcessNoise(const std::string &ofType,
                                                const std::string &withName)
{
   if (ofType == "LinearTime")
      return new LinearProcessNoise(withName);
   if (ofType == "StateNoiseCompensation")
      return new SNCProcessNoise(withName);
   // Add others here as needed
   return NULL;
}


//------------------------------------------------------------------------------
//  ProcessNoiseFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProcessNoiseFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
ProcessNoiseFactory::ProcessNoiseFactory() :
   Factory(GmatType::RegisterType("ProcessNoise"))
{
   if (creatables.empty())
   {
      Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
      if (runmode == GmatGlobal::TESTING || runmode == GmatGlobal::TESTING_NO_PLOTS)
      {
         creatables.push_back("LinearTime");
      }

      creatables.push_back("StateNoiseCompensation");
   }
}

//------------------------------------------------------------------------------
//  ProcessNoiseFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProcessNoiseFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects
 *
 */
//------------------------------------------------------------------------------
ProcessNoiseFactory::ProcessNoiseFactory(StringArray createList) :
   Factory(createList, GmatType::RegisterType("ProcessNoise"))
{
}

//------------------------------------------------------------------------------
//  ProcessNoiseFactory(const ProcessNoiseFactory &fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class ProcessNoiseFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
ProcessNoiseFactory::ProcessNoiseFactory(const ProcessNoiseFactory &fact) 
   :
   Factory(fact)
{

}

//------------------------------------------------------------------------------
//  ProcessNoiseFactory& operator= (const ProcessNoiseFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ProcessNoiseFactory base class.
 *
 * @param <fact> the ProcessNoiseFactory object whose data to assign to "this"
 *  factory.
 *
 * @return "this" ProcessNoiseFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
ProcessNoiseFactory& ProcessNoiseFactory::operator= (const ProcessNoiseFactory &fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~ProcessNoiseFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the ProcessNoiseFactory base class.
   *
   */
//------------------------------------------------------------------------------
ProcessNoiseFactory::~ProcessNoiseFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
