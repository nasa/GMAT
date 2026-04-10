//$Id$
//------------------------------------------------------------------------------
//                                  TopocentricAxes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2008.09.11
//
/**
 * Definition of the TopocentricAxes class.
 *
 * @note There are three data files currently needed:
 *    - EOP file containing polar motion (x,y) and UT1-UTC offset
 *    - coefficient files containing nutation and planetary coefficients
 */
//------------------------------------------------------------------------------

#ifndef TopocentricAxes_hpp
#define TopocentricAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "DynamicAxes.hpp"
//#include "BodyFixedPoint.hpp"

// forward reference for BodyFixedPoint
class BodyFixedPoint;

class GMAT_API TopocentricAxes : public DynamicAxes
{
public:

   // default constructor
   TopocentricAxes(const std::string &itsName = "");
   // copy constructor
   TopocentricAxes(const TopocentricAxes &tAxes);
   // operator = for assignment
   const TopocentricAxes& operator=(const TopocentricAxes &tAxes);
   // destructor
   virtual ~TopocentricAxes();
   
   
   // method to initialize the data
   virtual bool Initialize();
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;

protected:

   enum
   {
      TopocentricAxesParamCount = DynamicAxesParamCount,
   };
   
   BodyFixedPoint   *bfPoint;
   CoordinateSystem *bfcs;
   CelestialBody    *centralBody;
   std::string      itsBodyName;
   std::string      horizonReference;
   Real             flattening;
   Real             radius;
   Rmatrix33        RFT;
   Rvector3         bfLocation;
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);
   virtual void CalculateRFT(const A1Mjd &atEpoch, const Rvector3 newLocation);
   
};
#endif // TopocentricAxes_hpp
