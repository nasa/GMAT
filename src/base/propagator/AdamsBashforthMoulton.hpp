//$Id$
//------------------------------------------------------------------------------
//                        AdamsBashforthMoulton
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// *** File Name : AdamsBashforthMoulton.hpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 6/17/2004 - A. Greene, Mission Applications, GSFC
//                             - Updated style using GMAT cpp style guide
//                             - All double types to Real types, int to Integer
// **************************************************************************


#ifndef AdamsBashforthMoulton_H
#define AdamsBashforthMoulton_H

#include "gmatdefs.hpp"
#include "PredictorCorrector.hpp"
#include "RungeKutta.hpp"

/**
 * Implementation of the Adams-Bashford-Moulton Predictor-Corrector
 *
 * This code implements a fourth-order Adams-Bashford predictor / Adams-Moulton
 * corrector pair to integrate a set of first order differential equations.  The
 * algorithm is found at 
 * http://chemical.caeds.eng.uml.edu/onlinec/white/math/s1/s1num/s1num.html
 * or in Bate, Mueller and White, pp. 415-417.
 *
 * The predictor step extrapolates the next state \f$ r_{i+1} \f$ of the variables
 * using the the derivative information \f$ (f) \f$ at the current state and three 
 * previous states of the variables, by applying the equation
 *
 * \f[ r_{i+1}^{*j} = r_i^j + {{h}\over{24}}\left[55 f_n^j - 59 f_{n-1}^j 
 *                 + 37 f_{n-2}^j - 9 f_{n-3}^j \right] \f]
 *
 * The corrector uses derivative information evaluated for this state, along 
 * with the derivative information at the original state and two preceding 
 * states, to tune this state, giving the final, corrected state:
 *
 * \f[ r_{i+1}^{j} = r_i^j + {{h}\over{24}}\left[9 f_{n+1}^{*j} + 19 f_{n}^j 
 *                 - 5 f_{n-1}^j + 1 f_{n-2}^j \right] \f]
 *
 * Bate, Mueller and White give the estimated accuracy of this solution to be
 *
 * \f[ ee = {{19}\over{270}} \left|r_{i+1}^{*j} - r_{i+1}^{j}\right| \f]
 */
 
class GMAT_API AdamsBashforthMoulton :
    public PredictorCorrector
{
public:
//    AdamsBashforthMoulton();
    AdamsBashforthMoulton(const std::string &nomme);

    virtual ~AdamsBashforthMoulton();

    AdamsBashforthMoulton(const AdamsBashforthMoulton& abm);
    AdamsBashforthMoulton    operator=(const AdamsBashforthMoulton& abm);

    virtual GmatBase*       Clone() const;

protected:
    /// The error estimate prefactor
    Real                    eeFactor;

    bool                    SetWeights();
    bool                    FireStartupStep();
    bool                    Predict();
    bool                    Correct();
    Real                    EstimateError();
    bool                    Reset();
};

#endif  //AdamsBashformMoulton_hpp
