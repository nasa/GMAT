//------------------------------------------------------------------------------
//                         Magnetic History
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
 * Store the the Earth's magnetic field history
 */
//------------------------------------------------------------------------------

#include "MagneticHistory.hpp"

#include "fstream"
#include "sstream"
#include "MeasurementException.hpp"

// GCC places max in the std namespace
#ifndef min
   #define min std::min
#endif
#ifndef max
   #define max std::max
#endif


// Load data from files
//------------------------------------------------------------------------------
// MagneticHistory::MagneticHistory(std::string data_path)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 * @param data_path directory containing IonosphereData directory
 */
//------------------------------------------------------------------------------
MagneticHistory::MagneticHistory(std::string data_path)
{
    load_data(data_path + "/IonosphereData/dgrf45.dat", 1945);
    load_data(data_path + "/IonosphereData/dgrf50.dat", 1950);
    load_data(data_path + "/IonosphereData/dgrf55.dat", 1955);
    load_data(data_path + "/IonosphereData/dgrf60.dat", 1960);
    load_data(data_path + "/IonosphereData/dgrf65.dat", 1965);
    load_data(data_path + "/IonosphereData/dgrf70.dat", 1970);
    load_data(data_path + "/IonosphereData/dgrf75.dat", 1975);
    load_data(data_path + "/IonosphereData/dgrf80.dat", 1980);
    load_data(data_path + "/IonosphereData/dgrf85.dat", 1985);
    load_data(data_path + "/IonosphereData/dgrf90.dat", 1990);
    load_data(data_path + "/IonosphereData/dgrf95.dat", 1995);
    load_data(data_path + "/IonosphereData/dgrf00.dat", 2000);
    load_data(data_path + "/IonosphereData/igrf05.dat", 2005);
    load_data_sv(data_path + "/IonosphereData/igrf05s.dat");
}

//------------------------------------------------------------------------------
// MagneticHistory(const MagneticHistory& magnetic_history)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
MagneticHistory::MagneticHistory(const MagneticHistory& magnetic_history):
    history(magnetic_history.history),
    sv(magnetic_history.sv)
{}

//------------------------------------------------------------------------------
// load_data(std::string file_path, Integer year)
//------------------------------------------------------------------------------
/**
 * Load a sigle snapshot file
 * @param file_path path to file to load
 * @param year year of file
 */
//------------------------------------------------------------------------------
void MagneticHistory::load_data(std::string file_path, Integer year)
{
    std::fstream fs;
    std::string line;
    Real g, h;
    Integer _order, _degree;

    try
    {
        fs.open(file_path.c_str(), std::fstream::in);
    }
    catch (...)
    {
        throw MeasurementException("Error: " + file_path + " file does not exist or cannot open.\n");
    }

    std::getline(fs, line); // name
    std::getline(fs, line);
    Integer max_degree = atoi(line.substr(0, 3).c_str());

    auto it = history.insert({ year, std::vector<std::vector<std::pair<Real, Real>>>() });

    it.first->second.push_back(std::vector<std::pair<Real, Real>>()); // degree = 0
    it.first->second[0].push_back(std::pair<Real, Real>(0.0, 0.0));

    for (Integer degree = 1; degree <= max_degree; degree++)
    {
        it.first->second.push_back(std::vector<std::pair<Real, Real>>());

        for (Integer order = 0; order <= degree; order++)
        {
            std::stringstream ss;

            std::getline(fs, line);
            ss << line;
            ss >> _order >> _degree >> g >> h;

            // Data is listed in Gauss and stored in nanotesla (nT)
            it.first->second[degree].push_back(std::pair<Real, Real>(g / 1e5, h / 1e5));
        }
    }

    fs.close();
}

//------------------------------------------------------------------------------
// load_data_sv(std::string file_path)
//------------------------------------------------------------------------------
/**
 * Load the secular variation file that lists rates of change at the end of the timespan
 * @param file_path path to file to load
 */
