//------------------------------------------------------------------------------
//                         Ionosphere Coefficients
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
// Created: 2010/06/21
//
/**
 * Ionosphere coefficients at a moment in time
 * Logic copied from the International Reference Ionosphere 2007 model
 */
//------------------------------------------------------------------------------

#include "IonosphereCoefficients.hpp"

// Numbers of terms to include in sumations
const IntegerArray IonosphereCoefficients::lengths_m3000f2 = { 7, 8, 6, 3, 2, 1, 1 };
const IntegerArray IonosphereCoefficients::lengths_fof2 = { 12, 12, 9, 5, 2, 1, 1, 1, 1 };

//------------------------------------------------------------------------------
// IonosphereCoefficients(Real rssn)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 * @param rssn
 */
//------------------------------------------------------------------------------
IonosphereCoefficients::IonosphereCoefficients(Real rssn):
    rssn(rssn)
{}

//------------------------------------------------------------------------------
// IonosphereCoefficients(const IonosphereCoefficients& ionosphere_coefficients)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
IonosphereCoefficients::IonosphereCoefficients(const IonosphereCoefficients& ionosphere_coefficients) :
    rssn(ionosphere_coefficients.rssn),
    cos_m3000f2(ionosphere_coefficients.cos_m3000f2),
    sin_m3000f2(ionosphere_coefficients.sin_m3000f2),
    cos_fof2(ionosphere_coefficients.cos_fof2),
    sin_fof2(ionosphere_coefficients.sin_fof2)
{}

//------------------------------------------------------------------------------
// get_vals(Real cos_lat, Real sin_lon, Real cos_lon, Real sin_modip, Real& m3000f2, Real& fof2)
//------------------------------------------------------------------------------
/**
 * Calculate m3000f2 and fof2
 * Locgic coped from IRI2007
 * @param cos_lat cos(latitude)
 * @param sin_lon sin(londitude)
 * @param cos_lon cos(longitude)
 * @param sin_modip sin(modified dipole)
 * @param m3000f2
 * @param fof2
 */
//------------------------------------------------------------------------------
void IonosphereCoefficients::get_vals(Real cos_lat, Real sin_lon, Real cos_lon, Real sin_modip, Real& m3000f2, Real& fof2)
{
    RealArray cos_lat_powers(9);
    cos_lat_powers[0] = 1.0;
    cos_lat_powers[1] = cos_lat;

    RealArray sin_i_lon(9);
    sin_i_lon[0] = 0.0;
    sin_i_lon[1] = sin_lon;

    RealArray cos_i_lon(9);
    cos_i_lon[0] = 1.0;
    cos_i_lon[1] = cos_lon;

    for (Integer i = 2; i < 9; i++)
    {
        cos_lat_powers[i] = cos_lat * cos_lat_powers[i - 1];

        const Integer idx1 = i >> 1, idx2 = i - idx1;
        sin_i_lon[i] = sin_i_lon[idx1] * cos_i_lon[idx2] + cos_i_lon[idx1] * sin_i_lon[idx2];
        cos_i_lon[i] = cos_i_lon[idx1] * cos_i_lon[idx2] - sin_i_lon[idx1] * sin_i_lon[idx2];
    }

    RealArray sin_modip_powers(12);
    sin_modip_powers[0] = 1.0;
    for (Integer j = 1; j < 12; j++)
        sin_modip_powers[j] = sin_modip * sin_modip_powers[j - 1];

    m3000f2 = 0.0;
    for (Integer i = 0; i < IonosphereCoefficients::lengths_m3000f2.size(); i++)
        for (Integer j = 0; j < IonosphereCoefficients::lengths_m3000f2[i]; j++)
            m3000f2 += (sin_m3000f2[i][j] * sin_i_lon[i] + cos_m3000f2[i][j] * cos_i_lon[i]) * cos_lat_powers[i] * sin_modip_powers[j];

    fof2 = 0.0;
    for (Integer i = 0; i < IonosphereCoefficients::lengths_fof2.size(); i++)
        for (Integer j = 0; j < IonosphereCoefficients::lengths_fof2[i]; j++)
            fof2 += (sin_fof2[i][j] * sin_i_lon[i] + cos_fof2[i][j] * cos_i_lon[i]) * cos_lat_powers[i] * sin_modip_powers[j];
}
