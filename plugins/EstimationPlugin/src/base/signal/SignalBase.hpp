//------------------------------------------------------------------------------
//                           SignalBase
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 9, 2014
/**
 * Base class for signals between two measurement participants
 */
//------------------------------------------------------------------------------

#ifndef SignalBase_hpp
#define SignalBase_hpp

#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "GmatState.hpp"
#include "MeasurementData.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "ProgressReporter.hpp"
#include "SignalData.hpp"
#include "SignalDataCache.hpp"
#include "GmatTime.hpp"
#include "RampTableData.hpp"

class PropSetup;


/**
 * Base class for signals between two measurement participants
 *
 * This class sets the nodes for measurement signals, and defines the interfaces
 * used by measurements when calculating signal data.
 *
 * SignalBase objects are arranged in a linked list, so that a full signal path
 * can be defined through the list.
 */
class ESTIMATION_API SignalBase: public GmatBase
{
public:
   SignalBase(const std::string &typeStr, const std::string &name = "");
   virtual ~SignalBase();
   SignalBase(const SignalBase& sb);
   SignalBase& operator=(const SignalBase& sb);

   DEFAULT_TO_NO_CLONES

   bool                 SetProgressReporter(ProgressReporter* reporter);

   bool                 SetTransmitParticipantName(std::string name);
   bool                 SetReceiveParticipantName(std::string name);

   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   SignalBase           *GetStart(bool epochIsAtEnd);
   virtual SignalBase*  GetNext();
   virtual bool         Add(SignalBase *signalToAdd);

   virtual void         SetPropagator(PropSetup *propagator,
                                      GmatBase *forObj = NULL);

   virtual bool         Initialize();
   virtual void         InitializeSignal(bool chainForwards = false);

   virtual bool         ModelSignal(const GmatTime atEpoch, bool forSimulation, 
                                    bool EpochAtReceive = true) = 0;

   virtual const std::vector<RealArray>&
                        ModelSignalDerivative(GmatBase *obj,
                                              Integer forId, 
                                              GmatTime *measTime,
                                              bool forRangeRate = false) = 0;

   /// This fucntion is used to compute signal frequency on each signal leg
   virtual bool         SignalFrequencyCalculation(std::vector<RampTableData>* rampTB, Real uplinkFrequency = 0.0, GmatTime measTime = 0.0) = 0;


   /// This function is used to add media correction to measurement model
   virtual void         AddCorrection(const std::string& modelName,
                             const std::string& mediaCorrectionType) = 0;

   virtual bool         MediaCorrectionCalculation(std::vector<RampTableData>* rampTB = NULL) = 0;
   /// This function is used to calculate total hardware delay
   virtual bool         HardwareDelayCalculation() = 0;

   virtual std::string  GetPathDescription(bool fullList = true);

   SignalData*          GetSignalDataObject();
   void                 SetSignalData(const SignalData& newData);
   bool                 IsSignalFeasible();
   virtual void         UsesLighttime(const bool tf);

   bool                 StepParticipant(Real stepToTake,
                                              bool forTransmitter);
   void                 MoveToEpoch(const GmatTime theEpoch,
                                          bool epochAtReceive,
                                          bool moveAll = true);

   void                SetIonosphereCache(SignalDataCache::SimpleSignalDataCache* cache);

   void                SetStrandId(unsigned long id);

   void                SetupHorp(Real height, Real angle, bool useOblateness, bool force);

protected:
   /// The next node in the list of signals (NULL at the end of the list)
   SignalBase                 *next;
   /// The previous node in the list of signals (NULL at the start of the list)
   SignalBase                 *previous;

   /// State data for the transmitting participant
   GmatState                  tState;
   /// State data for the receiving participant
   GmatState                  rState;
   // Participant data that can be passed to callers
   SignalData                 theData;
   /// Participant data that can be passed to callers
   std::vector<RealArray>     theDataDerivatives;

   /// Participant data rate that can be passed to callers
   std::vector<RealArray>     theDataRateDerivatives;

   // Member data used
   // Coordinate Systems
   /// Coordinate system of the transmitting participant
   CoordinateSystem           *tcs;
   /// Coordinate system of the receiving participant
   CoordinateSystem           *rcs;
   /// Coordinate system in which observation is expressed
   CoordinateSystem           *ocs;
   /// J2K coordinate system
   CoordinateSystem           *j2k;

   // Storage buffers

