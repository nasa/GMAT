//$Id$
//------------------------------------------------------------------------------
//                         HardwareFactory
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
// order 124
//
// Author: Darrel Conway (Based on HardwareFactory)
// Created: 2004/11/10
//
/**
 *  This class is the factory class for hardware.  
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "HardwareFactory.hpp"
#include "FuelTank.hpp"
#include "ChemicalThruster.hpp"
#include "ElectricThruster.hpp"
#include "ChemicalTank.hpp"
#include "ElectricTank.hpp"
#include "NuclearPowerSystem.hpp"
#include "SolarPowerSystem.hpp"
#include "Imager.hpp"


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
GmatBase* HardwareFactory::CreateObject(const std::string &ofType,
                                        const std::string &withName)
{
   return CreateHardware(ofType, withName);
}

//------------------------------------------------------------------------------
//  Hardware* CreateHardware(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates tanks, thrusters, and other hardware attached to the GMAT spacecraft.
 * 
 * Must be implemented by derived classes that create Hardware objects -
 * in that case, it returns a new Hardware object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Hardware.
 *
 * @param <ofType>   specific type of Hardware object to create.
 * @param <withName> name to give to the newly created Hardware object.
 *
 * @return pointer to a new Hardware object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type Hardware.
 */
//------------------------------------------------------------------------------
Hardware* HardwareFactory::CreateHardware(const std::string &ofType,
                                  const std::string &withName)
{
   if ((ofType == "FuelTank") || (ofType == "ChemicalTank"))
      return new ChemicalTank(withName);
   if (ofType == "ElectricTank")
      return new ElectricTank(withName);
   if ((ofType == "Thruster") || (ofType == "ChemicalThruster"))
      return new ChemicalThruster(withName);
   if (ofType == "ElectricThruster")
      return new ElectricThruster(withName);
   if (ofType == "NuclearPowerSystem")
      return new NuclearPowerSystem(withName);
   if (ofType == "SolarPowerSystem")
      return new SolarPowerSystem(withName);
   // Imager can funtction as either a leaf class or an intermediate class, but
   // doesn't have the "leaf only" c'tor, so it needs the full constructor list
   if (ofType == "Imager")
      return new Imager(Gmat::HARDWARE, ofType, withName);

   return NULL;
}



//------------------------------------------------------------------------------
//  HardwareFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class HardwareFactory
 *  (default constructor)
 */
//------------------------------------------------------------------------------
HardwareFactory::HardwareFactory() :
Factory(Gmat::HARDWARE)
{
   if (creatables.empty())
   {
      creatables.push_back("FuelTank");       // deprecated
      creatables.push_back("ChemicalTank");
      creatables.push_back("ElectricTank");
      creatables.push_back("Thruster");        // deprecated
      creatables.push_back("ChemicalThruster");
      creatables.push_back("ElectricThruster");
      creatables.push_back("NuclearPowerSystem");
      creatables.push_back("SolarPowerSystem");
      creatables.push_back("Imager");
   }

   GmatType::RegisterType(Gmat::HARDWARE, "Hardware");
   GmatType::RegisterType(Gmat::FUEL_TANK, "FuelTank");
   GmatType::RegisterType(Gmat::THRUSTER, "Thruster");
   GmatType::RegisterType(Gmat::CHEMICAL_THRUSTER, "ChemicalThruster");
   GmatType::RegisterType(Gmat::ELECTRIC_THRUSTER, "ElectricThruster");
   GmatType::RegisterType(Gmat::CHEMICAL_FUEL_TANK, "ChemicalTank");
   GmatType::RegisterType(Gmat::ELECTRIC_FUEL_TANK, "ElectricTank");
   GmatType::RegisterType(Gmat::POWER_SYSTEM, "PowerSystem");
   GmatType::RegisterType(Gmat::SOLAR_POWER_SYSTEM, "SolarPowerSystem");
   GmatType::RegisterType(Gmat::NUCLEAR_POWER_SYSTEM, "NuclearPowerSystem");

}

//------------------------------------------------------------------------------
//  HardwareFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class HardwareFactory
 *  (constructor)
 *
 * @param <createList> the initial list of creatable objects for this class
 */
//------------------------------------------------------------------------------
HardwareFactory::HardwareFactory(StringArray createList) :
Factory(createList, Gmat::HARDWARE)
{
   if (creatables.empty())
   {
      creatables.push_back("FuelTank");       // deprecated
      creatables.push_back("ChemicalTank");
      creatables.push_back("ElectricTank");
      creatables.push_back("Thruster");        // deprecated
      creatables.push_back("ChemicalThruster");
      creatables.push_back("ElectricThruster");
      creatables.push_back("NuclearPowerSystem");
      creatables.push_back("SolarPowerSystem");
      creatables.push_back("Imager");

   }
}

//------------------------------------------------------------------------------
//  HardwareFactory(const HardwareFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the (base) class HardwareFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
HardwareFactory::HardwareFactory(const HardwareFactory& fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("FuelTank");       // deprecated
      creatables.push_back("ChemicalTank");
      creatables.push_back("ElectricTank");
      creatables.push_back("Thruster");        // deprecated
      creatables.push_back("ChemicalThruster");
      creatables.push_back("ElectricThruster");
      creatables.push_back("NuclearPowerSystem");
      creatables.push_back("SolarPowerSystem");
      creatables.push_back("Imager");

   }
}

//------------------------------------------------------------------------------
//  HardwareFactory& operator= (const HardwareFactory& fact)
//------------------------------------------------------------------------------
/**
   * Assignment operator for the HardwareFactory base class.
   *
   * @param <fact> the HardwareFactory object whose data to assign
   *                 to "this" factory.
   *
   * @return "this" HardwareFactory with data of input factory fact.
   */
//------------------------------------------------------------------------------
HardwareFactory& HardwareFactory::operator= (const HardwareFactory& fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("FuelTank");       // deprecated
      creatables.push_back("ChemicalTank");
      creatables.push_back("ElectricTank");
      creatables.push_back("Thruster");        // deprecated
      creatables.push_back("ChemicalThruster");
      creatables.push_back("ElectricThruster");
      creatables.push_back("NuclearPowerSystem");
      creatables.push_back("SolarPowerSystem");
      creatables.push_back("Imager");

   }
   return *this;
}

//------------------------------------------------------------------------------
// ~HardwareFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the HardwareFactory base class.
   *
   */
//------------------------------------------------------------------------------
HardwareFactory::~HardwareFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
