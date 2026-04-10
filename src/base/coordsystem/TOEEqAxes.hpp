//$Id$
//------------------------------------------------------------------------------
//                                  TOEEqAxes
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
// Created: 2005/04/27
//
/**
 * Definition of the TOEEqAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef TOEEqAxes_hpp
#define TOEEqAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "InertialAxes.hpp"

class GMAT_API TOEEqAxes : public InertialAxes
{
public:

   // default constructor
   TOEEqAxes(const std::string &itsName = "");
   // copy constructor
   TOEEqAxes(const TOEEqAxes &toe);
   // operator = for assignment
   const TOEEqAxes& operator=(const TOEEqAxes &toe);
   // destructor
   virtual ~TOEEqAxes();
   
   // method to initialize the data
   virtual bool Initialize();

   virtual GmatCoordinate::ParameterUsage UsesEpoch() const;
   virtual GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "FK5") const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;
   
protected:

   enum
   {
      TOEEqAxesParamCount = InertialAxesParamCount,
   };
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

};
#endif // TOEEqAxes_hpp
