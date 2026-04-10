//$Id$
//------------------------------------------------------------------------------
//                                  BodyInertialAxes
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
// Created: 2006.08.01
//
/**
 * Definition of the BodyInertialAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef BodyInertialAxes_hpp
#define BodyInertialAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "InertialAxes.hpp"

class GMAT_API BodyInertialAxes : public InertialAxes
{
public:

   // default constructor
   BodyInertialAxes(const std::string &itsName = "");
   // copy constructor
   BodyInertialAxes(const BodyInertialAxes &bodyInertial);
   // operator = for assignment
   const BodyInertialAxes& operator=(const BodyInertialAxes &bodyInertial);
   // destructor
   virtual ~BodyInertialAxes();
   
   // method to initialize the data
   virtual bool                           Initialize();

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*                      Clone(void) const;

   
protected:

   enum
   {
      BodyInertialAxesParamCount = InertialAxesParamCount,
   };

   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);
};
#endif // BodyInertialAxes_hpp
