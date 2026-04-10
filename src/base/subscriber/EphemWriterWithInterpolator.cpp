//$Id$
//------------------------------------------------------------------------------
//                             EphemWriterWithInterpolator
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
// Author: Linda Jun / NASA
// Created: 2015.10.28
//
/**
 * Intermediate class to handle ephemeris file interpolation.
 */
//------------------------------------------------------------------------------

#include "EphemWriterWithInterpolator.hpp"
#include "CelestialBody.hpp"
#include "SubscriberException.hpp"   // for exception
#include "StringUtil.hpp"            // for ToString()
#include "LagrangeInterpolator.hpp"  // for LagrangeInterpolator
#include "MessageInterface.hpp"

//#define DEBUG_EPHEMFILE_INSTANCE
//#define DEBUG_EPHEMFILE_INIT
//#define DEBUG_EPHEMFILE_INTERPOLATOR
//#define DEBUG_EPHEMFILE_TIME
//#define DEBUG_EPHEMFILE_ORBIT
//#define DEBUG_EPHEMFILE_FINISH
//#define DEBUG_EPHEMFILE_RESTART
//#define DEBUG_INTERPOLATOR_TRACE

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

//----------------------------------
// methods for this class
//----------------------------------

//------------------------------------------------------------------------------
// EphemWriterWithInterpolator(const std::string &name, const std::string &type)
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EphemWriterWithInterpolator::EphemWriterWithInterpolator(const std::string &name,
                                                         const std::string &type) :
   EphemerisWriter      (name, type),
   interpolator         (NULL),
   interpolatorStatus   (-1),
   initialCount         (0),
   waitCount            (0),
   afterFinalEpochCount (0),
   isNextOutputEpochInLeapSecond (false),
   handleFinalEpoch     (true),
   processingLargeStep  (false),
   checkForLargeTimeGap (false),
   allowLowerOrder      (false),
   timeTolerance        (1.0e-6)
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator() default constructor <%p> enterd, name='%s', "
       "type='%s', writingNewSegment=%d\n", this, name.c_str(), type.c_str(),
       writingNewSegment);
   #endif
}


//------------------------------------------------------------------------------
// ~EphemWriterWithInterpolator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EphemWriterWithInterpolator::~EphemWriterWithInterpolator()
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::~EphemWriterWithInterpolator <%p>'%s' entered\n",
       this, ephemName.c_str());
   #endif
   
   if (interpolator != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (interpolator, interpolator->GetName(), "EphemWriterWithInterpolator::~EphemWriterWithInterpolator()()",
          "deleting local interpolator");
      #endif
      delete interpolator;
   }
   
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::~EphemWriterWithInterpolator() <%p>'%s' leaving\n",
       this, ephemName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// EphemWriterWithInterpolator(const EphemerisWriter &ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EphemWriterWithInterpolator::EphemWriterWithInterpolator(const EphemWriterWithInterpolator &ef) :
   EphemerisWriter      (ef),
   interpolator         (NULL),
   interpolatorStatus   (ef.interpolatorStatus),
   initialCount         (ef.initialCount),
   waitCount            (ef.waitCount),
   afterFinalEpochCount (ef.afterFinalEpochCount),
   isNextOutputEpochInLeapSecond (ef.isNextOutputEpochInLeapSecond),
   handleFinalEpoch     (ef.handleFinalEpoch),
   processingLargeStep  (ef.processingLargeStep),
   checkForLargeTimeGap (ef.checkForLargeTimeGap),
   allowLowerOrder      (ef.allowLowerOrder),
   timeTolerance        (ef.timeTolerance)
{
}


//------------------------------------------------------------------------------
// EphemWriterWithInterpolator&
// EphemWriterWithInterpolator::operator=(const EphemWriterWithInterpolator& ef)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
EphemWriterWithInterpolator&
EphemWriterWithInterpolator::operator=(const EphemWriterWithInterpolator& ef)
{
   if (this == &ef)
      return *this;
   
   EphemerisWriter::operator=(ef);
   
   interpolator         = NULL;
   interpolatorStatus   = ef.interpolatorStatus;
   initialCount         = ef.initialCount;
   waitCount            = ef.waitCount;
   afterFinalEpochCount = ef.afterFinalEpochCount;
   isNextOutputEpochInLeapSecond = ef.isNextOutputEpochInLeapSecond;
   handleFinalEpoch     = ef.handleFinalEpoch;
   processingLargeStep  = ef.processingLargeStep;
   checkForLargeTimeGap = ef.checkForLargeTimeGap;
   allowLowerOrder      = ef.allowLowerOrder;
   timeTolerance        = ef.timeTolerance;
   return *this;
}

//----------------------------------
// methods inherited from Subscriber
//----------------------------------

//----------------------------------
// methods inherited from GmatBase
//----------------------------------

//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void EphemWriterWithInterpolator::Copy(const GmatBase* orig)
{
   operator=(*((EphemWriterWithInterpolator *)(orig)));
}

//--------------------------------------
// protected methods
//--------------------------------------


//------------------------------------------------------------------------------
// void InitializeData(bool saveEpochInfo = true)
//------------------------------------------------------------------------------
void EphemWriterWithInterpolator::InitializeData(bool saveEpochInfo)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterWithInterpolator::InitializeData() <%p>'%s' entered, "
       "saveEpochInfo=%d\n", this, ephemName.c_str(), saveEpochInfo);
   #endif
   
   EphemerisWriter::InitializeData(saveEpochInfo);
   
   epochsOnWaiting.clear();
   
   if (interpolator != NULL)
      interpolator->Clear();
   
   initialCount         = 0;
   waitCount            = 0;
   afterFinalEpochCount = 0;
   allowLowerOrder      = false;
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterWithInterpolator::InitializeData() <%p>'%s' leaving\n",
       this, ephemName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void CreateInterpolator()
