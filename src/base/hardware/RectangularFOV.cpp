//------------------------------------------------------------------------------
//                           RectangularFOV
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
// Author: Syeda Kazmi, NASA/GSFC
// Created: 2019.04.03
//
/**
 * Implementation of the Rectangular FOV class
 */
 //------------------------------------------------------------------------------
#include "RectangularFOV.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "FieldOfViewException.hpp"
#include "StringUtil.hpp"



using namespace GmatMathUtil; // for trig functions, and
                              // temporarily square root fn

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None
//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

/// Labels used for the rectangular field of view parameters.
const std::string
RectangularFOV::PARAMETER_TEXT[RectangleFOVParamCount - FieldOfViewParamCount] =
{
   "AngleWidth",
   "AngleHeight"
};


/// Types of the parameters used by field of view.
const Gmat::ParameterType
RectangularFOV::PARAMETER_TYPE[RectangleFOVParamCount - FieldOfViewParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
};
//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// RectangularFOV(Real angleWidthIn, Real angleHeightIn)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 */
//------------------------------------------------------------------------------
RectangularFOV::RectangularFOV(const std::string &itsName) :
   FieldOfView(Gmat::RECTANGULAR_FOV, "RectangularFOV", itsName),
   angleWidth (30.0 * GmatMathConstants::RAD_PER_DEG),
   angleHeight(10.0 * GmatMathConstants::RAD_PER_DEG)

{
   objectTypes.push_back(Gmat::RECTANGULAR_FOV);
   objectTypeNames.push_back("RectangularFOV");

   parameterCount = RectangleFOVParamCount;
	   // length great circle from origin (0,0) to (angleHeight,angleWidth)
	   // angular equivalent of length of hypotenuse of triangle for computing
	   // length of a rectangle's diagonal from origin to (height, width)
   maxExcursionAngle = ACos( Cos(angleHeight)*Cos(angleWidth) );
}

//------------------------------------------------------------------------------
// RectangularFOV(const RectangularFOV &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
RectangularFOV::RectangularFOV(const RectangularFOV &copy) :
   FieldOfView(copy),
   angleWidth (copy.angleWidth),
   angleHeight(copy.angleHeight)
{
}

