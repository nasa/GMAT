//$Id$
//------------------------------------------------------------------------------
//                                  EphemWriterCode500
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
// Created: 2015.10.27
//
/**
 * Writes a spacecraft orbit states or attitude to an ephemeris file in
 * Code-500 format.
 */
//------------------------------------------------------------------------------

#include "EphemWriterCode500.hpp"
#include "SubscriberException.hpp"   // for exception
#include "TimeSystemConverter.hpp"   // For IsInLeapSecond()
#include "MessageInterface.hpp"


//#define DEBUG_EPHEMFILE_INSTANCE
//#define DEBUG_EPHEMFILE_INIT
//#define DEBUG_EPHEMFILE_CREATE
//#define DEBUG_EPHEMFILE_DATA
//#define DEBUG_EPHEMFILE_CODE500
//#define DEBUG_CODE500_DATA_SEGMENT
//#define DEBUG_EPHEMFILE_BUFFER
//#define DEBUG_EPHEMFILE_WRITE
//#define DEBUG_EPHEMFILE_FINISH
//#define DEBUG_EPHEMFILE_RESTART
//#define DEBUG_LEAP_SECOND
//#define DEBUG_EPHEMFILE_READBACK

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------


//------------------------------------------------------------------------------
// EphemWriterCode500(const std::string &name, const std::string &type = "EphemWriterCode500")
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EphemWriterCode500::EphemWriterCode500(const std::string &name, const std::string &type) :
   EphemWriterWithInterpolator(name, type),
   code500EphemFile     (NULL),
   code500WriteFailed   (true)
{
   fileType = CODE500_EPHEM;
}


//------------------------------------------------------------------------------
// ~EphemWriterCode500()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EphemWriterCode500::~EphemWriterCode500()
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterCode500::~EphemWriterCode500() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
      
   // Delete Code500 ephemeris
   if (code500EphemFile)
      delete code500EphemFile;
   
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterCode500::~EphemWriterCode500() <%p>'%s' leaving\n", this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// EphemWriterCode500(const EphemWriterCode500 &ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EphemWriterCode500::EphemWriterCode500(const EphemWriterCode500 &ef) :
   EphemWriterWithInterpolator(ef),
   code500EphemFile     (NULL),
   code500WriteFailed   (ef.code500WriteFailed)
{
   coordConverter = ef.coordConverter;
}


//------------------------------------------------------------------------------
// EphemWriterCode500& EphemWriterCode500::operator=(const EphemWriterCode500& ef)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
EphemWriterCode500& EphemWriterCode500::operator=(const EphemWriterCode500& ef)
{
   if (this == &ef)
      return *this;
   
   EphemWriterWithInterpolator::operator=(ef);
   
   code500EphemFile     = NULL;
   code500WriteFailed   = ef.code500WriteFailed;
   
   return *this;
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool EphemWriterCode500::Initialize()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemWriterCode500::Initialize() <%p>'%s' entered, spacecraftName='%s', "
       "isInitialized=%d\n   ephemType='%s', stateType='%s', "
       "outputFormat='%s'\n", this, ephemName.c_str(), spacecraftName.c_str(), 
       isInitialized, ephemType.c_str(), stateType.c_str(),
       outputFormat.c_str());
   #endif
   
   if (isInitialized)
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("EphemWriterCode500::Initialize() <%p>'%s' is already initialized so just returning true\n",
          this, ephemName.c_str());
      #endif
      return true;
   }
   
   EphemWriterWithInterpolator::Initialize();
   
   // Set maximum segment size
   maxSegmentSize = 50; // 50 orbit states per data record
   
   // Set default step size
   if (stepSizeInSecs == -999.999)
      stepSizeInSecs = 60.0;
   
   // Set Code500 header data
   SetCode500HeaderData();
   
   // Always create interpolator for Code500
   CreateInterpolator();
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemWriterCode500::Initialize() <%p>'%s' returning true, writeOrbit=%d, writeAttitude=%d, "
       "useFixedStepSize=%d,\n   writeDataInDataCS=%d, initialEpochA1Mjd=%.9f, "
       "finalEpochA1Mjd=%.9f, stepSizeInSecs=%.9f\n", this, ephemName.c_str(), writeOrbit,
       writeAttitude, useFixedStepSize, writeDataInDataCS, initialEpochA1Mjd, finalEpochA1Mjd,
       stepSizeInSecs);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  EphemerisWriter* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EphemWriterCode500.
 *
 * @return clone of the EphemWriterCode500.
 *
 */
