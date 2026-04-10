//$Id$
//------------------------------------------------------------------------------
//                               Thruster
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
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class definition for the thrusters.
 */
//------------------------------------------------------------------------------


#ifndef Thruster_hpp
#define Thruster_hpp

#include "Hardware.hpp"
#include "FuelTank.hpp"
#include "CoordinateSystem.hpp"
#include "CelestialBody.hpp"

// Declare forward reference since Spacecraft owns Thruster
class Spacecraft;

/**
 * Thruster model used for finite maneuvers
 */
class GMAT_API Thruster : public Hardware
{
public:
   
   static const Integer AXES_COUNT = 4;
   
   Thruster(const std::string &typeStr, const std::string &nomme);
   virtual ~Thruster();
   Thruster(const Thruster& th);
   Thruster&            operator=(const Thruster& th);
   
   Real                 GetMassFlowRate();
   Real                 GetThrustMagnitude();
   Real                 GetIsp();
   
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
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);

   virtual const Rvector&
                        GetRvectorParameter(const Integer id) const;
   virtual const Rvector&
                        SetRvectorParameter(const Integer id,
                                            const Rvector &value);
   virtual const Rvector&
                        GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&
                        SetRvectorParameter(const std::string &label,
                                            const Rvector &value);

   
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const; 
   
   // for enumerated strings
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const std::string &label) const;
   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;
   
   // Ref. object access methods - overridden from GmatBase
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         SetRefObject(GmatBase *obj, 
                                     const UnsignedInt type,
                                     const std::string &name = "");
   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);
   
   
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual bool         Initialize();
   virtual void         SetDerivativeState(Real *state, CelestialBody *origin);
   virtual bool         SetPower(Real allocatedPower);
   virtual Real         CalculateMassFlow() = 0;
   
protected:
   /// Finite burn instances access thruster data directly
   friend class FiniteBurn;
   
   /// Solar system used to find the J2000 body, etc.
   SolarSystem                *solarSystem;
   /// Local Coordinate system
   CoordinateSystem           *localCoordSystem;
   /// Coordinate system
   CoordinateSystem           *coordSystem;
   /// Origin object pointer if coordinate system is set to Local
   CelestialBody              *localOrigin;
   /// J2000body pointer
   CelestialBody              *j2000Body;
   /// Secondary Spacecraft object if coordinate system is set to Local
   Spacecraft                 *spacecraft;
   /// Coordinate system name
   std::string                coordSystemName;
   /// Origin name if coordinate system is set to Local
   std::string                localOriginName;
   /// Axes name if coordinate system is set to Local
   std::string                localAxesName;
   /// Name of the J2000 body
   std::string                j2000BodyName;
   /// Name of the Spacecraft that has thruster
   std::string                satName;
   /// The power allocated to the thruster
   Real                       power;
   /// Acceleration due to gravity, used to specify Isp in seconds
   Real                       gravityAccel;   
   /// Thrust duty cycle for this thruster
   Real                       dutyCycle;
   /// Thrust scale factor for this thruster
   Real                       thrustScaleFactor;
   /// Current tank pressure
   Real                       pressure;
   /// Current tank temperature divided by reference temperature
   Real                       temperatureRatio;
   /// Most recently calculated thrust
   Real                       thrust;
   /// Most recently calculated applied thrust magnitude
   Real                       appliedThrustMag;
   /// Most recently calculated specific impulse
   Real                       impulse;
   /// Most recently calculated mass flow rate
   Real                       mDot;
   /// Thrust direction projected into the inertial coordinate system
   Real                       inertialDirection[3];
   /// Decrement mass flag
   bool                       decrementMass;
   /// Flag used to turn thruster on or off
   bool                       thrusterFiring;
   /// Flag used for constant thrust and Isp
   bool                       constantExpressions;
   /// Flag used for thrust and Isp that only use the first 3 coefficients
   bool                       simpleExpressions;
   /// Flag indicating if local coordinate system is used
   bool                       usingLocalCoordSys;
   /// Flag indicating if axes is MJ2000Eq
   bool                       isMJ2000EqAxes;
   /// Flag indicating if axes is SpacecrftBody
   bool                       isSpacecraftBodyAxes;
   /// Tank names
   StringArray                tankNames;
   /// The tanks
   std::vector<FuelTank *>    tanks;
   /// The mix ratio for the tank draws
   Rvector                    mixRatio;

   /// Epoch for the inertial reference frame
   GmatEpoch                  inertialEpoch;
   /// Current state for thrust evaluation (when needed)
   Real                       *derivState;
   /// Origin used for the derivState
   CelestialBody              *derivOrigin;

   /// Temporary buffer used to get ref objects
   ObjectArray                tempArray;
   
   /// Available local axes labels
   static  StringArray        localAxesLabels;

   /// Published parameters for thrusters
   enum
   {
      THRUSTER_FIRING = HardwareParamCount, 
      COORDINATE_SYSTEM,
      ORIGIN,
      AXES,
      DUTY_CYCLE,
      THRUST_SCALE_FACTOR,
      DECREMENT_MASS,
      TANK,
      MIXRATIO,
      GRAVITATIONAL_ACCELERATION,
      THRUST,
      APPLIED_THRUST_MAG,
      ISP,
      MASS_FLOW_RATE,
      ThrusterParamCount
   };
   
   /// Thruster parameter labels
   static const std::string
                        PARAMETER_TEXT[ThrusterParamCount - HardwareParamCount];
   /// Thruster parameter types
   static const Gmat::ParameterType
                        PARAMETER_TYPE[ThrusterParamCount - HardwareParamCount];
   
   virtual bool         CalculateThrustAndIsp() = 0;
   bool                 SetSpacecraft(Spacecraft *sat);
   
   CoordinateSystem*    CreateLocalCoordinateSystem();
   void                 ConvertDirectionToInertial(Real *dv, Real *dvInertial,
                                                   Real epoch);
   void                 ComputeInertialDirection(Real epoch);
   void                 WriteDeprecatedMessage(const std::string &oldProp,
                                               const std::string &newProp) const;
   
};

#endif // Thruster_hpp