//------------------------------------------------------------------------------
// RectangularFOV& operator=(const RectangularFOV &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the RectangularFOV
 *
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
RectangularFOV& RectangularFOV::operator=(const RectangularFOV &copy)
{
   if (&copy != this)
   {
      FieldOfView::operator=(copy);
      angleHeight = copy.angleHeight;
      angleWidth  = copy.angleWidth;
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~RectangularFieldOfView()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 */
//------------------------------------------------------------------------------
RectangularFOV::~RectangularFOV()
{
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the object.
 *
 * @return clone of the object.
 */
//------------------------------------------------------------------------------
GmatBase* RectangularFOV::Clone() const
{
	return new RectangularFOV(*this);
}

//------------------------------------------------------------------------------
//  bool CheckTargetVisibility(const Rvector3 &target)
//------------------------------------------------------------------------------
/**
 * Determines whether or not the point is in the FOV
 *
 * @param target   target vector
 *
 * @return true if the point is in the FOV ; false otherwise
 */
//------------------------------------------------------------------------------
bool     RectangularFOV::CheckTargetVisibility(const Rvector3 &target)
{
	Real RA, dec;
	Real cone, clock;
	UnitVecToRADEC(target, RA, dec);
	RADECtoConeClock(RA, dec, cone, clock);
	return CheckTargetVisibility(cone, clock);
}

//------------------------------------------------------------------------------
//  bool CheckTargetVisibility(Real viewConeAngle, Real viewClockAngle = 0.0)
//------------------------------------------------------------------------------
/**
 * Determines whether or not the point is in the FieldOfView FOV
 *
 * @param viewConeAngle   the view cone angle
 * @param viewClockAngle  the view clock angle <unused for this class>
 *
 * @return true if the point is in the FieldOfView FOV; false otherwise
 */
//------------------------------------------------------------------------------
bool RectangularFOV::CheckTargetVisibility(Real viewConeAngle,
                                           Real viewClockAngle)
{
   bool retVal = true;
   // using <= assures that 0 width, 0 height FOV never has point in FOV
   // if you want (0.0,0.0) to always be in FOV change to strict inequalities
   if ((viewConeAngle  >= angleHeight) || (viewConeAngle  <= -angleHeight) ||
       (viewClockAngle >= angleWidth)  || (viewClockAngle <= -angleWidth) )
         retVal = false;

   return retVal;
}

//------------------------------------------------------------------------------
//  Rvector GetMaskConeAngles()
//------------------------------------------------------------------------------
/**
 * Returns the mask cone angles for the sensor
 *
 * @return array of mask cone angles
 */
//------------------------------------------------------------------------------
Rvector  RectangularFOV::GetMaskConeAngles()
{
	return Rvector(1, angleHeight);

}

//------------------------------------------------------------------------------
//  Rvector GetMaskClockAngles()
//------------------------------------------------------------------------------
/**
 * Returns the mask clock angles for the sensor
 *
 * @return array of mask clock angles
 */
//------------------------------------------------------------------------------
Rvector  RectangularFOV::GetMaskClockAngles()
{
	return Rvector(1, angleWidth);
}


//------------------------------------------------------------------------------
//  void SetAngleWidth(Real angleWidthIn)
//------------------------------------------------------------------------------
/**
 * Sets the angle width for the RectangularFOV
 *
 * @param angleWidthIn angle width
 */
//------------------------------------------------------------------------------
void  RectangularFOV::SetAngleWidth(Real angleWidthIn)
{
   angleWidth = angleWidthIn;
}

//------------------------------------------------------------------------------
//  Real GetAngleWidth()
//------------------------------------------------------------------------------
/**
 * Returns the angle width for the RectangularFOV
 *
 * @return the angle width
 */
//------------------------------------------------------------------------------
Real RectangularFOV::GetAngleWidth()
{
   return angleWidth;
}

//------------------------------------------------------------------------------
//  void SetAngleHeight(Real angleHeightIn)
//------------------------------------------------------------------------------
/**
 * Sets the angle height for the RectangularFOV
 *
 * @param angleHeightIn angle height
 */
//------------------------------------------------------------------------------
void  RectangularFOV::SetAngleHeight(Real angleHeightIn)
{
   angleHeight = angleHeightIn;
}

//------------------------------------------------------------------------------
//  Real GetAngleHeight()
//------------------------------------------------------------------------------
/**
 * Returns the angle height for the RectangularFOV
 *
 * @return the angle height
 */
//------------------------------------------------------------------------------
Real RectangularFOV::GetAngleHeight()
{
   return angleHeight;
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
std::string RectangularFOV::GetParameterText(const Integer id) const
{
	if (id >= FieldOfViewParamCount && id < RectangleFOVParamCount)
		return PARAMETER_TEXT[id - FieldOfViewParamCount];
	return FieldOfView::GetParameterText(id);
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
Integer RectangularFOV::GetParameterID(const std::string &str) const
{
	for (Integer i = FieldOfViewParamCount; i < RectangleFOVParamCount; i++)
	{
		if (str == PARAMETER_TEXT[i - FieldOfViewParamCount])
			return i;
	}

	return FieldOfView::GetParameterID(str);
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
Gmat::ParameterType RectangularFOV::GetParameterType(const Integer id) const
{
	if (id >= FieldOfViewParamCount && id < RectangleFOVParamCount)
		return PARAMETER_TYPE[id - FieldOfViewParamCount];

	return FieldOfView::GetParameterType(id);
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
std::string RectangularFOV::GetParameterTypeString(const Integer id) const
{
	return FieldOfView::PARAM_TYPE_STRING[GetParameterType(id)];
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
Real RectangularFOV::GetRealParameter(const Integer id) const
{
	if (id == ANGLE_WIDTH)
		return angleWidth * GmatMathConstants::DEG_PER_RAD;
	if (id == ANGLE_HEIGHT)
		return angleHeight * GmatMathConstants::DEG_PER_RAD;
	return FieldOfView::GetRealParameter(id);
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
Real RectangularFOV::SetRealParameter(const Integer id, const Real value)
{
	if (id == ANGLE_WIDTH)
	{
		if (value >= 0.0)
		{
			angleWidth = value * GmatMathConstants::RAD_PER_DEG;  // NEED to add validation here!
			return angleWidth;
		}
		else
		{
			FieldOfViewException fe("");
			fe.SetDetails(errorMessageFormat.c_str(),
				GmatStringUtil::ToString(value, 16).c_str(),
				"AngleWidth", "Real Number >= 0.0");
			throw fe;

		}
	}
	if (id == ANGLE_HEIGHT)
	{
		if (value >= 0)
		{
			angleHeight = value * GmatMathConstants::RAD_PER_DEG;
			return angleHeight;
		}
		else
		{
			FieldOfViewException fe("");
			fe.SetDetails(errorMessageFormat.c_str(),
				GmatStringUtil::ToString(value, 16).c_str(),
				"AngleHeight", "Real Number >= 0.0");
			throw fe;

		}

	}

	return FieldOfView::SetRealParameter(id, value);
}


// Required so derived classes work correctly:

//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Real RectangularFOV::GetRealParameter(const std::string &label) const
{
	return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
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
Real RectangularFOV::SetRealParameter(const std::string &label, const Real value)
{
	return SetRealParameter(GetParameterID(label), value);
}
