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

#ifndef MagneticHistory_hpp
#define MagneticHistory_hpp

#include "MagneticField.hpp"

#include "gmatdefs.hpp"

class MagneticHistory
{
public:
	MagneticHistory(std::string data_path);
	MagneticHistory(const MagneticHistory& magnetic_history);

	MagneticField* make_magnetic_field(Integer year, Integer doy);

private:
	std::map<Integer, std::vector<std::vector<std::pair<Real, Real>>>> history;
	std::vector<std::vector<std::pair<Real, Real>>> sv; // Secular variation after the last point in time stored.

	void load_data(std::string file_path, Integer year);
	void load_data_sv(std::string file_path);
};

#endif // MagneticHistory