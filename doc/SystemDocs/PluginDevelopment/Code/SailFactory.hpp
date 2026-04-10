//$Id: SailFactory.hpp,v 1.1 2008/07/03 19:15:33 djc Exp $
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
 * Declaration code for the SailFactory class, which creates objects that extend
 * the SRP model for solar sailing.
 * 
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------


#ifndef SailFactory_hpp
#define SailFactory_hpp


#include "Factory.hpp"


/**
 * SailFactory is a factory plug-in that creates SolarSailForce objects for use 
 * in GMAT's force model.
 */
class SailFactory : public Factory
{
public:
   virtual PhysicalModel* CreatePhysicalModel(const std::string &ofType,
                                const std::string &withName /* = "" */);
   
   // default constructor
   SailFactory();
   // constructor
   SailFactory(StringArray createList);
   // copy constructor
   SailFactory(const SailFactory& fact);
   // assignment operator
   SailFactory& operator=(const SailFactory& fact);

   virtual ~SailFactory();
   
};

#endif // SailFactory_hpp