//------------------------------------------------------------------------------
EphemerisWriter* EphemWriterCode500::Clone(void) const
{
   return (new EphemWriterCode500(*this));
}


//---------------------------------------------------------------------------
// void Copy(const EphemerisWriter* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void EphemWriterCode500::Copy(const EphemerisWriter* orig)
{
   operator=(*((EphemWriterCode500 *)(orig)));
}


//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
// void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21], const Real accel[3])
//------------------------------------------------------------------------------
void EphemWriterCode500::BufferOrbitData(Real epochInDays, const Real state[6],
                                         const Real cov[21], const Real accel[3],
                                         const Real quat[4])
{
   #ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
      ("EphemWriterCode500::BufferOrbitData() <%p>'%s' entered, epochInDays=%.15f, "
       "state[0]=%.15f\n", this, ephemName.c_str(), epochInDays, state[0]);
   DebugWriteTime("   epochInDays = ", epochInDays, true, 2);
   #endif
   
   // if buffer is full, dump the data
   if (a1MjdArray.size() >= maxSegmentSize)
      WriteCode500OrbitDataSegment(false);
   
   A1Mjd *a1mjd = new A1Mjd(epochInDays);
   a1MjdArray.push_back(a1mjd);
   
   Rvector6 *rv6 = new Rvector6(state);
   stateArray.push_back(rv6);

   Rvector3 *acc = new Rvector3(accel[0], accel[1], accel[2]);
   accelArray.push_back(acc);
   Rvector* covar = new Rvector(21);
   for (Integer i = 0; i < 21; ++i)
      (*covar)[i] = cov[i];
   covArray.push_back(covar);
   Rvector* rvacov = new Rvector(30);
   for (Integer i = 0; i < 6; ++i)
      (*rvacov)[i] = state[i];
   for (Integer i = 0; i < 3; ++i)
      (*rvacov)[i] = accel[i];
   for (Integer i = 0; i < 21; ++i)
      (*rvacov)[i+9] = cov[i];
   rvacovArray.push_back(rvacov);

   #ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
      ("BufferOrbitData() <%p>'%s' leaving, there is(are) %d data point(s)\n",
       this, ephemName.c_str(), a1MjdArray.size());
   #endif
} // BufferOrbitData()


//------------------------------------------------------------------------------
// void CreateEphemerisFile(bool useDefaultFileName, const std::string &stType,
//                          const std::string &outFormat, const std::string &covFormat)
//------------------------------------------------------------------------------
void EphemWriterCode500::CreateEphemerisFile(bool useDefaultFileName,
                                             const std::string &stType,
                                             const std::string &outFormat,
                                             const std::string &covFormat)
{
   EphemerisWriter::CreateEphemerisFile(useDefaultFileName, stType, outFormat, covFormat);
   CreateCode500EphemerisFile();
   isEphemFileOpened = true;
}


