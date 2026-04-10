//$Id$
//------------------------------------------------------------------------------
//                        PrinceDormand78
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
// *** File Name : PrinceDormand78.cpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
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
//                           : 6/1/2004 - A. Greene, Missions Applications Branch
// **************************************************************************

#include "gmatdefs.hpp"
#include "PrinceDormand78.hpp"

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// PrinceDormand78::PrinceDormand78()
//------------------------------------------------------------------------------
/**
 * The constructor
 */
//------------------------------------------------------------------------------
//PrinceDormand78::PrinceDormand78() :
//    RungeKutta      (13, 8)
//{
//}
PrinceDormand78::PrinceDormand78(const std::string &nomme) :
    RungeKutta      (13, 8, "PrinceDormand78", nomme)
{
}

//------------------------------------------------------------------------------
// PrinceDormand78::~PrinceDormand78()
//------------------------------------------------------------------------------
/**
 * The destructor
 */
//------------------------------------------------------------------------------
PrinceDormand78::~PrinceDormand78()
{
}


//------------------------------------------------------------------------------
// PrinceDormand78::PrinceDormand78(const PrinceDormand78& rk)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
PrinceDormand78::PrinceDormand78(const PrinceDormand78& rk) :
    RungeKutta      (rk)
{
}


//------------------------------------------------------------------------------
// PrinceDormand78 & PrinceDormand78::operator=(const PrinceDormand78& rk)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
PrinceDormand78 & PrinceDormand78::operator=(const PrinceDormand78& rk)
{
    if (this == &rk)
        return *this;

    RungeKutta::operator=(rk);

    return *this;
}


//------------------------------------------------------------------------------
// GmatBase* PrinceDormand78::Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* PrinceDormand78::Clone() const
{
    return new PrinceDormand78(*this);
}

//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// void PrinceDormand78::SetCoefficients()
//------------------------------------------------------------------------------
/**
 * The function sets the coefficients for the Prince-Dormand 7(8) integrator.
 * This method, called from Initialize, initializes the stages, ai, bij, cj, and 
 * ee arrays for the Prince-Dormand 7(8) integrator. The coefficients in this 
 * class are the coefficients found in Prince and Dormand, 1981.  
 *
 * These coefficients were retrieved from an implementation provided by Goddard 
 * Space Flight Center.  Thinking Systems reformatted the coefficients into this 
 * form, and would like to express appreciation to GSFC for providing the tables 
 * of coefficients during a visit September 2002.
 */
