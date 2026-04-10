//------------------------------------------------------------------------------
//                         SPICEPropagator.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Darrel Conway
// Created: 2019/09/26
//
// Copyright (c) 2019, Thinking Systems, Inc.
// All rights reserved

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Edited by Jairus Elarbee 10/05/2021 
//   -Removed ICRF to FK5 conversion and added TEME conversion function
//   -Added TEME Debug flag
// Edited by Jairus Elarbee 11/11/2021 
//   -Added additional parameters needed for SPICE v66 update
//   -Removed TEME functions

/**
 *  Declaration code for the SPICEPropagator class.
 */


#ifndef SPICEPropagator_hpp
#define SPICEPropagator_hpp

#include "tleprop_defs.hpp"
#include "Factory.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "Propagator.hpp"
#include "TLEData.hpp"

/**
   A Two-Line Element propagator built using the cspice implementation

   Notes from the SPICE docs/files:

   Suppose you have a set of two-line elements and an array containing
   the related geophysical constants necessary to evaluate a state.
   The example below shows how you can use this routine together with
   the routine EV2LIN to propagate a state to an epoch of interest.
 
   The parameters below are used in the propagator code in the cspice
   library, set as the array GEOPHS required by EV2LIN.

      J2  --- location of J2
      J3  --- location of J3
      J4  --- location if J4
      KE  --- location of KE = sqrt(GM) in eart-radii**1.5/MIN
      QO  --- location of upper bound of atmospheric model in KM
      SO  --- location of lower bound of atmospheric model in KM
      ER  --- location of earth equatorial radius in KM.
      AE  --- location of distance units/earth radius
 */
class TLE_PROPAGATOR_API SPICEPropagator : public Propagator
{
public:
   SPICEPropagator(const std::string &withName);
   ~SPICEPropagator();
   SPICEPropagator(const SPICEPropagator &sgp);
   SPICEPropagator& operator=(const SPICEPropagator &sgp);

   virtual GmatBase* Clone() const;

   // Parameter accessor methods -- overridden from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool IsParameterReadOnly(const Integer id) const;
   virtual bool IsParameterReadOnly(const std::string &label) const;

   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);

   virtual Real GetRealParameter(const Integer id, const Integer index) const;
   virtual Real GetRealParameter(const Integer id, const Integer row,
         const Integer col) const;
   virtual Real SetRealParameter(const Integer id, const Real value,
         const Integer index);
   virtual Real SetRealParameter(const Integer id, const Real value,
         const Integer row, const Integer col);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const char *value,
                                           const Integer index);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const char *value);
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

   // Initialization setup methods
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);

   virtual bool         UsesODEModel();
   virtual void         SetPropStateManager(PropagationStateManager *sm);

   virtual bool         Initialize();

   virtual bool         Step();
   virtual bool         Step(Real dt);
   virtual bool         RawStep();
   virtual Real         GetStepTaken();
   Real                 GetTime();

   virtual Real*          AccessOutState();

   virtual Integer      GetDimension() { return dimension; }
   virtual Real*        GetState() { return state; }
   virtual Real*        GetJ2KState() { return j2kState; }

   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void         UpdateSpaceObject(Real newEpoch = -1.0);
   virtual void         UpdateFromSpaceObject();
   virtual void         RevertSpaceObject();
   virtual void         BufferState();
   


protected:
   /// The configuration file name
   std::string configFileName;

   /// Structure used for the TLE based spacecraft members
   class PropObject
   {
   public:
      PropObject(const std::string &name, CoordinateConverter *cvt);
      ~PropObject();
      PropObject(const PropObject& po);
      PropObject& operator=(const PropObject& po);

      bool SetBaseCS(CoordinateSystem *bcs);
      bool Initialize(const GmatEpoch toTime, double *params);

      /// Name of the spacecraft
      std::string satName;
      /// Spacecraft used for this element
      Spacecraft *theSat;
      /// Coordinate system used by the Spacecraft
      CoordinateSystem *satCoords;
      /// TLE Inputs for SPICE v66
      double inputs[32];
      /// Base TLE epoch, A.1 Mod Julian
      GmatEpoch a1epoch;
      /// Offset used to sync times with GMAT, in seconds
      Real timeOffset;
      /// TLE spacecraft name, used to find the elements in the file
      std::string tleName;
      /// TLE data sets for the propagated spacecraft
      TLEData tleSettings;

      /// Owning object's CS converter
      CoordinateConverter *cconverter;
      /// Internal CS
      CoordinateSystem *baseCS;
   };

//   /// The GMAT Spacecraft used for the propagation -- BASE CLASS?
//   std::string satName;
//   /// The spacecraft that we propagate -- BASE CLASS?
//   Spacecraft *theSpacecraft;


   /// Array of parameters used for the SGP4 propagator, in CSPICE units
   double params[8];

   /// Names of the objects that are propagated
   StringArray                propObjectNames;
   /// The propagated objects
   ObjectArray                propObjects;
   /// TLE settings for the propagated objects
   std::vector<PropObject>    SPICEPropObjects;

   /// The propagation state manager, used to manage the state
   PropagationStateManager    *psm;

   /// CS converter to set Spacecraft states
   CoordinateConverter        cconverter;
   /// GMAT Internal CS
   CoordinateSystem           *baseCS;

   /// State vector for the latest propagated vector
   Real                       *state;
   /// Second state vector, in the J2000 frame
   Real                       *j2kState;
   /// Initial epoch from the spacecraft, in A.1 time
   Real                       initialEpoch;
   /// Initial epoch in TLE time (sec from J2000 epoch); matches initialEpoch
   Real                       initialTleEpoch;
   /// Current A1ModJulian epoch
   Real                       currentEpoch;
   /// Time from initial epoch, in seconds (used to minimize accumulated error)
   Real                       timeFromEpoch;
   /// Step taken for the propagation, set after calling a Step() method
   Real                       stepTaken;

   /// Buffer that allows quick reversion to the previous state
   GmatState                  previousState;

   /// Parameter IDs
   enum
   {
      STEPSIZE = PropagatorParamCount,
      // Optional file to override default TLE parameters
      CONFIG_FILENAME,
      // Optional SPICE/SPG4 parameter settings to override settings
      J2,    // J2 value 
      J3,    // J3 value 
      J4,    // J4 value 
      KE,    // sqrt(mu) in earth-radii**1.5/MIN
      QO,    // Upper bound of atmospheric model in KM 
      SO,    // Lower bound of atmospheric model in KM 
      ER,    // Earth equatorial radius in KM. 
      AE,    // Distance units/earth radius 
      SPICEPropParamCount
   };

   /// SPICEPropagator parameter types
   static const Gmat::ParameterType
         PARAMETER_TYPE[SPICEPropParamCount - PropagatorParamCount];
   /// SPICEPropagator parameter labels
   static const std::string
         PARAMETER_TEXT[SPICEPropParamCount - PropagatorParamCount];


   virtual void UpdateState();
   bool SyncEpochs(bool satSource);
};

#endif
