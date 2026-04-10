//$Id$
//------------------------------------------------------------------------------
//                               FuelTank
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
// Author: Darrel J. Conway
// Created: 2004/11/08
// Modified:
//    2010.03.18 Thomas Grubb
//      - Changed visiblity of PARAMETER_TEXT, PARAMETER_TYPE, and enum from
//        protected to public
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class definition for the Fuel Tank base class.
 */
//------------------------------------------------------------------------------


#ifndef FuelTank_hpp
#define FuelTank_hpp

#include "Hardware.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"

/**
 * Basic fuel tank model used in finite maneuvers.
 */
class GMAT_API FuelTank : public Hardware
{
public:

   FuelTank(const std::string &typeStr, const std::string &nomme);
   virtual ~FuelTank();
   FuelTank(const FuelTank& ft);
   FuelTank&            operator=(const FuelTank& ft);

   // Parameter access methods - overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);

//***** NEED GETS & SETS FOR Rvector AND Rmatrix ***********************

//   virtual std::string  GetStringParameter(const Integer id) const;
//   virtual bool         SetStringParameter(const Integer id,
//                                           const std::string &value);
//   virtual std::string  GetStringParameter(const std::string &label) const;
//   virtual bool         SetStringParameter(const std::string &label,
//                                           const std::string &value);

//   // for enumerated strings
//   virtual const StringArray&
//                        GetPropertyEnumStrings(const Integer id) const;
//   virtual const StringArray&
//                        GetPropertyEnumStrings(const std::string &label) const;

//   // required method for all subclasses
//   virtual GmatBase*    Clone() const;
//   virtual void         Copy(const GmatBase* orig);

   virtual bool         Initialize();
   virtual bool         Validate() = 0;

   void                 SetFlowWithoutThruster(bool directFlowAllowed);
   bool                 NoThrusterNeeded();

   // selectors for mass properties
   virtual Real         GetFuelMass();
   virtual Rvector3     GetFuelCM_BCS() const;
   virtual Rmatrix33    GetFuelMOI_BCS(const Rvector3 &SC_Cm) const;

   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Mass of the fuel in the tank
   Real                 fuelMass;
   /// Center of mass in tank coordinates (meters)
   Rvector3             fuelCM;
   /// Fuel moment of inertia in tank coordinates, relative to fuel center of mass (kg-m**2)
   Rmatrix33            fuelMOI;
   /// Flag indicating if negative fuel mass is allowed
   bool                 allowNegativeFuelMass;
   /// Flag used to allow direct mass flow, for instance for thrust history file
   bool                 noThrusterNeeded;
   /// Flag to check whether we have successfully performed the first validation pass
   bool                 firstValidation;

   virtual void         UpdateTank()         = 0;
   virtual void         DepleteFuel(Real dm) = 0;

public:
   /// Published parameters for generic fuel tanks
   enum
   {
      ALLOW_NEGATIVE_FUEL_MASS = HardwareParamCount,
      FUEL_MASS,
      FUEL_CM_X,
      FUEL_CM_Y,
      FUEL_CM_Z,
      FUEL_MOI_XX,
      FUEL_MOI_XY,
      FUEL_MOI_XZ,
      FUEL_MOI_YY,
      FUEL_MOI_YZ,
      FUEL_MOI_ZZ,
      FUEL_CM_X_BCS,
      FUEL_CM_Y_BCS,
      FUEL_CM_Z_BCS,
      FUEL_MOI_XX_BCS,
      FUEL_MOI_XY_BCS,
      FUEL_MOI_XZ_BCS,
      FUEL_MOI_YY_BCS,
      FUEL_MOI_YZ_BCS,
      FUEL_MOI_ZZ_BCS,
      FuelTankParamCount
   };

   /// Parameter labels
   static const std::string
      PARAMETER_TEXT[FuelTankParamCount - HardwareParamCount];
   /// Parameter types
   static const Gmat::ParameterType
      PARAMETER_TYPE[FuelTankParamCount - HardwareParamCount];
};

#endif // FuelTank_hpp
