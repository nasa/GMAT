//------------------------------------------------------------------------------
//                             EphemerisPropagator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Created: Mar 26, 2010 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the EphemerisPropagator class
 */
//------------------------------------------------------------------------------


#ifndef EphemerisPropagator_hpp
#define EphemerisPropagator_hpp

#include "ephempropagator_defs.hpp"
#include "Propagator.hpp"
#include "Ephemeris.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton

/**
 * EphemerisPropagator is the base class for objects that model orbit evolution
 * through interpolation of data in an ephemeris file.
 */
class EPHEM_PROPAGATOR_API EphemerisPropagator : public Propagator
{
public:
   EphemerisPropagator(const std::string &typeStr, const std::string &name = "");
   virtual ~EphemerisPropagator();
   EphemerisPropagator(const EphemerisPropagator& ep);
   EphemerisPropagator& operator=(const EphemerisPropagator& ep);

   // Access methods for the scriptable parameters
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   // Access methods for the scriptable parameters
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         GetRealParameter(const Integer id, const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         SetRealParameter(const Integer id, const Real value,
                                         const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value, const Integer row,
                                         const Integer col);


   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);


   // Access methods derived classes can override on reference objects
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
//   virtual const ObjectTypeArray&
//                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
//   virtual GmatBase*    GetRefObject(const UnsignedInt type,
//                                     const std::string &name);
//   virtual GmatBase*    GetRefObject(const UnsignedInt type,
//                                     const std::string &name,
//                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
//   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
//   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

   virtual bool         UsesODEModel();
   virtual void         SetPropStateManager(PropagationStateManager *sm);
   virtual bool         Initialize();
   virtual bool         Step(Real dt);
   virtual bool         Step() = 0;

   virtual Integer      GetDimension();
   virtual Real*        GetState();
   virtual Real*        GetJ2KState();
   
   virtual void         UpdateSpaceObject(Real newEpoch = -1.0);
   virtual void         UpdateSpaceObjectGT(GmatTime newEpoch = -1.0);

   virtual void         UpdateFromSpaceObject();
   virtual void         RevertSpaceObject();
   virtual void         BufferState();

   virtual Real         GetTime();
   virtual void         SetTime(Real t);
   virtual void         SetSolarSystem(SolarSystem *ss);

   /// Types of initial epoch data supported
   enum StartEpochSource
   {
      FROM_SCRIPT,
      FROM_EPHEM,
      FROM_SPACECRAFT
   };

protected:
   /// The ephemeris used for propagation  (Note: Refactoring needed for some types)
   Ephemeris                  *theEphem;
   /// Step used to propagate through the ephemeris
   Real                       ephemStep;
   /// Format used for the start epoch data
   std::string                epochFormat;
   /// Start epoch
   std::string                startEpoch;
   /// Initial epoch
   Real                       initialEpoch;
   GmatTime                   initialEpochGT;
   /// Current epoch
   Real                       currentEpoch;
   GmatTime                   currentEpochGT;
   /// Current epoch - initial epoch (used to minimize accumulated error)
   Real                       timeFromEpoch;

   /// Start time on the ephem
   GmatEpoch                  ephemStart;
   /// End time on the ephem
   GmatEpoch                  ephemEnd;
   /// Flag used to stop a run if the epoch is outside of the ephem span
   bool                       stopOutsideOfEphem;

   /// Names of the objects that are propagated
   StringArray                propObjectNames;
   /// The propagated objects
   ObjectArray                propObjects;
   /// The (current) ephemeris file names, one per prop object
   StringArray                theEphems;

   /// The propagation state manager, used to manage the state
   PropagationStateManager    *psm;

   /// State vector for the latest propagated vector
   Real                       *state;
   /// Second state vector, in the J2000 frame
   Real                       *j2kState;
   /// Size of the most recent prop step
   Real                       stepTaken;
   /// Test strings describing the different types of start options
   StringArray                startOptions;
   /// Type of source used to obtain start epoch
   StartEpochSource           startEpochSource;
   /// Flag set to ensure initialization from ephemeris file is completed
   bool						  initialEphemEpochSet;
   /// Buffer that allows quick reversion to the previous state
   GmatState                  previousState;
   /// Direction of propagation
   Real                       stepDirection;

   ///The solar system that supplies the j2kBody and propOrigin
   SolarSystem                *solarSystem;

   /// Time converter singleton
   TimeSystemConverter        *theTimeConverter;

   /// Parameter IDs
   enum
   {
      EPHEM_STEP_SIZE = PropagatorParamCount,
      EPHEM_CENTRAL_BODY,
      EPHEM_EPOCH_FORMAT,
      EPHEM_START_EPOCH,
      EPHEM_START_A1MJD,
      EPHEM_END_A1MJD,
      EPHEM_START_OPTIONS,
      EphemerisPropagatorParamCount
   };

   /// EphemerisPropagator parameter types
   static const Gmat::ParameterType
         PARAMETER_TYPE[EphemerisPropagatorParamCount - PropagatorParamCount];
   /// EphemerisPropagator parameter labels
   static const std::string
         PARAMETER_TEXT[EphemerisPropagatorParamCount - PropagatorParamCount];

   GmatEpoch ConvertToRealEpoch(const std::string &theEpoch,
         const std::string &theFormat);
   virtual void UpdateState() = 0;
   virtual Real* GetAnalyticState();

   virtual void SetEphemSpan(const GmatEpoch start, const GmatEpoch);
   virtual void SetEphemSpan(Integer whichOne = 0);

   virtual void MoveToOrigin(Real newEpoch = -1.0);
   virtual void ReturnFromOrigin(Real newEpoch = -1.0);
   virtual void MoveToOriginGT(GmatTime newEpoch = -1.0);
   virtual void ReturnFromOriginGT(GmatTime newEpoch = -1.0);

};

#endif /* EphemerisPropagator_hpp */
