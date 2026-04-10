//------------------------------------------------------------------------------
//                           RightAscAdapter
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
* A measurement adapter for right ascension angle measurement
*/
//------------------------------------------------------------------------------

#include "RightAscAdapter.hpp"
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
// RightAscAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param name The name of the adapter
*/
//------------------------------------------------------------------------------
RightAscAdapter::RightAscAdapter(const std::string& name) :
AngleAdapterDeg(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RightAscAdapter default constructor <%p>\n", this);
#endif

   typeName = "RightAscension";             // "Angle";           // "AngleDeg"?
}


//------------------------------------------------------------------------------
// ~RightAscAdapter()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
RightAscAdapter::~RightAscAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RightAscAdapter default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// RightAscAdapter(const RightAscAdapter& aad)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param aad The adapter copied to make this one
*/
//------------------------------------------------------------------------------
RightAscAdapter::RightAscAdapter(const RightAscAdapter& aad) :
AngleAdapterDeg(aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RightAscAdapter copy constructor   from <%p> to <%p>\n", &aad, this);
#endif

}


//------------------------------------------------------------------------------
// RightAscAdapter& operator=(const RightAscAdapter& aad)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param aad The adapter copied to make this one match it
*
* @return This adapter made to look like aad
*/
//------------------------------------------------------------------------------
RightAscAdapter& RightAscAdapter::operator=(const RightAscAdapter& aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RightAscAdapter operator =   set <%p> = <%p>\n", this, &aad);
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
GmatBase* RightAscAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("RightAscAdapter::Clone() clone this <%p>\n", this);
#endif

   return new RightAscAdapter(*this);
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
bool RightAscAdapter::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start Initializing an RightAscAdapter <%p>\n", this);
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
   MessageInterface::ShowMessage("End Initializing an RightAscAdapter <%p>\n", this);
#endif

   return retval;
}


//------------------------------------------------------------------------------
// Real CalcMeasValue()
//------------------------------------------------------------------------------
/**
* Compute the right ascension value in degrees.
*
* @return The adapter's measurement value in degrees
*/
//------------------------------------------------------------------------------
Real RightAscAdapter::CalcMeasValue()
{
   Real rightAscension, declination;
   
   // TBD - this is not correct - fix RA calc
   MJ2000ToRaDec(mj2000Range, rightAscension, declination);
   //BodyFixedToRaDec(bfVector, rightAscension, declination);

   return rightAscension * GmatMathConstants::DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Rvector6 CalcDerivValue()
//------------------------------------------------------------------------------
/**
* Compute the partial derivatives of the right ascensino with respect to the
* Cartesian state.  Note that all partials derivatives with respect to the
* velocity are zero.
*
* This function must be called after CalcMeasValue().
*
* See GMAT Angle Measurements spec.  See Moyer 9-1, 9-2, 13-189, and 13-191.
*
* @return The adapter's derivative vector
*/
//------------------------------------------------------------------------------
Rvector6 RightAscAdapter::CalcDerivValue()
{
   Real declination, longitude;
   Rvector6 result(0, 0, 0, 0, 0, 0);

   BodyFixedToLongDec(bfRange, longitude, declination);

   Rvector3 A_bf(-sin(longitude), cos(longitude), 0);

   Real r23 = topoRange.GetMagnitude();

   Rvector3 A_inertial = BodyFixedToMJ2000T3(A_bf);

   Rvector3 deriv = A_inertial / (r23 * cos(declination));

   for (int i = 0; i < 3; i++)
      result[i] = deriv[i] * GmatMathConstants::DEG_PER_RAD;       // convert radian to degree

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("RightAscAdapter::CalcDerivValue:  bfRange = %.14lg  %.14lg  %.14lg\n",
      bfRange[0], bfRange[1], bfRange[2]);
   MessageInterface::ShowMessage("RightAscAdapter::CalcDerivValue:  longitude = %.14lg, declination = %.14lg, r23 = %.14lg\n",
      longitude, declination, r23);
   MessageInterface::ShowMessage("RightAscAdapter::CalcDerivValue:  A_bf = %.14lg  %.14lg  %.14lg\n",
      A_bf[0], A_bf[1], A_bf[2]);
   MessageInterface::ShowMessage("RightAscAdapter::CalcDerivValue:  A_inertial = %.14lg  %.14lg  %.14lg\n",
      A_inertial[0], A_inertial[1], A_inertial[2]);
   MessageInterface::ShowMessage("RightAscAdapter::CalcDerivValue:  deriv = %.14lg  %.14lg  %.14lg\n",
      deriv[0], deriv[1], deriv[2]);
#endif
   return result;
}

