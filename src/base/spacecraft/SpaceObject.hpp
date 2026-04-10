//$Id$
//------------------------------------------------------------------------------
//                              SpaceObject
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
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/7/24
//
/**
 * Defines the base class used for spacecraft and formations. 
 */
//------------------------------------------------------------------------------


#ifndef SpaceObject_hpp
#define SpaceObject_hpp

#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "GmatState.hpp"
#include "SpaceObjectException.hpp"

class GMAT_API SpaceObject : public SpacePoint
{
public:
   SpaceObject(UnsignedInt typeId, const std::string &typeStr,
               const std::string &instName);
   virtual ~SpaceObject();
   SpaceObject(const SpaceObject& so);
   SpaceObject&         operator=(const SpaceObject& so);
   
   virtual Rvector6     GetLastState();
   virtual GmatState&   GetState();
   virtual Rvector3     GetAcceleration() { return acceleration; };
   virtual void         SetAcceleration(Rvector3 accel) { acceleration = accel; };

   virtual Real         GetEpoch();
   virtual GmatTime     GetEpochGT();

   virtual Real         SetEpoch(const Real ep);
   virtual GmatTime     SetEpochGT(const GmatTime ep);

   virtual bool         IsManeuvering();
   virtual void         IsManeuvering(bool mnvrFlag);
   virtual const StringArray&
                        GetManeuveringMembers();
   virtual bool         ParametersHaveChanged();
   virtual void         ParametersHaveChanged(bool flag);
   
//   virtual Integer GetParameterID(const std::string &str) const;
   virtual GmatTime     GetGmatTimeParameter(const Integer id) const;
   virtual GmatTime     SetGmatTimeParameter(const Integer id, 
                                             const GmatTime value);
   virtual GmatTime     GetGmatTimeParameter(const std::string &label) const;
   virtual GmatTime     SetGmatTimeParameter(const std::string &label, 
                                             const GmatTime value);

   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
   
   virtual Real         GetRealParameter(const Integer id, const Integer row,
                                         const Integer col) const;
   virtual Real         GetRealParameter(const std::string &label, 
                                         const Integer row, 
                                         const Integer col) const;
   virtual Real         SetRealParameter(const Integer id, const Real value,
                                         const Integer row, const Integer col);
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value, const Integer row,
                                         const Integer col);
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);

   /// @todo Waiting for CoordinateSystems in Spacecraft, then see if needed
   virtual void SetOriginName(std::string cbName);
   virtual const std::string GetOriginName();
   virtual void SetOrigin(SpacePoint *cb);
   virtual SpacePoint* GetOrigin();
   
   virtual const Rvector6 GetMJ2000State(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Position(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Acceleration(const A1Mjd &atTime);

   virtual const Rvector6 GetMJ2000State(const Real atTime) { return GetMJ2000State(A1Mjd(atTime)); };
   virtual const Rvector3 GetMJ2000Position(const Real atTime) { return GetMJ2000Position(A1Mjd(atTime)); };
   virtual const Rvector3 GetMJ2000Velocity(const Real atTime) { return GetMJ2000Velocity(A1Mjd(atTime)); };
   virtual const Rvector3 GetMJ2000Acceleration(const Real atTime) { return GetMJ2000Acceleration(A1Mjd(atTime)); };

   virtual const Rvector6 GetMJ2000State(const GmatTime &atTime);
   virtual const Rvector3 GetMJ2000Position(const GmatTime &atTime);
   virtual const Rvector3 GetMJ2000Velocity(const GmatTime &atTime);
   virtual const Rvector3 GetMJ2000Acceleration(const GmatTime &atTime);

   //   virtual std::string GetParameterText(const Integer id) const;
//   virtual Gmat::ParameterType
//                       GetParameterType(const Integer id) const;
//   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual void ClearLastStopTriggered();
   virtual void SetLastStopTriggered(const std::string &stopCondName);
   virtual const std::string GetLastStopTriggered();
   virtual bool WasLastStopTriggered(const std::string &stopCondName);
   
   virtual bool SetPrecisionTimeFlag(bool onOff);

   bool HasEphemPropagated();
   void HasEphemPropagated(bool tf);
   
   /** 
    * Start on a fix for bug 648; these methods are not currently used, but
    * are in place for use when the single step publishing issues are ready 
    * to be worked.
    */
   void         HasPublished(bool tf);
   bool         HasPublished();


   virtual bool			HasEphemerisFile();

protected:
   /// The spacecraft state
   GmatState         state;
   /// The space object's acceleration
   Rvector3          acceleration;

   /// true when a finite burn needs to be applied to this SpaceObject
   bool              isManeuvering;
   /// List of maneuvering members
   StringArray       maneuveringMembers;
   /// Reference SpacePoint for the data
   std::string       originName;
   /// Reference SpacePoint for the data
   SpacePoint        *origin;
   /// Flag indicating if the force model parms have changed
   bool              parmsChanged;
   /// The names of the last set of stopping conditions met
   StringArray       lastStopTriggered;
   /// Flag indicating if object has published data - not currently used
   bool              hasPublished;
   /// Flag indicating if the object has been propagated via an ephem
   bool					hasEphemPropagated;

   /// Enumerated parameter IDs   
   enum
   {
//      EPOCH_PARAM = SpacePointParamCount,
//      SpaceObjectParamCount
      SpaceObjectParamCount = SpacePointParamCount,
   };
//   /// Array of supported parameters
//   static const std::string PARAMETER_TEXT[SpaceObjectParamCount -
//                                           SpacePointParamCount];
//   /// Array of parameter types
//   static const Gmat::ParameterType PARAMETER_TYPE[SpaceObjectParamCount -
//                                                   SpacePointParamCount];
   
};

#endif // SpaceObject_hpp
