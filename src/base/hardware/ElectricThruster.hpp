//$Id$
//------------------------------------------------------------------------------
//                               ElectricThruster
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
// Created: 2014.06.19
//
/**
 * Class definition for the electric thrusters.
 */
//------------------------------------------------------------------------------


#ifndef ElectricThruster_hpp
#define ElectricThruster_hpp

#include "Thruster.hpp"
#include "FuelTank.hpp"
#include "CoordinateSystem.hpp"
#include "CelestialBody.hpp"

/**
 * Electric Thruster model used for finite maneuvers
 */
class GMAT_API ElectricThruster : public Thruster
{
public:

   static const Integer ELECTRIC_COEFF_COUNT = 5;

   ElectricThruster(const std::string &nomme);
   virtual ~ElectricThruster();
   ElectricThruster(const ElectricThruster& th);
   ElectricThruster&    operator=(const ElectricThruster& th);

   // required method for all subclasses
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* inst);

   // Parameter access methods - overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;

   // for enumerated strings
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const std::string &label) const;

   virtual bool         Initialize();
   virtual bool         SetPower(Real allocatedPower);
   virtual Real         CalculateMassFlow();

protected:
//   /// Finite burn instances access thruster data directly
//   friend class FiniteBurn;

   /// labels/strings for the ThrustModel
   static  StringArray        thrustModelLabels;
   /// Thrust coefficient units
   static  StringArray        thrustCoeffUnits;
   /// Mass Flow coefficient units
   static  StringArray        mfCoeffUnits;

   /// Published parameters for thrusters
   enum
   {
      THRUST_MODEL = ThrusterParamCount,
      MAXIMUM_USABLE_POWER,
      MINIMUM_USABLE_POWER,
      THRUST_COEFF1,
      THRUST_COEFF2,
      THRUST_COEFF3,
      THRUST_COEFF4,
      THRUST_COEFF5,
      MASS_FLOW_COEFF1,
      MASS_FLOW_COEFF2,
      MASS_FLOW_COEFF3,
      MASS_FLOW_COEFF4,
      MASS_FLOW_COEFF5,
      EFFICIENCY,
      ISP,
      CONSTANT_THRUST,
      T_UNITS,
      MF_UNITS,
      ElectricThrusterParamCount
   };

   std::string   thrustModel;
   Real          maxUsablePower;  // kW
   Real          minUsablePower;  // kW
   Real          thrustCoeff[5];
   Real          massFlowCoeff[5];
   Real          efficiency;
   Real          isp;
   Real          constantThrust;
   Real          powerToUse;
   Real          powerToUse2;
   Real          powerToUse3;
   Real          powerToUse4;

   /// Thruster parameter labels
   static const std::string
                        PARAMETER_TEXT[ElectricThrusterParamCount - ThrusterParamCount];
   /// Thruster parameter types
   static const Gmat::ParameterType
                        PARAMETER_TYPE[ElectricThrusterParamCount - ThrusterParamCount];


   bool                 CalculateThrustAndIsp();
};

#endif // ElectricThruster_hpp
