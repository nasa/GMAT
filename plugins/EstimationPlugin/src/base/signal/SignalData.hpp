//------------------------------------------------------------------------------
//                           SignalData
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
// Created: Feb 27, 2014
/**
 * Container for signal data
 */
//------------------------------------------------------------------------------

#ifndef SignalData_hpp
#define SignalData_hpp


#include "estimation_defs.hpp"
#include "gmatdefs.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"

#include "Rmatrix.hpp"

#include "Rvector6.hpp"
#include "GmatTime.hpp"

// Forward references
class SpacePoint;
class PropSetup;


/**
 * The SignalData class is a structure for communicating signal data information
 */
class ESTIMATION_API SignalData
{
public:
   SignalData();
   virtual ~SignalData();
   SignalData(const SignalData& sd);
   SignalData& operator=(const SignalData& sd);
   void CleanUp();

   // The data structures
   /// Name of the starting participant
   std::string transmitParticipant;
   /// Name of the end point participant
   std::string receiveParticipant;
   /// The starting participant
   SpacePoint *tNode;
   /// Flag indicating if the transmitter moved during light time iteration
   bool tMovable;
   /// The end point participant
   SpacePoint *rNode;
   /// Flag indicating if the transmitter moved during light time iteration
   bool rMovable;
   /// The propagator used for the transmitter, if used
   PropSetup *tPropagator;
   /// The propagator used for the receiver, if used
   PropSetup *rPropagator;
   /// Flag indicating if one of the participants is a ground station
   bool stationParticipant;

   /// Transmitter epoch
   GmatTime tPrecTime;
   /// Receiver epoch
   GmatTime rPrecTime;

   /// MJ2000Eq location of the transmit node
   Rvector3 tLoc;
   /// SSBMJ2000 state of the transmit node's origin
   Rvector6 tOStateSSB;
   /// Location of the transmit node in its coordinate system
   Rvector3 tLocTcs;
   /// MJ2000Eq location of the receive node
   Rvector3 rLoc;
   /// SSBMJ2000 state of the receive node's origin
   Rvector6 rOStateSSB;
   /// Location of the receive node in its coordinate system
   Rvector3 rLocRcs;
   /// MJ2000Eq transmitter velocity
   Rvector3 tVel;
   /// MJ2000Eq receiver velocity
   Rvector3 rVel;
   /// Displacement of origins from the transmit node at time tTime and receive node at time rTime
   Rvector3 j2kOriginSep;
   /// Relative velocity of the origin of the receive node at time rTime w.r.t. the origin of the transmit node at time tTime
   Rvector3 j2kOriginVel;
   /// The SSBMj2000 equatorial range vector from transmit at time tTime to receive node at time rTime
   Rvector3 rangeVecInertial;
   Rvector3 rangeVecI;
   /// Relative velocity of the receive node at time rTime w.r.t. the transmit node at time tTime
   Rvector3 rangeRateVecInertial;
   /// The range vector from transmit to receive node in obs coordinates
   Rvector3 rangeVecObs;
   /// The range rate vector from transmit to receive node in obs coordinates
   Rvector3 rangeRateVecObs;

   /// feasibility
   bool        feasibility;            // Flag indicate signal is feasible
   std::string feasibilityReason;      // reason why signal is unfeasible
   Real        feasibilityValue;       // Contain elevation angle

   // State Transition Matrices for derivatives of light time measurements
   /// The STM of transmit participant at transmit time t1
   Rmatrix tSTM;
   /// The STM of receive participant at receive time t2
   Rmatrix rSTM;

   /// The STM of transmit participant at measurement time tm
   ///(note that : measurement time tm is different from transmit time t1 and receive time t2 due to hardware delay)
   Rmatrix tSTMtm;
   /// The STM of receive participant at  measurement time tm
   ///(note that : measurement time tm is different from transmit time t1 and receive time t2 due to hardware delay)
   Rmatrix rSTMtm;

   /// Rotation matrix from J2K to transmitter coordinate system
   Rmatrix33 tJ2kRotation;
   /// Rotation matrix from J2K to receiver coordinate system
   Rmatrix33 rJ2kRotation;

   /// Rotation dot matrix from J2K to transmitter coordinate system
   Rmatrix33 tJ2kRotationDot;
   /// Rotation dot matrix from J2K to receiver coordinate system
   Rmatrix33 rJ2kRotationDot;

   /// Correction identifiers
   StringArray correctionIDs;
   /// Correction types
   StringArray correctionTypes;
   /// Correction data
   std::vector<Real> corrections;
   /// Flags for the corrections to use
   std::vector<bool> useCorrection;
   /// Flag for light time solution
   bool solveLightTime;

   /// Hardware delay associated with transmit participant
   Real tDelay;
   /// Hardware delay associated with receive participant
   Real rDelay;

   /// signal frequencies
   /// Frequency (MHz) of the received signal at tNode's transponder. For the first signal leg, arrivedFreq is not used due to tNode is a transmiter only.
   Real arriveFreq;
   // Transmit frequency (MHz) from tNode
   Real transmitFreq;
   // Receive frequency (MHz) at rNode
   Real receiveFreq;
   // Frequency used for media corrections. Sometimes used, sometimes other freq used.
   Real mediaCorrectionFrequency;
   //Frequency bias from Transmitter oscillator. Only used in TDRS 3L as of now
   Real transmitFreqOscillatorBias;
   //HORP height closest distance to body
   Real horpHeight;
   //Horp Angle angle between the transmitter and the reciever
   Real horpAngle;

   bool forTDRS;
   /// Linked list so separate signal paths are clear
   SignalData *next;
};

#endif /* SignalData_hpp */
