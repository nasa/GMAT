//------------------------------------------------------------------------------
//                           ConicalFOV
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
 * Implementation of the Conical FOV class
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "ConicalFOV.hpp"
#include "GmatConstants.hpp"
#include "FieldOfViewException.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"



//#define DEBUG_CONICAL_FOV_INIT // used in constructors & Initialize()
//#define DEBUG_CONICAL_FOV // use in main DCM and angular velocity computation
//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

/// Labels used for the conical field of view parameters.
const std::string
ConicalFOV::PARAMETER_TEXT[ConicalFOVParamCount - FieldOfViewParamCount] =
{
   "FieldOfViewAngle",
};


/// Types of the parameters used by field of view.
const Gmat::ParameterType
ConicalFOV:: PARAMETER_TYPE[ConicalFOVParamCount - FieldOfViewParamCount] =
{
   Gmat::REAL_TYPE,
};
//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ConicalFOV(Real fov)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param fov field-of-view (radians), FOV half-angle
 */
//------------------------------------------------------------------------------
ConicalFOV::ConicalFOV(const std::string &itsName) :
   FieldOfView(Gmat::CONICAL_FOV, "ConicalFOV", itsName),
   fieldOfViewAngle (30.0 * GmatMathConstants::RAD_PER_DEG)
{
   #ifdef DEBUG_CONICAL_FOV_INIT
      MessageInterface::ShowMessage("Entering ConicalFOV constructor\n");
      MessageInterface::ShowMessage("object name='%s', pointer=<%p>\n",
                                    GetName().c_str(),this);
      MessageInterface::ShowMessage("fieldOfViewAngle = %f\n",
            fieldOfViewAngle);
   #endif
   
   maxExcursionAngle = fieldOfViewAngle;
   
   objectTypes.push_back(Gmat::CONICAL_FOV);
   objectTypeNames.push_back("ConicalFOV");
   parameterCount = ConicalFOVParamCount;
}

//------------------------------------------------------------------------------
// ConicalFOV(const ConicalFOV &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
ConicalFOV::ConicalFOV(const ConicalFOV &copy) :
   FieldOfView(copy),
   fieldOfViewAngle (copy.fieldOfViewAngle)
{
   #ifdef DEBUG_CONICAL_FOV_INIT
      MessageInterface::ShowMessage("Executed ConicalFOV copy constructor\n\n");
   #endif
}

//------------------------------------------------------------------------------
// ConicalFOV& operator=(const ConicalFOV &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the ConicalFOV
 *
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
ConicalFOV& ConicalFOV::operator=(const ConicalFOV &copy)
{

   if (&copy == this)
      return *this;

   FieldOfView::operator=(copy);

   fieldOfViewAngle    = copy.fieldOfViewAngle;
   return *this;

}

//------------------------------------------------------------------------------
// ~ConicalFOV()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 */
//------------------------------------------------------------------------------
ConicalFOV::~ConicalFOV()
{
}

//---------------------------------------------------------------------------
//  GmatBase* Clone() const
//---------------------------------------------------------------------------
/**
 * Provides a clone of this object by calling the copy constructor.
 *
 * @return A GmatBase pointer to the cloned conical field of view.
 */
 //---------------------------------------------------------------------------
GmatBase* ConicalFOV::Clone() const
{
   #ifdef DEBUG_CONICAL_FOV_INIT
      MessageInterface::ShowMessage("Entered ConicalFOV::Clone()\n");
   #endif
	return new ConicalFOV(*this);
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
bool ConicalFOV::CheckTargetVisibility(const Rvector3 &target)
{
	Real RA, dec;
	Real cone, clock;
	UnitVecToRADEC(target, RA, dec);
	RADECtoConeClock(RA,dec,cone, clock);
   return CheckTargetVisibility(cone,clock);
}

//------------------------------------------------------------------------------
//  bool CheckTargetVisibility(Real viewConeAngle, Real viewClockAngle = 0.0)
//------------------------------------------------------------------------------
/**
 * Determines whether or not the point is in the FOV
 *
 * @param viewConeAngle   the view cone angle
 * @param viewClockAngle  the view clock angle <unused for this class>
 *
 * @return true if the point is in the FOV ; false otherwise
 */
//------------------------------------------------------------------------------
bool ConicalFOV::CheckTargetVisibility(Real viewConeAngle,Real viewClockAngle)
{

  return CheckTargetMaxExcursionAngle(viewConeAngle);
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
Rvector  ConicalFOV::GetMaskConeAngles()
{
   return Rvector (1, fieldOfViewAngle);
   
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
Rvector  ConicalFOV::GetMaskClockAngles()
{
   return Rvector(1, 0.0);
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
std::string ConicalFOV::GetParameterText(const Integer id) const
{
   if (id >= FieldOfViewParamCount && id < ConicalFOVParamCount)
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
Integer ConicalFOV::GetParameterID(const std::string &str) const
{
   for (Integer i = FieldOfViewParamCount; i < ConicalFOVParamCount; i++)
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
Gmat::ParameterType ConicalFOV::GetParameterType(const Integer id) const
{
   if (id >= FieldOfViewParamCount && id < ConicalFOVParamCount)
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
std::string ConicalFOV::GetParameterTypeString(const Integer id) const
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
Real ConicalFOV::GetRealParameter(const Integer id) const
{

   if (id == FIELD_OF_VIEW_ANGLE)
      return fieldOfViewAngle * GmatMathConstants::DEG_PER_RAD;

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
Real ConicalFOV::SetRealParameter(const Integer id, const Real value)
{
   if (id == FIELD_OF_VIEW_ANGLE)
   {
	   if (value >= 0.0)
	   {
		   fieldOfViewAngle = value * GmatMathConstants::RAD_PER_DEG;  // NEED to add validation here!
	   }
	   else
	   {
		   FieldOfViewException fe("");
		   fe.SetDetails(errorMessageFormat.c_str(),
			   GmatStringUtil::ToString(value, 16).c_str(),
			   "FieldOfViewAngle", "Real Number >= 0.0");
		   throw fe;

	   }

      #ifdef DEBUG_CONICAL_FOV_INIT
         MessageInterface::ShowMessage("ConicalFOV::SetRealParameter():\n");
         MessageInterface::ShowMessage(
            "Conical FOV set field of view angle to %f\n",
            fieldOfViewAngle );
         MessageInterface::ShowMessage("Exiting ConicalFOV constructor\n\n");
      #endif

      return fieldOfViewAngle;
   }


   #ifdef DEBUG_CONICAL_FOV_INIT
      MessageInterface::ShowMessage("ConicalFOV::SetRealParameter():");
      MessageInterface::ShowMessage("   Calling superclass FieldOfView\n");
      MessageInterface::ShowMessage("Exiting ConicalFOV constructor\n\n");
   #endif
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
Real ConicalFOV::GetRealParameter(const std::string &label) const
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
Real ConicalFOV::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


