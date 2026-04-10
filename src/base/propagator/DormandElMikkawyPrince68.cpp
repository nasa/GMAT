//$Id$
//------------------------------------------------------------------------------
//                             dormandelmikkawyprince68
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
// *** File Name : dormandelmikkawyprince68.cpp
// *** Created   : January 28, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
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
// Modification History      : 1/28/03 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
// **************************************************************************
//                                    Changes
//                           : 02/21/2004 - L. Ruley, Missions Applications Branch
//                                            - Updated style using GMAT cpp style guide
//                                            - All Real types to Real types
//                            - All int types to Integer types
// **************************************************************************


#include "DormandElMikkawyPrince68.hpp"

/** \brief The default constructor */
DormandElMikkawyPrince68::DormandElMikkawyPrince68(const std::string &nomme) :
    RungeKuttaNystrom           (9, 8, "RungeKutta68"
          /*"DormandElMikkawyPrince68"*/, nomme)
{
    derivativeError = true;
}


/** \brief The destructor */
DormandElMikkawyPrince68::~DormandElMikkawyPrince68()
{
}


/** \brief The copy constructor */
DormandElMikkawyPrince68::DormandElMikkawyPrince68
                                        (const DormandElMikkawyPrince68& dpn) :
    RungeKuttaNystrom           (dpn)
{
    derivativeError = true;
}


DormandElMikkawyPrince68& DormandElMikkawyPrince68::operator=
                                        (const DormandElMikkawyPrince68& dpn)
{
    if (this == &dpn)
        return *this;

    RungeKuttaNystrom::operator=(dpn);
    return *this;
}


/** \brief Method used to create a copy of the object

  Each class that can be instantiated provides an implementation of this
  method so that copies can be made from a base class pointer. 
 */
GmatBase* DormandElMikkawyPrince68::Clone() const
{
    return new DormandElMikkawyPrince68(*this);
}


/** \brief Sets coefficients for Dormand-ElMikkawy-Prince 6(8) Integrator */
void DormandElMikkawyPrince68::SetCoefficients()
{
    if (!isInitialized || (ai == NULL) || (bij == NULL) || (cj == NULL) ||
        (ee == NULL) || (eeDeriv == NULL) || (cdotj == NULL) || (ki == NULL)) {
       isInitialized = false;
        throw PropagatorException("DormandElMikkawyPrince68 cannot set coefficients");
    }

    // Fill in ai, bij, and cj
    ai[0] = 0.0;
    ai[1] = 1.0 / 20.0;
    ai[2] = 1.0 / 10.0;
    ai[3] = 3.0 / 10.0;
    ai[4] = 1.0 / 2.0;
    ai[5] = 7.0 / 10.0;
    ai[6] = 9.0 / 10.0;
    ai[7] = 1.0;
    ai[8] = 1.0;

    bij[0][0] = 0.0;

    bij[1][0] = 1.0 / 800.0;
    bij[1][1] = 0.0;
    
    bij[2][0] = 1.0 / 600.0;
    bij[2][1] = 1.0 / 300.0;
    bij[2][2] = 0.0;

    bij[3][0] = 9.0 / 200.0;
    bij[3][1] = -9.0 / 100.0;
    bij[3][2] = 9.0 / 100.0;
    bij[3][3] = 0.0;

    bij[4][0] = -66701.0 / 197352.0;
    bij[4][1] = 28325.0 / 32892.0;
    bij[4][2] = -2665.0 / 5482.0;
    bij[4][3] = 2170.0 / 24669.0;
    bij[4][4] = 0.0;

    bij[5][0] = 227015747.0 / 304251000.0;
    bij[5][1] = -54897451.0 / 30425100.0;
    bij[5][2] = 12942349.0 / 10141700.0;
    bij[5][3] = -9499.0 / 304251.0;
    bij[5][4] = 539.0 / 9250.0;
    bij[5][5] = 0.0;

    bij[6][0] = -1131891597.0 / 901789000.0;
    bij[6][1] = 41964921.0 / 12882700.0;
    bij[6][2] = -6663147.0 / 3220675.0;
    bij[6][3] = 270954.0 / 644135.0;
    bij[6][4] = -108.0 / 5875.0;
    bij[6][5] = 114.0 / 1645.0;
    bij[6][6] = 0.0;

    bij[7][0] = 13836959.0 / 3667458.0;
    bij[7][1] = -17731450.0 / 1833729.0;
    bij[7][2] = 1063919505.0 / 156478208.0;
    bij[7][3] = -33213845.0 / 39119552.0;
    bij[7][4] = 13335.0 / 28544.0;
    bij[7][5] = -705.0 / 14272.0;
    bij[7][6] = 1645.0 / 57088.0;
    bij[7][7] = 0.0;

    bij[8][0] = 223.0 / 7938.0;
    bij[8][1] = 0.0;
    bij[8][2] = 1175.0 / 8064.0;
    bij[8][3] = 925.0 / 6048.0;
    bij[8][4] = 41.0 / 448.0;
    bij[8][5] = 925.0 / 14112.0;
    bij[8][6] = 1175.0 / 72576.0;
    bij[8][7] = 0.0;
    bij[8][8] = 0.0;

    cj[0] = 223.0 / 7938.0;
    cj[1] = 0.0;
    cj[2] = 1175.0 / 8064.0;
    cj[3] = 925.0 / 6048.0;
    cj[4] = 41.0 / 448.0;
    cj[5] = 925.0 / 14112.0;
    cj[6] = 1175.0 / 72576.0;
    cj[7] = 0.0;
    cj[8] = 0.0;


    // Fill in cdotj
    cdotj[0] = 223.0 / 7938.0;
    cdotj[1] = 0.0;
    cdotj[2] = 5875.0 / 36288.0;
    cdotj[3] = 4625.0 / 21168.0;
    cdotj[4] = 41.0 / 224.0;
    cdotj[5] = 4625.0 / 21168.0;
    cdotj[6] = 5875.0 / 36288.0;
    cdotj[7] = 223.0 / 7938.0;
    cdotj[8] = 0.0;

    // Fill in ee
    ee[0] = cj[0] - 7987313.0 / 109941300.0;
    ee[1] = 0.0;
    ee[2] = cj[2] - 1610737.0 / 44674560.0;
    ee[3] = cj[3] - 10023263.0 / 33505920.0;
    ee[4] = cj[4] + 497221.0 / 12409600.0;
    ee[5] = cj[5] - 10023263.0 / 78180480.0;
    ee[6] = cj[6] - 1610737.0 / 402071040.0;
    ee[7] = 0.0;
    ee[8] = 0.0;

    eeDeriv[0] = cdotj[0] - 7987313.0 / 109941300.0;
    eeDeriv[1] = 0.0;
    eeDeriv[2] = cdotj[2] - 1610737.0 / 40207104.0;
    eeDeriv[3] = cdotj[3] - 10023263.0 / 23454144.0;
    eeDeriv[4] = cdotj[4] + 497221.0 / 6204800.0;
    eeDeriv[5] = cdotj[5] - 10023263.0 / 23454144.0;
    eeDeriv[6] = cdotj[6] - 1610737.0 / 40207104.0;
    eeDeriv[7] = cdotj[7] + 4251941.0 / 54970650.0;
    eeDeriv[8] = cdotj[8] - 3.0 / 20.0;
}
