//------------------------------------------------------------------------------
//                         Magnetic Field
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Author: Ed Behn
// Created: 20126/01/15
//
/**
 * Store the the Earth's magnetic field as harmonic coefficients at a point in time.
 * Logic copied from the International Reference Ionosphere 2007 model
 */
//------------------------------------------------------------------------------

#include "MagneticField.hpp"

#include "GmatConstants.hpp"
#include <cmath>

const Real MagneticField::EARTH_RADIUS = 6371.2;

const Real MagneticField::EREQU = 6378.16;
const Real MagneticField::ERPOL = 6356.775;
const Real MagneticField::AQUAD = MagneticField::EREQU * MagneticField::EREQU;
const Real MagneticField::BQUAD = MagneticField::ERPOL * MagneticField::ERPOL;

//------------------------------------------------------------------------------
// MagneticField()
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
//------------------------------------------------------------------------------
MagneticField::MagneticField()
{
    sl0 = 0.0;
    cl0 = 0.0;
    st0 = 0.0;
    ct0 = 0.0;
    stcl = 0.0;
    stsl = 0.0;
    ctsl = 0.0;
    ctcl = 0.0;
}

//-----------------------------------------------------------------------------
// MagneticField::configure()
//-----------------------------------------------------------------------------
/**
 * Calculate fields used to calculate geomagnetic latitude (geomag_lat method)
 */
//-----------------------------------------------------------------------------
void MagneticField::configure()
{
    Real g10 = data[1][0].first;
    Real g11 = data[1][1].first;
    Real h11 = data[1][1].second;
    Real sqq = std::hypot(g11, h11);
    Real sqr = std::hypot(g10, g11, h11);
    sl0 = -h11 / sqq;
    cl0 = -g11 / sqq;
    st0 = sqq / sqr;
    ct0 = -g10 / sqr;
    stcl = st0 * cl0;
    stsl = st0 * sl0;
    ctsl = ct0 * sl0;
    ctcl = ct0 * cl0;
}

//------------------------------------------------------------------------------
// MagneticField(const MagneticField & magnetic_field)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
MagneticField::MagneticField(const MagneticField & magnetic_field) :
    data(magnetic_field.data),
    ctcl(magnetic_field.ctcl),
    ctsl(magnetic_field.ctsl),
    st0(magnetic_field.st0),
    cl0(magnetic_field.cl0),
    sl0(magnetic_field.sl0),
    stcl(magnetic_field.stcl),
    stsl(magnetic_field.stsl),
    ct0(magnetic_field.ct0)
{}

//-----------------------------------------------------------------------------
// geomag_lat(const Real sin_lat, const Real cos_lat, const Real sin_lon, const Real cos_lon)
//-----------------------------------------------------------------------------
/**
 * Calculate geomagetic latitude (degrees)
 *
 * @param sin_lat sin(latitude)
 * @param cos_lat cos(latitude)
 * @param sin_lon sin(longitude)
 * @param cos_lon cos(longitude)
 *
 * @return geomagetic latitude (degrees)
 */
//-----------------------------------------------------------------------------
Real MagneticField::geomag_lat(const Real sin_lat, const Real cos_lat, const Real sin_lon, const Real cos_lon)
{
    Real const x_geo = cos_lat * cos_lon;
    Real const y_geo = cos_lat * sin_lon;
    Real const z_geo = sin_lat;

    Real const x_mag = x_geo * ctcl + y_geo * ctsl - z_geo * st0;
    Real const y_mag = y_geo * cl0 - x_geo * sl0;
    Real const z_mag = x_geo * stcl + y_geo * stsl + z_geo * ct0;

    return atan2(z_mag, std::hypot(x_mag, y_mag)) * GmatMathConstants::DEG_PER_RAD;
}

//-----------------------------------------------------------------------------
// get_field(const Real sin_lat, const Real cos_lat, const Real sin_lon, const Real cos_lon, Real& magbr, Real& modip, Real& sin_modip)
//-----------------------------------------------------------------------------
/**
 * Calculate direction of mangitic field
 *
 * @param sin_lat sin(latitude)
 * @param cos_lat cos(latitude)
 * @param sin_lon sin(longitude)
 * @param cos_lon cos(longitude)
 * @param magbr (degrees)
 * @param modip modified dipole (degrees)
 * @param sin_modip sin(modip)
 */
