//$Id$
//------------------------------------------------------------------------------
//                                  EphemWriterSTK
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
// Created: 2016.02.04
//
/**
 * Writes a spacecraft orbit states or attitude to an ephemeris file in
 * STK EphemerisTimePosVel format.
 */
//------------------------------------------------------------------------------

#include "EphemWriterSTK.hpp"
#include "SubscriberException.hpp"   // for exception
#include "MessageInterface.hpp"


//#define DEBUG_EPHEMFILE_INSTANCE
//#define DEBUG_EPHEMFILE_INIT
//#define DEBUG_EPHEMFILE_CREATE
//#define DEBUG_EPHEMFILE_STK
//#define DEBUG_STK_DATA_SEGMENT
//#define DEBUG_EPHEMFILE_BUFFER
//#define DEBUG_EPHEMFILE_WRITE
//#define DEBUG_EPHEMFILE_FINISH
//#define DEBUG_EPHEMFILE_RESTART
//#define DEBUG_EPHEMFILE_READBACK
//#define DEBUG_DISTANCEUNIT

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
// EphemWriterSTK(const std::string &name, const std::string &type = "EphemWriterSTK")
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EphemWriterSTK::EphemWriterSTK(const std::string &name, const std::string &type) :
   EphemWriterWithInterpolator(name, type),
   stkEphemFile     (NULL),
   stkVersion       ("stk.v.10.0"),
   stkWriteFailed   (true)
{
   fileType = STK_TIMEPOSVEL;
}


//------------------------------------------------------------------------------
// ~EphemWriterSTK()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EphemWriterSTK::~EphemWriterSTK()
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterSTK::~EphemWriterSTK() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
      
   // Delete STK ephemeris
   if (stkEphemFile)
      delete stkEphemFile;
   
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterSTK::~EphemWriterSTK() <%p>'%s' leaving\n", this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// EphemWriterSTK(const EphemWriterSTK &ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EphemWriterSTK::EphemWriterSTK(const EphemWriterSTK &ef) :
   EphemWriterWithInterpolator(ef),
   stkEphemFile     (NULL),
   stkVersion       (ef.stkVersion),
   stkWriteFailed   (ef.stkWriteFailed)
{
   coordConverter = ef.coordConverter;
}


