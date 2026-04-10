//$Id$
//------------------------------------------------------------------------------
//                                 Propagate
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Definition for the Propagate command class
 */
//------------------------------------------------------------------------------


#ifndef Propagate_hpp
#define Propagate_hpp

#include "PropagationEnabledCommand.hpp"
#include "ElementWrapper.hpp"
#include <list>


// Headers for the referenced classes
#include "Spacecraft.hpp"
#include "PropSetup.hpp"
#include "StopCondition.hpp"
#include "FormationInterface.hpp"


//#define DUMP_PLANET_DATA


/**
 * The Propagate Command Class
 * 
 * The Propagate command controls the integrators and force modeling, and uses 
 * those components to evolve the mission through time.  Propagation can proceed 
 * in one of several different modes: 
 * 
 *  - In Synchronized mode, epochs remain synchronized for multiple PropSetups.
 *  - In single step mode, the PropSetups take one integration step.  
 *  - In BackProp mode, integration moves SpaceObjects backwards in time.
 * 
 * Logic associated with stopping propagation is also part of the Propagate 
 * command.  It uses the StopCondition class and its embedded "not-a-knot" 
 * interpolator (see KnotAKnotInterpolator) to generate an estimated stop epoch, 
 * and then refines that epoch if necessary using a secant solver.
 */
class GMAT_API Propagate : public PropagationEnabledCommand
{
public:
   Propagate();
   virtual ~Propagate();
   Propagate(const Propagate &prp);
   Propagate&           operator=(const Propagate &prp);
   
   // Methods used for configuration
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   
   virtual bool         SetObject(const std::string &name,
                                  const UnsignedInt type,
                                  const std::string &associate = "",
                                  const UnsignedInt associateType =
                                  Gmat::UNKNOWN_OBJECT);
   virtual bool         SetObject(GmatBase *obj, const UnsignedInt type);
   virtual GmatBase*    GetGmatObject(const UnsignedInt type,
                                  const std::string objName = "");
   virtual void         ClearObject(const UnsignedInt type);
   
   // inherited from GmatBase
   virtual GmatBase*    Clone(void) const;
   
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode =
                                            Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   // Reference object accessor methods
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   
   // Parameter accessor methods
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray& 
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray& 
                        GetStringArrayParameter(const Integer id, 
                                                const Integer index) const;
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);

   virtual bool         GetPropStatus();
   
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
   // Methods used for interpreting the command
   virtual bool         InterpretAction();
//   virtual bool         Validate();
   virtual bool         AcceptsObjectType(UnsignedInt theType);
   
   // Methods used for setting ElementWrapper
   virtual const StringArray& 
                        GetWrapperObjectNameArray(bool completeSet = false);
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                                          const std::string &withName);
   virtual void         ClearWrappers();

   // Methods used for segment orbit colors
   virtual void         SetSegmentOrbitColor(UnsignedInt &intColor);
   virtual UnsignedInt  GetSegmentOrbitColor();
   virtual std::string  GetFirstSpaceObjectName();
   virtual void         SetOverrideSegmentColor(bool override);
   virtual bool         GetOverrideSegmentColor();
   virtual bool DoPropsHaveFiniteBurn();
   
   // Methods used for running the command
   virtual bool         Initialize();
   virtual void         FillFormation(SpaceObject *so, StringArray& owners,
                                      StringArray& elements);
   virtual GmatCommand* GetNext();
   virtual bool         Execute();
   virtual void         RunComplete();
   virtual GmatBase*    GetClone(Integer cloneIndex = 0);
   
