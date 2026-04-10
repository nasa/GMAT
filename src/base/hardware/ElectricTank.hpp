//$Id$
//------------------------------------------------------------------------------
//                               ElectricTank
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
// Author: Wendy Shoan
// Created: 2014.07.22
//
/**
 * Class definition for the Electric Fuel Tank class.
 */
//------------------------------------------------------------------------------


#ifndef ElectricTank_hpp
#define ElectricTank_hpp

#include "FuelTank.hpp"


/**
 * Chemical fuel tank model used in finite maneuvers.
 */
class GMAT_API ElectricTank : public FuelTank
{
public:

   ElectricTank(const std::string &nomme);
   virtual ~ElectricTank();
   ElectricTank(const ElectricTank& ft);
   ElectricTank&            operator=(const ElectricTank& ft);

   // Parameter access methods - overridden from GmatBase
//   virtual std::string  GetParameterText(const Integer id) const;
//   virtual std::string  GetParameterUnit(const Integer id) const;
//   virtual Integer      GetParameterID(const std::string &str) const;
//   virtual Gmat::ParameterType
//                        GetParameterType(const Integer id) const;
//   virtual std::string  GetParameterTypeString(const Integer id) const;
//
//   virtual bool         IsParameterReadOnly(const Integer id) const;
//   virtual bool         IsParameterCommandModeSettable(const Integer id) const;


   // required method for all subclasses
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);

   virtual bool         Initialize();
   virtual bool         Validate();

   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// No additional data at this time

   virtual void         UpdateTank();
   virtual void         DepleteFuel(Real dm);

public:
   /// Published parameters for generic fuel tanks
   enum
   {
      ElectricTankParamCount = FuelTankParamCount,
   };

//   /// Parameter labels
//   static const std::string
//      PARAMETER_TEXT[ElectricTankParamCount - FuelTankParamCount];
//   /// Parameter types
//   static const Gmat::ParameterType
//      PARAMETER_TYPE[ElectricTankParamCount - FuelTankParamCount];
};

#endif // ElectricTank_hpp
