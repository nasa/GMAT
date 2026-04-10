//------------------------------------------------------------------------------
//                         Ionosphere Coefficients Full
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
 * Ionosphere coefficients for all months and ig and rz values. 
 * Logic copied from the International Reference Ionosphere 2007 model
 */
//------------------------------------------------------------------------------

#include "IonosphereCoefficientsFull.hpp"

#include "fstream"
#include "MeasurementException.hpp"
#include "GmatConstants.hpp"
#include <StringUtil.hpp>

#include <cmath>

//------------------------------------------------------------------------------
// IonosphereCoefficientsFull(std::string data_path)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 * @param data_path directory containing IonosphereData directory 
 */
//------------------------------------------------------------------------------
IonosphereCoefficientsFull::IonosphereCoefficientsFull(std::string data_path)
{
    RealArray ig, rz;

    std::fstream fs;
    std::string line, segment;
    std::string filename = data_path + "/IonosphereData/ig_rz.dat";

    try
    {
        fs.open(filename.c_str(), std::fstream::in);
    }
    catch (...)
    {
        throw MeasurementException("Error: " + filename + " file does not exist or cannot open.\n");
    }

    std::getline(fs, segment, ','); // Publication date (not used)
    std::getline(fs, segment, ',');
    std::getline(fs, segment, ',');

    std::getline(fs, segment, ','); Integer start_month = atoi(segment.c_str());
    std::getline(fs, segment, ','); Integer start_year = atoi(segment.c_str());
    std::getline(fs, segment, ','); Integer end_month = atoi(segment.c_str());
    std::getline(fs, segment, ','); Integer end_year = atoi(segment.c_str());

    start_month_idx = std::pair<Integer, Integer>(start_year, start_month);
    end_month_idx = std::pair<Integer, Integer>(end_year, end_month);

    Integer count = 12 * (end_year - start_year) + (end_month - start_month) + 3;

    while (ig.size() < count && std::getline(fs, segment, ','))
        ig.push_back(atof(segment.c_str()));

    while (rz.size() < count && std::getline(fs, segment, ','))
        rz.push_back(atof(segment.c_str()));

    std::pair<Integer, Integer> month_idx(start_year, start_month);
    month_idx.second--; // starts one month before
    if (month_idx.second == 0)
    {
        month_idx.second = 12;
        month_idx.first--;
    }

    for (Integer i = 0; i < count; i++)
    {
        Real rrr = rz[i];
        if (rrr < 0.0)
        {
            rrr = sqrt(rrr + 85.12) * 33.52 - 408.99;
            if (rrr < 0.0)
                rrr = 0.0;
        }

        rz_history[month_idx] = rrr;

        if (ig[i] > -90.0)
        {
            ig_history[month_idx] = ig[i];
        }
        else
        {
            Real zi = (1.4683266 - rrr * 0.00267690893) * rrr - 12.349154;
            if (zi > 274.0)
                zi = 274.0;

            ig_history[month_idx] = zi;
        }

        if (month_idx.second == 12)
        {
            month_idx.first++;
            month_idx.second = 1;
        }
        else
            month_idx.second++;
    }

    load_coeff_data(data_path + "/IonosphereData/ursi11.asc", data_path + "/IonosphereData/ccir11.asc", 1);
    load_coeff_data(data_path + "/IonosphereData/ursi12.asc", data_path + "/IonosphereData/ccir12.asc", 2);
    load_coeff_data(data_path + "/IonosphereData/ursi13.asc", data_path + "/IonosphereData/ccir13.asc", 3);
    load_coeff_data(data_path + "/IonosphereData/ursi14.asc", data_path + "/IonosphereData/ccir14.asc", 4);
    load_coeff_data(data_path + "/IonosphereData/ursi15.asc", data_path + "/IonosphereData/ccir15.asc", 5);
    load_coeff_data(data_path + "/IonosphereData/ursi16.asc", data_path + "/IonosphereData/ccir16.asc", 6);
    load_coeff_data(data_path + "/IonosphereData/ursi17.asc", data_path + "/IonosphereData/ccir17.asc", 7);
    load_coeff_data(data_path + "/IonosphereData/ursi18.asc", data_path + "/IonosphereData/ccir18.asc", 8);
    load_coeff_data(data_path + "/IonosphereData/ursi19.asc", data_path + "/IonosphereData/ccir19.asc", 9);
    load_coeff_data(data_path + "/IonosphereData/ursi20.asc", data_path + "/IonosphereData/ccir20.asc", 10);
    load_coeff_data(data_path + "/IonosphereData/ursi21.asc", data_path + "/IonosphereData/ccir21.asc", 11);
    load_coeff_data(data_path + "/IonosphereData/ursi22.asc", data_path + "/IonosphereData/ccir22.asc", 12);
}

