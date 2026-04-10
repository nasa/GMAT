//------------------------------------------------------------------------------
//                         AP Data
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
 * Store ap solar values from a historical file and return the rap value.
 * Logic copied from the International Reference Ionosphere 2007 model
 */
//------------------------------------------------------------------------------

#include "APData.hpp"

#include "fstream"
#include "MeasurementException.hpp"
#include "DateUtil.hpp"
#include <StringUtil.hpp>
#include <cmath>

//------------------------------------------------------------------------------
// APData(std::string data_path)
//------------------------------------------------------------------------------
/**
 * Standard constructor - load data from file
 * @param data_path directory containing IonosphereData directory
 */
//------------------------------------------------------------------------------
APData::APData(std::string data_path)
{
    Integer year, month, day, julian_day;

    std::fstream fs;
    std::string line;

    std::string file_path = data_path + "/IonosphereData/ap.dat";
    try
    {
        fs.open(file_path.c_str(), std::fstream::in);
    }
    catch (...)
    {
        throw MeasurementException("Error: " + file_path + " file does not exist or cannot open.\n");
    }

    base_julian_date = -1;

    while (!fs.eof()) {
        std::getline(fs, line);
        if (!line.length())
            continue;

        year = atoi(line.substr(1, 2).c_str());
        month = atoi(line.substr(4, 2).c_str());
        day = atoi(line.substr(7, 2).c_str());

        // This will need to be updated if/when four-digit years are introduced
        if (year < 58)
            year += 2000;
        else
            year += 1900;

        julian_day = DateUtil::JulianDay(year, month, day);
        if (base_julian_date == -1)
        {
            base_julian_date = julian_day;
            start = GmatStringUtil::Trim(GmatStringUtil::ToString(year)) + "-" + GmatStringUtil::Trim(GmatStringUtil::ToString(month)) + "-" + GmatStringUtil::Trim(GmatStringUtil::ToString(day));
        }

        for (Integer start = 9; start < 33; start += 3)
        {
            // File contains values in three-hour steps
            data.push_back(atoi(line.substr(start, 3).c_str()));
            data.push_back(-1); // Placeholder
            data.push_back(-1);
        }

        end = GmatStringUtil::Trim(GmatStringUtil::ToString(year)) + "-" + GmatStringUtil::Trim(GmatStringUtil::ToString(month)) + "-" + GmatStringUtil::Trim(GmatStringUtil::ToString(day));
    }

    fs.close();

    // Interpolate
    for (Integer hour = 1; hour < data.size() - 2; hour++)
        if (hour % 3 == 1)
            data[hour] = (2 * data[hour - 1] + data[hour + 2]) / 3;
        else if (hour % 3 == 2)
            data[hour] = (data[hour - 2] + 2 * data[hour + 1]) / 3;
}

//------------------------------------------------------------------------------
// APData(const APData& ap_data)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
APData::APData(const APData& ap_data) :
    data(ap_data.data),
    base_julian_date(ap_data.base_julian_date),
    start(ap_data.start),
    end(ap_data.end)
{}

// Calculate rap
//-----------------------------------------------------------------------------
// get_rap(Integer year, Integer month, Integer day, Integer hour)
//-----------------------------------------------------------------------------
/**
 * Logic copied from IRI2007
 *
 * @param year  UTC year
 * @param month UTC month
 * @param day   UTC day of month
 * @param hour  UTC hour of day
 *
 * @return rap
 */
//-----------------------------------------------------------------------------
Real APData::get_rap(Integer year, Integer month, Integer day, Integer hour)
{
    Integer julian_date = DateUtil::JulianDay(year, month, day);
    Integer idx = 24 * (julian_date - base_julian_date) + hour;

    if (idx - 34 < 0 || idx - 1 >= data.size() - 2)
        return std::nan("");

    Real rap = 0.0;
    rap += 0.037037037 * data[idx - 34];
    rap += 0.074074074 * data[idx - 33];
    rap += 0.111111111 * data[idx - 32];
    rap += 0.148148148 * data[idx - 31];
    rap += 0.185185185 * data[idx - 30];
    rap += 0.222222222 * data[idx - 29];
    rap += 0.259259259 * data[idx - 28];
    rap += 0.296296296 * data[idx - 27];
    rap += 0.333333333 * data[idx - 26];
    rap += 0.370370370 * data[idx - 25];
    rap += 0.407407407 * data[idx - 24];
    rap += 0.444444444 * data[idx - 23];
    rap += 0.481481481 * data[idx - 22];
    rap += 0.518518519 * data[idx - 21];
    rap += 0.555555556 * data[idx - 20];
    rap += 0.592592593 * data[idx - 19];
    rap += 0.629629629 * data[idx - 18];
    rap += 0.666666667 * data[idx - 17];
    rap += 0.703703704 * data[idx - 16];
    rap += 0.740740741 * data[idx - 15];
    rap += 0.777777778 * data[idx - 14];
    rap += 0.814814815 * data[idx - 13];
    rap += 0.851851852 * data[idx - 12];
    rap += 0.888888889 * data[idx - 11];
    rap += 0.925925926 * data[idx - 10];
    rap += 0.962962963 * data[idx - 9];
    rap += data[idx - 8];
    rap += 0.666666667 * data[idx - 7];
    rap += 0.333333333 * data[idx - 6];
    // rap += 0. * data[idx - 5];
    rap += 0.333333333 * data[idx - 4];
    rap += 0.666666667 * data[idx - 3];
    rap += data[idx - 2];

    rap += .7 * data[idx - ((hour % 3 == 2) ? 2 : 1)];

    return rap;
}

std::string APData::start_date()
{
    return start;
}

std::string APData::end_date()
{
    return end;
}
