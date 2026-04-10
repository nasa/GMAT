//$Id$
//------------------------------------------------------------------------------
//                            CoordinateTranslation
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
* Declares the CoordinateTranslation class.
*/
//------------------------------------------------------------------------------
#ifndef CoordinateTranslation_hpp
#define CoordinateTranslation_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "CoordinateSystem.hpp"
#include "EphemSmoother.hpp"

class GMAT_API CoordinateTranslation
{
public:
   static Rvector6 TranslateOrigin(const A1Mjd &epoch, SpacePoint *oldOriginBody,
      const Rvector6 &stateWrtOldOrigin, SpacePoint *newOriginBody,
      CoordinateSystem *referenceFrame);
   static Rvector6 TranslateOriginSmoothed(const A1Mjd &epoch,
      SpacePoint *oldOriginBody, const Rvector6 &stateWrtOldOrigin,
      SpacePoint *newOriginBody, CoordinateSystem *referenceFrame,
      EphemSmoother *ephemerisSmoother);
   static void TranslateOriginSmoothedWithDerivatives(const A1Mjd &epoch,
      SpacePoint *oldOriginBody, const Rvector6 &stateWrtOldOrigin,
      SpacePoint *newOriginBody, CoordinateSystem *referenceFrame,
      EphemSmoother *ephemerisSmoother, Rvector6 &stateWrtNewOrigin, 
      Rmatrix &dOldOriginWrtNewOriginDState,
      Rmatrix &dOldOriginWrtNewOriginDt);

private:
   CoordinateTranslation();
   CoordinateTranslation(const CoordinateTranslation &copy);
   CoordinateTranslation& operator=(const CoordinateTranslation &copy);
   ~CoordinateTranslation();
};
#endif
