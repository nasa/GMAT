//------------------------------------------------------------------------------
//                              ScalingUtility
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Wendy Shoan / NASA
// Created: 2018.12.04
//
/**
 * From original Python prototype:
 * Author: S. Hughes, N. Hatten
 */
//------------------------------------------------------------------------------

#ifndef ScalingUtility_hpp
#define ScalingUtility_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"

class CSALT_API ScalingUtility
{
public:
   
   /// Constructors/destructors
   ScalingUtility();
   ScalingUtility(const ScalingUtility &copy);
   ScalingUtility& operator=(const ScalingUtility &copy);
   virtual ~ScalingUtility();
   
   // Initialization/setup methods
   bool ValidateUnit(const std::string &unitName, bool isException = true);
   bool SetUnit(const std::string &unitName, Real factor);
   bool SetShift(const std::string &unitName, Real shift);
   bool SetUnitAndShift(const std::string &unitName, Real factor, Real shift);
   bool AddUnitAndShift(const std::string &unitName, Real factor, Real shift);

   // Accessor methods
   void GetUnitAndShift(const std::string &unitName, Real &factor, Real &shift);

   // Scaling methods
   Real ScaleParameter(const Real &unscaled, const std::string &unit);
   Real UnscaleParameter(const Real &scaled, const std::string &unit);

   Rvector ScaleVector(const Rvector &unscaled, const StringArray &units);
   Rvector UnscaleVector(const Rvector &scaled, const StringArray &units);
   
   Rmatrix ScaleJacobian(const Rmatrix &unscaled, const StringArray &funUnits,
                         const StringArray &varUnits);
   Rmatrix ScaleJacobianByVars(const Rmatrix &unscaled,
	                            const StringArray &varUnits);
   Rmatrix ScaleJacobianByFun(const Rmatrix &unscaled,
	                           const StringArray &funUnits);
   
   Rmatrix UnscaleJacobian(const Rmatrix &scaled, const StringArray &funUnits,
                           const StringArray &varUnits);
   Rmatrix UnscaleJacobianByVars(const Rmatrix &scaled,
                                 const StringArray &varUnits);
   Rmatrix UnscaleJacobianByFun(const Rmatrix &scaled,
	                             const StringArray &funUnits);

protected:

   /// scale factors and shifts for the units   
   std::map<std::string, Real> unitFactors;
   std::map<std::string, Real> unitShifts;
};

#endif // ScalingUtility_hpp
