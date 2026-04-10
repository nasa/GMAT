//$Id$
//------------------------------------------------------------------------------
//                               TransformUtil
//------------------------------------------------------------------------------
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2019.04.10
//
/**
* Original Python prototype:
* Author: N. Hatten
*
* Declares the TransformUtil class.
*/
//------------------------------------------------------------------------------
#ifndef TransformUtil_hpp
#define TransformUtil_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "AxisSystem.hpp"
#include "CelestialBody.hpp"
#include "EphemSmoother.hpp"

class GMAT_API TransformUtil
{
public:
   static Rvector6 TransformOrbitalState(const Rvector6 &stateIn, const Real &timeIn,
      CoordinateSystem *coordSysIn, const std::string &stateRepIn,
      CoordinateSystem *coordSysOut, const std::string &stateRepOut,
      const std::string &ephemType = "Spline",
      EphemSmoother *ephemerisSmoother = NULL);
   static void CalculateOrbitalJacobian(Rmatrix &jacOrbitStateOutputVarsWrtInputVars,
      Rmatrix &jacOrbitStateOutputVarsWrtTime, const Rvector6 &stateIn,
      const Real &timeIn, CoordinateSystem *coordSysIn,
      const std::string &stateRepIn, CoordinateSystem *coordSysOut,
      const std::string &stateRepOut, const std::string &ephemType = "Spline",
      EphemSmoother *ephemerisSmoother = NULL);

private:
   static Rmatrix DecVecTimeDerivatives(const Rvector6 &stateOut,
      const std::string &stateRepIn, const std::string &axesIn,
      const std::string &originIn);

   TransformUtil();
   TransformUtil(const TransformUtil &copy);
   ~TransformUtil();
   TransformUtil& operator=(const TransformUtil &copy);
};
#endif
