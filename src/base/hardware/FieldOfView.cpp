//------------------------------------------------------------------------------
//                           FieldOfView
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Author: Mike Stark, NASA/GSFC
// Created: 2019.04.02
//
/**
 * Implementation of the FieldOfView class
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "FieldOfView.hpp"
#include "HardwareException.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "RgbColor.hpp"
#include "MessageInterface.hpp"
#include "FieldOfViewException.hpp"

using namespace GmatMathUtil; // 

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

/// Labels used for the field of view parameters.
const std::string
FieldOfView::PARAMETER_TEXT[FieldOfViewParamCount - GmatBaseParamCount] =
{
    "Color",
   "Alpha",
};


/// Types of the parameters used by field of view.
const Gmat::ParameterType
FieldOfView::PARAMETER_TYPE[FieldOfViewParamCount - GmatBaseParamCount] =
{
   Gmat::COLOR_TYPE,
   Gmat::UNSIGNED_INT_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FieldOfView (UnsignedInt typeId, const std::string &typeStr,
//              const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 * 
 * */
//------------------------------------------------------------------------------
FieldOfView::FieldOfView (UnsignedInt typeId, const std::string &typeStr,const std::string &itsName) :
   GmatBase(typeId, typeStr, itsName),
   // maxExcursionAngle is computed by each subclass constructor
   color(0),
   alpha(255)
{
   objectTypes.push_back(Gmat::FIELD_OF_VIEW);
   objectTypeNames.push_back("FieldOfView");
   parameterCount = FieldOfViewParamCount;
   
   // maxExcursionAngle is computed by subclasses
}

//------------------------------------------------------------------------------
// FieldOfView (const FieldOfView &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 * 
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
FieldOfView::FieldOfView(const FieldOfView &copy) :
   GmatBase(copy),
   maxExcursionAngle (copy.maxExcursionAngle),
   color(copy.color),
   alpha(copy.alpha)
{
}

//------------------------------------------------------------------------------
// FieldOfView& operator=(const FieldOfView &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the FieldOfView
 * 
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
FieldOfView& FieldOfView::operator=(const FieldOfView &copy)
{
   if (&copy == this)
      return *this;
   
   GmatBase::operator=(copy);
   
   maxExcursionAngle    = copy.maxExcursionAngle;
   color = copy.color;
   alpha = copy.alpha;
   return *this;
}

//------------------------------------------------------------------------------
// ~FieldOfView()
//------------------------------------------------------------------------------
/**
 * Destructor
 * 
 */
//------------------------------------------------------------------------------
FieldOfView::~FieldOfView()
{

}
// access functions for color and opacity

// needs header
UnsignedInt FieldOfView::GetColor() const
{
   return color;
}

