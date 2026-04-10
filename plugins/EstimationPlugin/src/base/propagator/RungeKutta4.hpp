//$Id$
//------------------------------------------------------------------------------
//                           RungeKutta4
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
// contract, Task Order 26
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Dec 15, 2015
/**
 * A basic Runge-Kutta 4 integrator with no step size control
 */
//------------------------------------------------------------------------------

#ifndef RungeKutta4_hpp
#define RungeKutta4_hpp

#include "estimation_defs.hpp"
#include "RungeKutta.hpp"

class ESTIMATION_API RungeKutta4: public RungeKutta
{
public:
   RungeKutta4(const std::string &name);
   virtual ~RungeKutta4();
   RungeKutta4(const RungeKutta4& rk);
   RungeKutta4& operator=(const RungeKutta4& rk);

   virtual Real EstimateError();
   virtual bool AdaptStep(Real maxerror);
   virtual Propagator*     Clone() const;

protected:
   void SetCoefficients();

};

#endif /* RungeKutta4_hpp */
