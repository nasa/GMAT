//$Id$
//------------------------------------------------------------------------------
//                              RungeKutta89
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
// *** File Name : RungeKutta89.cpp
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
//
//                           : 09/24/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/21/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
// **************************************************************************

#include "gmatdefs.hpp"
#include "RungeKutta89.hpp"

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// RungeKutta89::RungeKutta89()
//------------------------------------------------------------------------------
/**
 * The default constructor
 */
//------------------------------------------------------------------------------
//RungeKutta89::RungeKutta89() :
//RungeKutta (16, 9, NULL, NULL, NULL)
//{
//}

//------------------------------------------------------------------------------
// RungeKutta89::RungeKutta89(const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * The constructor
 */
//------------------------------------------------------------------------------
RungeKutta89::RungeKutta89(const std::string &nomme) :
    RungeKutta (16, 9, "RungeKutta89", nomme)
{
}

//------------------------------------------------------------------------------
// RungeKutta89::~RungeKutta89()
//------------------------------------------------------------------------------
/**
 * The destructor
 */
//------------------------------------------------------------------------------
RungeKutta89::~RungeKutta89()
{
}

//------------------------------------------------------------------------------
// RungeKutta89::RungeKutta89(const RungeKutta89& rk)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
RungeKutta89::RungeKutta89(const RungeKutta89& rk) :
    RungeKutta (rk)
{
}

//------------------------------------------------------------------------------
// RungeKutta89 & RungeKutta89::operator=(const RungeKutta89& rk)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
RungeKutta89 & RungeKutta89::operator=(const RungeKutta89& rk)
{
    if (this == &rk)
        return *this;

    RungeKutta::operator=(rk);

    return *this;
}

//------------------------------------------------------------------------------
// GmatBase* RungeKutta89::Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* RungeKutta89::Clone() const
{
    return new RungeKutta89(*this);
}

//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// void RungeKutta89::SetCoefficients()
//------------------------------------------------------------------------------
/**
 * The function to set the coefficients for the RK 8(9) integrator
 * This method, called from Initialize, sets the coefficients for a given 
 * Runge-Kutta integrator.  Derived classes must allocate and initialize the 
 * stages, ai, bij, cj, and ee.  The memory allocation is managed by the 
 * Initialize() method. The coefficients in this class are the coefficients
 * found in Verner's 1978 (Siam J. Numer. Anal., 15(4)) paper.
 */
