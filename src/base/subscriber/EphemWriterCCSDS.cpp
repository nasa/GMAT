//$Id$
//------------------------------------------------------------------------------
//                                  EphemWriterCCSDS
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
// Created: 2009/09/02
//
/**
 * Writes a spacecraft orbit states or attitude to an ephemeris file in
 * CCSDS, SPK, or Code-500 format.
 */
//------------------------------------------------------------------------------

#include "EphemWriterCCSDS.hpp"
#include "SubscriberException.hpp"   // for exception
#include "StringUtil.hpp"            // for ToString()
#include "FileUtil.hpp"              // for ParseFileExtension()
#include "MessageInterface.hpp"
#include <sstream>                   // for <<, std::endl


//#define DEBUG_EPHEMFILE_INSTANCE
//#define DEBUG_EPHEMFILE_INIT
//#define DEBUG_EPHEMFILE_OPEN
//#define DEBUG_EPHEMFILE_DATA
//#define DEBUG_EPHEMFILE_CCSDS
//#define DEBUG_EPHEMFILE_BUFFER
//#define DEBUG_EPHEMFILE_WRITE
//#define DEBUG_EPHEMFILE_FINISH
//#define DEBUG_EPHEMFILE_RESTART
//#define DEBUG_EPHEMFILE_COMMENTS
//#define DEBUG_EPHEMFILE_HEADER
//#define DEBUG_EPHEMFILE_METADATA
//#define DEBUG_TO_TEXT_EPHEM
//#define DEBUG_CCSDS_DATA_SEGMENT

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
// EphemWriterCCSDS(const std::string &name, const std::string &type = "EphemWriterCCSDS")
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EphemWriterCCSDS::EphemWriterCCSDS(const std::string &name, const std::string &type) :
   EphemWriterWithInterpolator(name, type),
   ccsdsOemWriter       (NULL),
   ccsdsEpochFormat     ("UTC"),
   metaDataStart        (-999.999),
   metaDataStop         (-999.999),
   metaDataStartStr     (""),
   metaDataStopStr      (""),
   metaDataWriteOption  (0),
   metaDataBegPosition  (0),
   metaDataEndPosition  (0),
   continuousSegment    (false),
   firstTimeMetaData    (true),
   saveMetaDataStart    (true)
{
   fileType = CCSDS_OEM;
   if (type == "CCSDS-AEM")
      fileType = CCSDS_AEM;
   
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS() default constructor <%p> enterd, name='%s', type='%s', "
       "writingNewSegment=%d\n", this, name.c_str(), type.c_str(), writingNewSegment);
   #endif
}


//------------------------------------------------------------------------------
// ~EphemWriterCCSDS()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EphemWriterCCSDS::~EphemWriterCCSDS()
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::~EphemWriterCCSDS() <%p>'%s' entered\n", this, ephemName.c_str());
   #endif

   if (ccsdsOemWriter)
      delete ccsdsOemWriter;
   
   // Close CCSDS ephemeris
   dstream.flush();
   dstream.close();
   
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::~EphemWriterCCSDS() <%p>'%s' leaving\n", this, ephemName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// EphemWriterCCSDS(const EphemWriterCCSDS &ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EphemWriterCCSDS::EphemWriterCCSDS(const EphemWriterCCSDS &ef) :
   EphemWriterWithInterpolator(ef),
   ccsdsOemWriter       (NULL),
   ccsdsEpochFormat     (ef.ccsdsEpochFormat),
   metaDataStart        (ef.metaDataStart),
   metaDataStop         (ef.metaDataStop),
   metaDataStartStr     (ef.metaDataStartStr),
   metaDataStopStr      (ef.metaDataStopStr),
   metaDataWriteOption  (ef.metaDataWriteOption),
   metaDataBegPosition  (ef.metaDataBegPosition),
   metaDataEndPosition  (ef.metaDataEndPosition),
   continuousSegment    (ef.continuousSegment),
   firstTimeMetaData    (ef.firstTimeMetaData),
   saveMetaDataStart    (ef.saveMetaDataStart)
{
}


//------------------------------------------------------------------------------
// EphemWriterCCSDS& EphemWriterCCSDS::operator=(const EphemWriterCCSDS& ef)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
EphemWriterCCSDS& EphemWriterCCSDS::operator=(const EphemWriterCCSDS& ef)
{
   if (this == &ef)
      return *this;
   
   EphemWriterWithInterpolator::operator=(ef);
   ccsdsEpochFormat     = ef.ccsdsEpochFormat;
   metaDataStart        = ef.metaDataStart;
   metaDataStop         = ef.metaDataStop;
   metaDataStartStr     = ef.metaDataStartStr;
   metaDataStopStr      = ef.metaDataStopStr;
   metaDataWriteOption  = ef.metaDataWriteOption;
   metaDataBegPosition  = ef.metaDataBegPosition;
   metaDataEndPosition  = ef.metaDataEndPosition;
   continuousSegment    = ef.continuousSegment;
   firstTimeMetaData    = ef.firstTimeMetaData;
   saveMetaDataStart    = ef.saveMetaDataStart;
   
   if (ccsdsOemWriter)
      delete ccsdsOemWriter;
   ccsdsOemWriter = NULL;
   
   return *this;
}

