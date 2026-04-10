//$Id$
//------------------------------------------------------------------------------
//                             RungeKuttaFehlberg56
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
// *** File Name : rungekuttafehlberg56.cpp
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
//                           : 1/15/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//                                    Changes
//                           : 02/26/2004 - L. Ruley, Missions Applications Branch
//                                            - Updated style using GMAT cpp style guide
//                                            - All Real types to Real types
//                            - All int types to Integer types
// **************************************************************************

#include "RungeKuttaFehlberg56.hpp"


/** \brief Constructor for the Fehlberg's 4(5) Runge-Kutta integrator */
RungeKuttaFehlberg56::RungeKuttaFehlberg56(const std::string &nomme) :
    RungeKutta      (8, 6, "RungeKutta56" /*"RungeKuttaFehlberg56"*/, nomme)
//    RungeKutta      (8, 6)
{
}


/** \brief Destructor for the Fehlberg's 4(5) Runge-Kutta integrator */
RungeKuttaFehlberg56::~RungeKuttaFehlberg56()
{
}


/** \brief The copy constructor */
RungeKuttaFehlberg56::RungeKuttaFehlberg56(const RungeKuttaFehlberg56& rk) :
    RungeKutta      (rk)
{}


/**\brief The assignment operator */
RungeKuttaFehlberg56 & 
            RungeKuttaFehlberg56::operator=(const RungeKuttaFehlberg56& rk)
{
    if (this == &rk)
        return *this;

    RungeKutta::operator=(rk);

    return *this;
}


/** \brief Method used to create a copy of the object */
GmatBase* RungeKuttaFehlberg56::Clone() const
{
    return new RungeKuttaFehlberg56(*this);
}


/** \brief Function to set coefficients for Fehlberg's 5(6) R-K integrator

  This method, called from Initialize, initializes the stages, ai, bij, cj, and 
  ee arrays for the Runge-Kutta 5(6) integrator using Fehlberg's coefficients.

  The coefficients in this class are the coefficients found in <B>Numerical 
  Algorithms with C</B>, 1996.
 */

void RungeKuttaFehlberg56::SetCoefficients()
{
    if ((ai == NULL) || (bij == NULL) || (cj == NULL) || (ee == NULL)) {
        isInitialized = false;
        return;
    }

    ai[0] = 0.0;    
    ai[1] = 1.0 / 6.0;    
    ai[2] = 4.0 / 15.0;    
    ai[3] = 2.0 / 3.0;    
    ai[4] = 4.0 / 5.0;    
    ai[5] = 1.0;    
    ai[6] = 0.0;    
    ai[7] = 1.0;    

    bij[0][0] =  0.0;

    bij[1][0] =  1.0 / 6.0;
    bij[1][1] =  0.0;

    bij[2][0] =  4.0 / 75.0;
    bij[2][1] =  16.0 / 75.0;
    bij[2][2] =  0.0;

    bij[3][0] =  5.0 / 6.0;
    bij[3][1] = -8.0 / 3.0;
    bij[3][2] =  5.0 / 2.0;
    bij[3][3] =  0.0;

    bij[4][0] = -8.0 / 5.0;
    bij[4][1] =  144.0 /  25.0;
    bij[4][2] = -4.0;
    bij[4][3] =  16.0 / 25.0;
    bij[4][4] =  0.0;

    bij[5][0] =  361.0 / 320.0;
    bij[5][1] = -18.0 / 5.0;
    bij[5][2] =  407.0 / 128.0;
    bij[5][3] = -11.0 / 80.0;
    bij[5][4] =  55.0 / 128.0;
    bij[5][5] =  0.0;

    bij[6][0] = -11.0 / 640.0;
    bij[6][1] =  0.0;
    bij[6][2] =  11.0 / 256.0;
    bij[6][3] = -11.0 / 160.0;
    bij[6][4] =  11.0 / 256.0;
    bij[6][5] =  0.0;
    bij[6][6] =  0.0;

    bij[7][0] =  93.0 / 640.0;
    bij[7][1] = -18.0 / 5.0;
    bij[7][2] =  803.0 / 256.0;
    bij[7][3] = -11.0 / 160.0;
    bij[7][4] =  99.0 / 256.0;
    bij[7][5] =  0.0;
    bij[7][6] =  1.0;
    bij[7][7] =  0.0;

    cj[0] =  7.0 / 1408.0;
    cj[1] =  0.0;
    cj[2] =  1125.0 / 2816.0;
    cj[3] =  9.0 /  32.0;
    cj[4] =  125.0 / 768.0;
    cj[5] =  0.0;
    cj[6] =  5.0 / 66.0;
    cj[7] =  5.0 / 66.0;

    ee[0] =  7.0 / 1408.0 - 31.0 / 384.0;
    ee[1] =  0.0;
    ee[2] =  0.0;
    ee[3] =  0.0;
    ee[4] =  0.0;
    ee[5] = -5.0 / 66.0;
    ee[6] =  5.0 / 66.0;
    ee[7] =  5.0 / 66.0;
}
