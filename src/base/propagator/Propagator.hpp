//$Id$
//------------------------------------------------------------------------------
//                              Propagator
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
// *** File Name : Propagator.hpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 12/18/2002 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 09/23/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/09/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - virtual char* GetParameterName(const int parm) const to
//                                  virtual std::string GetParameterName(const int parm) const
//                              Additions:
//                                - GetParameterType()
//                                - GetParameterText()
//                                - GetParameterTypeString()
//                                - GetRealParameter()
//                                - SetRealParameter()
//                              Removals:
//                                - GetParameter()
//                                - SetParameter()
//                                - ParameterCount()
//
//                           : 10/15/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                - STEP_SIZE_PARAMETER to stepSizeParameter
//                              Removals:
//                                - static Real parameterUndefined
//                                - SetUndefinedValue()
//                                - GetParameterName(), replaced by GetParameterText()
//                              Additions:
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//
// **************************************************************************

#ifndef Propagator_hpp
#define Propagator_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "PhysicalModel.hpp"
#include "PropagatorException.hpp"      // Propagator Exceptions
#include "PropagationStateManager.hpp"


class GMAT_API Propagator : public GmatBase
{
public:
   Propagator(const std::string &typeStr,
              const std::string &nomme = "");
   virtual ~Propagator(void);

   Propagator(const Propagator&);
   Propagator& operator=(const Propagator&);

   virtual bool RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName);

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

   virtual bool GetBooleanParameter(const Integer id) const;
   virtual bool SetBooleanParameter(const Integer id, const bool value);
   virtual bool GetBooleanParameter(const Integer id, const Integer index) const;
   virtual bool SetBooleanParameter(const Integer id, const bool value,
                                    const Integer index);
   virtual bool GetBooleanParameter(const std::string &label) const;
   virtual bool SetBooleanParameter(const std::string &label, const bool value);
   virtual bool GetBooleanParameter(const std::string &label,
                                    const Integer index) const;
   virtual bool SetBooleanParameter(const std::string &label, const bool value,
                                    const Integer index);

   virtual bool Initialize();
   virtual void SetPhysicalModel(PhysicalModel *pPhysicalModel);
   virtual bool Step(Real dt);
   virtual void SetAsFinalStep(bool fs);

   virtual void Update(bool forwards = true);
   virtual void ResetInitialData();
   virtual void UpdateInitialData(bool dynamicOnly = false, bool updateEpoch = true);
   const Real* AccessOutState();

   virtual Integer GetPropagatorOrder() const;
   virtual bool UsesODEModel();
   std::string GetPropOriginName();
   SpacePoint* GetPropOrigin();

   // Methods that control behavior for analytic or interpolation evolution
   // operators
   virtual void SetPropStateManager(PropagationStateManager *sm);
   virtual Integer GetDimension();
   virtual Real* GetState();
   virtual Real* GetJ2KState();
   virtual void UpdateSpaceObject(Real newEpoch = -1.0);
   virtual void UpdateSpaceObjectGT(GmatTime newEpoch = -1.0);

   virtual void UpdateFromSpaceObject();
   virtual void RevertSpaceObject();
   virtual void BufferState();

   virtual Real GetTime();
   virtual void SetTime(Real t);

   virtual bool PropagatesForward();
   virtual void SetForwardPropagation(bool tf);
   virtual Real GetStepSize();
   virtual void SetStepSize(Real newStepSize);
   virtual void SetFollowUpStep(bool setYes);
   virtual void SetBurning(bool setYes);
   virtual void SetLastBurn(bool setYes);
   virtual bool GetFollowUpStep();
   virtual bool GetlastBurn();
   virtual bool IsBurning();
   virtual bool GetHasFiniteBurn();
   virtual void SetHasFiniteBurn(bool yesNo);
   virtual bool LoadSpans();
   virtual bool IsValidEpoch(GmatEpoch theTime);

   virtual void TurnDebug(bool debugFlag);

   virtual bool UsesErrorControl();
   bool FindTimeStep();
   bool SetNoiseStep();

   // Abstract methods

   //---------------------------------------------------------------------------
   // virtual bool Step(void)
   //---------------------------------------------------------------------------
   /**
    * Method to propagate by the default step
    * This pure virtual method provides the interface used to advance the state
    * by the amount specified in the stepSize parameter.  This is the core
    * propagation routine.
    * Adaptive step propagators will take the desired step if accuracy permits,
    * and then adjust the stepSize parameter to the estimated optimal size for
    * the next call.
    */
   //---------------------------------------------------------------------------
   virtual bool Step() = 0;

   //---------------------------------------------------------------------------
   // virtual bool RawStep(void)
   //---------------------------------------------------------------------------
   /**
    * Method to take a raw propagation step without error control
    */
   virtual bool RawStep() = 0;

   //---------------------------------------------------------------------------
   // virtual bool GetStepTaken()
   //---------------------------------------------------------------------------
   /**
    * Method to report the size of the most recent propagation step.
    */
   virtual Real                GetStepTaken() = 0;

   virtual bool RawStep(Real dt);

   enum
   {
      INITIAL_STEP_SIZE = GmatBaseParamCount, ///< Stepsize for the propagation
      AlwaysUpdateStepsize,
      PROCESS_NOISE_STEP,
      TIME_TO_NEXT_NOISE_STEP,
      PropagatorParamCount                    ///< Count of the parameters for this class
   };

   DEFAULT_TO_NO_CLONES