//-----------------------------------------------------------------------------
void MagneticField::get_field(const Real sin_lat, const Real cos_lat, const Real sin_lon, const Real cos_lon, Real& magbr, Real& modip, Real& sin_modip)
{
    const Integer max_degree = data.size() - 1;

    // Convert to spherical coordinates
    const Real height = 300.0; // km
    const Real d = sqrt(AQUAD - (AQUAD - BQUAD) * sin_lat * sin_lat);
    const Real z = (height + BQUAD / d) * sin_lat / EARTH_RADIUS;
    const Real rho = (height + AQUAD / d) * cos_lat / EARTH_RADIUS;
    const Real r = std::hypot(rho, z);
    const Real sin_theta = rho / r;
    const Real cos_theta = z / r;

    RealArray sin_theta_powers(max_degree + 1);
    sin_theta_powers[0] = 1.0;
    sin_theta_powers[1] = sin_theta;

    RealArray cos_theta_powers(max_degree + 1);
    cos_theta_powers[0] = 1.0;
    cos_theta_powers[1] = cos_theta;

    RealArray sin_n_phi(max_degree + 1);
    sin_n_phi[0] = 0.0;
    sin_n_phi[1] = sin_lon;

    RealArray cos_n_phi(max_degree + 1);
    cos_n_phi[0] = 1.0;
    cos_n_phi[1] = cos_lon;

    for (Integer n = 2; n <= max_degree; n++)
    {
        sin_theta_powers[n] = sin_theta_powers[n-1] * sin_theta;
        cos_theta_powers[n] = cos_theta_powers[n-1] * cos_theta;

        Integer idx1 = (n >> 1), idx2 = n - idx1;
        sin_n_phi[n] = sin_n_phi[idx1] * cos_n_phi[idx2] + cos_n_phi[idx1] * sin_n_phi[idx2];
        cos_n_phi[n] = cos_n_phi[idx1] * cos_n_phi[idx2] - sin_n_phi[idx1] * sin_n_phi[idx2];
    }

    Real b_r = 0.0, b_theta = 0.0, b_phi = 0.0; // Spherical components of the magnetic field

    Real denom = r * r;
    for (Integer degree = 1; degree <= max_degree; degree++)
    {
        denom *= r;
        for (Integer order = 0; order <= degree; order++)
        {
            const Real g = data[degree][order].first;
            const Real h = data[degree][order].second;

            Real P, dP;

            switch (degree)
            {
            case 0:
                switch (order)
                {
                case 0:
                    P = 1.0;
                    dP = 0;
                    break;
                }
                break;
            case 1:
                switch (order)
                {
                case 0:
                    P = cos_theta_powers[1];
                    dP = -sin_theta_powers[1];
                    break;
                case 1:
                    P = sin_theta_powers[1];
                    dP = cos_theta_powers[1];
                    break;
                }
                break;
            case 2:
                switch (order)
                {
                case 0:
                    P = 1.5 * cos_theta_powers[2] - 0.5;
                    dP = -3.0 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 1:
                    P = 1.73205080756888 * cos_theta_powers[1] * sin_theta_powers[1];
                    dP = 1.73205080756888 * cos_theta_powers[2] - 1.73205080756888 * sin_theta_powers[2];
                    break;
                case 2:
                    P = 0.866025403784439 * sin_theta_powers[2];
                    dP = 1.73205080756888 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                }
                break;
            case 3:
                switch (order)
                {
                case 0:
                    P = 2.5 * cos_theta_powers[3] - 1.5 * cos_theta_powers[1];
                    dP = -7.5 * cos_theta_powers[2] * sin_theta_powers[1] + 1.5 * sin_theta_powers[1];
                    break;
                case 1:
                    P = 3.06186217847897 * cos_theta_powers[2] * sin_theta_powers[1] - 0.612372435695795 * sin_theta_powers[1];
                    dP = 3.06186217847897 * cos_theta_powers[3] - 6.12372435695795 * cos_theta_powers[1] * sin_theta_powers[2] - 0.612372435695795 * cos_theta_powers[1];
                    break;
                case 2:
                    P = 1.93649167310371 * cos_theta_powers[1] * sin_theta_powers[2];
                    dP = 3.87298334620742 * cos_theta_powers[2] * sin_theta_powers[1] - 1.93649167310371 * sin_theta_powers[3];
                    break;
                case 3:
                    P = 0.790569415042095 * sin_theta_powers[3];
                    dP = 2.37170824512628 * cos_theta_powers[1] * sin_theta_powers[2];
                    break;
                }
                break;
            case 4:
                switch (order)
                {
                case 0:
                    P = 4.375 * cos_theta_powers[4] - 3.75 * cos_theta_powers[2] + 0.375;
                    dP = -17.5 * cos_theta_powers[3] * sin_theta_powers[1] + 7.5 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 1:
                    P = 5.53398590529466 * cos_theta_powers[3] * sin_theta_powers[1] - 2.37170824512628 * cos_theta_powers[1] * sin_theta_powers[1];
                    dP = 5.53398590529466 * cos_theta_powers[4] - 16.601957715884 * cos_theta_powers[2] * sin_theta_powers[2] - 2.37170824512628 * cos_theta_powers[2] + 2.37170824512628 * sin_theta_powers[2];
                    break;
                case 2:
                    P = 3.91311896062463 * cos_theta_powers[2] * sin_theta_powers[2] - 0.559016994374947 * sin_theta_powers[2];
                    dP = 7.82623792124926 * cos_theta_powers[3] * sin_theta_powers[1] - 7.82623792124926 * cos_theta_powers[1] * sin_theta_powers[3] - 1.11803398874989 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 3:
                    P = 2.09165006633519 * cos_theta_powers[1] * sin_theta_powers[3];
                    dP = 6.27495019900557 * cos_theta_powers[2] * sin_theta_powers[2] - 2.09165006633519 * sin_theta_powers[4];
                    break;
                case 4:
                    P = 0.739509972887452 * sin_theta_powers[4];
                    dP = 2.95803989154981 * cos_theta_powers[1] * sin_theta_powers[3];
                    break;
                }
                break;
            case 5:
                switch (order)
                {
                case 0:
                    P = 7.875 * cos_theta_powers[5] - 8.75 * cos_theta_powers[3] + 1.875 * cos_theta_powers[1];
                    dP = -39.375 * cos_theta_powers[4] * sin_theta_powers[1] + 26.25 * cos_theta_powers[2] * sin_theta_powers[1] - 1.875 * sin_theta_powers[1];
                    break;
                case 1:
                    P = 10.1665812837945 * cos_theta_powers[4] * sin_theta_powers[1] - 6.77772085586298 * cos_theta_powers[2] * sin_theta_powers[1] + 0.484122918275927 * sin_theta_powers[1];
                    dP = 10.1665812837945 * cos_theta_powers[5] - 40.6663251351779 * cos_theta_powers[3] * sin_theta_powers[2] - 6.77772085586298 * cos_theta_powers[3] + 13.555441711726 * cos_theta_powers[1] * sin_theta_powers[2] + 0.484122918275927 * cos_theta_powers[1];
                    break;
                case 2:
                    P = 7.6852130744697 * cos_theta_powers[3] * sin_theta_powers[2] - 2.5617376914899 * cos_theta_powers[1] * sin_theta_powers[2];
                    dP = 15.3704261489394 * cos_theta_powers[4] * sin_theta_powers[1] - 23.0556392234091 * cos_theta_powers[2] * sin_theta_powers[3] - 5.1234753829798 * cos_theta_powers[2] * sin_theta_powers[1] + 2.5617376914899 * sin_theta_powers[3];
                    break;
                case 3:
                    P = 4.70621264925417 * cos_theta_powers[2] * sin_theta_powers[3] - 0.522912516583797 * sin_theta_powers[3];
                    dP = 14.1186379477625 * cos_theta_powers[3] * sin_theta_powers[2] - 9.41242529850835 * cos_theta_powers[1] * sin_theta_powers[4] - 1.56873754975139 * cos_theta_powers[1] * sin_theta_powers[2];
                    break;
                case 4:
                    P = 2.21852991866236 * cos_theta_powers[1] * sin_theta_powers[4];
                    dP = 8.87411967464942 * cos_theta_powers[2] * sin_theta_powers[3] - 2.21852991866236 * sin_theta_powers[5];
                    break;
                case 5:
                    P = 0.701560760020114 * sin_theta_powers[5];
                    dP = 3.50780380010057 * cos_theta_powers[1] * sin_theta_powers[4];
                    break;
                }
                break;
            case 6:
                switch (order)
                {
                case 0:
                    P = 14.4375 * cos_theta_powers[6] - 19.6875 * cos_theta_powers[4] + 6.5625 * cos_theta_powers[2] - 0.3125;
                    dP = -86.625 * cos_theta_powers[5] * sin_theta_powers[1] + 78.75 * cos_theta_powers[3] * sin_theta_powers[1] - 13.125 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 1:
                    P = 18.9031247416928 * cos_theta_powers[5] * sin_theta_powers[1] - 17.1846588560844 * cos_theta_powers[3] * sin_theta_powers[1] + 2.8641098093474 * cos_theta_powers[1] * sin_theta_powers[1];
                    dP = 18.9031247416928 * cos_theta_powers[6] - 94.5156237084642 * cos_theta_powers[4] * sin_theta_powers[2] - 17.1846588560844 * cos_theta_powers[4] + 51.5539765682532 * cos_theta_powers[2] * sin_theta_powers[2] + 2.8641098093474 * cos_theta_powers[2] - 2.8641098093474 * sin_theta_powers[2];
                    break;
                case 2:
                    P = 14.9442322695079 * cos_theta_powers[4] * sin_theta_powers[2] - 8.15139941973156 * cos_theta_powers[2] * sin_theta_powers[2] + 0.45285552331842 * sin_theta_powers[2];
                    dP = 29.8884645390157 * cos_theta_powers[5] * sin_theta_powers[1] - 59.7769290780314 * cos_theta_powers[3] * sin_theta_powers[3] - 16.3027988394631 * cos_theta_powers[3] * sin_theta_powers[1] + 16.3027988394631 * cos_theta_powers[1] * sin_theta_powers[3] + 0.90571104663684 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 3:
                    P = 9.96282151300524 * cos_theta_powers[3] * sin_theta_powers[3] - 2.71713313991052 * cos_theta_powers[1] * sin_theta_powers[3];
                    dP = 29.8884645390157 * cos_theta_powers[4] * sin_theta_powers[2] - 29.8884645390157 * cos_theta_powers[2] * sin_theta_powers[4] - 8.15139941973156 * cos_theta_powers[2] * sin_theta_powers[2] + 2.71713313991052 * sin_theta_powers[4];
                    break;
                case 4:
                    P = 5.45686207907072 * cos_theta_powers[2] * sin_theta_powers[4] - 0.496078370824611 * sin_theta_powers[4];
                    dP = 21.8274483162829 * cos_theta_powers[3] * sin_theta_powers[3] - 10.9137241581414 * cos_theta_powers[1] * sin_theta_powers[5] - 1.98431348329844 * cos_theta_powers[1] * sin_theta_powers[3];
                    break;
                case 5:
                    P = 2.32681380862329 * cos_theta_powers[1] * sin_theta_powers[5];
                    dP = 11.6340690431164 * cos_theta_powers[2] * sin_theta_powers[4] - 2.32681380862329 * sin_theta_powers[6];
                    break;
                case 6:
                    P = 0.671693289381396 * sin_theta_powers[6];
                    dP = 4.03015973628838 * cos_theta_powers[1] * sin_theta_powers[5];
                    break;
                }
                break;
            case 7:
                switch (order)
                {
                case 0:
                    P = 26.8125 * cos_theta_powers[7] - 43.3125 * cos_theta_powers[5] + 19.6875 * cos_theta_powers[3] - 2.1875 * cos_theta_powers[1];
                    dP = -187.6875 * cos_theta_powers[6] * sin_theta_powers[1] + 216.5625 * cos_theta_powers[4] * sin_theta_powers[1] - 59.0625 * cos_theta_powers[2] * sin_theta_powers[1] + 2.1875 * sin_theta_powers[1];
                    break;
                case 1:
                    P = 35.4696035139597 * cos_theta_powers[6] * sin_theta_powers[1] - 40.9264655930304 * cos_theta_powers[4] * sin_theta_powers[1] + 11.1617633435537 * cos_theta_powers[2] * sin_theta_powers[1] - 0.413398642353842 * sin_theta_powers[1];
                    dP = 35.4696035139597 * cos_theta_powers[7] - 212.817621083758 * cos_theta_powers[5] * sin_theta_powers[2] - 40.9264655930304 * cos_theta_powers[5] + 163.705862372122 * cos_theta_powers[3] * sin_theta_powers[2] + 11.1617633435537 * cos_theta_powers[3] - 22.3235266871075 * cos_theta_powers[1] * sin_theta_powers[2] - 0.413398642353842 * cos_theta_powers[1];
                    break;
                case 2:
                    P = 28.9608099960101 * cos_theta_powers[5] * sin_theta_powers[2] - 22.277546150777 * cos_theta_powers[3] * sin_theta_powers[2] + 3.03784720237868 * cos_theta_powers[1] * sin_theta_powers[2];
                    dP = 57.9216199920203 * cos_theta_powers[6] * sin_theta_powers[1] - 144.804049980051 * cos_theta_powers[4] * sin_theta_powers[3] - 44.555092301554 * cos_theta_powers[4] * sin_theta_powers[1] + 66.8326384523311 * cos_theta_powers[2] * sin_theta_powers[3] + 6.07569440475737 * cos_theta_powers[2] * sin_theta_powers[1] - 3.03784720237868 * sin_theta_powers[3];
                    break;
                case 3:
                    P = 20.4783851368339 * cos_theta_powers[4] * sin_theta_powers[3] - 9.45156237084642 * cos_theta_powers[2] * sin_theta_powers[3] + 0.42961647140211 * sin_theta_powers[3];
                    dP = 61.4351554105017 * cos_theta_powers[5] * sin_theta_powers[2] - 81.9135405473357 * cos_theta_powers[3] * sin_theta_powers[4] - 28.3546871125393 * cos_theta_powers[3] * sin_theta_powers[2] + 18.9031247416928 * cos_theta_powers[1] * sin_theta_powers[4] + 1.28884941420633 * cos_theta_powers[1] * sin_theta_powers[2];
                    break;
                case 4:
                    P = 12.3489308747762 * cos_theta_powers[3] * sin_theta_powers[4] - 2.8497532787945 * cos_theta_powers[1] * sin_theta_powers[4];
                    dP = 49.3957234991047 * cos_theta_powers[4] * sin_theta_powers[3] - 37.0467926243285 * cos_theta_powers[2] * sin_theta_powers[5] - 11.399013115178 * cos_theta_powers[2] * sin_theta_powers[3] + 2.8497532787945 * sin_theta_powers[5];
                    break;
                case 5:
                    P = 6.17446543738808 * cos_theta_powers[2] * sin_theta_powers[5] - 0.474958879799083 * sin_theta_powers[5];
                    dP = 30.8723271869404 * cos_theta_powers[3] * sin_theta_powers[4] - 12.3489308747762 * cos_theta_powers[1] * sin_theta_powers[6] - 2.37479439899542 * cos_theta_powers[1] * sin_theta_powers[4];
                    break;
                case 6:
                    P = 2.4218245962497 * cos_theta_powers[1] * sin_theta_powers[6];
                    dP = 14.5309475774982 * cos_theta_powers[2] * sin_theta_powers[5] - 2.4218245962497 * sin_theta_powers[7];
                    break;
                case 7:
                    P = 0.64725984928775 * sin_theta_powers[7];
                    dP = 4.53081894501425 * cos_theta_powers[1] * sin_theta_powers[6];
                    break;
                }
                break;
            case 8:
                switch (order)
                {
                case 0:
                    P = 50.2734375 * cos_theta_powers[8] - 93.84375 * cos_theta_powers[6] + 54.140625 * cos_theta_powers[4] - 9.84375 * cos_theta_powers[2] + 0.2734375;
                    dP = -402.1875 * cos_theta_powers[7] * sin_theta_powers[1] + 563.0625 * cos_theta_powers[5] * sin_theta_powers[1] - 216.5625 * cos_theta_powers[3] * sin_theta_powers[1] + 19.6875 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 1:
                    P = 67.03125 * cos_theta_powers[7] * sin_theta_powers[1] - 93.84375 * cos_theta_powers[5] * sin_theta_powers[1] + 36.09375 * cos_theta_powers[3] * sin_theta_powers[1] - 3.28125 * cos_theta_powers[1] * sin_theta_powers[1];
                    dP = 67.03125 * cos_theta_powers[8] - 469.21875 * cos_theta_powers[6] * sin_theta_powers[2] - 93.84375 * cos_theta_powers[6] + 469.21875 * cos_theta_powers[4] * sin_theta_powers[2] + 36.09375 * cos_theta_powers[4] - 108.28125 * cos_theta_powers[2] * sin_theta_powers[2] - 3.28125 * cos_theta_powers[2] + 3.28125 * sin_theta_powers[2];
                    break;
                case 2:
                    P = 56.0823674036123 * cos_theta_powers[6] * sin_theta_powers[2] - 56.0823674036123 * cos_theta_powers[4] * sin_theta_powers[2] + 12.942084785449 * cos_theta_powers[2] * sin_theta_powers[2] - 0.392184387437848 * sin_theta_powers[2];
                    dP = 112.164734807225 * cos_theta_powers[7] * sin_theta_powers[1] - 336.494204421673 * cos_theta_powers[5] * sin_theta_powers[3] - 112.164734807225 * cos_theta_powers[5] * sin_theta_powers[1] + 224.329469614449 * cos_theta_powers[3] * sin_theta_powers[3] + 25.884169570898 * cos_theta_powers[3] * sin_theta_powers[1] - 25.884169570898 * cos_theta_powers[1] * sin_theta_powers[3] - 0.784368774875696 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 3:
                    P = 41.4195733281682 * cos_theta_powers[5] * sin_theta_powers[3] - 27.6130488854454 * cos_theta_powers[3] * sin_theta_powers[3] + 3.18612102524371 * cos_theta_powers[1] * sin_theta_powers[3];
                    dP = 124.258719984505 * cos_theta_powers[6] * sin_theta_powers[2] - 207.097866640841 * cos_theta_powers[4] * sin_theta_powers[4] - 82.8391466563363 * cos_theta_powers[4] * sin_theta_powers[2] + 82.8391466563363 * cos_theta_powers[2] * sin_theta_powers[4] + 9.55836307573112 * cos_theta_powers[2] * sin_theta_powers[2] - 3.18612102524371 * sin_theta_powers[4];
                    break;
                case 4:
                    P = 26.7362196178354 * cos_theta_powers[4] * sin_theta_powers[4] - 10.6944878471341 * cos_theta_powers[2] * sin_theta_powers[4] + 0.411326455659006 * sin_theta_powers[4];
                    dP = 106.944878471341 * cos_theta_powers[5] * sin_theta_powers[3] - 106.944878471341 * cos_theta_powers[3] * sin_theta_powers[5] - 42.7779513885366 * cos_theta_powers[3] * sin_theta_powers[3] + 21.3889756942683 * cos_theta_powers[1] * sin_theta_powers[5] + 1.64530582263602 * cos_theta_powers[1] * sin_theta_powers[3];
                    break;
                case 5:
                    P = 14.8305862683341 * cos_theta_powers[3] * sin_theta_powers[5] - 2.96611725366682 * cos_theta_powers[1] * sin_theta_powers[5];
                    dP = 74.1529313416705 * cos_theta_powers[4] * sin_theta_powers[4] - 44.4917588050023 * cos_theta_powers[2] * sin_theta_powers[6] - 14.8305862683341 * cos_theta_powers[2] * sin_theta_powers[4] + 2.96611725366682 * sin_theta_powers[6];
                    break;
                case 6:
                    P = 6.86522742931725 * cos_theta_powers[2] * sin_theta_powers[6] - 0.45768182862115 * sin_theta_powers[6];
                    dP = 41.1913645759035 * cos_theta_powers[3] * sin_theta_powers[5] - 13.7304548586345 * cos_theta_powers[1] * sin_theta_powers[7] - 2.7460909717269 * cos_theta_powers[1] * sin_theta_powers[5];
                    break;
                case 7:
                    P = 2.50682661696018 * cos_theta_powers[1] * sin_theta_powers[7];
                    dP = 17.5477863187212 * cos_theta_powers[2] * sin_theta_powers[6] - 2.50682661696018 * sin_theta_powers[8];
                    break;
                case 8:
                    P = 0.626706654240044 * sin_theta_powers[8];
                    dP = 5.01365323392035 * cos_theta_powers[1] * sin_theta_powers[7];
                    break;
                }
                break;
            case 9:
                switch (order)
                {
                case 0:
                    P = 94.9609375 * cos_theta_powers[9] - 201.09375 * cos_theta_powers[7] + 140.765625 * cos_theta_powers[5] - 36.09375 * cos_theta_powers[3] + 2.4609375 * cos_theta_powers[1];
                    dP = -854.6484375 * cos_theta_powers[8] * sin_theta_powers[1] + 1407.65625 * cos_theta_powers[6] * sin_theta_powers[1] - 703.828125 * cos_theta_powers[4] * sin_theta_powers[1] + 108.28125 * cos_theta_powers[2] * sin_theta_powers[1] - 2.4609375 * sin_theta_powers[1];
                    break;
                case 1:
                    P = 127.403466874265 * cos_theta_powers[8] * sin_theta_powers[1] - 209.841004263496 * cos_theta_powers[6] * sin_theta_powers[1] + 104.920502131748 * cos_theta_powers[4] * sin_theta_powers[1] - 16.1416157125766 * cos_theta_powers[2] * sin_theta_powers[1] + 0.366854902558559 * sin_theta_powers[1];
                    dP = 127.403466874265 * cos_theta_powers[9] - 1019.22773499412 * cos_theta_powers[7] * sin_theta_powers[2] - 209.841004263496 * cos_theta_powers[7] + 1259.04602558098 * cos_theta_powers[5] * sin_theta_powers[2] + 104.920502131748 * cos_theta_powers[5] - 419.682008526992 * cos_theta_powers[3] * sin_theta_powers[2] - 16.1416157125766 * cos_theta_powers[3] + 32.2832314251532 * cos_theta_powers[1] * sin_theta_powers[2] + 0.366854902558559 * cos_theta_powers[1];
                    break;
                case 2:
                    P = 108.650041615127 * cos_theta_powers[7] * sin_theta_powers[2] - 134.214757289274 * cos_theta_powers[5] * sin_theta_powers[2] + 44.738252429758 * cos_theta_powers[3] * sin_theta_powers[2] - 3.44140403305831 * cos_theta_powers[1] * sin_theta_powers[2];
                    dP = 217.300083230253 * cos_theta_powers[8] * sin_theta_powers[1] - 760.550291305887 * cos_theta_powers[6] * sin_theta_powers[3] - 268.429514578548 * cos_theta_powers[6] * sin_theta_powers[1] + 671.07378644637 * cos_theta_powers[4] * sin_theta_powers[3] + 89.4765048595161 * cos_theta_powers[4] * sin_theta_powers[1] - 134.214757289274 * cos_theta_powers[2] * sin_theta_powers[3] - 6.88280806611662 * cos_theta_powers[2] * sin_theta_powers[1] + 3.44140403305831 * sin_theta_powers[3];
                    break;
                case 3:
                    P = 82.98283999357 * cos_theta_powers[6] * sin_theta_powers[3] - 73.2201529355029 * cos_theta_powers[4] * sin_theta_powers[3] + 14.6440305871006 * cos_theta_powers[2] * sin_theta_powers[3] - 0.37548796377181 * sin_theta_powers[3];
                    dP = 248.94851998071 * cos_theta_powers[7] * sin_theta_powers[2] - 497.89703996142 * cos_theta_powers[5] * sin_theta_powers[4] - 219.660458806509 * cos_theta_powers[5] * sin_theta_powers[2] + 292.880611742012 * cos_theta_powers[3] * sin_theta_powers[4] + 43.9320917613017 * cos_theta_powers[3] * sin_theta_powers[2] - 29.2880611742012 * cos_theta_powers[1] * sin_theta_powers[4] - 1.12646389131543 * cos_theta_powers[1] * sin_theta_powers[2];
                    break;
                case 4:
                    P = 56.375738371689 * cos_theta_powers[5] * sin_theta_powers[4] - 33.16219904217 * cos_theta_powers[3] * sin_theta_powers[4] + 3.316219904217 * cos_theta_powers[1] * sin_theta_powers[4];
                    dP = 225.502953486756 * cos_theta_powers[6] * sin_theta_powers[3] - 281.878691858445 * cos_theta_powers[4] * sin_theta_powers[5] - 132.64879616868 * cos_theta_powers[4] * sin_theta_powers[3] + 99.48659712651 * cos_theta_powers[2] * sin_theta_powers[5] + 13.264879616868 * cos_theta_powers[2] * sin_theta_powers[3] - 3.316219904217 * sin_theta_powers[5];
                    break;
                case 5:
                    P = 33.6909476870967 * cos_theta_powers[4] * sin_theta_powers[5] - 11.890922713093 * cos_theta_powers[2] * sin_theta_powers[5] + 0.396364090436432 * sin_theta_powers[5];
                    dP = 168.454738435484 * cos_theta_powers[5] * sin_theta_powers[4] - 134.763790748387 * cos_theta_powers[3] * sin_theta_powers[6] - 59.4546135654648 * cos_theta_powers[3] * sin_theta_powers[4] + 23.7818454261859 * cos_theta_powers[1] * sin_theta_powers[6] + 1.98182045218216 * cos_theta_powers[1] * sin_theta_powers[4];
                    break;
                case 6:
                    P = 17.3979305746761 * cos_theta_powers[3] * sin_theta_powers[6] - 3.0702230425899 * cos_theta_powers[1] * sin_theta_powers[6];
                    dP = 104.387583448057 * cos_theta_powers[4] * sin_theta_powers[5] - 52.1937917240283 * cos_theta_powers[2] * sin_theta_powers[7] - 18.4213382555394 * cos_theta_powers[2] * sin_theta_powers[5] + 3.0702230425899 * sin_theta_powers[7];
                    break;
                case 7:
                    P = 7.53352492547375 * cos_theta_powers[2] * sin_theta_powers[7] - 0.443148525027868 * sin_theta_powers[7];
                    dP = 52.7346744783163 * cos_theta_powers[3] * sin_theta_powers[6] - 15.0670498509475 * cos_theta_powers[1] * sin_theta_powers[8] - 3.10203967519508 * cos_theta_powers[1] * sin_theta_powers[6];
                    break;
                case 8:
                    P = 2.58397773170915 * cos_theta_powers[1] * sin_theta_powers[8];
                    dP = 20.6718218536732 * cos_theta_powers[2] * sin_theta_powers[7] - 2.58397773170915 * sin_theta_powers[9];
                    break;
                case 9:
                    P = 0.609049392175524 * sin_theta_powers[9];
                    dP = 5.48144452957971 * cos_theta_powers[1] * sin_theta_powers[8];
                    break;
                }
                break;
            case 10:
                switch (order)
                {
                case 0:
                    P = 180.42578125 * cos_theta_powers[10] - 427.32421875 * cos_theta_powers[8] + 351.9140625 * cos_theta_powers[6] - 117.3046875 * cos_theta_powers[4] + 13.53515625 * cos_theta_powers[2] - 0.24609375;
                    dP = -1804.2578125 * cos_theta_powers[9] * sin_theta_powers[1] + 3418.59375 * cos_theta_powers[7] * sin_theta_powers[1] - 2111.484375 * cos_theta_powers[5] * sin_theta_powers[1] + 469.21875 * cos_theta_powers[3] * sin_theta_powers[1] - 27.0703125 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 1:
                    P = 243.286073807146 * cos_theta_powers[9] * sin_theta_powers[1] - 460.96308721354 * cos_theta_powers[7] * sin_theta_powers[1] + 284.712495043657 * cos_theta_powers[5] * sin_theta_powers[1] - 63.2694433430349 * cos_theta_powers[3] * sin_theta_powers[1] + 3.6501601928674 * cos_theta_powers[1] * sin_theta_powers[1];
                    dP = 243.286073807146 * cos_theta_powers[10] - 2189.57466426431 * cos_theta_powers[8] * sin_theta_powers[2] - 460.96308721354 * cos_theta_powers[8] + 3226.74161049478 * cos_theta_powers[6] * sin_theta_powers[2] + 284.712495043657 * cos_theta_powers[6] - 1423.56247521828 * cos_theta_powers[4] * sin_theta_powers[2] - 63.2694433430349 * cos_theta_powers[4] + 189.808330029105 * cos_theta_powers[2] * sin_theta_powers[2] + 3.6501601928674 * cos_theta_powers[2] - 3.6501601928674 * sin_theta_powers[2];
                    break;
                case 2:
                    P = 210.691920303964 * cos_theta_powers[8] * sin_theta_powers[2] - 310.493356237421 * cos_theta_powers[6] * sin_theta_powers[2] + 136.982363045921 * cos_theta_powers[4] * sin_theta_powers[2] - 18.2643150727895 * cos_theta_powers[2] * sin_theta_powers[2] + 0.351236828322875 * sin_theta_powers[2];
                    dP = 421.383840607929 * cos_theta_powers[9] * sin_theta_powers[1] - 1685.53536243171 * cos_theta_powers[7] * sin_theta_powers[3] - 620.986712474842 * cos_theta_powers[7] * sin_theta_powers[1] + 1862.96013742453 * cos_theta_powers[5] * sin_theta_powers[3] + 273.964726091842 * cos_theta_powers[5] * sin_theta_powers[1] - 547.929452183684 * cos_theta_powers[3] * sin_theta_powers[3] - 36.528630145579 * cos_theta_powers[3] * sin_theta_powers[1] + 36.528630145579 * cos_theta_powers[1] * sin_theta_powers[3] + 0.702473656645749 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 3:
                    P = 165.280340459423 * cos_theta_powers[7] * sin_theta_powers[3] - 182.678271034099 * cos_theta_powers[5] * sin_theta_powers[3] + 53.7289032453233 * cos_theta_powers[3] * sin_theta_powers[3] - 3.58192688302155 * cos_theta_powers[1] * sin_theta_powers[3];
                    dP = 495.841021378269 * cos_theta_powers[8] * sin_theta_powers[2] - 1156.96238321596 * cos_theta_powers[6] * sin_theta_powers[4] - 548.034813102298 * cos_theta_powers[6] * sin_theta_powers[2] + 913.391355170496 * cos_theta_powers[4] * sin_theta_powers[4] + 161.18670973597 * cos_theta_powers[4] * sin_theta_powers[2] - 161.18670973597 * cos_theta_powers[2] * sin_theta_powers[4] - 10.7457806490647 * cos_theta_powers[2] * sin_theta_powers[2] + 3.58192688302155 * sin_theta_powers[4];
                    break;
                case 4:
                    P = 116.870849535679 * cos_theta_powers[6] * sin_theta_powers[4] - 92.2664601597469 * cos_theta_powers[4] * sin_theta_powers[4] + 16.2823164987789 * cos_theta_powers[2] * sin_theta_powers[4] - 0.361829255528419 * sin_theta_powers[4];
                    dP = 467.483398142717 * cos_theta_powers[7] * sin_theta_powers[3] - 701.225097214076 * cos_theta_powers[5] * sin_theta_powers[5] - 369.065840638987 * cos_theta_powers[5] * sin_theta_powers[3] + 369.065840638987 * cos_theta_powers[3] * sin_theta_powers[5] + 65.1292659951154 * cos_theta_powers[3] * sin_theta_powers[3] - 32.5646329975577 * cos_theta_powers[1] * sin_theta_powers[5] - 1.44731702211368 * cos_theta_powers[1] * sin_theta_powers[3];
                    break;
                case 5:
                    P = 73.9156153223158 * cos_theta_powers[5] * sin_theta_powers[5] - 38.9029554327978 * cos_theta_powers[3] * sin_theta_powers[5] + 3.43261371465863 * cos_theta_powers[1] * sin_theta_powers[5];
                    dP = 369.578076611579 * cos_theta_powers[6] * sin_theta_powers[4] - 369.578076611579 * cos_theta_powers[4] * sin_theta_powers[6] - 194.514777163989 * cos_theta_powers[4] * sin_theta_powers[4] + 116.708866298393 * cos_theta_powers[2] * sin_theta_powers[6] + 17.1630685732931 * cos_theta_powers[2] * sin_theta_powers[4] - 3.43261371465863 * sin_theta_powers[6];
                    break;
                case 6:
                    P = 41.3200851148558 * cos_theta_powers[4] * sin_theta_powers[6] - 13.0484479310071 * cos_theta_powers[2] * sin_theta_powers[6] + 0.383777880323738 * sin_theta_powers[6];
                    dP = 247.920510689135 * cos_theta_powers[5] * sin_theta_powers[5] - 165.280340459423 * cos_theta_powers[3] * sin_theta_powers[7] - 78.2906875860425 * cos_theta_powers[3] * sin_theta_powers[5] + 26.0968958620142 * cos_theta_powers[1] * sin_theta_powers[7] + 2.30266728194243 * cos_theta_powers[1] * sin_theta_powers[5];
                    break;
                case 7:
                    P = 20.043185339772 * cos_theta_powers[3] * sin_theta_powers[7] - 3.16471347470085 * cos_theta_powers[1] * sin_theta_powers[7];
                    dP = 140.302297378404 * cos_theta_powers[4] * sin_theta_powers[6] - 60.1295560193161 * cos_theta_powers[2] * sin_theta_powers[8] - 22.1529943229059 * cos_theta_powers[2] * sin_theta_powers[6] + 3.16471347470085 * sin_theta_powers[8];
                    break;
                case 8:
                    P = 8.1825961504123 * cos_theta_powers[2] * sin_theta_powers[8] - 0.430662955284858 * sin_theta_powers[8];
                    dP = 65.4607692032984 * cos_theta_powers[3] * sin_theta_powers[7] - 16.3651923008246 * cos_theta_powers[1] * sin_theta_powers[9] - 3.44530364227886 * cos_theta_powers[1] * sin_theta_powers[7];
                    break;
                case 9:
                    P = 2.65478475211798 * cos_theta_powers[1] * sin_theta_powers[9];
                    dP = 23.8930627690618 * cos_theta_powers[2] * sin_theta_powers[8] - 2.65478475211798 * sin_theta_powers[10];
                    break;
                case 10:
                    P = 0.593627917136573 * sin_theta_powers[10];
                    dP = 5.93627917136573 * cos_theta_powers[1] * sin_theta_powers[9];
                    break;
                }
                break;
            case 11:
                switch (order)
                {
                case 0:
                    P = 344.44921875 * cos_theta_powers[11] - 902.12890625 * cos_theta_powers[9] + 854.6484375 * cos_theta_powers[7] - 351.9140625 * cos_theta_powers[5] + 58.65234375 * cos_theta_powers[3] - 2.70703125 * cos_theta_powers[1];
                    dP = -3788.94140625 * cos_theta_powers[10] * sin_theta_powers[1] + 8119.16015625 * cos_theta_powers[8] * sin_theta_powers[1] - 5982.5390625 * cos_theta_powers[6] * sin_theta_powers[1] + 1759.5703125 * cos_theta_powers[4] * sin_theta_powers[1] - 175.95703125 * cos_theta_powers[2] * sin_theta_powers[1] + 2.70703125 * sin_theta_powers[1];
                    break;
                case 1:
                    P = 466.386446928642 * cos_theta_powers[10] * sin_theta_powers[1] - 999.399529132805 * cos_theta_powers[8] * sin_theta_powers[1] + 736.399653045225 * cos_theta_powers[6] * sin_theta_powers[1] - 216.588133248595 * cos_theta_powers[4] * sin_theta_powers[1] + 21.6588133248595 * cos_theta_powers[2] * sin_theta_powers[1] - 0.333212512690147 * sin_theta_powers[1];
                    dP = 466.386446928642 * cos_theta_powers[11] - 4663.86446928642 * cos_theta_powers[9] * sin_theta_powers[2] - 999.399529132805 * cos_theta_powers[9] + 7995.19623306244 * cos_theta_powers[7] * sin_theta_powers[2] + 736.399653045225 * cos_theta_powers[7] - 4418.39791827135 * cos_theta_powers[5] * sin_theta_powers[2] - 216.588133248595 * cos_theta_powers[5] + 866.352532994382 * cos_theta_powers[3] * sin_theta_powers[2] + 21.6588133248595 * cos_theta_powers[3] - 43.3176266497191 * cos_theta_powers[1] * sin_theta_powers[2] - 0.333212512690147 * cos_theta_powers[1];
                    break;
                case 2:
                    P = 409.047973374878 * cos_theta_powers[9] * sin_theta_powers[2] - 701.225097214076 * cos_theta_powers[7] * sin_theta_powers[2] + 387.519132670937 * cos_theta_powers[5] * sin_theta_powers[2] - 75.984143660968 * cos_theta_powers[3] * sin_theta_powers[2] + 3.7992071830484 * cos_theta_powers[1] * sin_theta_powers[2];
                    dP = 818.095946749756 * cos_theta_powers[10] * sin_theta_powers[1] - 3681.4317603739 * cos_theta_powers[8] * sin_theta_powers[3] - 1402.45019442815 * cos_theta_powers[8] * sin_theta_powers[1] + 4908.57568049853 * cos_theta_powers[6] * sin_theta_powers[3] + 775.038265341874 * cos_theta_powers[6] * sin_theta_powers[1] - 1937.59566335468 * cos_theta_powers[4] * sin_theta_powers[3] - 151.968287321936 * cos_theta_powers[4] * sin_theta_powers[1] + 227.952430982904 * cos_theta_powers[2] * sin_theta_powers[3] + 7.5984143660968 * cos_theta_powers[2] * sin_theta_powers[1] - 3.7992071830484 * sin_theta_powers[3];
                    break;
                case 3:
                    P = 327.96800809779 * cos_theta_powers[8] * sin_theta_powers[3] - 437.29067746372 * cos_theta_powers[6] * sin_theta_powers[3] + 172.6147411041 * cos_theta_powers[4] * sin_theta_powers[3] - 20.3076166004824 * cos_theta_powers[2] * sin_theta_powers[3] + 0.338460276674706 * sin_theta_powers[3];
                    dP = 983.904024293371 * cos_theta_powers[9] * sin_theta_powers[2] - 2623.74406478232 * cos_theta_powers[7] * sin_theta_powers[4] - 1311.87203239116 * cos_theta_powers[7] * sin_theta_powers[2] + 2623.74406478232 * cos_theta_powers[5] * sin_theta_powers[4] + 517.844223312301 * cos_theta_powers[5] * sin_theta_powers[2] - 690.458964416401 * cos_theta_powers[3] * sin_theta_powers[4] - 60.9228498014471 * cos_theta_powers[3] * sin_theta_powers[2] + 40.6152332009648 * cos_theta_powers[1] * sin_theta_powers[4] + 1.01538083002412 * cos_theta_powers[1] * sin_theta_powers[2];
                    break;
                case 4:
                    P = 239.513968233596 * cos_theta_powers[7] * sin_theta_powers[4] - 239.513968233596 * cos_theta_powers[5] * sin_theta_powers[4] + 63.0299916404199 * cos_theta_powers[3] * sin_theta_powers[4] - 3.70764656708352 * cos_theta_powers[1] * sin_theta_powers[4];
                    dP = 958.055872934383 * cos_theta_powers[8] * sin_theta_powers[3] - 1676.59777763517 * cos_theta_powers[6] * sin_theta_powers[5] - 958.055872934383 * cos_theta_powers[6] * sin_theta_powers[3] + 1197.56984116798 * cos_theta_powers[4] * sin_theta_powers[5] + 252.11996656168 * cos_theta_powers[4] * sin_theta_powers[3] - 189.08997492126 * cos_theta_powers[2] * sin_theta_powers[5] - 14.8305862683341 * cos_theta_powers[2] * sin_theta_powers[3] + 3.70764656708352 * sin_theta_powers[5];
                    break;
                case 5:
                    P = 158.42359886808 * cos_theta_powers[6] * sin_theta_powers[5] - 113.1597134772 * cos_theta_powers[4] * sin_theta_powers[5] + 17.8673231806105 * cos_theta_powers[2] * sin_theta_powers[5] - 0.350339670208049 * sin_theta_powers[5];
                    dP = 792.117994340398 * cos_theta_powers[7] * sin_theta_powers[4] - 950.541593208478 * cos_theta_powers[5] * sin_theta_powers[6] - 565.798567385999 * cos_theta_powers[5] * sin_theta_powers[4] + 452.638853908799 * cos_theta_powers[3] * sin_theta_powers[6] + 89.3366159030524 * cos_theta_powers[3] * sin_theta_powers[4] - 35.734646361221 * cos_theta_powers[1] * sin_theta_powers[6] - 1.75169835104024 * cos_theta_powers[1] * sin_theta_powers[4];
                    break;
                case 6:
                    P = 94.1176423012508 * cos_theta_powers[5] * sin_theta_powers[6] - 44.8179249053575 * cos_theta_powers[3] * sin_theta_powers[6] + 3.53825722937033 * cos_theta_powers[1] * sin_theta_powers[6];
                    dP = 564.705853807505 * cos_theta_powers[6] * sin_theta_powers[5] - 470.588211506254 * cos_theta_powers[4] * sin_theta_powers[7] - 268.907549432145 * cos_theta_powers[4] * sin_theta_powers[5] + 134.453774716073 * cos_theta_powers[2] * sin_theta_powers[7] + 21.229543376222 * cos_theta_powers[2] * sin_theta_powers[5] - 3.53825722937033 * sin_theta_powers[7];
                    break;
                case 7:
                    P = 49.6043529461606 * cos_theta_powers[4] * sin_theta_powers[7] - 14.1726722703316 * cos_theta_powers[2] * sin_theta_powers[7] + 0.372965059745569 * sin_theta_powers[7];
                    dP = 347.230470623124 * cos_theta_powers[5] * sin_theta_powers[6] - 198.417411784643 * cos_theta_powers[3] * sin_theta_powers[8] - 99.2087058923213 * cos_theta_powers[3] * sin_theta_powers[6] + 28.3453445406632 * cos_theta_powers[1] * sin_theta_powers[8] + 2.61075541821898 * cos_theta_powers[1] * sin_theta_powers[6];
                    break;
                case 8:
                    P = 22.7600380686356 * cos_theta_powers[3] * sin_theta_powers[8] - 3.25143400980509 * cos_theta_powers[1] * sin_theta_powers[8];
                    dP = 182.080304549085 * cos_theta_powers[4] * sin_theta_powers[7] - 68.2801142059068 * cos_theta_powers[2] * sin_theta_powers[9] - 26.0114720784407 * cos_theta_powers[2] * sin_theta_powers[7] + 3.25143400980509 * sin_theta_powers[9];
                    break;
                case 9:
                    P = 8.81492483988725 * cos_theta_powers[2] * sin_theta_powers[9] - 0.419758325708917 * sin_theta_powers[9];
                    dP = 79.3343235589853 * cos_theta_powers[3] * sin_theta_powers[8] - 17.6298496797745 * cos_theta_powers[1] * sin_theta_powers[10] - 3.77782493138025 * cos_theta_powers[1] * sin_theta_powers[8];
                    break;
                case 10:
                    P = 2.72034486491732 * cos_theta_powers[1] * sin_theta_powers[10];
                    dP = 27.2034486491732 * cos_theta_powers[2] * sin_theta_powers[9] - 2.72034486491732 * sin_theta_powers[11];
                    break;
                case 11:
                    P = 0.579979473934679 * sin_theta_powers[11];
                    dP = 6.37977421328147 * cos_theta_powers[1] * sin_theta_powers[10];
                    break;
                }
                break;
            case 12:
                switch (order)
                {
                case 0:
                    P = 660.1943359375 * cos_theta_powers[12] - 1894.470703125 * cos_theta_powers[10] + 2029.7900390625 * cos_theta_powers[8] - 997.08984375 * cos_theta_powers[6] + 219.9462890625 * cos_theta_powers[4] - 17.595703125 * cos_theta_powers[2] + 0.2255859375;
                    dP = -7922.33203125 * cos_theta_powers[11] * sin_theta_powers[1] + 18944.70703125 * cos_theta_powers[9] * sin_theta_powers[1] - 16238.3203125 * cos_theta_powers[7] * sin_theta_powers[1] + 5982.5390625 * cos_theta_powers[5] * sin_theta_powers[1] - 879.78515625 * cos_theta_powers[3] * sin_theta_powers[1] + 35.19140625 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 1:
                    P = 897.027461585248 * cos_theta_powers[11] * sin_theta_powers[1] - 2145.0656690082 * cos_theta_powers[9] * sin_theta_powers[1] + 1838.62771629274 * cos_theta_powers[7] * sin_theta_powers[1] - 677.389158634169 * cos_theta_powers[5] * sin_theta_powers[1] + 99.616052740319 * cos_theta_powers[3] * sin_theta_powers[1] - 3.98464210961276 * cos_theta_powers[1] * sin_theta_powers[1];
                    dP = 897.027461585248 * cos_theta_powers[12] - 9867.30207743773 * cos_theta_powers[10] * sin_theta_powers[2] - 2145.0656690082 * cos_theta_powers[10] + 19305.5910210738 * cos_theta_powers[8] * sin_theta_powers[2] + 1838.62771629274 * cos_theta_powers[8] - 12870.3940140492 * cos_theta_powers[6] * sin_theta_powers[2] - 677.389158634169 * cos_theta_powers[6] + 3386.94579317084 * cos_theta_powers[4] * sin_theta_powers[2] + 99.616052740319 * cos_theta_powers[4] - 298.848158220957 * cos_theta_powers[2] * sin_theta_powers[2] - 3.98464210961276 * cos_theta_powers[2] + 3.98464210961276 * sin_theta_powers[2];
                    break;
                case 2:
                    P = 795.129860697466 * cos_theta_powers[10] * sin_theta_powers[2] - 1555.68885788635 * cos_theta_powers[8] * sin_theta_powers[2] + 1037.12590525756 * cos_theta_powers[6] * sin_theta_powers[2] - 272.927869804622 * cos_theta_powers[4] * sin_theta_powers[2] + 24.0818708651137 * cos_theta_powers[2] * sin_theta_powers[2] - 0.32109161153485 * sin_theta_powers[2];
                    dP = 1590.25972139493 * cos_theta_powers[11] * sin_theta_powers[1] - 7951.29860697466 * cos_theta_powers[9] * sin_theta_powers[3] - 3111.37771577269 * cos_theta_powers[9] * sin_theta_powers[1] + 12445.5108630908 * cos_theta_powers[7] * sin_theta_powers[3] + 2074.25181051513 * cos_theta_powers[7] * sin_theta_powers[1] - 6222.75543154539 * cos_theta_powers[5] * sin_theta_powers[3] - 545.855739609245 * cos_theta_powers[5] * sin_theta_powers[1] + 1091.71147921849 * cos_theta_powers[3] * sin_theta_powers[3] + 48.1637417302275 * cos_theta_powers[3] * sin_theta_powers[1] - 48.1637417302275 * cos_theta_powers[1] * sin_theta_powers[3] - 0.6421832230697 * cos_theta_powers[1] * sin_theta_powers[1];
                    break;
                case 3:
                    P = 649.22081265302 * cos_theta_powers[9] * sin_theta_powers[3] - 1016.17170676125 * cos_theta_powers[7] * sin_theta_powers[3] + 508.085853380625 * cos_theta_powers[5] * sin_theta_powers[3] - 89.1378690141447 * cos_theta_powers[3] * sin_theta_powers[3] + 3.93255304474168 * cos_theta_powers[1] * sin_theta_powers[3];
                    dP = 1947.66243795906 * cos_theta_powers[10] * sin_theta_powers[2] - 5842.98731387718 * cos_theta_powers[8] * sin_theta_powers[4] - 3048.51512028375 * cos_theta_powers[8] * sin_theta_powers[2] + 7113.20194732874 * cos_theta_powers[6] * sin_theta_powers[4] + 1524.25756014187 * cos_theta_powers[6] * sin_theta_powers[2] - 2540.42926690312 * cos_theta_powers[4] * sin_theta_powers[4] - 267.413607042434 * cos_theta_powers[4] * sin_theta_powers[2] + 267.413607042434 * cos_theta_powers[2] * sin_theta_powers[4] + 11.797659134225 * cos_theta_powers[2] * sin_theta_powers[2] - 3.93255304474168 * sin_theta_powers[4];
                    break;
                case 4:
                    P = 486.915609489765 * cos_theta_powers[8] * sin_theta_powers[4] - 592.766828944062 * cos_theta_powers[6] * sin_theta_powers[4] + 211.702438908594 * cos_theta_powers[4] * sin_theta_powers[4] - 22.2844672535362 * cos_theta_powers[2] * sin_theta_powers[4] + 0.327712753728473 * sin_theta_powers[4];
                    dP = 1947.66243795906 * cos_theta_powers[9] * sin_theta_powers[3] - 3895.32487591812 * cos_theta_powers[7] * sin_theta_powers[5] - 2371.06731577625 * cos_theta_powers[7] * sin_theta_powers[3] + 3556.60097366437 * cos_theta_powers[5] * sin_theta_powers[5] + 846.809755634374 * cos_theta_powers[5] * sin_theta_powers[3] - 846.809755634374 * cos_theta_powers[3] * sin_theta_powers[5] - 89.1378690141447 * cos_theta_powers[3] * sin_theta_powers[3] + 44.5689345070723 * cos_theta_powers[1] * sin_theta_powers[5] + 1.31085101491389 * cos_theta_powers[1] * sin_theta_powers[3];
                    break;
                case 5:
                    P = 334.021352445188 * cos_theta_powers[7] * sin_theta_powers[5] - 304.976017449955 * cos_theta_powers[5] * sin_theta_powers[5] + 72.6133374880844 * cos_theta_powers[3] * sin_theta_powers[5] - 3.82175460463602 * cos_theta_powers[1] * sin_theta_powers[5];
                    dP = 1670.10676222594 * cos_theta_powers[8] * sin_theta_powers[4] - 2338.14946711632 * cos_theta_powers[6] * sin_theta_powers[6] - 1524.88008724977 * cos_theta_powers[6] * sin_theta_powers[4] + 1524.88008724977 * cos_theta_powers[4] * sin_theta_powers[6] + 363.066687440422 * cos_theta_powers[4] * sin_theta_powers[4] - 217.840012464253 * cos_theta_powers[2] * sin_theta_powers[6] - 19.1087730231801 * cos_theta_powers[2] * sin_theta_powers[4] + 3.82175460463602 * sin_theta_powers[6];
                    break;
                case 6:
                    P = 208.29891011946 * cos_theta_powers[6] * sin_theta_powers[6] - 135.8471152953 * cos_theta_powers[4] * sin_theta_powers[6] + 19.4067307564714 * cos_theta_powers[2] * sin_theta_powers[6] - 0.34046896063985 * sin_theta_powers[6];
                    dP = 1249.79346071676 * cos_theta_powers[7] * sin_theta_powers[5] - 1249.79346071676 * cos_theta_powers[5] * sin_theta_powers[7] - 815.082691771801 * cos_theta_powers[5] * sin_theta_powers[5] + 543.3884611812 * cos_theta_powers[3] * sin_theta_powers[7] + 116.440384538829 * cos_theta_powers[3] * sin_theta_powers[5] - 38.8134615129429 * cos_theta_powers[1] * sin_theta_powers[7] - 2.0428137638391 * cos_theta_powers[1] * sin_theta_powers[5];
                    break;
                case 7:
                    P = 117.05388227149 * cos_theta_powers[5] * sin_theta_powers[7] - 50.8929922919522 * cos_theta_powers[3] * sin_theta_powers[7] + 3.63521373513945 * cos_theta_powers[1] * sin_theta_powers[7];
                    dP = 819.377175900431 * cos_theta_powers[6] * sin_theta_powers[6] - 585.269411357451 * cos_theta_powers[4] * sin_theta_powers[8] - 356.250946043666 * cos_theta_powers[4] * sin_theta_powers[6] + 152.678976875857 * cos_theta_powers[2] * sin_theta_powers[8] + 25.4464961459761 * cos_theta_powers[2] * sin_theta_powers[6] - 3.63521373513945 * sin_theta_powers[8];
                    break;
                case 8:
                    P = 58.5269411357451 * cos_theta_powers[4] * sin_theta_powers[8] - 15.2678976875857 * cos_theta_powers[2] * sin_theta_powers[8] + 0.363521373513944 * sin_theta_powers[8];
                    dP = 468.21552908596 * cos_theta_powers[5] * sin_theta_powers[7] - 234.10776454298 * cos_theta_powers[3] * sin_theta_powers[9] - 122.143181500685 * cos_theta_powers[3] * sin_theta_powers[7] + 30.5357953751713 * cos_theta_powers[1] * sin_theta_powers[9] + 2.90817098811156 * cos_theta_powers[1] * sin_theta_powers[7];
                    break;
                case 9:
                    P = 25.5432512332168 * cos_theta_powers[3] * sin_theta_powers[9] - 3.33172842172393 * cos_theta_powers[1] * sin_theta_powers[9];
                    dP = 229.889261098951 * cos_theta_powers[4] * sin_theta_powers[8] - 76.6297536996504 * cos_theta_powers[2] * sin_theta_powers[10] - 29.9855557955154 * cos_theta_powers[2] * sin_theta_powers[8] + 3.33172842172393 * sin_theta_powers[10];
                    break;
                case 10:
                    P = 9.43247063626901 * cos_theta_powers[2] * sin_theta_powers[10] - 0.410107418968218 * sin_theta_powers[10];
                    dP = 94.3247063626901 * cos_theta_powers[3] * sin_theta_powers[9] - 18.864941272538 * cos_theta_powers[1] * sin_theta_powers[11] - 4.10107418968218 * cos_theta_powers[1] * sin_theta_powers[9];
                    break;
                case 11:
                    P = 2.78148384397026 * cos_theta_powers[1] * sin_theta_powers[11];
                    dP = 30.5963222836729 * cos_theta_powers[2] * sin_theta_powers[10] - 2.78148384397026 * sin_theta_powers[12];
                    break;
                case 12:
                    P = 0.567768012126856 * sin_theta_powers[12];
                    dP = 6.81321614552228 * cos_theta_powers[1] * sin_theta_powers[11];
                    break;
                }
                break;
            case 13:
                switch (order)
                {
                case 0:
                    P = 1269.6044921875 * cos_theta_powers[13] - 3961.166015625 * cos_theta_powers[11] + 4736.1767578125 * cos_theta_powers[9] - 2706.38671875 * cos_theta_powers[7] + 747.8173828125 * cos_theta_powers[5] - 87.978515625 * cos_theta_powers[3] + 2.9326171875 * cos_theta_powers[1];
                    dP = -16504.8583984375 * cos_theta_powers[12] * sin_theta_powers[1] + 43572.826171875 * cos_theta_powers[10] * sin_theta_powers[1] - 42625.5908203125 * cos_theta_powers[8] * sin_theta_powers[1] + 18944.70703125 * cos_theta_powers[6] * sin_theta_powers[1] - 3739.0869140625 * cos_theta_powers[4] * sin_theta_powers[1] + 263.935546875 * cos_theta_powers[2] * sin_theta_powers[1] - 2.9326171875 * sin_theta_powers[1];
                    break;
                case 1:
                    P = 1730.17927913244 * cos_theta_powers[12] * sin_theta_powers[1] - 4567.67329690965 * cos_theta_powers[10] * sin_theta_powers[1] + 4468.37605132466 * cos_theta_powers[8] * sin_theta_powers[1] - 1985.94491169985 * cos_theta_powers[6] * sin_theta_powers[1] + 391.962811519707 * cos_theta_powers[4] * sin_theta_powers[1] - 27.667963166097 * cos_theta_powers[2] * sin_theta_powers[1] + 0.307421812956633 * sin_theta_powers[1];
                    dP = 1730.17927913244 * cos_theta_powers[13] - 20762.1513495893 * cos_theta_powers[11] * sin_theta_powers[2] - 4567.67329690965 * cos_theta_powers[11] + 45676.7329690965 * cos_theta_powers[9] * sin_theta_powers[2] + 4468.37605132466 * cos_theta_powers[9] - 35747.0084105973 * cos_theta_powers[7] * sin_theta_powers[2] - 1985.94491169985 * cos_theta_powers[7] + 11915.6694701991 * cos_theta_powers[5] * sin_theta_powers[2] + 391.962811519707 * cos_theta_powers[5] - 1567.85124607883 * cos_theta_powers[3] * sin_theta_powers[2] - 27.667963166097 * cos_theta_powers[3] + 55.3359263321939 * cos_theta_powers[1] * sin_theta_powers[2] + 0.307421812956633 * cos_theta_powers[1];
                    break;
                case 2:
                    P = 1547.51939256069 * cos_theta_powers[11] * sin_theta_powers[2] - 3404.54266363352 * cos_theta_powers[9] * sin_theta_powers[2] + 2664.42469327841 * cos_theta_powers[7] * sin_theta_powers[2] - 888.141564426136 * cos_theta_powers[5] * sin_theta_powers[2] + 116.860732161334 * cos_theta_powers[3] * sin_theta_powers[2] - 4.12449642922354 * cos_theta_powers[1] * sin_theta_powers[2];
                    dP = 3095.03878512138 * cos_theta_powers[12] * sin_theta_powers[1] - 17022.7133181676 * cos_theta_powers[10] * sin_theta_powers[3] - 6809.08532726704 * cos_theta_powers[10] * sin_theta_powers[1] + 30640.8839727017 * cos_theta_powers[8] * sin_theta_powers[3] + 5328.84938655681 * cos_theta_powers[8] * sin_theta_powers[1] - 18650.9728529488 * cos_theta_powers[6] * sin_theta_powers[3] - 1776.28312885227 * cos_theta_powers[6] * sin_theta_powers[1] + 4440.70782213068 * cos_theta_powers[4] * sin_theta_powers[3] + 233.721464322667 * cos_theta_powers[4] * sin_theta_powers[1] - 350.582196484001 * cos_theta_powers[2] * sin_theta_powers[3] - 8.24899285844708 * cos_theta_powers[2] * sin_theta_powers[1] + 4.12449642922354 * sin_theta_powers[3];
                    break;
                case 3:
                    P = 1283.13529523063 * cos_theta_powers[10] * sin_theta_powers[3] - 2309.64353141513 * cos_theta_powers[8] * sin_theta_powers[3] + 1405.86997564399 * cos_theta_powers[6] * sin_theta_powers[3] - 334.730946581903 * cos_theta_powers[4] * sin_theta_powers[3] + 26.4261273617292 * cos_theta_powers[2] * sin_theta_powers[3] - 0.310895616020344 * sin_theta_powers[3];
                    dP = 3849.40588569189 * cos_theta_powers[11] * sin_theta_powers[2] - 12831.3529523063 * cos_theta_powers[9] * sin_theta_powers[4] - 6928.9305942454 * cos_theta_powers[9] * sin_theta_powers[2] + 18477.1482513211 * cos_theta_powers[7] * sin_theta_powers[4] + 4217.60992693198 * cos_theta_powers[7] * sin_theta_powers[2] - 8435.21985386396 * cos_theta_powers[5] * sin_theta_powers[4] - 1004.19283974571 * cos_theta_powers[5] * sin_theta_powers[2] + 1338.92378632761 * cos_theta_powers[3] * sin_theta_powers[4] + 79.2783820851876 * cos_theta_powers[3] * sin_theta_powers[2] - 52.8522547234584 * cos_theta_powers[1] * sin_theta_powers[4] - 0.932686848061031 * cos_theta_powers[1] * sin_theta_powers[2];
                    break;
                case 4:
                    P = 984.119847396227 * cos_theta_powers[9] * sin_theta_powers[4] - 1417.13258025057 * cos_theta_powers[7] * sin_theta_powers[4] + 646.951830114389 * cos_theta_powers[5] * sin_theta_powers[4] - 102.690766684824 * cos_theta_powers[3] * sin_theta_powers[4] + 4.05358289545357 * cos_theta_powers[1] * sin_theta_powers[4];
                    dP = 3936.47938958491 * cos_theta_powers[10] * sin_theta_powers[3] - 8857.07862656604 * cos_theta_powers[8] * sin_theta_powers[5] - 5668.53032100227 * cos_theta_powers[8] * sin_theta_powers[3] + 9919.92806175397 * cos_theta_powers[6] * sin_theta_powers[5] + 2587.80732045756 * cos_theta_powers[6] * sin_theta_powers[3] - 3234.75915057195 * cos_theta_powers[4] * sin_theta_powers[5] - 410.763066739295 * cos_theta_powers[4] * sin_theta_powers[3] + 308.072300054471 * cos_theta_powers[2] * sin_theta_powers[5] + 16.2143315818143 * cos_theta_powers[2] * sin_theta_powers[3] - 4.05358289545357 * sin_theta_powers[5];
                    break;
                case 5:
                    P = 695.877817594142 * cos_theta_powers[8] * sin_theta_powers[5] - 779.383155705439 * cos_theta_powers[6] * sin_theta_powers[5] + 254.146681208295 * cos_theta_powers[4] * sin_theta_powers[5] - 24.2044458293615 * cos_theta_powers[2] * sin_theta_powers[5] + 0.318479550386335 * sin_theta_powers[5];
                    dP = 3479.38908797071 * cos_theta_powers[9] * sin_theta_powers[4] - 5567.02254075314 * cos_theta_powers[7] * sin_theta_powers[6] - 3896.9157785272 * cos_theta_powers[7] * sin_theta_powers[4] + 4676.29893423264 * cos_theta_powers[5] * sin_theta_powers[6] + 1270.73340604148 * cos_theta_powers[5] * sin_theta_powers[4] - 1016.58672483318 * cos_theta_powers[3] * sin_theta_powers[6] - 121.022229146807 * cos_theta_powers[3] * sin_theta_powers[4] + 48.408891658723 * cos_theta_powers[1] * sin_theta_powers[6] + 1.59239775193168 * cos_theta_powers[1] * sin_theta_powers[4];
                    break;
                case 6:
                    P = 451.545154013981 * cos_theta_powers[7] * sin_theta_powers[6] - 379.297929371744 * cos_theta_powers[5] * sin_theta_powers[6] + 82.4560716025531 * cos_theta_powers[3] * sin_theta_powers[6] - 3.92647960012158 * cos_theta_powers[1] * sin_theta_powers[6];
                    dP = 2709.27092408389 * cos_theta_powers[8] * sin_theta_powers[5] - 3160.81607809787 * cos_theta_powers[6] * sin_theta_powers[7] - 2275.78757623047 * cos_theta_powers[6] * sin_theta_powers[5] + 1896.48964685872 * cos_theta_powers[4] * sin_theta_powers[7] + 494.736429615319 * cos_theta_powers[4] * sin_theta_powers[5] - 247.368214807659 * cos_theta_powers[2] * sin_theta_powers[7] - 23.5588776007295 * cos_theta_powers[2] * sin_theta_powers[5] + 3.92647960012158 * sin_theta_powers[7];
                    break;
                case 7:
                    P = 267.137715681872 * cos_theta_powers[6] * sin_theta_powers[7] - 160.282629409123 * cos_theta_powers[4] * sin_theta_powers[7] + 20.9064299229291 * cos_theta_powers[2] * sin_theta_powers[7] - 0.331848094014747 * sin_theta_powers[7];
                    dP = 1869.9640097731 * cos_theta_powers[7] * sin_theta_powers[6] - 1602.82629409123 * cos_theta_powers[5] * sin_theta_powers[8] - 1121.97840586386 * cos_theta_powers[5] * sin_theta_powers[6] + 641.130517636492 * cos_theta_powers[3] * sin_theta_powers[8] + 146.345009460504 * cos_theta_powers[3] * sin_theta_powers[6] - 41.8128598458582 * cos_theta_powers[1] * sin_theta_powers[8] - 2.32293665810323 * cos_theta_powers[1] * sin_theta_powers[6];
                    break;
                case 8:
                    P = 142.79111530957 * cos_theta_powers[5] * sin_theta_powers[8] - 57.1164461238281 * cos_theta_powers[3] * sin_theta_powers[8] + 3.7249856167714 * cos_theta_powers[1] * sin_theta_powers[8];
                    dP = 1142.32892247656 * cos_theta_powers[6] * sin_theta_powers[7] - 713.955576547852 * cos_theta_powers[4] * sin_theta_powers[9] - 456.931568990625 * cos_theta_powers[4] * sin_theta_powers[7] + 171.349338371484 * cos_theta_powers[2] * sin_theta_powers[9] + 29.7998849341712 * cos_theta_powers[2] * sin_theta_powers[7] - 3.7249856167714 * sin_theta_powers[9];
                    break;
                case 9:
                    P = 68.0729932621644 * cos_theta_powers[4] * sin_theta_powers[9] - 16.3375183829195 * cos_theta_powers[2] * sin_theta_powers[9] + 0.355163443106945 * sin_theta_powers[9];
                    dP = 612.65693935948 * cos_theta_powers[5] * sin_theta_powers[8] - 272.291973048658 * cos_theta_powers[3] * sin_theta_powers[10] - 147.037665446275 * cos_theta_powers[3] * sin_theta_powers[8] + 32.6750367658389 * cos_theta_powers[1] * sin_theta_powers[10] + 3.1964709879625 * cos_theta_powers[1] * sin_theta_powers[8];
                    break;
                case 10:
                    P = 28.3884006063428 * cos_theta_powers[3] * sin_theta_powers[10] - 3.40660807276114 * cos_theta_powers[1] * sin_theta_powers[10];
                    dP = 283.884006063428 * cos_theta_powers[4] * sin_theta_powers[9] - 85.1652018190285 * cos_theta_powers[2] * sin_theta_powers[11] - 34.0660807276114 * cos_theta_powers[2] * sin_theta_powers[9] + 3.40660807276114 * sin_theta_powers[11];
                    break;
                case 11:
                    P = 10.0368152878927 * cos_theta_powers[2] * sin_theta_powers[11] - 0.401472611515706 * sin_theta_powers[11];
                    dP = 110.404968166819 * cos_theta_powers[3] * sin_theta_powers[10] - 20.0736305757853 * cos_theta_powers[1] * sin_theta_powers[12] - 4.41619872667277 * cos_theta_powers[1] * sin_theta_powers[10];
                    break;
                case 12:
                    P = 2.83884006063428 * cos_theta_powers[1] * sin_theta_powers[12];
                    dP = 34.0660807276114 * cos_theta_powers[2] * sin_theta_powers[11] - 2.83884006063428 * sin_theta_powers[13];
                    break;
                case 13:
                    P = 0.556742340967044 * sin_theta_powers[13];
                    dP = 7.23765043257157 * cos_theta_powers[1] * sin_theta_powers[12];
                    break;
                }
                break;
            }
            b_r += (degree + 1) * (g * cos_n_phi[order] + h * sin_n_phi[order]) * P / denom;
            b_theta -= (g * cos_n_phi[order] + h * sin_n_phi[order]) * dP / denom;
            b_phi += order * (g * sin_n_phi[order] - h * cos_n_phi[order]) * P / (denom * sin_theta);
        }
    }

    const Real b_abs = std::hypot(b_r, b_theta, b_phi);
    const Real b_east = b_phi;

    // alpha = lat + theta;
    const Real sin_alpha = sin_lat * cos_theta + cos_lat * sin_theta;
    const Real cos_alpha = cos_lat * cos_theta - sin_lat * sin_theta;
    const Real b_north = -sin_alpha * b_theta + cos_alpha * b_r;
    const Real b_down = -cos_alpha * b_theta - sin_alpha * b_r;

    const Real dip = asin(b_down / b_abs);

    // Outputs
    // Logic copied from IRI2007
    magbr = atan(b_down / (2 * std::hypot(b_north, b_east))) * GmatMathConstants::DEG_PER_RAD;
    sin_modip = dip / sqrt(dip * dip + cos_lat);
    modip = asin(sin_modip) * GmatMathConstants::DEG_PER_RAD;
}

