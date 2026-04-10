//------------------------------------------------------------------------------
//                           AzimuthAdapter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: 
// Created: Aug 31, 2018
/**
* A measurement adapter for azimuth angle measurement
*/
//------------------------------------------------------------------------------

#include "AzimuthAdapter.hpp"
#include "RandomNumber.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "SignalBase.hpp"
#include "ErrorModel.hpp"
#include "BodyFixedPoint.hpp"
#include "SpaceObject.hpp"
#include "PropSetup.hpp"
#include "ODEModel.hpp"
#include "GroundstationInterface.hpp"
#include <sstream>


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_ANGLE_CALCULATION


//------------------------------------------------------------------------------
// AzimuthAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param name The name of the adapter
*/
//------------------------------------------------------------------------------
AzimuthAdapter::AzimuthAdapter(const std::string& name) :
AngleAdapterDeg(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("AzimuthAdapter default constructor <%p>\n", this);
#endif

   typeName = "Azimuth"; // "Angle";           // "AngleDeg"?
}


//------------------------------------------------------------------------------
// ~AzimuthAdapter()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
AzimuthAdapter::~AzimuthAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("AzimuthAdapter default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// AzimuthAdapter(const AzimuthAdapter& aad)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param aad The adapter copied to make this one
*/
//------------------------------------------------------------------------------
AzimuthAdapter::AzimuthAdapter(const AzimuthAdapter& aad) :
AngleAdapterDeg(aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("AzimuthAdapter copy constructor   from <%p> to <%p>\n", &aad, this);
#endif

}


//------------------------------------------------------------------------------
// AzimuthAdapter& operator=(const AzimuthAdapter& aad)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param aad The adapter copied to make this one match it
*
* @return This adapter made to look like aad
*/
//------------------------------------------------------------------------------
AzimuthAdapter& AzimuthAdapter::operator=(const AzimuthAdapter& aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("AzimuthAdapter operator =   set <%p> = <%p>\n", this, &aad);
#endif

   if (this != &aad)
   {
      TrackingDataAdapter::operator=(aad);
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
* Creates a new adapter that matches this one
*
* @return A new adapter set to match this one
*/
//------------------------------------------------------------------------------
GmatBase* AzimuthAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("AzimuthAdapter::Clone() clone this <%p>\n", this);
#endif

   return new AzimuthAdapter(*this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
* Prepares the adapter for use
*
* @return true if the initialization succeeds, false if it fails
*/
//------------------------------------------------------------------------------
bool AzimuthAdapter::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start Initializing an AzimuthAdapter <%p>\n", this);
#endif

   bool retval = false;

   if (AngleAdapterDeg::Initialize())
   {
      retval = true;
      cMeasurement.isPeriodic = true;
      cMeasurement.minValue = 0.0;
      cMeasurement.period = 360.0;
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("End Initializing an AzimuthAdapter <%p>\n", this);
#endif

   return retval;
}


//------------------------------------------------------------------------------
// Real CalcMeasValue()
//------------------------------------------------------------------------------
/**
* Compute the azimuth value in degrees.
*
* @return The adapter's measurement value in degrees
*/
//------------------------------------------------------------------------------
Real AzimuthAdapter::CalcMeasValue()
{
   Real azimuthRad, elevationRad;

   // topoRange was computed by AngleAdapterDeg::CalculateMeasurement()
   TopocentricSEZToAzEl(topoRange, azimuthRad, elevationRad);

   return azimuthRad * GmatMathConstants::DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Rvector6 CalcDerivValue()
//------------------------------------------------------------------------------
/**
* Compute the partial derivatives of the azimuth with respect to the Cartesian
* state.  Note that all partials derivatives with respect to the velocity are
* zero. 
*
* This function must be called after CalcMeasValue().
*
* See GMAT Angle Measurements spec.  See Moyer 9-9, 9-10, 13-192, and 13-193.
*
* @return The adapter's derivative vector
*/
//------------------------------------------------------------------------------
Rvector6 AzimuthAdapter::CalcDerivValue()
{
   Real azimuthRad, elevationRad;
   Rvector3 E_unit, N_unit, Z_unit;
   Rvector6 result(0, 0, 0, 0, 0, 0);

   GetENZUnitVectors(E_unit, N_unit, Z_unit);

   // topoRange was computed by AngleAdapterDeg::CalculateMeasurement()
   TopocentricSEZToAzEl(topoRange, azimuthRad, elevationRad);

   Rvector3 Atilde_bf = -sin(azimuthRad) * N_unit + cos(azimuthRad) * E_unit;

   Real r23 = topoRange.GetMagnitude();

   Rvector3 Atilde_inertial = BodyFixedToMJ2000T3(Atilde_bf);

   Rvector3 deriv = Atilde_inertial / (r23 * cos(elevationRad));

   for (int i = 0; i < 3; i++)
      result[i] = deriv[i] * GmatMathConstants::DEG_PER_RAD;       // convert radian to degree

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("AzimuthAdapter::CalcDerivValue:  topoRange = %.14lg  %.14lg  %.14lg\n",
      topoRange[0], topoRange[1], topoRange[2]);
   MessageInterface::ShowMessage("AzimuthAdapter::CalcDerivValue:  azimuth = %.14lg, elevation = %.14lg, r23 = %.14lg\n",
      azimuthRad, elevationRad, r23);
   MessageInterface::ShowMessage("AzimuthAdapter::CalcDerivValue:  Atilde_bf = %.14lg  %.14lg  %.14lg\n",
      Atilde_bf[0], Atilde_bf[1], Atilde_bf[2]);
   MessageInterface::ShowMessage("AzimuthAdapter::CalcDerivValue:  Atilde_inertial = %.14lg  %.14lg  %.14lg\n",
      Atilde_inertial[0], Atilde_inertial[1], Atilde_inertial[2]);
   MessageInterface::ShowMessage("AzimuthAdapter::CalcDerivValue:  deriv = %.14lg  %.14lg  %.14lg\n",
      deriv[0], deriv[1], deriv[2]);
#endif
   return result;
}

