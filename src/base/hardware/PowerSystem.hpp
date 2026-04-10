//$Id$
//------------------------------------------------------------------------------
//                               PowerSystem
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
// Created: 2014.04.28
//
/**
 * Base class definition for the PowerSystem.
 */
//------------------------------------------------------------------------------

#ifndef PowerSystem_hpp
#define PowerSystem_hpp

#include "Hardware.hpp"
#include "gmatdefs.hpp"
#include "SpacePoint.hpp"
#include "SolarSystem.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton

// Declare forward reference since Spacecraft owns PowerSystem
class Spacecraft;

/**
 * Basic power system model attached to Spacecraft.
 */
class GMAT_API PowerSystem : public Hardware
{
public:

   PowerSystem(const std::string &systemType, const std::string &nomme);
   virtual ~PowerSystem();
   PowerSystem(const PowerSystem& copy);
   PowerSystem& operator=(const PowerSystem& copy);

   /// Initialize the PowerSystem
   virtual bool         Initialize();
   virtual void         Copy(const GmatBase*);
   /// Set reference objects
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetSpacecraft(Spacecraft *sc);
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

   /// Return computed quantities
   virtual Real         GetBasePower() const;
   virtual Real         GetPowerGenerated() const = 0;      // Total Power Available
   virtual Real         GetSpacecraftBusPower() const;      // Required Bus Power
   virtual Real         GetThrustPower() const;             // Thrust Power Available

   virtual void         SetEpoch(const std::string &ep);
   virtual std::string  GetEpochString();

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


protected:
   /// Epoch format
   std::string          epochFormat;
   /// Initial epoch as input/set
   std::string          initialEpoch;
   /// Initial Maximum Power
   Real                 initialMaxPower;
   /// Annual Decay Rate
   Real                 annualDecayRate;
   /// Margin
   Real                 margin;
   /// Bus Coefficents
   Real                 busCoeff1;
   Real                 busCoeff2;
   Real                 busCoeff3;

   /// initialEpoch as a real A1Mjd
   Real                 initialEp;
   /// initialEpoch

   /// Pointer to the SolarSystem
   SolarSystem         *solarSystem;
   /// Pointer to the Sun
   CelestialBody       *sun;
   /// pointer to the owning Spacecraft
   Spacecraft          *spacecraft;
   /// the origin of the Spacecraft
   SpacePoint          *scOrigin;
   /// Radius of the sun
   Real                sunRadius;

   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;

   /// Published parameters for all power systems
   enum
   {
      EPOCH_FORMAT = HardwareParamCount,
      INITIAL_EPOCH,
      INITIAL_MAX_POWER,
      ANNUAL_DECAY_RATE,
      MARGIN,
      BUS_COEFF1,
      BUS_COEFF2,
      BUS_COEFF3,
      TOTAL_POWER_AVAILABLE,  // need these to make Parameters?
      REQUIRED_BUS_POWER,
      THRUST_POWER_AVAILABLE,
      PowerSystemParamCount
   };

   /// Parameter labels
   static const std::string
      PARAMETER_TEXT[PowerSystemParamCount - HardwareParamCount];
   /// Parameter types
   static const Gmat::ParameterType
      PARAMETER_TYPE[PowerSystemParamCount - HardwareParamCount];


   Real EpochToReal(const std::string &ep);
   Real GetSunToSCDistance(Real atEpoch) const;

};

#endif // PowerSystem_hpp