// Used to calculate geomagetic latitude
const std::vector<RealArray> MagneticField::CORMAG = {
    { 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68, 163.68 },
    { 162.60, 163.12, 163.64, 164.18, 164.54, 164.90, 165.16, 165.66, 166.00, 165.86, 165.20, 164.38, 163.66, 162.94, 162.42, 162.00, 161.70, 161.70, 161.80, 162.14 },
    { 161.20, 162.18, 163.26, 164.44, 165.62, 166.60, 167.42, 167.80, 167.38, 166.82, 166.00, 164.66, 163.26, 162.16, 161.18, 160.40, 159.94, 159.80, 159.98, 160.44 },
    { 159.80, 161.14, 162.70, 164.50, 166.26, 167.90, 169.18, 169.72, 169.36, 168.24, 166.70, 164.80, 162.90, 161.18, 159.74, 158.60, 157.94, 157.80, 157.98, 158.72 },
    { 158.40, 160.10, 162.02, 164.28, 166.64, 169.00, 170.80, 171.72, 171.06, 169.46, 167.10, 164.64, 162.18, 160.02, 158.20, 156.80, 156.04, 155.80, 156.16, 157.02 },
    { 157.00, 158.96, 161.24, 163.86, 166.72, 169.80, 172.42, 173.72, 172.82, 170.34, 167.30, 164.22, 161.34, 158.74, 156.60, 155.00, 154.08, 153.90, 154.36, 155.36 },
    { 155.50, 157.72, 160.36, 163.32, 166.60, 170.20, 173.70, 175.64, 174.18, 170.80, 167.10, 163.56, 160.24, 157.36, 154.96, 153.10, 152.08, 151.92, 152.46, 153.76 },
    { 154.10, 156.52, 159.36, 162.52, 166.24, 170.30, 174.62, 177.48, 175.04, 170.82, 166.60, 162.70, 159.02, 155.88, 153.22, 151.20, 150.08, 149.92, 150.64, 152.20 },
    { 152.80, 155.32, 158.28, 161.70, 165.58, 170.00, 174.84, 178.46, 175.18, 170.38, 165.80, 161.64, 157.80, 154.38, 151.52, 149.30, 148.18, 148.02, 148.92, 150.60 },
    { 151.40, 154.08, 157.18, 160.68, 164.78, 169.40, 174.34, 177.44, 174.28, 169.44, 164.70, 160.34, 156.30, 152.78, 149.72, 147.40, 146.18, 146.04, 147.12, 149.04 },
    { 150.10, 152.88, 156.00, 159.58, 163.78, 168.50, 173.28, 175.60, 172.86, 168.14, 163.40, 158.98, 154.88, 151.10, 147.98, 145.50, 144.18, 144.14, 145.40, 147.48 },
    { 148.80, 151.68, 154.88, 158.48, 162.68, 167.40, 171.76, 173.60, 171.12, 166.68, 162.00, 157.48, 153.28, 149.50, 146.18, 143.50, 142.18, 142.24, 143.68, 145.98 },
    { 147.50, 150.54, 153.68, 157.28, 161.42, 166.10, 170.10, 171.48, 169.22, 164.98, 160.40, 155.88, 151.68, 147.80, 144.34, 141.60, 140.18, 140.26, 141.98, 144.62 },
    { 146.30, 149.34, 152.48, 155.98, 160.08, 164.60, 168.34, 169.38, 167.20, 163.18, 158.60, 154.18, 149.98, 146.02, 142.54, 139.70, 138.18, 138.46, 140.26, 143.16 },
    { 145.10, 148.14, 151.18, 154.60, 158.68, 163.10, 166.48, 167.28, 165.18, 161.32, 156.90, 152.48, 148.28, 144.32, 140.74, 137.80, 136.22, 136.48, 138.64, 141.76 },
    { 143.90, 146.98, 149.98, 153.30, 157.24, 161.40, 164.52, 165.16, 162.86, 159.42, 155.00, 150.68, 146.48, 142.52, 138.94, 135.90, 134.22, 134.68, 137.02, 140.40 },
    { 142.70, 145.84, 148.76, 151.92, 155.74, 159.70, 162.52, 162.96, 160.98, 157.42, 153.10, 148.84, 144.68, 140.82, 137.20, 134.00, 132.32, 132.80, 135.42, 139.10 },
    { 141.60, 144.74, 147.46, 150.52, 154.20, 158.00, 160.46, 160.76, 158.86, 155.36, 151.20, 146.94, 142.88, 139.02, 135.40, 132.10, 130.32, 131.00, 133.80, 137.74 },
    { 140.50, 143.58, 146.24, 149.12, 152.60, 156.20, 158.40, 158.66, 156.76, 153.36, 149.30, 145.04, 141.08, 137.30, 133.60, 130.30, 128.42, 129.12, 132.28, 136.44 },
    { 139.30, 142.48, 144.94, 147.64, 150.48, 154.30, 156.34, 156.36, 154.56, 151.26, 147.30, 143.14, 139.20, 135.50, 131.90, 128.40, 126.52, 127.32, 130.76, 135.18 },
    { 138.20, 141.28, 143.72, 146.24, 149.26, 152.40, 154.24, 154.16, 152.36, 149.16, 145.30, 141.24, 137.30, 133.70, 130.10, 126.60, 124.62, 125.54, 129.16, 133.92 },
    { 137.10, 140.18, 142.42, 144.66, 147.62, 150.50, 152.18, 151.96, 150.16, 147.10, 143.30, 139.24, 135.50, 131.90, 128.36, 124.80, 122.72, 123.74, 127.64, 132.62 },
    { 135.90, 139.02, 141.12, 143.18, 145.92, 148.60, 149.98, 149.76, 148.04, 145.00, 141.20, 137.30, 133.60, 130.10, 126.60, 123.00, 120.86, 121.96, 126.12, 131.36 },
    { 134.80, 137.88, 139.80, 141.68, 144.08, 146.60, 147.88, 147.56, 145.84, 142.90, 139.20, 135.30, 131.70, 128.28, 124.86, 121.30, 118.96, 120.18, 124.70, 130.16 },
    { 133.60, 136.72, 138.48, 140.10, 142.38, 144.60, 145.72, 145.34, 143.64, 140.80, 137.10, 133.30, 129.72, 126.48, 123.10, 119.50, 117.16, 118.48, 123.18, 128.86 },
    { 132.40, 135.42, 137.08, 138.50, 140.54, 142.60, 143.52, 143.06, 141.44, 138.70, 135.10, 131.30, 127.82, 124.58, 121.40, 117.70, 115.26, 116.70, 121.66, 127.60 },
    { 131.20, 134.22, 135.66, 136.82, 138.70, 140.60, 141.36, 140.86, 139.24, 136.50, 133.00, 129.30, 125.92, 122.78, 119.60, 116.00, 113.40, 114.92, 120.16, 126.30 },
    { 130.00, 132.92, 134.24, 135.14, 136.80, 138.60, 139.16, 138.64, 137.12, 134.40, 130.90, 127.20, 123.92, 120.96, 117.90, 114.20, 111.56, 113.12, 118.64, 124.90 },
    { 128.70, 131.56, 132.74, 133.44, 134.90, 136.50, 137.00, 136.36, 134.82, 132.30, 128.70, 125.16, 121.94, 119.06, 116.10, 112.50, 109.70, 111.42, 117.14, 123.60 },
    { 127.30, 130.16, 131.22, 131.66, 133.00, 134.50, 134.80, 134.14, 132.62, 130.14, 126.60, 123.06, 119.94, 117.16, 114.30, 110.70, 107.80, 109.64, 115.62, 122.24 },
    { 125.90, 128.76, 129.62, 129.96, 131.06, 132.40, 132.60, 131.86, 130.42, 128.00, 124.50, 120.96, 117.96, 115.26, 112.54, 108.90, 105.94, 107.86, 114.02, 120.84 },
    { 124.05, 126.79, 127.55, 127.83, 128.90, 130.21, 130.41, 129.71, 128.33, 125.96, 122.49, 118.96, 115.97, 113.26, 110.52, 106.89, 104.01, 106.00, 112.21, 119.06 },
    { 122.19, 124.82, 125.48, 125.69, 126.73, 128.03, 128.22, 127.55, 126.23, 123.92, 120.47, 116.97, 113.97, 111.26, 108.50, 104.89, 102.08, 104.14, 110.41, 117.29 },
    { 120.34, 122.85, 123.40, 123.56, 124.57, 125.84, 126.03, 125.40, 124.14, 121.88, 118.46, 114.97, 111.98, 109.26, 106.48, 102.88, 100.15, 102.28, 108.60, 115.51 },
    { 118.49, 120.88, 121.33, 121.42, 122.40, 123.65, 123.84, 123.24, 122.04, 119.83, 116.45, 112.97, 109.98, 107.26, 104.46, 100.87, 98.22, 100.42, 106.79, 113.74 },
    { 116.63, 118.91, 119.26, 119.29, 120.24, 121.47, 121.65, 121.09, 119.95, 117.79, 114.43, 110.98, 107.99, 105.26, 102.44, 98.87, 96.29, 98.56, 104.98, 111.96 },
    { 114.78, 116.94, 117.19, 117.15, 118.07, 119.28, 119.46, 118.93, 117.86, 115.75, 112.42, 108.98, 106.00, 103.26, 100.42, 96.86, 94.36, 96.70, 103.18, 110.19 },
    { 112.93, 114.97, 115.12, 115.02, 115.91, 117.09, 117.27, 116.78, 115.76, 113.71, 110.41, 106.98, 104.00, 101.26, 98.40, 94.85, 92.43, 94.84, 101.37, 108.41 },
    { 111.07, 113.00, 113.04, 112.88, 113.74, 114.91, 115.08, 114.62, 113.67, 111.67, 108.39, 104.99, 102.01, 99.26, 96.38, 92.85, 90.51, 92.97, 99.56, 106.64 },
    { 109.22, 111.03, 110.97, 110.75, 111.58, 112.72, 112.89, 112.47, 111.57, 109.63, 106.38, 102.99, 100.01, 97.26, 94.36, 90.84, 88.58, 91.11, 97.75, 104.86 },
    { 107.37, 109.06, 108.90, 108.61, 109.41, 110.53, 110.70, 110.31, 109.48, 107.59, 104.37, 100.99, 98.02, 95.26, 92.34, 88.83, 86.65, 89.25, 95.95, 103.09 },
    { 105.51, 107.09, 106.83, 106.48, 107.25, 108.35, 108.51, 108.16, 107.39, 105.55, 102.35, 99.00, 96.03, 93.26, 90.32, 86.83, 84.72, 87.39, 94.14, 101.31 },
    { 103.66, 105.12, 104.76, 104.34, 105.08, 106.16, 106.32, 106.00, 105.29, 103.50, 100.34, 97.00, 94.03, 91.26, 88.30, 84.82, 82.79, 85.53, 92.33, 99.54 },
    { 101.81, 103.15, 102.68, 102.21, 102.92, 103.97, 104.13, 103.85, 103.20, 101.46, 98.33, 95.00, 92.04, 89.26, 86.28, 82.81, 80.86, 83.67, 90.52, 97.76 },
    { 99.95, 101.18, 100.61, 100.07, 100.75, 101.79, 101.94, 101.69, 101.10, 99.42, 96.31, 93.01, 90.04, 87.26, 84.26, 80.81, 78.93, 81.81, 88.72, 95.99 },
    { 98.10, 99.21, 98.54, 97.94, 98.59, 99.60, 99.75, 99.54, 99.01, 97.38, 94.30, 91.01, 88.05, 85.26, 82.24, 78.80, 77.00, 79.95, 86.91, 94.21 },
    { 96.25, 97.24, 96.47, 95.81, 96.43, 97.41, 97.56, 97.39, 96.92, 95.34, 92.29, 89.01, 86.06, 83.26, 80.22, 76.79, 75.07, 78.09, 85.10, 92.43 },
    { 94.39, 95.27, 94.40, 93.67, 94.26, 95.23, 95.37, 95.23, 94.82, 93.30, 90.27, 87.02, 84.06, 81.26, 78.20, 74.79, 73.14, 76.23, 83.30, 90.66 },
    { 92.54, 93.30, 92.32, 91.54, 92.10, 93.04, 93.18, 93.08, 92.73, 91.26, 88.26, 85.02, 82.07, 79.26, 76.18, 72.78, 71.21, 74.37, 81.49, 88.88 },
    { 90.69, 91.33, 90.25, 89.40, 89.93, 90.85, 90.99, 90.92, 90.63, 89.21, 86.25, 83.02, 80.07, 77.26, 74.16, 70.77, 69.28, 72.51, 79.68, 87.11 },
    { 88.83, 89.36, 88.18, 87.27, 87.77, 88.67, 88.80, 88.77, 88.54, 87.17, 84.23, 81.03, 78.08, 75.26, 72.14, 68.77, 67.35, 70.65, 77.87, 85.33 },
    { 86.98, 87.39, 86.11, 85.13, 85.60, 86.48, 86.61, 86.61, 86.45, 85.13, 82.22, 79.03, 76.09, 73.26, 70.12, 66.76, 65.42, 68.79, 76.07, 83.56 },
    { 85.13, 85.42, 84.04, 83.00, 83.44, 84.29, 84.42, 84.46, 84.35, 83.09, 80.21, 77.03, 74.09, 71.26, 68.10, 64.75, 63.49, 66.93, 74.26, 81.78 },
    { 83.27, 83.45, 81.96, 80.86, 81.27, 82.11, 82.23, 82.30, 82.26, 81.05, 78.19, 75.04, 72.10, 69.26, 66.08, 62.75, 61.57, 65.06, 72.45, 80.01 },
    { 81.42, 81.48, 79.89, 78.73, 79.11, 79.92, 80.04, 80.15, 80.16, 79.01, 76.18, 73.04, 70.10, 67.26, 64.06, 60.74, 59.64, 63.20, 70.64, 78.23 },
    { 79.57, 79.51, 77.82, 76.59, 76.94, 77.73, 77.85, 77.99, 78.07, 76.97, 74.17, 71.04, 68.11, 65.26, 62.04, 58.73, 57.71, 61.34, 68.84, 76.46 },
    { 77.71, 77.54, 75.75, 74.46, 74.78, 75.55, 75.66, 75.84, 75.98, 74.93, 72.15, 69.05, 66.12, 63.26, 60.02, 56.73, 55.78, 59.48, 67.03, 74.68 },
    { 75.86, 75.57, 73.68, 72.32, 72.61, 73.36, 73.47, 73.68, 73.88, 72.88, 70.14, 67.05, 64.12, 61.26, 58.00, 54.72, 53.85, 57.62, 65.22, 72.91 },
    { 74.01, 73.60, 71.60, 70.19, 70.45, 71.17, 71.28, 71.53, 71.79, 70.84, 68.13, 65.05, 62.13, 59.26, 55.98, 52.71, 51.92, 55.76, 63.41, 71.13 },
    { 72.15, 71.63, 69.53, 68.05, 68.28, 68.99, 69.09, 69.37, 69.69, 68.80, 66.11, 63.06, 60.13, 57.26, 53.96, 50.71, 49.99, 53.90, 61.61, 69.36 },
    { 70.30, 69.66, 67.46, 65.92, 66.12, 66.80, 66.90, 67.22, 67.60, 66.76, 64.10, 61.06, 58.14, 55.26, 51.94, 48.70, 48.06, 52.04, 59.80, 67.58 },
    { 67.70, 67.06, 65.08, 63.72, 63.98, 64.60, 64.80, 65.12, 65.60, 64.86, 62.40, 59.26, 56.24, 53.18, 49.84, 46.60, 46.12, 50.12, 57.52, 64.80 },
    { 64.90, 64.42, 62.70, 61.62, 61.78, 62.40, 62.60, 63.04, 63.58, 63.00, 60.60, 57.46, 54.42, 51.18, 47.70, 44.60, 44.22, 48.02, 55.06, 61.92 },
    { 62.10, 61.72, 60.32, 59.50, 59.68, 60.20, 60.46, 60.94, 61.58, 61.00, 58.70, 55.66, 52.52, 49.18, 45.60, 42.50, 42.22, 46.00, 52.60, 58.98 },
    { 59.20, 59.18, 58.12, 57.32, 57.48, 58.00, 58.30, 58.84, 59.48, 59.04, 56.90, 53.86, 50.62, 47.10, 43.50, 40.50, 40.28, 43.98, 50.22, 56.18 },
    { 56.40, 56.64, 55.84, 55.20, 55.38, 55.80, 56.16, 56.84, 57.48, 57.04, 55.10, 52.06, 48.70, 45.10, 41.40, 38.40, 38.28, 41.88, 47.94, 53.44 },
    { 53.70, 54.14, 53.56, 53.10, 53.24, 53.70, 54.06, 54.74, 55.38, 55.14, 53.20, 50.26, 46.80, 43.10, 39.34, 36.40, 36.38, 39.96, 45.56, 50.84 },
    { 51.10, 51.70, 51.36, 51.00, 51.14, 51.50, 51.96, 52.64, 53.38, 53.08, 51.30, 48.36, 44.90, 41.02, 37.24, 34.40, 34.38, 37.86, 43.28, 48.20 },
    { 48.50, 49.26, 49.18, 48.90, 49.04, 49.40, 49.86, 50.64, 51.28, 51.08, 49.40, 46.46, 42.98, 39.02, 35.14, 32.40, 32.48, 35.72, 41.00, 45.70 },
    { 46.00, 46.96, 46.98, 46.80, 46.94, 47.30, 47.76, 48.54, 49.28, 49.08, 47.40, 44.56, 41.08, 37.02, 33.14, 30.40, 30.58, 33.84, 38.72, 43.20 },
    { 43.50, 44.62, 44.80, 44.80, 44.94, 45.20, 45.76, 46.54, 47.18, 46.98, 45.50, 42.66, 39.08, 35.02, 31.14, 28.40, 28.58, 31.82, 36.52, 40.80 },
    { 41.20, 42.32, 42.54, 42.70, 42.84, 43.20, 43.66, 44.44, 45.08, 44.98, 43.50, 40.76, 37.08, 33.04, 29.04, 26.40, 26.68, 29.82, 34.34, 38.40 },
    { 38.80, 40.12, 40.60, 40.70, 40.84, 41.10, 41.62, 42.34, 42.98, 42.88, 41.50, 38.76, 35.18, 31.04, 27.14, 24.50, 24.78, 27.70, 32.14, 36.06 },
    { 36.50, 37.88, 38.50, 38.68, 38.84, 39.10, 39.56, 40.34, 40.88, 40.82, 39.40, 36.76, 33.18, 29.12, 25.14, 22.50, 22.88, 25.90, 29.96, 33.86 },
    { 34.30, 35.68, 36.42, 36.68, 36.84, 37.10, 37.56, 38.24, 38.88, 38.72, 37.40, 34.76, 31.18, 27.12, 23.14, 20.60, 20.98, 23.90, 27.88, 31.66 },
    { 32.10, 33.58, 34.32, 34.68, 34.84, 35.10, 35.56, 36.24, 36.78, 36.62, 35.30, 32.72, 29.18, 25.14, 21.24, 18.70, 19.08, 21.90, 25.88, 29.42 },
    { 29.90, 31.48, 32.32, 32.68, 32.84, 33.10, 33.56, 34.22, 34.68, 34.42, 33.20, 30.72, 27.28, 23.22, 19.34, 16.80, 17.24, 20.00, 23.78, 27.32 },
    { 27.70, 29.38, 30.24, 30.68, 30.94, 31.20, 31.66, 32.22, 32.58, 32.32, 31.10, 28.62, 25.28, 21.32, 17.48, 15.00, 15.38, 18.18, 21.80, 25.22 },
    { 25.70, 27.28, 28.24, 28.78, 29.04, 29.30, 29.66, 30.22, 30.50, 30.22, 29.00, 26.62, 23.30, 19.42, 15.64, 13.10, 13.54, 16.28, 19.80, 23.12 },
    { 23.60, 25.24, 26.24, 26.78, 27.14, 27.40, 27.76, 28.22, 28.40, 28.12, 26.80, 24.52, 21.30, 17.52, 13.78, 11.30, 11.74, 14.48, 17.90, 21.12 },
    { 21.60, 23.24, 24.34, 24.88, 25.24, 25.50, 25.86, 26.22, 26.40, 25.98, 24.70, 22.48, 19.40, 15.72, 12.04, 9.50, 9.94, 12.58, 16.02, 19.12 },
    { 19.60, 21.24, 22.34, 22.98, 23.34, 23.70, 24.00, 24.30, 24.40, 23.88, 22.60, 20.48, 17.52, 14.00, 10.34, 7.80, 8.18, 10.88, 14.22, 17.18 },
    { 17.60, 19.34, 20.44, 21.16, 21.54, 21.90, 22.16, 22.40, 22.32, 21.78, 20.60, 18.48, 15.62, 12.20, 8.68, 6.00, 6.44, 9.18, 12.42, 15.28 },
    { 15.80, 17.44, 18.54, 19.26, 19.74, 20.10, 20.30, 20.50, 20.32, 19.72, 18.50, 16.54, 13.84, 10.68, 7.14, 4.40, 4.74, 7.58, 10.74, 13.48 },
    { 14.00, 15.54, 16.74, 17.46, 17.94, 18.30, 18.50, 18.58, 18.32, 17.72, 16.50, 14.64, 12.24, 9.18, 5.84, 2.90, 3.30, 6.16, 9.14, 11.84 },
    { 12.30, 13.78, 14.94, 15.66, 16.24, 16.50, 16.70, 16.70, 16.42, 5.78, 14.60, 12.90, 10.66, 7.86, 4.88, 1.60, 1.72, 4.96, 7.84, 10.24 },
    { 10.70, 12.14, 13.24, 13.96, 14.44, 14.80, 14.90, 14.88, 14.52, 13.92, 12.80, 11.30, 9.28, 6.94, 4.32, 1.80, 1.94, 4.34, 6.78, 8.94 },
    { 9.40, 10.58, 11.64, 12.36, 12.74, 13.10, 13.20, 13.08, 12.72, 12.12, 11.10, 9.86, 8.30, 6.50, 4.60, 3.10, 3.16, 4.50, 6.20, 7.90 },
    { 8.40, 9.42, 10.14, 10.76, 11.14, 11.40, 11.40, 11.38, 11.02, 10.46, 9.70, 8.72, 7.64, 6.46, 5.42, 4.60, 4.70, 5.34, 6.24, 7.36 },
    { 7.90, 8.46, 8.92, 9.28, 9.54, 9.70, 9.70, 9.68, 9.42, 9.06, 8.60, 8.08, 7.56, 7.02, 6.56, 6.30, 6.30, 6.52, 6.96, 7.38 },
    { 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15, 8.15 }};