// needs header
UnsignedInt FieldOfView::GetAlpha() const
{
   return alpha;
}
//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string FieldOfView::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < FieldOfViewParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer FieldOfView::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < FieldOfViewParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType FieldOfView::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < FieldOfViewParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   
   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string FieldOfView::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real FieldOfView::GetRealParameter(const Integer id) const
{
   return GmatBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real FieldOfView::SetRealParameter(const Integer id, const Real value)
{
  return GmatBase::SetRealParameter(id, value);
}


// Required so derived classes work correctly:

//------------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real FieldOfView::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real FieldOfView::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const Integer id, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * Set real valued parameter data in vector containers
 *
 * @param id The parameter ID
 * @param value The new parameter vector of values
 *
 * @return The vector after the setting has occurred
 */
 //------------------------------------------------------------------------------
const Rvector& FieldOfView::SetRvectorParameter(const Integer id,
	const Rvector &value)
{
	return GmatBase::SetRvectorParameter(id, value);
}


Real FieldOfView::GetRealParameter(const Integer id, const Integer index) const
{
	return GmatBase::GetRealParameter(id, index);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Pass through method for setting a real parameter
 *
 * @param id The parameter ID
 * @param value The new parameter value
 * @param index Array index for the parameter
 *
 * @return The parameter value after the call
 */
 //------------------------------------------------------------------------------
Real FieldOfView::SetRealParameter(const Integer id, const Real value,
	const Integer index)
{
	return GmatBase::SetRealParameter(id, value, index);
}

// functions added to handle graphics colors
// needs header
std::string FieldOfView::GetStringParameter(const Integer id) const
{
   if (id == COLOR)
      return RgbColor::ToRgbString(color);
   else
      return GmatBase::GetStringParameter(id);
}

// needs header
bool FieldOfView::SetStringParameter(const Integer id,
                                     const std::string &value)
{
   try
   {
      // Convert RGB value to unsigned integer
      color = RgbColor::ToIntColor(value);
   }
   catch (BaseException &be)
   {
      // write a message here
      throw;
   }
   return true;
}

// needs header
std::string FieldOfView::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

// needs header
bool FieldOfView::SetStringParameter(const std::string &label,
                                     const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

// needs header
UnsignedInt FieldOfView::GetUnsignedIntParameter(const Integer id) const
{
   if (id == ALPHA)
      return alpha;
   else
      return GmatBase::GetUnsignedIntParameter(id);
}

// needs header
UnsignedInt FieldOfView::SetUnsignedIntParameter(const Integer id,
                                                 const UnsignedInt value)
{
   if (id == ALPHA)
   {
      if ((value >= 0) && (value <= 255))
      {
         alpha = value;
         return alpha;
      }
      else
         throw FieldOfViewException(
            "ERROR: Opacity out of range 0-255 in FieldOfView");
   }
   else
      return GmatBase::SetUnsignedIntParameter(id,value);
}

// needs header
UnsignedInt FieldOfView::GetUnsignedIntParameter(const std::string &label) const
{
   return GetUnsignedIntParameter(GetParameterID(label));
}
                                                 
// needs header
UnsignedInt FieldOfView::SetUnsignedIntParameter(const std::string &label,
                                                 const UnsignedInt value)
{
   return SetUnsignedIntParameter(GetParameterID(label), value);
}

// needs header
bool FieldOfView::IsSquareBracketAllowedInSetting(const Integer id) const
{
   if (id == COLOR)
      return true;
   else
      return GmatBase::IsSquareBracketAllowedInSetting(id);
}

/// needs header
bool FieldOfView::IsParameterCommandModeSettable(const Integer id) const
{
   if (id == COLOR)
      return true;
   else
      return GmatBase::IsParameterCommandModeSettable(id);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  bool CheckTargetMaxExcursionAngle(Real viewConeAngle)
//------------------------------------------------------------------------------
/**
 * Checks if the target lies inside the max excursion angle
 *
 * @param viewConeAngle  the view cone angle
 *
 * @return true if the point lies inside the max excursion angle; 
 *         false otherwise
 */
//------------------------------------------------------------------------------
bool FieldOfView::CheckTargetMaxExcursionAngle(Real targetConeAngle)
{
   // Check if the target lies inside the max excursion angle
   if (targetConeAngle < maxExcursionAngle)
      return true;
   else
      return false;
}
// coordinate conversion utilities

//------------------------------------------------------------------------------
// void ConeClocktoRADEC(Real coneAngle, Real clockAngle, Real &RA, Real &dec);
//------------------------------------------------------------------------------
/* converts cone and clock angles to right ascension and declination
 *
 * @param coneAngle  [in] cone angle to be converted
 * @param clockAngle [in] clock angle to be converted
 * @param RA         [out] computed right ascension
 * @param dec        [out] computed declilnation
 */
 //------------------------------------------------------------------------------
void FieldOfView::ConeClocktoRADEC(Real coneAngle, Real clockAngle,
	Real &RA, Real &dec)
{
	RA = clockAngle;
	dec = GmatMathConstants::PI / 2 - coneAngle;
}

//------------------------------------------------------------------------------
// Rvector3 RADECtoUnitVec(Real RA, Real dec)
//------------------------------------------------------------------------------
/*
 * converts right ascension and declination angles to a unit vector
 *
 * @param   right ascension (rad)
 * @param   declination (rad)
 *
 * @return  unit vector for point represented by (RA,dec)
 *
 */
 //------------------------------------------------------------------------------
Rvector3 FieldOfView::RADECtoUnitVec(Real RA, Real dec)
{
	Rvector3 u;
	Real cosDec = cos(dec);
	u[0] = cosDec * cos(RA);
	u[1] = cosDec * sin(RA);
	u[2] = sin(dec);
	return u;
}
//------------------------------------------------------------------------------
// void UnitVecToStereographic(const Rvector3 &U, Real &xCoord, Real &yCoord)
//------------------------------------------------------------------------------
/*
 * converts a 3-d unit vector to a 2-d stereographic projection
 *
 * @param u        [in] unit vector to be converted into stereographic
 *                 projection
 * @param xCoord   [out] x coordinate of resulting stereographic projection
 * @param yCoord   [out] y coordinate of resulting stereographic projection
 *
 */
 //------------------------------------------------------------------------------
void FieldOfView::UnitVecToStereographic(const Rvector3 &u,
	Real &xCoord, Real &yCoord)
{
	xCoord = u[0] / (1 + u[2]);
	yCoord = u[1] / (1 + u[2]);
}

//------------------------------------------------------------------------------
// ConeClocktoStereographic(Real coneAngle, Real clockAngle,
//                          Real &xCoord, Real &yCoord);
//------------------------------------------------------------------------------
/*
 * converts a (coneAngle, clockAngle) pair into stereographic projection
 * yielding an x and a y coordinate in that projection.
 *
 * @param coneAngle      [in] cone angle to be converted (rad)
 * @param clockAngle     [in] clock angle to be converted (rad)
 * @param xCoord         [out] x coordinate of resulting projection
 * @param yCoord         [out] y coordinate of resulting projection
 *
 */
 //------------------------------------------------------------------------------
void FieldOfView::ConeClockToStereographic(Real coneAngle, Real clockAngle,
	Real &xCoord, Real &yCoord)
{
	Real RA, dec;
	ConeClocktoRADEC(coneAngle, clockAngle, RA, dec);
	Rvector3 unitVec = RADECtoUnitVec(RA, dec);
	UnitVecToStereographic(unitVec, xCoord, yCoord);
}
//------------------------------------------------------------------------------
// coneClockArraysToStereographic(const Rvector &coneAngleVec,
//                                const Rvector &clockAngleVec,
//                                Rvector &xArray, Rvector &yArray)
/*
 * converts the cone and clock angles in their respective Rectors
 * to stereographic projection coordinates in their respective Rvectors. Each
 * pair of vectors represents a set of points that when connnected by line
 * segments enclose a region.
 *
 * @param coneAngleVec  [in] vector of cone angles (rad)
 * @param clockAngleVec [in] vector of clock angles (rad)
 * @param xArray        [out] resulting vector of x values
 *                      (stereographic projection)
 * @param yArray        [out] resulting vector of y values
 *                      (stereographic projection)
 *
 */
 //------------------------------------------------------------------------------
void FieldOfView::ConeClockArraysToStereographic(const Rvector &coneAngleVec,
	const Rvector &clockAngleVec,
	Rvector &xArray,
	Rvector &yArray)
{
	Real x, y;
	for (int i = 0; i < coneAngleVec.GetSize(); i++)
	{
		ConeClockToStereographic(coneAngleVec.GetElement(i),
			clockAngleVec.GetElement(i), x, y);
		xArray.SetElement(i, x);
		yArray.SetElement(i, y);
	}

}
//------------------------------------------------------------------------------
// RADECtoConeClock(Real coneAngle, Real clockAngle,Real &RA, Real &dec)
 //------------------------------------------------------------------------------
void FieldOfView::RADECtoConeClock(Real RA, Real dec,
	Real &coneAngle, Real &clockAngle)
{
	coneAngle = GmatMathConstants::PI / 2 -dec;
	clockAngle = RA;
}
//------------------------------------------------------------------------------
// UnitVectoRADEC(Real RA, Real dec)
 //------------------------------------------------------------------------------
void FieldOfView::UnitVecToRADEC(const Rvector3 v,Real &RA, Real &dec)
{
	if ((v[0] == 0) && (v[1] == 0))
	{
		if (v[2] > 0)
		{
			dec = GmatMathConstants::PI / 2;

		}
		else if (v[2]<0)
		{
			dec = -GmatMathConstants::PI / 2;
		}
		else
		{
			throw FieldOfViewException("*** ERROR *** Have Zero Vector");

		}
		RA = 0;
	}
	else
	{
		RA = ATan2(v[1], v[0]);
		dec = ASin(v[2]);
	}
}
