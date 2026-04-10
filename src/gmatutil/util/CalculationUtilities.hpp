//$Id:  $
//------------------------------------------------------------------------------
//                              CalculationUtilities
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Created: 2011.08.12
//
/**
 * This namespace provides Calculation Utilities that can be used to compute the
 * GMAT Calculation Objects.
 *
 * NOTE: the input state must be in the coordinate system desired - no
 *       conversions are done here
 */
//------------------------------------------------------------------------------
#ifndef CalculationUtilities_hpp
#define CalculationUtilities_hpp

#include "BaseException.hpp"
#include "GmatConstants.hpp"
#include "Rvector6.hpp"


namespace GmatCalcUtil
{
   Real GMATUTIL_API  CalculateBPlaneData(const std::string &item, const Rvector6 &state, const Real originMu);
   Real GMATUTIL_API  CalculateAngularData(const std::string &item, const Rvector6 &state,
                                       const Real &originMu, const Rvector3 &originToSunUnit);
   Real GMATUTIL_API  CalculateKeplerianData(const std::string &item, const Rvector6 &state, const Real originMu);
   Real GMATUTIL_API  CalculatePlanetData(const std::string &item, const Rvector6 &state,
                                      const Real originRadius, const Real originFlattening, const Real originHourAngle);
}

#endif /* CalculationUtilities_hpp */
