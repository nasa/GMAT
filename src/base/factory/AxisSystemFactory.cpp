//$Id$
//------------------------------------------------------------------------------
//                            AxisSystemFactory
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
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy Shoan
// Created: 2004/12/23
//
/**
 *  Implementation code for the AxisSystemFactory class, responsible for
 *  creating AxisSystem objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "AxisSystemFactory.hpp"
#include "AxisSystem.hpp"
#include "MessageInterface.hpp"
#include "MJ2000EqAxes.hpp"
#include "MJ2000EcAxes.hpp"
#include "BodyFixedAxes.hpp"
#include "BodyInertialAxes.hpp"
#include "EquatorAxes.hpp"
#include "ObjectReferencedAxes.hpp"
#include "TOEEqAxes.hpp"
#include "MOEEqAxes.hpp"
#include "TODEqAxes.hpp"
#include "MODEqAxes.hpp"
#include "TOEEcAxes.hpp"
#include "MOEEcAxes.hpp"
#include "TODEcAxes.hpp"
#include "MODEcAxes.hpp"
#include "SpiceAxes.hpp"
#include "GeocentricSolarEclipticAxes.hpp"
#include "GeocentricSolarMagneticAxes.hpp"
#include "TopocentricAxes.hpp"
#include "LocalAlignedConstrainedAxes.hpp"
#include "TEMEAxes.hpp"

//#include "ITRFAxes.hpp"
#include "ICRFAxes.hpp"
#include "BodySpinSunAxes.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateObject(const std::string &ofType, const std::string &withName)
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
GmatBase* AxisSystemFactory::CreateObject(const std::string &ofType,
                                          const std::string &withName)
{
   return CreateAxisSystem(ofType, withName);
}

//------------------------------------------------------------------------------
// AxisSystem* CreateAxisSystem(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an AxisSystem object of the requested 
 * type.
 *
 * @param <ofType>   the AxisSystem object to create and return.
 * @param <withName> the name to give the newly-created AxisSystem object.
 *
 */
//------------------------------------------------------------------------------
AxisSystem*
AxisSystemFactory::CreateAxisSystem(const std::string &ofType,
                                    const std::string &withName)
{
   AxisSystem *withAxes = NULL;
   if (ofType == "MJ2000Eq")
   {
      withAxes = new MJ2000EqAxes(withName);
   }
   else if (ofType == "MJ2000Ec")
   {
      withAxes = new MJ2000EcAxes(withName);
   }
   else if (ofType == "TOEEq")
   {
      withAxes = new TOEEqAxes(withName);
   }
   else if (ofType == "TOEEc")
   {
      withAxes = new TOEEcAxes(withName);
   }
   else if (ofType == "MOEEq")
   {
      withAxes = new MOEEqAxes(withName);
   }
   else if (ofType == "MOEEc")
   {
      withAxes = new MOEEcAxes(withName);
   }
   else if (ofType == "TODEq")
   {
      withAxes = new TODEqAxes(withName);
   }
   else if (ofType == "TODEc")
   {
      withAxes = new TODEcAxes(withName);
   }
   else if (ofType == "MODEq")
   {
      withAxes = new MODEqAxes(withName);
   }
   else if (ofType == "MODEc")
   {
      withAxes = new MODEcAxes(withName);
   }
   else if (ofType == "ObjectReferenced")  
   {
      withAxes = new ObjectReferencedAxes(withName);
   }
   else if (ofType == "Equator")
   {
      withAxes = new EquatorAxes(withName);
   }
   else if (ofType == "BodyFixed")
   {
      withAxes = new BodyFixedAxes(withName);
   }
   else if (ofType == "SPICE")
   {
      withAxes = new SpiceAxes(withName);
   }
   else if (ofType == "BodyInertial")
   {
      withAxes = new BodyInertialAxes(withName);
   }
   else if ((ofType == "GSE") || (ofType == "GeocentricSolarEcliptic"))
   {
      withAxes = new GeocentricSolarEclipticAxes(withName);
   }
   else if ((ofType == "GSM") || (ofType == "GeocentricSolarMagnetic"))
   {
      withAxes = new GeocentricSolarMagneticAxes(withName);
   }
   else if (ofType == "Topocentric")
   {
      withAxes = new TopocentricAxes(withName);
   }
   else if (ofType == "LocalAlignedConstrained")
   {
      withAxes = new LocalAlignedConstrainedAxes(withName);
   }
//   else if (ofType == "ITRF")
//   {
//      withAxes = new ITRFAxes(withName);
//   }
   else if (ofType == "ICRF")
   {
      withAxes = new ICRFAxes(withName);
   }
   else if (ofType == "BodySpinSun")
   {
      withAxes = new BodySpinSunAxes(withName);
   }
   else if (ofType == "TEME")
   {
      withAxes = new TEMEAxes(withName);
   }

   return withAxes;
}


//------------------------------------------------------------------------------
//  AxisSystemFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AxisSystemFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
AxisSystemFactory::AxisSystemFactory() :
Factory(Gmat::AXIS_SYSTEM) //loj: 1/19/05 Changed from ATMOSPHERE
{
   if (creatables.empty())
   {
      creatables.push_back("MJ2000Eq");
      creatables.push_back("MJ2000Ec");
      creatables.push_back("TOEEq");
      creatables.push_back("TOEEc");
      creatables.push_back("MOEEq");
      creatables.push_back("MOEEc");
      creatables.push_back("TODEq");
      creatables.push_back("TODEc");
      creatables.push_back("MODEq");
      creatables.push_back("MODEc");
      creatables.push_back("ObjectReferenced");
      creatables.push_back("Equator");
      creatables.push_back("BodyFixed");
      creatables.push_back("BodyInertial");
      creatables.push_back("GSE");
      creatables.push_back("GSM");
      creatables.push_back("Topocentric");
      creatables.push_back("LocalAlignedConstrained");
      creatables.push_back("SPICE");

//	  creatables.push_back("ITRF");
     creatables.push_back("ICRF");
     creatables.push_back("BodySpinSun");
     creatables.push_back("TEME");
   }

   GmatType::RegisterType(Gmat::AXIS_SYSTEM, "AxisSystem");
}

//------------------------------------------------------------------------------
//  AxisSystemFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AxisSystemFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
AxisSystemFactory::AxisSystemFactory(StringArray createList) :
Factory(createList,Gmat::AXIS_SYSTEM)
{
}

//------------------------------------------------------------------------------
//  AxisSystemFactory(const AxisSystemFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AxisSystemFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
AxisSystemFactory::AxisSystemFactory(const AxisSystemFactory& fact) :
Factory(fact)
{
}

//------------------------------------------------------------------------------
//  AxisSystemFactory& operator= (const AxisSystemFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the AxisSystemFactory class.
 *
 * @param <fact> the AxisSystemFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" AxisSystemFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
AxisSystemFactory& AxisSystemFactory::operator= (
                                             const AxisSystemFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~AxisSystemFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the AxisSystemFactory base class.
 *
 */
//------------------------------------------------------------------------------
AxisSystemFactory::~AxisSystemFactory()
{
   // deletes handled by Factory base class
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------





