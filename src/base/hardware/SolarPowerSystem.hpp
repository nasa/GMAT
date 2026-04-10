//$Id$
//------------------------------------------------------------------------------
//                               SolarPowerSystem
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
// Author: Wendy C. Shoan
// Created: 2014.05.01
//
/**
 * Base class definition for the SolarPowerSystem.
 */
//------------------------------------------------------------------------------


#ifndef SolarPowerSystem_hpp
#define SolarPowerSystem_hpp

#include "PowerSystem.hpp"
#include "ShadowState.hpp"

/**
 * Basic solar power system model attached to Spacecraft.
 */
class GMAT_API SolarPowerSystem : public PowerSystem
{
public:

   SolarPowerSystem(const std::string &nomme);
   virtual ~SolarPowerSystem();
   SolarPowerSystem(const SolarPowerSystem& copy);
   SolarPowerSystem&            operator=(const SolarPowerSystem& copy);

   virtual bool         Initialize();
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase*);
   virtual void         SetSpacecraft(Spacecraft *sc);
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

   virtual Real         GetPowerGenerated() const;      // Total Power

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;

//   virtual GmatBase*    GetRefObject(const UnsignedInt type,
//                                     const std::string &name,
//                                     const Integer index);
//   virtual bool         SetRefObject(GmatBase *obj,
//                                     const UnsignedInt type,
//                                     const std::string &name = "");
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
//   virtual bool         HasRefObjectTypeArray();
//   virtual const ObjectTypeArray&
//                        GetRefObjectTypeArray();
//   virtual const StringArray&
//                        GetRefObjectNameArray(const UnsignedInt type);

   virtual bool         WriteEmptyStringArray(Integer id);

protected:
   /// Published parameters for solar power systems
   enum
   {
      SHADOW_MODEL  = PowerSystemParamCount,
      SHADOW_BODIES,
      SOLAR_COEFF1,
      SOLAR_COEFF2,
      SOLAR_COEFF3,
      SOLAR_COEFF4,
      SOLAR_COEFF5,
      SolarPowerSystemParamCount
   };

   /// Parameter labels
   static const std::string
      PARAMETER_TEXT[SolarPowerSystemParamCount - PowerSystemParamCount];
   /// Parameter types
   static const Gmat::ParameterType
      PARAMETER_TYPE[SolarPowerSystemParamCount - PowerSystemParamCount];

   Real                solarCoeff1;
   Real                solarCoeff2;
   Real                solarCoeff3;
   Real                solarCoeff4;
   Real                solarCoeff5;

   std::string         shadowModel;
   /// list of body names
   StringArray         shadowBodyNames;
   /// list of default body names
   StringArray         defaultShadowBodyNames;
   /// Array of shadow bodies
   std::vector<CelestialBody*> shadowBodies;

   /// a pointer to an instance of the ShadowState class, to
   /// compute shadow states at a given epoch
   ShadowState         *shadowState;
   /// Did the user set zero bodies (allowed)?
   bool                settingNoBodies;

   static bool    occultationWarningWritten;

};

#endif // SolarPowerSystem_hpp
