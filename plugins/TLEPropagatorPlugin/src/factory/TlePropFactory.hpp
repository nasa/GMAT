//------------------------------------------------------------------------------
//                         TlePropFactory.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Darrel Conway
// Created: 2019/09/26
//
// Copyright (c) 2019, Thinking Systems, Inc.
// All rights reserved

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


/**
 *  Declaration code for the TlePropFactory class.
 */


#ifndef TlePropFactoryFactory_hpp
#define TlePropFactoryFactory_hpp

#include "tleprop_defs.hpp"
#include "Factory.hpp"


class TLE_PROPAGATOR_API TlePropFactory : public Factory
{
public:
   virtual GmatBase*    CreateObject(const std::string &ofType,
                                     const std::string &withName = "");
   virtual Propagator*  CreatePropagator(const std::string &ofType,
                                         const std::string &withName = "");
   
   // default constructor
   TlePropFactory();
   // constructor
   TlePropFactory(StringArray createList);
   // copy constructor
   TlePropFactory(const TlePropFactory& fact);
   // assignment operator
   TlePropFactory& operator=(const TlePropFactory& fact);
   
   virtual ~TlePropFactory();
};

#endif // TlePropFactoryFactory_hpp
