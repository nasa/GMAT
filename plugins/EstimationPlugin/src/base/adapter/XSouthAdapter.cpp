//------------------------------------------------------------------------------
//                           XSouthAdapter
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
* A measurement adapter for XSouth angle measurement
*/
//------------------------------------------------------------------------------

#include "XSouthAdapter.hpp"
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
// XSouthAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param name The name of the adapter
*/
//------------------------------------------------------------------------------
XSouthAdapter::XSouthAdapter(const std::string& name) :
AngleAdapterDeg(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("XSouthAdapter default constructor <%p>\n", this);
#endif

   typeName = "XSouth";   // "Angle";           // "AngleDeg"?
}


//------------------------------------------------------------------------------
// ~XSouthAdapter()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
XSouthAdapter::~XSouthAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("XSouthAdapter default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// XSouthAdapter(const XSouthAdapter& aad)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param aad The adapter copied to make this one
*/
//------------------------------------------------------------------------------
XSouthAdapter::XSouthAdapter(const XSouthAdapter& aad) :
AngleAdapterDeg(aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("XSouthAdapter copy constructor   from <%p> to <%p>\n", &aad, this);
#endif

}


//------------------------------------------------------------------------------
// XSouthAdapter& operator=(const XSouthAdapter& aad)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param aad The adapter copied to make this one match it
*
* @return This adapter made to look like aad
*/
//------------------------------------------------------------------------------
XSouthAdapter& XSouthAdapter::operator=(const XSouthAdapter& aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("XSouthAdapter operator =   set <%p> = <%p>\n", this, &aad);
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
GmatBase* XSouthAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("XSouthAdapter::Clone() clone this <%p>\n", this);
#endif

   return new XSouthAdapter(*this);
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
bool XSouthAdapter::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start Initializing an XSouthAdapter <%p>\n", this);
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
   MessageInterface::ShowMessage("End Initializing an XSouthAdapter <%p>\n", this);
#endif

   return retval;
}


//------------------------------------------------------------------------------
// Real CalcMeasValue()
//------------------------------------------------------------------------------
/**
* Compute the X South angle value in degrees.
*
* @return The adapter's measurement value in degrees
*/
//------------------------------------------------------------------------------
Real XSouthAdapter::CalcMeasValue()
{
   Real xSouth, yEast, partial;

   TopocentricSEZToXSYE(topoRange, xSouth, yEast);

   // Update media corrections
   PartialXSPartialEl(topoRange, partial);
   cMeasurement.ionoCorrectValue = partial * cMeasurement.ionoCorrectRawValue;
   cMeasurement.tropoCorrectValue = partial * cMeasurement.tropoCorrectRawValue;

   return xSouth * GmatMathConstants::DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Rvector6 CalcDerivValue()
//------------------------------------------------------------------------------
/**
* Compute the partial derivatives of the X South angle with respect to the
* Cartesian state.  Note that all partials derivatives with respect to the
* velocity are zero.
*
* This function must be called after CalcMeasValue().
*
* See GMAT Angle Measurements spec.  See Moyer 9-13, 9-14, 13-196, and 13-197.
*
* @return The adapter's derivative vector
*/
//------------------------------------------------------------------------------
Rvector6 XSouthAdapter::CalcDerivValue()
{
   Real xSouth, yEast;
   Rvector3 E_unit, N_unit, Z_unit;
   Rvector6 result(0, 0, 0, 0, 0, 0);

   GetENZUnitVectors(E_unit, N_unit, Z_unit);

   TopocentricSEZToXSYE(topoRange, xSouth, yEast);

   Rvector3 Adoubleprime_bf = -cos(xSouth) * N_unit - sin(xSouth) * Z_unit;

   Real r23 = topoRange.GetMagnitude();

   Rvector3 Adoubleprime_inertial = BodyFixedToMJ2000T3(Adoubleprime_bf);

   Rvector3 deriv = Adoubleprime_inertial / (r23 * cos(yEast));

   for (int i = 0; i < 3; i++)
      result[i] = deriv[i] * GmatMathConstants::DEG_PER_RAD;       // convert radian to degree

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("XSouthAdapter::CalcDerivValue:  topoRange = %.14lg  %.14lg  %.14lg\n",
      topoRange[0], topoRange[1], topoRange[2]);
   MessageInterface::ShowMessage("XSouthAdapter::CalcDerivValue:  xSouth = %.14lg, yEast = %.14lg, r23 = %.14lg\n",
      xSouth, yEast, r23);
   MessageInterface::ShowMessage("XSouthAdapter::CalcDerivValue:  Adoubleprime_bf = %.14lg  %.14lg  %.14lg\n",
      Adoubleprime_bf[0], Adoubleprime_bf[1], Adoubleprime_bf[2]);
   MessageInterface::ShowMessage("XSouthAdapter::CalcDerivValue:  Adoubleprime_inertial = %.14lg  %.14lg  %.14lg\n",
      Adoubleprime_inertial[0], Adoubleprime_inertial[1], Adoubleprime_inertial[2]);
   MessageInterface::ShowMessage("XSouthAdapter::CalcDerivValue:  deriv = %.14lg  %.14lg  %.14lg\n",
      deriv[0], deriv[1], deriv[2]);
#endif
   return result;
}

