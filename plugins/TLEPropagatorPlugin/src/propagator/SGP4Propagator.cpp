//------------------------------------------------------------------------------
//                         SGP4Propagtor.cpp
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


#include "SGP4Propagator.hpp"


SGP4Propagator::SGP4Propagator(const std::string &withName) :
   Propagator("SGP4", withName)
{
}

SGP4Propagator::~SGP4Propagator()
{
}

SGP4Propagator::SGP4Propagator(const SGP4Propagator &sgp) :
   Propagator(sgp)
{
}

SGP4Propagator::SGP4Propagator& operator=(const SGP4Propagator &sgp)
{
   if (&sgp != this)
   {

   }

   return *this;
}
