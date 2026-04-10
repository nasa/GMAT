//$Id$
//------------------------------------------------------------------------------
//                           OrbitTime
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Claire R Conway, Thinking Systems, Inc.
// Created: Jan 18, 2022
/**
 * 
 */
//------------------------------------------------------------------------------


#ifndef OrbitTime_hpp
#define OrbitTime_hpp

#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API OrbitTime : public OrbitReal
{
public:
   OrbitTime(const std::string &name = "", GmatBase *obj = NULL);
   OrbitTime(const OrbitTime &copy);
   virtual ~OrbitTime();
   OrbitTime& operator=(const OrbitTime &copy);

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:
   CelestialBody *theSun;

};


#endif /* OrbitTime_hpp */