//------------------------------------------------------------------------------
// void CreateCode500EphemerisFile()
//------------------------------------------------------------------------------
void EphemWriterCode500::CreateCode500EphemerisFile()
{
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("\nEphemWriterCode500::CreateCode500EphemerisFile() this=<%p>'%s' entered, code500EphemFile=<%p>,"
       " outputFormat='%s'\n", this, ephemName.c_str(), code500EphemFile, outputFormat.c_str());
   #endif
   
   // If code500EphemFile is not NULL, delete it first
   if (code500EphemFile != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (code500EphemFile, "code500EphemFile", "EphemWriterCode500::CreateCode500EphemerisFile()",
          "deleting local code500EphemFile");
      #endif
      delete code500EphemFile;
      code500EphemFile = NULL;
   }
   
   Real        satId       = 101.0;   // dummy for now
   std::string timeSystem  = "UTC";   // Figure out time system here
   std::string sourceId    = "GTDS";
   //std::string centralBody = spacecraft->GetOriginName();
   std::string centralBody = outCoordSystem->GetOriginName();
   
   Integer coordSystem = 4;
   if (outCoordSystem->AreAxesOfType("MJ2000EqAxes"))
      coordSystem = 4;
   else if (outCoordSystem->AreAxesOfType("BodyFixedAxes"))
      coordSystem = 5;
   else if (outCoordSystem->AreAxesOfType("TrueOfDateAxes"))
      coordSystem = 3;

   int ephemOutputFormat = 1;
   if (outputFormat == "BigEndian")
      ephemOutputFormat = 2;
   
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("   Creating Code500EphemerisFile with satId=%f, timeSystem='%s', sourceId='%s', "
       "centralBody='%s', coordSystem='%s', ephemOutputFormat=%d\n", satId, timeSystem.c_str(),
       sourceId.c_str(), centralBody.c_str(), coordSystem.c_str(), ephemOutputFormat);
   #endif
   
   try
   {
      code500EphemFile =
         new Code500EphemerisFile(fullPathFileName, satId, timeSystem, sourceId,
                                  centralBody, coordSystem, 2, ephemOutputFormat);
      
      // Set origin mu to code500 ephem so that it can do conversion
      SpacePoint *origin = outCoordSystem->GetOrigin();
      if (!origin->IsOfType("CelestialBody"))
      {
         SubscriberException se;
         se.SetDetails("Output coordinate system for Code500 ephemeris file must"
                       "have a celestial body origin\n");
         throw se;
      }
      
      Real cbMu = ((CelestialBody*) origin)->GetGravitationalConstant();
      #ifdef DEBUG_EPHEMFILE_CREATE
      MessageInterface::ShowMessage
         ("   Setting initial mu=%f, stepSizeInSecs=%f to code500EphemFile\n",
          cbMu, stepSizeInSecs);
      #endif
      code500EphemFile->SetCentralBodyMu(cbMu);
      code500EphemFile->SetTimeIntervalBetweenPoints(stepSizeInSecs);
   }
   catch (BaseException &e)
   {
      // Keep from getting a compiler warning about e not being used
      e.GetMessageType();
      
      #ifdef DEBUG_EPHEMFILE_CREATE
      MessageInterface::ShowMessage
         ("Error opening Code500EphemerisFile: %s", (e.GetFullMessage()).c_str());
      #endif
      throw;
   }
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (code500EphemFile, "code500EphemFile", "EphemWriterCode500::CreateCode500EphemerisFile()",
       "code500EphemFile = new SpiceOrbitKernelWriter()");
   #endif
   
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("EphemWriterCode500::CreateCode500EphemerisFile() <%p>'%s' leaving, code500EphemFile=<%p>\n",
       this, ephemName.c_str(), code500EphemFile);
   #endif
}


//------------------------------------------------------------------------------
// bool NeedToHandleBackwardProp()
//------------------------------------------------------------------------------
/**
 * Checks if backward prop is allowed or don't need special handling.
 *
 * @return  false  if no special handling is needed
 *          true   if need to procced to next step
 */
//------------------------------------------------------------------------------
bool EphemWriterCode500::NeedToHandleBackwardProp()
{
   #if DBGLVL_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemWriterCode500::NeedToHandleBackwardProp() entered\n");
   #endif
   
   // Throw an exception for CODE-500 as backward prop is not allowed
   throw SubscriberException(GetBackwardPropWarning());
}


//------------------------------------------------------------------------------
// void HandleOrbitData()
//------------------------------------------------------------------------------
void EphemWriterCode500::HandleOrbitData()
{
   // Check user defined initial and final epoch
   bool processData = CheckInitialAndFinalEpoch();
   
   #if DBGLVL_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemWriterCode500::HandleOrbitData() checked initial and final epoch\n");
   #endif
   
   // Check if it is time to write
   bool timeToWrite = IsTimeToWrite(currEpochInSecs, currState, currCov, currAccel);
   
   #if DBGLVL_EPHEMFILE_DATA > 0
   MessageInterface::ShowMessage
      ("   Start writing data, currEpochInDays=%.15f, currEpochInSecs=%.15f, %s\n"
       "   writeOrbit=%d, writeAttitude=%d, processData=%d, timeToWrite=%d\n",
       currEpochInDays, currEpochInSecs, ToUtcGregorian(currEpochInSecs).c_str(),
       writeOrbit, writeAttitude, processData, timeToWrite);
   #endif
   
   // For now we only write Orbit data
   HandleCode500OrbitData(processData, timeToWrite);
}


//------------------------------------------------------------------------------
// void StartNewSegment(const std::string &comments, bool saveEpochInfo,
//                      bool writeAfterData, bool ignoreBlankComments)
//------------------------------------------------------------------------------
/**
 * Finishes writing remaining data and resets flags to start new segment.
 */
