//
//------------------------------------------------------------------------------
//                                PrinceDormand853
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// *** File Name : PrinceDormand853.hpp
// *** Created   : July 1, 2013
// **************************************************************************
// ***  Developed By  :  Hwayeong Kim, CBNU                               ***
// **************************************************************************

#ifndef PrinceDormand853_H
#define PrinceDormand853_H

#include "ProductionPropagatorDefs.hpp"
#include "gmatdefs.hpp"
#include "RungeKutta.hpp"

/**
 * This class implements a Runge-Kutta integrator using the coefficients derived 
 * by Prince and Dormand.  This particular set of coefficients implements the
 * eighth order integrator with seventh order error control.  
 */
class PRODUCTIONPROPAGATOR_API PrinceDormand853 :
    public RungeKutta
{
public:
//    PrinceDormand853();
    PrinceDormand853(const std::string &nomme = "");
    virtual ~PrinceDormand853();

    PrinceDormand853(const PrinceDormand853&);
    PrinceDormand853 & operator=(const PrinceDormand853&);

    virtual Propagator* Clone() const;
protected:
    void                        SetCoefficients();

};

#endif // PrinceDormand853_hpp
