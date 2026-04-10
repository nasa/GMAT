//------------------------------------------------------------------------------
//                           ConicalSensor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.05.02
//
/**
 * Implementation of the ConicalSensor class
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ConicalSensor.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ConicalSensor(Real fov)
//------------------------------------------------------------------------------
/**
 * Constructor
 * 
 * @param fov field-of-view for the sensor (radians), sensor half-angle
 */
//------------------------------------------------------------------------------
ConicalSensor::ConicalSensor(Real fov) :
   Sensor(),
   fieldOfView (fov)
{
   maxExcursionAngle = fov;
}

//------------------------------------------------------------------------------
// ConicalSensor(const ConicalSensor &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 * 
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
ConicalSensor::ConicalSensor(const ConicalSensor &copy) :
   Sensor(copy),
   fieldOfView (copy.fieldOfView)
{
}

//------------------------------------------------------------------------------
// ConicalSensor& operator=(const ConicalSensor &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the ConicalSensor
 * 
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
ConicalSensor& ConicalSensor::operator=(const ConicalSensor &copy)
{
   if (&copy == this)
      return *this;
   
   Sensor::operator=(copy);
   
   fieldOfView    = copy.fieldOfView;
   
   return *this;
}

//------------------------------------------------------------------------------
// ~ConicalSensor()
//------------------------------------------------------------------------------
/**
 * Destructor
 * 
 */
//------------------------------------------------------------------------------
ConicalSensor::~ConicalSensor()
{
}

//------------------------------------------------------------------------------
//  void SetFieldOfView(Real fov)
//------------------------------------------------------------------------------
/**
 * Sets the field-of-view for the ConicalSensor
 * 
 * @param fov field-of-view (radians)
 */
//------------------------------------------------------------------------------
void ConicalSensor::SetFieldOfView(Real fov)
{
   fieldOfView = fov;
}

//------------------------------------------------------------------------------
//  Real GetFieldOfView()
//------------------------------------------------------------------------------
/**
 * Returns the field-of-view for the ConicalSensor
 * 
 * @return field-of-view (radians)
 */
//------------------------------------------------------------------------------
Real ConicalSensor::GetFieldOfView()
{
   return fieldOfView;
}

//------------------------------------------------------------------------------
//  bool CheckTargetVisibility(Real viewConeAngle, Real viewClockAngle = 0.0)
//------------------------------------------------------------------------------
/**
 * Determines whether or not the point is in the sensor FOV
 *
 * @param viewConeAngle   the view cone angle
 * @param viewClockAngle  the view clock angle <unused for this class>
 *
 * @return true if the point is in the sensor FOV; false otherwise
 */
//------------------------------------------------------------------------------
bool ConicalSensor::CheckTargetVisibility(Real viewConeAngle,
                                          Real viewClockAngle) 
{
   // Determine if a point is in sensor FOV
   return CheckTargetMaxExcursionAngle(viewConeAngle);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
