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

#ifndef APData_hpp
#define APData_hpp

#include "gmatdefs.hpp"

class APData
{
public:
	APData(std::string data_path);
	APData(const APData& ap_data);

	Real get_rap(Integer year, Integer month, Integer day, Integer hour);

	std::string start_date();
	std::string end_date();

private:
	Integer base_julian_date;
	IntegerArray data;

	std::string start;
	std::string end;
};

#endif // APData_hpp