protected:
   /// Name of the propagator setup(s) used in this command
   StringArray                  propName;
   /// The spacecraft associated with this propagation, grouped by propagator
   std::vector<StringArray *>   satName;
   /// Flag used to determine if the spacecraft are propagated coupled
   std::string                  currentPropMode;
   /// Frequency used to check for user interrupts of the run
   Integer                      interruptCheckFrequency;
   /// Flag that specifies if we are rejoining a run in progress
   bool                         inProgress;
   /// ID for the spacecraft epoch parameter
   Integer                      epochID;
   
   /// The stopping conditions
   std::vector<StopCondition *> stopWhen;
   /// The time step that we need to interpolate across
   Real                         stopInterval;
   /// Index to the first StopCondition that triggered the stop
   Integer                      stopTrigger;
   /// Array of triggers
   std::vector <StopCondition*> triggers;
   /// Full list of spacecraft propagating
   StringArray                 fullSatList;
   /// Names of the spacecraft used in the stopping conditions
   StringArray                  stopSatNames;
   /// String used in the stopping conditions
   StringArray                  stopNames;
   /// String used in the stopping goals
   StringArray                  goalNames;
   /// ElementWraper pointers of stopping conditions
   std::vector<ElementWrapper*> stopWrappers;
   /// ElementWraper pointers of stopping goals
   std::vector<ElementWrapper*> goalWrappers;
   
   /// The spacecraft used by the stopping conditions
   std::vector<SpaceObject *>   stopSats;
   /// The object array used in GetRefObjectArray()
   ObjectArray                  objectArray;
   
   // Parameters moved from Execute so that it can be reentrant
   // The following variables are defined in PropagationEnabledCommand
   ///// Time elapsed during this Propagate 
   //RealArray                    elapsedTime;
   ///// Start epoch for the step 
   //RealArray                    currEpoch;
   //std::vector<GmatTime>        currEpochGT;

   ///// The Propagation State Managers
   //std::vector<PropagationStateManager*>  psm;
   
   // Flags used to globally trigger STM or A-Matrix calculations
   /// Set to trigger all STM propagation
   bool                         propAllSTMs;
   /// Set to trigger all A-Matrix calculations
   bool                         calcAllAmatrices;
   /// Set to trigger all Covariance Matrix calculations
   bool                         calcAllCovariance;
   /// Flag used to indicate if propagators should reset step during initialization
   bool                         resetLoopDataFlag;

   // Members used to flag most recent detected stop, so we don't stop multiple 
   // times at the same point
   /// Flag used to indicate that the first step logic must be executed
   bool                         checkFirstStep;

   /// Allowed modes of propagation
   enum PropModes
   {
      INDEPENDENT,
      SYNCHRONIZED,
      BACK_PROP,
      PropModeCount 
   };

   ///// The state that is propagated
   //Real                    *state;
   ///// The J2000 body state that is propagated
   //Real                    *j2kState;


   /// Flag for stopping
   bool                    stopCondMet;
   /// Epoch used for stop
   Real                    stopEpoch;
   /// Required accuracy for stopping with stop conditions
   Real                    stopAccuracy;
   /// Required accuracy for stopping with stop conditions
   Real                    timeAccuracy;
   /// Tolerance used when determining if a triggered stop is the previous stop
   Real                    firstStepTolerance;
   /// Dimension used for (local) state vector
   Integer                 dim;

   // This variable had been declared in PropagationEnabeldCommand.hpp so it has to be removed here
   /// Expanded dimension used for (local) state vector adding dimension of spacecarfts' covariace and acceleration
   /// Integer                 expandedDim;

   /// Dimension used for (local) state vector's Cartesian elements
   Integer                 cartDim;
   /// Identifies when the command is in single step mode
   bool                    singleStepMode;
   /// Variable that tracks the current propagation mode
   PropModes               currentMode;
   /// Bracketing timesteps used in the bisection method
   Real                    stepBrackets[2];
   
   // Stopping condition parameter IDs
   /// Epoch on the stopping condition
   Integer                 stopCondEpochID;
   /// Epoch on the stopping condition
   Integer                 stopCondBaseEpochID;
   /// Epoch on the stopping condition
   Integer                 stopCondStopVarID;
   
   // Orbit color
   std::string segmentOrbitColorStr;
   UnsignedInt segmentOrbitColor;
   bool overrideSegmentColor;
   
   /// Parameter IDs
   enum
   {
      AVAILABLE_PROP_MODES = GmatCommandParamCount,
      PROP_COUPLED,
      INTERRUPT_FREQUENCY,
      STOP_ACCURACY,
      SAT_NAME,
      PROP_NAME,
      STOP_WHEN,
      PROP_FORWARD,
      PROP_ALL_STM,
      CALC_ALL_AMATRIX,
      CALC_ALL_COVARIANCE,
      ORBIT_COLOR,
      PropagateCommandParamCount,
   };

   /// Parameter ID Types
   static const Gmat::ParameterType
      PARAMETER_TYPE[PropagateCommandParamCount - GmatCommandParamCount];
   /// Parameter ID strings
   static const std::string
      PARAMETER_TEXT[PropagateCommandParamCount - GmatCommandParamCount];
   /// Array of allowed propagation modes
   static std::string      PropModeList[PropModeCount];

   //Moved to PropagationEnabledCommand for GMT-5101 fix (LOJ: 2015.05.14)
   // virtual void            SetNames(const std::string& name, 
   //                                  StringArray& owners, StringArray& elements);
   virtual void            CheckForOptions(Integer &loc, 
                                           std::string& generatingString);

   virtual void            AssemblePropagators(Integer &loc, 
                              std::string& generatingString);
   void                    FindSetupsAndStops(Integer &loc, 
                              std::string& generatingString,
                              StringArray &setupStrings, 
                              StringArray &stopStrings);
   void                    ParseSegmentColor(const std::string &colorString);
   void                    ConfigurePropSetup(std::string &setupDesc);
   void                    ConfigureStoppingCondition(std::string &stopDesc);
   void                    CleanString(std::string &theString, 
                              const StringArray *extras = NULL);
   virtual void            PrepareToPropagate();
   virtual void            PrepareStoppingConditions();
   virtual void            CheckStopConditions(Integer EpochID);
   virtual void            TakeFinalStep(Integer EpochID, Integer trigger);
   
   virtual bool            TakeAStep(Real propStep = 0.0);
   
   
//------------------------------------------------------------------------------
// MOVED TO PROPAGATIONENABLEDCOMMAND
//------------------------------------------------------------------------------
//   void                    AddTransientForce(StringArray *sats, ODEModel *p,
//                                 PropagationStateManager *propMan);
//   void                    ClearTransientForces();
   
   bool                    CheckFirstStepStop(Integer i);
   
   Real                    InterpolateToStop(StopCondition *sc);
   Real                    RefineFinalStep(Real secsToStep, 
                                           StopCondition *stopper);
   Real                    BisectFinalStep(StopCondition *stopper);
   Real                    BisectToStop(StopCondition *stopper);
   
   Real                    GetRangedParam(const Real value, const Real midpt, 
                                          const Integer type);
      
private:

   #ifdef DUMP_PLANET_DATA
      static std::ofstream        planetData;
      static CelestialBody        *body[11];
      static Integer              bodiesDefined;      
   #endif
    
};


#endif // Propagate_hpp
