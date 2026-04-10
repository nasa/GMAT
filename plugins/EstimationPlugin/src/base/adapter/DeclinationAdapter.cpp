//------------------------------------------------------------------------------
//                           DeclinationAdapter
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
* A measurement adapter for declination angle measurement
*/
//------------------------------------------------------------------------------

#include "DeclinationAdapter.hpp"
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
// DeclinationAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param name The name of the adapter
*/
//------------------------------------------------------------------------------
DeclinationAdapter::DeclinationAdapter(const std::string& name) :
AngleAdapterDeg(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("DeclinationAdapter default constructor <%p>\n", this);
#endif

   typeName = "Declination";        // "Angle";           // "AngleDeg"?
}


//------------------------------------------------------------------------------
// ~DeclinationAdapter()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
DeclinationAdapter::~DeclinationAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("DeclinationAdapter default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// DeclinationAdapter(const DeclinationAdapter& aad)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param aad The adapter copied to make this one
*/
//------------------------------------------------------------------------------
DeclinationAdapter::DeclinationAdapter(const DeclinationAdapter& aad) :
AngleAdapterDeg(aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("DeclinationAdapter copy constructor   from <%p> to <%p>\n", &aad, this);
#endif

}


//------------------------------------------------------------------------------
// DeclinationAdapter& operator=(const DeclinationAdapter& aad)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param aad The adapter copied to make this one match it
*
* @return This adapter made to look like aad
*/
//------------------------------------------------------------------------------
DeclinationAdapter& DeclinationAdapter::operator=(const DeclinationAdapter& aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("DeclinationAdapter operator =   set <%p> = <%p>\n", this, &aad);
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
GmatBase* DeclinationAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("DeclinationAdapter::Clone() clone this <%p>\n", this);
#endif

   return new DeclinationAdapter(*this);
}


//------------------------------------------------------------------------------
// Real CalcMeasValue()
//------------------------------------------------------------------------------
/**
* Compute the declination value in degrees.
*
* @return The adapter's measurement value in degrees
*/
//------------------------------------------------------------------------------
Real DeclinationAdapter::CalcMeasValue()
{
   Real rightAscension, declination;

   MJ2000ToRaDec(mj2000Range, rightAscension, declination);
   //BodyFixedToRaDec(bfVector, rightAscension, declination);

   return declination * GmatMathConstants::DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Rvector6 CalcDerivValue()
//------------------------------------------------------------------------------
/**
* Compute the partial derivatives of the declination with respect to the
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
Rvector6 DeclinationAdapter::CalcDerivValue()
{
   Real declination, longitude;
   Rvector6 result(0, 0, 0, 0, 0, 0);

   BodyFixedToLongDec(bfRange, longitude, declination);

   Real sinLong = sin(longitude);
   Real cosLong = cos(longitude);
   Real sinDecl = sin(declination);
   Real cosDecl = cos(declination);

   Rvector3 D_bf(-sinDecl * cosLong, -sinDecl * sinLong, cosDecl);

   Real r23 = topoRange.GetMagnitude();

   Rvector3 D_inertial = BodyFixedToMJ2000T3(D_bf);

   Rvector3 deriv = D_inertial / r23;

   for (int i = 0; i < 3; i++)
      result[i] = deriv[i] * GmatMathConstants::DEG_PER_RAD;       // convert radian to degree

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("DeclinationAdapter::CalcDerivValue:  bfRange = %.14lg  %.14lg  %.14lg\n",
      bfRange[0], bfRange[1], bfRange[2]);
   MessageInterface::ShowMessage("DeclinationAdapter::CalcDerivValue:  longitude = %.14lg, declination = %.14lg, r23 = %.14lg\n",
      longitude, declination, r23);
   MessageInterface::ShowMessage("DeclinationAdapter::CalcDerivValue:  D_bf = %.14lg  %.14lg  %.14lg\n",
      D_bf[0], D_bf[1], D_bf[2]);
   MessageInterface::ShowMessage("DeclinationAdapter::CalcDerivValue:  D_inertial = %.14lg  %.14lg  %.14lg\n",
      D_inertial[0], D_inertial[1], D_inertial[2]);
   MessageInterface::ShowMessage("DeclinationAdapter::CalcDerivValue:  deriv = %.14lg  %.14lg  %.14lg\n",
      deriv[0], deriv[1], deriv[2]);
#endif
   return result;
}