//---------------------------------
// methods for this class
//---------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool EphemWriterCCSDS::Initialize()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::Initialize() <%p>'%s' entered, spacecraftName='%s', "
       "isInitialized=%d\n   ephemType='%s', stateType='%s', "
       "outputFormat='%s'\n", this, ephemName.c_str(), spacecraftName.c_str(),
       isInitialized, ephemType.c_str(), stateType.c_str(),
       outputFormat.c_str());
   #endif
   
   EphemWriterWithInterpolator::Initialize();
   
   // Set maximum segment size
   maxSegmentSize = 1000;
   
   // Initialize CCSDS data
   metaDataWriteOption = 0;
   metaDataBegPosition = 0;
   metaDataEndPosition = 0;
   firstTimeMetaData = true;
   saveMetaDataStart = true;
   InitializeData(false);
   maneuversHandled.clear();
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("   fileType=%d, spacecraft=<%p>'%s', outCoordSystem=<%p>'%s'\n", fileType,
       spacecraft, spacecraft->GetName().c_str(), outCoordSystem,
       outCoordSystem->GetName().c_str());
   MessageInterface::ShowMessage
      ("   useFixedStepSize=%d, interpolateInitialState=%d, interpolateFinalState=%d\n",
       useFixedStepSize, interpolateInitialState, interpolateFinalState);
   #endif
   
   // Create CCSDS-OEM writer
   if (ccsdsOemWriter == NULL)
   {
      ccsdsOemWriter = new CCSDSOEMWriter;

      // Specify options to write
      ccsdsOemWriter->SetWritingAccelerationOption(writeAccelerationOption);
      ccsdsOemWriter->SetWritingCovarianceOption(writeCovarianceOption);
   }

   // Check if interpolator needs to be created
   if (useFixedStepSize || interpolateInitialState || interpolateFinalState)
      createInterpolator = true;
   else
      createInterpolator = false;
   
   // Create interpolator if needed
   if (createInterpolator)
      CreateInterpolator();
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::Initialize() <%p>'%s' returning true, writeOrbit=%d, writeAttitude=%d, "
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
 * This method returns a clone of the EphemWriterCCSDS.
 *
 * @return clone of the EphemWriterCCSDS.
 *
 */
//------------------------------------------------------------------------------
EphemerisWriter* EphemWriterCCSDS::Clone(void) const
{
   return (new EphemWriterCCSDS(*this));
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
void EphemWriterCCSDS::Copy(const EphemerisWriter* orig)
{
   operator=(*((EphemWriterCCSDS *)(orig)));
}


//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
// void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21],
//                      const Real accel[3])
//------------------------------------------------------------------------------
void EphemWriterCCSDS::BufferOrbitData(Real epochInDays, const Real state[6],
                                       const Real cov[21], const Real accel[3],
                                       const Real quat[4])
{
   #ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::BufferOrbitData() entered, epochInDays=%.15f, "
       "state[0]=%.15f, a1MjdArray.size()=%d, maxSegmentSize=%d\n", epochInDays,
       state[0], a1MjdArray.size(), maxSegmentSize);
   DebugWriteTime("   epochInDays = ", epochInDays, true, 2);
   #endif
   
   // if buffer is full, dump the data
   if (a1MjdArray.size() >= maxSegmentSize)
   {
      if (fileType == CCSDS_OEM)
      {
         metaDataWriteOption = 0;
         saveMetaDataStart = false;
         continuousSegment = true;
         if (firstTimeMetaData)
            saveMetaDataStart = true;
         #ifdef DEBUG_EPHEMFILE_WRITE
         DebugWriteTime("********** WRITING CONTINUOUS SEGMENT AT epochInDays = ", epochInDays, true, 2);
         #endif
         WriteCcsdsOrbitDataSegment();
         //firstTimeMetaData = false;
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** INTERNAL ERROR *** Currently only CCSDS-OEM data is available to write\n");
         return;
      }
   }
   
   // Add new data point
   A1Mjd *a1mjd = new A1Mjd(epochInDays);
   a1MjdArray.push_back(a1mjd);
   Rvector6 *rv6 = new Rvector6(state);
   stateArray.push_back(rv6);

   Rvector *covar = new Rvector(21);
   for (Integer i = 0; i < 21; ++i)
      (*covar)[i] = cov[i];
   covArray.push_back(covar);
   Rvector3 *acc = new Rvector3(accel[0], accel[1], accel[2]);
   accelArray.push_back(acc);
   Rvector *rvacov = new Rvector(30);
   for (Integer i = 0; i < 6; ++i)
      (*rvacov)[i] = state[i];
   for (Integer i = 0; i < 3; ++i)
      (*rvacov)[i+6] = accel[i];
   for (Integer i = 0; i < 21; ++i)
      (*rvacov)[i+9] = cov[i];
   rvacovArray.push_back(rvacov);

   #ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::BufferOrbitData() leaving, there is(are) %d data point(s)\n",
       a1MjdArray.size());
   #endif
} // BufferOrbitData()


//------------------------------------------------------------------------------
// void InitializeData(bool saveEpochInfo = true)
//------------------------------------------------------------------------------
void EphemWriterCCSDS::InitializeData(bool saveEpochInfo)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterCCSDS::InitializeData() <%p>'%s' entered, saveEpochInfo=%d\n",
       this, ephemName.c_str(), saveEpochInfo);
   #endif
   
   EphemWriterWithInterpolator::InitializeData(saveEpochInfo);
   
   continuousSegment    = false;
   firstTimeMetaData    = true;
   saveMetaDataStart    = true;
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterCCSDS::InitializeData() <%p>'%s' leaving, currEpochInSecs=%.15f, "
       "prevEpochInSecs=%.15f\n", this, ephemName.c_str(), currEpochInSecs, prevEpochInSecs);
   #endif
}


//------------------------------------------------------------------------------
// void CreateEphemerisFile(bool useDefaultFileName, const std::string &stType,
//                          const std::string &outFormat, const std::string &covFormat)
//------------------------------------------------------------------------------
void EphemWriterCCSDS::CreateEphemerisFile(bool useDefaultFileName,
                                           const std::string &stType,
                                           const std::string &outFormat,
                                           const std::string &covFormat)
{
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::CreateEphemerisFile() <%p> entered, useDefaultFileName=%d, "
       "outFormat='%s'\n   fullPathFileName='%s'\n", this, useDefaultFileName,
       outFormat.c_str(), fullPathFileName.c_str());
   #endif
   
   EphemerisWriter::CreateEphemerisFile(useDefaultFileName, stType, outFormat, covFormat);
   
   // If default file name is used, write informational message about the file location (LOJ: 2014.06.24)
   if (useDefaultFileName)
      MessageInterface::ShowMessage
         ("*** The output file '%s' will be written as '%s'\n",
          fileName.c_str(), fullPathFileName.c_str());
   
   if (!OpenCcsdsEphemerisFile())
   {
      SubscriberException se;
      se.SetDetails("**** ERROR **** Unable to create CCSDS EphemerisFile '%s'\n",
                    fullPathFileName.c_str());
      throw se;
   }
   
   isEphemFileOpened = true;
   
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::CreateEphemerisFile() <%p> leaving, writingNewSegment=%d\n   "
       "fullPathFileName='%s'\n", this, writingNewSegment, fullPathFileName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// bool OpenCcsdsEphemerisFile()
