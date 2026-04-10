//$Id$
//------------------------------------------------------------------------------
//                              ImpulsiveBurn
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the ImpulsiveBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#ifndef ImpulsiveBurn_hpp
#define ImpulsiveBurn_hpp


#include "Burn.hpp"
#include "FuelTank.hpp"

// ChemicalThruster.hpp static const Integer COEFFICIENT_COUNT = 16;
// confuses with COEFFICIENT_COUNT 16, so commented out here.
//#define COEFFICIENT_COUNT 16

class GMAT_API ImpulsiveBurn : public Burn
{
public:
   
   ImpulsiveBurn(const std::string &nomme = "");
   virtual ~ImpulsiveBurn();
   ImpulsiveBurn(const ImpulsiveBurn &copy);
   ImpulsiveBurn&       operator=(const ImpulsiveBurn &orig);
   
   // inherited from Burn
   virtual void         SetSpacecraftToManeuver(Spacecraft *sat);
   virtual bool         Fire(Real *burnData = NULL, Real epoch = GmatTimeConstants::MJD_OF_J2000,
                             bool backwards = false);
   
   // inherited from GmatBase
   virtual bool         Validate();
   virtual bool         Initialize();
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
   // for parameters
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;

   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   
   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;
   // for Ref. objects
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
protected:
   void                 TransformDeltaVToJ2kFrame(Real *deltaV, Real epoch);
   bool                 SetTankFromSpacecraft();
   void                 DecrementMass(bool backwards);
   
   /// Isp value in seconds
   Real                    isp;
   /// Acceleration due to gravity, used to specify Isp in seconds
   Real                    gravityAccel;
   /// Computed delta tank mass
   Real                    deltaTankMass;
   /// Computed total tank mass
   Real                    totalTankMass;
   /// Decrement mass flag
   bool                    decrementMass;   
   /// Flag used for constant Isp
   bool                    constantExpressions;
   /// Flag used for Isp that only use the first 3 coefficients
   bool                    simpleExpressions;
   /// Tank names
   StringArray             tankNames;
   /// The tanks
   ObjectMap               tankMap;
   
   /// Published parameters
   enum
   {
      DECREMENT_MASS = BurnParamCount,
      FUEL_TANK,
      ISP,
      GRAVITATIONAL_ACCELERATION,
      DELTA_TANK_MASS,
      ImpulsiveBurnParamCount
   };
   
   /// Parameter labels
   static const std::string
      PARAMETER_TEXT[ImpulsiveBurnParamCount - BurnParamCount];
   /// Parameter types
   static const Gmat::ParameterType 
      PARAMETER_TYPE[ImpulsiveBurnParamCount - BurnParamCount];
};

#endif // ImpulsiveBurn_hpp
