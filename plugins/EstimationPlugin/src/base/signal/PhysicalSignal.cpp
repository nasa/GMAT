//------------------------------------------------------------------------------
//                           PhysicalSignal
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
 * Class used to model signals between two participants
 */
 //------------------------------------------------------------------------------

#include "PhysicalSignal.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include "PropSetup.hpp"
#include "GroundstationInterface.hpp"
#include "Barycenter.hpp"
#include "Spacecraft.hpp"
#include "Transmitter.hpp"
#include "Transponder.hpp"
#include "Receiver.hpp"
#include "ODEModel.hpp"
#include "Propagator.hpp"
#include "ErrorModel.hpp"
#include "StringUtil.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"

#include <sstream>                  // For stringstream


//#define DEBUG_EXECUTION
//#define DEBUG_LIGHTTIME
//#define SHOW_DATA
//#define DEBUG_FEASIBILITY
//#define DEBUG_DERIVATIVES
//#define DEBUG_TIMING
//#define DEBUG_CONSTRUCTION
//#define DEBUG_MEASUREMENT_CORRECTION
//#define DEBUG_IONOSPHERE_MEDIA_CORRECTION
//#define DEBUG_TROPOSPHERE_MEDIA_CORRECTION
//#define DEBUG_RELATIVITY_CORRECTION
//#define DEBUG_RANGE_CALCULATION
//#define DEBUG_ETTAI_CORRECTION
//#define DEBUG_API
//#define DEBUG_SCTOSC

#ifdef DEBUG_API
#include <fstream>
std::ofstream apipsFile;
bool apipsFileOpen = false;
#endif



//------------------------------------------------------------------------------
// PhysicalSignal(const std::string &typeStr, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param typeStr The object type
 * @param name Name of the new object
 */
 //------------------------------------------------------------------------------
PhysicalSignal::PhysicalSignal(const std::string &typeStr,
   const std::string &name) :
   SignalBase                    (typeStr, name),
   physicalSignalInitialized     (false),
   troposphere                   (NULL),
   ionosphere                    (NULL),
   useRelativity                 (false),
   useETTAI                      (false)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PhysicalSignal:: default construction\n");
#endif
   rampTable   = NULL;
   beginIndex  = 0;
   endIndex    = 0;
   
   blockedByOccultation = false;
   blockedByHORP        = false;

   objectTypeNames.push_back("PhysicalSignal");
#ifdef DEBUG_API
   if (!apipsFileOpen)
   {
      apipsFile.open("PhysicalSignal_API.txt");
      apipsFileOpen = true;
   }
   apipsFile << "Physical Signal class debug for the API\n" << std::endl;
#endif

}


//------------------------------------------------------------------------------
// ~PhysicalSignal()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
 //------------------------------------------------------------------------------
PhysicalSignal::~PhysicalSignal()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PhysicalSignal:: destruction\n");
#endif

   if (troposphere != NULL)
      delete troposphere;

   //   if (ionosphere != NULL)
   //      delete ionosphere;
}


//------------------------------------------------------------------------------
// PhysicalSignal(const PhysicalSignal& gs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ps Physical signal copied to make a new one
 */
 //------------------------------------------------------------------------------
PhysicalSignal::PhysicalSignal(const PhysicalSignal& ps) :
   SignalBase                 (ps),
   physicalSignalInitialized  (false),
   useRelativity              (ps.useRelativity),
   relCorrection              (ps.relCorrection),
   useETTAI                   (ps.useETTAI),
   troposphere                (NULL),
   ionosphere                 (NULL)
{

   blockedByOccultation = false;
   blockedByHORP = false;
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PhysicalSignal:: copy construction\n");
#endif
}


//------------------------------------------------------------------------------
// PhysicalSignal& operator=(const PhysicalSignal& ps)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ps PhysicalSignal that provides parameters for this one
 *
 * @return This signal set to match gs
 */
 //------------------------------------------------------------------------------
PhysicalSignal& PhysicalSignal::operator=(const PhysicalSignal& ps)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PhysicalSignal::operator =\n");
#endif

   if (this != &ps)
   {
      SignalBase::operator=(ps);

      physicalSignalInitialized  = false;
      useRelativity              = ps.useRelativity;

      relCorrection              = ps.relCorrection;
      useETTAI                   = ps.useETTAI;
   }
   blockedByOccultation = false;
   blockedByHORP = false;

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* PhysicalSignal::Clone() const
//------------------------------------------------------------------------------
/**
 * Cloning method used to make a replica of a PhysicalSignal.
 *
 * @return A new PhysicalSignal configured like this one.
 */
 //------------------------------------------------------------------------------
GmatBase* PhysicalSignal::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PhysicalSignal::Clone()\n");
#endif

   return new PhysicalSignal(*this);
}


//------------------------------------------------------------------------------
// void InitializeSignal()
//------------------------------------------------------------------------------
/**
 * Validates that everything needed is in place for the signal processing
 *
 * This method checks that all of the reference objects and object clones are
 * in place, and that they are initialized and ready to do the work required for
 * the signal computations.
 */
 //------------------------------------------------------------------------------
void PhysicalSignal::InitializeSignal(bool chainForwards)
{
   if (!physicalSignalInitialized)
   {
      // Initialize signal base
      SignalBase::InitializeSignal(chainForwards);
      physicalSignalInitialized = true;
   }
}


//------------------------------------------------------------------------------
// bool ModelSignal(const GmatEpoch atEpoch, bool forSimulation, bool epochAtReceive)
//------------------------------------------------------------------------------
/**
 * Models the signal
 *
 * The call here requires that the raw data in the measurement data is set
 * correctly when the call is made.  Specifically, the call into this method
 * Requires the following setup prior to the call (a t prefix refers to the
 * signal transmitting participant; r to the signal receiving participant):
 *
 *    theData.tTime: For Spacecraft, set to the epoch matching the state known
 *                   in the associated propagator.  For groundstations, set to
 *                   the epoch of the receiver.  Station to station signals are
 *                   not supported (unless this piece is changed)
 *
 *    theData.rTime: For Spacecraft, set to the epoch matching the state known
 *                   in the associated propagator.  For groundstations, set to
 *                   the epoch of the receiver.  Station to station signals are
 *                   not supported (unless this piece is changed)
 *
 * The data needed in theData.tLoc, theData.tVel, theData.rLoc, and theData.rVel
 * are populated during execution of this method.
 *
 * @param atEpoch The base epoch of the signal.  This is the epoch at the node
 *                selected by epochAtReceive.
 * @param forSimulation  true if it runs for siluation, false if it runs for estimation
 * @param epochAtReceive true if the receive node is fixed in time, false if the
 *                       transmit node is fixed
 *
 * @return true if the signal was modeled, false if not
 */
 //------------------------------------------------------------------------------