   /// Epoch of most recent calculation
   GmatTime                   satPrecEpoch;
   ///// Parameter ID used to retrieve internal epoch data
   //Integer                    satEpochID;                            // unused variable
   /// Rotation matrix from receiver to J2K (Identity by default)
   Rmatrix33                  R_j2k_Receiver;
   /// Rotation matrix from transmitter to J2K (Identity by default)
   Rmatrix33                  R_j2k_Transmitter;
   /// Rotation matrix from receiver to observation frame (Identity by default)
   Rmatrix33                  R_Obs_Receiver;
   /// Rotation matrix from F1 to observation frame (Identity by default)
   Rmatrix33                  R_Obs_Transmitter;
   /// Rotation matrix from J2K to observation frame (Identity by default)
   Rmatrix33                  R_Obs_j2k;
   /// Rotation Dot matrix from J2K to observation frame (Identity by default)
   Rmatrix33                  RDot_Obs_j2k;
   /// Rotation Dot matrix from F2 to observation frame (Identity by default)
   Rmatrix33                  RDot_Obs_Receiver;
   /// Rotation Dot matrix from transmitter to observation frame (Identity by default)
   Rmatrix33                  RDot_Obs_Transmitter;
   /// Feasibility for the signal, based on information from the signal nodes
   bool                       signalIsFeasible;
   /// Flag triggering inclusion of light time solution
   bool                       includeLightTime;

   /// Toggle used to turn on the height of ray path (HORP) algorithm
   bool useHorp;
   /// Toggle used to force on the height of ray path (HORP) algorithm for all bodies
   bool forceHorp; 
   /// The minimum altitude required for the HORP test to indicate a good signal
   Real horpHeight;
   /// The central angle used to determine if a ray passes behind a body
   Real horpAngle;
   /// Toggle used for an oblate central body rather than spherical
   bool horpOblateness;


   // Utility vectors and matrices
   /// Identity matrix, 3x3
   static const Rmatrix33     I33;
   /// Zero matrix, 3x3
   static const Rmatrix33     zero33;
   /// zero-vector
   static const Rvector3      zeroVector;
   /// [1,0,0]T
   static const Rvector3      xUnit;
   /// [0,1,0]T
   static const Rvector3      yUnit;
   /// [0,0,1]T
   static const Rvector3      zUnit;

   // Utility objects
   /// Converter to convert between F1, F2, Fo frames
   CoordinateConverter        converter;
   /// the Solar System in use
   SolarSystem                *solarSystem;

   /// The progress reporter that allows user control over the data logging
   ProgressReporter           *navLog;
   /// The current logging level for signals
   UnsignedInt                logLevel;

   /// The light time cache 
   SignalDataCache::SimpleSignalDataCache *ionosphereCache;

   /// The strandID
   unsigned long strandId;

   void                       SetPrevious(SignalBase *prev);

   // Some useful methods
   // Coordinate System methods based on methods in GeometricMeasurement
   virtual void               CalculateRangeVectorInertial();
   virtual void               CalculateRangeVectorObs();
   virtual void               CalculateRangeRateVectorObs();
   virtual void               UpdateRotationMatrix(Real atEpoch,
                                    const std::string &whichOne = "All");

   virtual Real               GetCrDerivative(GmatBase *forObj);
   virtual Real               GetCdDerivative(GmatBase *forObj);
   virtual Real               GetADSFDerivative(GmatBase *forObj); // Atmospheric Density Scale Factor Solve For
   virtual Real               GetTSFDerivative(GmatBase *forObj, const std::string &paramName);     // Thrust Scale Factor Solve For
   virtual Real               GetParamDerivative(GmatBase *forObj, std::string paramName, GmatBase *objPtr = NULL);
   virtual void               GetCDerivativeVector(GmatBase *forObj, Rvector &deriv, const std::string &solveForType);
   virtual void               GetRangeDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
                                 Rvector &deriv);
   virtual void               GetRangeVectorDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
                                 Rmatrix& derivMatrix);

   virtual void               GetRangeRateDerivative(GmatBase *forObj, bool wrtR, bool wrtV, Rvector &deriv);
   virtual void               GetRangeRateVectorDerivative(GmatBase *forObj, bool wrtR, bool wrtV, 
                                 Rmatrix& derivMatrix);

   Integer                    GetParmIdFromEstID(Integer forId, GmatBase *obj);
   //bool                       StepParticipant(Real stepToTake,                 // make this function public
   //                                           bool forTransmitter);
};

#endif /* SignalBase_hpp */