//------------------------------------------------------------------------------
void RungeKutta89::SetCoefficients()
{
    if ((ai == NULL) || (bij == NULL) || (cj == NULL) || (ee == NULL)) {
        isInitialized = false;
        return;
    }

    Real rt6 = sqrt(6.0);

    ai[0] = 0.0;    
    ai[1] = 1.0 / 12.0;    
    ai[2] = 1.0 / 9.0;
    ai[3] = 1.0 / 6.0;
    ai[4] = (2.0 + 2.0 * rt6) / 15.0;    
    ai[5] = (6.0 + rt6) / 15.0;
    ai[6] = (6.0 - rt6) / 15.0;    
    ai[7] = 2.0 / 3.0;    
    ai[8] = 1.0 / 2.0;    
    ai[9] = 1.0 / 3.0;    
    ai[10] = 1.0 / 4.0;    
    ai[11] = 4.0 / 3.0;    
    ai[12] = 5.0 / 6.0;    
    ai[13] = 1.0;    
    ai[14] = 1.0 / 6.0;    
    ai[15] = 1.0;    


    bij[0][0] =  0.0;

    bij[1][0] = 1.0 / 12.0;
    bij[1][1] = 0.0;

    bij[2][0] = 1.0 / 27.0;
    bij[2][1] = 2.0 / 27.0;
    bij[2][2] = 0.0;

    bij[3][0] = 1.0 / 24.0;
    bij[3][1] = 0.0;
    bij[3][2] = 1.0 / 8.0;
    bij[3][3] = 0.0;

    bij[4][0] = (4.0 + 94.0 * rt6) / 375.0;
    bij[4][1] = 0.0;
    bij[4][2] = (-94.0 - 84.0 * rt6) / 125.0;
    bij[4][3] =  (328.0 + 208.0 * rt6) / 375.0;
    bij[4][4] =   0.0;

    bij[5][0] = (9.0 - rt6) / 150.0;
    bij[5][1] = 0.0;
    bij[5][2] = 0.0;
    bij[5][3] = (312.0 + 32.0 * rt6) / 1425.0;
    bij[5][4] = (69.0 + 29.0 * rt6) / 570.0;
    bij[5][5] = 0.0;

    bij[6][0] = (927.0 - 347.0 * rt6) / 1250.0;
    bij[6][1] = 0.0;
    bij[6][2] = 0.0;
    bij[6][3] = (-16248.0 + 7328.0 * rt6) / 9375.0;
    bij[6][4] = (-489.0 + 179.0 * rt6) / 3750.0;
    bij[6][5] = (14268.0 - 5798.0 * rt6) / 9375.0;
    bij[6][6] = 0.0;

    bij[7][0] = 2.0 / 27.0;
    bij[7][1] = 0.0;
    bij[7][2] = 0.0;
    bij[7][3] = 0.0;
    bij[7][4] = 0.0;
    bij[7][5] = (16.0 - rt6) / 54.0;
    bij[7][6] = (16.0 + rt6) / 54.0;
    bij[7][7] = 0.0;

    bij[8][0] = 19.0 /256.0;
    bij[8][1] = 0.0;
    bij[8][2] = 0.0;
    bij[8][3] = 0.0;
    bij[8][4] = 0.0;
    bij[8][5] = (118.0 - 23.0 * rt6) / 512.0;
    bij[8][6] = (118.0 + 23.0 * rt6) / 512.0;
    bij[8][7] = -9.0 / 256.0;
    bij[8][8] = 0.0;

    bij[9][0] = 11.0 / 144.0;
    bij[9][1] = 0.0;
    bij[9][2] = 0.0;
    bij[9][3] = 0.0;
    bij[9][4] = 0.0;
    bij[9][5] = (266.0 - rt6) / 864.0;
    bij[9][6] = (266.0 + rt6) / 864.0;
    bij[9][7] = -1.0 / 16.0;
    bij[9][8] = -8.0 / 27.0;
    bij[9][9] = 0.0;

    bij[10][ 0] = (5034.0 - 271.0 * rt6) / 61440.0;
    bij[10][ 1] = 0.0;
    bij[10][ 2] = 0.0;
    bij[10][ 3] = 0.0;
    bij[10][ 4] = 0.0;
    bij[10][ 5] = 0.0;
    bij[10][ 6] = (7859.0 - 1626.0 * rt6) / 10240.0;
    bij[10][ 7] = (-2232.0 + 813.0 * rt6) / 20480.0;
    bij[10][ 8] = (-594.0 + 271.0 * rt6) / 960.0;
    bij[10][ 9] = (657.0 - 813.0 * rt6) / 5120.0;
    bij[10][10] = 0.0;

    bij[11][ 0] = (5996.0 - 3794.0 * rt6) / 405.0;
    bij[11][ 1] = 0.0;
    bij[11][ 2] = 0.0;
    bij[11][ 3] = 0.0;
    bij[11][ 4] = 0.0;
    bij[11][ 5] = (-4342.0 - 338.0 * rt6) / 9.0;
    bij[11][ 6] = (154922.0 - 40458.0 * rt6) / 135.0;
    bij[11][ 7] = (-4176.0 + 3794.0 * rt6) / 45.0;
    bij[11][ 8] = (-340864.0 + 242816.0 * rt6) / 405.0;
    bij[11][ 9] = (26304.0 - 15176.0 * rt6) / 45.0;
    bij[11][10] = -26624.0 / 81.0;
    bij[11][11] = 0.0;

    bij[12][ 0] = (3793.0 + 2168.0 * rt6) / 103680.0;
    bij[12][ 1] = 0.0;
    bij[12][ 2] = 0.0;
    bij[12][ 3] = 0.0;
    bij[12][ 4] = 0.0;
    bij[12][ 5] = (4042.0 + 2263.0 * rt6) / 13824.0;
    bij[12][ 6] = (-231278.0 + 40717.0 * rt6) / 69120.0;
    bij[12][ 7] = (7947.0 - 2168.0 * rt6) / 11520.0;
    bij[12][ 8] = (1048.0 - 542.0 * rt6) / 405.0;
    bij[12][ 9] = (-1383.0 + 542.0 * rt6) / 720.0;
    bij[12][10] = 2624.0 / 1053.0;
    bij[12][11] = 3.0 / 1664.0;
    bij[12][12] = 0.0;

    bij[13][ 0] = -137.0 / 1296.0;
    bij[13][ 1] = 0.0;
    bij[13][ 2] = 0.0;
    bij[13][ 3] = 0.0;
    bij[13][ 4] = 0.0;
    bij[13][ 5] = (5642.0 - 337.0 * rt6) / 864.0;
    bij[13][ 6] = (5642.0 + 337.0 * rt6) / 864.0;
    bij[13][ 7] = -299.0 / 48.0;
    bij[13][ 8] = 184.0 / 81.0;
    bij[13][ 9] = -44.0 / 9.0;
    bij[13][10] = -5120.0 / 1053.0;
    bij[13][11] = -11.0 / 468.0;
    bij[13][12] = 16.0 / 9.0;
    bij[13][13] = 0.0;

    bij[14][ 0] = (33617.0 - 2168.0 * rt6) / 518400.0;
    bij[14][ 1] = 0.0;
    bij[14][ 2] = 0.0;
    bij[14][ 3] = 0.0;
    bij[14][ 4] = 0.0;
    bij[14][ 5] = (-3846.0 + 31.0 * rt6) / 13824.0;
    bij[14][ 6] = (155338.0 - 52807.0 * rt6) / 345600.0;
    bij[14][ 7] = (-12537.0 + 2168.0 * rt6) / 57600.0;
    bij[14][ 8] = (92.0 + 542.0 * rt6) / 2025.0;
    bij[14][ 9] = (-1797.0 - 542.0 * rt6) / 3600.0;
    bij[14][10] = 320.0 / 567.0;
    bij[14][11] = -1.0 / 1920.0;
    bij[14][12] = 4.0 / 105.0;
    bij[14][13] = 0.0;
    bij[14][14] = 0.0;

    bij[15][ 0] = (-36487.0 - 30352.0 * rt6) / 279600.0;
    bij[15][ 1] = 0.0;
    bij[15][ 2] = 0.0;
    bij[15][ 3] = 0.0;
    bij[15][ 4] = 0.0;
    bij[15][ 5] = (-29666.0 - 4499.0 * rt6) / 7456.0;
    bij[15][ 6] = (2779182.0 - 615973.0 * rt6) / 186400.0;
    bij[15][ 7] = (-94329.0 + 91056.0 * rt6) / 93200.0;
    bij[15][ 8] = (-232192.0 + 121408.0 * rt6) / 17475.0;
    bij[15][ 9] = (101226.0 - 22764.0 * rt6) / 5825.0;
    bij[15][10] = -169984.0 / 9087.0;
    bij[15][11] = -87.0 / 30290.0;
    bij[15][12] = 492.0 / 1165.0;
    bij[15][13] = 0.0;
    bij[15][14] = 1260.0 / 233.0;
    bij[15][15] = 0.0;

    cj[ 0] = 23.0 / 525.0;
    cj[ 1] = 0.0;
    cj[ 2] = 0.0;
    cj[ 3] = 0.0;
    cj[ 4] = 0.0;
    cj[ 5] = 0.0;
    cj[ 6] = 0.0;
    cj[ 7] = 171.0 / 1400.0;
    cj[ 8] = 86.0 / 525.0;
    cj[ 9] = 93.0 / 280.0;
    cj[10] = -2048.0 / 6825.0;
    cj[11] = -3.0 / 18200.0;
    cj[12] = 39.0 / 175.0;
    cj[13] = 0.0;
    cj[14] = 9.0 / 25.0;
    cj[15] = 233.0 / 4200.0;

    ee[ 0] = -7.0 / 400.0;
    ee[ 1] = 0.0;
    ee[ 2] = 0.0;
    ee[ 3] = 0.0;
    ee[ 4] = 0.0;
    ee[ 5] = 0.0;
    ee[ 6] = 0.0;
    ee[ 7] = 63.0 / 200.0;
    ee[ 8] = -14.0 / 25.0;
    ee[ 9] = 21.0 / 20.0;
    ee[10] = -1024.0 / 975.0;
    ee[11] = -21.0 / 36400.0;
    ee[12] = -3.0 / 25.0;
    ee[13] = -9.0 / 280.0;
    ee[14] = 9.0 / 25.0;
    ee[15] = 233.0 / 4200.0;
}