protected:

   // Start with the parameter IDs and associated strings
   static const std::string
      PARAMETER_TEXT[PropagatorParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[PropagatorParamCount - GmatBaseParamCount];

   static const Real STEP_SIZE_TOLERANCE;

   /// Size of the default time step
   Real stepSize;
   /// Buffer for the stepsize.
   Real stepSizeBuffer;
   /// Flag used to determine if the propagator need to reset the initial step
   bool resetInitialData;
   /// Flag to force stepsize initialization whenever the propagator initializes
   bool alwaysUpdateStepsize;
   /// Pointer to the input state vector
   Real *inState;
   /// Pointer to the location for the propagated state
   Real *outState;
   /// Number of parameters being propagated
   Integer dimension;
   /// Pointer to the information related to the physics of the system
   PhysicalModel *physicalModel;
   /// Flag used to detect if a finite burn occurs during a propagation
   bool hasFiniteBurn;
   /// Flag used to detect if the code is taking the last Propagate step
   bool finalStep;
   /// Flag used to detect if the step is a follow up to a shortened step by GetForceMaxStep
   bool followUpStep;
   // Flag to tell if it is currently inside a burn
   bool burning;
   // Flag to detect when the last burn happens inside a given step
   bool lastBurn;
   // Pieces for prop with origin code
   /// Name of the common J2000 body that the state providers all use
   std::string               j2kBodyName;
   /// Pointer to the J2000 body
   CelestialBody             *j2kBody;
   /// Name of the central body
   std::string                centralBody;
   /// Pointer to the central body
   SpacePoint                *propOrigin;

   /// Buffer for starting epoch for propagators that need it
   RealArray                  spanStart;
   /// Buffer for end epochs for propagators that need it
   RealArray                  spanEnd;
   /// Size of Noise Update Step
   Real                    noiseStepSize =0.0;
   /// Time to next noise step
   Real                    timeToNextNoiseStep;
   

   /// State accessor for derived analytic propagators; default returns nullptr
   virtual Real* GetAnalyticState();

   virtual void MoveToOrigin(Real newEpoch = -1.0);
   virtual void ReturnFromOrigin(Real newEpoch = -1.0);
   virtual void MoveToOriginGT(GmatTime newEpoch = -1.0);
   virtual void ReturnFromOriginGT(GmatTime newEpoch = -1.0);

   bool debug;
};

#endif // Propagator_hpp

