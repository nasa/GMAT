//------------------------------------------------------------------------------
//                           YNorthAdapter
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
* A measurement adapter for YNorth angle measurement
*/
//------------------------------------------------------------------------------

#include "YNorthAdapter.hpp"
#include "RandomNumber.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "SignalBase.hpp"
#include "ErrorModel.hpp"
#include "BodyFixedPoint.hpp"
#include "SpaceObject.hpp"
#include "PropSetup.hpp"
#include "ODEModel.hpp"
#include <sstream>


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_ANGLE_CALCULATION


//------------------------------------------------------------------------------
// YNorthAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param name The name of the adapter
*/
//------------------------------------------------------------------------------
YNorthAdapter::YNorthAdapter(const std::string& name) :
AngleAdapterDeg(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("YNorthAdapter default constructor <%p>\n", this);
#endif

   typeName = "YNorth";      // "Angle";           // "AngleDeg"?
}


//------------------------------------------------------------------------------
// ~YNorthAdapter()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
YNorthAdapter::~YNorthAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("YNorthAdapter default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// YNorthAdapter(const YNorthAdapter& aad)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param aad The adapter copied to make this one
*/
//------------------------------------------------------------------------------
YNorthAdapter::YNorthAdapter(const YNorthAdapter& aad) :
AngleAdapterDeg(aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("YNorthAdapter copy constructor   from <%p> to <%p>\n", &aad, this);
#endif

}


//------------------------------------------------------------------------------
// YNorthAdapter& operator=(const YNorthAdapter& aad)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param aad The adapter copied to make this one match it
*
* @return This adapter made to look like aad
*/
//------------------------------------------------------------------------------
YNorthAdapter& YNorthAdapter::operator=(const YNorthAdapter& aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("YNorthAdapter operator =   set <%p> = <%p>\n", this, &aad);
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
GmatBase* YNorthAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("YNorthAdapter::Clone() clone this <%p>\n", this);
#endif

   return new YNorthAdapter(*this);
}


//------------------------------------------------------------------------------
// Real CalcMeasValue()
//------------------------------------------------------------------------------
/**
* Compute the Y North angle value in degrees.
*
* @return The adapter's measurement value in degrees
*/
//------------------------------------------------------------------------------
Real YNorthAdapter::CalcMeasValue()
{
   Real xEast, yNorth, partial;

   TopocentricSEZToXEYN(topoRange, xEast, yNorth);

   // Update media corrections
   PartialYNPartialEl(topoRange, partial);
   cMeasurement.ionoCorrectValue = partial * cMeasurement.ionoCorrectRawValue;
   cMeasurement.tropoCorrectValue = partial * cMeasurement.tropoCorrectRawValue;

   return yNorth * GmatMathConstants::DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Rvector6 CalcDerivValue()
//------------------------------------------------------------------------------
/**
* Compute the partial derivatives of the Y North angle with respect to the
* Cartesian state.  Note that all partials derivatives with respect to the
* velocity are zero.
*
* This function must be called after CalcMeasValue().
*
* See GMAT Angle Measurements spec.  See Moyer 9-11, 9-12, 13-194, and 13-195.
*
* @return The adapter's derivative vector
*/
//------------------------------------------------------------------------------
Rvector6 YNorthAdapter::CalcDerivValue()
{
   Real xEast, yNorth, sinYNorth;
   Rvector3 E_unit, N_unit, Z_unit;
   Rvector6 result(0, 0, 0, 0, 0, 0);

   GetENZUnitVectors(E_unit, N_unit, Z_unit);

   TopocentricSEZToXEYN(topoRange, xEast, yNorth);

   sinYNorth = sin(yNorth);

   Rvector3 Dprime_bf = cos(yNorth) * N_unit - sinYNorth * sin(xEast) * E_unit
      - sinYNorth * cos(xEast) * Z_unit;

   Real r23 = topoRange.GetMagnitude();

   Rvector3 Dprime_inertial = BodyFixedToMJ2000T3(Dprime_bf);

   Rvector3 deriv = Dprime_inertial / r23;

   for (int i = 0; i < 3; i++)
      result[i] = deriv[i] * GmatMathConstants::DEG_PER_RAD;       // convert radian to degree

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("YNorthAdapter::CalcDerivValue:  topoRange = %.14lg  %.14lg  %.14lg\n",
      topoRange[0], topoRange[1], topoRange[2]);
   MessageInterface::ShowMessage("YNorthAdapter::CalcDerivValue:  xEast = %.14lg, yNorth = %.14lg, r23 = %.14lg\n",
      xEast, yNorth, r23);
   MessageInterface::ShowMessage("YNorthAdapter::CalcDerivValue:  Dprime_bf = %.14lg  %.14lg  %.14lg\n",
      Dprime_bf[0], Dprime_bf[1], Dprime_bf[2]);
   MessageInterface::ShowMessage("YNorthAdapter::CalcDerivValue:  Dprime_inertial = %.14lg  %.14lg  %.14lg\n",
      Dprime_inertial[0], Dprime_inertial[1], Dprime_inertial[2]);
   MessageInterface::ShowMessage("YNorthAdapter::CalcDerivValue:  deriv = %.14lg  %.14lg  %.14lg\n",
      deriv[0], deriv[1], deriv[2]);
#endif
   return result;
}