//------------------------------------------------------------------------------
bool EphemWriterCCSDS::OpenCcsdsEphemerisFile()
{
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::OpenCcsdsEphemerisFile() <%p> entered, fileName = %s\n   "
       "ccsdsOemWriter=<%p>\n", this, fullPathFileName.c_str(), ccsdsOemWriter);
   #endif
   
   bool retval = false;
   
   if (ccsdsOemWriter == NULL)
   {
      ccsdsOemWriter = new CCSDSOEMWriter;

      // Specify options to write
      ccsdsOemWriter->SetWritingAccelerationOption(writeAccelerationOption);
      ccsdsOemWriter->SetWritingCovarianceOption(writeCovarianceOption);
   }
   
   retval = ccsdsOemWriter->OpenFile(fullPathFileName);
   
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("   ccsdsOemWriter->OpenFile() returned %d\n", retval);
   #endif
   
   #ifdef DEBUG_TO_TEXT_EPHEM
   // Open text ephemeris file
   retval = OpenTextEphemerisFile(fullPathFileName + ".debug.txt");
   #endif
   
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::OpenCcsdsEphemerisFile() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void CloseEphemerisFile(bool done = true, writeMetaData = true)
//------------------------------------------------------------------------------
void EphemWriterCCSDS::CloseEphemerisFile(bool done, bool writeMetaData)
{
   dstream.flush();
   dstream.close();
}


//------------------------------------------------------------------------------
// bool IsBackwardPropAllowed(Real propDirection)
//------------------------------------------------------------------------------
/**
 * Checks if backward prop is allowed or don't need special handling.
 *
 * @return  false  if no special handling is needed
 *          true   if need to procced to next step
 */
//------------------------------------------------------------------------------
bool EphemWriterCCSDS::IsBackwardPropAllowed(Real propDirection)
{
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::IsBackwardPropAllowed() entered\n");
   #endif
   
   //====================================================
   // Write one time warning and continue for other types
   static bool firstTimeWarning = true;
   
   if (propIndicator >= 3 ||
       (propDirection == -1.0 && (ephemType != "CCSDS-OEM" && ephemType != "CCSDS-AEM")))
   {        
      if (firstTimeWarning)
      {
         MessageInterface::ShowMessage(GetBackwardPropWarning().c_str());
         firstTimeWarning = false;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::IsBackwardPropAllowed() returning true\n");
   #endif
   return true;
} // IsBackwardPropAllowed()


//------------------------------------------------------------------------------
// void HandleOrbitData()
//------------------------------------------------------------------------------
void EphemWriterCCSDS::HandleOrbitData()
{
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::HandleOrbitData() <%p>'%s' entered, currEpochInDays=%.15f, "
       "writingNewSegment=%d\n", this, ephemName.c_str(), currEpochInDays, writingNewSegment);
   MessageInterface::ShowMessage("Checking user specified initial and final epoch...\n");
   #endif
   
   // Check user defined initial and final epoch
   bool processData = CheckInitialAndFinalEpoch();
   
   // Buffer a few more (interpolationOrder + 1) data after finalEpochReached,
   // if final epoch was not already written
   if (finalEpochReached && !finalEpochProcessed)
   {
      processData = true;
      afterFinalEpochCount++;
      if (afterFinalEpochCount > (interpolationOrder + 1))
         processData = false;
   }
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::HandleOrbitData() checked initial and final epoch, processData=%d\n",
       processData);
   #endif
   
   // Check if it is time to write
   bool timeToWrite = IsTimeToWrite(currEpochInSecs, currState, currCov, currAccel);
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("   Start writing data, currEpochInDays=%.15f, currEpochInSecs=%.15f, %s\n"
       "   writeOrbit=%d, writeAttitude=%d, processData=%d, timeToWrite=%d\n",
       currEpochInDays, currEpochInSecs, ToUtcGregorian(currEpochInSecs).c_str(),
       writeOrbit, writeAttitude, processData, timeToWrite);
   #endif
   
   // Write orbit or attitude data
   HandleCcsdsOrbitData(processData, timeToWrite);
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::HandleOrbitData() <%p>'%s' leaving, writingNewSegment=%d\n",
       this, ephemName.c_str(), writingNewSegment);
   #endif
}


//------------------------------------------------------------------------------
// void StartNewSegment(const std::string &comments, bool saveEpochInfo,
//                      bool writeAfterData, bool ignoreBlankComments)
//------------------------------------------------------------------------------
/**
 * Finishes writing remaining data and resets flags to start new segment.
 */
