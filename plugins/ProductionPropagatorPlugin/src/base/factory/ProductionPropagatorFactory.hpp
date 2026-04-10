//------------------------------------------------------------------------------
//                            ProductionPropagatorFactory
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
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: Nov 6, 2013
//
/**
 * Declaration code for the ProductionPropagatorFactory class, which creates
 * propagators that are not part of the core GMAT system.
 */
//------------------------------------------------------------------------------


#ifndef ProductionPropagatorFactory_hpp
#define ProductionPropagatorFactory_hpp


#include "ProductionPropagatorDefs.hpp"
#include "Factory.hpp"


/**
 * ProductionPropagatorFactory is a factory plug-in that creates Propagator objects
 * for use in GMAT's propagation subsystem.
 */
class PRODUCTIONPROPAGATOR_API ProductionPropagatorFactory : public Factory
{
public:
   virtual GmatBase* CreateObject(const std::string &ofType,
                                const std::string &withName /* = "" */);
   virtual Propagator* CreatePropagator(const std::string &ofType,
                                const std::string &withName /* = "" */);

   // default constructor
   ProductionPropagatorFactory();
   // constructor
   ProductionPropagatorFactory(StringArray createList);
   // copy constructor
   ProductionPropagatorFactory(const ProductionPropagatorFactory& fact);
   // assignment operator
   ProductionPropagatorFactory& operator=(const ProductionPropagatorFactory& fact);

   virtual ~ProductionPropagatorFactory();

};

#endif // ProductionPropagatorFactory_hpp