bool PhysicalSignal::ModelSignal(const GmatTime atEpoch, bool forSimulation, bool epochAtReceive)
{
#ifdef DEBUG_API
   if (apipsFileOpen)
   {
      GmatTime aep = atEpoch;
      apipsFile << "Entered PhysicalSignal::ModelSignal(" << (aep.GetMjd())
         << ", " << epochAtReceive << ") for path "
         << GetPathDescription(false) << std::endl;
   }
#endif

   bool retval = false;

   //satPrecEpoch = atEpoch;
   if (epochAtReceive)
   {
      // time tag is at the end of signal path
      if (next == NULL)
      {
         // for the last signal leg, rDelay = hardware delay
         //satPrecEpoch = atEpoch - theData.rDelay/GmatTimeConstants::SECS_PER_DAY;
         satPrecEpoch = atEpoch; satPrecEpoch.SubtractSeconds(theData.rDelay);
      }
      else
      {
         // for other leg, rDelay = 1/2 hardware delay
         //satPrecEpoch = atEpoch - 2*theData.rDelay/GmatTimeConstants::SECS_PER_DAY;
         satPrecEpoch = atEpoch; satPrecEpoch.SubtractSeconds(2 * theData.rDelay);
      }
   }
   else
   {
      // time tag is at the beginning of signal path
      if (previous == NULL)
      {
         // for the first signal leg, tDelay = hardware delay
         //satPrecEpoch = atEpoch + theData.tDelay/GmatTimeConstants::SECS_PER_DAY;
         satPrecEpoch = atEpoch; satPrecEpoch.AddSeconds(theData.tDelay);
      }
      else
      {
         // for other leg, tDelay = 1/2 hardware delay
         //satPrecEpoch = atEpoch + 2*theData.tDelay/GmatTimeConstants::SECS_PER_DAY;
         satPrecEpoch = atEpoch; satPrecEpoch.AddSeconds(2 * theData.tDelay);
      }
   }


   relCorrection = 0.0;
   ettaiCorrection = 0.0;                                                                       // unit: km

#ifdef DEBUG_EXECUTION
   MessageInterface::ShowMessage("ModelSignal(%s, %s) called\n", satPrecEpoch.ToString().c_str(),
      epochAtReceive ? "with fixed Receiver" : "with fixed Transmitter");

   MessageInterface::ShowMessage("Modeling %s -> %s\n",
      theData.transmitParticipant.c_str(),
      theData.receiveParticipant.c_str());

   MessageInterface::ShowMessage("tPrecTime = %s, rPrecTime = %s satPrecEpoch = "
      "%s\n", theData.tPrecTime.ToString().c_str(), theData.rPrecTime.ToString().c_str(), satPrecEpoch.ToString().c_str());
#endif

#ifdef DEBUG_RANGE_CALCULATION 
   GmatTime tm = atEpoch;
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("   ++++   Range, relativity correction, and ET-TAI correction calculation for leg from %s to %s at fixed time %s A1Mjd:\n",
      theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str(), tm.ToString().c_str());
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
#endif

   if (!isInitialized)
   {
#ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("   Calling signal initialization\n");
#endif
      InitializeSignal(!epochAtReceive);
   }

   if (isInitialized)
   {
#ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("   Signal initialized; Computing data\n");
#endif

#ifdef DEBUG_API
      if (apipsFileOpen)
      {
         GmatTime aep = atEpoch;
         apipsFile << "   Moving to epoch " << satPrecEpoch.GetMjd() << std::endl;
      }
#endif
      // 1. First make sure we start at the desired epoch
      MoveToEpoch(satPrecEpoch, epochAtReceive, true);
#ifdef DEBUG_API
      if (apipsFileOpen)
      {
         apipsFile << "   Calculating Range" << std::endl;
      }
#endif
      CalculateRangeVectorInertial();
      Real geoRange = theData.rangeVecInertial.GetMagnitude();
#ifdef DEBUG_API
      if (apipsFileOpen)
      {
         apipsFile << "   Range is " << geoRange << std::endl;
      }
#endif


      // Build the other data vectors
      CalculateRangeVectorObs();
      CalculateRangeRateVectorObs();

#ifdef DEBUG_API
      if (apipsFileOpen)
      {
         apipsFile << "   Additional Range vectors calculated" << std::endl;
      }
#endif


#ifdef DEBUG_RANGE_CALCULATION 
      MessageInterface::ShowMessage("   1. Compute Range Vector before light time correction for the Leg from <TNode = %s> to <RNode = %s>:\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
      MessageInterface::ShowMessage("      . %s state in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf)km at time tT= %s\n", theData.tNode->GetName().c_str(), tcs->GetOriginName().c_str(), theData.tLoc[0], theData.tLoc[1], theData.tLoc[2], theData.tVel[0], theData.tVel[1], theData.tVel[2], theData.tPrecTime.ToString().c_str());
      MessageInterface::ShowMessage("      . %s state in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf)km at time tR = %s\n", theData.rNode->GetName().c_str(), rcs->GetOriginName().c_str(), theData.rLoc[0], theData.rLoc[1], theData.rLoc[2], theData.rVel[0], theData.rVel[1], theData.rVel[2], theData.rPrecTime.ToString().c_str());
      MessageInterface::ShowMessage("      . State of %s's origin SSBMJ2000 cs : (%.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf)km at time tT = %s\n", tcs->GetOriginName().c_str(), theData.tOStateSSB[0], theData.tOStateSSB[1], theData.tOStateSSB[2], theData.tOStateSSB[3], theData.tOStateSSB[4], theData.tOStateSSB[5], theData.tPrecTime.ToString().c_str());
      MessageInterface::ShowMessage("      . State of %s's origin SSBMJ2000 cs : (%.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf)km at time tR = %s\n", rcs->GetOriginName().c_str(), theData.rOStateSSB[0], theData.rOStateSSB[1], theData.rOStateSSB[2], theData.rOStateSSB[3], theData.rOStateSSB[4], theData.rOStateSSB[5], theData.rPrecTime.ToString().c_str());
      Rvector3 tCSorigin = theData.tOStateSSB.GetR();
      Rvector3 rCSorigin = theData.rOStateSSB.GetR();
      Rvector3 tLocSSB = theData.tLoc + theData.tOStateSSB.GetR();
      Rvector3 rLocSSB = theData.rLoc + theData.rOStateSSB.GetR();
      Rvector3 tVelSSB = theData.tVel + theData.tOStateSSB.GetV();
      Rvector3 rVelSSB = theData.rVel + theData.rOStateSSB.GetV();
      MessageInterface::ShowMessage("      . %s position in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tT = %s\n", theData.tNode->GetName().c_str(), tLocSSB[0], tLocSSB[1], tLocSSB[2], theData.tPrecTime.ToString().c_str());
      MessageInterface::ShowMessage("      . %s position in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tR = %s\n", theData.rNode->GetName().c_str(), rLocSSB[0], rLocSSB[1], rLocSSB[2], theData.rPrecTime.ToString().c_str());
      //MessageInterface::ShowMessage("      . %s velocity in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tT = %s\n", theData.tNode->GetName().c_str(), tVelSSB[0], tVelSSB[1], tVelSSB[2], theData.tPrecTime.ToString().c_str());
      //MessageInterface::ShowMessage("      . %s velocity in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tR = %s\n", theData.rNode->GetName().c_str(), rVelSSB[0], rVelSSB[1], rVelSSB[2], theData.rPrecTime.ToString().c_str());
      MessageInterface::ShowMessage("      . Range vector in SSBMJ2000: (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecInertial[0], theData.rangeVecInertial[1], theData.rangeVecInertial[2]);
      MessageInterface::ShowMessage("      . Range vector in obs CS   : (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecObs[0], theData.rangeVecObs[1], theData.rangeVecObs[2]);
      MessageInterface::ShowMessage("      . R_Obs_j2k matrix  : (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(0, 0), R_Obs_j2k(0, 1), R_Obs_j2k(0, 2));
      MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(1, 0), R_Obs_j2k(1, 1), R_Obs_j2k(1, 2));
      MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(2, 0), R_Obs_j2k(2, 1), R_Obs_j2k(2, 2));
#endif

      // 2. Compute light time solution if it is needed and solve for range vector
#ifdef DEBUG_API
      if (apipsFileOpen)
      {
         apipsFile << "   Mucking with light time (should be skipped)" << std::endl;
      }
#endif
      if (includeLightTime)
      {
         GenerateLightTimeData(satPrecEpoch, epochAtReceive);
      }


#ifdef DEBUG_RANGE_CALCULATION 
      MessageInterface::ShowMessage("   2. Compute Range Vector after light time correction for the Leg from <TNode = %s> to <RNode = %s>:\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
      MessageInterface::ShowMessage("      . %s state in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf)km at time tT= %s\n", theData.tNode->GetName().c_str(), tcs->GetOriginName().c_str(), theData.tLoc[0], theData.tLoc[1], theData.tLoc[2], theData.tVel[0], theData.tVel[1], theData.tVel[2], theData.tPrecTime.ToString().c_str());
      MessageInterface::ShowMessage("      . %s state in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf)km at time tR = %s\n", theData.rNode->GetName().c_str(), rcs->GetOriginName().c_str(), theData.rLoc[0], theData.rLoc[1], theData.rLoc[2], theData.rVel[0], theData.rVel[1], theData.rVel[2], theData.rPrecTime.ToString().c_str());
      MessageInterface::ShowMessage("      . State of %s's origin SSBMJ2000 cs : (%.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf)km at time tT = %s\n", tcs->GetOriginName().c_str(), theData.tOStateSSB[0], theData.tOStateSSB[1], theData.tOStateSSB[2], theData.tOStateSSB[3], theData.tOStateSSB[4], theData.tOStateSSB[5], theData.tPrecTime.ToString().c_str());
      MessageInterface::ShowMessage("      . State of %s's origin SSBMJ2000 cs : (%.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf,   %.12lf)km at time tR = %s\n", rcs->GetOriginName().c_str(), theData.rOStateSSB[0], theData.rOStateSSB[1], theData.rOStateSSB[2], theData.rOStateSSB[3], theData.rOStateSSB[4], theData.rOStateSSB[5], theData.rPrecTime.ToString().c_str());
      tCSorigin = theData.tOStateSSB.GetR();
      rCSorigin = theData.rOStateSSB.GetR();
      tLocSSB = theData.tLoc + theData.tOStateSSB.GetR();
      rLocSSB = theData.rLoc + theData.rOStateSSB.GetR();
      tVelSSB = theData.tVel + theData.tOStateSSB.GetV();
      rVelSSB = theData.rVel + theData.rOStateSSB.GetV();
      MessageInterface::ShowMessage("      . %s position in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tT = %s\n", theData.tNode->GetName().c_str(), tLocSSB[0], tLocSSB[1], tLocSSB[2], theData.tPrecTime.ToString().c_str());
      MessageInterface::ShowMessage("      . %s position in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tR = %s\n", theData.rNode->GetName().c_str(), rLocSSB[0], rLocSSB[1], rLocSSB[2], theData.rPrecTime.ToString().c_str());
      //MessageInterface::ShowMessage("      . %s velocity in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tT = %s\n", theData.tNode->GetName().c_str(), tVelSSB[0], tVelSSB[1], tVelSSB[2], theData.tPrecTime.ToString().c_str());
      //MessageInterface::ShowMessage("      . %s velocity in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tR = %s\n", theData.rNode->GetName().c_str(), rVelSSB[0], rVelSSB[1], rVelSSB[2], theData.rPrecTime.ToString().c_str());
      MessageInterface::ShowMessage("      . Range vector in SSBMJ2000: (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecInertial[0], theData.rangeVecInertial[1], theData.rangeVecInertial[2]);
      MessageInterface::ShowMessage("      . Range vector in obs CS   : (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecObs[0], theData.rangeVecObs[1], theData.rangeVecObs[2]);
      MessageInterface::ShowMessage("      . R_Obs_j2k matrix  : (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(0, 0), R_Obs_j2k(0, 1), R_Obs_j2k(0, 2));
      MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(1, 0), R_Obs_j2k(1, 1), R_Obs_j2k(1, 2));
      MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(2, 0), R_Obs_j2k(2, 1), R_Obs_j2k(2, 2));
#endif


#ifdef DEBUG_API
      if (apipsFileOpen)
      {
         apipsFile << "   Mucking with ET-TAI (should be skipped)" << std::endl;
      }
#endif
      // 3. Calculate ET-TAI correction for this signal leg
      if (useETTAI)
      {
         // Compute ET-TAI at trasnmite node
         Real tETTAI = ETminusTAI(theData.tPrecTime.GetMjd(), theData.tNode);
         // Compute ET-TAI at receive node
         Real rETTAI = ETminusTAI(theData.rPrecTime.GetMjd(), theData.rNode);
         // Compute ET-TAI correction for this signal leg
         ettaiCorrection = (tETTAI - rETTAI)*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM;      // unit: km
         UnsignedInt i = 0;
         for (; i < theData.correctionIDs.size(); ++i)
         {
            if (theData.correctionIDs[i] == "ET-TAI")
               break;
         }
         theData.corrections[i] = ettaiCorrection;                                                         // unit: km
      }

#ifdef DEBUG_API
      if (apipsFileOpen)
      {
         apipsFile << "   Checking feasibility" << std::endl;
      }
#endif
      bool ranOccultationCheck = false;
      // 4. Perform feasibility check
      if (theData.stationParticipant)
      {
         const Real* elData;
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         {
            Rvector6 state_sez(theData.rangeVecObs,
               theData.rangeRateVecObs);
            elData = ((GroundstationInterface*)(theData.tNode))->
               IsValidElevationAngle(state_sez);
            signalIsFeasible = (elData[2] > 0.0);
            theData.feasibility = signalIsFeasible;
            theData.feasibilityReason = (theData.feasibility ? "N" : "B");   // "B": signal is blocked by Earth; "N": normal state
            theData.feasibilityValue = elData[0];

#ifdef DEBUG_FEASIBILITY
            MessageInterface::ShowMessage("At transmit node: Obs vector = [%.12lf,  %.12lf,  %.12lf]km "
               "so %s\n", theData.rangeVecObs(0), theData.rangeVecObs(1),
               theData.rangeVecObs(2), (signalIsFeasible ? "feasible" :
                  "infeasible"));
            MessageInterface::ShowMessage(" elData = (%.8lf  %.8lf   %.8lf) degree\n", elData[0], elData[1], elData[2]);
#endif
         }
         else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         {
            Rvector6 state_sez(-theData.rangeVecObs,
               -theData.rangeRateVecObs);
            elData = ((GroundstationInterface*)(theData.rNode))->
               IsValidElevationAngle(state_sez);
            signalIsFeasible = (elData[2] > 0.0);
            theData.feasibility = signalIsFeasible;
            theData.feasibilityReason = (theData.feasibility ? "N" : "B");   // "B": signal is blocked by Earth; "N": normal state
            theData.feasibilityValue = elData[0];

#ifdef DEBUG_FEASIBILITY
            MessageInterface::ShowMessage("At receive node: Obs vector = [%.12lf,  %.12lf,  %.12lf]km "
               "so %s\n", theData.rangeVecObs(0), theData.rangeVecObs(1),
               theData.rangeVecObs(2), (signalIsFeasible ? "feasible" :
                  "infeasible"));
            MessageInterface::ShowMessage(" elData = (%.8lf  %.8lf   %.8lf) degree\n", elData[0], elData[1], elData[2]);
#endif
         }
      }
      else if (forSimulation && !(theData.rNode->IsOfType(Gmat::GROUND_STATION)) && !(theData.tNode->IsOfType(Gmat::GROUND_STATION)))
      {
         signalIsFeasible = TestSignalBlockedBetweenTwoParticipants(SELECT_ALL_BODIES);
         theData.feasibility = signalIsFeasible;
         theData.feasibilityReason = (theData.feasibility ? "N" : "B");   // "B": signal is blocked by central body; "N": normal state
         
         if (blockedByHORP)
         {
            theData.feasibilityReason = "HORP";
            blockedByHORP = false;
         }
         if (blockedByOccultation)
         {
            theData.feasibilityReason = "B";
            blockedByOccultation = false;
         }

         if (!theData.feasibility)
            theData.feasibilityValue = -100.0;
         ranOccultationCheck = true;
      }

      // Put in test for obstructing central body only if it runs for simulation
      if (forSimulation)
      {
         if (signalIsFeasible && !ranOccultationCheck)
         {
            signalIsFeasible = TestSignalBlockedBetweenTwoParticipants(SELECT_CENTRAL_BODY);
            theData.feasibility = signalIsFeasible;
            theData.feasibilityReason = (theData.feasibility ? "N" : "BCB");   // "BCB": signal is blocked by central body; "N": normal state
            
            if (blockedByHORP)
            {
               theData.feasibilityReason = "HORP";
               theData.feasibility = false;
               blockedByHORP = false;
            }
            if (blockedByOccultation)
            {
               theData.feasibilityReason = "BCB";
               blockedByOccultation = false;
            }

            if (!theData.feasibility)
               theData.feasibilityValue = -100.0;
         }
      }

      //Section: Run Horp during estimation
      //This section it used to run horp if selected during estimation. Only Earth for now. This may be updated later for more bodies and a different solution editing
      //TestSignalBlockedBetweenTwoParticipants() will be required
      if (!forSimulation && !(theData.rNode->IsOfType(Gmat::GROUND_STATION)) && !(theData.tNode->IsOfType(Gmat::GROUND_STATION)))
      {

         if (useHorp || forceHorp)
         { 
            CelestialBody* earth = solarSystem->GetBody("Earth");
            Rvector3 tSSB = theData.tLoc + theData.tOStateSSB.GetR();
            Rvector3 rSSB = theData.rLoc + theData.rOStateSSB.GetR();

            signalIsFeasible = TestHeightOfRayPath(earth, tSSB, rSSB, theData.tPrecTime, theData.rPrecTime);

            theData.feasibility = signalIsFeasible;
            theData.feasibilityReason = (theData.feasibility ? "N" : "HORP");   // "BCB": signal is blocked by central body; "N": normal state
            blockedByHORP = false;
            if (!theData.feasibility)
               theData.feasibilityValue = -100.0;
         }
      }

#ifdef DEBUG_RANGE_CALCULATION 
      MessageInterface::ShowMessage("   3. Summary for signal leg from %s to %s:\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());

      MessageInterface::ShowMessage("      . Geometric range       = %.12lf km\n", geoRange);
      if (includeLightTime)
      {
         Real lightTimeRange = theData.rangeVecInertial.GetMagnitude();
         MessageInterface::ShowMessage("      . Light time solution range = %.12lf km\n", lightTimeRange);
      }
      MessageInterface::ShowMessage("      . Relativity correction = %.12lf km\n", relCorrection);
      MessageInterface::ShowMessage("      . ET-TAI correction     = %.12lf km\n", ettaiCorrection);
      MessageInterface::ShowMessage("      . Feasibility           = %s\n\n", (signalIsFeasible ? "true" : "false"));
#endif

#ifdef DEBUG_API
      if (apipsFileOpen)
      {
         apipsFile << "   Reporting raw data: navLog = " << navLog << std::endl;
      }
#endif

      // 5. Report raw data
      if (navLog)
      {
         std::stringstream data;
         data.precision(16);

         if (logLevel <= 1)
         {
            Real range = theData.rangeVecInertial.GetMagnitude();
            if (range >= 0.0)
               data << "   " << GetPathDescription(false)
               << " Range at A.1 epoch " << satPrecEpoch.GetMjd()
               << " = " << range << "\n";
            else
               data << "   Range not valid\n";
         }

         if (logLevel == 0)
         {
            data.str() = "";
            data << "      Range vector:         "
               << (theData.rangeVecInertial.ToString())
               << "      Range vector Obs:     "
               << (theData.rangeVecObs.ToString())
               << "      RangeRate vector Obs: "
               << (theData.rangeRateVecObs.ToString())
               << "\n      Transmitter location: "
               << (theData.tLoc.ToString())
               << "      Receiver location:    "
               << (theData.rLoc.ToString());
         }
         navLog->WriteData(data.str());
      }

      // 6. Run ModelSignal for the next leg in signal path:
      // if epoctAtReceive was true, transmitter moved and we need its epoch,
      // if false, we need the receiver epoch
      GmatTime nextPrecEpoch = (epochAtReceive ? theData.tPrecTime : theData.rPrecTime);
      //// Assumption: theData.tDelay and theData.rDelay have to be specified before running ModelSignal() function
      //GmatTime nextPrecEpoch;
      //if (epochAtReceive)
      //   nextPrecEpoch = theData.tPrecTime - theData.tDelay*2/GmatTimeConstants::SECS_PER_DAY;          // go backward
      //else
      //   nextPrecEpoch = theData.rPrecTime + theData.rDelay*2/GmatTimeConstants::SECS_PER_DAY;          // go foreward

      // This transmitter is the receiver for the next node
      bool nextFixed = (epochAtReceive ? true : false);

      bool nodePassed = true;

      if (epochAtReceive)
      {
         if (previous)
         {
            previous->SetSignalData(theData);

            /// @todo: If there is a transponder delay, apply it here, moving
            /// nextEpoch back by the delay time
            nodePassed = previous->ModelSignal(nextPrecEpoch, forSimulation, nextFixed);
         }
      }
      else
      {
         if (next)
         {
            next->SetSignalData(theData);

            /// @todo: If there is a transponder delay, apply it here, moving
            /// nextEpoch ahead by the delay time
            nodePassed = next->ModelSignal(nextPrecEpoch, forSimulation, nextFixed);
         }
      }

      retval = nodePassed;
   }

#ifdef DEBUG_API
   if (apipsFileOpen)
   {
      apipsFile << "   That should do it; returning " << retval << std::endl;
   }
#endif

#ifdef DEBUG_EXECUTION
   MessageInterface::ShowMessage("ModelSignal(%s, %s) exit\n", satPrecEpoch.ToString().c_str(),
      epochAtReceive ? "with fixed Receiver" : "with fixed Transmitter");
#endif

   return retval;
}


//------------------------------------------------------------------------------
// bool TestSignalBlockedBetweenTwoSpacecrafts()
//------------------------------------------------------------------------------
/**
* This function is used to test feasibility of signal between 2 spacecrafts
*
* return            true if signal is feasible, false otherwise.
*/
//------------------------------------------------------------------------------
bool PhysicalSignal::TestSignalBlockedBetweenTwoParticipants(Integer selection)
{
   // 1. Get all celestial bodies used in force models associated to spacecrafts
   StringArray bodyNameList1;
   blockedByHORP = false;
   blockedByOccultation = false;
   if (theData.tNode->IsOfType(Gmat::SPACECRAFT))
   {
      std::string cbName;
      StringArray bodyNames1;
      StringArray bodyNames2;

      // 1.1. Get names of all celestial bodies in force model used to propagate transmitted spacecraft
      if (theData.tPropagator->GetPropagator()->UsesODEModel())
      {
         ODEModel* ode = theData.tPropagator->GetODEModel();
         cbName = ode->GetStringParameter("CentralBody");
         bodyNames1 = ode->GetStringArrayParameter("PrimaryBodies");
         bodyNames2 = ode->GetStringArrayParameter("PointMasses");
      }
      else
      {
         cbName = theData.tPropagator->GetPropagator()->GetPropOriginName();
      }

      if (selection & SELECT_CENTRAL_BODY)
         bodyNameList1.push_back(cbName);

      if (selection & SELECT_PRIMARY_BODY)
      {
         for (UnsignedInt i = 0; i < bodyNames1.size(); ++i)
            bodyNameList1.push_back(bodyNames1[i]);
      }

      if (selection & SELECT_POINT_MASSES)
      {
         for (UnsignedInt i = 0; i < bodyNames2.size(); ++i)
            bodyNameList1.push_back(bodyNames2[i]);
      }
   }

   // 1.2. Get names of all celestial bodies in force model used to propagate received spacecraft
   if (theData.rNode->IsOfType(Gmat::SPACECRAFT))
   {
      std::string cbName;
      StringArray bodyNames1;
      StringArray bodyNames2;

      // 1.1. Get names of all celestial bodies in force model used to propagate transmitted spacecraft
      if (theData.rPropagator->GetPropagator()->UsesODEModel())
      {
         ODEModel* ode = theData.rPropagator->GetODEModel();
         cbName = ode->GetStringParameter("CentralBody");
         bodyNames1 = ode->GetStringArrayParameter("PrimaryBodies");
         bodyNames2 = ode->GetStringArrayParameter("PointMasses");
      }
      else
      {
         cbName = theData.rPropagator->GetPropagator()->GetPropOriginName();
      }


      if (selection & SELECT_CENTRAL_BODY)
         bodyNameList1.push_back(cbName);

      if (selection & SELECT_PRIMARY_BODY)
      {
         for (UnsignedInt i = 0; i < bodyNames1.size(); ++i)
            bodyNameList1.push_back(bodyNames1[i]);
      }

      if (selection & SELECT_POINT_MASSES)
      {
         for (UnsignedInt i = 0; i < bodyNames2.size(); ++i)
            bodyNameList1.push_back(bodyNames2[i]);
      }
   }

   // 1.3. Create a unique name list
   StringArray bodyNameList;
   for (UnsignedInt i = 0; i < bodyNameList1.size(); ++i)
   {
      // Check for unique name
      bool found = false;
      for (UnsignedInt j = 0; j < bodyNameList.size(); ++j)
      {
         if (bodyNameList[j] == bodyNameList1[i])
         {
            found = true;
            break;
         }
      }

      // if not found, add to the list
      if (!found)
         bodyNameList.push_back(bodyNameList1[i]);
   }

   // 1.4. Get all celestial bodies in force models
   std::vector<CelestialBody*> bodies;
   for (UnsignedInt i = 0; i < bodyNameList.size(); ++i)
   {
      bool found = false;
      for (UnsignedInt j = 0; j < bodies.size(); ++j)
      {
         if (bodies[j]->GetName() == bodyNameList[i])
         {
            found = true;
            break;
         }
      }

      if (!found)
      {
         CelestialBody* body = solarSystem->GetBody(bodyNameList[i]);
         if (body != NULL)
            bodies.push_back(body);
      }
   }

   // 2. Test for obstructing bodies at time t
   // 2.1. Specify location of 2 participants in SSB coordinate system at time t
   Rvector3 tSSB = theData.tLoc + theData.tOStateSSB.GetR();
   Rvector3 rSSB = theData.rLoc + theData.rOStateSSB.GetR();

   // 2.2. Test for obstructing bodies at time t
   bool obstructed = false;
   CelestialBody* body = NULL;
   for (UnsignedInt i = 0; i < bodies.size(); ++i)
   {
      obstructed = TestSignalBlockedByBody(bodies[i], tSSB, rSSB, theData.tPrecTime, theData.rPrecTime);
      if (obstructed)
      {
         body = bodies[i];
         break;
      }
   }

#ifdef DEBUG_FEASIBILITY
   MessageInterface::ShowMessage("List of all celestial bodies used in obstruction test:\n");
   for (UnsignedInt i = 0; i < bodies.size(); ++i)
      MessageInterface::ShowMessage("celestial body %d: <%s,%p>\n", i, bodies[i]->GetName().c_str(), bodies[i]);

   if (obstructed)
      MessageInterface::ShowMessage("Signal from %s to %s is blocked by %s.\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str(), body->GetName().c_str());
   else
      MessageInterface::ShowMessage("Signal from %s to %s is feasible.\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
#endif

   return !obstructed;
}


//------------------------------------------------------------------------------
// bool TestSignalBlockedByBody(CelestialBody* body, Rvector3 tRSSB, 
//        Rvector3 rRSSB, GmatTime tTime, GmatTime rTime)
//------------------------------------------------------------------------------
/**
* This function is used to test signal obstructed by a celestial body
*
* @param body       celestial body is used for obstruction test
* @param tRSSB      location of transmit spacecraft in the SSB coordinate system
* @param rRSSB      location of received spacecraft in the SSB coordinate system
* @param tTime      transmit time
* @param rTime      receive time
*
* return            true if signal is blocked, false otherwise.
*/
//------------------------------------------------------------------------------
bool PhysicalSignal::TestSignalBlockedByBody(CelestialBody* body,
   Rvector3 tRSSB, Rvector3 rRSSB, GmatTime tTime, GmatTime rTime)
{
   static const Real distanceTolerance = 1.0e-6;            // 1 mm
   static const Integer maxIterations = 10;
   static bool firstWarningMaxIter = true;

   bool obstructed = false;
   bool checkUseHorp = false;
   if (useHorp || forceHorp)
   {
	   checkUseHorp = (body->GetName() == "Earth" || forceHorp);
   }


   // When selected, use the HORP algorithm for sat to sat signals
   if (checkUseHorp && ((theData.tNode->IsOfType(Gmat::SPACECRAFT)) &&
      (theData.rNode->IsOfType(Gmat::SPACECRAFT))))
      obstructed = !TestHeightOfRayPath(body, tRSSB, rRSSB, tTime, rTime);
   if(!obstructed)
   {
      // Let: O      : the center of celestial body
      //      R : the radius of celestial body
      //      A      : location of the transmit participant
      //      B      : location of the received participant
      //      N      : a point on surface of the celestial body such as line AN tangential to the surface

      GmatTime t = tTime;
      Rvector3 pointA = tRSSB;
      Rvector3 pointB = rRSSB;

      // specify radius R in km
      Real R = body->GetEquatorialRadius();

      // specify point O at time t in SSB coordinate system  
      SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
      Rvector3 pointO = body->GetMJ2000PrecPosition(t) - ssb->GetMJ2000PrecPosition(t);

      // Specify length from point A to point N at time t
      Rvector3 ABvec = pointB - pointA;
      Rvector3 AOvec = pointO - pointA;
      Rvector3 ABunit = ABvec.GetUnitVector();
      Rvector3 AOunit = AOvec.GetUnitVector();
      Real AO = AOvec.GetMagnitude();
      Real cos_angleA = ABunit * AOunit;
      Real AN = AO * cos_angleA;

      // Calculate travel length of the signal from time tTime to time t
      Real travelTime = (t - tTime).GetTimeInSec();
      Real travelLen = travelTime * GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / 1000;       // unit : Km

      // Calculate distance from signal to point N
      Real distanceDiff = AN - travelLen;                                                  // unit: Km

      // Specify location of point O
      Integer loopCount = 0;
      while ((GmatMathUtil::Abs(distanceDiff) > distanceTolerance) && (loopCount < maxIterations))
      {
         GmatTime dt;
         dt.SetTimeInSec(distanceDiff * 1000 / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);
         t = t + dt;

         // specify point O at time t in SSB coordinate system
         pointO = body->GetMJ2000PrecPosition(t) - ssb->GetMJ2000PrecPosition(t);

         // Specify length from point A to point N at time t
         AOvec = pointO - pointA;
         AOunit = AOvec.GetUnitVector();
         AO = AOvec.GetMagnitude();
         cos_angleA = ABunit * AOunit;
         AN = AO * cos_angleA;

         // Calculate travel length of the signal from time tTime to time t
         travelTime = (t - tTime).GetTimeInSec();
         travelLen = travelTime * GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / 1000;       // unit : Km

         // Caluclate distance from signal to point N
         distanceDiff = AN - travelLen;                                                  // unit: Km
         loopCount++;
      }

      if (loopCount >= maxIterations && firstWarningMaxIter)
      {
         MessageInterface::ShowMessage("*** WARNING *** The light time calculation "
            "for celestial body occultation failed to converge in %d iterations. "
            "The convergence tolerance is %g, while the difference on the last iteration was %g \n",
            maxIterations, distanceTolerance, GmatMathUtil::Abs(distanceDiff));
         firstWarningMaxIter = false;
      }

      // Check for obstruction:
      /// Let:   M is a projection of point O on line AB
      Rvector3 OMvec = (AN)*ABunit - AOvec;
      Rvector3 AMvec = AOvec + OMvec;
	  Real OMvecMag = OMvec.GetMagnitude();
	  // The current implementation uses a spherical model.  If oblateness is
      //// needed, uncomment the following lines.
      //if (useOblateBody)
      //{
      //   Real f = body->GetFlattening();
      //   // Latitude of the subsatellite point at closest approach, in radians
      //   Real lat = atan2(OMvec[2], sqrt(OMvec[0]*OMvec[0] + OMvec[1]*OMvec[1]));

      //   // Oblateness math per M Nicholson
      //   //
      //   //   Oblate radius at lat P = RE * SQRT ((1.0 - ESQ) / (1.0 - ESQ * COS(P)))
      //   //
      //   // where
      //   //
      //   //   RE = Body's equatorial radius, (e.g. 6378.14 km for Earth)
      //   //   ESQ = FLTCOF * (2.0 * FLTCOF)
      //   //   FLTCOF = 1 / FINV
      //   //   FINV = user input with default =298.25 in some places and 298.257 in others
      //   Real flatfactor = f * (2.0 - f);
      //   R = R * sqrt((1.0 - flatfactor) / (1.0 - flatfactor * cos(lat)));
      //}
#ifdef DEBUG_SCTOSC
	  MessageInterface::ShowMessage("PhysicalSignal::TestSignalBlockedByBody() OMmag = %lf, R = %lf, ABvecMag = %lf, AMVecMAg = %lf, ABvec * AMvec = %lf, at tTime %.15lf\n", OMvec.GetMagnitude(), R, ABvec.GetMagnitude(), AMvec.GetMagnitude(), ABvec * AMvec, tTime.GetMjd());
#endif
	  if ((OMvec.GetMagnitude() <= R) && (ABvec.GetMagnitude() > AMvec.GetMagnitude()) && (ABvec * AMvec >= 0.0))          // It needs to line up in direction from A to M and from M to B
	  {
		  obstructed = true;
        blockedByOccultation = true;
#ifdef DEBUG_SCTOSC
		  MessageInterface::ShowMessage("PhysicalSignal::TestSignalBlockedByBody() Signal is OBSTRUCTED!\n");
#endif
	  }

	  
#ifdef DEBUG_FEASIBILITY
      MessageInterface::ShowMessage("Point A: [%lf,   %lf,   %lf] at tTime %.15lf for %s\n", pointA[0], pointA[1], pointA[2], tTime.GetMjd(), theData.tNode->GetName().c_str());
      MessageInterface::ShowMessage("Point B: [%lf,   %lf,   %lf] at rTime %.15lf for %s\n", pointB[0], pointB[1], pointB[2], rTime.GetMjd(), theData.rNode->GetName().c_str());
      MessageInterface::ShowMessage("Point O: [%lf,   %lf,   %lf] at transmit time tTime = %.15lf\n", pointO[0], pointO[1], pointO[2], tTime.GetMjd());
      MessageInterface::ShowMessage("Point O: [%lf,   %lf,   %lf] at time signal arrive to %s surface %.15lf\n", pointO[0], pointO[1], pointO[2], body->GetName().c_str(), t.GetMjd());
      MessageInterface::ShowMessage("Point M: [%lf,   %lf,   %lf]\n", pointA[0] + AMvec[0], pointA[1] + AMvec[2], pointA[2] + AMvec[2]);
      MessageInterface::ShowMessage("Obstructed = %s\n\n\n", (obstructed ? "true" : "false"));
#endif

   }
   return obstructed;
}

//------------------------------------------------------------------------------
// bool TestHeightOfRayPass(CelestialBody* body, Rvector3 r1SSB, Rvector3 r2SSB,
//       GmatTime t1, GmatTime t2)
//------------------------------------------------------------------------------
/**
 * Checks the body approach distance for a spacecraft to spacecraft ray
 *
 * This method implements the Height of Ray Path algorithm, used to check if
 * the vector between two spacecraft is obstructed by the atmosphere around a
 * celestial body.
 *
 * Class settings for this code have the following behavior:
 *
 *    useHorp toggles the algorithm on or off.  The HORP algorithm is toggled on
 *    in current code, per instruction from the FDF.
 *
 *    horpHeight specifies the minimum height above the body's surface required
 *    for the closest point on the spacecraft to spacecraft ray that allows a
 *    good signal.
 *
 *    horpAngle specifies the central angle (spacecraft - body - spacecraft)
 *    used to determine if the signal is in front of or (potentially) behind the
 *    body.
 *
 *    horpOblateness toggles on heights based on an oblate body model.  The
 *    default uses a spherical model based on the body's equatorial radius.
 *
 * The algorithm specification matches the specification in the GEONS
 * mathematical specification, pages 2-16 and following, adapted to the GMAT
 * code and internals.
 *
 *
 * @param body The body that may obstruct the ray
 * @param r1SSB The solar system barycenter position of the first spacecraft at time t1
 * @param r2SSB The solar system barycenter position of the second spacecraft at time t2
 * @param t1 The epoch of state r1SSB
 * @param t2 The epoch of state r2SSB
 *
 * @return true if the ray passes at least horpHeight (km) above the body
 */
 //------------------------------------------------------------------------------
bool PhysicalSignal::TestHeightOfRayPath(CelestialBody* body, Rvector3 r1SSB,
   Rvector3 r2SSB, GmatTime t1, GmatTime t2)
{
   bool clearSignal = true;

   // Assumption: Station to spacecraft checked elsewhere - might just remove this check
   if ((theData.tNode->IsOfType(Gmat::SPACECRAFT)) &&
      (theData.rNode->IsOfType(Gmat::SPACECRAFT)))
   {
      Real bodyRadius = body->GetEquatorialRadius();

      // Find the body centered position vectors
      SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
      Rvector3 origin = body->GetMJ2000PrecPosition(t1) - ssb->GetMJ2000PrecPosition(t1);
      Rvector3 r1 = r1SSB - origin;
      origin = body->GetMJ2000PrecPosition(t2) - ssb->GetMJ2000PrecPosition(t2);
      Rvector3 r2 = r2SSB - origin;

      // Vector between the spacecraft
      Rvector3 ray = r2 - r1;

      Real r1sq = r1 * r1;
      Real r2sq = r2 * r2;
      Real raysq = ray * ray;
      Real r1dotr2 = r1 * r2;

      Real sinBeta = sqrt((r1sq * r2sq - r1dotr2 * r1dotr2) / (r1sq * raysq));
      Real closest = sqrt(r1sq) * sinBeta;

      if (horpOblateness)
      {
         // Factor in oblateness
         Real portion = sqrt(r1sq - closest * closest);
         Rvector3 hRay = r1 + ray * portion / ray.GetMagnitude();

         // Approximate latitude - ignoring flattening here
         Real lat = atan2(hRay[2], sqrt(hRay[0] * hRay[0] + hRay[1] * hRay[1]));
         Real flattening = body->GetFlattening();
         Real e2 = flattening * (2.0 - flattening);
         bodyRadius *= sqrt((1.0 - e2) / (1.0 - e2 * cos(lat)));
#ifdef DEBUG_HORP
         MessageInterface::ShowMessage("%s   HORP:  %lf, dist: %lf, Lat: %lf, Radius: %lf\n",
            (closest - bodyRadius > minHeight ? "Above" : "Below"),
            closest - bodyRadius - minHeight, closest,
            lat * 180.0 / 3.14159, bodyRadius);
#endif
      }

      // Height test
      Real horpRadius = horpHeight + bodyRadius;

      theData.horpHeight = closest - bodyRadius;

      if (theData.horpHeight < 0)
      {
         theData.horpHeight += 0.0001;
      }
      // Refine with the angle test - the two spacecraft may be on the same
      // side of the body.  We accept any signal that has a spacecraft-body-
      // spacecraft angle smaller than the user set HORP angle
      Real dsq = closest * closest;

      Real xp1 = GmatMathUtil::Abs((ray * r1) / ray.GetMagnitude());
      Real xp2 = GmatMathUtil::Abs((ray * r2) / ray.GetMagnitude());
      Real xp  = GmatMathUtil::Max(xp1, xp2);

      Real delta = xp - sqrt(horpRadius * horpRadius - dsq);

      Real angle = acos(r1dotr2 / sqrt(r1sq * r2sq)) * GmatMathConstants::DEG_PER_RAD;

      theData.horpAngle = angle;

      if (sqrt(raysq) > delta)
      { 
         if (angle >= horpAngle)
            clearSignal = false;

         #ifdef DEBUG_HORP
            MessageInterface::ShowMessage("Angle test: %s  (%lf < %lf?)\n",
               clearSignal ? "Pass" : "Fail", angle, horpAngle);
         #endif
      }

      if (closest >= horpRadius)
         clearSignal = true;

#ifdef DEBUG_HORP
      MessageInterface::ShowMessage("%s:  %s to %s:  H = %lf, height = %lf\n",
         (clearSignal ? "true" : "false"),
         theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str(),
         sqrt(r1sq) * sinBeta, closest);
#endif
   }

   if (!clearSignal)
      blockedByHORP = true;

   return clearSignal;
}

//------------------------------------------------------------------------------
// const std::vector<RealArray>& ModelSignalDerivative(GmatBase* obj,
//       Integer forId, Integer passIndex, GmatTime* measTime, bool forRangeRate)
//------------------------------------------------------------------------------
/**
 * Generates the derivative data for the signal path
 *
 * This method builds the derivative data for the current signal path by adding
 * together the data signal by signal in the theDataDerivatives data vector.
 *
 * @param obj            The object supplying the "with respect to" parameter
 * @param id             The ID for the "with respect to" parameter
 * @oaram measTime       Measurement time. It is needed to specify pass number for pass bias
 * @param forRangeRate   Flag to indicate it calculates range rate partial 
 *                       derivative
 *
 * @return         The vector od derivative data for this signal path
 */
 //------------------------------------------------------------------------------
const std::vector<RealArray>& PhysicalSignal::ModelSignalDerivative(
   GmatBase* obj, Integer forId, GmatTime* measTime, bool forRangeRate)
{
   // Verify valid input
   if (obj == NULL)
      throw MeasurementException("Error: a NULL object inputs to PhysicalSignal::ModelSignalDerivative() function\n");

   // Get parameter ID
   Integer parameterID = -1;
   if (forId > 250)
      parameterID = GetParmIdFromEstID(forId, obj);
   else
      parameterID = forId;
   std::string paramName = obj->GetParameterText(parameterID);

   // Verify initialization
   if (!isInitialized)
   {
#ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("   Calling signal initialization\n");
#endif
      InitializeSignal();
   }

   // Clear derivative data
   theDataDerivatives.clear();
   theDataRateDerivatives.clear();
   if (logLevel < 2)
   {
      std::stringstream msg;
      msg << "Derivative computations performed for the "
         << GetPathDescription(false) << " Signal" << "\n";
      navLog->WriteData(msg.str());
   }

   Integer size = obj->GetEstimationParameterSize(forId);
   if (next)
   {
      if (logLevel < 2)
         navLog->WriteData("   Accessing a 'next' node\n");
      // Collect the data from the "next" node
      theDataDerivatives = next->ModelSignalDerivative(obj, forId, measTime, forRangeRate);
      if (logLevel < 2)
         navLog->WriteData("   Access complete\n");
   }
   else
   {
      // Set up the vector for the data
      if (size <= 0)
         throw MeasurementException("The derivative parameter on derivative "
            "object " + obj->GetName() + "is not recognized");

      RealArray oneRow;
      oneRow.assign(size, 0.0);
      theDataDerivatives.push_back(oneRow);
   }

   // Check to see if obj is a participant
#ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("PhysicalSignal::ModelSignalDerivative(%s, %d) called for signal leg <%s %s>\n", obj->GetName().c_str(), forId, theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   MessageInterface::ShowMessage("Solve-for parameter: %s.%s\n", obj->GetFullName().c_str(), obj->GetParameterText(parameterID).c_str());
#endif
   GmatBase *objPtr = NULL;
   if (theData.tNode == obj)
      objPtr = theData.tNode;
   if (theData.rNode == obj)
      objPtr = theData.rNode;

   //Integer parameterID = -1;
   if (objPtr != NULL)                    // Derivative object is a participant (either GroundStation or Spacecraft):
   {
      if (paramName == "Position")
      {
         Rvector3 result;
         GetRangeDerivative(objPtr, true, false, result);

         for (UnsignedInt jj = 0; jj < 3; ++jj)
         {
            // It accumulates derivatives of all signal legs in its path
            if (next)
               theDataDerivatives[0][jj] += result[jj];
            else
               theDataDerivatives[0][jj] = result[jj];
         }
      }
      else if (paramName == "Velocity")
      {
         Rvector3 result;
         GetRangeDerivative(objPtr, false, true, result);

         for (UnsignedInt jj = 0; jj < 3; ++jj)
         {
            // It accumulates derivatives of all signal legs in its path
            if (next)
               theDataDerivatives[0][jj] += result[jj];
            else
               theDataDerivatives[0][jj] = result[jj];
         }
      }
      else if (paramName == "CartesianX")
      {
         Rvector6 result;
         if (forRangeRate == false)
            GetRangeDerivative(objPtr, true, true, result);
         else
            GetRangeRateDerivative(objPtr, true, true, result);

         for (UnsignedInt jj = 0; jj < 6; ++jj)
         {
            // It accumulates derivatives of all signal legs in its path
            if (next)
               theDataDerivatives[0][jj] += result[jj];
            else
               theDataDerivatives[0][jj] = result[jj];
         }
#ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("Derivatives for signal path <%s %s>: [", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
         for (UnsignedInt jj = 0; jj < 6; ++jj)
            MessageInterface::ShowMessage("   %.12lf", result[jj]);
         MessageInterface::ShowMessage("]\n");
         MessageInterface::ShowMessage("Accumulate derivatives: [");
         for (UnsignedInt jj = 0; jj < 6; ++jj)
            MessageInterface::ShowMessage("   %.12lf", theDataDerivatives[0][jj]);
         MessageInterface::ShowMessage("]\n");

#endif
      }
      else if (paramName == "Cr_Epsilon")
      {
         Real result = GetCrDerivative(objPtr);
         theDataDerivatives[0][0] += result;
      }
      else if (paramName == "Cd_Epsilon")
      {
         Real result = GetCdDerivative(objPtr);
         theDataDerivatives[0][0] += result;
      }
      else if (paramName == "AtmosDensity_Epsilon")
      {
         Real result = GetADSFDerivative(objPtr);
         theDataDerivatives[0][0] += result;
      }
   }
   else         // Derivative object is not a participant (neither a GroundStation nor a Spacecraft): such as ErrorModel object
   {
      if (obj->IsOfType("Plate"))
      {
         // Get Spacecraft object having that plate
         std::string plateFullName = obj->GetFullName();
         size_t pos = plateFullName.find_last_of('.');
         std::string scName = "";
         if (pos == plateFullName.npos)
            throw GmatBaseException("Error: Plate '" + plateFullName + "' has no associated spacecraft.\n");
         else
            scName = plateFullName.substr(0, pos);

         if (scName == theData.tNode->GetName())
            objPtr = theData.tNode;
         else if (scName == theData.rNode->GetName())
            objPtr = theData.rNode;

         Real result = GetParamDerivative(obj, paramName, objPtr);
         theDataDerivatives[0][0] += result;
      }
      else
      {
         if (paramName == "Bias")
         {
            // Take derivative the measurement value w.r.t. Bias only one time for a signal path.                              // made changes by TUAN NGUYEN
            // The calculation of derivative is performed when the current signal leg is the first one in the path.            // made changes by TUAN NGUYEN
            // For example, the participants in signal path is {P0,P1,..., Pn}                                                 // made changes by TUAN NGUYEN
            // the signal path is {<P0,P1>, <P1,P2>, ... <Pn-1,Pn>} with <Pi,Pi+1> is the ith signal leg from Pi to Pi+1.      // made changes by TUAN NGUYEN
            // The derivative of the measurement value of the signal path is skipped to calculate until the current            // made changes by TUAN NGUYEN
            // signal leg is the first one in the path. When the leg is the first one in the path, it calculates derivative.   // made changes by TUAN NGUYEN
            // The expect return value of the derivative has to be 1.0.                                                        // made changes by TUAN NGUYEN

            if (previous == NULL)
            {  
               // This signal leg  object is the first one in signal path. 
               SignalBase* firstleg = this;
               // Get last signal leg
               SignalBase* lastleg = this;
               while (lastleg->GetNext() != NULL)
                  lastleg = lastleg->GetNext();

               if ((firstleg->GetSignalDataObject()->tNode->IsOfType(Gmat::GROUND_STATION)) && (lastleg->GetSignalDataObject()->rNode->IsOfType(Gmat::GROUND_STATION) == false))
               {
                  // if ground station is only at first transmit node in signal path, take derivative w.r.t the bias associate to ground station's error mode, otherwise keep default value 0

                  // Get full name of the derivitive object
                  std::string derivObjName = obj->GetFullName();

                  // Get names of all error models defined in the ground station
                  GroundstationInterface* gs = (GroundstationInterface*)this->GetSignalDataObject()->tNode;
                  std::map<std::string, ObjectArray> errmodelMap = gs->GetErrorModelMap();

                  // Search for error model
                  bool found = false;
                  for (std::map<std::string, ObjectArray>::iterator mapIndex = errmodelMap.begin(); mapIndex != errmodelMap.end(); ++mapIndex)
                  {
                     for (UnsignedInt j = 0; j < mapIndex->second.size(); ++j)
                     {
                        if (mapIndex->second.at(j)->GetFullName() == derivObjName)
                        {
                           found = true;
                           break;
                        }
                     }
                     
                     // Get out of for loop when it was found
                     if (found)
                        break;
                  }

                  if (found)
                  {
                     for (Integer i = 0; i < size; ++i)
                        theDataDerivatives[0][i] += 1.0;
                  }
               }
               //if (theData.rNode->IsOfType(Gmat::GROUND_STATION))                                   // made changes by TUAN NGUYEN
               else if (lastleg->GetSignalDataObject()->rNode->IsOfType(Gmat::GROUND_STATION))        // made changes by TUAN NGUYEN
               {
                  // if ground station is at the end of signal path, take derivative w.r.t the bias associate to ground station's error mode, otherwise keep default value 0
                  // Get full name of the derivitive object
                  std::string derivObjName = obj->GetFullName();

                  // Get names of all error models defined in the ground station
                  // GroundstationInterface* gs = (GroundstationInterface*)this->GetSignalDataObject()->rNode;      // made changes by TUAN NGUYEN
                  GroundstationInterface* gs = (GroundstationInterface*)(lastleg->GetSignalDataObject()->rNode);    // made changes by TUAN NGUYEN
                  std::map<std::string, ObjectArray> errmodelMap = gs->GetErrorModelMap();

                  // Search for error model
                  bool found = false;
                  for (std::map<std::string, ObjectArray>::iterator mapIndex = errmodelMap.begin();
                     mapIndex != errmodelMap.end(); ++mapIndex)
                  {
                     for (UnsignedInt j = 0; j < (mapIndex)->second.size(); ++j)
                     {
                        if (mapIndex->second.at(j)->GetFullName() == derivObjName)
                        {
                           found = true;
                           break;
                        }
                     }

                     // Get out of for loop when it was found
                     if (found)
                        break;
                  }

                  if (found)
                  {
                     for (Integer i = 0; i < size; ++i)
                        theDataDerivatives[0][i] += 1.0;
                  }
               }
               // The following is to account for SC-SC measurements, which will always end with the receiving SC.
               // else if (theData.rNode->IsOfType(Gmat::SPACECRAFT) && theData.tNode->IsOfType(Gmat::SPACECRAFT))    // made changes by TUAN NGUYEN
               else if (lastleg->GetSignalDataObject()->rNode->IsOfType(Gmat::SPACECRAFT) &&                          // made changes by TUAN NGUYEN
                        firstleg->GetSignalDataObject()->tNode->IsOfType(Gmat::SPACECRAFT))                           // made changes by TUAN NGUYEN
               {
                  // if Spacecraft is at the end of signal path, take derivative w.r.t the bias associate to spacecraft's error mode, otherwise keep default value 0
                  // Get full name of the derivitive object
                  std::string derivObjName = obj->GetFullName();

                  // Get names of all error models defined in the Spacecraft receiver
                  // Spacecraft* sc = (Spacecraft*)this->GetSignalDataObject()->rNode;                                // made changes by TUAN NGUYEN
                  Spacecraft* sc = (Spacecraft*)(lastleg->GetSignalDataObject()->rNode);                              // made changes by TUAN NGUYEN

                  ObjectArray hardwareList = sc->GetRefObjectArray(Gmat::HARDWARE);
                  UnsignedInt i;
                  ObjectArray errmodelMap;
                  for (i = 0; i < hardwareList.size(); ++i)
                  {
                     if (hardwareList[i]->IsOfType("Receiver"))
                     {
                        errmodelMap = ((Receiver*)hardwareList[i])->GetRefObjectArray(Gmat::ERROR_MODEL);
                        break;
                     }
                  }

                  if (i == hardwareList.size())
                  {
                     std::stringstream ss;
                     ss << "Error: Spacecraft " << sc->GetName() << " does not have a receiver to receive signal\n";
                     throw MeasurementException(ss.str());
                  }

                  // Search for error model
                  bool found = false;
                  for (i = 0; i < errmodelMap.size(); ++i)
                  {
                     if (errmodelMap[i]->GetFullName() == (derivObjName))
                     {
                        found = true;
                        break;               // It will jump out of for loop
                     }
                     //if (found)
                     //   break;
                  }
                  if (found)
                  {
                     for (Integer i = 0; i < size; ++i)
                        theDataDerivatives[0][i] += 1.0;
                  }
               }
            }

#ifdef DEBUG_DERIVATIVES
            // MessageInterface::ShowMessage("   Deriv is w.r.t. %s  it value %lf\n", paramName.c_str(), theDataDerivatives[0][0]);  // made changes by TUAN NGUYEN
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s  it value = [");                                                    // made changes by TUAN NGUYEN
            for (Integer idx = 0; idx < size; ++idx)                                                                                 // made changes by TUAN NGUYEN
               MessageInterface::ShowMessage("%.12lf   ", theDataDerivatives[0][idx]);                                               // made changes by TUAN NGUYEN
            MessageInterface::ShowMessage("]\n");                                                                                    // made changes by TUAN NGUYEN
#endif
         }
         // Pass Biases solve-for
         else if (paramName == "PassBiases")
         {
            // Take derivative the measurement value w.r.t. Bias only one time for a signal path.                              // made changes by TUAN NGUYEN
            // The calculation of derivative is performed when the current signal leg is the first one in the path.            // made changes by TUAN NGUYEN
            // For example, the participants in signal path is {P0,P1,..., Pn}                                                 // made changes by TUAN NGUYEN
            // the signal path is {<P0,P1>, <P1,P2>, ... <Pn-1,Pn>} with <Pi,Pi+1> is the ith signal leg from Pi to Pi+1.      // made changes by TUAN NGUYEN
            // The derivative of the measurement value of the signal path is skipped to calculate until the current            // made changes by TUAN NGUYEN
            // signal leg is the first one in the path. When the leg is the first one in the path, it calculates derivative.   // made changes by TUAN NGUYEN
            // The expect return value of the derivative has to be 1.0.                                                        // made changes by TUAN NGUYEN
            
            if (previous == NULL)
            {  // This signal leg  object is the first one in signal path. 
               SignalBase* firstleg = this;
               // Get last signal leg
               SignalBase* lastleg = this;
               while (lastleg->GetNext() != NULL)
                  lastleg = lastleg->GetNext();

               if ((firstleg->GetSignalDataObject()->tNode->IsOfType(Gmat::GROUND_STATION)) && (lastleg->GetSignalDataObject()->rNode->IsOfType(Gmat::GROUND_STATION) == false))
               {
                  // if ground station is only at first transmit node in signal path, take derivative w.r.t the bias associate to ground station's error mode, otherwise keep default value 0

                  // Get full name of the derivitive object
                  std::string derivObjName = obj->GetFullName();

                  // Get names of all error models defined in the ground station
                  GroundstationInterface* gs = (GroundstationInterface*)this->GetSignalDataObject()->tNode;
                  std::map<std::string, ObjectArray> errmodelMap = gs->GetErrorModelMap();

                  // Search for error model
                  GmatBase* errmodel = NULL;
                  bool found = false;
                  for (std::map<std::string, ObjectArray>::iterator mapIndex = errmodelMap.begin();
                     mapIndex != errmodelMap.end(); ++mapIndex)
                  {
                     for (UnsignedInt j = 0; j < mapIndex->second.size(); ++j)
                     {
                        if (mapIndex->second.at(j)->GetFullName() == derivObjName)
                        {
                           found = true;
                           errmodel = mapIndex->second.at(j);
                           break;
                        }
                     }

                     // Get out of for loop when it was found
                     if (found)
                        break;
                  }

                  if (found)
                  {
                     std::vector<GmatTime*> startEpoches = ((ErrorModel*)errmodel)->GetPassBiasStartEpoches();
                     Real currentTime = measTime->GetMjd();
                     Integer currentPassNum = 0;
                     for (Integer i = 1; i < startEpoches.size(); ++i)
                     {
                        if (currentTime >= startEpoches[i]->GetMjd())
                           currentPassNum = i;
                        else
                           break;
                     }

                     for (Integer passIndex = 0; passIndex < size; ++passIndex)
                     {
                        if (passIndex == currentPassNum)
                           theDataDerivatives[0][passIndex] += 1.0;
                        else
                           theDataDerivatives[0][passIndex] = 0.0;
                     }
                  }
               }
               // if (theData.rNode->IsOfType(Gmat::GROUND_STATION))                                              // made changes by TUAN NGUYEN
               else if (lastleg->GetSignalDataObject()->rNode->IsOfType(Gmat::GROUND_STATION))                    // made changes by TUAN NGUYEN
               {
                  // if ground station is at the end of signal path, take derivative w.r.t the bias associate to ground station's error mode, otherwise keep default value 0
                  // Get full name of the derivitive object
                  std::string derivObjName = obj->GetFullName();

                  // Get names of all error models defined in the ground station
                  // GroundstationInterface* gs = (GroundstationInterface*)this->GetSignalDataObject()->rNode;    // made changes by TUAN NGUYEN
                  GroundstationInterface* gs = (GroundstationInterface*)(lastleg->GetSignalDataObject()->rNode);  // made changes by TUAN NGUYEN
                  std::map<std::string, ObjectArray> errmodelMap = gs->GetErrorModelMap();

                  // Search for error model
                  GmatBase* errmodel = NULL;
                  bool found = false;
                  for (std::map<std::string, ObjectArray>::iterator mapIndex = errmodelMap.begin();
                     mapIndex != errmodelMap.end(); ++mapIndex)
                  {
                     for (UnsignedInt j = 0; j < (mapIndex)->second.size(); ++j)
                     {
                        if (mapIndex->second.at(j)->GetFullName() == derivObjName)
                        {
                           found = true;
                           errmodel = mapIndex->second.at(j);
                           break;
                        }
                     }

                     // Get out of for loop when it was found
                     if (found)
                        break;
                  }

                  if (found)
                  {
                     std::vector<GmatTime*> startEpoches = ((ErrorModel*)errmodel)->GetPassBiasStartEpoches();
                     Real currentTime = measTime->GetMjd();
                     Integer currentPassNum = 0;
                     for (Integer i = 1; i < startEpoches.size(); ++i)
                     {
                        if (currentTime >= startEpoches[i]->GetMjd())
                           currentPassNum = i;
                        else
                           break;
                     }

                     for (Integer passIndex = 0; passIndex < size; ++passIndex)
                     {
                        if (passIndex == currentPassNum)
                           theDataDerivatives[0][passIndex] += 1.0;
                        else
                           theDataDerivatives[0][passIndex] = 0.0;
                     }
                  }
               }
               // The following is to account for SC-SC measurements, which will always end with the receiving SC.
               // else if (theData.rNode->IsOfType(Gmat::SPACECRAFT) && theData.tNode->IsOfType(Gmat::SPACECRAFT))  // made changes by TUAN NGUYEN      
               else if (lastleg->GetSignalDataObject()->rNode->IsOfType(Gmat::SPACECRAFT) &&                        // made changes by TUAN NGUYEN
                        firstleg->GetSignalDataObject()->tNode->IsOfType(Gmat::SPACECRAFT))                         // made changes by TUAN NGUYEN
               {
                  // if Spacecraft is at the end of signal path, take derivative w.r.t the bias associate to spacecraft's error mode, otherwise keep default value 0
                  // Get full name of the derivitive object
                  std::string derivObjName = obj->GetFullName();

                  // Get names of all error models defined in the Spacecraft receiver
                  // Spacecraft* sc = (Spacecraft*)this->GetSignalDataObject()->rNode;                              // made changes by TUAN NGUYEN
                  Spacecraft* sc = (Spacecraft*)(lastleg->GetSignalDataObject()->rNode);                            // made changes by TUAN NGUYEN

                  ObjectArray hardwareList = sc->GetRefObjectArray(Gmat::HARDWARE);
                  UnsignedInt i;
                  ObjectArray errmodelMap;
                  for (i = 0; i < hardwareList.size(); ++i)
                  {
                     if (hardwareList[i]->IsOfType("Receiver"))
                     {
                        errmodelMap = ((Receiver*)hardwareList[i])->GetRefObjectArray(Gmat::ERROR_MODEL);
                        break;
                     }
                  }

                  if (i == hardwareList.size())
                  {
                     std::stringstream ss;
                     ss << "Error: Spacecraft " << sc->GetName() << " does not have a receiver to receive signal\n";
                     throw MeasurementException(ss.str());
                  }

                  // Search for error model
                  bool found = false;
                  GmatBase* errmodel = NULL;
                  for (i = 0; i < errmodelMap.size(); ++i)
                  {
                     if (errmodelMap[i]->GetFullName() == (derivObjName))
                     {
                        found = true;
                        errmodel = errmodelMap[i];
                        break;                        // it will jump out of for loop
                     }
                  }
                  if (found)
                  {
                     std::vector<GmatTime*> startEpoches = ((ErrorModel*)errmodel)->GetPassBiasStartEpoches();
                     Real currentTime = measTime->GetMjd();
                     Integer currentPassNum = 0;
                     for (Integer i = 1; i < startEpoches.size(); ++i)
                     {
                        if (currentTime >= startEpoches[i]->GetMjd())
                           currentPassNum = i;
                        else
                           break;
                     }

                     for (Integer passIndex = 0; passIndex < size; ++passIndex)
                     {
                        if (passIndex == currentPassNum)
                           theDataDerivatives[0][passIndex] += 1.0;
                        else
                           theDataDerivatives[0][passIndex] = 0.0;
                     }
                  }
               }

            }

#ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s  it value [\n");
            for (Integer i = 0; i < size; ++i)
               MessageInterface::ShowMessage("%lf   ", theDataDerivatives[0][i]);
            MessageInterface::ShowMessage("]\n");
#endif
         }
         // Thrust Scale Factor Solve For
         else if (GmatStringUtil::EndsWith(paramName, ".TSF_Epsilon"))
         {
            // find S/C
            GmatBase *forObj = NULL;
            ODEModel *odeModel = (ODEModel *)obj;
            int numForces = odeModel->GetNumForces();
            for (int forceIndex = 0; forceIndex < numForces; forceIndex++) {   // for each force
               PhysicalModel *physMod = odeModel->GetForce(forceIndex);
               if (physMod->GetTypeName() == "FileThrust") {   // is is a FileThrust force?
                  // does this FileThrust contain our parameter?
                  Integer id = physMod->GetParameterID(paramName);
                  if (id > -1) {
                     StringArray scNames = physMod->GetRefObjectNameArray(Gmat::SPACECRAFT);

                     if (theData.tNode != NULL && theData.tNode->GetType() == Gmat::SPACECRAFT) {
                        std::string name = theData.tNode->GetName();
                        if (find(scNames.begin(), scNames.end(), name) != scNames.end()) {
                           forObj = theData.tNode;
                           break;
                        }
                     }
                     if (theData.rNode != NULL && theData.rNode->GetType() == Gmat::SPACECRAFT) {
                        std::string name = theData.rNode->GetName();
                        if (find(scNames.begin(), scNames.end(), name) != scNames.end()) {
                           forObj = theData.rNode;
                           break;
                        }
                     }
                  }
               }
            }
            if (forObj != NULL) {
               Real result = GetTSFDerivative(forObj, paramName);
               theDataDerivatives[0][0] += result;
            }
         }
         else
         {
#ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. something "
               "independent, so zero\n");
#endif
            // Set 0 to all elements (number of elements is specified by size)
            for (UnsignedInt i = 0; i < size; ++i)
               theDataDerivatives[0][i] += 0.0;
         }
      }
   }

   if ((parameterID >= 0) && (logLevel < 2))
   {
      if (logLevel == 0)
      {
         std::stringstream msg;
         msg.precision(15);
         msg << "   Derivative is w.r.t " << obj->GetName()
            << "." << obj->GetParameterText(parameterID) << "\n";
         for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
         {
            msg << "      " << i << ":  [";
            for (UnsignedInt j = 0; j < theDataDerivatives[i].size(); ++j)
            {
               if (j > 0)
                  msg << ", ";
               msg << theDataDerivatives[i][j];
            }
            msg << "]\n";
         }

         navLog->WriteData(msg.str());
      }
   }

#ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("Exit PhysicalSignal::ModelSignalDerivative(%s, %d) for signal leg <%s %s>\n", obj->GetName().c_str(), forId, theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
#endif


   return theDataDerivatives;
}


//------------------------------------------------------------------------------
// bool GenerateLightTimeData(const GmatEpoch atEpoch,const bool epochAtReceive)
//------------------------------------------------------------------------------
/**
 * Iterates propagation to generate a light time solution
 *
 * @param atEpoch Epoch for the fixed point state
 * @param epochAtReceive Flag indicating that the receiver is held fixed
 *
 * @return true is light time data was generated, false if there was a failure
 *         that did not throw
 */
 //------------------------------------------------------------------------------


bool PhysicalSignal::GenerateLightTimeData(const GmatTime atEpoch,
   const bool epochAtReceive)
{
#ifdef DEBUG_EXECUTION
   GmatTime t = atEpoch;
   MessageInterface::ShowMessage("Called GenerateLightTimeData(%.12lf, "
      "%s)\n", t.GetMjd(), (epochAtReceive ? "Receiver fixed" :
         "Transmitter fixed"));
#endif

   // It is equivalant to range tolerance = time tolerance * speed of light = (1.0e-12 s)x(299792458.0 m/s) = 0.0002998 m = 0.3 mm 
   Real timeTolerance = 1.0e-12;

   bool retval = false;

   if (includeLightTime)
   {
      // First make sure we start at the desired epoch
      MoveToEpoch(atEpoch, epochAtReceive, true);

      // Then compute the initial data
      Rvector3 rangeGeoInertial = theData.rLoc - theData.tLoc;                                              // Range vector as seen from geocentric inertial obeserver (GMAT MathSpec Eq. 6.10)
      Rvector3 displacement = rangeGeoInertial + (theData.rOStateSSB.GetR() - theData.tOStateSSB.GetR());   // Range vector as seen from Barycentric inertial observer (GMAT MathSpec Eq. 6.12)

      Real deltaR = displacement.GetMagnitude();
      Real deltaT = (epochAtReceive ? -1.0 : 1.0) * deltaR /
         (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / 1000.0);

#ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("   DeltaT for light travel over "
         "distance %.3lf km = %le\n", deltaR, deltaT);
#endif

      // Here we go; iterating for a light time solution
      Integer loopCount = 0;

      // Epoch difference, in seconds
      Real deltaE = (theData.rPrecTime - theData.tPrecTime).GetTimeInSec();

#ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("      Starting: dEpoch = %.12le second, dR = "
         "%.3lf km, dT = %.12le second\n", deltaE, deltaR, deltaT);
      MessageInterface::ShowMessage("Initial x Positions: %s  %.3lf -->  "
         "%s  %.3lf\n", theData.tNode->GetName().c_str(), theData.tLoc(0),
         theData.rNode->GetName().c_str(), theData.rLoc(0));
#endif

      // Loop to half microsecond precision or 10 times, whichever comes first
      while ((GmatMathUtil::Abs(deltaE - deltaT) > timeTolerance) && (loopCount < 10))
      {
#ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("      Loop iteration %d\n",
            loopCount);
#endif
         MoveToEpoch(atEpoch + deltaT / GmatTimeConstants::SECS_PER_DAY,
            !epochAtReceive, false);

         deltaE = (epochAtReceive ? -1.0 : 1.0) *
            (theData.rPrecTime - theData.tPrecTime).GetTimeInSec();
         Rvector3 rLocSSB = theData.rLoc + theData.rOStateSSB.GetR();
         Rvector3 tLocSSB = theData.tLoc + theData.tOStateSSB.GetR();
         // Range vector as seen from Barycentric inertial observer (GMAT MathSpec Eq. 6.12)
         displacement = rLocSSB - tLocSSB;

#ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("Positions in SSBMJ2000 cs: %s  (%.12lf   %.12lf   %.12lf)km -->  %s  "
            "(%.12lf   %.12lf   %.12lf)km\n", theData.tNode->GetName().c_str(), tLocSSB(0), tLocSSB(1), tLocSSB(2),
            theData.rNode->GetName().c_str(), rLocSSB(0), rLocSSB(1), rLocSSB(2));
#endif

         relCorrection = 0.0;
         if (useRelativity)
         {
            relCorrection = RelativityCorrection(theData.tLoc + theData.tOStateSSB.GetR(), theData.rLoc + theData.rOStateSSB.GetR(), theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd());
         }

         Real lightTimeRange = displacement.GetMagnitude();
         deltaR = lightTimeRange + relCorrection;
         deltaT = (epochAtReceive ? -1.0 : 1.0) * deltaR /
            (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / 1000.0);
#ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("Light Time range       = %.12lf km\n", lightTimeRange);
         MessageInterface::ShowMessage("Relativity correction = %.12lf km\n", relCorrection);
         MessageInterface::ShowMessage("      ===> dEpoch = %.12le second, dR = "
            "%.8lf km, dT = %.12le second, trigger = %le second\n", deltaE, deltaR, deltaT,
            deltaE - deltaT);
#endif
         ++loopCount;
      }
   }

   // Temporary check on data flow
   // Build the other data vectors
   CalculateRangeVectorInertial();
   CalculateRangeVectorObs();
   CalculateRangeRateVectorObs();

   return retval;
}


bool PhysicalSignal::HardwareDelayCalculation()
{
   bool retval = false;
   Real timeDelay;

   // 1. Get hardware delay from theData.tNode
   if (theData.tNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Transmit participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }

   //if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   //{
   //   // Get time delay from ground station' transmitter
   //   ObjectArray hardwareList = ((GroundstationInterface*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
   //   UnsignedInt i;
   //   for (i = 0; i < hardwareList.size(); ++i)
   //   {
   //      if (hardwareList[i]->IsOfType("Transmitter"))
   //      {
   //         timeDelay = ((Transmitter*)hardwareList[i])->GetDelay(); // unit: second 
   //         break;
   //      }
   //   }
   //      
   //   if (i == hardwareList.size())
   //      timeDelay = 0.0;                     // no delay if no hardware is used
   //}
   //else if (theData.tNode->IsOfType(Gmat::SPACECRAFT))
   //{
   //   // Get time delay from spacecraft's transmitter or transponder
   ObjectArray hardwareList = ((Spacecraft*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
   UnsignedInt i;

   bool isStartNode = (previous == NULL); // These flags are to fix GMT-7511
   bool isEndNode = (next == NULL);

   for (i = 0; i < hardwareList.size(); ++i)
   {
      if (hardwareList[i]->IsOfType("Transmitter") && isStartNode)
      {
         timeDelay = ((Transmitter*)hardwareList[i])->GetDelay();    // unit: second 
         break;
      }

      if (hardwareList[i]->IsOfType("Transponder") && !isStartNode)
      {
         timeDelay = ((Transponder*)hardwareList[i])->GetDelay();    // unit: second 
         break;
      }
      if (i == hardwareList.size() -1 && isStartNode == false) // Rolls back check if no transponder on a non-end node to use the transmitter instead
      {
         i = 0;
         isStartNode = true;
      }
   }

   if (i == hardwareList.size())
      timeDelay = 0.0;                   // no delay if no hardware is used
//}

   if (previous == NULL)
      theData.tDelay = timeDelay;          // for the first signal leg, it needs to add all delay time of ground station's transmitter 
   else
      theData.tDelay = timeDelay / 2;        // otherwise, it needs to add only half delay time of spacecraft's transponder


   // 2. Get hardware delay from theData.rNode
   if (theData.rNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Receive participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }

   //if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
   //{
   //   // Get time delay from ground station' receiver
   //   ObjectArray hardwareList = ((GroundstationInterface*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
   //   UnsignedInt i;
   //   for (i = 0; i < hardwareList.size(); ++i)
   //   {
   //      if (hardwareList[i]->IsOfType("Receiver"))
   //      {
   //         timeDelay = ((Receiver*)hardwareList[i])->GetDelay();           // unit: second 
   //         break;
   //      }
   //   }
   //   if (i == hardwareList.size())
   //      timeDelay = 0.0;                      // no delay if no hardware is used
   //}
   //else
   //{
   //   // Get time delay from spacecraft's receiver or transponder
   hardwareList.clear();
   //ObjectArray hardwareList = ((Spacecraft*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
   hardwareList = ((Spacecraft*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
   //UnsignedInt i;
   for (i = 0; i < hardwareList.size(); ++i)
   {
      if (hardwareList[i]->IsOfType("Receiver") && isEndNode)
      {
         timeDelay = ((Receiver*)hardwareList[i])->GetDelay();    // unit: second 
         break;
      }
      if (hardwareList[i]->IsOfType("Transponder") && !isEndNode)
      {
         timeDelay = ((Transponder*)hardwareList[i])->GetDelay();    // unit: second 
         break;
      }

      if (i == hardwareList.size() - 1 && isEndNode == false)   // Rolls back check if no transponder on a non-end node to use the transmitter instead
      {
         i = 0;
         isEndNode = true;
      }
   }

   if (i == hardwareList.size())
      timeDelay = 0.0;                      // no delay if no hardware is used
//}

   if (next == NULL)
      theData.rDelay = timeDelay;             // for the last signal leg, it needs to add all delay time of ground station's receiver 
   else
      theData.rDelay = timeDelay / 2;           // otherwise, it needs to add only half delay time of spacecraft's transponder

#ifdef DEBUG_RANGE_CALCULATION 
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("   ++++    Hardware delay calculation for leg from %s to %s :\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("     . %s's %s hardware delay   : %.12le sec\n", theData.tNode->GetName().c_str(), ((previous == NULL) ? "" : "half of"), theData.tDelay);
   MessageInterface::ShowMessage("     . %s's %s hardware delay   : %.12le sec\n", theData.rNode->GetName().c_str(), ((next == NULL) ? "" : "half of"), theData.rDelay);
   MessageInterface::ShowMessage("     . Total hardware delay for this signal leg: %.12le sec\n\n", theData.tDelay + theData.rDelay);
#endif
   retval = true;
   return retval;
}

//------------------------------------------------------------------------------
// SignalFrequencyCalculation(std::vector<RampTableData>* rampTB, Real uplinkFrequency, GmatTime measTime = 0.0)
//------------------------------------------------------------------------------
/**
 * Calculates the frequency of a given signal
 *
 * @param rampTB The ramp table to be used 
 * @param uplinkFrequency The frequency transmitted on the uplink leg
 * @param measTime the measurement time, only needed if a transmitter bias needs to be calculated
 *
 * @return true if the signal was calculated, false if an issue occured
 */
 //------------------------------------------------------------------------------
bool PhysicalSignal::SignalFrequencyCalculation(std::vector<RampTableData>* rampTB, Real uplinkFrequency, GmatTime measTime)
{
   // 1. Verify the exsisting of transmit participant and receive participant 
   if (theData.tNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Transmit participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }
   if (theData.rNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Receive participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }


   bool retval = false;
   Real frequency = 0.0;                                                                // unit: MHz
   Real dsFrequency = 0.0;                                                              // unit: MHz
   Real frequencyOscillatorBias = 0.0;                                                 //unit: Hz
   // 2. Get transmit frequency from theData.tNode
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      // This case: the transmit node is a ground station
      if (theData.arriveFreq == -1.0)               // theData.arriveFerq only equals -1.0 when Transmit Node is the first node in signal path 
      {
         // Case 1: Transmit Node is the first node and a ground station: 
         // Transmit frequency is specified by ramp table at the transmit time or by Transmitter's transmit frequency
         if (rampTB)
         {
            // Get transmit frequency from ramp table if it is used
            GmatTime t1 = theData.tPrecTime - theData.tDelay / GmatTimeConstants::SECS_PER_DAY;
            frequency = GetFrequencyFromRampTable(t1.GetMjd(), rampTB) / 1.0e6;                     // unit: Mhz
         }
         else if (uplinkFrequency == 0.0)
         {
            // Get transmit frequency from ground station's transmitter
            ObjectArray hardwareList = ((GroundstationInterface*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
            UnsignedInt i;
            for (i = 0; i < hardwareList.size(); ++i)
            {
               // Before getting transmit signal, it has to specify time at which the signal is transmitted
               // This step is only needed for variant frequency caused by Transmitter
               // Get transmit frequency
               if (hardwareList[i]->IsOfType("Transmitter"))
               {
                  if (measTime != 0.0)
                  {
                     ((Transmitter*)hardwareList[i])->SetEpoch(measTime);
                  }
                  else
                  {
                     GmatTime t1 = theData.tPrecTime - theData.tDelay / GmatTimeConstants::SECS_PER_DAY;
                     ((Transmitter*)hardwareList[i])->SetEpoch(t1);
                  }
                  
                  frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();           // unit: MHz 
                  frequencyOscillatorBias = ((Transmitter*)hardwareList[i])->GetFrequencyOscillatorBias();
                  break;
               }
            }

            if (i == hardwareList.size())
            {
               std::stringstream ss;
               ss << "Error: Ground station " << theData.tNode->GetName() << " does not have a transmitter to transmit signal\n";
               throw MeasurementException(ss.str());
            }
         }
         else
            frequency = uplinkFrequency;
      }
      else
      {
         // Case 2: Transmit Node is an intermediate ground station: 
         // In this case, ground station is used as an intermediate signal transponder.
         // Example: role of GS2 in signal path: {GS1, SC1, GS2, SC2, GS3}
         // Get transmit frequency from ground station's transmitter
         ObjectArray hardwareList = ((GroundstationInterface*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            if (hardwareList[i]->IsOfType("Transponder"))
            {
               frequency = ((Transponder*)hardwareList[i])->GetSignal(1)->GetValue();           // unit: MHz
               break;
            }
         }

         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << theData.tNode->GetName() << " does not have a transponder to pass signal\n";
            throw MeasurementException(ss.str());
         }
      }
   }
   else
   {
      // This case: the transmit node is a spacecraft
      if (theData.arriveFreq == -1.0)               // theData.arriveFerq only equals -1.0 when Transmit Node is the first node in signal path 
      {
         if (uplinkFrequency == 0.0)
         {
            // Case 3: Transmit Node is a spacecraft and is the first node in signal path 
            // Get transmit frequency from spacecraft's transmitter
            ObjectArray hardwareList = ((Spacecraft*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
            UnsignedInt i;
            for (i = 0; i < hardwareList.size(); ++i)
            {
               // Before getting transmit signal, it has to specify time at which the signal is transmitted
               // This step is only needed for variant frequency caused by Transmitter
               // Get transmit frequency
               if (hardwareList[i]->IsOfType("Transmitter"))
               {
                  if (measTime != 0.0)
                  {
                     ((Transmitter*)hardwareList[i])->SetEpoch(measTime);
                  }
                  else
                  {
                     GmatTime t1 = theData.tPrecTime - theData.tDelay / GmatTimeConstants::SECS_PER_DAY;
                     ((Transmitter*)hardwareList[i])->SetEpoch(t1);
                  }
                  frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();    // unit: MHz 
                  frequencyOscillatorBias = ((Transmitter*)hardwareList[i])->GetFrequencyOscillatorBias();
                  break;
               }
            }

            if (i == hardwareList.size())
            {
               std::stringstream ss;
               ss << "Error: Spacecraft " << theData.tNode->GetName() << " does not have a transmitter to transmit signal\n";
               throw MeasurementException(ss.str());
            }
         }
         else
            frequency = uplinkFrequency;
      }
      else
      {
         // Case 4: Transmit Node is a spacecraft and is not the first node in signal path 
         // Get transmit frequency from spacecraft's transponder
         ObjectArray hardwareList = ((Spacecraft*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            if (hardwareList[i]->IsOfType("Transponder"))
            {
               frequency = ((Transponder*)hardwareList[i])->GetSignal(1)->GetValue();    // unit: MHz 
               break;
            }
         }

         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Spacecraft " << theData.tNode->GetName() << " does not have a transponder to pass signal\n";
            throw MeasurementException(ss.str());
         }
      }
   }

   // 3. Set value for transmit frequency in signal data object
   theData.transmitFreq = frequency;
   theData.transmitFreqOscillatorBias = frequencyOscillatorBias;
   // 4. Compute range rate from transmit participant to receive participant 
   // (range rate equals the projection of range rate vector to range vector)
   Real rangeRate = theData.rangeVecInertial.GetUnitVector() * theData.rangeRateVecInertial;                            // unit: km/s

   // 5. Compute doppler shift frequency at receiver
   dsFrequency = frequency * (1 - rangeRate / (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM));   // unt: Mhz

   // 6. Set value for receive frequency signal data object
   theData.receiveFreq = dsFrequency;

   // 7. Set receive frequency to receiver
   if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Receive Node is a ground station
      if (next)
      {
         // Case 1: Ground station is used as an itermediate signal transponder
         // Set doppler shift frequency to ground station's transponder
         ObjectArray hardwareList = ((GroundstationInterface*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            if (hardwareList[i]->IsOfType("Transponder"))
            {
               Signal* inputSignal = ((Transponder*)hardwareList[i])->GetSignal(0);
               inputSignal->SetValue(dsFrequency);                               // unit: Mhz
               ((Transponder*)hardwareList[i])->SetSignal(inputSignal, 0);       // This function forces to calculate transmit frequency of the transponder
               break;
            }
         }

         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << theData.rNode->GetName() << " does not have a transponder to pass signal\n";
            throw MeasurementException(ss.str());
         }
      }
      else
      {
         // Case 2: Ground station is used as the last node
         // Set doppler shift frequency to ground station' receiver
         ObjectArray hardwareList = ((GroundstationInterface*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            if (hardwareList[i]->IsOfType("Receiver"))
            {
               Signal* inputSignal = ((Receiver*)hardwareList[i])->GetSignal();
               inputSignal->SetValue(dsFrequency);                               // unit: MHz 
               ((Receiver*)hardwareList[i])->SetSignal(inputSignal);
               break;
            }
         }

         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << theData.rNode->GetName() << " does not have a receiver to receive signal\n";
            throw MeasurementException(ss.str());
         }
      }
   }
   else
   {

	   // Receive Node is a Spacecraft
	   if (next)
	   {
		   // Case 1: Spacecraft is used as an itermediate signal transponder
			// Set doppler shift frequency to spacecraft's transponder
		   ObjectArray hardwareList = ((Spacecraft*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
		   UnsignedInt i;
		   for (i = 0; i < hardwareList.size(); ++i)
		   {
			   if (hardwareList[i]->IsOfType("Transponder"))
			   {
				   Signal* inputSignal = ((Transponder*)hardwareList[i])->GetSignal(0);
				   inputSignal->SetValue(dsFrequency);                               // unit: Mhz
				   ((Transponder*)hardwareList[i])->SetSignal(inputSignal, 0);       // This function force to calculate transmit frequency of the transponder
				   break;
			   }
		   }

		   if (i == hardwareList.size())
		   {
			   std::stringstream ss;
			   ss << "Error: Spacecraft " << theData.rNode->GetName() << " does not have a transponder to pass signal\n";
			   throw MeasurementException(ss.str());
		   }
	   }
	   else
	   {
		   // Case 2: Spacecraft is used as the last node
		   // Set doppler shift frequency to spacecraft' receiver
		   ObjectArray hardwareList = ((Spacecraft*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
		   UnsignedInt i;
		   for (i = 0; i < hardwareList.size(); ++i)
		   {
			   if (hardwareList[i]->IsOfType("Receiver"))
			   {
				   Signal* inputSignal = ((Receiver*)hardwareList[i])->GetSignal();
				   inputSignal->SetValue(dsFrequency);                               // unit: MHz 
				   ((Receiver*)hardwareList[i])->SetSignal(inputSignal);
				   break;
			   }
		   }

		   if (i == hardwareList.size())
		   {
			   std::stringstream ss;
			   ss << "Error: Spacecraft " << theData.rNode->GetName() << " does not have a receiver to receive signal\n";
			   throw MeasurementException(ss.str());
		   }
	   }
   }

   // 8. Set value for arriveFreq in signal data object of the next signal leg
   if (next)
      next->GetSignalDataObject()->arriveFreq = dsFrequency;

   if (theData.forTDRS)
      theData.mediaCorrectionFrequency = GetTDRSTransmitFrequency(rampTB);

#ifdef DEBUG_RANGE_CALCULATION
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("   ++++    Signal Frequency calculation for leg from %s to %s :\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("     . Arrival frequency      : %.12le Mhz\n", theData.arriveFreq);
   MessageInterface::ShowMessage("     . Transmit frequency     : %.12le Mhz\n", theData.transmitFreq);
   MessageInterface::ShowMessage("     . Doppler shift frequency: %.12le Mhz\n\n", theData.receiveFreq);

#endif

   retval = true;
   return retval;
}




//---------------------------------------------------------------------------------------------
// bool MediaCorrectionCalculation(std::vector<RampTableData>* rampTB)
//---------------------------------------------------------------------------------------------
/**
* This function is used to calculate media (troposphere and ionosphere) correction for a given
* signal leg
*
* @param rampTB   Table containing the information about ramped frequency
*
* return true if calculation is completed
*/
//---------------------------------------------------------------------------------------------
bool PhysicalSignal::MediaCorrectionCalculation(std::vector<RampTableData>* rampTB)
{
   // 1. Get media correction options from ground station
   GroundstationInterface* gs = NULL;
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
      gs = (GroundstationInterface*)theData.tNode;
   else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
      gs = (GroundstationInterface*)theData.rNode;

   // 2. Set tropspohere and ionosphere
   std::string troposphereModel = "None";
   std::string ionosphereModel = "None";
   if (gs)
   {
      troposphereModel = gs->GetStringParameter("TroposphereModel");
      ionosphereModel = gs->GetStringParameter("IonosphereModel");
   }

   UnsignedInt i = 0;
   for (; i < theData.correctionIDs.size(); ++i)
      if (theData.correctionIDs[i] == "Troposphere")
         break;
   if (i == theData.correctionIDs.size())
   {
      theData.correctionIDs.push_back("Troposphere");
      theData.correctionTypes.push_back("Range");
      theData.useCorrection.push_back(true);
      theData.corrections.push_back(0.0);
   }
   else
   {
      theData.useCorrection[i] = true;
      theData.corrections[i] = 0.0;
   }

   if (troposphereModel == "HopfieldSaastamoinen" || troposphereModel == "Marini" || troposphereModel == "TRK-2-23")
   {
      if (troposphere == NULL)
         troposphere = new Troposphere(gs->GetName() + "_Troposphere");
      theData.useCorrection[i] = true;
      troposphere->SetModelTypeName(troposphereModel);
   }
   else
      theData.useCorrection[i] = false;

   bool useTroposphere = theData.useCorrection[i];
   UnsignedInt ij = 0;
   for (; ij < theData.correctionIDs.size(); ++ij)
      if (theData.correctionIDs[ij] == "Troposphere-Elev")
         break;
   if (ij == theData.correctionIDs.size())
   {
      theData.correctionIDs.push_back("Troposphere-Elev");
      theData.correctionTypes.push_back("Elevation");
      theData.useCorrection.push_back(useTroposphere);
      theData.corrections.push_back(0.0);
   }
   else
   {
      theData.useCorrection[ij] = useTroposphere;
      theData.corrections[ij] = 0.0;
   }


   UnsignedInt i1 = 0;
   for (; i1 < theData.correctionIDs.size(); ++i1)
      if (theData.correctionIDs[i1] == "Ionosphere")
         break;
   if (i1 == theData.correctionIDs.size())
   {
      theData.correctionIDs.push_back("Ionosphere");
      theData.correctionTypes.push_back("Range");
      theData.useCorrection.push_back(true);
      theData.corrections.push_back(0.0);
   }
   else
   {
      theData.useCorrection[i1] = true;
      theData.corrections[i1] = 0.0;
   }

   if (ionosphereModel == "IRI2007" || ionosphereModel == "TRK-2-23")
   {
      if (ionosphere == NULL)
      {
         ionosphere = IonosphereCorrectionModel::Instance()->GetIonosphereInstance();
      }
      theData.useCorrection[i1] = true;
      ionosphere->SetModelTypeName(ionosphereModel);
   }
   else
      theData.useCorrection[i1] = false;

   bool useIonosphere = theData.useCorrection[i1];
   UnsignedInt ij1 = 0;
   for (; ij1 < theData.correctionIDs.size(); ++ij1)
      if (theData.correctionIDs[ij1] == "Ionosphere-Elev")
         break;
   if (ij1 == theData.correctionIDs.size())
   {
      theData.correctionIDs.push_back("Ionosphere-Elev");
      theData.correctionTypes.push_back("Elevation");
      theData.useCorrection.push_back(useIonosphere);
      theData.corrections.push_back(0.0);
   }
   else
   {
      theData.useCorrection[ij1] = useIonosphere;
      theData.corrections[ij1] = 0.0;
   }


   if ((troposphere == NULL) && (ionosphere == NULL))
      return true;


   bool retval = false;
   // 3. Computer media correction
   Real frequency = 0;
   Rvector3 r1B = theData.tLoc + theData.tOStateSSB.GetR();
   Rvector3 r2B = theData.rLoc + theData.rOStateSSB.GetR();

   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      // signal is transmitted from ground station
      Real minElevAngle = theData.tNode->GetRealParameter("MinimumElevationAngle");
      frequency = theData.transmitFreq;
      if (theData.mediaCorrectionFrequency != -1.0)
         frequency = theData.mediaCorrectionFrequency;
      MediaCorrection(frequency, r1B, r2B, theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd(), minElevAngle);
   }
   else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
   {
      // signal is received at ground station
      Real minElevAngle = theData.rNode->GetRealParameter("MinimumElevationAngle");
      frequency = theData.receiveFreq;
      if (theData.mediaCorrectionFrequency != -1.0)
         frequency = theData.mediaCorrectionFrequency;
      MediaCorrection(frequency, r2B, r1B, theData.rPrecTime.GetMjd(), theData.tPrecTime.GetMjd(), minElevAngle);
   }
   else
   {
      // signal is transmited from a spacecraft to a spacecraft
      frequency = theData.transmitFreq;
      if (theData.mediaCorrectionFrequency != -1.0)
         frequency = theData.mediaCorrectionFrequency;
      MediaCorrection(frequency, r1B, r2B, theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd(), -90.0);
   }

#ifdef DEBUG_RANGE_CALCULATION
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("   ++++    Media corrections calculation for leg from %s to %s :\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("     .Frequency       : %.12le Mhz\n", frequency);
   //UnsignedInt i;
   for (i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Troposphere")
         break;
   }
   if (theData.useCorrection[i])
      MessageInterface::ShowMessage("     .Troposphere range correction : %.12lf m\n", theData.corrections[i]);
   for (i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Troposphere-Elev")
         break;
   }
   if (theData.useCorrection[i])
      MessageInterface::ShowMessage("     .Troposphere elevation correction : %.12lf rad\n", theData.corrections[i]);
   for (i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Ionosphere")
         break;
   }
   if (theData.useCorrection[i])
      MessageInterface::ShowMessage("     .Ionosphere range correction : %.12lf m\n", theData.corrections[i]);
   for (i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Ionosphere-Elev")
         break;
   }
   if (theData.useCorrection[i])
      MessageInterface::ShowMessage("     .Ionosphere elevation correction : %.12lf rad\n", theData.corrections[i]);

#endif
   retval = true;

   return retval;
}


//----------------------------------------------------------------------
// void AddCorrection(const std::string& modelName, 
//                    const std::string& correctionType)
//----------------------------------------------------------------------
/**
 * This function is used to add correction model to the measurement
 *
 * @param nodelName         The name of correction model involving in the
 *                          measurement
 * @param correctionType    The name of correction type such as:
 *                          //TroposphereModel, IonosphereModel,
 *                          ET-TAI, or Relativity
 */
 //----------------------------------------------------------------------
void PhysicalSignal::AddCorrection(const std::string& modelName,
   const std::string& correctionType)
{
#ifdef DEBUG_MEASUREMENT_CORRECTION
   MessageInterface::ShowMessage("Adding correction type <%s> with named <%s>\n",
      correctionType.c_str(), modelName.c_str());
#endif

   if (correctionType == "Relativity")
   {
      // Set relativity correction
      UnsignedInt i = 0;
      for (; i < theData.correctionIDs.size(); ++i)
         if (theData.correctionIDs[i] == "Relativity")
            break;
      if (i == theData.correctionIDs.size())
      {
         theData.correctionIDs.push_back("Relativity");
         theData.correctionTypes.push_back("Range");
         theData.useCorrection.push_back(true);
         theData.corrections.push_back(0.0);
      }
      else
      {
         theData.useCorrection[i] = true;
         theData.corrections[i] = 0.0;
      }
      useRelativity = true;
   }
   else if (correctionType == "ET-TAI")
   {
      // Set ET-TAI correction
      UnsignedInt i = 0;
      for (; i < theData.correctionIDs.size(); ++i)
         if (theData.correctionIDs[i] == "ET-TAI")
            break;
      if (i == theData.correctionIDs.size())
      {
         theData.correctionIDs.push_back("ET-TAI");
         theData.correctionTypes.push_back("Range");
         theData.useCorrection.push_back(true);
         theData.corrections.push_back(0.0);
      }
      else
      {
         theData.useCorrection[i] = true;
         theData.corrections[i] = 0.0;
      }
      useETTAI = true;
   }

   // Add correction model to the next SignalBase in this signal path
   if (next)
      next->AddCorrection(modelName, correctionType);
}



//--------------------------------------------------------------------------------------------
// Real LightTimeCorrection::RelativityCorrection(Rvector3 r1B, Rvector3 r2B, Real t1, Real t2)
//--------------------------------------------------------------------------------------------
/**
 * Calculate relativity correction
 *
 * @param t1     Epoch used by the first participant (unit: A1Mid)
 * @param t2     Epoch used by the second participant (unit: A1Mid)
 * @param r1B    Position of the first participant in SSB coordinate system at epoch t1 (unit:km)
 * @param r2B    Position of the second participant in SSB coordinate system at epoch t2 (unit:km)
 *
 * return range correction (unit: km)
 */
 //--------------------------------------------------------------------------------------------
Real PhysicalSignal::RelativityCorrection(Rvector3 r1B, Rvector3 r2B, Real t1, Real t2)
{
#ifdef DEBUG_RELATIVITY_CORRECTION
   MessageInterface::ShowMessage("PhysicalSignal::RelativityCorrection(): Input parameters\n");
   MessageInterface::ShowMessage(" r1B = (%.6lf   %.6lf   %.6lf)km at time t1 = %.12lf\n", r1B[0], r1B[1], r1B[2], t1);
   MessageInterface::ShowMessage(" r2B = (%.6lf   %.6lf   %.6lf)km at time t2 = %.12lf\n", r2B[0], r2B[1], r2B[2], t2);
#endif
   Rvector3 P1, P2, BP1, BP2, r1, r2, r12;
   Real r1Mag, r2Mag, r12Mag;

   SpecialCelestialPoint* barycenter = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
   Rvector3 B1 = barycenter->GetMJ2000Position(t1);
   Rvector3 B2 = barycenter->GetMJ2000Position(t2);

   StringArray planetList = solarSystem->GetBodiesInUse();
   CelestialBody* sun = solarSystem->GetBody("Sun");
   Real gammar = 1.0;

   Real relCorr = 0.0;
   for (UnsignedInt i = 0; i < planetList.size(); ++i)
   {
      CelestialBody* planet = solarSystem->GetBody(planetList[i]);
      Real planetMu = planet->GetRealParameter(planet->GetParameterID("Mu"));

      P1 = planet->GetMJ2000Position(t1);            // Planet's position in SSB coordinate system at time t1
      P2 = planet->GetMJ2000Position(t2);            // Planet's position in SSB coordinate system at time t2
      BP1 = P1 - B1;                           // vector from SSB to planet in  SSB coordinate system at time t1
      BP2 = P2 - B2;                           // vector from SSB to planet in  SSB coordinate system at time t2

      r1 = r1B - BP1;                          // position of the first participant in the local inertial coordinate system of the planet at time t1
      r2 = r2B - BP2;                          // position of the first participant in the local inertial coordinate system of the planet at time t2
      r12 = r2 - r1;                           // range vector in the local inertial coordinate system of the planet 
      r1Mag = r1.Norm();                       // unit: km
      r2Mag = r2.Norm();                       // unit: km
      r12Mag = r12.Norm();                     // unit: km

      Real c = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M;      // unit: km/s
      Real term1 = (1 + gammar)*(planetMu / c) / c;         // unit: km
      Real correction;
      if (planet == sun)
      {
         correction = term1 * GmatMathUtil::Ln((r1Mag + r2Mag + r12Mag + term1) / (r1Mag + r2Mag - r12Mag + term1));
      }
      else
      {
         correction = term1 * GmatMathUtil::Ln((r1Mag + r2Mag + r12Mag) / (r1Mag + r2Mag - r12Mag));
      }
      relCorr += correction;
   }

   UnsignedInt i = 0;
   for (; i < theData.correctionIDs.size(); ++i)
      if (theData.correctionIDs[i] == "Relativity")
         break;

   if (i < theData.correctionIDs.size())
   {
      if (theData.useCorrection[i])
         theData.corrections[i] = relCorr;                  // unit: Km
      else
         theData.corrections[i] = 0.0;
   }

#ifdef DEBUG_RELATIVITY_CORRECTION
   MessageInterface::ShowMessage("Relativity correction: %le\n", relCorr);
#endif
   return relCorr;
}


/// Calculate ET - TAI at a ground station on Earth or a spacecraft
Real PhysicalSignal::ETminusTAI(Real tA1MJD, SpacePoint* participant)
{
   // Step 2:
   // Specify celestial bodies and special celestial points:
   CelestialBody* sun = solarSystem->GetBody("Sun");
   CelestialBody* earth = solarSystem->GetBody("Earth");
   CelestialBody* luna = solarSystem->GetBody("Luna");
   CelestialBody* mars = solarSystem->GetBody("Mars");
   CelestialBody* jupiter = solarSystem->GetBody("Jupiter");
   CelestialBody* saturn = solarSystem->GetBody("Saturn");
   SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
   // Create Eath-Moon Barycenter
   Barycenter* emb = new Barycenter("EarthMoonBarycenter");
   emb->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");
   emb->SetRefObject(luna, Gmat::SPACE_POINT, "Luna");
   emb->SetStringParameter("BodyNames", "Earth", 0);
   emb->SetStringParameter("BodyNames", "Luna", 1);
   emb->Initialize();

   // Specify position and velocity of celestial bodies and special celestial points
   Rvector3 ssbPos = ssb->GetMJ2000Position(tA1MJD);
   Rvector3 ssbVel = ssb->GetMJ2000Velocity(tA1MJD);
   Rvector3 sunPos = sun->GetMJ2000Position(tA1MJD);
   Rvector3 sunVel = sun->GetMJ2000Velocity(tA1MJD);
   Rvector3 earthPos = earth->GetMJ2000Position(tA1MJD);
   Rvector3 earthVel = earth->GetMJ2000Velocity(tA1MJD);
   Rvector3 marsPos = mars->GetMJ2000Position(tA1MJD);
   Rvector3 marsVel = mars->GetMJ2000Velocity(tA1MJD);
   Rvector3 jupiterPos = jupiter->GetMJ2000Position(tA1MJD);
   Rvector3 jupiterVel = jupiter->GetMJ2000Velocity(tA1MJD);
   Rvector3 saturnPos = saturn->GetMJ2000Position(tA1MJD);
   Rvector3 saturnVel = saturn->GetMJ2000Velocity(tA1MJD);

   Rvector3 emPos = emb->GetMJ2000Position(tA1MJD);
   Rvector3 emVel = emb->GetMJ2000Velocity(tA1MJD);

   Rvector3 lunaPos = luna->GetMJ2000Position(tA1MJD);
   Rvector3 lunaVel = luna->GetMJ2000Velocity(tA1MJD);

   // Step 3:
   // Note that: position vector participant->GetMJ2000Position(tA1MJD) is pointing from j2kBody to participant (not from SSB nor Earth)
   SpacePoint* j2kBody = participant->GetJ2000Body();

   Rvector3 partMj2000Pos;
   Rvector3 partMj2000Vel;

   if (participant == theData.tNode)
   {
      partMj2000Pos[0] = theData.tLoc[0];
      partMj2000Pos[1] = theData.tLoc[1];
      partMj2000Pos[2] = theData.tLoc[2];
      partMj2000Vel[0] = theData.tVel[0];
      partMj2000Vel[1] = theData.tVel[1];
      partMj2000Vel[2] = theData.tVel[2];
   }
   else
   {
      partMj2000Pos[0] = theData.rLoc[0];
      partMj2000Pos[1] = theData.rLoc[1];
      partMj2000Pos[2] = theData.rLoc[2];
      partMj2000Vel[0] = theData.rVel[0];
      partMj2000Vel[1] = theData.rVel[1];
      partMj2000Vel[2] = theData.rVel[2];
   }


   Rvector3 Earth2GS = partMj2000Pos + j2kBody->GetMJ2000Position(tA1MJD) - earth->GetMJ2000Position(tA1MJD);
   Rvector3 Earth2GS_Vel = partMj2000Vel + j2kBody->GetMJ2000Velocity(tA1MJD) - earth->GetMJ2000Velocity(tA1MJD);

   // Step 4:
   //Define constants
   Real c = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM * GmatMathConstants::M_TO_KM;         // light speed (unit: km/s)
   Real muEarth = earth->GetRealParameter(earth->GetParameterID("Mu"));                  // mu = 398600.4415 for Earth
   Real muSun = sun->GetRealParameter(earth->GetParameterID("Mu"));                    // mu = 132712440017.99 for Sun
   Real muJupiter = jupiter->GetRealParameter(earth->GetParameterID("Mu"));                // mu = 126712767.8578 for Jupiter
   Real muSaturn = saturn->GetRealParameter(earth->GetParameterID("Mu"));                 // mu = 37940626.061137 for Saturn
   Real muMars = mars->GetRealParameter(earth->GetParameterID("Mu"));                   // mu = 42828.314258067 for Mars
   Rvector3 Sun_wrt_SSB_Vel = sunVel - ssbVel;
   Rvector3 EM_wrt_Sun_Pos = emPos - sunPos;
   Rvector3 EM_wrt_Sun_Vel = emVel - sunVel;
   Rvector3 EM_wrt_SSB_Vel = emVel - ssbVel;
   Rvector3 E_wrt_SSB_Vel = earthVel - ssbVel;
   Rvector3 E_wrt_EM_Pos = earthPos - emPos;
   Rvector3 Jup_wrt_Sun_Pos = jupiterPos - sunPos;
   Rvector3 Jup_wrt_Sun_Vel = jupiterVel - sunVel;
   Rvector3 Sat_wrt_Sun_Pos = saturnPos - sunPos;
   Rvector3 Sat_wrt_Sun_Vel = saturnVel - sunVel;

   // ET minus TAI calculation.  Eq. 2-23 on p. 2-14 of Moyer
   Real ET_TAI = 32.184 + 2 * (EM_wrt_Sun_Vel / c)*(EM_wrt_Sun_Pos / c) +
      (EM_wrt_SSB_Vel / c)*(E_wrt_EM_Pos / c) +
      (E_wrt_SSB_Vel / c)*(Earth2GS / c) +
      (muJupiter / (muSun + muJupiter)) * (Jup_wrt_Sun_Vel / c)*(Jup_wrt_Sun_Pos / c) +
      (muSaturn / (muSun + muSaturn)) * (Sat_wrt_Sun_Vel / c)*(Sat_wrt_Sun_Pos / c) +
      (Sun_wrt_SSB_Vel / c)*(EM_wrt_Sun_Pos / c);

   //(muMars  / (c^2 * (muSun + muMars) )) * (Mars_wrt_Sun_Vel'*Mars_wrt_Sun_Pos);  % is this Mars term correct?

#ifdef DEBUG_ETTAI_CORRECTION
   MessageInterface::ShowMessage("_______________________________________________________________________________________________________________________________________________________\n");
   MessageInterface::ShowMessage("    Compute ET-TAI correction for <Participant = %s> at <Time = %.12f>:\n", participant->GetName().c_str(), tA1MJD);
   MessageInterface::ShowMessage("      . participant state in MJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km\n", partMj2000Pos[0], partMj2000Pos[1], partMj2000Pos[2]);
   MessageInterface::ShowMessage("      . participant vel in MJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km\n",partMj2000Vel[0], partMj2000Vel[1], partMj2000Vel[2]);
   MessageInterface::ShowMessage("      . Earth2GS in MJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km\n", Earth2GS[0], Earth2GS[1], Earth2GS[2]);
   MessageInterface::ShowMessage("      . Earth2GS_Vel in MJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km\n", Earth2GS_Vel[0], Earth2GS_Vel[1], Earth2GS_Vel[2]);
#endif
   if (participant->IsOfType(Gmat::SPACECRAFT))
   {
      // Compute PSat in Eq 2-24 Moyer:
      Real Psat = 2 * (Earth2GS_Vel / c)*(Earth2GS / c);
      ET_TAI = ET_TAI + Psat;
   }

   if (emb)
   {
      delete emb;
      emb = NULL;
   }

   return ET_TAI;               // unit: second
}


//----------------------------------------------------------------------------
// RealArray MediaCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch1, 
//                           Real epoch2, Real minElevationAngle)
//----------------------------------------------------------------------------
/**
 * This function is used to calculate media corrections.
 *
 * @param freq    The frequency of signal   (unit: MHz)
 * @param r1B     Position of ground station in SSB FK5 coordinate system
 * @param r2B     Position of spacecraft in SSB FK5 coordinate system
 * @param epoch1  The time at which signal is transmitted from or received at ground station
 * @param epoch2  The time at which signal is received from or transmitted at spacecraft
 * @param minElevationANgle      Minimum elevation angle (unit: degree) at which
 *                               signal can send or receive at ground station
 *
 * return         An array containing results of media correction (units: (m, rad, s))
 */
 //----------------------------------------------------------------------------
RealArray PhysicalSignal::MediaCorrection(Real freq, Rvector3 r1B, Rvector3 r2B,
   Real epoch1, Real epoch2, Real minElevationAngle)
{
#ifdef DEBUG_MEASUREMENT_CORRECTION
   MessageInterface::ShowMessage("start PhysicalMeasurement::MediaCorrection()\n");
#endif
   Real epsilon = 1.0e-8;

   RealArray tropoCorrection;                // units: (m, rad, s)
   tropoCorrection.push_back(0.0);
   tropoCorrection.push_back(0.0);
   tropoCorrection.push_back(0.0);

   RealArray ionoCorrection;                // units: (m, rad, s)
   ionoCorrection.push_back(0.0);
   ionoCorrection.push_back(0.0);
   ionoCorrection.push_back(0.0);

   RealArray mediaCorrection;               // units: (m, rad, s)
   mediaCorrection.push_back(0.0);
   mediaCorrection.push_back(0.0);
   mediaCorrection.push_back(0.0);

   // 1. Run Troposphere correction:
   UpdateRotationMatrix(epoch1, "o_j2k");
   Rvector3 rangeVector = r2B - r1B;                                         // vector pointing from ground station to spacecraft in FK5 coordinate system
   Real elevationAngle = asin((R_Obs_j2k*(rangeVector.GetUnitVector())).GetElement(2));   // unit: radian

   // we always get media correction when elevationAngle > 0
   if (elevationAngle > epsilon)
      //   if (elevationAngle > minElevationAngle*GmatMathConstants::RAD_PER_DEG)
   {
      tropoCorrection = TroposphereCorrection(freq, rangeVector.GetMagnitude(), elevationAngle, epoch1);
#ifdef DEBUG_MEASUREMENT_CORRECTION
      MessageInterface::ShowMessage(" frequency = %le MHz,  epoch1 = %.12lf   epoch2 = %.12lf,   r2B-r1B = ('%.8lf   %.8lf   %.8lf')km\n", freq, epoch1, epoch2, rangeVector[0], rangeVector[1], rangeVector[2]);
      MessageInterface::ShowMessage(" TroposhereCorrection = (%lf m,  %lf arcsec,   %le s)\n", tropoCorrection[0], tropoCorrection[1], tropoCorrection[2]);
#endif

      mediaCorrection[0] = tropoCorrection[0];         // unit: m
      mediaCorrection[1] = tropoCorrection[1];         // unit: rad
      mediaCorrection[2] = tropoCorrection[2];         // unit: second
   }
   // Update value of theData.corrections and theData.useCorrection
   UnsignedInt i = 0;
   for (; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Troposphere")
         break;
   }
   theData.corrections[i] = mediaCorrection[0] * GmatMathConstants::M_TO_KM;            // unit: km
   theData.useCorrection[i] = (troposphere != NULL);

   for (i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Troposphere-Elev")
         break;
   }
   theData.corrections[i] = mediaCorrection[1];                                       // unit: rad
   theData.useCorrection[i] = (troposphere != NULL);


   // 2. Run Ionosphere correction:
   // we always get media correction when elevationAngle > 0
   if (elevationAngle > epsilon)
      //   if (elevationAngle > minElevationAngle*GmatMathConstants::RAD_PER_DEG)
   {
      SignalDataCache::CacheKey cacheKey(strandId, freq, epoch1, epoch2);
      SignalDataCache::SimpleSignalDataCacheIter cachedEntry;
      bool cacheFound = false;

      if (ionosphereCache) {
         cachedEntry = ionosphereCache->find(cacheKey);
         cacheFound = (cachedEntry != ionosphereCache->end());
      }

      if (cacheFound) {
         ionoCorrection = cachedEntry->second.ionoCorrection;
      }
      else {
         ionoCorrection = IonosphereCorrection(freq, r1B, r2B, epoch1, epoch2);

         if (ionosphereCache) {
            ionosphereCache->insert({ cacheKey, SignalDataCache::CacheValue(theData, ionoCorrection) });
         }
      }

#ifdef DEBUG_MEASUREMENT_CORRECTION
      MessageInterface::ShowMessage(" frequency = %le MHz, epoch1 = %lf,  r1B = ('%.8lf   %.8lf   %.8lf')km\n", freq, epoch1, r1B[0], r1B[1], r1B[2]);
      MessageInterface::ShowMessage("                      epoch2 = %lf,  r2B = ('%.8lf   %.8lf   %.8lf')km\n", epoch2, r2B[0], r2B[1], r2B[2]);
      MessageInterface::ShowMessage(" IonoshereCorrection = (%lf m,  %lf rad,   %le s)\n", ionoCorrection[0], ionoCorrection[1], ionoCorrection[2]);
#endif

      // 3. Combine effects:
      mediaCorrection[0] += ionoCorrection[0];               // unit: m
      mediaCorrection[1] += ionoCorrection[1];               // unit: rad
      mediaCorrection[2] += ionoCorrection[2];               // unit: second
   }

   for (i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Ionosphere")
         break;
   }
   theData.corrections[i] = ionoCorrection[0] * GmatMathConstants::M_TO_KM;        // convert unit from m to km
   theData.useCorrection[i] = (ionosphere != NULL);

   for (i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Ionosphere-Elev")
         break;
   }
   theData.corrections[i] = ionoCorrection[1];                // unit: rad
   theData.useCorrection[i] = (ionosphere != NULL);


#ifdef DEBUG_MEASUREMENT_CORRECTION
   MessageInterface::ShowMessage("exit PhysicalMeasurement::MediaCorrection()\n");
#endif

   return mediaCorrection;
}


//------------------------------------------------------------------------
//RealArray TroposphereCorrection(Real freq, Real distance, Real elevationAngle, Real epoch)
//------------------------------------------------------------------------
/**
 * This function is used to calculate Troposphere correction.
 *
 * @param freq            The frequency of signal                               (unit: MHz)
 * @param distance        Distance from ground station to spacecraft            (unit: km)
 * @param elevationAngle  The elevation angle from ground station to spacecraft (unit: radian)
 * @param epoch  Time at which the signal is transmitted or received at ground station      (unit: Julian day)
 *
 * return Troposphere correction vector                                         (units: (m, rad, s))
 */
 //------------------------------------------------------------------------
RealArray PhysicalSignal::TroposphereCorrection(Real freq, Real distance, Real elevationAngle, Real epoch)
{
   RealArray tropoCorrection;

   if (troposphere != NULL)
   {
      // Set troposphere's ref objects
      troposphere->SetSolarSystem(solarSystem);

      // Set temperature, pressure, humidity to Troposphere object

      Real wavelength = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / (freq*1.0e6);

      GroundstationInterface* gs = NULL;
      if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         gs = (GroundstationInterface*)theData.tNode;
      if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         gs = (GroundstationInterface*)theData.rNode;
      if (gs != NULL)
      {
         Rvector3 gsLoc = gs->GetBodyFixedLocation(epoch);
         SpacePoint *gsBody = gs->GetBodyFixedCoordinateSystem()->GetOrigin();

         Rvector3 lla;
         if (gsBody->IsOfType("CelestialBody"))
         {
            CelestialBody *gsBodyCB = (CelestialBody*)gs->GetBodyFixedCoordinateSystem()->GetOrigin();
            lla = BodyFixedStateConverterUtil::CartesianToSphericalEllipsoid(gsLoc, gsBodyCB->GetFlattening(), gsBodyCB->GetEquatorialRadius());
         }
         else
         {
            lla = BodyFixedStateConverterUtil::CartesianToSpherical(gsLoc, 0, 0);
         }
         // spacecraft to ground station troposphere correction
         if (gs->GetStringParameter("TroposphereModel") == "TRK-2-23")
         {
            troposphere->SetDSNDatabase( solarSystem);
            troposphere->SetStationHeight(lla[2]);
            troposphere->SetStationId(gs->GetStringParameter("Id"));
         }
         troposphere->SetTime(epoch);
         troposphere->SetTemperature(gs->GetRealParameter("Temperature"));
         troposphere->SetPressure(gs->GetRealParameter("Pressure"));
         troposphere->SetHumidityFraction(gs->GetRealParameter("Humidity") / 100);
         troposphere->SetWaveLength(wavelength);
         troposphere->SetElevationAngle(elevationAngle);
         troposphere->SetRange(distance*GmatMathConstants::KM_TO_M);
         troposphere->SetLatitude(lla[0]);
         troposphere->SetLongitude(lla[1]);
         tropoCorrection = troposphere->Correction();                            // units: (m, rad, s)
         //Real rangeCorrection = tropoCorrection[0]/GmatMathConstants::KM_TO_M;
      }
      else
      {
         // @todo: spacecraft to spacraft troposphere correction is not define in MathSpec
         // Now it is set to 0
         tropoCorrection.push_back(0.0);
         tropoCorrection.push_back(0.0);
         tropoCorrection.push_back(0.0);
      }

#ifdef DEBUG_TROPOSPHERE_MEDIA_CORRECTION
      MessageInterface::ShowMessage("       *Run Troposphere media correction:\n");
      MessageInterface::ShowMessage("         .Wave length = %.12lf m\n", wavelength);
      MessageInterface::ShowMessage("         .Elevation angle = %.12lf degree\n", elevationAngle*GmatMathConstants::DEG_PER_RAD);
      MessageInterface::ShowMessage("         .Range correction = %.12lf m\n", tropoCorrection[0]);
#endif
   }
   else
   {
      tropoCorrection.push_back(0.0);
      tropoCorrection.push_back(0.0);
      tropoCorrection.push_back(0.0);
   }

   return tropoCorrection;
}


//------------------------------------------------------------------------
//RealArray PhysicalMeasurement::IonosphereCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch1, Real epoch2)
//------------------------------------------------------------------------
/**
 * This function is used to calculate Ionosphere correction.
 *
 * @param freq    The frequency of signal                     (unit: MHz)
 * @param r1B     Position of ground station in SSBMJ2000Eq   (unit: km)
 * @param r2B     Position of spacecraft in SSBMJMJ2000Eq     (unit: km)
 * @param epoch1  Time at which the signal is transmitted or received at ground station      (unit: Julian day)
 * @param epoch2  Time at which the signal is transmitted or received at spacecraft          (unit: Julian day)
 * return         An array containing results of Ionosphere correction   (units: m, rad, s)
 */
 //------------------------------------------------------------------------
RealArray PhysicalSignal::IonosphereCorrection(Real freq, Rvector3 r1B, Rvector3 r2B, Real epoch1, Real epoch2)
{
   RealArray ionoCorrection;
   if (ionosphere != NULL)
   {
      GroundstationInterface* gs = NULL;
      Spacecraft* sc = NULL;
      if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
      {
         gs = (GroundstationInterface*)theData.tNode;
         sc = (Spacecraft*)theData.rNode;
      }
      else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
      {
         gs = (GroundstationInterface*)theData.rNode;
         sc = (Spacecraft*)theData.tNode;
      }

      if (gs == NULL)
      {
         ionoCorrection.push_back(0.0);
         ionoCorrection.push_back(0.0);
         ionoCorrection.push_back(0.0);
      }
      else if (sc == NULL && gs->GetStringParameter("TroposphereModel") == "TRK-2-23")
      {
         ionoCorrection.push_back(0.0);
         ionoCorrection.push_back(0.0);
         ionoCorrection.push_back(0.0);
      }
      else
      {
         // 0. Set ionosphere's ref objects
         ionosphere->SetSolarSystem(solarSystem);

         // 1. Set wave length:
         Real wavelength = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / (freq*1.0e6);      // unit: meter
         ionosphere->SetWaveLength(wavelength);

         // 2. Set time:
         ionosphere->SetTime(epoch1);                                                        // unit: Julian day

         // 3. Set station and spacecraft positions:
         // Create EarthMJ2000Eq coordinate system
         CelestialBody *earthBody = solarSystem->GetBody("Earth");
         CoordinateSystem* fk5cs = CoordinateSystem::CreateLocalCoordinateSystem("Earthfk5", "MJ2000Eq",
            earthBody, NULL, NULL, earthBody, solarSystem);
         // Get Earthfixed coordinate system
         CoordinateSystem* cs = gs->GetBodyFixedCoordinateSystem();
         // Get rotation matrix from EarthMJ2000 cs to Earthfixed cs
         Rvector inState(6, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
         Rvector outState(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
         CoordinateConverter* cv = new CoordinateConverter();
         A1Mjd time1(epoch1);                                        // time when signal is transmitted or received at ground station
         A1Mjd time2(epoch2);                                        // time when signal is transmitted or received at spacecraft
         //cv->Convert(time, inState, cs, outState, fk5cs);
         // Rmatrix33 R_g_j2k    = cv->GetLastRotationMatrix().Transpose();
         cv->Convert(time1, inState, fk5cs, outState, cs);
         Rmatrix33 R_g_j2k = cv->GetLastRotationMatrix();        // rotation matrix converting from fK5 to Earth Bodyfixed at time when ground station transmits or receive signal

         // Specify location of ground station and spacecraft in EarthFixed coordinate system
         SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
         Rvector3 ssb2Earth1 = earthBody->GetMJ2000Position(time1) - ssb->GetMJ2000Position(time1);
         Rvector3 ssb2Earth2 = earthBody->GetMJ2000Position(time1) - ssb->GetMJ2000Position(time2);
         Rvector3 r1 = r1B - ssb2Earth1;                            // ground station position at epoch1 in EarthMJ2000 at epoch1
         Rvector3 r2 = r2B - ssb2Earth2;                            // spacecraft position at epoch2 in EarthMJ2000 at epoch1
         Rvector3 r1_ebf = R_g_j2k * r1;                              // ground station position at epoch1 in EarthBodyFixed at epoch1
         Rvector3 r2_ebf = R_g_j2k * r2;                              // spacecraft position at epoch2 in EarthBodyFixed at epoch1

         ionosphere->SetStationPosition(r1_ebf);                                  // unit: km
         ionosphere->SetSpacecraftPosition(r2_ebf);                               // unit: km

         // 4. Set earth radius:
         SpacePoint* earth = (SpacePoint*)gs->GetRefObject(Gmat::SPACE_POINT, "Earth");
         Real earthRadius = earth->GetRealParameter("EquatorialRadius");
         ionosphere->SetEarthRadius(earthRadius);                                 // unit: km

#ifdef DEBUG_IONOSPHERE_MEDIA_CORRECTION
         MessageInterface::ShowMessage("      *Run Ionosphere media correction for:\n");
         MessageInterface::ShowMessage("         +Earth radius = %lf km\n", earthRadius);
         MessageInterface::ShowMessage("         +Wave length = %.12lf m\n", wavelength);
         MessageInterface::ShowMessage("         +Time = %.12lf\n", epoch1);
         MessageInterface::ShowMessage("         +Station location in Earth body fixed coordinate system (km)   : (%.12lf,  %.12lf,   %.12lf)\n", r1_ebf[0], r1_ebf[1], r1_ebf[2]);
         MessageInterface::ShowMessage("         +Spacecraft location in Earth body fixed coordinate system (km): (%.12lf,  %.12lf,   %.12lf)\n", r2_ebf[0], r2_ebf[1], r2_ebf[2]);
#endif


         if (gs->GetStringParameter("IonosphereModel") == "TRK-2-23")
         {


            ionosphere->SetStationId(gs->GetStringParameter("Id"));
            ionosphere->SetSpacecraftId(GmatMathUtil::Abs(sc->GetIntegerParameter("NAIFId")));
            ionosphere->SetDSNDatabase(solarSystem);
         }

         // 5. Run ionosphere correction:
         ionoCorrection = ionosphere->Correction();             // ionoCorrection units: (m, rad, s) 

         // 6. Clean up
         if (cv)
            delete cv;
         if (fk5cs)
            delete fk5cs;
         cv = NULL; fk5cs = NULL;

#ifdef DEBUG_IONOSPHERE_MEDIA_CORRECTION
         //   MessageInterface::ShowMessage("      *Ionosphere media correction result:\n");
         MessageInterface::ShowMessage("         +Range correction = %.12lf m\n", ionoCorrection[0]);
#endif
      }
   }
   else
   {
      ionoCorrection.push_back(0.0);
      ionoCorrection.push_back(0.0);
      ionoCorrection.push_back(0.0);
   }

   return ionoCorrection;
}


void PhysicalSignal::SpecifyBeginEndIndexesOfRampTable()
{
   // 1. Get search key
   std::string gsName, scName, gsID, scID, searchkey, searchKeySpacetoSpace;
   bool isSpaceToSpace = false;
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      gsName = theData.tNode->GetName();
      gsID = theData.tNode->GetStringParameter("Id");
      scName = theData.rNode->GetName();
      scID = theData.rNode->GetStringParameter("Id");
   }
   else if (theData.tNode->IsOfType(Gmat::SPACECRAFT) && theData.rNode->IsOfType(Gmat::SPACECRAFT))
   {
      gsName = theData.tNode->GetName();
      gsID = theData.tNode->GetStringParameter("Id");
      scName = theData.rNode->GetName();
      scID = theData.rNode->GetStringParameter("Id");
      isSpaceToSpace = true;
   }
   else
   {
      gsName = theData.rNode->GetName();
      gsID = theData.rNode->GetStringParameter("Id");
      scName = theData.tNode->GetName();
      scID = theData.tNode->GetStringParameter("Id");
   }
   searchkey = gsID + " " + scID + " ";
   if (isSpaceToSpace)
   {
      searchKeySpacetoSpace = scID + " " + gsID + " ";
   }

   // 2. Search for the beginning index
   if (rampTable == NULL)
      throw MeasurementException("Error: No ramp table was set for " + GetName() + "\n");
   else if ((*rampTable).size() == 0)
      throw MeasurementException("Error: Ramp table has no data records.\n");

   beginIndex = 0;
   for (; beginIndex < (*rampTable).size(); ++beginIndex)
   {
      if (searchkey == (*rampTable)[beginIndex].indexkey.substr(0, searchkey.size()))
         break;
      if (isSpaceToSpace && searchKeySpacetoSpace == (*rampTable)[beginIndex].indexkey.substr(0, searchkey.size()))
      {
         searchkey = searchKeySpacetoSpace;
         break;
      }
   }

   // 3. Search for the ending index
   endIndex = beginIndex;
   for (; endIndex < (*rampTable).size(); ++endIndex)
   {

      if (searchkey != (*rampTable)[endIndex].indexkey.substr(0, searchkey.size()))
         break;
   }

   // 4. Verify number of data records
   if ((endIndex - beginIndex) == 0)
   {
      std::stringstream ss;
      ss << "Error: Ramp table has no frequency data records for uplink signal from " << gsName << " to " << scName << ". It needs at least 1 record.\n";
      throw MeasurementException(ss.str());
   }
}




//------------------------------------------------------------------------------------------------
// Real PhysicalSignal::GetFrequencyFromRampTable(Real t, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------------------------
/**
* This function is used to get frequency at a given epoch from ramped frequency table
*
* @param t        Epoch (in A1Mjd) at which frequency needed to spicify
* @param rampTB   Table containing information about ramped frequency
*
* return          frequency (unit: Hz)
* Assumptions: ramp table had been sorted by epoch
*/
//------------------------------------------------------------------------------------------------
Real PhysicalSignal::GetFrequencyFromRampTable(Real t, std::vector<RampTableData>* rampTB)
{
   if (rampTB == NULL)
      throw MeasurementException("Error: No ramp table available for measurement calculation\n");
   if ((*rampTB).size() == 0)
      throw MeasurementException("Error: No data is in Ramp table\n");

   if (rampTable == NULL)
   {
      rampTable = rampTB;
      SpecifyBeginEndIndexesOfRampTable();
   }

   if (t <= (*rampTB)[beginIndex].epoch)
      return (*rampTB)[beginIndex].rampFrequency;
   //   else if (t >= (*rampTB)[endIndex-1].epoch)
   //	   return (*rampTB)[endIndex-1].rampFrequency;

      // search for interval which contains time t:
   UnsignedInt interval_index = beginIndex;
   for (UnsignedInt i = beginIndex; i < endIndex; ++i)
   {
      if (t >= (*rampTB)[i].epoch)
         interval_index = i;
      else
         break;
   }


   // specify frequency at time t:
   Real t_start = (*rampTB)[interval_index].epoch;
   Real f0 = (*rampTB)[interval_index].rampFrequency;                    // unit: Hz
   Real f_dot = (*rampTB)[interval_index].rampRate;                      // unit: Hz/second

   Real f = f0 + f_dot * (t - t_start)*GmatTimeConstants::SECS_PER_DAY;    // unit: Hz
//   MessageInterface::ShowMessage("f0 = %lf Hz     f_dot = %lf Hz/s    f = %lf\n", f0, f_dot, f);
   return f;
}


//------------------------------------------------------------------------------------------------
// Real PhysicalSignal::GetFrequencyBandFromRampTable(Real t, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------------------------
/**
* This function is used to get frequency band at a given epoch from ramped frequency table
*
* @param t        Epoch (in A1Mjd) at which frequency needed to specify
* @param rampTB   Table containing information about ramped frequency
*
* return          band index of frequency
* Assumptions: ramp table had been sorted by epoch
*/
//------------------------------------------------------------------------------------------------
Integer PhysicalSignal::GetFrequencyBandFromRampTable(Real t, std::vector<RampTableData>* rampTB)
{
   if (rampTB == NULL)
      throw MeasurementException("Error: No ramp table available for measurement calculation\n");
   if ((*rampTB).size() == 0)
      throw MeasurementException("Error: No data is in ramp table\n");

   if (rampTable == NULL)
   {
      rampTable = rampTB;
      SpecifyBeginEndIndexesOfRampTable();
   }

   if (t <= (*rampTB)[beginIndex].epoch)
      return (*rampTB)[beginIndex].uplinkBand;
   else if (t >= (*rampTB)[endIndex - 1].epoch)
      return (*rampTB)[endIndex - 1].uplinkBand;

   // search for interval which contains time t:
   Integer upBand = 0;
   for (UnsignedInt i = beginIndex; i < endIndex; ++i)
   {
      if (t >= (*rampTB)[i].epoch)
         upBand = (*rampTB)[i].uplinkBand;
      else
         break;
   }

   return upBand;
}


//------------------------------------------------------------------------------
// Integer FrequencyBand(Real frequency)
//------------------------------------------------------------------------------
/**
 * Get frequency band corresponding to a given frequency
 *
 * @param frequency   frequency in Hz
 *
 * @return an integer corresponding to frequency band
 */
 //------------------------------------------------------------------------------
Integer PhysicalSignal::FrequencyBand(Real frequency)
{
   Integer freqBand = -1;
   bool isCBand = false;

   // C-band
   if ((frequency > 4000000000.0) && (frequency < 7000000000.0))
      freqBand = 0;

   // S-band
   if ((frequency >= 2000000000.0) && (frequency <= 4000000000.0))
      freqBand = 1;               // 1 for S-band

   // X-band These values roughly match the JPL DSN specifications for bands. All of these may want to be updated to the IEEE values instead. 
   if ((frequency >= 7000000000.0) && (frequency <= 8400000000.0))
      freqBand = 2;               // 2 for X-band

   // K-band
   if ((frequency > 18000000000.0) && (frequency < 27000000000.0))
      freqBand = 3;              // 3 for K-band

   return freqBand;
}

Real PhysicalSignal::GetTDRSTransmitFrequency(std::vector<RampTableData>* rampTB)
{
   if (theData.tNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Transmit participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }
   if (theData.rNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Receive participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }

   Real frequency = 2000.0;                                                                // unit: MHz

   // 2. Get transmit frequency from theData.tNode
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      // This case: the transmit node is a ground station
      // Case 1: Transmit Node is the first node and a ground station: 
      // Transmit frequency is specified by ramp table at the transmit time or by Transmitter's transmit frequency
      if (rampTB)
      {
         // Get transmit frequency from ramp table if it is used
         GmatTime t1 = theData.tPrecTime - theData.tDelay / GmatTimeConstants::SECS_PER_DAY;
         frequency = GetFrequencyFromRampTable(t1.GetMjd(), rampTB) / 1.0e6;                     // unit: Mhz
      }
      else
      {
         // Get transmit frequency from ground station's transmitter
         ObjectArray hardwareList = ((GroundstationInterface*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            // Before getting transmit signal, it has to specify time at which the signal is transmitted
            // This step is only needed for variant frequency caused by Transmitter
            // Get transmit frequency
            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               GmatTime t1 = theData.tPrecTime - theData.tDelay / GmatTimeConstants::SECS_PER_DAY;
               ((Transmitter*)hardwareList[i])->SetEpoch(t1);
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();           // unit: MHz 
               break;
            }

            if (hardwareList[i]->IsOfType("Transponder"))
            {
               frequency = ((Transponder*)hardwareList[i])->GetSignal(1)->GetValue();           // unit: MHz
            }
         }
      }
   }
   else
   {
      // This case: the transmit node is a spacecraft
      // Case 3: Transmit Node is a spacecraft and is the first node in signal path 
      // Get transmit frequency from spacecraft's transmitter
      if (next == NULL)
      {
         if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         {
            // This case: the transmit node is a ground station
            // Case 1: Transmit Node is the first node and a ground station: 
            // Transmit frequency is specified by ramp table at the transmit time or by Transmitter's transmit frequency
            if (rampTB)
            {
               // Get transmit frequency from ramp table if it is used
               GmatTime t1 = theData.tPrecTime - theData.tDelay / GmatTimeConstants::SECS_PER_DAY;
               frequency = GetFrequencyFromRampTable(t1.GetMjd(), rampTB) / 1.0e6;                     // unit: Mhz
            }
            else
            {
               // Get transmit frequency from ground station's transmitter
               ObjectArray hardwareList = ((GroundstationInterface*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
               UnsignedInt i;
               for (i = 0; i < hardwareList.size(); ++i)
               {
                  // Before getting transmit signal, it has to specify time at which the signal is transmitted
                  // This step is only needed for variant frequency caused by Transmitter
                  // Get transmit frequency
                  if (hardwareList[i]->IsOfType("Transmitter"))
                  {
                     GmatTime t1 = theData.tPrecTime - theData.tDelay / GmatTimeConstants::SECS_PER_DAY;
                     ((Transmitter*)hardwareList[i])->SetEpoch(t1);
                     frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();           // unit: MHz 
                     break;
                  }
               }

               if (i == hardwareList.size())
               {
                  std::stringstream ss;
                  ss << "Error: Ground station " << theData.rNode->GetName() << " does not have a transmitter to transmit signal\n";
                  throw MeasurementException(ss.str());
               }
            }
         }
      }
      else
      {
         ObjectArray hardwareList = ((Spacecraft*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            // Before getting transmit signal, it has to specify time at which the signal is transmitted
            // This step is only needed for variant frequency caused by Transmitter
            // Get transmit frequency
            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               GmatTime t1 = theData.tPrecTime - theData.tDelay / GmatTimeConstants::SECS_PER_DAY;
               ((Transmitter*)hardwareList[i])->SetEpoch(t1);
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();    // unit: MHz 
               break;
            }
         }
      }
   }

   return frequency;
}