//------------------------------------------------------------------------------
void EphemWriterWithInterpolator::CreateInterpolator()
{
   #ifdef DEBUG_EPHEMFILE_INTERPOLATOR
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::CreateInterpolator() entered, interpolatorName='%s', "
       "interpolator=<%p>'%s'\n", interpolatorName.c_str(),
       interpolator, interpolator ? interpolator->GetName().c_str() : "NULL");
   #endif
   
   // If interpolator is not NULL, delete it first
   if (interpolator != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (interpolator, interpolator->GetName(), "EphemWriterWithInterpolator::CreateInterpolator()",
          "deleting local interpolator");
      #endif
      delete interpolator;
      interpolator = NULL;
   }
   
   // Create Interpolator
   if (interpolatorName == "Lagrange")
   {
      // interpolator = new LagrangeInterpolator(ephemName + "_Lagrange", 27, 
      //                                        interpolationOrder);

      // It needs 9 elements for position, velocity, and acceleration and 21
      // elements for lower triangular covariance matrix. Total is 30 elements.
      // interpolator = new LagrangeInterpolator(ephemName + "_Lagrange", 6,
      //                                         interpolationOrder);
      interpolator = new LagrangeInterpolator(ephemName + "_Lagrange", 30,
                                              interpolationOrder);
      #ifdef DEBUG_INTERPOLATOR_TRACE
      MessageInterface::ShowMessage
         ("===> CreateInterpolator() calling interpolator->SetForceInterpolation(false)\n");
      #endif
      // Set force interpolation to false to collect more data if needed
      interpolator->SetForceInterpolation(false);
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (interpolator, interpolator->GetName(), "EphemWriterWithInterpolator::CreateInterpolator()",
          "interpolator = new LagrangeInterpolator()");
      #endif
   }
   else if (interpolatorName == "SLERP")
   {
      throw SubscriberException("The SLERP Interpolator is not ready\n");
      //interpolator = new SLERP;
   }
   
   #ifdef DEBUG_EPHEMFILE_INTERPOLATOR
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::CreateInterpolator() leaving, interpolator=<%p>'%s'\n",
       interpolator, interpolator ? interpolator->GetName().c_str() : "NULL");
   #endif
}

//------------------------------------------------------------------------------
// void FindNextOutputEpoch(Real reqEpochInSecs, Real &outEpochInSecs, 
//        Real stateToWrite[6], Real covToWrite[21],Real accelToWrite[3])
//------------------------------------------------------------------------------
void EphemWriterWithInterpolator::FindNextOutputEpoch(Real reqEpochInSecs,
                                                      Real &outEpochInSecs,
                                                      Real stateToWrite[6],
                                                      Real covToWrite[21],
                                                      Real accelToWrite[3])
{
   #ifdef DEBUG_EPHEMFILE_TIME
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::FindNextOutputEpoch() <%p>'%s' entered\n   "
       "currEpochInSecs=%.15f, reqEpochInSecs=%.15f\n", this, ephemName.c_str(),
       currEpochInSecs, reqEpochInSecs);
   #endif
   
   // If the difference between current epoch and requested epoch is less 
   // than timeTolerance, write out current state (LOJ: 2010.09.30)
   if (GmatMathUtil::Abs(currEpochInSecs - reqEpochInSecs) < timeTolerance)
   {
      outEpochInSecs = currEpochInSecs;
      nextOutEpochInSecs = outEpochInSecs + (stepSizeInSecs * currPropDirection);
      
      #ifdef DEBUG_EPHEMFILE_TIME
      MessageInterface::ShowMessage
         ("***** The difference between current epoch and requested epoch is less "
          "than timeTolerance:%.15f, so writing current state\n", timeTolerance);
      DebugWriteTime("   outEpochInSecs = ", outEpochInSecs);
      DebugWriteTime("     nextOutEpochInSecs = ", nextOutEpochInSecs);
      DebugWriteOrbit("   =====> current state ", currEpochInSecs, currState,
                      false, true);
      #endif
      
      for (int i=0; i<6; i++)
         stateToWrite[i] = currState[i];
      for (int i = 0; i < 21; i++)
         covToWrite[i] = currCov[i];
      for (int i = 0; i < 3; i++)
         accelToWrite[i] = currAccel[i];

      // Erase requested epoch from the epochs on waiting list if found (LOJ: 2010.02.28)
      RemoveEpochAlreadyWritten(reqEpochInSecs, "   =====> WriteOrbit() now erasing ");
      AddNextEpochToWrite(nextOutEpochInSecs,
                          "   ===== Adding current epoch: Adding nextOutEpochInSecs to epochsOnWaiting");
   }
   
   #ifdef DEBUG_EPHEMFILE_TIME
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::FindNextOutputEpoch() <%p>'%s' leaving, "
       "nextOutEpochInSecs=%.15f\n", this, ephemName.c_str(), nextOutEpochInSecs);
   DebugWriteState("stateToWrite =", stateToWrite);
   #endif
}


//------------------------------------------------------------------------------
// bool IsTimeToWrite(Real epochInSecs, const Real state[6], const Real cov[21],
//                    const Real accel[3])
//------------------------------------------------------------------------------
/*
 * Determines if it is time to write to ephemeris file based on the step size.
 *
 * @param epochInSecs Epoch in seconds
 */
