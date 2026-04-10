//------------------------------------------------------------------------------
//                           ElevationAdapter
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
* A measurement adapter for elevation angle measurements
*/
//------------------------------------------------------------------------------

#include "ElevationAdapter.hpp"
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
// ElevationAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param name The name of the adapter
*/
//------------------------------------------------------------------------------
ElevationAdapter::ElevationAdapter(const std::string& name) :
AngleAdapterDeg(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("ElevationAdapter default constructor <%p>\n", this);
#endif

   typeName = "Elevation"; //"Angle";           // "AngleDeg"?
}


//------------------------------------------------------------------------------
// ~ElevationAdapter()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
ElevationAdapter::~ElevationAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("ElevationAdapter default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// ElevationAdapter(const ElevationAdapter& aad)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param aad The adapter copied to make this one
*/
//------------------------------------------------------------------------------
ElevationAdapter::ElevationAdapter(const ElevationAdapter& aad) :
AngleAdapterDeg(aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("ElevationAdapter copy constructor   from <%p> to <%p>\n", &aad, this);
#endif

}


//------------------------------------------------------------------------------
// ElevationAdapter& operator=(const ElevationAdapter& aad)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param aad The adapter copied to make this one match it
*
* @return This adapter made to look like rak
*/
//------------------------------------------------------------------------------
ElevationAdapter& ElevationAdapter::operator=(const ElevationAdapter& aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("ElevationAdapter operator =   set <%p> = <%p>\n", this, &aad);
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
GmatBase* ElevationAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("ElevationAdapter::Clone() clone this <%p>\n", this);
#endif

   return new ElevationAdapter(*this);
}


//------------------------------------------------------------------------------
// Real CalcMeasValue()
//------------------------------------------------------------------------------
/**
* Compute the elevation value in degrees.
*
* @return The adapter's measurement value in degrees
*/
//------------------------------------------------------------------------------
Real ElevationAdapter::CalcMeasValue()
{
   Real azimuthRad, elevationRad;

   TopocentricSEZToAzEl(topoRange, azimuthRad, elevationRad);

   // Update media corrections
   cMeasurement.ionoCorrectValue = cMeasurement.ionoCorrectRawValue;
   cMeasurement.tropoCorrectValue = cMeasurement.tropoCorrectRawValue;

   return elevationRad * GmatMathConstants::DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Rvector6 CalcDerivValue()
//------------------------------------------------------------------------------
/**
* Compute the partial derivatives of the elevation with respect to the Cartesian
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
Rvector6 ElevationAdapter::CalcDerivValue()
{
   Real azimuthRad, elevationRad;
   Rvector3 E_unit, N_unit, Z_unit;
   Rvector6 result(0, 0, 0, 0, 0, 0);

   GetENZUnitVectors(E_unit, N_unit, Z_unit);

   // topoRange was computed by AngleAdapterDeg::CalculateMeasurement()
   TopocentricSEZToAzEl(topoRange, azimuthRad, elevationRad);

   Real sinAzim = sin(azimuthRad);
   Real cosAzim = cos(azimuthRad);
   Real sinElev = sin(elevationRad);
   Real cosElev = cos(elevationRad);

   Rvector3 Dtilde_bf = -sinElev * cosAzim * N_unit - sinElev * sinAzim * E_unit + cosElev * Z_unit;

   Rvector3 Dtilde_inertial = BodyFixedToMJ2000T3(Dtilde_bf);

   Real r23 = topoRange.GetMagnitude();

   Rvector3 deriv = Dtilde_inertial / r23;

   for (int i = 0; i < 3; i++)
      result[i] = deriv[i] * GmatMathConstants::DEG_PER_RAD;       // convert radian to degree

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("ElevationAdapter::CalcDerivValue:  topoRange = %.14lg  %.14lg  %.14lg\n",
      topoRange[0], topoRange[1], topoRange[2]);
   MessageInterface::ShowMessage("ElevationAdapter::CalcDerivValue:  azimuth = %.14lg, elevation = %.14lg, r23 = %.14lg\n",
      azimuthRad, elevationRad, r23);
   MessageInterface::ShowMessage("ElevationAdapter::CalcDerivValue:  Dtilde_bf = %.14lg  %.14lg  %.14lg\n",
      Dtilde_bf[0], Dtilde_bf[1], Dtilde_bf[2]);
   MessageInterface::ShowMessage("ElevationAdapter::CalcDerivValue:  Dtilde_inertial = %.14lg  %.14lg  %.14lg\n",
      Dtilde_inertial[0], Dtilde_inertial[1], Dtilde_inertial[2]);
   MessageInterface::ShowMessage("ElevationAdapter::CalcDerivValue:  deriv = %.14lg  %.14lg  %.14lg\n",
      deriv[0], deriv[1], deriv[2]);
#endif
   return result;
}