//------------------------------------------------------------------------------
// IonosphereCoefficientsFull(const IonosphereCoefficientsFull& ionosphere_coefficients_full) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
IonosphereCoefficientsFull::IonosphereCoefficientsFull(const IonosphereCoefficientsFull& ionosphere_coefficients_full) :
    cos_cos_lo_m3000f2(ionosphere_coefficients_full.cos_cos_lo_m3000f2),
    cos_cos_hi_m3000f2(ionosphere_coefficients_full.cos_cos_hi_m3000f2),
    cos_sin_lo_m3000f2(ionosphere_coefficients_full.cos_sin_lo_m3000f2),
    cos_sin_hi_m3000f2(ionosphere_coefficients_full.cos_sin_hi_m3000f2),
    sin_cos_lo_m3000f2(ionosphere_coefficients_full.sin_cos_lo_m3000f2),
    sin_cos_hi_m3000f2(ionosphere_coefficients_full.sin_cos_hi_m3000f2),
    sin_sin_lo_m3000f2(ionosphere_coefficients_full.sin_sin_lo_m3000f2),
    sin_sin_hi_m3000f2(ionosphere_coefficients_full.sin_sin_hi_m3000f2),
    cos_cos_lo_fof2(ionosphere_coefficients_full.cos_cos_lo_fof2),
    cos_cos_hi_fof2(ionosphere_coefficients_full.cos_cos_hi_fof2),
    cos_sin_lo_fof2(ionosphere_coefficients_full.cos_sin_lo_fof2),
    cos_sin_hi_fof2(ionosphere_coefficients_full.cos_sin_hi_fof2),
    sin_cos_lo_fof2(ionosphere_coefficients_full.sin_cos_lo_fof2),
    sin_cos_hi_fof2(ionosphere_coefficients_full.sin_cos_hi_fof2),
    sin_sin_lo_fof2(ionosphere_coefficients_full.sin_sin_lo_fof2),
    sin_sin_hi_fof2(ionosphere_coefficients_full.sin_sin_hi_fof2),
    start_month_idx(ionosphere_coefficients_full.start_month_idx),
    end_month_idx(ionosphere_coefficients_full.end_month_idx),
    rz_history(ionosphere_coefficients_full.rz_history),
    ig_history(ionosphere_coefficients_full.ig_history)
{}

//------------------------------------------------------------------------------
// make_ionosphere_coefficients(const Integer year, const Integer month, const Integer day, const Real hours)
//------------------------------------------------------------------------------
/**
 * Build an IonosphereCoefficients object for the specifed point in time
 * Works by consolidating as many coefficients as possible
 * @param year UTC year
 * @param month UTC month
 * @param day UTC day
 * @param hours hours since UTC midnight (including fractional)
 */
