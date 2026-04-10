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

#ifndef IonosphereCoefficients_hpp
#define IonosphereCoefficients_hpp

#include "gmatdefs.hpp"

class IonosphereCoefficients
{
public:
	IonosphereCoefficients(const IonosphereCoefficients& ionosphere_coefficients);
	friend class IonosphereCoefficientsFull;

	void get_vals(Real cos_lat, Real sin_lon, Real cos_lon, Real sin_modip, Real& m3000f2, Real& fof2);

	const Real rssn;

private:
	IonosphereCoefficients(Real rssn);

	static const IntegerArray lengths_m3000f2, lengths_fof2;

	std::vector<RealArray> cos_m3000f2; //indexed by (i, j) : coeff of cos(lat)**i * cos(i*lon) * sin(modip)**j
	std::vector<RealArray> sin_m3000f2; //indexed by (i, j) : coeff of cos(lat)**i * sin(i*lon) * sin(modip)**j

	std::vector<RealArray> cos_fof2; //indexed by (i, j) : coeff of cos(lat)**i * cos(i*lon) * sin(modip)**j
	std::vector<RealArray> sin_fof2; //indexed by (i, j) : coeff of cos(lat)**i * sin(i*lon) * sin(modip)**j
};

#endif // IonosphereCoefficientsFull