//-----------------------------------------------------------------------------
// get_field(const Real sin_lat, const Real cos_lat, const Real sin_lon, const Real cos_lon, Real& magbr, Real& modip, Real& sin_modip)
//-----------------------------------------------------------------------------
/**
 * Calculate complement of geomagetic latitude (degrees)
 * Logic copied from IRI2007
 *
 * @param lat latitude (degrees)
 * @param lon sin(longitude)
 *
 * @return complement of geomagetic latitude (degrees)
 */
//-----------------------------------------------------------------------------
Real MagneticField::conver(const Real lat, const Real lon)
{
    const Real rla = lat + 90.0; // degrees from South Pole
    Real la1_r, x = modf(rla / 2, &la1_r);
    Real lo1_r, y = modf(lon / 18, &lo1_r);

    const Integer la1 = (Integer)la1_r;
    const Integer lo1 = (Integer)lo1_r;

    Integer la2 = la1 + 1;
    if (la2 > 91)
        la2 = 91;

    const Integer lo2 = (lo1 + 1) % 20;

    const Real gm1 = CORMAG[la1][lo1];
    const Real gm2 = CORMAG[la2][lo1];
    const Real gm3 = CORMAG[la1][lo2];
    const Real gm4 = CORMAG[la2][lo2];

    const Real gmla = gm1 * (1 - x) * (1 - y) + gm2 * x * (1 - y) + gm3 * (1 - x) * y + gm4 * x * y;
    return 90.0 - gmla;
}
