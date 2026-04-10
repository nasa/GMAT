//$Id:  $
//------------------------------------------------------------------------------
//                                  ICRFAxes
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Wendy C. Shoan/GSFC/GSSB
//         Tuan Dang Nguyen/GSFC
// Created: 2012.02.23
//
/**
 * Definition of the ICRFAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef ICRFAxes_hpp
#define ICRFAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "InertialAxes.hpp"

class GMAT_API ICRFAxes : public InertialAxes
{
public:

   // default constructor
   ICRFAxes(const std::string &itsName = "");
   // copy constructor
   ICRFAxes(const ICRFAxes &icrf);
   // operator = for assignment
   const ICRFAxes& operator=(const ICRFAxes &icrf);
   // destructor
   virtual ~ICRFAxes();

   // method to initialize the data
   virtual bool Initialize();

   virtual GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "FK5") const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;

   Rmatrix33  GetRotationMatrix(const A1Mjd &atEpoch, bool forceComputation = false);

protected:

   enum
   {
      ICRFAxesParamCount = InertialAxesParamCount,
   };

   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

};
#endif // ICRFAxes_hpp
