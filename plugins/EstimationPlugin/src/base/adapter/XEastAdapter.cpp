//------------------------------------------------------------------------------
//                           XEastAdapter
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
* A measurement adapter for XEast angle measurement
*/
//------------------------------------------------------------------------------

#include "XEastAdapter.hpp"
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
// XEastAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param name The name of the adapter
*/
//------------------------------------------------------------------------------
XEastAdapter::XEastAdapter(const std::string& name) :
AngleAdapterDeg(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("XEastAdapter default constructor <%p>\n", this);
#endif

   typeName = "XEast"; // "Angle";           // "AngleDeg"?
}


//------------------------------------------------------------------------------
// ~XEastAdapter()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
XEastAdapter::~XEastAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("XEastAdapter default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// XEastAdapter(const XEastAdapter& aad)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param aad The adapter copied to make this one
*/
//------------------------------------------------------------------------------
XEastAdapter::XEastAdapter(const XEastAdapter& aad) :
AngleAdapterDeg(aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("XEastAdapter copy constructor   from <%p> to <%p>\n", &aad, this);
#endif

}


//------------------------------------------------------------------------------
// XEastAdapter& operator=(const XEastAdapter& aad)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param aad The adapter copied to make this one match it
*
* @return This adapter made to look like aad
*/
//------------------------------------------------------------------------------
XEastAdapter& XEastAdapter::operator=(const XEastAdapter& aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("XEastAdapter operator =   set <%p> = <%p>\n", this, &aad);
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
GmatBase* XEastAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("XEastAdapter::Clone() clone this <%p>\n", this);
#endif

   return new XEastAdapter(*this);
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
bool XEastAdapter::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start Initializing an XEastAdapter <%p>\n", this);
#endif

   bool retval = false;

   if (AngleAdapterDeg::Initialize())
   {
      retval = true;
      cMeasurement.isPeriodic = true;
      cMeasurement.minValue = -180.0;
      cMeasurement.period = 360.0;
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("End Initializing an XEastAdapter <%p>\n", this);
#endif

   return retval;
}


//------------------------------------------------------------------------------
// Real CalcMeasValue()
//------------------------------------------------------------------------------
/**
* Compute the X East angle value in degrees.
*
* @return The adapter's measurement value in degrees
*/
//------------------------------------------------------------------------------
Real XEastAdapter::CalcMeasValue()
{
   Real xEastRad, yNorthRad, partial;

   TopocentricSEZToXEYN(topoRange, xEastRad, yNorthRad);

   // Update media corrections
   PartialXEPartialEl(topoRange, partial);
   cMeasurement.ionoCorrectValue = partial * cMeasurement.ionoCorrectRawValue;
   cMeasurement.tropoCorrectValue = partial * cMeasurement.tropoCorrectRawValue;

   return xEastRad * GmatMathConstants::DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Rvector6 CalcDerivValue()
//------------------------------------------------------------------------------
/**
* Compute the partial derivatives of the X East angle with respect to the
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
Rvector6 XEastAdapter::CalcDerivValue()
{
   Real xEast, yNorth;
   Rvector3 E_unit, N_unit, Z_unit;
   Rvector6 result(0, 0, 0, 0, 0, 0);

   GetENZUnitVectors(E_unit, N_unit, Z_unit);

   TopocentricSEZToXEYN(topoRange, xEast, yNorth);

   Rvector3 Aprime_bf = cos(xEast) * E_unit - sin(xEast) * Z_unit;

   Real r23 = topoRange.GetMagnitude();

   Rvector3 Aprime_inertial = BodyFixedToMJ2000T3(Aprime_bf);

   Rvector3 deriv = Aprime_inertial / (r23 * cos(yNorth));

   for (int i = 0; i < 3; i++)
      result[i] = deriv[i] * GmatMathConstants::DEG_PER_RAD;       // convert radian to degree

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("XEastAdapter::CalcDerivValue:  topoRange = %.14lg  %.14lg  %.14lg\n",
      topoRange[0], topoRange[1], topoRange[2]);
   MessageInterface::ShowMessage("XEastAdapter::CalcDerivValue:  xEast = %.14lg, yNorth = %.14lg, r23 = %.14lg\n",
      xEast, yNorth, r23);
   MessageInterface::ShowMessage("XEastAdapter::CalcDerivValue:  Aprime_bf = %.14lg  %.14lg  %.14lg\n",
      Aprime_bf[0], Aprime_bf[1], Aprime_bf[2]);
   MessageInterface::ShowMessage("XEastAdapter::CalcDerivValue:  Aprime_inertial = %.14lg  %.14lg  %.14lg\n",
      Aprime_inertial[0], Aprime_inertial[1], Aprime_inertial[2]);
   MessageInterface::ShowMessage("XEastAdapter::CalcDerivValue:  deriv = %.14lg  %.14lg  %.14lg\n",
      deriv[0], deriv[1], deriv[2]);
#endif
   return result;
}

