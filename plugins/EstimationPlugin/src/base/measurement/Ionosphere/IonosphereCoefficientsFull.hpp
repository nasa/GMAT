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

#ifndef IonosphereCoefficientsFull_hpp
#define IonosphereCoefficientsFull_hpp

#include "IonosphereCoefficients.hpp"

#include "string"

class IonosphereCoefficientsFull
{
public:
	IonosphereCoefficientsFull(std::string data_path);
	IonosphereCoefficientsFull(const IonosphereCoefficientsFull& ionosphere_coefficients_full);
	IonosphereCoefficients* make_ionosphere_coefficients(const Integer year, const Integer month, const Integer day, const Real hours);

	std::string month_range();

private:
	std::map <Integer, std::vector < std::vector < RealArray>>> cos_cos_lo_m3000f2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * cos(i*lon) * sin(modip)**j * cos(k * hou) (for low values of arig)
	std::map <Integer, std::vector < std::vector < RealArray>>> cos_sin_lo_m3000f2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * cos(i*lon) * sin(modip)**j * sin(k * hou) (for low values of arig)
	std::map <Integer, std::vector < std::vector < RealArray>>> sin_cos_lo_m3000f2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * sin(i*lon) * sin(modip)**j * cos(k * hou) (for low values of arig)
	std::map <Integer, std::vector < std::vector < RealArray>>> sin_sin_lo_m3000f2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * sin(i*lon) * sin(modip)**j * sin(k * hou) (for low values of arig)

	std::map <Integer, std::vector < std::vector < RealArray>>> cos_cos_hi_m3000f2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * cos(i*lon) * sin(modip)**j * cos(k * hou) (for high values of arig)
	std::map <Integer, std::vector < std::vector < RealArray>>> cos_sin_hi_m3000f2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * cos(i*lon) * sin(modip)**j * sin(k * hou) (for high values of arig)
	std::map <Integer, std::vector < std::vector < RealArray>>> sin_cos_hi_m3000f2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * sin(i*lon) * sin(modip)**j * cos(k * hou) (for high values of arig)
	std::map <Integer, std::vector < std::vector < RealArray>>> sin_sin_hi_m3000f2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * sin(i*lon) * sin(modip)**j * sin(k * hou) (for high values of arig)

	std::map <Integer, std::vector < std::vector < RealArray>>> cos_cos_lo_fof2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * cos(i*lon) * sin(modip)**j * cos(k * hou) (for low values of rzar)
	std::map <Integer, std::vector < std::vector < RealArray>>> cos_sin_lo_fof2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * cos(i*lon) * sin(modip)**j * sin(k * hou) (for low values of rzar)
	std::map <Integer, std::vector < std::vector < RealArray>>> sin_cos_lo_fof2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * sin(i*lon) * sin(modip)**j * cos(k * hou) (for low values of rzar)
	std::map <Integer, std::vector < std::vector < RealArray>>> sin_sin_lo_fof2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * sin(i*lon) * sin(modip)**j * sin(k * hou) (for low values of rzar)

	std::map <Integer, std::vector < std::vector < RealArray>>> cos_cos_hi_fof2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * cos(i*lon) * sin(modip)**j * cos(k * hou) (for high values of rzar)
	std::map <Integer, std::vector < std::vector < RealArray>>> cos_sin_hi_fof2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * cos(i*lon) * sin(modip)**j * sin(k * hou) (for high values of rzar)
	std::map <Integer, std::vector < std::vector < RealArray>>> sin_cos_hi_fof2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * sin(i*lon) * sin(modip)**j * cos(k * hou) (for high values of rzar)
	std::map <Integer, std::vector < std::vector < RealArray>>> sin_sin_hi_fof2; //indexed by (month, i, j, k) : coeff of cos(lat)**i * sin(i*lon) * sin(modip)**j * sin(k * hou) (for high values of rzar)

	std::pair<Integer, Integer> start_month_idx, end_month_idx; // Listed start and end of data
	std::map<std::pair<Integer, Integer>, Real> ig_history, rz_history;

	void load_coeff_data(std::string file_path_ursi, std::string file_path_ccir, Integer month_idx);
	static Integer month_length(const std::pair<Integer, Integer>& month_idx);
};

#endif // IonosphereCoefficientsFull_hpp
