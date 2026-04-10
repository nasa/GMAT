//------------------------------------------------------------------------------
//                         SGP4Propagator.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Darrel Conway
// Created: 2019/09/26
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
 *  Declaration code for the SGP4Propagator class.
 */


#ifndef SGP4Propagator_hpp
#define SGP4Propagator_hpp

#include "tleprop_defs.hpp"
#include "Factory.hpp"
#include "Propagator.hpp"


class TLE_PROPAGATOR_API SGP4Propagator : public Propagator
{
public:
   SGP4Propagator(const std::string &withName);
   ~SGP4Propagator();
   SGP4Propagator(const SGP4Propagator &sgp);
   SGP4Propagator& operator=(const SGP4Propagator &sgp);

protected:
   /// The TLE file name
   std::string tleFileName;
   /// The GMAT Spacecraft used for the propagation
   std::string satName;
   /// The spacecraft that we propagate
   Spacecraft *theSpacecraft;
};

#endif