//------------------------------------------------------------------------------
bool EphemWriterWithInterpolator::IsTimeToWrite(Real epochInSecs, const Real state[6],
                                                const Real cov[21], const Real accel[3])
{
   #ifdef DEBUG_EPHEMFILE_TIME
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::IsTimeToWrite() <%p>'%s' entered\n   writingNewSegment=%d, "
       "writeOrbit=%d, writeAttitude=%d, state[0]=%.15f\n", this, ephemName.c_str(),
       writingNewSegment, writeOrbit, writeAttitude, state[0]);
   DebugWriteTime("   prevEpochInSecs = ", prevEpochInSecs);
   DebugWriteTime("       epochInSecs = ", epochInSecs);
   MessageInterface::ShowMessage
      ("   useFixedStepSize=%d, interpolateInitialState=%d, interpolateFinalState=%d, "
       "finalEpochProcessed=%d\n", useFixedStepSize, interpolateInitialState, interpolateFinalState,
       finalEpochProcessed);
   #endif
   
   if (finalEpochProcessed)
   {
      #ifdef DEBUG_EPHEMFILE_TIME
      MessageInterface::ShowMessage
         ("EphemWriterWithInterpolator::IsTimeToWrite() returning false, the final epoch has been processed\n");
      #endif
      return false;
   }
   
   bool retval = true;
   // If writing at specified interval step, do checking
   if (useFixedStepSize || interpolateInitialState || interpolateFinalState)
   {
      // Add data points
      if (writeOrbit)
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         MessageInterface::ShowMessage("   Checking if new data points need to be added\n");
         DebugWriteTime("       epochInSecs = ", epochInSecs);
         DebugWriteTime("   prevEpochInSecs = ", prevEpochInSecs);
         #endif
         
         // If current epoch is greater than previous epoch, add data to interpolator
         if ((epochInSecs > prevEpochInSecs) ||
             ((epochInSecs <= prevEpochInSecs) && currPropDirection == -1.0))
         {
            #ifdef DEBUG_EPHEMFILE_TIME
            DebugWriteTime("   ===== Adding to interpolator, epochInSecs = ", epochInSecs);
            #endif
            #ifdef DEBUG_INTERPOLATOR_TRACE
            MessageInterface::ShowMessage
               ("   ===> IsTimeToWrite() calling interpolator->AddPoint(epochInSecs, state)\n");
            #endif
            // Real fullState[27];
            // memcpy(&fullState[0], state, sizeof (Real)*6);
            // memcpy(&fullState[6], cov, sizeof (Real)*21);
            // interpolator->AddPoint(epochInSecs, fullState);

            
            Real dependent[30];
            for (Integer i = 0; i < 6; ++i)
               dependent[i] = state[i];
            for (Integer i = 0; i < 3; ++i)
               dependent[i + 6] = accel[i];
            for (Integer i = 0; i < 21; ++i)
               dependent[i+9] = cov[i];

            // the state's dimension is expanded by adding accelaration and covariance
            // interpolator->AddPoint(epochInSecs, state);
            interpolator->AddPoint(epochInSecs, dependent);
            prevEpochInSecs = epochInSecs;
         }
         else
         {
            #ifdef DEBUG_EPHEMFILE_TIME
            MessageInterface::ShowMessage
               ("   ========== skipping epoch <= prevEpochInSecs epochInSecs=%.15f, prevEpochInSecs=%.15f\n",
                epochInSecs, prevEpochInSecs);
            #endif
         }
      }
      else if (writeAttitude)
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         MessageInterface::ShowMessage("Adding points to interpolator is todo work\n");
         #endif
      }
      else
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         MessageInterface::ShowMessage
            ("EphemWriterWithInterpolator::IsTimeToWrite() returning false, "
             "not writing orbit or attitude data\n");
         #endif
         //retval;
         // may need to ask original intent, not clear that every branch in the
         // code reached below sets retval rather than using the original
         // default value of true;
      }
      
      #ifdef DEBUG_EPHEMFILE_TIME
      MessageInterface::ShowMessage
         ("   ===== processingLargeStep=%d, finalEpochReached=%d, handleFinalEpoch=%d, "
          "waitCount=%d\n", processingLargeStep, finalEpochReached, handleFinalEpoch, waitCount);
      #endif
      
      // If step size is to large, we may miss the data points since interpolator
      // buffer size is limited. So do additional process here.
      // If user defined final epoch is reached, process data (for GMT-3342 fix)
      if (processingLargeStep  || (finalEpochReached && handleFinalEpoch))
      {
         waitCount++;
         
         if (waitCount >= interpolationOrder / 2)
         {
            #ifdef DEBUG_EPHEMFILE_TIME
            MessageInterface::ShowMessage
               ("   waitCount=%d, Calling ProcessEpochsOnWaiting()\n", waitCount);
            #endif
            
            ProcessEpochsOnWaiting(false, false);
            waitCount = 0;
            
            if (processingLargeStep)
               processingLargeStep = false;
            
            if (finalEpochReached && handleFinalEpoch)
               handleFinalEpoch = false;
         }
      }
      
      #ifdef DEBUG_EPHEMFILE_TIME
      MessageInterface::ShowMessage("   Computing next output time\n");
      #endif
      
      // compute next output time
      if (writingNewSegment)
      {
         nextOutEpochInSecs = epochInSecs;
         #ifdef DEBUG_EPHEMFILE_TIME
         DebugWriteTime("   nextOutEpochInSecs = ", epochInSecs);
         #endif
         // If user initial epoch is defined, set it to nextOutEpochInSecs
         //if (initialEpochA1Mjd != -999.999)
         // If user initial epoch is defined and it is first time writng a segment
         // set it to nextOutEpochInSecs (LOJ: 2014.04.15)
         if (initialEpochA1Mjd != -999.999 && firstTimeWriting)
         {
            nextOutEpochInSecs = initialEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY;
            
            // Add user defined initial epoch to epochsOnWaiting so that initial state
            // can be interpolated
            AddNextEpochToWrite(nextOutEpochInSecs,
                                "   ===== First time writing: Adding nextOutEpochInSecs to epochsOnWaiting, ");
            
            #ifdef DEBUG_EPHEMFILE_TIME
            MessageInterface::ShowMessage
               ("   Using user defined initial epoch and first time writing a segment\n");
            DebugWriteTime("   nextOutEpochInSecs = ", epochInSecs);
            #endif
         }
         
         nextReqEpochInSecs = nextOutEpochInSecs;
         #ifdef DEBUG_EPHEMFILE_TIME
         MessageInterface::ShowMessage
            ("   ===== Writing new segment, setting new nextOutEpochInSecs and nextReqEpochInSecs to %.15f, %s\n",
             nextReqEpochInSecs, ToUtcGregorian(nextReqEpochInSecs, false, 2).c_str());
         #endif
         retval = true;
      }
      else
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         DebugWriteTime("          epochInSecs = ", epochInSecs);
         DebugWriteTime("   nextOutEpochInSecs = ", nextOutEpochInSecs);
         MessageInterface::ShowMessage("   handleFinalEpoch=%d\n", handleFinalEpoch);
         #endif
         
         if (useFixedStepSize)
         {
            // If final user defined epoch needs to be handled and received the next
            // output epoch, then add a new next epoch. We don't need to keep adding
            // the new next output epoch when final epoch has already written out. (LOJ: 2012.12.18)
            if (handleFinalEpoch && (((epochInSecs >= nextOutEpochInSecs) && currPropDirection == 1.0) ||
                                     ((epochInSecs <= nextOutEpochInSecs) && currPropDirection == -1.0)))
            {
               nextOutEpochInSecs = nextOutEpochInSecs + (stepSizeInSecs * currPropDirection);
               AddNextEpochToWrite(nextOutEpochInSecs,
                                   "   ===== Using fixed step: Adding nextOutEpochInSecs to epochsOnWaiting, ");
               
               // Handle step size less than integrator step size
               Real nextOut = nextOutEpochInSecs;
               #ifdef DEBUG_EPHEMFILE_TIME
               MessageInterface::ShowMessage
                  ("   ==========> Now handle output step size less than integrator step size\n");
               DebugWriteTime("   ===> nextOut = ", nextOut);
               #endif
               while (((nextOut <= epochInSecs) && currPropDirection == 1.0) ||
                      ((nextOut >= epochInSecs) && currPropDirection == -1.0))
               {
                  // Compute new output time
                  nextOut = nextOut + (stepSizeInSecs * currPropDirection);
                  AddNextEpochToWrite(nextOut, "   ===== Using fixed step: Adding nextOut to epochsOnWaiting, ");
                  nextOut = nextOutEpochInSecs;
               }
               retval = true;
            }
            else
            {
               retval = false;
            }
         }
         else if (interpolateFinalState)
         {
            if (epochInSecs >= finalEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY)
            {
               nextOutEpochInSecs = finalEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY;
               nextReqEpochInSecs = nextOutEpochInSecs;
               AddNextEpochToWrite(nextOutEpochInSecs,
                                   "   ===== Interpolating final state: Adding nextOutEpochInSecs to epochsOnWaiting, ");
               #ifdef DEBUG_EPHEMFILE_TIME
               MessageInterface::ShowMessage
                  ("   ===== current epoch passed final epoch, so setting %f to nextOutEpochInSecs\n",
                   nextOutEpochInSecs);
               #endif
            }
         }
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_TIME
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::IsTimeToWrite() returning %d\n\n", retval);
   DebugWriteTime("   nextOutEpochInSecs = ", nextOutEpochInSecs);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// void WriteOrbitAt(Real reqEpochInSecs, const Real state[6],