//------------------------------------------------------------------------------
IonosphereCoefficients* IonosphereCoefficientsFull::make_ionosphere_coefficients(const Integer year, const Integer month, const Integer day, const Real hours)
{
    Integer mid_day = (month == 2) ? 14 : 15;
    Real ttt; // Interpolation coefficient

    std::pair<Integer, Integer> month_idx(year, month);

    if (month_idx < start_month_idx || month_idx > end_month_idx)
        return nullptr;

    std::pair<Integer, Integer> other_month_idx;
    if (day >= mid_day)
    {
        other_month_idx.first = year;
        other_month_idx.second = month + 1;
        if (other_month_idx.second == 13)
        {
            other_month_idx.first++;
            other_month_idx.second = 1;
        }

        Integer other_mid_day = (other_month_idx.second == 2) ? 14 : 15;
        ttt = (Real)(month_length(month_idx) + other_mid_day - day) / (Real)(month_length(month_idx) + other_mid_day - mid_day);
    }
    else
    {
        other_month_idx.first = year;
        other_month_idx.second = month - 1;
        if (other_month_idx.second == 0)
        {
            other_month_idx.first--;
            other_month_idx.second = 12;
        }

        Integer other_mid_day = (other_month_idx.second == 2) ? 14 : 15;
        ttt = (Real)(month_length(other_month_idx) + day - other_mid_day) / (Real)(month_length(other_month_idx) + mid_day - other_mid_day);
    }

    Real rssn = ttt * rz_history[month_idx] + (1.0 - ttt) * rz_history[other_month_idx];
    IonosphereCoefficients *ret_val = new IonosphereCoefficients(rssn);

    Real hou = GmatMathConstants::PI * (hours / 12.0 + 1);

    RealArray sin_k_hou(7);
    sin_k_hou[0] = 0.0;
    sin_k_hou[1] = sin(hou);

    RealArray cos_k_hou(7);
    cos_k_hou[0] = 1.0;
    cos_k_hou[1] = cos(hou);

    for (Integer k = 2; k < 7; k++)
    {
        Integer idx1 = k >> 1, idx2 = k - idx1;
        sin_k_hou[k] = sin_k_hou[idx1] * cos_k_hou[idx2] + cos_k_hou[idx1] * sin_k_hou[idx2];
        cos_k_hou[k] = cos_k_hou[idx1] * cos_k_hou[idx2] - sin_k_hou[idx1] * sin_k_hou[idx2];
    }

    const std::vector<std::vector<RealArray>> cos_cos_lo_m3000f2_m = cos_cos_lo_m3000f2[month_idx.second];
    const std::vector<std::vector<RealArray>> cos_cos_lo_m3000f2_m_other = cos_cos_lo_m3000f2[other_month_idx.second];
    const std::vector<std::vector<RealArray>> cos_sin_lo_m3000f2_m = cos_sin_lo_m3000f2[month_idx.second];
    const std::vector<std::vector<RealArray>> cos_sin_lo_m3000f2_m_other = cos_sin_lo_m3000f2[other_month_idx.second];

    const std::vector<std::vector<RealArray>> cos_cos_hi_m3000f2_m = cos_cos_hi_m3000f2[month_idx.second];
    const std::vector<std::vector<RealArray>> cos_cos_hi_m3000f2_m_other = cos_cos_hi_m3000f2[other_month_idx.second];
    const std::vector<std::vector<RealArray>> cos_sin_hi_m3000f2_m = cos_sin_hi_m3000f2[month_idx.second];
    const std::vector<std::vector<RealArray>> cos_sin_hi_m3000f2_m_other = cos_sin_hi_m3000f2[other_month_idx.second];

    const std::vector<std::vector<RealArray>> sin_cos_lo_m3000f2_m = sin_cos_lo_m3000f2[month_idx.second];
    const std::vector<std::vector<RealArray>> sin_cos_lo_m3000f2_m_other = sin_cos_lo_m3000f2[other_month_idx.second];
    const std::vector<std::vector<RealArray>> sin_sin_lo_m3000f2_m = sin_sin_lo_m3000f2[month_idx.second];
    const std::vector<std::vector<RealArray>> sin_sin_lo_m3000f2_m_other = sin_sin_lo_m3000f2[other_month_idx.second];

    const std::vector<std::vector<RealArray>> sin_cos_hi_m3000f2_m = sin_cos_hi_m3000f2[month_idx.second];
    const std::vector<std::vector<RealArray>> sin_cos_hi_m3000f2_m_other = sin_cos_hi_m3000f2[other_month_idx.second];
    const std::vector<std::vector<RealArray>> sin_sin_hi_m3000f2_m = sin_sin_hi_m3000f2[month_idx.second];
    const std::vector<std::vector<RealArray>> sin_sin_hi_m3000f2_m_other = sin_sin_hi_m3000f2[other_month_idx.second];

    Real rr2 = rz_history[month_idx] / 100;
    Real other_rr2 = rz_history[other_month_idx] / 100;

    for (Integer i = 0; i < IonosphereCoefficients::lengths_m3000f2.size(); i++)
    {
        ret_val->cos_m3000f2.push_back(RealArray(IonosphereCoefficients::lengths_m3000f2[i]));
        ret_val->sin_m3000f2.push_back(RealArray(IonosphereCoefficients::lengths_m3000f2[i]));
        for (Integer j = 0; j < IonosphereCoefficients::lengths_m3000f2[i]; j++)
        {
            Real cos_sum = 0.0, sin_sum = 0.0;
            for (Integer k = 0; k < 5; k++)
            {
                cos_sum += cos_k_hou[k] * ttt * (1.0 - rr2) * cos_cos_lo_m3000f2_m[i][j][k];
                cos_sum += cos_k_hou[k] * (1.0 - ttt) * (1.0 - other_rr2) * cos_cos_lo_m3000f2_m_other[i][j][k];
                cos_sum += sin_k_hou[k] * ttt * (1.0 - rr2) * cos_sin_lo_m3000f2_m[i][j][k];
                cos_sum += sin_k_hou[k] * (1.0 - ttt) * (1.0 - other_rr2) * cos_sin_lo_m3000f2_m_other[i][j][k];

                cos_sum += cos_k_hou[k] * ttt * rr2 * cos_cos_hi_m3000f2_m[i][j][k];
                cos_sum += cos_k_hou[k] * (1.0 - ttt) * other_rr2 * cos_cos_hi_m3000f2_m_other[i][j][k];
                cos_sum += sin_k_hou[k] * ttt * rr2 * cos_sin_hi_m3000f2_m[i][j][k];
                cos_sum += sin_k_hou[k] * (1.0 - ttt) * other_rr2 * cos_sin_hi_m3000f2_m_other[i][j][k];

                sin_sum += cos_k_hou[k] * ttt * (1.0 - rr2) * sin_cos_lo_m3000f2_m[i][j][k];
                sin_sum += cos_k_hou[k] * (1.0 - ttt) * (1.0 - other_rr2) * sin_cos_lo_m3000f2_m_other[i][j][k];
                sin_sum += sin_k_hou[k] * ttt * (1.0 - rr2) * sin_sin_lo_m3000f2_m[i][j][k];
                sin_sum += sin_k_hou[k] * (1.0 - ttt) * (1.0 - other_rr2) * sin_sin_lo_m3000f2_m_other[i][j][k];

                sin_sum += cos_k_hou[k] * ttt * rr2 * sin_cos_hi_m3000f2_m[i][j][k];
                sin_sum += cos_k_hou[k] * (1.0 - ttt) * other_rr2 * sin_cos_hi_m3000f2_m_other[i][j][k];
                sin_sum += sin_k_hou[k] * ttt * rr2 * sin_sin_hi_m3000f2_m[i][j][k];
                sin_sum += sin_k_hou[k] * (1.0 - ttt) * other_rr2 * sin_sin_hi_m3000f2_m_other[i][j][k];
            }
            ret_val->cos_m3000f2[i][j] = cos_sum;
            ret_val->sin_m3000f2[i][j] = sin_sum;
        }
    }

    const std::vector<std::vector<RealArray>> cos_cos_lo_fof2_m = cos_cos_lo_fof2[month_idx.second];
    const std::vector<std::vector<RealArray>> cos_cos_lo_fof2_m_other = cos_cos_lo_fof2[other_month_idx.second];
    const std::vector<std::vector<RealArray>> cos_sin_lo_fof2_m = cos_sin_lo_fof2[month_idx.second];
    const std::vector<std::vector<RealArray>> cos_sin_lo_fof2_m_other = cos_sin_lo_fof2[other_month_idx.second];

    const std::vector<std::vector<RealArray>> cos_cos_hi_fof2_m = cos_cos_hi_fof2[month_idx.second];
    const std::vector<std::vector<RealArray>> cos_cos_hi_fof2_m_other = cos_cos_hi_fof2[other_month_idx.second];
    const std::vector<std::vector<RealArray>> cos_sin_hi_fof2_m = cos_sin_hi_fof2[month_idx.second];
    const std::vector<std::vector<RealArray>> cos_sin_hi_fof2_m_other = cos_sin_hi_fof2[other_month_idx.second];

    const std::vector<std::vector<RealArray>> sin_cos_lo_fof2_m = sin_cos_lo_fof2[month_idx.second];
    const std::vector<std::vector<RealArray>> sin_cos_lo_fof2_m_other = sin_cos_lo_fof2[other_month_idx.second];
    const std::vector<std::vector<RealArray>> sin_sin_lo_fof2_m = sin_sin_lo_fof2[month_idx.second];
    const std::vector<std::vector<RealArray>> sin_sin_lo_fof2_m_other = sin_sin_lo_fof2[other_month_idx.second];

    const std::vector<std::vector<RealArray>> sin_cos_hi_fof2_m = sin_cos_hi_fof2[month_idx.second];
    const std::vector<std::vector<RealArray>> sin_cos_hi_fof2_m_other = sin_cos_hi_fof2[other_month_idx.second];
    const std::vector<std::vector<RealArray>> sin_sin_hi_fof2_m = sin_sin_hi_fof2[month_idx.second];
    const std::vector<std::vector<RealArray>> sin_sin_hi_fof2_m_other = sin_sin_hi_fof2[other_month_idx.second];

    Real ff0 = ig_history[month_idx] / 100;
    Real other_ff0 = ig_history[other_month_idx] / 100;

    for (Integer i = 0; i < IonosphereCoefficients::lengths_fof2.size(); i++)
    {
        ret_val->cos_fof2.push_back(RealArray(IonosphereCoefficients::lengths_fof2[i]));
        ret_val->sin_fof2.push_back(RealArray(IonosphereCoefficients::lengths_fof2[i]));
        for (Integer j = 0; j < IonosphereCoefficients::lengths_fof2[i]; j++)
        {
            Real cos_sum = 0.0, sin_sum = 0.0;
            for (Integer k = 0; k < 7; k++)
            {
                cos_sum += cos_k_hou[k] * ttt * (1.0 - ff0) * cos_cos_lo_fof2_m[i][j][k];
                cos_sum += cos_k_hou[k] * (1.0 - ttt) * (1.0 - other_ff0) * cos_cos_lo_fof2_m_other[i][j][k];
                cos_sum += sin_k_hou[k] * ttt * (1.0 - ff0) * cos_sin_lo_fof2_m[i][j][k];
                cos_sum += sin_k_hou[k] * (1.0 - ttt) * (1.0 - other_ff0) * cos_sin_lo_fof2_m_other[i][j][k];

                cos_sum += cos_k_hou[k] * ttt * ff0 * cos_cos_hi_fof2_m[i][j][k];
                cos_sum += cos_k_hou[k] * (1.0 - ttt) * other_ff0 * cos_cos_hi_fof2_m_other[i][j][k];
                cos_sum += sin_k_hou[k] * ttt * ff0 * cos_sin_hi_fof2_m[i][j][k];
                cos_sum += sin_k_hou[k] * (1.0 - ttt) * other_ff0 * cos_sin_hi_fof2_m_other[i][j][k];

                sin_sum += cos_k_hou[k] * ttt * (1.0 - ff0) * sin_cos_lo_fof2_m[i][j][k];
                sin_sum += cos_k_hou[k] * (1.0 - ttt) * (1.0 - other_ff0) * sin_cos_lo_fof2_m_other[i][j][k];
                sin_sum += sin_k_hou[k] * ttt * (1.0 - ff0) * sin_sin_lo_fof2_m[i][j][k];
                sin_sum += sin_k_hou[k] * (1.0 - ttt) * (1.0 - other_ff0) * sin_sin_lo_fof2_m_other[i][j][k];

                sin_sum += cos_k_hou[k] * ttt * ff0 * sin_cos_hi_fof2_m[i][j][k];
                sin_sum += cos_k_hou[k] * (1.0 - ttt) * other_ff0 * sin_cos_hi_fof2_m_other[i][j][k];
                sin_sum += sin_k_hou[k] * ttt * ff0 * sin_sin_hi_fof2_m[i][j][k];
                sin_sum += sin_k_hou[k] * (1.0 - ttt) * other_ff0 * sin_sin_hi_fof2_m_other[i][j][k];
            }
            ret_val->cos_fof2[i][j] = cos_sum;
            ret_val->sin_fof2[i][j] = sin_sum;
        }
    }

    return ret_val;
}

