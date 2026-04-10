//------------------------------------------------------------------------------
//                            NavPropagatorFactory
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
// Author: Darrel J. Conway (Thinking Systems, Inc.)
// Created: 2015/12/15
//
/**
 *  Definition code for the NavPropagatorFactory class, responsible
 *  for creating Propagator objects exclusive to the Nav code.
 */
//------------------------------------------------------------------------------
#ifndef NavPropagatorFactory_hpp
#define NavPropagatorFactory_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"
#include "Propagator.hpp"

class ESTIMATION_API NavPropagatorFactory : public Factory
{
public:
   GmatBase*  CreateObject(const std::string &ofType,
                           const std::string &withName = "");
   Propagator*  CreatePropagator(const std::string &ofType,
                                 const std::string &withName = "");

   // default constructor
   NavPropagatorFactory();
   // constructor
   NavPropagatorFactory(StringArray createList);
   // copy constructor
   NavPropagatorFactory(const NavPropagatorFactory &fact);
   // assignment operator
   NavPropagatorFactory& operator= (const NavPropagatorFactory &fact);

   // destructor
   ~NavPropagatorFactory();
};

#endif // NavPropagatorFactory_hpp

