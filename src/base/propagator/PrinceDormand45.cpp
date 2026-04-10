//$Id$
//------------------------------------------------------------------------------
//                        PrinceDormand45
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
// *** File Name : PrinceDormand45.hpp
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
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//            - All double types to Real types
//
//                           : 7/14/2004 - A. Greene, Missions Applications Branch
// **************************************************************************

#include "PrinceDormand45.hpp"

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// PrinceDormand45::PrinceDormand45(const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * The constructor
 */
//------------------------------------------------------------------------------
//PrinceDormand45::PrinceDormand45(void) :
//    RungeKutta      (7, 5)
//{
//}
PrinceDormand45::PrinceDormand45(const std::string &nomme) :
    RungeKutta      (7, 5, "PrinceDormand45", nomme)
{
}


//------------------------------------------------------------------------------
// PrinceDormand45::~PrinceDormand45()
//------------------------------------------------------------------------------
/**
 * The destructor
 */
//------------------------------------------------------------------------------
PrinceDormand45::~PrinceDormand45()
{
}


//------------------------------------------------------------------------------
// PrinceDormand45::PrinceDormand45(const PrinceDormand45& rk)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
PrinceDormand45::PrinceDormand45(const PrinceDormand45& rk) :
    RungeKutta      (rk)
{
}

//------------------------------------------------------------------------------
// PrinceDormand45 & PrinceDormand45::operator=(const PrinceDormand45& rk)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
PrinceDormand45 & PrinceDormand45::operator=(const PrinceDormand45& rk)
{
    if (this == &rk)
        return *this;

    RungeKutta::operator=(rk);

    return *this;
}


//------------------------------------------------------------------------------
// GmatBase* PrinceDormand45::Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* PrinceDormand45::Clone() const
{
    return new PrinceDormand45(*this);
}

//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// void PrinceDormand78::SetCoefficients(void)
//------------------------------------------------------------------------------
/**
 * This function sets the coefficients for the Prince-Dormand 4(5) integrator. 
 * This method, called from Initialize, initializes the stages, ai, bij, cj, and 
 * ee arrays for the Prince-Dormand 4(5) integrator.
 *
 * The coefficients in this class are the coefficients found in Prince and 
 * Dormand, 1981.
 */
//------------------------------------------------------------------------------
void PrinceDormand45::SetCoefficients()
{
    if ((ai == NULL) || (bij == NULL) || (cj == NULL) || (ee == NULL)) {
        isInitialized = false;
        return;
    }

    ai[0] = 0.0;    
    ai[1] = 2.0 / 9.0;    
    ai[2] = 1.0 / 3.0;    
    ai[3] = 5.0 / 9.0;    
    ai[4] = 2.0 / 3.0;    
    ai[5] = 1.0;    
    ai[6] = 1.0;    

    bij[0][0] =  0.0;

    bij[1][0] =  2.0 / 9.0;
    bij[1][1] =  0.0;

    bij[2][0] =  1.0 / 12.0;
    bij[2][1] =  1.0 / 4.0;
    bij[2][2] =  0.0;

    bij[3][0] =  55.0 / 324.0;
    bij[3][1] = -25.0 / 108.0;
    bij[3][2] =  50.0 / 81.0;
    bij[3][3] =  0.0;

    bij[4][0] =  83.0 / 330.0;
    bij[4][1] = -13.0 / 22.0;
    bij[4][2] =  61.0 / 66.0;
    bij[4][3] =  9.0 / 110.0;
    bij[4][4] =  0.0;

    bij[5][0] = -19.0 / 28.0;
    bij[5][1] =  9.0 / 4.0;
    bij[5][2] =  1.0 / 7.0;
    bij[5][3] = -27.0 / 7.0;
    bij[5][4] =  22.0 / 7.0;
    bij[5][5] =  0.0;

    bij[6][0] =  19.0 / 200.0;
    bij[6][1] =  0.0;
    bij[6][2] =  3.0 / 5.0;
    bij[6][3] = -243.0 / 400.0;
    bij[6][4] =  33.0 / 40.0;
    bij[6][5] =  7.0 / 80.0;
    bij[6][6] =  0.0;


    cj[0] =  19.0 / 200.0;
    cj[1] =  0.0;
    cj[2] =  3.0 / 5.0;
    cj[3] = -243.0 / 400.0;
    cj[4] =  33.0 / 40.0;
    cj[5] =  7.0 / 80.0;
    cj[6] =  0.0;

    ee[0] =  19.0 / 200.0  - 431.0 / 5000.0;
    ee[1] =  0.0;
    ee[2] =  3.0 / 5.0     - 333.0 / 500.0;
    ee[3] = -243.0 / 400.0 + 7857.0 / 10000.0;
    ee[4] =  33.0 / 40.0   - 957.0 / 1000.0;
    ee[5] =  7.0 / 80.0    - 193.0 / 2000.0;
    ee[6] =                  1.0 / 50.0;
}
