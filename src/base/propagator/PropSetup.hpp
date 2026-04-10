//$Id$
//------------------------------------------------------------------------------
//                              PropSetup
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/10/15
//
/**
 * Declares propagator setup operations.
 */
//------------------------------------------------------------------------------
#ifndef PropSetup_hpp
#define PropSetup_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "PropagationStateManager.hpp"

// Forward references
class PhysicalModel;
class ODEModel;
class Propagator;

/**
 * The propagator subsystem interface
 * 
 * PropSetup is a container class that acts as the interface between the 
 * propagation subsystem and the rest of GMAT.  The core elements of the class 
 * are the Propagator, a PropagationStateManager, and, if needed, an ODEModel.  
 * 
 * The PropagationStateManager (PSM) is the interface between GMAT objects that 
 * move in time in the simulation and the propagation subsystem.  The PSM holds 
 * the data that is needed to build the state data that is propagated, and to
 * configure the corresponding ODEModel for propagators that perform numerical
 * integration.  The PSM retrieves current values for the elements that need 
 * propagation -- examples are the orbital state of a Spacecraft, the State 
 * Transition Matrix, and tank mass during a finite burn -- and constructs a 
 * vector that will change as the epoch changes in the simulation.  After a 
 * propagation step has been taken, the PSM is the interface used to update 
 * GMAT's objects with the new state data.
 * 
 * The Propagator performs the actual evolution of the state, changing the 
 * values in the state vector to match changes in the simulation epoch.  GMAT
 * supports three classes of propagators: Numerical Integrators, Analytic 
 * Propagators (not yet implemented), and ephemeris based propagators (not yet
 * implemented).  Of these three classes, the numerical integrators require 
 * additional configuration in the PropSetup so that the differential equations 
 * that are integrated are constructed consistently with the propagation state 
 * vector.  The differential equations are managed in the ODEModel.
 * 
 * GMAT's ODEModel class is a PhysicalModel that collects other PhysicalModel
 * objects and adds up their contributions, generating the total contribution to 
 * an integrator.  In other words, the ODEModel instance is a container class 
 * that collects individual objects that supply contributions to the total
 * differential equation, and adds these contributions together, generating the
 * superposition needed by the integrator.  The ODEModel places these totals 
 * into a state vector supplied by the PropagationStateManager.  The ODEModel is
 * responsible for ensuring that each contribution is assembled in the correct
 * element of the state vector. 
 */
class GMAT_API PropSetup : public GmatBase
{
public:

   PropSetup(const std::string &name);
   PropSetup(const PropSetup &ps);
   PropSetup& operator= (const PropSetup &ps); 
   virtual ~PropSetup();
   
   bool                 IsInitialized();
   Propagator*          GetPropagator();
   ODEModel*            GetODEModel();
   PropagationStateManager*            
                        GetPropStateManager();
   void                 SetPropagator(Propagator *propagator,
                                      bool fromGUI = false);
   void                 SetODEModel(ODEModel *odeModel);
   
   void                 AddForce(PhysicalModel *force);
   PhysicalModel*       GetForce(Integer index);
   Integer              GetNumForces();

   void                 AddPropObject(GmatBase* sat);
   bool                 RemovePropObject(GmatBase* sat);
   void                 PrepareInternals();
   
   // inherited from GmatBase
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   
   virtual GmatBase*    Clone(void) const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         HasRefObjectTypeArray();
   virtual const        ObjectTypeArray& GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual GmatBase*    GetOwnedObject(Integer whichOne);
   virtual bool         IsOwnedObject(Integer id) const;
   
   virtual const std::string*
                        GetParameterList() const;   
   virtual Integer      GetParameterCount() const;
   
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const Integer id,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value,
                                            const Integer index);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value,
                                            const Integer index);

//   virtual std::string  GetStringParameter(const Integer id,
//                                           const Integer index) const;
//   virtual bool         SetStringParameter(const Integer id,
//                                           const std::string &value,
//                                           const Integer index);
//   virtual std::string  GetStringParameter(const std::string &label,
//                                           const Integer index) const;
//   virtual bool         SetStringParameter(const std::string &label,
//                                           const std::string &value,
//                                           const Integer index);

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   
   virtual bool         Initialize();
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   virtual bool HasLocalClones();
   virtual void UpdateClonedObject(GmatBase *obj);
   virtual void UpdateClonedObjectParameter(GmatBase *obj,
                                            Integer updatedParameterId);

   virtual bool SetPrecisionTimeFlag(bool onOff);


private:
   
   bool mInitialized;
   bool mMcsCreated;
   bool includeODEModelInGenString;
   std::string mPropagatorName;
   std::string mODEModelName;
   Propagator *mPropagator;
   ODEModel *mODEModel;
   PropagationStateManager psm;
   
   void    ClonePropagator(Propagator *prop);
   void    CloneODEModel(ODEModel *fm);
   void    DeleteOwnedObject(Integer id, bool forceDelete = false);
   Integer GetOwnedObjectId(Integer id, UnsignedInt objType) const;
   
   enum
   {
      ODE_MODEL = GmatBaseParamCount,
      PROPAGATOR,
      
      // owned object parameters
      INITIAL_STEP_SIZE,
      ACCURACY,
      ERROR_THRESHOLD,
      SMALLEST_INTERVAL,
      MIN_STEP,
      MAX_STEP,
      MAX_STEP_ATTEMPTS,
      LOWER_ERROR,
      TARGET_ERROR,
      STOP_IF_ACCURACY_VIOLATED,
      ANALYTIC_STEPSIZE,
      ANALYTIC_CENTRALBODY,
      ANALYTIC_EPOCHFORMAT,
      ANALYTIC_STARTEPOCH,
      BULIRSCH_MINIMUMREDUCTION,
      BULIRSCH_MAXIMUMREDUCTION,
      BULIRSCH_MINIMUMTOLERANCE,

      PropSetupParamCount
   };
   
   static const std::string
      PARAMETER_TEXT[PropSetupParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[PropSetupParamCount - GmatBaseParamCount];
   
};

#endif // PropSetup_hpp