//                   const Real cov[21], const Real accel[3])
//------------------------------------------------------------------------------
/**
 * Writes spacecraft orbit data to a ephemeris file at requested epoch
 *
 * @param reqEpochInSecs Requested epoch to write state in seconds
 * @param state State to write 
 * @param cov Covariance to write
 * @param accel    Acceleration to write
 */
//------------------------------------------------------------------------------
void EphemWriterWithInterpolator::WriteOrbitAt(Real reqEpochInSecs, const Real state[6],
                                    const Real cov[21], const Real accel[3], const Real quat[4])
{
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::WriteOrbitAt() <%p> entered, writingNewSegment=%d\n"
       "   reqEpochInSecs=%.15f, %s, state[0]=%.15f\n", this, writingNewSegment, reqEpochInSecs,
       ToUtcGregorian(reqEpochInSecs).c_str(), state[0]);
   #endif
   
   if (writingNewSegment)
   {
      // If it needs to interpolate initial state, process it
      if (interpolateInitialState)
      {
         #ifdef DEBUG_EPHEMFILE_ORBIT
         MessageInterface::ShowMessage
            ("   ===> Need to interpolate initial state, so calling ProcessEpochsOnWaiting()\n");
         #endif
         ProcessEpochsOnWaiting(false, false);
      }
      else
         WriteOrbit(reqEpochInSecs, state, cov, accel, quat);
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_ORBIT
      MessageInterface::ShowMessage
         ("WriteOrbitAt() Not writing new segment, so Calling ProcessEpochsOnWaiting()\n");
      #endif
      // Process epochs on waiting
      ProcessEpochsOnWaiting(false, false);
   }
   
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage("EphemWriterWithInterpolator::WriteOrbitAt() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void ProcessFinalDataOnWaiting(bool canFinish = true)
//------------------------------------------------------------------------------
void EphemWriterWithInterpolator::ProcessFinalDataOnWaiting(bool canFinish)
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("\nEphemWriterWithInterpolator::ProcessFinalDataOnWaiting() entered, canFinish=%d, "
       "epochsOnWaiting.size()=%d\n", canFinish, epochsOnWaiting.size());
   MessageInterface::ShowMessage
      ("===> ProcessFinalDataOnWaiting() calling interpolator->SetForceInterpolation(true)\n");
   #endif
   interpolator->SetForceInterpolation(true);
   ProcessEpochsOnWaiting(true, !canFinish);
   #ifdef DEBUG_INTERPOLATOR_TRACE
   MessageInterface::ShowMessage
      ("===> ProcessFinalDataOnWaiting() calling interpolator->SetForceInterpolation(false)\n");
   #endif
   interpolator->SetForceInterpolation(false);
   
   // When running more than 5 days or so, the last epoch to process is a few
   // milliseconds after the last epoch received, so the interpolator flags
   // as epoch after the last buffered epoch, so handle last data point here.
   // If there is 1 epoch left and the difference between the current epoch
   // is less than timeTolerance then use the current epoch
   if (epochsOnWaiting.size() == 1)
   {
      Real lastEpoch = epochsOnWaiting.back();
      #ifdef DEBUG_EPHEMFILE_FINISH
      DebugWriteTime("   lastEpoch = ", lastEpoch);
      #endif
      if (GmatMathUtil::Abs(lastEpoch - currEpochInSecs) < timeTolerance)
      {
         #ifdef DEBUG_EPHEMFILE_FINISH
         DebugWriteTime
            ("   ===== Removing last epoch and adding currEpochInSecs to "
             "epochsOnWaiting, currEpochInSecs = ", currEpochInSecs);
         #endif
         epochsOnWaiting.pop_back();
         epochsOnWaiting.push_back(currEpochInSecs);
         #ifdef DEBUG_INTERPOLATOR_TRACE
         MessageInterface::ShowMessage
            ("===> ProcessFinalDataOnWaiting() calling interpolator->SetForceInterpolation(true)\n");
         #endif
         interpolator->SetForceInterpolation(true);
         ProcessEpochsOnWaiting(true, false);
         #ifdef DEBUG_INTERPOLATOR_TRACE
         MessageInterface::ShowMessage
            ("===> ProcessFinalDataOnWaiting() calling interpolator->SetForceInterpolation(false)\n");
         #endif
         interpolator->SetForceInterpolation(false);
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("Write last data received if finalizing, canFinish=%d, fileType=%d, "
       "useFixedStepSize=%d\n", canFinish, fileType, useFixedStepSize);
   #endif
   
   // Write last data received if not written yet(Do attitude later)
   if (canFinish)
   {
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage("   Checking if last received data should be written out...\n");
      #endif
      
      if (useFixedStepSize)
      {
         #ifdef DEBUG_EPHEMFILE_FINISH
         MessageInterface::ShowMessage("   finalEpochA1Mjd=%f\n", finalEpochA1Mjd);
         #endif
         // If not using user defined final epock, do more checking for the final data
         if (finalEpochA1Mjd == -999.999)
         {
            bool writeFinalData = false;
            #ifdef DEBUG_EPHEMFILE_FINISH
            DebugWriteTime("   currEpochInSecs    = ", currEpochInSecs);
            DebugWriteTime("   lastEpochWrote     = ", lastEpochWrote);
            DebugWriteTime("   nextReqEpochInSecs = ", nextReqEpochInSecs);
            MessageInterface::ShowMessage("   stepSizeInSecs     = %.12f\n", stepSizeInSecs);
            #endif
            // Check if current data needs to be written out
            if (fileType == CCSDS_OEM)
            {
               // For CCSDS, write out final data if current epoch is after last epoch
               if (currEpochInSecs > (lastEpochWrote + timeTolerance))
                  writeFinalData = true;
            }
            else if (fileType == STK_TIMEPOSVEL)
            {
                // For STK_TIMEPOSVEL, write out final data if current epoch is after last epoch
                if (currEpochInSecs > (lastEpochWrote + timeTolerance))
                    writeFinalData = true;
            }
            else if (fileType == CODE500_EPHEM)
            {
               // For Code500, write out final data if current epoch is same as the
               // next output epoch (LOJ: 2016.05.18)
               Real nextEpoch = lastEpochWrote + stepSizeInSecs;
               if (GmatMathUtil::Abs(currEpochInSecs - nextEpoch) < timeTolerance)
                  writeFinalData = true;
            }
            
            #ifdef DEBUG_EPHEMFILE_FINISH
            MessageInterface::ShowMessage
               ("   %s final data\n", writeFinalData ? "Write" : "Do not write");
            #endif
            if (writeFinalData)
            {
               #ifdef DEBUG_EPHEMFILE_FINISH
               MessageInterface::ShowMessage
                  ("   ===> Not using user defined final epoch, so writing final data\n");
               #endif
               WriteOrbit(currEpochInSecs, currState, currCov, currAccel, currQuat);
            }
         }
         else
         {
            #ifdef DEBUG_EPHEMFILE_FINISH
            MessageInterface::ShowMessage("   ===> process final data on waiting\n");
            #endif
            ProcessEpochsOnWaiting(false, false);
         }
      }
   }
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::ProcessFinalDataOnWaiting() leaving\n\n");
   #endif
}


//------------------------------------------------------------------------------
// void ProcessEpochsOnWaiting(bool checkFinalEpoch, bool checkEventEpoch)
//------------------------------------------------------------------------------
/*
 * Process epochs on waiting.
 *
 * @param checkFinalEpoch Set to true if checking for final epoch
 *
 */
//------------------------------------------------------------------------------
void EphemWriterWithInterpolator::ProcessEpochsOnWaiting(bool checkFinalEpoch,
                                                         bool checkEventEpoch)
{
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::ProcessEpochsOnWaiting() entered, checkFinalEpoch=%d, "
       "checkEventEpoch=%d, finalEpochReached=%d\n   prevPropDirection=%f, currPropDirection=%f, "
       "propIndicator=%d, epochsOnWaiting.size()=%d\n", checkFinalEpoch, checkEventEpoch,
       finalEpochReached, prevPropDirection, currPropDirection, propIndicator, epochsOnWaiting.size());
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   #ifdef DEBUG_EPHEMFILE_ORBIT_MORE
   DebugWriteEpochsOnWaiting("   ");
   #endif
   #endif
   
   // estimates contains position, velocity, acceleration, and lower triagular covariance matrix
   // Real estimates[6];
   Real estimates[34];
   Real stateEstimates[6];
   Real covEstimates[21];
   Real accelEstimates[3];
   Real quatEstimates[4];

   // Real fullEstimates[27];
   Real reqEpochInSecs = 0.0;
   Integer finishDirection = 1; // forward
   
   if (propIndicator == 2 || propIndicator == 4)
      finishDirection = 2; // backward
   else if (propIndicator == 0 && currPropDirection == -1.0)
      finishDirection = 2; // backward
   
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage("   finishDirection=%d\n", finishDirection);
   if (epochsOnWaiting.size() == 0)
      MessageInterface::ShowMessage("   ===> There are no epochs on waiting\n");
   #endif
   
   while (epochsOnWaiting.size() > 0)
   {
      reqEpochInSecs = epochsOnWaiting[0];
      
      #ifdef DEBUG_EPHEMFILE_ORBIT
      DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
      DebugWriteTime("    reqEpochInSecs = ", reqEpochInSecs);
      #endif
      
      // Do not write after the final epoch
      if (checkFinalEpoch)
      {
         Real tol = timeTolerance;
         if ( (((reqEpochInSecs + tol) > currEpochInSecs) && finishDirection == 1) ||
              (((reqEpochInSecs + tol) < currEpochInSecs) && finishDirection == 2) )
         {
            #ifdef DEBUG_EPHEMFILE_ORBIT
            MessageInterface::ShowMessage
               ("   =====> reqEpochInSecs %.15f %s currEpochInSecs %.15f so exiting while "
                "loop for %s prop\n", reqEpochInSecs, finishDirection == 1 ? ">" : "<",
                currEpochInSecs, finishDirection == 1 ? "forward" : "backward");
            #endif
            
            break;
         }
      }
      
      // We don't want to write epoch that matches event epoch which the beginning epoch
      // of the new segment
      if (checkEventEpoch)
      {
         if ( ((reqEpochInSecs >= eventEpochInSecs) && currPropDirection == 1.0) ||
              ((reqEpochInSecs <= eventEpochInSecs) && currPropDirection == -1.0) )
         {
            #ifdef DEBUG_EPHEMFILE_ORBIT
            MessageInterface::ShowMessage
               ("   =====> reqEpochInSecs %.15f %s eventEpochInSecs %.15f so exiting while "
                "loop for %s prop\n", reqEpochInSecs, currPropDirection == 1.0 ? ">=" : "<=",
                currEpochInSecs, currPropDirection == 1.0 ? "forward" : "backward");
            #endif
            break;
         }
      }
      
      #ifdef DEBUG_EPHEMFILE_ORBIT
      DebugWriteTime("   finalEpochA1Mjd = ", finalEpochA1Mjd, true);
      DebugWriteTime("   Checking to see if epoch is before user defined final epoch, "
                     "reqEpochInSecs = ", reqEpochInSecs);
      #endif
      
      if ((finalEpochA1Mjd != -999.999) && finalEpochReached &&
          (((reqEpochInSecs > finalEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY) && currPropDirection == 1.0) ||
           ((reqEpochInSecs < finalEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY) && currPropDirection == -1.0)) )
      {
         #ifdef DEBUG_EPHEMFILE_ORBIT
         MessageInterface::ShowMessage
            ("   =====> currEpochOnWaiting > User defined final epoch, so exiting while loop\n");
         #endif
         break;
      }
      
      #ifdef DEBUG_EPHEMFILE_ORBIT
      DebugWriteTime("   Checking to see if epoch is feasible reqEpochInSecs = ", reqEpochInSecs);
      #endif
      
      #ifdef DEBUG_INTERPOLATOR_TRACE
      MessageInterface::ShowMessage
         ("===> ProcessEpochsOnWaiting() calling interpolator->IsInterpolationFeasible(reqEpochInSecs)\n");
      #endif
      Integer retval = interpolator->IsInterpolationFeasible(reqEpochInSecs);
      interpolatorStatus = retval;
      
      // Attempt to lower order to make interpolation feasible
      if (retval == -1 && allowLowerOrder)
      {
         if (interpolator->AdjustOrderToNumPoints())
         {
            retval = interpolator->IsInterpolationFeasible(reqEpochInSecs);
            interpolatorStatus = retval;
         }
      }
      
      #ifdef DEBUG_EPHEMFILE_ORBIT
      MessageInterface::ShowMessage
         ("   =====> interpolation feasibility at reqEpochInSecs %.15f is %d\n",
          reqEpochInSecs, retval);
      #endif
      
      if (retval == 1)
      {
         // Now interpolate at epoch
         #ifdef DEBUG_EPHEMFILE_ORBIT
         MessageInterface::ShowMessage
            ("   =====> now try interpolating at epoch %.15f\n", reqEpochInSecs);
         #endif
         #ifdef DEBUG_INTERPOLATOR_TRACE
         MessageInterface::ShowMessage
            ("===> ProcessEpochsOnWaiting() calling interpolator->Interpolate(reqEpochInSecs, estimates)\n");
         #endif
         // if (interpolator->Interpolate(reqEpochInSecs, fullEstimates))
         if (interpolator->Interpolate(reqEpochInSecs, estimates))
         {
            // memcpy(&estimates, fullEstimates, sizeof (Real)*6);
            // memcpy(&covEstimates, fullEstimates +6, sizeof (Real) * 21);
            // WriteOrbit(reqEpochInSecs, estimates, covEstimates);

            for (Integer i = 0; i < 6; ++i)
               stateEstimates[i] = estimates[i];

            // This line of code causes data overflow
            //for (Integer i = 0; i < 6; ++i)
            for (Integer i = 0; i < 3; ++i)
               accelEstimates[i] = estimates[i+6];
            for (Integer i = 0; i < 21; ++i)
               covEstimates[i] = estimates[i+9];
            for (Integer i = 0; i < 4; ++i)
               quatEstimates[i] = estimates[i + 30];

            WriteOrbit(reqEpochInSecs, stateEstimates, covEstimates, accelEstimates, quatEstimates);
            RemoveEpochAlreadyWritten
               (reqEpochInSecs, "   =====> ProcessEpochsOnWaiting() now erasing ");
         }
         else
         {
            // Check if interpolation needs to be forced
            if (initialCount <= interpolationOrder/2)
            {
               initialCount++;
               
               #ifdef DEBUG_EPHEMFILE_ORBIT
               MessageInterface::ShowMessage
                  ("   =====> Forcing to interpolate at epoch %.15f, initialCount=%d\n",
                   reqEpochInSecs, initialCount);
               #endif
               
               // Since time should be in order, force process epochs on waiting.
               // First few request time can not be placed in the middle of the buffer.
               #ifdef DEBUG_INTERPOLATOR_TRACE
               MessageInterface::ShowMessage
                  ("===> ProcessEpochsOnWaiting() calling interpolator->SetForceInterpolation(true)\n");
               #endif
               interpolator->SetForceInterpolation(true);
               ProcessEpochsOnWaiting(false, false);
               #ifdef DEBUG_INTERPOLATOR_TRACE
               MessageInterface::ShowMessage
                  ("===> ProcessEpochsOnWaiting() calling interpolator->SetForceInterpolation(false)\n");
               #endif
               interpolator->SetForceInterpolation(false);
            }
            else
            {
               #ifdef DEBUG_EPHEMFILE_ORBIT
               MessageInterface::ShowMessage
                  ("   initialCount (%d) <= interpolationOrder/2 + 1 (%d)\n", initialCount,
                   interpolationOrder/2 + 1);
               DebugWriteTime
                  ("   =====> epoch failed to interpolate so exiting while loop, reqEpochInSecs = ",
                   reqEpochInSecs);
               #endif
               break;
            }
         }
      }
      else
      {
         // If epoch is after the last data, collect number of order points
         // and process before epoch becomes out of the first data range
         if (retval ==  -3)
         {
            #ifdef DEBUG_EPHEMFILE_ORBIT
            MessageInterface::ShowMessage("   Setting processingLargeStep to true\n");
            #endif
            processingLargeStep = true;
         }
         
         // @todo Is there more checking needs here?
         #ifdef DEBUG_EPHEMFILE_ORBIT
         DebugWriteTime
            ("   =====> epoch is not feasible so exiting while loop, reqEpochInSecs = ", reqEpochInSecs);
         #endif
         break;
      }
      
      // Note: Don't need iter++, since the data is erased as it is written to a file,
      // otherwise it will skip one data.
   }
   
   #ifdef DEBUG_EPHEMFILE_ORBIT
   DebugWriteEpochsOnWaiting("   ");
   MessageInterface::ShowMessage
      ("EphemWriterWithInterpolator::ProcessEpochsOnWaiting() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// RealArray::iterator FindEpochOnWaiting(Real epochInSecs, const std::string &msg = "")
//------------------------------------------------------------------------------
/**
 * Finds epoch from epochsOnWaiting list.
 * It uses timeTolerance tolerance to find matching epoch.
 */
//------------------------------------------------------------------------------
RealArray::iterator EphemWriterWithInterpolator::FindEpochOnWaiting(Real epochInSecs,
                                                                      const std::string &msg)
{
   #ifdef DEBUG_FIND_EPOCH
   MessageInterface::ShowMessage("FindEpochOnWaiting() entered\n");
   DebugWriteTime("   epochInSecs = ", epochInSecs);
   DebugWriteEpochsOnWaiting();
   #endif
   
   // Find matching epoch
   RealArray::iterator iterFound = epochsOnWaiting.begin();
   while (iterFound != epochsOnWaiting.end())
   {
      #ifdef DEBUG_FIND_EPOCH
      DebugWriteTime("      iterFound, epoch = ", *iterFound);
      #endif
      
      if (GmatMathUtil::Abs(*iterFound - epochInSecs) < timeTolerance)
      {
         #ifdef DEBUG_EPHEMFILE_ORBIT
         DebugWriteTime(msg + " epoch = ", *iterFound);
         #endif
         return iterFound;
      }
      iterFound++;
   }
   
   return epochsOnWaiting.end();
}


//------------------------------------------------------------------------------
// void RemoveEpochAlreadyWritten(Real epochInSecs, const std::string &msg = "")
//------------------------------------------------------------------------------
/**
 * Erases epoch already processed from epochsOnWaiting list. It uses timeTolerance
 * tolerance to find matching epoch.
 */
//------------------------------------------------------------------------------
void EphemWriterWithInterpolator::RemoveEpochAlreadyWritten(Real epochInSecs,
                                                            const std::string &msg)
{
   // Find matching epoch
   RealArray::iterator iterFound = epochsOnWaiting.begin();
   while (iterFound != epochsOnWaiting.end())
   {
      if (GmatMathUtil::Abs(*iterFound - epochInSecs) < timeTolerance)
      {
         #ifdef DEBUG_EPHEMFILE_ORBIT
         DebugWriteTime(msg + " epoch = ", *iterFound);
         #endif
         // erase returns the next one
         iterFound = epochsOnWaiting.erase(iterFound);
      }
      else
         ++iterFound;
   }
}


//------------------------------------------------------------------------------
// void AddNextEpochToWrite(Real epochInSecs, const std::string &msg);
//------------------------------------------------------------------------------
/**
 * Adds epoch to write to epochsOnWaiting list. It uses timeTolerance
 * tolerance to find matching epoch.
 */
//------------------------------------------------------------------------------
void EphemWriterWithInterpolator::AddNextEpochToWrite(Real epochInSecs,
                                                      const std::string &msg)
{
   if (FindEpochOnWaiting(epochInSecs, msg) == epochsOnWaiting.end())
   {
      #ifdef DEBUG_EPHEMFILE_TIME
      DebugWriteTime(msg + " epochInSecs = ", epochInSecs);
      #endif
      
      // Check if epoch is in leap second for Code500
      if (fileType == CODE500_EPHEM)
      {
         Real a1Mjd = epochInSecs / 86400.0;
         Real taiMjd = TimeSystemConverter::Instance()->ConvertToTaiMjd(
               TimeSystemConverter::A1MJD, a1Mjd, GmatTimeConstants::JD_NOV_17_1858,
               &isNextOutputEpochInLeapSecond);
         
         if (isNextOutputEpochInLeapSecond)
         {
            #ifdef DEBUG_EPHEMFILE_TIME
            MessageInterface::ShowMessage
               ("=====> EphemWriterWithInterpolator::AddNextEpochToWrite() epochInSecs "
                "%.15f is in leap second\n   So adding one time extra 1 sec to epochInSecs "
                "for Code500\n",
                epochInSecs);
            DebugWriteTime("   ", epochInSecs + 1.0);
            #endif
            epochsOnWaiting.push_back(epochInSecs + 1.0);
            nextOutEpochInSecs = epochInSecs + 1.0;
            isNextOutputEpochInLeapSecond = false;
         }
         else
         {
            epochsOnWaiting.push_back(epochInSecs);
            nextOutEpochInSecs = epochInSecs;
         }
      }
      else
      {
         epochsOnWaiting.push_back(epochInSecs);
         nextOutEpochInSecs = epochInSecs;
      }
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_TIME
      DebugWriteTime("   ========== skipping redundant epochInSecs = ", epochInSecs);
      #endif
   }
}


//------------------------------------------------------------------------------
// void FormatErrorMessage(std::string &ephemMsg, std::string &errMsg)
//------------------------------------------------------------------------------
void EphemWriterWithInterpolator::FormatErrorMessage(std::string &ephemMsg, std::string &errMsg)
{
   std::string commonMsg = "There is not enough data available to generate the current block of "
      "ephemeris";
   std::string ephemFileStr = " to EphemerisWriter: \"" + fileName + "\"";

   Real timeSpanInSecs = (currEpochInDays - blockBeginA1Mjd) * GmatTimeConstants::SECS_PER_DAY;

#ifdef DEBUG_TIME_SPAN
   DebugWriteTime("=> blockBeginA1Mjd = ", blockBeginA1Mjd, true, 2);
   DebugWriteTime("=> currEpochInDays = ", currEpochInDays, true, 2);
   MessageInterface::ShowMessage("=> timeSpanInSecs  = %f\n", timeSpanInSecs);
#endif

   // Format error message
   if (initialEpochA1Mjd != -999.999 && (currEpochInDays < initialEpochA1Mjd))
   {
      //std::string initEpochStr = ToUtcGregorian(initialEpochA1Mjd, true, 2);
      std::string currentEpochStr = ToUtcGregorian(currEpochInDays, true, 2);
      std::string detailedMsg = ". The block ended at " + currentEpochStr + "(" +
         GmatStringUtil::ToString(currEpochInDays) + ") before the user defined initial epoch of " +
         initialEpochStr + "(" + GmatStringUtil::ToString(initialEpochA1Mjd) + ").";
      ephemMsg = commonMsg + detailedMsg;
      errMsg = commonMsg + ephemFileStr + detailedMsg;
   }
   else if (timeSpanInSecs < stepSizeInSecs)
   {
      std::string blockBeginEpochStr = ToUtcGregorian(blockBeginA1Mjd, true, 2);
      std::string currentEpochStr = ToUtcGregorian(currEpochInDays, true, 2);
      std::string detailedMsg = ".  The data time span (" + blockBeginEpochStr + " - " +
         currentEpochStr + ") is less than the step size of " +
         GmatStringUtil::ToString(stepSizeInSecs, 2, true) + " seconds.";
      ephemMsg = commonMsg + detailedMsg;
      errMsg = commonMsg + ephemFileStr + detailedMsg;
   }
   else
   {
      std::string detailedMsg1 = " at the requested interpolation order or to "
         "temporarily reduce the order. ";
      std::string detailedMsg2 = "Number of required points is " +
         GmatStringUtil::ToString(interpolationOrder + 1, 1) + " for the requested "
         "order, or at least 2 points for a reduced order, but received " +
         GmatStringUtil::ToString(interpolator->GetPointCount(), 1) + ". ";
      std::string detailedMsg3 = "There should be at least one data point more than interpolation order.";
      ephemMsg = commonMsg + detailedMsg1 + detailedMsg3;
      errMsg = commonMsg + ephemFileStr + detailedMsg1 + detailedMsg2 + detailedMsg3;
   }
}


//------------------------------------------------------------------------------
// void DebugWriteEpochsOnWaiting(const std::string &msg = "")
//------------------------------------------------------------------------------
void EphemWriterWithInterpolator::DebugWriteEpochsOnWaiting(const std::string &msg)
{
   MessageInterface::ShowMessage
      ("%sThere are %d epochs on waiting\n", msg.c_str(), epochsOnWaiting.size());
   for (UnsignedInt i = 0; i < epochsOnWaiting.size(); i++)
      DebugWriteTime("      ", epochsOnWaiting[i]);
}

