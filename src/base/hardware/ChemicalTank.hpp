//$Id$
//------------------------------------------------------------------------------
//                               ChemicalTank
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
// Author: Darrel J. Conway (original FuelTank code)
// Created: 2004/11/08
//
/**
 * Class definition for the Chemical Fuel Tank class.  Code was pulled
 * from the original FuleTank class.
 */
//------------------------------------------------------------------------------


#ifndef ChemicalTank_hpp
#define ChemicalTank_hpp

#include "FuelTank.hpp"


/**
 * Chemical fuel tank model used in finite maneuvers.
 */
class GMAT_API ChemicalTank : public FuelTank
{
public:

   ChemicalTank(const std::string &nomme);
   virtual ~ChemicalTank();
   ChemicalTank(const ChemicalTank& ft);
   ChemicalTank&            operator=(const ChemicalTank& ft);

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
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);

   // for enumerated strings
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const std::string &label) const;

   // required method for all subclasses
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);

   virtual bool         Initialize();
   virtual bool         Validate();

   DEFAULT_TO_NO_REFOBJECTS

protected:
//   /// Mass of the fuel in the tank
//   Real                 fuelMass;
   /// Tank pressure
   Real                 pressure;
   /// Fuel temperature
   Real                 temperature;
   /// Reference temperature
   Real                 refTemperature;
   /// Tank volume
   Real                 volume;
   /// Fuel density
   Real                 density;
   /// Flag indicating if the tank is pressure regulated or blow-down
   bool                 pressureRegulated; // deprecated
//   /// Flag indicating if negative fuel mass is allowed
//   bool                 allowNegativeFuelMass;
   /// Pressure model used
   Integer              pressureModel;

   // Parameters used for internal calculations

   /// Pressurant volume, a calculated parameter
   Real                 gasVolume;
   /// Baseline product of the pressure and temperature
   Real                 pvBase;

   virtual void         UpdateTank();
   virtual void         DepleteFuel(Real dm);

   /// Pressure model list
   enum
   {
      TPM_PRESSURE_REGULATED,
      TPM_BLOW_DOWN
   };

   /// Availabel pressure model list
   static StringArray   pressureModelList;
   //static const std::string pressureModelList[2];
public:
   /// Published parameters for generic fuel tanks
   enum
   {
//      ALLOW_NEGATIVE_FUEL_MASS = FuelTankParamCount,
//      FUEL_MASS,
      PRESSURE = FuelTankParamCount,
      TEMPERATURE,
      REFERENCE_TEMPERATURE,
      VOLUME,
      FUEL_DENSITY,
      PRESSURE_MODEL,
      PRESSURE_REGULATED,  // deprecated
      ChemicalTankParamCount
   };

   /// Parameter labels
   static const std::string
      PARAMETER_TEXT[ChemicalTankParamCount - FuelTankParamCount];
   /// Parameter types
   static const Gmat::ParameterType
      PARAMETER_TYPE[ChemicalTankParamCount - FuelTankParamCount];
};

#endif // ChemicalTank_hpp