//------------------------------------------------------------------------------
void EphemWriterCCSDS::StartNewSegment(const std::string &comments, bool saveEpochInfo,
                                       bool writeAfterData, bool ignoreBlankComments)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterCCSDS::StartNewSegment() entered\n   comments='%s'\n   "
       "saveEpochInfo=%d, writeAfterData=%d, ignoreBlankComments=%d, canFinalize=%d, "
       "firstTimeWriting=%d\n", comments.c_str(), saveEpochInfo, writeAfterData,
       ignoreBlankComments, canFinalize, firstTimeWriting);
   #endif
   
   // If no first data has written out yet, just return
   if (firstTimeWriting)
   {
      #ifdef DEBUG_EPHEMFILE_RESTART
      MessageInterface::ShowMessage
         ("EphemWriterCCSDS::StartNewSegment() returning, no first data written out yet\n");
      #endif
      return;
   }
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::StartNewSegment() Calling FinishUpWriting(), canFinalize=%d\n",
       canFinalize);
   #endif
   
   // Write data for the rest of times on waiting
   FinishUpWriting();
   
   // Add non-empty comments to the meta data object, so that they
   // are included in meta data upon call to 
   // CCSDSOEMSegment::GetMetaDataForWriting()
   if (comments != "")
   {
       ccsdsOemWriter->AddMetaComment(comments);
   }
   
   ++numSegments;

   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterCCSDS::StartNewSegment() leaving\n");
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
void EphemWriterCCSDS::FinishUpWriting()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::FinishUpWriting() <%p>'%s' entered, canFinalize=%d, isFinalized=%d, "
       "firstTimeWriting=%d\n   interpolatorStatus=%d, continuousSegment=%d, isEndOfRun=%d\n",
       this, ephemName.c_str(), canFinalize, isFinalized, firstTimeWriting, interpolatorStatus,
       continuousSegment, isEndOfRun);
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
      
      if (writeOrbit)
         FinishUpWritingOrbitData();
      else if (writeAttitude)
         FinishUpWritingAttitudeData();
      else
      {
         // Throw an exception         
         #ifdef DEBUG_EPHEMFILE_FINISH
         MessageInterface::ShowMessage
            ("EphemWriterCCSDS::FinishUpWriting() throwing exception, "
             "Not writing orbit nor attitude\n");
         #endif
         throw SubscriberException
            ("*** INTERNAL ERROR *** EphemWriterCCSDS::FinishUpWriting() "
             "Not writing orbit nor attitude");
      }
      
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
      ("EphemWriterCCSDS::FinishUpWriting() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void HandleCcsdsOrbitData(bool writeData, bool &timeToWrite)
//------------------------------------------------------------------------------
void EphemWriterCCSDS::HandleCcsdsOrbitData(bool writeData, bool &timeToWrite)
{
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::HandleCcsdsOrbitData() <%p> entered, writeData=%d, "
       "timeToWrite=%d, writingNewSegment=%d, writeOrbit=%d\n", this, writeData,
       timeToWrite, writingNewSegment, writeOrbit);
   #endif
   
   // LagrangeInterpolator's maximum buffer size is set to 80 which can hold
   // 80 min of data assuming average of 60 sec data interveval.
   // Check at least 10 min interval for large step size, since interpolater
   // buffer size is limited
   if (!timeToWrite)
   {
      if ((currEpochInSecs - prevProcTime) > 600.0)
      {
         #ifdef DEBUG_EPHEMFILE_CCSDS
         MessageInterface::ShowMessage
            ("   ==> 10 min interval is over, so setting timeToWrite to true\n");
         #endif
         
         timeToWrite = true;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage("   After checking, timeToWrite=%d\n", timeToWrite);
   #endif
   
   if (timeToWrite)
      prevProcTime = currEpochInSecs;
   
   //------------------------------------------------------------
   // write data to file
   //------------------------------------------------------------
   // Now actually write data
   if (writeData && timeToWrite)
   {
      #ifdef DEBUG_EPHEMFILE_CCSDS
      MessageInterface::ShowMessage("   Write header\n");
      #endif
      if (firstTimeWriting)
         WriteHeader();
      
      #ifdef DEBUG_EPHEMFILE_CCSDS
      MessageInterface::ShowMessage("   Write new segment\n");
      #endif
      if (writingNewSegment)
      {
         #ifdef DEBUG_EPHEMFILE_WRITE
         //WriteDataComments("********** DEBUG OUTPUT - NEW SEGMENT **********", false);
         DebugWriteTime
            ("********** WRITING NEW SEGMENT AT currEpochInSecs = ", currEpochInSecs, false, 2);
         #endif
         
         metaDataWriteOption = 1;
         saveMetaDataStart = true;
         WriteCcsdsOrbitDataSegment();
      }
      
      #ifdef DEBUG_EPHEMFILE_CCSDS
      MessageInterface::ShowMessage("   Write DATA_START\n");
      #endif
      if (fileType == CCSDS_AEM && (firstTimeWriting || writingNewSegment))
         WriteStringToFile("DATA_START\n");
      
      #ifdef DEBUG_EPHEMFILE_CCSDS
      MessageInterface::ShowMessage("   Handle write orbit data: writeOrbit = '%s'\n", (writeOrbit?"true":"false"));
      #endif

      if (writeOrbit)
         HandleWriteOrbit();
      else if (writeAttitude)
         HandleWriteAttitude();
      
      if (firstTimeWriting)
      {
         firstTimeWriting = false;
         #ifdef DEBUG_EPHEMFILE_CCSDS
         MessageInterface::ShowMessage("===> firstTimeWriting is set to false\n");
         #endif
      }
      
      if (writingNewSegment)
      {
         writingNewSegment = false;
         #ifdef DEBUG_EPHEMFILE_CCSDS
         MessageInterface::ShowMessage("===> writingNewSegment is set to false\n");
         #endif
      }
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_CCSDS
      MessageInterface::ShowMessage
         ("   It %s time to write but initial epoch has %s\n", timeToWrite ? "is" : "is not",
          writeData ? "reached" : "not reached");
      #endif
   }
   
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::HandleCcsdsOrbitData() <%p> leaving, firstTimeWriting=%d, "
       "writingNewSegment=%d\n", this, firstTimeWriting, writingNewSegment);
   #endif
}


//------------------------------------------------------------------------------
// void FinishUpWritingOrbitData()
//------------------------------------------------------------------------------
void EphemWriterCCSDS::FinishUpWritingOrbitData()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::FinishUpWritingOrbitData() entered, canFinalize=%d, "
       "interpolatorStatus=%d\n", canFinalize, interpolatorStatus);
   #endif
   
   if (interpolator != NULL && useFixedStepSize)
   {
      // Check for not enough data points for interpolation
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage
         ("===> FinishUpWritingOrbitData() checking for not enough data points\n"
          "   metaDataStartStr='%s', currEpochInSecs='%s', canFinalize=%d, interpolatorStatus=%d\n",
          metaDataStartStr.c_str(), ToUtcGregorian(currEpochInSecs).c_str(), canFinalize,
          interpolatorStatus);
      #endif
      
      if (canFinalize && interpolatorStatus == -1)
      {
         #ifdef DEBUG_EPHEMFILE_FINISH
         MessageInterface::ShowMessage
            ("initialEpochA1Mjd=%f, currEpochInDays=%f\n", initialEpochA1Mjd, currEpochInDays);
         MessageInterface::ShowMessage
            ("===> FinishUpWritingOrbitData() checking if user defined initial epoch has not reached yet\n");
         #endif
         
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

         // Check for user defined initial epoch has reached yet
         if (initialEpochA1Mjd != -999.999 && (currEpochInDays < initialEpochA1Mjd))
         {
            if (a1MjdArray.empty() && !isEndOfRun)
            {
               #ifdef DEBUG_EPHEMFILE_FINISH
               MessageInterface::ShowMessage
                  ("EphemWriterCCSDS::FinishUpWritingOrbitData() leaving, no further processing is needed "
                   "current epoch < initial user epoch\n");
               #endif
               return;
            }
         }
         
         if (interpolatorStatus == -1)
         {
         isFinalized = true;
         std::string ccsdsMsg, errMsg;
         
         FormatErrorMessage(ccsdsMsg, errMsg);
         
         // Clear last MetaData with COMMENT
         ClearLastCcsdsOemMetaData(ccsdsMsg);
         
         // Throw an exception         
         #ifdef DEBUG_EPHEMFILE_FINISH
         MessageInterface::ShowMessage
            ("EphemWriterCCSDS::FinishUpWritingOrbitData() throwing exception, a1MjdArray.size()=%d\n"
             "   errMsg = %s\n", a1MjdArray.size(), errMsg.c_str());
         #endif
         
         throw SubscriberException(errMsg);
      }
      }
      
      // Finish up final data waiting to be output
      ProcessFinalDataOnWaiting();
   }
   
   writeCommentAfterData = false;
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   //WriteDataComments("********** DEBUG OUTPUT - FINAL SEGMENT **********", false);
   DebugWriteTime
      ("********** WRITING FINAL SEGMENT AT currEpochInSecs = ", currEpochInSecs, false, 2);
   #endif
   if (canFinalize)
   {
      metaDataWriteOption = 2;
      saveMetaDataStart = true;
      if (continuousSegment)
         saveMetaDataStart = false;
   }
   else
   {
      metaDataWriteOption = 0;
      if (firstTimeMetaData)
         metaDataWriteOption = 2; // Overwrite previous meta data
      saveMetaDataStart = true;
      if (continuousSegment)
         saveMetaDataStart = false;
   }
   
   WriteCcsdsOrbitDataSegment();
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemWriterCCSDS::FinishUpWritingOrbitData() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void FinishUpWritingAttitudeData()
//------------------------------------------------------------------------------
void EphemWriterCCSDS::FinishUpWritingAttitudeData()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::FinishUpWritingAttitudeData() entered, canFinalize=%d, "
       "interpolatorStatus=%d\n", canFinalize, interpolatorStatus);
   #endif
   
   // Need implementation here
   
   if (fileType == CCSDS_AEM)
      WriteStringToFile("DATA_STOP\n");
}