//------------------------------------------------------------------------------
void EphemWriterCode500::StartNewSegment(const std::string &comments,
                                         bool saveEpochInfo, bool writeAfterData,
                                         bool ignoreBlankComments)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterCode500::StartNewSegment() entered\n   comments='%s'\n   "
       "saveEpochInfo=%d, writeAfterData=%d, ignoreBlankComments=%d, canFinalize=%d, firstTimeWriting=%d\n",
       comments.c_str(), saveEpochInfo, writeAfterData, ignoreBlankComments, canFinalize, firstTimeWriting);
   #endif
   
   // If no first data has written out yet, just return
   if (firstTimeWriting)
   {
      #ifdef DEBUG_EPHEMFILE_RESTART
      MessageInterface::ShowMessage
         ("EphemWriterCode500::StartNewSegment() returning, no first data written out yet\n");
      #endif
      return;
   }
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("EphemWriterCode500::StartNewSegment() Calling FinishUpWriting(), canFinalize=%d\n",
       canFinalize);
   #endif
   
   // Write data for the rest of times on waiting
   FinishUpWriting();
   
   // Set comments
   writeCommentAfterData = writeAfterData;
   currComments = comments;
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   WriteComments(comments, ignoreBlankComments);
   #endif
   
   if (code500EphemFile != NULL)
      WriteCode500OrbitDataSegment(false);
   
   // Initialize data
   InitializeData(saveEpochInfo);
   
   ++numSegments;

   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterCode500::StartNewSegment() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void FinishUpWriting()
//------------------------------------------------------------------------------
/*
 * Finishes up writing data at epochs on waiting.
 *
 * @param  canFinalize  If this flag is true it will process epochs on waiting
 *                      and write or overwrite metadata depends on the metadata
 *                      write option [true]
 */