//------------------------------------------------------------------------------
// EphemWriterSTK& EphemWriterSTK::operator=(const EphemWriterSTK& ef)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
EphemWriterSTK& EphemWriterSTK::operator=(const EphemWriterSTK& ef)
{
   if (this == &ef)
      return *this;
   
   EphemWriterWithInterpolator::operator=(ef);
   
   stkEphemFile     = NULL;
   stkVersion       = ef.stkVersion;
   stkWriteFailed   = ef.stkWriteFailed;
   
   return *this;
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool EphemWriterSTK::Initialize()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemWriterSTK::Initialize() <%p>'%s' entered, spacecraftName='%s', "
       "isInitialized=%d\n   ephemType='%s', stateType='%s', "
       "outputFormat='%s'\n", this, ephemName.c_str(), spacecraftName.c_str(), 
       isInitialized, ephemType.c_str(), stateType.c_str(),
       outputFormat.c_str());
   #endif
   
   if (isInitialized)
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("EphemWriterSTK::Initialize() <%p>'%s' is already initialized so just returning true\n",
          this, ephemName.c_str());
      #endif
      return true;
   }
   
   EphemWriterWithInterpolator::Initialize();
   
   // Maximum segment size is not used by STK ephem writer
   
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
      ("EphemWriterSTK::Initialize() <%p>'%s' returning true, writeOrbit=%d, writeAttitude=%d, "
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
 * This method returns a clone of the EphemWriterSTK.
 *
 * @return clone of the EphemWriterSTK.
 *
 */
//------------------------------------------------------------------------------
EphemerisWriter* EphemWriterSTK::Clone(void) const
{
   return (new EphemWriterSTK(*this));
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
void EphemWriterSTK::Copy(const EphemerisWriter* orig)
{
   operator=(*((EphemWriterSTK *)(orig)));
}


//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
// void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21], const Real accel[3])
//------------------------------------------------------------------------------
void EphemWriterSTK::BufferOrbitData(Real epochInDays, const Real state[6],
                                     const Real cov[21], const Real accel[3],
                                     const Real quat[4])
{
   #ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
      ("BufferOrbitData() <%p>'%s' entered, epochInDays=%.15f, state[0]=%.15f\n",
       this, ephemName.c_str(), epochInDays, state[0]);
   DebugWriteTime("   epochInDays = ", epochInDays, true, 2);
   #endif
   
   // STK ephem does not enforce maxSegmentSize
   
   // Add new data point
   A1Mjd *a1mjd = new A1Mjd(epochInDays);
   a1MjdArray.push_back(a1mjd);

   Rvector6 *rv6 = new Rvector6(state);
   stateArray.push_back(rv6);
   
   Rvector *covar = new Rvector(21);
   covar->Set((Real *)cov, 21);
   covArray.push_back(covar);
   
   Rvector3 *acc = new Rvector3(accel[0], accel[1], accel[2]);
   accelArray.push_back(acc);

   Rvector *rvacov = new Rvector(30);
   for (Integer i = 0; i < 6; ++i)
      (*rvacov)[i] = state[i];
   for (Integer i = 0; i < 6; ++i)
      (*rvacov)[i+6] = accel[i];
   for (Integer i = 0; i < 21; ++i)
      (*rvacov)[i] = cov[i];
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
/**
 * Creates ephemeris file writer.
 */
//------------------------------------------------------------------------------
void EphemWriterSTK::CreateEphemerisFile(bool useDefaultFileName,
                                         const std::string &stType,
                                         const std::string &outFormat,
                                         const std::string &covFormat)
{
   EphemerisWriter::CreateEphemerisFile(useDefaultFileName, stType, outFormat, covFormat);
   CreateSTKEphemerisFile();
   isEphemFileOpened = true;
}


//------------------------------------------------------------------------------
// void CreateSTKEphemerisFile()
//------------------------------------------------------------------------------
/**
 * Creates ephemeris file writer and sets STK file header values.
 */
//------------------------------------------------------------------------------
void EphemWriterSTK::CreateSTKEphemerisFile()
{
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("\nEphemWriterSTK::CreateSTKEphemerisFile() this=<%p>'%s' entered, stkEphemFile=<%p>,"
       " outputFormat='%s'\n", this, ephemName.c_str(), stkEphemFile, outputFormat.c_str());
   #endif
   
   // If stkEphemFile is not NULL, delete it first
   if (stkEphemFile != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (stkEphemFile, "stkEphemFile", "EphemWriterSTK::CreateSTKEphemerisFile()",
          "deleting local stkEphemFile");
      #endif
      delete stkEphemFile;
      stkEphemFile = NULL;
   }
   
   std::string timeSystem  = "UTC";   // Figure out time system here
   std::string centralBody = outCoordSystem->GetOriginName();
   if (centralBody == "Luna")
      centralBody = "Moon";

   std::string coordSystemName = outCoordSystem->GetName(); 
   std::string axisTypeName = (outCoordSystem->GetAxisSystem())->GetTypeName(); 
   std::string coordBaseSystem = outCoordSystem->GetBaseSystem();
   
   // Figure out what cs type to write (ICRF, J2000, Fixed)
   std::string csTypeName = axisTypeName;
   if (axisTypeName == "MJ2000Eq")
      csTypeName = "J2000";
   else if (axisTypeName == "BodyFixed")
      csTypeName = "Fixed";
   else if (axisTypeName == "ICRF")
      csTypeName = "ICRF";
   else if (axisTypeName == "MJ2000Ec")
      csTypeName = "J2000_Ecliptic";
   else if (axisTypeName == "ObjectReferenced")
      csTypeName = "ObjectReferenced";
   else if (axisTypeName == "TODEq")
   {
      csTypeName = "TrueOfDate";

      if (centralBody != "Earth")
         throw SubscriberException("Use of the TrueOfDate axes in STK ephemeris files is not permitted for central bodies other than Earth. See the User's Guide for more details.");
   }
   else if (axisTypeName == "Inertial" || axisTypeName == "BodyInertial")
      throw SubscriberException("Use of the Inertial axes in STK ephemeris files is not permitted. J2000 or ICRF are recommended for use instead.");
   else if (axisTypeName == "MODEq")
      throw SubscriberException("Use of MeanOfDate (MODEq) axes is not permitted in an STK Ephemeris file.\n");
   else if (axisTypeName == "TOEEq")
      throw SubscriberException("Use of TrueOfEpoch (TOEEq) axes is not permitted in an STK Ephemeris file.\n");
   else if (axisTypeName == "MOEEq")
      throw SubscriberException("Use of MeanOfEpoch (MOEEq) axes is not permitted in an STK Ephemeris file.\n");
   else if (axisTypeName == "LocalAlignedConstrained")
      throw SubscriberException("Use of LocalAlignedConstrained axes is not permitted in an STK Ephemeris file.\n");
   else if (axisTypeName == "Equator")
      throw SubscriberException("Use of Equator axes is not permitted in an STK Ephemeris file.\n");
   else if (axisTypeName == "GSE")
      throw SubscriberException("Use of GSE axes is not permitted in an STK Ephemeris file.\n");
   else if (axisTypeName == "GSM")
      throw SubscriberException("Use of GSM axes is not permitted in an STK Ephemeris file.\n");
   else if (axisTypeName == "Topocentric")
      throw SubscriberException("Use of Topocentric axes is not permitted in an STK Ephemeris file.\n");
   else if (axisTypeName == "BodySpinSun")
      throw SubscriberException("Use of BodySpinSun axes is not permitted in an STK Ephemeris file.\n");
   else
   {
      std::string msg = "Use of " + axisTypeName + " axes is not permitted in an STK Ephemeris file.\n";
      throw SubscriberException(msg);
   }
      

   // Determine what covariance to write
   std::string ephemCovType = "";
   if (covarianceFormat == "Position")
      ephemCovType = "TimePos";
   else if (covarianceFormat == "PositionAndVelocity")
      ephemCovType = "TimePosVel";

   if (covarianceFormat != "None")
   {
      if (csTypeName != "J2000")
      {
         std::string msg = "Writing covariance to an STK ephemeris file with a coordinate system other than J2000 is not allowed.\n";
         throw SubscriberException(msg);
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("   Creating STKEphemerisFile with timeSystem='%s', centralBody='%s', "
       "coordSystemName='%s', axisTypeName='%s', csTypeName='%s', coordBaseSystem='%s'\n",
       timeSystem.c_str(), centralBody.c_str(), coordSystemName.c_str(), axisTypeName.c_str(),
       csTypeName.c_str(), coordBaseSystem.c_str());
   #endif
   
   try
   {
      stkEphemFile = new STKEphemerisFile;
      //std::string distanceUnit = stkEphemFile->GetDistanceUnit();

      #ifdef DEBUG_DISTANCEUNIT
      MessageInterface::ShowMessage
         ("   Retrieved the following distanceUnit: %s", distanceUnit.c_str());
      #endif

      if (stkEphemFile->OpenForWrite(fullPathFileName, "TimePosVel", ephemCovType))
      {
         stkEphemFile->SetVersion(stkVersion);
         if (useFixedStepSize)
         {
            stkEphemFile->SetHeaderForWriting("InterpolationMethod", interpolatorName);
            stkEphemFile->SetInterpolationOrder(interpolationOrder);
         }
         stkEphemFile->SetHeaderForWriting("CentralBody", centralBody);
         stkEphemFile->SetHeaderForWriting("CoordinateSystem", csTypeName);
         stkEphemFile->SetHeaderForWriting("DistanceUnit", distanceUnit);

         stkEphemFile->SetIncludeEventBoundaries(includeEventBoundaries);
      }
      else
      {
         SubscriberException se;
         se.SetDetails("Unable to open STK ephemeris file: '%s'\n", fullPathFileName.c_str());
         throw se;
      }
   }
   catch (BaseException &e)
   {
      // Keep from getting a compiler warning about e not being used
      e.GetMessageType();
      
      #ifdef DEBUG_EPHEMFILE_CREATE
      MessageInterface::ShowMessage
         ("Error opening STKEphemerisFile: %s", (e.GetFullMessage()).c_str());
      #endif
      throw;
   }
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (stkEphemFile, "stkEphemFile", "EphemWriterSTK::CreateSTKEphemerisFile()",
       "stkEphemFile = new SpiceOrbitKernelWriter()");
   #endif
   
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("EphemWriterSTK::CreateSTKEphemerisFile() <%p>'%s' leaving, stkEphemFile=<%p>\n",
       this, ephemName.c_str(), stkEphemFile);
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
bool EphemWriterSTK::NeedToHandleBackwardProp()
{
   #if DBGLVL_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemWriterSTK::NeedToHandleBackwardProp() entered\n");
   #endif
   
   // Throw an exception for STK as backward prop is not allowed
   throw SubscriberException(GetBackwardPropWarning());
}


//------------------------------------------------------------------------------
// void HandleOrbitData()
//------------------------------------------------------------------------------
/** Handles writing orbit data includes checking epoch to write if writing at
 * fixed step size.
 */
//------------------------------------------------------------------------------
void EphemWriterSTK::HandleOrbitData()
{
   // Check user defined initial and final epoch
   bool processData = CheckInitialAndFinalEpoch();
   
   #if DBGLVL_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemWriterSTK::HandleOrbitData() checked initial and final epoch\n");
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
   HandleSTKOrbitData(processData, timeToWrite);
}


//------------------------------------------------------------------------------
// void StartNewSegment(const std::string &comments, bool saveEpochInfo,
//                      bool writeAfterData, bool ignoreBlankComments)
//------------------------------------------------------------------------------
/**
 * Finishes writing remaining data and resets flags to start new segment.
 */
//------------------------------------------------------------------------------
void EphemWriterSTK::StartNewSegment(const std::string &comments,
                                         bool saveEpochInfo, bool writeAfterData,
                                         bool ignoreBlankComments)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterSTK::StartNewSegment() entered\n   comments='%s'\n   "
       "saveEpochInfo=%d, writeAfterData=%d, ignoreBlankComments=%d, canFinalize=%d, firstTimeWriting=%d\n",
       comments.c_str(), saveEpochInfo, writeAfterData, ignoreBlankComments, canFinalize, firstTimeWriting);
   #endif
   
   // If no first data has written out yet, just return
   if (firstTimeWriting)
   {
      #ifdef DEBUG_EPHEMFILE_RESTART
      MessageInterface::ShowMessage
         ("EphemWriterSTK::StartNewSegment() returning, no first data written out yet\n");
      #endif
      return;
   }
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("EphemWriterSTK::StartNewSegment() Calling FinishUpWriting(), canFinalize=%d\n",
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
   
   if (stkEphemFile != NULL)
      WriteSTKOrbitDataSegment(false);
   
   // Initialize data
   InitializeData(saveEpochInfo);
   
   ++numSegments;
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterSTK::StartNewSegment() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void FinishUpWriting()
//------------------------------------------------------------------------------
/*
 * Finishes up writing data at epochs on waiting.
 */
//------------------------------------------------------------------------------
void EphemWriterSTK::FinishUpWriting()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterSTK::FinishUpWriting() '%s' entered, canFinalize=%d, isFinalized=%d, "
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
      
      FinishUpWritingSTK();
   }
      
   if (canFinalize)
   {
      if (isEndOfRun)
      {
         #ifdef DEBUG_EPHEMFILE_FINISH
         MessageInterface::ShowMessage("   It is end of run, so closing ephemeris file\n");
         #endif

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
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterSTK::FinishUpWriting() leaving, isFinalized=%d\n", isFinalized);
   #endif
}


//------------------------------------------------------------------------------
// void CloseEphemerisFile(bool done = true, writeMetaData = true)
//------------------------------------------------------------------------------
void EphemWriterSTK::CloseEphemerisFile(bool done, bool writeMetaData)
{
   FinalizeSTKEphemeris();
}

//------------------------------------------------------------------------------
// void HandleSTKOrbitData(bool writeData, bool timeToWrite)
//------------------------------------------------------------------------------
void EphemWriterSTK::HandleSTKOrbitData(bool writeData, bool timeToWrite)
{
   #ifdef DEBUG_EPHEMFILE_STK
   MessageInterface::ShowMessage
      ("EphemWriterSTK::HandleSTKOrbitData() <%p>'%s' entered, writeData=%d, "
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
         #ifdef DEBUG_EPHEMFILE_STK
         MessageInterface::ShowMessage
            ("   ==> 10 min interval is over, so setting timeToWrite to true\n");
         #endif
         
         timeToWrite = true;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_STK
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
      if (writingNewSegment)
      {
         //WriteComments("********** NEW SEGMENT **********");
         #ifdef DEBUG_EPHEMFILE_WRITE
         DebugWriteTime
            ("********** WRITING NEW SEGMENT AT currEpochInSecs = ", currEpochInSecs, false, 2);
         #endif
         
         WriteSTKOrbitDataSegment(false);
      }
      
      if (writeOrbit)
         HandleWriteOrbit();
      
      if (firstTimeWriting)
         firstTimeWriting = false;
      
      if (writingNewSegment)
         writingNewSegment = false;
   }
   
   #ifdef DEBUG_EPHEMFILE_STK
   MessageInterface::ShowMessage
      ("EphemWriterSTK::HandleSTKOrbitData() leaving, firstTimeWriting=%d, writingNewSegment=%d\n",
       firstTimeWriting, writingNewSegment);
   #endif
}


//------------------------------------------------------------------------------
// void FinishUpWritingSTK()
//------------------------------------------------------------------------------
/**
 * Writes final data segment.
 */
//------------------------------------------------------------------------------
void EphemWriterSTK::FinishUpWritingSTK()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterSTK::FinishUpWritingSTK() entered, canFinalize=%d\n", canFinalize);
   #endif
   
   if (interpolator != NULL && useFixedStepSize)
   {
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage
         ("===> FinishUpWritingSTK() checking for not enough data points\n"
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
   if (stkEphemFile != NULL)
   {
      WriteSTKOrbitDataSegment(canFinalize);
   }
   else
   {
      if (a1MjdArray.size() > 0)
      {
         throw SubscriberException
            ("*** INTERNAL ERROR *** STKEphemFile is NULL in "
             "EphemWriterSTK::FinishUpWritingSTK()\n");
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemWriterSTK::FinishUpWritingSTK() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteSTKOrbitDataSegment(bool canFinish)
//------------------------------------------------------------------------------
/**
 * Writes orbit data segment to STK file and deletes data array
 */
//------------------------------------------------------------------------------
void EphemWriterSTK::WriteSTKOrbitDataSegment(bool canFinish)
{
   #ifdef DEBUG_STK_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("=====> WriteSTKOrbitDataSegment() <%p>'%s' entered, canFinish=%d, "
       "isEndOfRun=%d, a1MjdArray.size()=%d, stateArray.size()=%d\n", this,
       ephemName.c_str(), canFinish, isEndOfRun, a1MjdArray.size(), stateArray.size());
   #endif
   
   if (a1MjdArray.size() > 0)
   {
      if (stkEphemFile == NULL)
         throw SubscriberException
            ("*** INTERNAL ERROR *** STK Ephem Writer is NULL in "
             "EphemWriterSTK::WriteSTKOrbitDataSegment()\n");
      
      #ifdef DEBUG_STK_DATA_SEGMENT
      A1Mjd *start = a1MjdArray.front();
      A1Mjd *end   = a1MjdArray.back();
      MessageInterface::ShowMessage
         ("   Writing start=%.15f, end=%.15f\n", start->GetReal(), end->GetReal());
      MessageInterface::ShowMessage("There are %d epochs and states:\n", a1MjdArray.size());
      #ifdef DEBUG_STK_DATA_SEGMENT_MORE
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
      #endif
      
      stkWriteFailed = false;
      try
      {
         #ifdef DEBUG_STK_DATA_SEGMENT
         MessageInterface::ShowMessage
            (".....Calling stkEphemFile->WriteDataSegment() stkEphemFile=<%p>, "
             "isEndOfRun=%d, epochsOnWaiting.size()=%d\n", stkEphemFile, isEndOfRun,
             epochsOnWaiting.size());
         if (!epochsOnWaiting.empty())
            DebugWriteTime("   ", epochsOnWaiting.back());
         #endif
         // Check if STK ephemeris file can be finalized (GMT-4060 fix)
         bool finalize = isEndOfRun && canFinish;
         stkEphemFile->WriteDataSegment(a1MjdArray, stateArray, covArray, finalize);
         ClearOrbitData();
      }
      catch (BaseException &e)
      {
         ClearOrbitData();
         stkWriteFailed = true;
         #ifdef DEBUG_STK_DATA_SEGMENT
         MessageInterface::ShowMessage("**** ERROR **** " + e.GetFullMessage());
         #endif
         e.SetFatal(true);
         throw;
      }
   }
   
   #ifdef DEBUG_STK_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("=====> WriteSTKOrbitDataSegment() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// FinalizeSTKEphemeris()
//------------------------------------------------------------------------------
void EphemWriterSTK::FinalizeSTKEphemeris()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemWriterSTK::FinalizeSTKEphemeris() entered\n");
   #endif
   
   if (stkEphemFile == NULL)
      throw SubscriberException
         ("*** INTERNAL ERROR *** STK Ephem Writer is NULL in "
          "EphemWriterSTK::FinalizeSTKEphemeris()\n");
   
   // Write any final data
   stkEphemFile->FinalizeEphemeris();
   
   #ifdef DEBUG_EPHEMFILE_READBACK
   if (isEndOfRun)
   {
      MessageInterface::ShowMessage
         ("===> EphemWriterSTK::FinalizeSTKEphemeris() calling stkEphemFile "
          "for debug output\n   fullPathFileName = '%s'\n", fullPathFileName.c_str());
      stkEphemFile->OpenForRead(fullPathFileName, 1, 1);
      //@todo Add more code to read data
      stkEphemFile->CloseForRead();
   }
   #endif
   
   // Close STK ephemeris file
   stkEphemFile->CloseForWrite();
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemWriterSTK::FinalizeSTKEphemeris() leaving\n");
   #endif
}

void EphemWriterSTK::SetDistanceUnit(const std::string &dU)
{
   distanceUnit = dU;
}

void EphemWriterSTK::SetIncludeEventBoundaries(bool iEB)
{
   includeEventBoundaries = iEB;
}