//------------------------------------------------------------------------------
void PrinceDormand78::SetCoefficients()
{
    if ((ai == NULL) || (bij == NULL) || (cj == NULL) || (ee == NULL)) {
        isInitialized = false;
        return;
    }

    ai[0] = 0.0;
    ai[1] = 1.0 / 18.0;
    ai[2] = 1.0 / 12.0;
    ai[3] = 0.125;
    ai[4] = 5.0 / 16.0;
    ai[5] = 3.0 / 8.0;
    ai[6] = 59.0 / 400.0;
    ai[7] = 93.0 / 200.0;
    ai[8] = 5490023248.0 / 9719169821.0;
    ai[9] = 13.0 / 20.0;
    ai[10] = 1201146811.0 / 1299019798.0;
    ai[11] =  1.0;
    ai[12] =  1.0;

    bij[0][0] = 0.0;

    bij[1][0] = 1.0 / 18.0;

    bij[2][0] = 1.0 / 48.0;
    bij[2][1] = 1.0 / 16.0;

    bij[3][0] = 1.0 / 32.0;
    bij[3][1] = 0.0;
    bij[3][2] = 3.0 / 32.0;

    bij[4][0] = 5.0 / 16.0;
    bij[4][1] = 0.0;
    bij[4][2] = -75.0 / 64.0;
    bij[4][3] = 75.0 / 64.0;

    bij[5][0] = 3.0 / 80.0;
    bij[5][1] = 0.0;
    bij[5][2] = 0.0;
    bij[5][3] = 3.0 / 16.0;
    bij[5][4] = 3.0 / 20.0;

    bij[6][0] = 29443841.0 / 614563906.0;
    bij[6][1] = 0.0;
    bij[6][2] = 0.0;
    bij[6][3] = 77736538.0 / 692538347.0;
    bij[6][4] = -28693883.0 / 1125000000.0;
    bij[6][5] = 23124283.0 / 1800000000.0;   

    bij[7][0] = 16016141.0 / 946692911.0;
    bij[7][1] = 0.0;
    bij[7][2] = 0.0;
    bij[7][3] = 61564180.0 / 158732637.0;
    bij[7][4] = 22789713.0 / 633445777.0;
    bij[7][5] = 545815736.0 / 2771057229.0;      
    bij[7][6] = -180193667.0 / 1043307555.0;

    bij[8][0] = 39632708.0 / 573591083.0;
    bij[8][1] = 0.0;
    bij[8][2] = 0.0;
    bij[8][3] = -433636366.0 / 683701615.0;
    bij[8][4] = -421739975.0 / 2616292301.0;
    bij[8][5] = 100302831.0 / 723423059.0;      
    bij[8][6] = 790204164.0 / 839813087.0;
    bij[8][7] = 800635310.0 / 3783071287.0;

    bij[9][0] = 246121993.0 / 1340847787.0;
    bij[9][1] = 0.0;
    bij[9][2] = 0.0;
    bij[9][3] = -37695042795.0 / 15268766246.0;
    bij[9][4] = -309121744.0 / 1061227803.0;
    bij[9][5] = -12992083.0 / 490766935.0;
    bij[9][6] = 6005943493.0 / 2108947869.0;
    bij[9][7] = 393006217.0 / 1396673457.0;
    bij[9][8] = 123872331.0 / 1001029789.0;

    bij[10][0] = -1028468189.0 / 846180014.0;
    bij[10][1] = 0.0;
    bij[10][2] = 0.0;
    bij[10][3] = 8478235783.0 / 508512852.0;
    bij[10][4] = 1311729495.0 / 1432422823.0;
    bij[10][5] = -10304129995.0 / 1701304382.0;
    bij[10][6] = -48777925059.0 / 3047939560.0;
    bij[10][7] = 15336726248.0 / 1032824649.0;
    bij[10][8] = -45442868181.0 / 3398467696.0;
    bij[10][9] = 3065993473.0 / 597172653.0;

    bij[11][0] = 185892177.0 / 718116043.0;
    bij[11][1] = 0.0;
    bij[11][2] = 0.0;
    bij[11][3] = -3185094517.0 / 667107341.0;
    bij[11][4] = -477755414.0 / 1098053517.0;
    bij[11][5] = -703635378.0 / 230739211.0;
    bij[11][6] = 5731566787.0 / 1027545527.0;
    bij[11][7] = 5232866602.0 / 850066563.0;
    bij[11][8] = -4093664535.0 / 808688257.0;
    bij[11][9] = 3962137247.0 / 1805957418.0;
    bij[11][10] = 65686358.0 / 487910083.0;
            
    bij[12][0] = 403863854.0 / 491063109.0;
    bij[12][1] = 0.0;
    bij[12][2] = 0.0;
    bij[12][3] = -5068492393.0 / 434740067.0;
    bij[12][4] = -411421997.0 / 543043805.0;
    bij[12][5] = 652783627.0 / 914296604.0;
    bij[12][6] = 11173962825.0 / 925320556.0;
    bij[12][7] = -13158990841.0 / 6184727034.0;
    bij[12][8] = 3936647629.0 / 1978049680.0;
    bij[12][9] = -160528059.0 / 685178525.0;
    bij[12][10] = 248638103.0 / 1413531060.0;
    bij[12][11] = 0.0;

    cj[0] = 14005451.0 / 335480064.0;
    cj[1] =  0.0;
    cj[2] =  0.0;
    cj[3] =  0.0;
    cj[4] =  0.0;
    cj[5] = -59238493.0 / 1068277825.0;
    cj[6] = 181606767.0 / 758867731.0;
    cj[7] = 561292985.0 / 797845732.0;
    cj[8] = -1041891430.0 / 1371343529.0;
    cj[9] = 760417239.0 / 1151165299.0;
    cj[10] = 118820643.0 / 751138087.0;
    cj[11] = -528747749.0 / 2220607170.0;
    cj[12] =  0.25;

    Real cjhat[13];

    cjhat[0] = 13451932.0 / 455176623.0;
    cjhat[1] =  0.0;
    cjhat[2] =  0.0;
    cjhat[3] =  0.0;
    cjhat[4] =  0.0;
    cjhat[5] = -808719846.0 / 976000145.0;
    cjhat[6] = 1757004468.0 / 5645159321.0;
    cjhat[7] = 656045339.0 / 265891186.0;
    cjhat[8] = -3867574721.0 / 1518517206.0;
    cjhat[9] = 465885868.0 / 322736535.0;
    cjhat[10] = 53011238.0 / 667516719.0;
    cjhat[11] = 2.0 / 45.0;
    cjhat[12] = 0.0;

    for (int i = 0; i < stages; i++)
        ee[i] = cj[i] - cjhat[i];
}
