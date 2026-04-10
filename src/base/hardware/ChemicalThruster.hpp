//$Id$
//------------------------------------------------------------------------------
//                               ChemicalThruster
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
// Author: Wendy Shoan/GSFC/GSSB
// Created: 2014.06.30
//
/**
 * Class definition for the ChemicalThrusters.
 * Code pulled from original Thruster class.
 */
//------------------------------------------------------------------------------


#ifndef ChemicalThruster_hpp
#define ChemicalThruster_hpp

#include "Thruster.hpp"
#include "FuelTank.hpp"
#include "CoordinateSystem.hpp"
#include "CelestialBody.hpp"

/**
 * ChemicalThruster model used for finite maneuvers
 */
class GMAT_API ChemicalThruster : public Thruster
{
public:
   
   //./burn/ImpulsiveBurn.hpp:#define COEFFICIENT_COUNT 16
   static const Integer COEFFICIENT_COUNT = 16;

   ChemicalThruster(const std::string &nomme);
   virtual ~ChemicalThruster();
   ChemicalThruster(const ChemicalThruster& th);
   ChemicalThruster&    operator=(const ChemicalThruster& th);

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

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;

   virtual bool         Initialize();
   virtual Real         CalculateMassFlow();

protected:
   
   /// Array of thrust coefficients
   Real                       cCoefficients[COEFFICIENT_COUNT];
   /// Array of specific impulse coefficients
   Real                       kCoefficients[COEFFICIENT_COUNT];

   /// C-coefficient units
   static  StringArray        cCoefUnits;
   /// K-coefficient units
   static  StringArray        kCoefUnits;

   /// Published parameters for ChemicalThrusters
   enum
   {
      C1 = ThrusterParamCount,
      C2,    C3,    C4,    C5,    C6,    C7,    C8,
      C9,   C10,   C11,   C12,   C13,   C14,   C15,   C16,
      K1,    K2,    K3,    K4,    K5,    K6,    K7,    K8,
      K9,   K10,   K11,   K12,   K13,   K14,   K15,   K16,
      C_UNITS,
      K_UNITS,
      ChemicalThrusterParamCount
   };

   /// ChemicalThruster parameter labels
   static const std::string
                        PARAMETER_TEXT[ChemicalThrusterParamCount - ThrusterParamCount];
   /// ChemicalThruster parameter types
   static const Gmat::ParameterType
                        PARAMETER_TYPE[ChemicalThrusterParamCount - ThrusterParamCount];

   bool                 CalculateThrustAndIsp();
};

#endif // ChemicalThruster_hpp
