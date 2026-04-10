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

#ifndef MagneticField_hpp
#define MagneticField_hpp

#include "gmatdefs.hpp"

class MagneticField
{
public:
	friend class MagneticHistory;
	MagneticField(const MagneticField& magnetic_field);
	Real geomag_lat(const Real sin_lat, const Real cos_lat, const Real sin_lon, const Real cos_lon);
	static Real conver(const Real lat, const Real lon);
	void get_field(const Real sin_lat, const Real cos_lat, const Real sin_lon, const Real cos_lon, Real& magbr, Real& modip, Real& sin_modip);
private:
	MagneticField();
	void configure();
	std::vector<std::vector<std::pair<Real, Real>>> data;

	Real ctcl, ctsl, st0, cl0, sl0, stcl, stsl, ct0;

	static const Real EARTH_RADIUS;
	static const Real EREQU, ERPOL, AQUAD, BQUAD;
	static const std::vector<RealArray> CORMAG;
};

#endif // MagneticField_hpp