std::string IonosphereCoefficientsFull::month_range()
{
    return GmatStringUtil::Trim(GmatStringUtil::ToString(start_month_idx.second)) + "/" + GmatStringUtil::Trim(GmatStringUtil::ToString(start_month_idx.first)) +
        " to " + GmatStringUtil::Trim(GmatStringUtil::ToString(end_month_idx.second)) + "/" + GmatStringUtil::Trim(GmatStringUtil::ToString(end_month_idx.first));
}

//------------------------------------------------------------------------------
// load_coeff_data(std::string file_path_ursi, std::string file_path_ccir, Integer month_idx)
//------------------------------------------------------------------------------
/**
 * Load coefficients from files
 * @param file_path_ursi path of URSI file
 * @param file_path_ccir path of CCIR file
 * @param month_idx month to load (1=Jan, 2=Feb etc.)
 */
//------------------------------------------------------------------------------
void IonosphereCoefficientsFull::load_coeff_data(std::string file_path_ursi, std::string file_path_ccir, Integer month_idx)
{
    std::fstream fs;
    std::string line;
    RealArray data_ursi, data_ccir;

    try
    {
        fs.open(file_path_ursi.c_str(), std::fstream::in);
    }
    catch (...)
    {
        throw MeasurementException("ERROR: Unable to open URSI coefficient file " + file_path_ursi + ". This file is needed when Ionosphere modeling is turned on.");
    }

    while (!fs.eof())
    {
        std::getline(fs, line);
        for (Integer start = 1; start < line.length(); start += 15)
            data_ursi.push_back(atof(line.substr(start, 15).c_str()));
    }

    fs.close();
    if (data_ursi.size() != 1976)
        throw MeasurementException("Bad Length");

    try
    {
        fs.open(file_path_ccir.c_str(), std::fstream::in);
    }
    catch (...)
    {
        throw MeasurementException("ERROR: Unable to open CCIR coefficient file " + file_path_ccir + ". This file is needed when Ionosphere modeling is turned on.");
    }

    while (!fs.eof())
    {
        std::getline(fs, line);
        for (Integer start = 1; start < line.length(); start += 15)
            data_ccir.push_back(atof(line.substr(start, 15).c_str()));
    }

    fs.close();
    if (data_ccir.size() != 2858)
        throw MeasurementException("Bad Length");

    auto it_ccir = data_ccir.cbegin() + 1976; // First block (1976 values) is not used
    for (Integer i = 0; i < IonosphereCoefficients::lengths_m3000f2.size(); i++)
    {
        cos_sin_lo_m3000f2[month_idx].push_back(std::vector<RealArray>());
        cos_cos_lo_m3000f2[month_idx].push_back(std::vector<RealArray>());
        sin_sin_lo_m3000f2[month_idx].push_back(std::vector<RealArray>());
        sin_cos_lo_m3000f2[month_idx].push_back(std::vector<RealArray>());
        for (Integer j = 0; j < IonosphereCoefficients::lengths_m3000f2[i]; j++)
        {
            cos_sin_lo_m3000f2[month_idx][i].push_back(RealArray());
            cos_cos_lo_m3000f2[month_idx][i].push_back(RealArray());
            for (Integer k = 0; k < 5; k++)
            {
                if (k)
                    cos_sin_lo_m3000f2[month_idx][i][j].push_back(*(it_ccir++));
                else
                    cos_sin_lo_m3000f2[month_idx][i][j].push_back(0.0);

                cos_cos_lo_m3000f2[month_idx][i][j].push_back(*(it_ccir++));
            }

            sin_sin_lo_m3000f2[month_idx][i].push_back(RealArray());
            sin_cos_lo_m3000f2[month_idx][i].push_back(RealArray());
            for (Integer k = 0; k < 5; k++)
            {
                if (i && k)
                    sin_sin_lo_m3000f2[month_idx][i][j].push_back(*(it_ccir++));
                else
                    sin_sin_lo_m3000f2[month_idx][i][j].push_back(0.0);

                if (i)
                    sin_cos_lo_m3000f2[month_idx][i][j].push_back(*(it_ccir++));
                else
                    sin_cos_lo_m3000f2[month_idx][i][j].push_back(0.0);
            }
        }
    }

    for (Integer i = 0; i < IonosphereCoefficients::lengths_m3000f2.size(); i++)
    {
        cos_sin_hi_m3000f2[month_idx].push_back(std::vector<RealArray>());
        cos_cos_hi_m3000f2[month_idx].push_back(std::vector<RealArray>());
        sin_sin_hi_m3000f2[month_idx].push_back(std::vector<RealArray>());
        sin_cos_hi_m3000f2[month_idx].push_back(std::vector<RealArray>());
        for (Integer j = 0; j < IonosphereCoefficients::lengths_m3000f2[i]; j++)
        {
            cos_sin_hi_m3000f2[month_idx][i].push_back(RealArray());
            cos_cos_hi_m3000f2[month_idx][i].push_back(RealArray());
            for (Integer k = 0; k < 5; k++)
            {
                if (k)
                    cos_sin_hi_m3000f2[month_idx][i][j].push_back(*(it_ccir++));
                else
                    cos_sin_hi_m3000f2[month_idx][i][j].push_back(0.0);

                cos_cos_hi_m3000f2[month_idx][i][j].push_back(*(it_ccir++));
            }

            sin_sin_hi_m3000f2[month_idx][i].push_back(RealArray());
            sin_cos_hi_m3000f2[month_idx][i].push_back(RealArray());
            for (Integer k = 0; k < 5; k++)
            {
                if (i && k)
                    sin_sin_hi_m3000f2[month_idx][i][j].push_back(*(it_ccir++));
                else
                    sin_sin_hi_m3000f2[month_idx][i][j].push_back(0.0);

                if (i)
                    sin_cos_hi_m3000f2[month_idx][i][j].push_back(*(it_ccir++));
                else
                    sin_cos_hi_m3000f2[month_idx][i][j].push_back(0.0);
            }
        }
    }

    if (it_ccir != data_ccir.cend())
        throw MeasurementException("Bad Length");

    auto it_ursi = data_ursi.cbegin();
    for (Integer i = 0; i < IonosphereCoefficients::lengths_fof2.size(); i++)
    {
        cos_sin_lo_fof2[month_idx].push_back(std::vector<RealArray>());
        cos_cos_lo_fof2[month_idx].push_back(std::vector<RealArray>());
        sin_sin_lo_fof2[month_idx].push_back(std::vector<RealArray>());
        sin_cos_lo_fof2[month_idx].push_back(std::vector<RealArray>());
        for (Integer j = 0; j < IonosphereCoefficients::lengths_fof2[i]; j++)
        {
            cos_sin_lo_fof2[month_idx][i].push_back(RealArray());
            cos_cos_lo_fof2[month_idx][i].push_back(RealArray());
            for (Integer k = 0; k < 7; k++)
            {
                if (k)
                    cos_sin_lo_fof2[month_idx][i][j].push_back(*(it_ursi++));
                else
                    cos_sin_lo_fof2[month_idx][i][j].push_back(0.0);

                cos_cos_lo_fof2[month_idx][i][j].push_back(*(it_ursi++));
            }

            sin_sin_lo_fof2[month_idx][i].push_back(RealArray());
            sin_cos_lo_fof2[month_idx][i].push_back(RealArray());
            for (Integer k = 0; k < 7; k++)
            {
                if (i && k)
                    sin_sin_lo_fof2[month_idx][i][j].push_back(*(it_ursi++));
                else
                    sin_sin_lo_fof2[month_idx][i][j].push_back(0.0);

                if (i)
                    sin_cos_lo_fof2[month_idx][i][j].push_back(*(it_ursi++));
                else
                    sin_cos_lo_fof2[month_idx][i][j].push_back(0.0);
            }
        }
    }

    for (Integer i = 0; i < IonosphereCoefficients::lengths_fof2.size(); i++)
    {
        cos_sin_hi_fof2[month_idx].push_back(std::vector<RealArray>());
        cos_cos_hi_fof2[month_idx].push_back(std::vector<RealArray>());
        sin_sin_hi_fof2[month_idx].push_back(std::vector<RealArray>());
        sin_cos_hi_fof2[month_idx].push_back(std::vector<RealArray>());
        for (Integer j = 0; j < IonosphereCoefficients::lengths_fof2[i]; j++)
        {
            cos_sin_hi_fof2[month_idx][i].push_back(RealArray());
            cos_cos_hi_fof2[month_idx][i].push_back(RealArray());
            for (Integer k = 0; k < 7; k++)
            {
                if (k)
                    cos_sin_hi_fof2[month_idx][i][j].push_back(*(it_ursi++));
                else
                    cos_sin_hi_fof2[month_idx][i][j].push_back(0.0);

                cos_cos_hi_fof2[month_idx][i][j].push_back(*(it_ursi++));
            }

            sin_sin_hi_fof2[month_idx][i].push_back(RealArray());
            sin_cos_hi_fof2[month_idx][i].push_back(RealArray());
            for (Integer k = 0; k < 7; k++)
            {
                if (i && k)
                    sin_sin_hi_fof2[month_idx][i][j].push_back(*(it_ursi++));
                else
                    sin_sin_hi_fof2[month_idx][i][j].push_back(0.0);

                if (i)
                    sin_cos_hi_fof2[month_idx][i][j].push_back(*(it_ursi++));
                else
                    sin_cos_hi_fof2[month_idx][i][j].push_back(0.0);
            }
        }
    }

    if (it_ursi != data_ursi.cend())
        throw MeasurementException("Bad Length");
}

//------------------------------------------------------------------------------
// month_length(const std::pair<Integer, Integer>& month_idx)
//------------------------------------------------------------------------------
/**
 * Number of days in a month
 * @param month_idx (year, month)
 *
 * @return length of month
 */
//------------------------------------------------------------------------------
Integer IonosphereCoefficientsFull::month_length(const std::pair<Integer, Integer>& month_idx)
{
    switch(month_idx.second)
    {
    case 1:
        return 31;
    case 2:
        if (((month_idx.first % 4 == 0) && (month_idx.first % 100 != 0)) || (month_idx.first % 400 == 0))
            return 29;
        else
            return 28;
    case 3:
        return 31;
    case 4:
        return 30;
    case 5:
        return 31;
    case 6:
        return 30;
    case 7:
        return 31;
    case 8:
        return 31;
    case 9:
        return 30;
    case 10:
        return 31;
    case 11:
        return 30;
    case 12:
        return 31;
    default:
        return -1;
    }
}
