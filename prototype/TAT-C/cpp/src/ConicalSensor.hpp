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
// Created: 2016.05.03
//
/**
 * Definition of the Conical Sensor class.  This class models a conical sensor.
 */
//------------------------------------------------------------------------------
#ifndef ConicalSensor_hpp
#define ConicalSensor_hpp

#include "gmatdefs.hpp"
#include "OrbitState.hpp"
#include "AbsoluteDate.hpp"
#include "Sensor.hpp"

class ConicalSensor : public Sensor
{
public:
   
   /// class construction/destruction
   ConicalSensor(Real fov);
   ConicalSensor( const ConicalSensor &copy);
   ConicalSensor& operator=(const ConicalSensor &copy);
   
   virtual ~ConicalSensor();
   
   /// Set and Get the field-of-view
   virtual void  SetFieldOfView(Real fov);
   virtual Real  GetFieldOfView();

   /// Check the target visibility given the input cone and clock angles:
   /// determines whether or not the point is in the sensor FOV.
   virtual bool  CheckTargetVisibility(Real viewConeAngle,
                                       Real viewClockAngle = 0.0);

protected:
   
   /// Field-of-View (radians)
   Real                  fieldOfView;
};
#endif // ConicalSensor_hpp