//------------------------------------------------------------------------------
// void WriteHeader()
//------------------------------------------------------------------------------
void EphemWriterCCSDS::WriteHeader()
{
   WriteCcsdsHeader();
}


//------------------------------------------------------------------------------
// void WriteMetaData()
//------------------------------------------------------------------------------
void EphemWriterCCSDS::WriteMetaData()
{
   WriteCcsdsOemMetaData();
}


//------------------------------------------------------------------------------
// void WriteDataComments(const std::string &comments, bool writeCmtsNow,
//                    bool ignoreBlankComments = true, bool writeKeyword = true)
//------------------------------------------------------------------------------
/**
 * Writes comments to specific file.
 */
//------------------------------------------------------------------------------
void EphemWriterCCSDS::WriteDataComments(const std::string &comments, bool writeCmtsNow,
                                     bool ignoreBlanks, bool writeKeyword)
{
   #ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage
      ("WriteDataComments() entered, comments='%s', writeCmtsNow=%d, ignoreBlanks=%d, "
       "writeKeyword=%d\n",  comments.c_str(), writeCmtsNow, ignoreBlanks,
       writeKeyword);
   #endif
   
   if (comments == "" && ignoreBlanks)
   {
      #ifdef DEBUG_EPHEMFILE_COMMENTS
      MessageInterface::ShowMessage("WriteDataComments() just leaving\n");
      #endif
      return;
   }
   
   WriteCcsdsDataComments(comments, writeCmtsNow, ignoreBlanks, writeKeyword);
   
   #ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage("WriteDataComments() wrote comment and leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void ClearLastCcsdsOemMetaData(const std::string &comments)
//------------------------------------------------------------------------------
void EphemWriterCCSDS::ClearLastCcsdsOemMetaData(const std::string &comments)
{
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::ClearLastCcsdsOemMetaData() entered\n   comments='%s', "
       "firstTimeMetaData=%d\n", comments.c_str(), firstTimeMetaData);
   #endif
   
   
   #ifdef DEBUG_TO_TEXT_EPHEM
   // Go to beginning of the last meta data position
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("===> metaDataBegPosition=%ld, metaDataEndPosition=%ld\n",
       (long)metaDataBegPosition, (long)metaDataEndPosition);
   #endif
   dstream.seekp(metaDataBegPosition, std::ios_base::beg);
   #endif
   
   
   if (comments != "")
      WriteDataComments(comments, true, true, true);
   
   
   #ifdef DEBUG_TO_TEXT_EPHEM
   // Clear with blanks
   std::stringstream ss("");
   ss << std::endl;
   long length = (long)metaDataEndPosition - (long)metaDataBegPosition;
   for (long i = 0; i < length; i++)
      ss << " ";
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("   ==> Writing following empty string, length=%d:\n%s\n", length, ss.str().c_str());
   #endif
   
   WriteStringToFile(ss.str());
   #endif
   
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::ClearLastCcsdsOemMetaData() leaving, firstTimeMetaData=%d\n",
       firstTimeMetaData);
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsHeader()
//------------------------------------------------------------------------------
void EphemWriterCCSDS::WriteCcsdsHeader()
{
   #ifdef DEBUG_EPHEMFILE_HEADER
   MessageInterface::ShowMessage("EphemWriterCCSDS::WriteCcsdsHeader() entered\n");
   #endif
   
   // Set header information and then write
   if (ccsdsOemWriter)
   {
      //ccsdsOemWriter->SetHeaderForWriting("VERSION_NUMBER", "1.0");
      ccsdsOemWriter->SetHeaderForWriting("VERSION_NUMBER", ephemVersion);
      ccsdsOemWriter->SetHeaderForWriting("ORIGINATOR", "GMAT USER");
      ccsdsOemWriter->WriteHeader("CCSDS_OEM_VERS");
   }
   
   
   #ifdef DEBUG_TO_TEXT_EPHEM
   std::string creationTime = GmatTimeUtil::FormatCurrentTime(2);
   std::string originator = "GMAT USER";
   
   std::stringstream ss("");
   
   if (fileType == CCSDS_OEM)
      ss << "CCSDS_OEM_VERS = 1.0" << std::endl;
   else
      ss << "CCSDS_AEM_VERS = 1.0" << std::endl;
   
   ss << "CREATION_DATE  = " << creationTime << std::endl;
   ss << "ORIGINATOR     = " << originator << std::endl;
   
   WriteStringToFile(ss.str());
   #endif
   
   #ifdef DEBUG_EPHEMFILE_HEADER
   MessageInterface::ShowMessage("EphemWriterCCSDS::WriteCcsdsHeader() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsOrbitDataSegment()
//------------------------------------------------------------------------------
/**
 * Writes CCSDS orbit data segment
 */
//------------------------------------------------------------------------------
void EphemWriterCCSDS::WriteCcsdsOrbitDataSegment()
{
   #ifdef DEBUG_CCSDS_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("=====> EphemWriterCCSDS::WriteCcsdsOrbitDataSegment() <%p>'%s' entered\n   "
       "metaDataWriteOption=%d, saveMetaDataStart=%d, firstTimeMetaData=%d, "
       "continuousSegment=%d, a1MjdArray.size()=%d\n", this, ephemName.c_str(),
       metaDataWriteOption, saveMetaDataStart, firstTimeMetaData, continuousSegment,
       a1MjdArray.size());
   #endif
   if (a1MjdArray.empty())
   {
      if (metaDataWriteOption == 1)
      {
         #ifdef DEBUG_CCSDS_DATA_SEGMENT
         MessageInterface::ShowMessage("***** array is empty so writing dummy meta data time\n");
         #endif
         metaDataStartStr = "YYYY-MM-DDTHH:MM:SS.SSS";
         metaDataStopStr = "YYYY-MM-DDTHH:MM:SS.SSS";
         WriteCcsdsOemMetaData();
      }
      
      #ifdef DEBUG_CCSDS_DATA_SEGMENT
      MessageInterface::ShowMessage
         ("=====> EphemWriterCCSDS::WriteCcsdsOrbitDataSegment() leaving, array is empty\n");
      #endif
      return;
   }
   
   if (saveMetaDataStart)
   {
      // Do not overwrite meta data start time (LOJ: 2013.03.21 GMT-3716 Fix)
      if (metaDataStartStr == "YYYY-MM-DDTHH:MM:SS.SSS")
      {
         metaDataStart = (a1MjdArray.front())->GetReal();
         metaDataStartStr = ToUtcGregorian(metaDataStart, true, 2);
      }
   }
   metaDataStop  = (a1MjdArray.back())->GetReal();
   metaDataStopStr = ToUtcGregorian(metaDataStop, true, 2);
   
   if (metaDataWriteOption == 1)
   {
      WriteCcsdsOemMetaData();
      firstTimeMetaData = false;
   }
   
   // Write OEM line to a file
   WriteCcsdsOemData();
   
   if (metaDataWriteOption == 2)
   {
      dstream.seekp(metaDataBegPosition, std::ios_base::beg);
      
      WriteCcsdsOemMetaData();
      firstTimeMetaData = false;
      dstream.seekp(0, std::ios_base::end);
   }
   
   #ifdef DEBUG_CCSDS_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("=====> Writing %d CCSDS orbit data points\n", a1MjdArray.size());
   DebugWriteTime("   First data, metaDataStart = ", metaDataStart, true);
   DebugWriteTime("     Last data, metaDataStop = ", metaDataStop, true);
   #endif
   
   // Clear orbit buffer
   ClearOrbitData();
   
   #ifdef DEBUG_CCSDS_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("=====> EphemWriterCCSDS::WriteCcsdsOrbitDataSegment() <%p>'%s' leaving\n",
       this, ephemName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsOemMetaData()
//------------------------------------------------------------------------------
void EphemWriterCCSDS::WriteCcsdsOemMetaData()
{
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::WriteCcsdsOemMetaData() entered, firstTimeMetaData=%d\n",
       firstTimeMetaData);
   #endif
   
   // Save meta data begin position
   metaDataBegPosition = dstream.tellp();
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("   ==> Setting metaDataBegPosition=%ld\n", (long)metaDataBegPosition);
   MessageInterface::ShowMessage
      ("   metaDataStartStr='%s', metaDataStopStr='%s'\n", metaDataStartStr.c_str(),
       metaDataStopStr.c_str());
   MessageInterface::ShowMessage
      ("   spacecraft=<%p>'%s', outCoordSystem=<%p>'%s'\n", spacecraft,
       spacecraft ? spacecraft->GetName().c_str() : "NULL", outCoordSystem,
       outCoordSystem ? outCoordSystem->GetName().c_str() : "NULL");
   #endif
   
   std::string origin = "UNKNOWN";
   std::string csType = "UNKNOWN";
   
   if (outCoordSystem)
   {
      csType = outCoordSystem->GetStringParameter("Axes");

      origin = outCoordSystem->GetStringParameter("Origin");

      if (csType == "MJ2000Eq")
         csType = "EME2000";
      else if (csType == "ObjectReferenced")
         csType = "ObjectReferenced";
      else if (csType == "TODEq" && (origin == "Earth" || GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING))        // made changes by TUAN NGUYEN
         csType = "TOD";
      else if (csType == "ICRF" && (origin == "Earth" || GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING))         // made changes by TUAN NGUYEN
         csType = "ICRF";
      else if (csType == "BodyFixed" && (origin == "Earth" || GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING))    // made changes by TUAN NGUYEN
         csType = "TDR";
      else if (csType == "MJ2000Ec" && (origin == "Earth" || GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING))     // made changes by TUAN NGUYEN
         csType = "MJ2000Ec";
      else
      {
         std::string msg = "Axes of type " + csType + " are not permitted to be written to a CCSDS Ephemeris file using central body " + origin + ".\n";
         // if(GmatGlobal::Instance()->GetRunMode() != GmatGlobal::TESTING)         // made changes by TUAN NGUYEN
         if (GmatGlobal::Instance()->GetRunModeStartUp() != GmatGlobal::TESTING)    // made changes by TUAN NGUYEN
            throw SubscriberException(msg);
      }

      if (origin == "Luna")
         origin = "Moon";

   }

   if (covarianceFormat != "None")
   {
      if (csType != "EME2000")
      {
         std::string msg = "Writing covariance to a CCSDS ephemeris file with a coordinate system other than J2000 is not allowed.\n";
         // if(GmatGlobal::Instance()->GetRunMode() != GmatGlobal::TESTING)         // made changes by TUAN NGUYEN
         if (GmatGlobal::Instance()->GetRunModeStartUp() != GmatGlobal::TESTING)    // made changes by TUAN NGUYEN
            throw SubscriberException(msg);
      }
   }
   
   Integer actualInterpOrder = interpolationOrder;
   Integer numData = a1MjdArray.size();
   Integer numPoints = 0;
   if (interpolator != NULL)
      numPoints = interpolator->GetPointCount();
   else
      numPoints = numData;
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("   actualInterpOrder=%d, numPoints=%d\n", actualInterpOrder, numPoints);
   #endif
   
   if (metaDataWriteOption == 2)
   {
      if (numPoints <= interpolationOrder)
      {
         actualInterpOrder = numPoints - 1;
         if (numPoints < 2)
            actualInterpOrder = 1;
      }
   }
   
   // Format interpolation order, width of 2 with left justified
   char interpOrdBuff[5];
   sprintf(interpOrdBuff, "%-2d", actualInterpOrder);

   
   #if (defined DEBUG_TO_TEXT_EPHEM) || (defined DEBUG_EPHEMFILE_METADATA)
   std::stringstream ss("");
   ss << std::endl;
   ss << "META_START" << std::endl;
   ss << "OBJECT_NAME          = " << spacecraftName << std::endl;
   ss << "OBJECT_ID            = " << spacecraftId << std::endl;
   ss << "CENTER_NAME          = " << origin << std::endl;
   ss << "REF_FRAME            = " << csType << std::endl;
   ss << "TIME_SYSTEM          = " << ccsdsEpochFormat << std::endl;
   ss << "START_TIME           = " << metaDataStartStr << std::endl;
   ss << "USEABLE_START_TIME   = " << metaDataStartStr << std::endl;
   ss << "USEABLE_STOP_TIME    = " << metaDataStopStr << std::endl;
   ss << "STOP_TIME            = " << metaDataStopStr << std::endl;
   ss << "INTERPOLATION        = " << interpolatorName << std::endl;
   ss << "INTERPOLATION_DEGREE = " << interpOrdBuff << std::endl;
   ss << "META_STOP" << std::endl << std::endl;
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage("   ==> Writing following META data:\n%s\n", ss.str().c_str());
   #endif
   WriteStringToFile(ss.str());
   #endif
   
      
   // Write final meta data with valid start and stop time
   if (metaDataWriteOption == 2)
   {
      if (ccsdsOemWriter)
      {
         ccsdsOemWriter->SetMetaDataForWriting("OBJECT_NAME", spacecraftName);
         ccsdsOemWriter->SetMetaDataForWriting("OBJECT_ID", spacecraftId);
         ccsdsOemWriter->SetMetaDataForWriting("CENTER_NAME", origin);
         ccsdsOemWriter->SetMetaDataForWriting("REF_FRAME", csType);
         ccsdsOemWriter->SetMetaDataForWriting("TIME_SYSTEM", ccsdsEpochFormat);
         ccsdsOemWriter->SetMetaDataForWriting("START_TIME", metaDataStartStr);
         ccsdsOemWriter->SetMetaDataForWriting("USEABLE_START_TIME", metaDataStartStr);
         ccsdsOemWriter->SetMetaDataForWriting("USEABLE_STOP_TIME", metaDataStopStr);
         ccsdsOemWriter->SetMetaDataForWriting("STOP_TIME", metaDataStopStr);
         ccsdsOemWriter->SetMetaDataForWriting("INTERPOLATION", interpolatorName);
         ccsdsOemWriter->SetMetaDataForWriting("INTERPOLATION_DEGREE", interpOrdBuff);
         ccsdsOemWriter->WriteDataComments();
         ccsdsOemWriter->WriteMetaData();
         ccsdsOemWriter->WriteDataSegment();
      }
   }
   
   // Save meta data end position
   metaDataEndPosition = dstream.tellp();
   
   if (metaDataWriteOption == 2)
   {
      if (numData < 2 && stepSize == "IntegratorSteps")
      {
         
         #ifdef DEBUG_EPHEMFILE_METADATA
         MessageInterface::ShowMessage("   ==> numData is less than 2, so writing COMMENT\n");
         #endif
         MessageInterface::ShowMessage("Warning: Ephemeris File " + ephemName + " contains a data block with less than 2 data points. When using integrator steps, there should be at least 2 data points.\n");
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage("EphemWriterCCSDS::WriteCcsdsOemMetaData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsAemMetaData()
//------------------------------------------------------------------------------
/**
 *  Writes CCSDS AEM metadata to a file
 *
 * @note This method is currently not used
 */
//------------------------------------------------------------------------------
void EphemWriterCCSDS::WriteCcsdsAemMetaData()
{
   std::string origin = outCoordSystem->GetOriginName();
   std::string csType = "UNKNOWN";
   GmatBase *cs = (GmatBase*)(spacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
   if (cs)
      csType = cs->GetTypeName();

   #ifdef DEBUG_TO_TEXT_EPHEM
   std::stringstream ss("");
   ss << "META_START" << std::endl;
   ss << "OBJECT_NAME = " << spacecraftName << std::endl;
   ss << "OBJECT_ID = " << spacecraftId << std::endl;
   ss << "CENTER_NAME = " << origin << std::endl;
   ss << "REF_FRAME_A = " << csType << std::endl;
   ss << "REF_FRAME_B = " << "@TODO_REFB" << std::endl;
   ss << "TIME_SYSTEM = " << ccsdsEpochFormat << std::endl;
   ss << "START_TIME = " << "@TODO_START" << std::endl;
   ss << "USEABLE_START_TIME = " << "@TODO_USTART" << std::endl;
   ss << "USEABLE_STOP_TIME = " << "@TODO_USTOP" << std::endl;
   ss << "STOP_TIME = " << "@TODO_STOP" << std::endl;
   ss << "ATTITUDE_TYPE = " << "@TODO_STOP" << std::endl;
   ss << "QUATERNION_TYPE = " << "@TODO_STOP" << std::endl;
   ss << "INTERPOLATION_METHOD = " << interpolatorName << std::endl;
   ss << "INTERPOLATION_DEGREE = " << interpolationOrder << std::endl;
   ss << "META_STOP" << std::endl << std::endl;
   WriteStringToFile(ss.str());
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsOemData()
//------------------------------------------------------------------------------
void EphemWriterCCSDS::WriteCcsdsOemData()
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::WriteCcsdsOemData() <%p>'%s' entered, a1MjdArray.size()=%d\n",
       this, ephemName.c_str(), a1MjdArray.size());
   #endif
   
   bool dataAdded = false;
   for (UnsignedInt i = 0; i < a1MjdArray.size(); i++)
   {
      Real epoch = a1MjdArray[i]->GetReal();
      
      #ifdef DEBUG_TO_TEXT_EPHEM
      const Real *outState = stateArray[i]->GetDataVector();
      std::string epochStr = ToUtcGregorian(epoch, true, 2);
      char strBuff[200];
      sprintf(strBuff, "%s  state = [% 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e]\n",
              epochStr.c_str(), outState[0], outState[1], outState[2], outState[3],
              outState[4], outState[5]);
      dstream << strBuff; 

      outState = accelArray[i]->GetDataVector();
      sprintf(strBuff, "%s  accel = [% 1.15e  % 1.15e  % 1.15e]\n",
         epochStr.c_str(), outState[0], outState[1], outState[2]);
      dstream << strBuff;

      dstream.flush();
      #endif
   
      #ifdef DEBUG_EPHEMFILE_WRITE
      MessageInterface::ShowMessage("rvacov array size = %d\n", (*rvacovArray[i]).GetSize());
      for (Integer k = 0; k < (*rvacovArray[i]).GetSize(); ++k)
         MessageInterface::ShowMessage("  (*rvacovArray[i])[%d] = %.15le\n", k, (*rvacovArray[i])[k]);
      #endif

      // Add data points to ccsds writer and then write
      // This code changes is used to add acceleration and covarian matrix to funtion AddDataForWriting()
      //if (ccsdsOemWriter && ccsdsOemWriter->AddDataForWriting(epoch, *stateArray[i]))
      if (ccsdsOemWriter && ccsdsOemWriter->AddDataForWriting(epoch, *rvacovArray[i]))
      {
         dataAdded = true;
      }
      else
      {
         dataAdded = false;
         break;
      }
   }
   
   if (!dataAdded)
   {
      SubscriberException se;
      se.SetDetails("**** ERROR **** Unable to add data points to CCSDS EphemerisFile '%s'\n",
                    fullPathFileName.c_str());
      throw se;
   }
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::WriteCcsdsOemData() <%p>'%s' leaving\n", this, ephemName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsAemData(Real reqEpochInSecs, const Real quat[4])
//------------------------------------------------------------------------------
void EphemWriterCCSDS::WriteCcsdsAemData(Real reqEpochInSecs, const Real quat[4])
{
   // future work
}


//------------------------------------------------------------------------------
// void WriteCcsdsDataComments(const std::string &comments, bool writeCmtsNow,
//                         bool ignoreBlanks = true, bool writeKeyword = true)
//------------------------------------------------------------------------------
/**
 * Writes actual data COMMENT section or blank lines
 *
 * @param  comments  Comments to write
 * @param  writeKeyword  Writes COMMENT keyword followed by comments, 
 *                       writes blank otherwise
 */
//------------------------------------------------------------------------------
void EphemWriterCCSDS::WriteCcsdsDataComments(const std::string &comments, bool writeCmtsNow,
                                              bool ignoreBlanks, bool writeKeyword)
{
   #ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::WriteCcsdsDataComments() entered, comments='%s', writeCmtsNow=%d, "
       "ignoreBlanks=%d, writeKeyword=%d\n", comments.c_str(), writeCmtsNow, ignoreBlanks,
       writeKeyword);
   #endif
   
   
   #ifdef DEBUG_TO_TEXT_EPHEM
   std::string ccsdsComments = comments;
   if (writeKeyword)
      ccsdsComments = "COMMENT  " + comments;
   else if (!writeCmtsNow)
      ccsdsComments = "         " + comments;
   WriteStringToFile("\n" + ccsdsComments + "\n");
   #endif
   
   
   if (ccsdsOemWriter)
   {
      std::string cmts = GmatStringUtil::RemoveAllBlanks(comments);
      #ifdef DEBUG_EPHEMFILE_COMMENTS
      MessageInterface::ShowMessage("cmts after removing blanks = '%s'\n", cmts.c_str());
      #endif
      if (cmts == "" ||
          (GmatStringUtil::StartsWith(cmts, "\n") &&
           GmatStringUtil::EndsWith(cmts, "\n")))
      {
         // If writing comments now, write comments as a string without keyword
         if (writeCmtsNow)
         {
            #ifdef DEBUG_EPHEMFILE_COMMENTS
            MessageInterface::ShowMessage("   ==> Writing blanks without keyword\n");
            #endif
            ccsdsOemWriter->WriteString(comments);
         }
         else
         {
            #ifdef DEBUG_EPHEMFILE_COMMENTS
            MessageInterface::ShowMessage("   ==> Writing blank line\n");
            #endif
            ccsdsOemWriter->WriteBlankLine();
         }
      }
      else
      {
         #ifdef DEBUG_EPHEMFILE_COMMENTS
         MessageInterface::ShowMessage
            ("   ==> Adding '%s' to data comments\n", comments.c_str());
         #endif
         ccsdsOemWriter->AddDataComment(comments);
         
         // Write comments if error message
         if (writeCmtsNow)
         {
            #ifdef DEBUG_EPHEMFILE_COMMENTS
            MessageInterface::ShowMessage
               ("   ==> Writing '%s' to a ephem file\n", comments.c_str());
            #endif
            ccsdsOemWriter->WriteDataComments();
         }
      }
   }
   #ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage
      ("EphemWriterCCSDS::WriteCcsdsDataComments() leaving\n");
   #endif
}