//------------------------------------------------------------------------------
void EphemWriterCode500::FinishUpWriting()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterCode500::FinishUpWriting() '%s' entered, canFinalize=%d, isFinalized=%d, "
       "firstTimeWriting=%d\n   interpolatorStatus=%d, isEndOfRun=%d\n",
       ephemName.c_str(), canFinalize, isFinalized, firstTimeWriting, interpolatorStatus,
       isEndOfRun);
   DebugWriteTime("    lastEpochWrote = ", lastEpochWrote);
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   #ifdef DEBUG_EPHEMFILE_FINISH_MORE
   DebugWriteEpochsOnWaiting("   ");
   #endif
   MessageInterface::ShowMessage
      ("   There is(are) %d data point(s) in the buffer\n", a1MjdArray.size());
   #endif
   
   if (!isFinalized)
   {
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage
         ("   It is not finalized yet, so trying to write the remainder of data\n");
      #endif
      
      FinishUpWritingCode500();
      
      if (canFinalize)
      {
         if (isEndOfRun)
         {
            // Close ephemeris file (GMT-4049 fix)
            CloseEphemerisFile();
            
            // Check for user defined final epoch (GMT-4108 fix)
            if (finalEpochA1Mjd != -999.999)
            {
               if (currEpochInDays < finalEpochA1Mjd)
               {
                  MessageInterface::ShowMessage
                     ("*** WARNING *** Run ended at %f before the user defined final epoch of %f\n",
                      currEpochInDays, finalEpochA1Mjd);
               }
            }
         }
         
         isFinalized = true;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterCode500::FinishUpWriting() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void HandleCode500OrbitData(bool writeData, bool timeToWrite)
//------------------------------------------------------------------------------
void EphemWriterCode500::HandleCode500OrbitData(bool writeData, bool timeToWrite)
{
   #ifdef DEBUG_EPHEMFILE_CODE500
   MessageInterface::ShowMessage
      ("EphemWriterCode500::HandleCode500OrbitData() <%p>'%s' entered, writeData=%d, "
       "currEpochInDays = %.13lf, \n   firstTimeWriting=%d, writingNewSegment=%d\n",
       this, ephemName.c_str(), writeData, currEpochInDays, firstTimeWriting, writingNewSegment);
   #endif
   
   // LagrangeInterpolator's maximum buffer size is set to 80 which can hold
   // 80 min of data assuming average of 60 sec data interveval.
   // Check at least 10 min interval for large step size, since interpolater
   // buffer size is limited
   if (!timeToWrite)
   {
      if ((currEpochInSecs - prevProcTime) > 600.0)
      {
         #ifdef DEBUG_EPHEMFILE_CODE500
         MessageInterface::ShowMessage
            ("   ==> 10 min interval is over, so setting timeToWrite to true\n");
         #endif
         
         timeToWrite = true;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_CODE500
   MessageInterface::ShowMessage
      ("   timeToWrite=%d, writingNewSegment=%d\n", timeToWrite, writingNewSegment);
   #endif
   
   if (timeToWrite)
      prevProcTime = currEpochInSecs;
   
   //------------------------------------------------------------
   // write data to file
   //------------------------------------------------------------
   // Now actually write data
   if (writeData && timeToWrite)
   {
      // if (firstTimeWriting)
      //    WriteHeader();
      
      if (writingNewSegment)
      {
         //WriteComments("********** NEW SEGMENT **********");
         #ifdef DEBUG_EPHEMFILE_WRITE
         DebugWriteTime
            ("********** WRITING NEW SEGMENT AT currEpochInSecs = ", currEpochInSecs, false, 2);
         #endif
         
         WriteCode500OrbitDataSegment(false);
      }
      
      if (writeOrbit)
         HandleWriteOrbit();
      
      if (firstTimeWriting)
         firstTimeWriting = false;
      
      if (writingNewSegment)
         writingNewSegment = false;
   }
   
   #ifdef DEBUG_EPHEMFILE_CODE500
   MessageInterface::ShowMessage
      ("EphemWriterCode500::HandleCode500OrbitData() leaving, firstTimeWriting=%d, writingNewSegment=%d\n",
       firstTimeWriting, writingNewSegment);
   #endif
}


//------------------------------------------------------------------------------
// void FinishUpWritingCode500()
//------------------------------------------------------------------------------
void EphemWriterCode500::FinishUpWritingCode500()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterCode500::FinishUpWritingCode500() entered, canFinalize=%d\n", canFinalize);
   #endif
   
   if (interpolator != NULL)
   {
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage
         ("===> FinishUpWritingCode500() checking for not enough data points\n"
          "   currEpochInSecs='%s'\n", ToUtcGregorian(currEpochInSecs).c_str());
      #endif
      
      // First check for not enough data points for interpolation
      if (canFinalize && interpolatorStatus == -1)
      {
         // Attempt to lower the order required to write data if needed
         Integer numPoints = interpolator->GetPointCount();

         if (numPoints < (interpolationOrder + 1))
            MessageInterface::ShowMessage("WARNING: Not enough points provided to "
               "the " + interpolatorName + " interpolator of order " +
               GmatStringUtil::ToString(interpolationOrder, 0) + " for ephemeris "
               "file \"" + fileName + "\", segment " +
               GmatStringUtil::ToString(numSegments, 0) + ". The order will be "
               "reduced to " + GmatStringUtil::ToString(numPoints - 1, 0) +
               " for this segment.\n");

         allowLowerOrder = true;
         ProcessEpochsOnWaiting(false, false);
         allowLowerOrder = false;

         if (interpolatorStatus == -1)
         {
         isFinalized = true;
         std::string ephemMsg, errMsg;
         FormatErrorMessage(ephemMsg, errMsg);
         throw SubscriberException(errMsg);
         }
      }
      
      // Process final data on waiting to be output
      ProcessFinalDataOnWaiting();
   }
   
   // Write final data
   if (code500EphemFile != NULL)
   {
      WriteCode500OrbitDataSegment(canFinalize);
      if (canFinalize)
         FinalizeCode500Ephemeris();
   }
   else
   {
      if (a1MjdArray.size() > 0)
      {
         throw SubscriberException
            ("*** INTERNAL ERROR *** Code500EphemFile is NULL in "
             "EphemWriterCode500::FinishUpWritingCode500()\n");
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemWriterCode500::FinishUpWritingCode500() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void SetCode500HeaderData()
//------------------------------------------------------------------------------
void EphemWriterCode500::SetCode500HeaderData()
{
   if (code500EphemFile != NULL)
      code500EphemFile->SetTimeIntervalBetweenPoints(stepSizeInSecs);
}

//------------------------------------------------------------------------------
// void WriteCode500OrbitDataSegment(bool canFinish)
//------------------------------------------------------------------------------
/**
 * Writes orbit data segment to Code500 file and deletes data array
 */
//------------------------------------------------------------------------------
void EphemWriterCode500::WriteCode500OrbitDataSegment(bool canFinish)
{
   #ifdef DEBUG_CODE500_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("=====> WriteCode500OrbitDataSegment() <%p>'%s' entered, canFinish=%d, "
       "isEndOfRun=%d, a1MjdArray.size()=%d, stateArray.size()=%d\n", this,
       ephemName.c_str(), canFinish, isEndOfRun, a1MjdArray.size(), stateArray.size());
   #endif
   
   if (a1MjdArray.size() > 0)
   {
      if (code500EphemFile == NULL)
         throw SubscriberException
            ("*** INTERNAL ERROR *** Code500 Ephem Writer is NULL in "
             "EphemWriterCode500::WriteCode500OrbitDataSegment()\n");
      
      #ifdef DEBUG_CODE500_DATA_SEGMENT
      A1Mjd *start = a1MjdArray.front();
      A1Mjd *end   = a1MjdArray.back();
      MessageInterface::ShowMessage
         ("   Writing start=%.15f, end=%.15f\n", start->GetReal(), end->GetReal());
      MessageInterface::ShowMessage("There are %d epochs and states:\n", a1MjdArray.size());
      for (unsigned int ii = 0; ii < a1MjdArray.size(); ii++)
      {
         A1Mjd *tm = a1MjdArray[ii];
         Real time = tm->GetReal();
         Rvector6 *sv = stateArray[ii];
         std::string format = "% 1.15e  ";
         MessageInterface::ShowMessage
            ("[%3d] %12.10f  %s  %s\n", ii, time, ToUtcGregorian(time, true).c_str(),
             (sv->ToString(format, 6)).c_str());
      }
      #endif
      
      code500WriteFailed = false;
      try
      {
         #ifdef DEBUG_CODE500_DATA_SEGMENT
         MessageInterface::ShowMessage
            (".....Calling code500EphemFile->WriteDataSegment() isEndOfRun=%d, "
             "canFinish=%d, epochsOnWaiting.size()=%d\n", isEndOfRun, canFinish,
             epochsOnWaiting.size());
         DebugWriteEpochsOnWaiting("   ");
         //DebugWriteTime("   lastEpochOnWaiting = ", epochsOnWaiting.back());
         #endif
         // Check if Code500 ephemeris file can be finalized (GMT-4060 fix)
         bool finalize = isEndOfRun && canFinish;
         //code500EphemFile->WriteDataSegment(a1MjdArray, stateArray, isEndOfRun);
         code500EphemFile->WriteDataSegment(a1MjdArray, stateArray, finalize);
         ClearOrbitData();
      }
      catch (BaseException &e)
      {
         ClearOrbitData();
         code500WriteFailed = true;
         #ifdef DEBUG_CODE500_DATA_SEGMENT
         MessageInterface::ShowMessage("**** ERROR **** " + e.GetFullMessage());
         #endif
         e.SetFatal(true);
         throw;
      }
   }
   
   #ifdef DEBUG_CODE500_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("=====> WriteCode500OrbitDataSegment() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// FinalizeCode500Ephemeris()
//------------------------------------------------------------------------------
void EphemWriterCode500::FinalizeCode500Ephemeris()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemWriterCode500::FinalizeCode500Ephemeris() entered\n");
   #endif
   
   if (code500EphemFile == NULL)
      throw SubscriberException
         ("*** INTERNAL ERROR *** Code500 Ephem Writer is NULL in "
          "EphemWriterCode500::FinalizeCode500Ephemeris()\n");
   
   // Write any final header data
   code500EphemFile->FinalizeHeaders();
   
   
   #ifdef DEBUG_EPHEMFILE_READBACK
   if (isEndOfRun)
   {
      MessageInterface::ShowMessage
         ("===> EphemWriterCode500::FinalizeCode500Ephemeris() calling code500EphemFile "
          "for debug output\n   fullPathFileName = '%s'\n", fullPathFileName.c_str());
      bool swapByteOrder = false;
      if (outputFormat == "BigEndian")
         swapByteOrder = true;
      code500EphemFile->OpenForRead(fullPathFileName, 1, 1);
      code500EphemFile->SetSwapEndian(swapByteOrder, 1);
      code500EphemFile->ReadHeader1(1);
      code500EphemFile->ReadDataRecords(-999, 2);
      code500EphemFile->CloseForRead();
   }
   #endif
   
   // Close Code500 ephemeris file
   code500EphemFile->CloseForWrite();
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemWriterCode500::FinalizeCode500Ephemeris() leaving\n");
   #endif
}

