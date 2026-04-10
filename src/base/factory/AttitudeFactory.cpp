//$Id$
//------------------------------------------------------------------------------
//                            AttitudeFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Wendy Shoan
// Created: 2006.03.16
//
/**
 *  Implementation code for the AttitudeFactory class, responsible for
 *  creating Attitude objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "AttitudeFactory.hpp"
#include "Attitude.hpp"   // for Attitude class
#include "CSFixed.hpp"
#include "Spinner.hpp"
#include "PrecessingSpinner.hpp"
#include "NadirPointing.hpp"
#include "CCSDSAttitude.hpp"
#ifdef __USE_SPICE__
   #include "SpiceAttitude.hpp"
#endif
#include "ThreeAxisKinematic.hpp"
#include "CommandableNadirPointing.hpp"


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
GmatBase* AttitudeFactory::CreateObject(const std::string &ofType,
                                        const std::string &withName)
{
   return CreateAttitude(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateAttitude(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Attitude class 
 *
 * @param <ofType> the Attitude object to create and return.
 * @param <withName> the name to give the newly-created Attitude object.
 *
 */
//------------------------------------------------------------------------------
Attitude* AttitudeFactory::CreateAttitude(const std::string &ofType,
                                          const std::string &withName)
{
   if ((ofType == "CSFixed") || (ofType == "CoordinateSystemFixed"))
      return new CSFixed(withName);
   else if (ofType == "Spinner")
      return new Spinner(withName);
   else if (ofType == "PrecessingSpinner")
      return new PrecessingSpinner(withName);
   else if (ofType == "NadirPointing")
      return new NadirPointing(withName);
   else if (ofType == "CCSDS-AEM")
      return new CCSDSAttitude(withName);
   else if (ofType == "ThreeAxisKinematic")
      return new ThreeAxisKinematic(withName);
   else if (ofType == "CommandableNadirPointing")
      return new CommandableNadirPointing(withName);
   #ifdef __USE_SPICE__
      else if (ofType == "SpiceAttitude")
         return new SpiceAttitude();
   #endif
   else
   {
      return NULL;   // doesn't match any known type of Attitude Model
   }
}


//------------------------------------------------------------------------------
//  AttitudeFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AttitudeFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
AttitudeFactory::AttitudeFactory() :
Factory(Gmat::ATTITUDE)
{
   if (creatables.empty())
   {
      //creatables.push_back("CSFixed");
      creatables.push_back("CoordinateSystemFixed");
      creatables.push_back("Spinner");
      creatables.push_back("PrecessingSpinner");
      creatables.push_back("NadirPointing");
      creatables.push_back("CCSDS-AEM");
      #ifdef __USE_SPICE__
         creatables.push_back("SpiceAttitude");
      #endif

      creatables.push_back("ThreeAxisKinematic");
      creatables.push_back("CommandableNadirPointing");
      
      // others TBD
   }
   GmatType::RegisterType(Gmat::ATTITUDE, "Attitude");
}

//------------------------------------------------------------------------------
//  AttitudeFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AttitudeFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
AttitudeFactory::AttitudeFactory(StringArray createList) :
Factory(createList,Gmat::ATTITUDE)
{
   if (creatables.empty())
   {
      //creatables.push_back("CSFixed");
      creatables.push_back("CoordinateSystemFixed");
      creatables.push_back("Spinner");
      creatables.push_back("PrecessingSpinner");
      creatables.push_back("NadirPointing");
      creatables.push_back("CCSDS-AEM");
      #ifdef __USE_SPICE__
         creatables.push_back("SpiceAttitude");
      #endif
      creatables.push_back("ThreeAxisKinematic");
      // others TBD
   }
}

//------------------------------------------------------------------------------
//  AttitudeFactory(const AttitudeFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class AttitudeFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
AttitudeFactory::AttitudeFactory(const AttitudeFactory& fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      //creatables.push_back("CSFixed");
      creatables.push_back("CoordinateSystemFixed");
      creatables.push_back("Spinner");
      creatables.push_back("PrecessingSpinner");
      creatables.push_back("NadirPointing");
      creatables.push_back("CCSDS-AEM");
      #ifdef __USE_SPICE__
         creatables.push_back("SpiceAttitude");
      #endif
      creatables.push_back("ThreeAxisKinematic");
      // others TBD
   }
}

//------------------------------------------------------------------------------
//  AttitudeFactory& operator= (const AttitudeFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the AttitudeFactory class.
 *
 * @param <fact> the AttitudeFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" AttitudeFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
AttitudeFactory& AttitudeFactory::operator= (const AttitudeFactory& fact)
{
   if (this == &fact) return *this;
   Factory::operator=(fact);
   if (creatables.empty())
   {
      //creatables.push_back("CSFixed");
      creatables.push_back("CoordinateSystemFixed");
      creatables.push_back("Spinner");
      creatables.push_back("PrecessingSpinner");
      creatables.push_back("NadirPointing");
      creatables.push_back("CCSDS-AEM");
      #ifdef __USE_SPICE__
         creatables.push_back("SpiceAttitude");
      #endif
      creatables.push_back("ThreeAxisKinematic");
      // others TBD
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~AttitudeFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the AttitudeFactory base class.
   *
   */
//------------------------------------------------------------------------------
AttitudeFactory::~AttitudeFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
