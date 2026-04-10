//------------------------------------------------------------------------------
//                            PolyhedronGravityModelFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Tuan Dang Nguyen
// Created: 2012/08/17
//
/**
 *  Declaration code for the PolyhedronGravityModelFactory class.
 */
//------------------------------------------------------------------------------
#ifndef PolyhedronGravityModelFactory_hpp
#define PolyhedronGravityModelFactory_hpp


#include "polyhedrongravitymodel_defs.hpp"
#include "Factory.hpp"
#include "PhysicalModel.hpp"
#include "PolyhedronGravityModel.hpp"

class POLYHEDRONGRAVITYMODEL_API PolyhedronGravityModelFactory : public Factory
{
public:
   virtual GmatBase* CreateObject(const std::string &ofType,
                                const std::string &withName);
   virtual PhysicalModel* CreatePhysicalModel(const std::string &ofType,
                                const std::string &withName);
   
   // default constructor
   PolyhedronGravityModelFactory();
   // constructor
   PolyhedronGravityModelFactory(StringArray createList);
   // copy constructor
   PolyhedronGravityModelFactory(const PolyhedronGravityModelFactory& fact);
   // assignment operator
   PolyhedronGravityModelFactory& operator=(const PolyhedronGravityModelFactory& fact);
   
   virtual ~PolyhedronGravityModelFactory();
   
};

#endif // PolyhedronGravityModelFactory_hpp
