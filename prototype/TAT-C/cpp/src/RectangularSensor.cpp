//------------------------------------------------------------------------------
//                           RectangularSensor
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
// Author: Mike Stark, NASA/GSFC
// Created: 2017.04.03
//
/**
 * Implementation of the RectangularSensor class
 */
//------------------------------------------------------------------------------

#include "RectangularSensor.hpp"
#include "RealUtilities.hpp"

using namespace GmatMathUtil; // for trig functions, and
                              // temporarily square root fn


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RectangularSensor(Real angleWidthIn, Real angleHeightIn)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param angleWidthIn   angle width
 * @param angleHeightIn  angle height
 */
//------------------------------------------------------------------------------
RectangularSensor::RectangularSensor(Real angleWidthIn, Real angleHeightIn) :
   Sensor()
{
   angleWidth  = angleWidthIn;
   angleHeight = angleHeightIn;

   // length great circle from origin (0,0) to (angleHeight,angleWidth)
   // angular equivalent of length of hypotenuse of triangle for computing
   // length of a rectangle's diagonal from origin to (height, width)
   maxExcursionAngle = ACos( Cos(angleHeight)*Cos(angleWidth) );
}

//------------------------------------------------------------------------------
// RectangularSensor(const RectangularSensor &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
RectangularSensor::RectangularSensor(const RectangularSensor &copy) :
   Sensor(copy),
   angleWidth (copy.angleWidth),
   angleHeight(copy.angleHeight)
{
}

//------------------------------------------------------------------------------
// RectangularSensor& operator=(const RectangularSensor &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the RectangularSensor
 *
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
RectangularSensor& RectangularSensor::operator=(const RectangularSensor &copy)
{
   if (&copy != this)
   {
      Sensor::operator=(copy);
      angleHeight = copy.angleHeight;
      angleWidth  = copy.angleWidth;
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~RectangularSensor()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 */
//------------------------------------------------------------------------------
RectangularSensor::~RectangularSensor()
{
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
bool RectangularSensor::CheckTargetVisibility(Real viewConeAngle,
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
//  void SetAngleWidth(Real angleWidthIn)
//------------------------------------------------------------------------------
/**
 * Sets the angle width for the RectangularSensor
 *
 * @param angleWidthIn angle width
 */
//------------------------------------------------------------------------------
void  RectangularSensor::SetAngleWidth(Real angleWidthIn)
{
   angleWidth = angleWidthIn;
}

//------------------------------------------------------------------------------
//  Real GetAngleWidth()
//------------------------------------------------------------------------------
/**
 * Returns the angle width for the RectangularSensor
 *
 * @return the angle width
 */
//------------------------------------------------------------------------------
Real RectangularSensor::GetAngleWidth()
{
   return angleWidth;
}

//------------------------------------------------------------------------------
//  void SetAngleHeight(Real angleHeightIn)
//------------------------------------------------------------------------------
/**
 * Sets the angle height for the RectangularSensor
 *
 * @param angleHeightIn angle height
 */
//------------------------------------------------------------------------------
void  RectangularSensor::SetAngleHeight(Real angleHeightIn)
{
   angleHeight = angleHeightIn;
}

//------------------------------------------------------------------------------
//  Real GetAngleHeight()
//------------------------------------------------------------------------------
/**
 * Returns the angle height for the RectangularSensor
 *
 * @return the angle height
 */
//------------------------------------------------------------------------------
Real RectangularSensor::GetAngleHeight()
{
   return angleHeight;
}





