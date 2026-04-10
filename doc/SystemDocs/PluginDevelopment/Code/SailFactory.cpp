//$Id: SailFactory.cpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            SailFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: 2008/07/03
//
/**
 * Implementation code for the SailFactory class, which creates objects that 
 * extend the SRP model for solar sailing.
 * 
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SailFactory.hpp"
#include "SolarSailForce.hpp"

#include "MessageInterface.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  PhysicalModel* SailFactory::(const std::string &ofType, 
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns a PhysicalModel object. 
 *
 * @param <ofType> type of PhysicalModel object to create and return.
 * @param <withName> the name for the newly-created PhysicalModel object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
PhysicalModel* SailFactory::CreatePhysicalModel(const std::string &ofType,
                                    const std::string &withName)
{
   if (ofType == "SailForce")
      return new SolarSailForce(withName);
    
   return NULL;
}


//------------------------------------------------------------------------------
//  SailFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SailFactory. 
 * (default constructor)
 */
//------------------------------------------------------------------------------
SailFactory::SailFactory() :
   Factory     (Gmat::PHYSICAL_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("SailForce");
   }
}

//------------------------------------------------------------------------------
//  SailFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SailFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
SailFactory::SailFactory(StringArray createList) :
   Factory(createList, Gmat::PHYSICAL_MODEL)
{
}


//------------------------------------------------------------------------------
//  SailFactory(const SailFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SailFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
SailFactory::SailFactory(const SailFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("SailForce");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * SailFactory operator for the SailFactory base class.
 *
 * @param <fact> the SailFactory object that is copied.
 *
 * @return "this" SailFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
SailFactory& SailFactory::operator=(const SailFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    

//------------------------------------------------------------------------------
// ~SailFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the SailFactory base class.
 */
//------------------------------------------------------------------------------
SailFactory::~SailFactory()
{
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