//------------------------------------------------------------------------------
void MagneticHistory::load_data_sv(std::string file_path)
{
    std::fstream fs;
    std::string line;
    Real g, h;
    Integer _order, _degree;

    try
    {
        fs.open(file_path.c_str(), std::fstream::in);
    }
    catch (...)
    {
        throw MeasurementException("Error: " + file_path + " file does not exist or cannot open.\n");
    }

    std::getline(fs, line); // name
    std::getline(fs, line);
    Integer max_degree = atoi(line.substr(0, 3).c_str());

    sv.push_back(std::vector<std::pair<Real, Real>>()); // degree = 0
    sv[0].push_back(std::pair<Real, Real>(0.0, 0.0));

    for (Integer degree = 1; degree <= max_degree; degree++)
    {
        sv.push_back(std::vector<std::pair<Real, Real>>());

        for (Integer order = 0; order <= degree; order++)
        {
            std::stringstream ss;

            std::getline(fs, line);
            ss << line;
            ss >> _order >> _degree >> g >> h;

            sv[degree].push_back(std::pair<Real, Real>(g / 1e5, h / 1e5));
        }
    }

    fs.close();
}

//------------------------------------------------------------------------------
// make_magnetic_field(Integer year, Integer doy)
//------------------------------------------------------------------------------
/**
 * Calculate the magnetic field at a point in time
 * @param year year of date to calculate
 * @param doy day of year
 */
//------------------------------------------------------------------------------
MagneticField* MagneticHistory::make_magnetic_field(Integer year, Integer doy)
{
    MagneticField *ret_val = new MagneticField();

    // Find the preceeding year
    auto it_lower = history.upper_bound(year);
    it_lower--;
    if (it_lower == history.cend())
        return nullptr;

    auto it_upper = std::next(it_lower); // Find the next year

    Integer max_degree;

    if (it_upper == history.cend()) // If there is no next year, use secular variations
    {
        Integer year_lower = it_lower->first;
        const Real coeff = (year - year_lower) + doy / 365.0; // Interpolation coefficient

        max_degree = max(sv.size(), it_lower->second.size()) - 1;

        for (Integer degree = 0; degree <= max_degree; degree++)
        {
            ret_val->data.push_back(std::vector<std::pair<Real, Real>>());

            for (Integer order = 0; order <= degree; order++)
            {
                Real g, h;

                if (it_lower->second.size() > degree && it_lower->second[degree].size() > order && sv.size() > degree && sv[degree].size() > order)
                {
                    g = it_lower->second[degree][order].first + coeff * sv[degree][order].first;
                    h = it_lower->second[degree][order].second + coeff * sv[degree][order].second;
                }
                else if (it_lower->second.size() > degree && it_lower->second[degree].size() > order)
                {
                    g = it_lower->second[degree][order].first;
                    h = it_lower->second[degree][order].second;
                }
                else
                {
                    g = coeff * sv[degree][order].first;
                    h = coeff * sv[degree][order].second;
                }

                ret_val->data[degree].push_back(std::pair<Real, Real>(g, h));
            }
        }
    }
    else
    {
        Integer year_lower = it_lower->first;
        Integer year_upper = it_upper->first;

        // Interpolation coefficients
        Real coeff_lower = ((Real)(year_upper - year - doy / 365.0)) / ((Real)(year_upper - year_lower));
        Real coeff_upper = 1 - coeff_lower;

        max_degree = min(it_lower->second.size(), it_upper->second.size()) - 1;

        for (Integer degree = 0; degree <= max_degree; degree++)
        {
            ret_val->data.push_back(std::vector<std::pair<Real, Real>>());

            for (Integer order = 0; order <= degree; order++)
            {
                Real g, h;

                if (it_lower->second.size() > degree && it_lower->second[degree].size() > order && it_upper->second.size() > degree && it_upper->second[degree].size() > order)
                {
                    g = coeff_lower * it_lower->second[degree][order].first + coeff_upper * it_upper->second[degree][order].first;
                    h = coeff_lower * it_lower->second[degree][order].second + coeff_upper * it_upper->second[degree][order].second;
                }
                else if (it_lower->second.size() > degree && it_lower->second[degree].size() > order)
                {
                    g = coeff_lower * it_lower->second[degree][order].first;
                    h = coeff_lower * it_lower->second[degree][order].second;
                }
                else
                {
                    g = coeff_upper * it_upper->second[degree][order].first;
                    h = coeff_upper * it_upper->second[degree][order].second;
                }

                ret_val->data[degree].push_back(std::pair<Real, Real>(g, h));
            }
        }
    }

    ret_val->configure();
    return ret_val;
}
