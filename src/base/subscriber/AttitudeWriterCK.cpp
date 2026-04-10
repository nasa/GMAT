//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeWriterCK
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under
// FDSS III.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: September 02, 2025
//
/**
 * Writes a spacecraft orbit states or attitude to an ephemeris file in
 * CK format.
 */
 //------------------------------------------------------------------------------

#include "AttitudeWriterCK.hpp"
#include "CelestialBody.hpp"
#include "SubscriberException.hpp"   // for exception
#include "RealUtilities.hpp"         // for IsEven()
#include "MessageInterface.hpp"
#include "FileUtil.hpp"
#include <sstream>                   // for <<, std::endl

#ifdef __USE_SPICE__
#include "SpiceAttitudeKernelWriter.hpp"
#endif

//#define DEBUG_EPHEMFILE_INSTANCE
//#define DEBUG_EPHEMFILE_INIT
//#define DEBUG_EPHEMFILE_CREATE
//#define DEBUG_EPHEMFILE_SPICE
//#define DEBUG_EPHEMFILE_BUFFER
//#define DEBUG_EPHEMFILE_ORBIT
//#define DEBUG_EPHEMFILE_WRITE
//#define DEBUG_EPHEMFILE_FINISH
//#define DEBUG_EPHEMFILE_RESTART
//#define DEBUG_EPHEMFILE_COMMENTS
//#define DEBUG_EPHEMFILE_TEXT

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
// AttitudeWriterCK(const std::string &name, const std::string &type = "AttitudeWriterCK")
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
 //------------------------------------------------------------------------------
AttitudeWriterCK::AttitudeWriterCK(const std::string& name, const std::string& type) :
   EphemerisWriter(type, name),
   ckWriter(NULL),
   sclkFileName(""),
   ckWriteFailed(false),
   numCKSegmentsWritten(0)
{
   fileType = CK_ATTITUDE;

#ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::AttitudeWriterCK() <%p>'%s' entered\n", this, ephemName.c_str());
#endif
}


//------------------------------------------------------------------------------
// ~AttitudeWriterCK()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
 //------------------------------------------------------------------------------
AttitudeWriterCK::~AttitudeWriterCK()
{
#ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::~AttitudeWriterCK() <%p>'%s' entered\n", this, ephemName.c_str());
#endif

#ifdef __USE_SPICE__
#ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
   ("   ckWriter=<%p>, ckWriteFailed=%s\n", ckWriter, (ckWriteFailed ? "true" : "false"));
#endif
   if (ckWriter != NULL)
   {
      if (!ckWriteFailed)
         FinalizeCKFile();

      ckWriter->UnloadKernel(sclkFileName.c_str());

#ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
      (ckWriter, "CK writer", "AttitudeWriterCK::~AttitudeWriterCK()()",
         "deleting local CK writer");
#endif
      delete ckWriter;
   }
   else
   {
      // Create a temporary spice interface to unload SCLK
      SpiceInterface* tempSpice = new SpiceInterface();
      tempSpice->UnloadKernel(sclkFileName.c_str());
      delete tempSpice;
   }

   remove(sclkFileName.c_str());
#endif

#ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::~AttitudeWriterCK() <%p>'%s' leaving\n", this, ephemName.c_str());
#endif
}


//------------------------------------------------------------------------------
// AttitudeWriterCK(const AttitudeWriterCK &ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
 //------------------------------------------------------------------------------
AttitudeWriterCK::AttitudeWriterCK(const AttitudeWriterCK& ef) :
   EphemerisWriter(ef),
   ckWriter(NULL),
   sclkFileName(ef.sclkFileName),
   ckWriteFailed(ef.ckWriteFailed),
   numCKSegmentsWritten(ef.numCKSegmentsWritten)
{
}


//------------------------------------------------------------------------------
// AttitudeWriterCK& AttitudeWriterCK::operator=(const AttitudeWriterCK& ef)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
 //------------------------------------------------------------------------------
AttitudeWriterCK& AttitudeWriterCK::operator=(const AttitudeWriterCK& ef)
{
   if (this == &ef)
      return *this;

   EphemerisWriter::operator=(ef);

   ckWriter = NULL;
   sclkFileName = ef.sclkFileName;
   ckWriteFailed = ef.ckWriteFailed;
   numCKSegmentsWritten = ef.numCKSegmentsWritten;

   return *this;
}

//---------------------------------
// methods for this class
//---------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool AttitudeWriterCK::Initialize()
{
   bool retval = EphemerisWriter::Initialize();

   // Set file format
   fileType = UNKNOWN_FILE_TYPE;
   if (ephemType == "CK" && stateType == "Quaternion")
      fileType = CK_ATTITUDE;
   else
      return false;

   // Set maximum segment size
   maxSegmentSize = 1000;

   return retval;
}


//------------------------------------------------------------------------------
//  EphemerisWriter* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the AttitudeWriterCK.
 *
 * @return clone of the AttitudeWriterCK.
 *
 */
 //------------------------------------------------------------------------------
EphemerisWriter* AttitudeWriterCK::Clone(void) const
{
   return (new AttitudeWriterCK(*this));
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
void AttitudeWriterCK::Copy(const EphemerisWriter* orig)
{
   operator=(*((AttitudeWriterCK*)(orig)));
}


//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
// void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21], const Real accel[3])
//------------------------------------------------------------------------------
/**
 * Buffer the orbit data to the kernel writer. For the CK file, we only care
 * about the quaternions.
 * 
 * @param epochInDays   The current epoch for the orbit data received
 * @param state         The state vector
 * @param cov           The covariance matrix
 * @param accel         The acceleration vector
 * @param quat          The quaternion vector
 */
 //------------------------------------------------------------------------------
void AttitudeWriterCK::BufferOrbitData(Real epochInDays, const Real state[6],
                                       const Real cov[21], const Real accel[3],
                                       const Real quat[4])
{
#ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
   ("BufferOrbitData() entered, epochInDays=%.15f, state[0]=%.15f\n", epochInDays,
      state[0]);
   DebugWriteTime("   epochInDays = ", epochInDays, true, 2);
#endif

   // if buffer is full, dump the data
   if (a1MjdArray.size() >= maxSegmentSize)
   {
      // Save last data to become first data of next segment
      A1Mjd* a1mjd = new A1Mjd(*a1MjdArray.back());
      Rvector* quatVec = new Rvector(*quatArray.back());

      // Write a segment and delete data array pointers
      WriteCKAttitudeDataSegment();

      // Add saved data to arrays
      a1MjdArray.push_back(a1mjd);
      quatArray.push_back(quatVec);
   }

   // Add new data point
   A1Mjd* a1mjd = new A1Mjd(epochInDays);
   Rvector* quatVec = new Rvector(4, quat[0], quat[1], quat[2], quat[3]);
   a1MjdArray.push_back(a1mjd);
   quatArray.push_back(quatVec);

#ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
   ("BufferOrbitData() leaving, there is(are) %d data point(s)\n", a1MjdArray.size());
#endif
} // BufferOrbitData()


//------------------------------------------------------------------------------
// void CreateEphemerisFile(bool useDefaultFileName, const std::string &stType,
//                          const std::string &outFormat, const std::string &covFormat)
//------------------------------------------------------------------------------
/**
 * Pass the kernely type data and call to create the new CK file
 * 
 * @param useDefaultFileName  Whether to use the default file name
 * @param stType              The state type in use
 * @param outFormat           The output format of the ephemeris file
 * @param covFormat           The covariance format of the ephemeris file
 */
//------------------------------------------------------------------------------
void AttitudeWriterCK::CreateEphemerisFile(bool useDefaultFileName,
   const std::string& stType,
   const std::string& outFormat,
   const std::string& covFormat)
{
#ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::CreateEphemerisFile() <%p> entered, useDefaultFileName=%d, "
      "outFormat='%s'\n   fullPathFileName='%s'\n", this, useDefaultFileName,
      outFormat.c_str(), fullPathFileName.c_str());
#endif

   EphemerisWriter::CreateEphemerisFile(useDefaultFileName, stType, outFormat, covFormat);

   CreateSpiceKernelWriter();
   isEphemFileOpened = true;

#ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::CreateEphemerisFile() <%p> leaving, writingNewSegment=%d\n   "
      "fullPathFileName='%s'\n", this, writingNewSegment, fullPathFileName.c_str());
#endif
}


//------------------------------------------------------------------------------
// void CreateSpiceKernelWriter()
//------------------------------------------------------------------------------
/**
 * Create the SPICE kernel writer object
 */
//------------------------------------------------------------------------------
void AttitudeWriterCK::CreateSpiceKernelWriter()
{
#ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::CreateSpiceKernelWriter() entered, ckWriter=<%p>, "
      "isInitialized=%d, firstTimeWriting=%d\n   prevFileName='%s'\n   "
      "    fileName='%s', spacecraft=<%p>\n", ckWriter, isInitialized, firstTimeWriting,
      prevFileName.c_str(), fileName.c_str(), spacecraft);
#endif

   if (spacecraft == NULL)
   {
#ifdef DEBUG_EPHEMFILE_CREATE
      MessageInterface::ShowMessage
      ("AttitudeWriterCK::CreateSpiceKernelWriter() just leaving, spacecraft is not set\n");
#endif
      return;
   }

   //=======================================================
#ifdef __USE_SPICE__
   // Create CKWrite if it is NULL
   if (ckWriter == NULL)
   {
      std::string name = spacecraft->GetName();
      std::string centerName = outCoordSystem->GetOriginName();
      Integer objNAIFId = spacecraft->GetIntegerParameter("NAIFId");
      Integer centerNAIFId = (outCoordSystem->GetOrigin())->GetIntegerParameter("NAIFId");
      std::stringstream ss("");
      ss << "tmp_" << name << "SCLK" << "_" << GmatTimeUtil::FormatCurrentTime(4);
      ss << ".tsc";
      sclkFileName = GmatFileUtil::GetTemporaryDirectory() + ss.str();

#ifdef DEBUG_EPHEMFILE_CREATE
      MessageInterface::ShowMessage
      ("   Creating SpiceAttitudeKernelWriter with name='%s', centerName='%s', "
         "objNAIFId=%d, centerNAIFId=%d\n   fileName='%s', interpolationOrder=%d\n",
         name.c_str(), centerName.c_str(), objNAIFId, centerNAIFId,
         fileName.c_str(), interpolationOrder);
#endif

      try
      {
         std::string attType = ((Attitude*)spacecraft->GetRefObject(Gmat::ATTITUDE, "Attitude"))->GetAttitudeModelName();
         bool writeSCLK = true;
         if (attType == "SpiceAttitude")
            writeSCLK = false;
         ckWriter =
            new SpiceAttitudeKernelWriter(name, objNAIFId,
               fullPathFileName, interpolationOrder, "J2000", sclkFileName, writeSCLK);
#ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
         (ckWriter, "ckWriter", "AttitudeWriterCK::CreateSpiceKernelWriter()",
            "ckWriter = new SpiceAttitudeKernelWriter()");
#endif
      }
      catch (BaseException& e)
      {
         // Keep from setting a warning
         e.GetMessageType();

#ifdef DEBUG_EPHEMFILE_CREATE
         MessageInterface::ShowMessage(
            "  Error creating SpiceAttitudeKernelWriter: %s", (e.GetFullMessage()).c_str());
#endif
         throw;
      }
   }
   else if (prevFileName != fileName)
   {
      // Provide new CK file, but we maintain the same SCLK file, 
      // otherwise previous segment ticks will be lost
      ckWriter->SetCKFile(fullPathFileName);
   }

   //=======================================================
#else
//=======================================================
   MessageInterface::ShowMessage
   ("*** WARNING *** Use of SpiceAttitudeKernelWriter is turned off\n");
   //=======================================================
#endif
//=======================================================

#ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::CreateSpiceKernelWriter() leaving, ckWriter=<%p>\n",
      ckWriter);
#endif
}


//------------------------------------------------------------------------------
// void CloseEphemerisFile(bool done = true, writeMetaData = true)
//------------------------------------------------------------------------------
/**
 * Finalize and close the CK file
 */
//------------------------------------------------------------------------------
void AttitudeWriterCK::CloseEphemerisFile(bool done, bool writeMetaData)
{
   // Close CK file
#ifdef __USE_SPICE__
#ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::CloseAttitudeWriterCK() ckWriter=<%p>, ckWriteFailed=%s\n",
      ckWriter, (ckWriteFailed ? "true" : "false"));
#endif
   if (ckWriter != NULL)
   {
      if (!ckWriteFailed)
         FinalizeCKFile(done, writeMetaData);

#ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
      (ckWriter, "CK writer", "AttitudeWriterCK::~AttitudeWriterCK()()",
         "deleting local CK writer");
#endif
      if (done)
      {
         delete ckWriter;
         ckWriter = NULL;
      }
   }
#endif
}


//------------------------------------------------------------------------------
// void HandleOrbitData()
//------------------------------------------------------------------------------
/**
 * Check if we are processing the current data and pass if off to the CK
 * specific handler
 */
//------------------------------------------------------------------------------
void AttitudeWriterCK::HandleOrbitData()
{
   // Check user defined initial and final epoch
   bool processData = CheckInitialAndFinalEpoch();

#if DBGLVL_EPHEMFILE_DATA
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::HandleOrbitData() checked initial and final epoch\n");
#endif

   // Check if it is time to write
   bool timeToWrite = false;

#if DBGLVL_EPHEMFILE_DATA > 0
   MessageInterface::ShowMessage
   ("   Start writing data, currEpochInDays=%.15f, currEpochInSecs=%.15f, %s\n"
      "   writeOrbit=%d, writeAttitude=%d, processData=%d, timeToWrite=%d\n",
      currEpochInDays, currEpochInSecs, ToUtcGregorian(currEpochInSecs).c_str(),
      writeOrbit, writeAttitude, processData, timeToWrite);
#endif

   // For now we only write Orbit data
   HandleCKOrbitData(processData, timeToWrite);
}


//------------------------------------------------------------------------------
// void HandleWriteOrbit()
//------------------------------------------------------------------------------
/**
 * Call to write the current orbit data
 */
//------------------------------------------------------------------------------
void AttitudeWriterCK::HandleWriteOrbit()
{
#ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::HandleWriteOrbit() entered\n");
#endif

   WriteOrbit(currEpochInSecs, currState, currCov, currAccel, currQuat);

#ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage("AttitudeWriterCK::HandleWriteOrbit() leaving\n");
#endif
}


//------------------------------------------------------------------------------
// void HandleCKOrbitData(bool writeData, bool timeToWrite)
//------------------------------------------------------------------------------
/**
 * If ready to write data, process it for the CK file
 * 
 * @param writeData     Whether or not data is ready to be written
 * @param timeToWrite   Whether or not the data should be written now
 */
//------------------------------------------------------------------------------
void AttitudeWriterCK::HandleCKOrbitData(bool writeData, bool timeToWrite)
{
#ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::HandleCKOrbitData() entered, writeData=%d, timeToWrite=%d, "
      "currEpochInDays = %.13lf, \n   firstTimeWriting=%d\n",
      writeData, timeToWrite, currEpochInDays, firstTimeWriting);
#endif

   if (writeData)
   {
      bool bufferData = false;

      if ((a1MjdArray.empty()) ||
         (!a1MjdArray.empty() && currEpochInDays > a1MjdArray.back()->GetReal()))
         bufferData = true;

      if (bufferData)
      {
         Real outState[6], outCov[21], outAccel[3], outQuat[4];
         // Convert if necessary
         if (!writeDataInDataCS)
         {
            ConvertState(currEpochInDays, currState, outState, currCov, outCov, currAccel, outAccel);
            for (unsigned int ii = 0; ii < 4; ii++)
               outQuat[ii] = currQuat[ii];
         }
         else
         {
            for (unsigned int ii = 0; ii < 6; ii++)
               outState[ii] = currState[ii];
            for (unsigned int ii = 0; ii < 21; ii++)
               outCov[ii] = currCov[ii];
            for (unsigned int ii = 0; ii < 3; ii++)
               outAccel[ii] = currAccel[ii];
            for (unsigned int ii = 0; ii < 4; ii++)
               outQuat[ii] = currQuat[ii];
         }

         BufferOrbitData(currEpochInDays, outState, outCov, outAccel, outQuat);

#ifdef DEBUG_EPHEMFILE_SPICE
         DebugWriteOrbit("In HandleCKOrbitData:", currEpochInDays, currState, true, true);
#endif
      }

      // Set flags
      if (firstTimeWriting)
         firstTimeWriting = false;
   }

#ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::HandleCKOrbitData() leaving, firstTimeWriting=%d\n",
      firstTimeWriting);
#endif
}


//------------------------------------------------------------------------------
// void StartNewSegment(const std::string &comments, bool saveEpochInfo,
//                      bool writeAfterData, bool ignoreBlankComments)
//------------------------------------------------------------------------------
/**
 * Finish writing remaining data and flags to start new segment.
 */
 //------------------------------------------------------------------------------
void AttitudeWriterCK::StartNewSegment(const std::string& comments,
   bool saveEpochInfo, bool writeAfterData,
   bool ignoreBlankComments)
{
#ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
   ("===== AttitudeWriterCK::StartNewSegment() entered\n   comments='%s'\n   "
      "saveEpochInfo=%d, writeAfterData=%d, ignoreBlankComments=%d, canFinalize=%d, firstTimeWriting=%d\n",
      comments.c_str(), saveEpochInfo, writeAfterData, ignoreBlankComments, canFinalize, firstTimeWriting);
#endif

   // If no first data has written out yet, just return
   if (firstTimeWriting)
   {
#ifdef DEBUG_EPHEMFILE_RESTART
      MessageInterface::ShowMessage
      ("AttitudeWriterCK::StartNewSegment() returning, no first data written out yet\n");
#endif
      return;
   }

#ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::StartNewSegment() Calling FinishUpWriting(), canFinalize=%d\n",
      canFinalize);
#endif

   // Finish up writing data
   FinishUpWriting();

   // Set comments
   writeCommentAfterData = writeAfterData;
   currComments = comments;

   if (ckWriter != NULL)
   {
      Integer mnSz = ckWriter->GetMinNumberOfAttitudes();
      Integer numPts = (Integer)a1MjdArray.size();
      if (!generateInBackground || (numPts >= mnSz))
      {
         if (!writeAfterData)
            WriteDataComments(comments, false, ignoreBlankComments);

         WriteCKAttitudeDataSegment();

         if (writeAfterData)
            WriteDataComments(comments, false, ignoreBlankComments);

         insufficientDataPoints = false; // there was enough data
         currComments = "";
      }
      else if (generateInBackground && (numPts > 1))
      {
#ifdef DEBUG_EPHEMFILE_SPICE
         MessageInterface::ShowMessage("NOT WRITING CK data - only %d points available!!!\n",
            numPts);
#endif
         insufficientDataPoints = true; // data is available, but has not been written yet
      }
   }

   // Initialize data
   InitializeData(saveEpochInfo);

#ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
   ("===== AttitudeWriterCK::StartNewSegment() leaving\n");
#endif
}


//------------------------------------------------------------------------------
// void FinishUpWriting()
//------------------------------------------------------------------------------
/*
 * Finishes up writing remaiing data and do appropriate action.
 *
 * @param  canFinalize  If this flag is true it will process epochs on waiting
 *                      and write or overwrite metadata depends on the metadata
 *                      write option [true]
 */
 //------------------------------------------------------------------------------
void AttitudeWriterCK::FinishUpWriting()
{
#ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::FinishUpWriting() '%s' entered, canFinalize=%d, isFinalized=%d, "
      "firstTimeWriting=%d, isEndOfRun=%d\n",
      ephemName.c_str(), canFinalize, isFinalized, firstTimeWriting, isEndOfRun);
   DebugWriteTime("    lastEpochWrote = ", lastEpochWrote);
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   MessageInterface::ShowMessage
   ("   There is(are) %d data point(s) in the buffer, ckWriter=<%p>\n", a1MjdArray.size(),
      ckWriter);
#endif

   if (!isFinalized)
   {
#ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage
      ("   It is not finalized yet, so trying to write the remainder of data\n");
#endif

      FinishUpWritingCK();

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
   ("AttitudeWriterCK::FinishUpWriting() leaving\n");
#endif
}


//------------------------------------------------------------------------------
// void FinishUpWritingCK()
//------------------------------------------------------------------------------
/**
 * Finish writing any current data stored for the CK file, and handle the case
 * of insufficient data provided if necessary.
 */
//------------------------------------------------------------------------------
void AttitudeWriterCK::FinishUpWritingCK()
{
#ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
   ("AttitudeWriterCK::FinishUpWritingCK) entered, canFinalize=%d\n", canFinalize);
#endif
   try
   {
      if (ckWriter != NULL)
      {
         Integer mnSz = ckWriter->GetMinNumberOfAttitudes();
         Integer numPts = (Integer)a1MjdArray.size();
#ifdef DEBUG_EPHEMFILE_SPICE
         MessageInterface::ShowMessage("FinishUpWritingCK::mnSz = %d, numPts = %d\n",
            mnSz, numPts);
         MessageInterface::ShowMessage("FinishUpWritingCK::generateInBackground = %s\n",
            (generateInBackground ? "true" : "false"));
#endif
         // if we are generating CK files in the background and there
         // are not enough states for the interpolation, we DO NOT
         // want to try to write and trigger the SPICE error;
         // for user-specified CK files, we DO want to present
         // errors to the user.
         if (!generateInBackground || (numPts >= mnSz))
         {
#ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("FinishUpWritingCK::about to write CK orbit data segment\n");
#endif

            // Write a segment and delete data array pointers
            WriteCKAttitudeDataSegment();

            insufficientDataPoints = false;
#ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("   DONE writing CK orbit data segment\n");
#endif
         }
         // background CKs need to know if there was data unwritten
         // will have 1 point from the last segment in the beginning of this
         // set of data
         else if (generateInBackground && (numPts > 1))
         {
#ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("NOT WRITING CK data - only %d points available!!!\n",
               numPts);
#endif
            insufficientDataPoints = true; // data is available, but has not been written yet
         }
         ckWriter->WriteCKData();
      }
      else
      {
#ifdef __USE_SPICE__
         if (a1MjdArray.size() > 0)
         {
            throw SubscriberException
            ("*** INTERNAL ERROR *** CK Writer is NULL in "
               "AttitudeWriterCK::FinishUpWritingCK()\n");
         }
#endif
      }
   }
   catch (BaseException& ex)
   {
      // Catch and ignore exceptions thrown from util, since we manage them later
#ifdef DEBUG_EPHEMFILE_WRITE
      MessageInterface::ShowMessage
      ("Caught the exception %s\n", ex.GetFullMessage().c_str());
#endif
   }

#ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("AttitudeWriterCK::FinishUpWritingCK() leaving\n");
#endif
}

void AttitudeWriterCK::WriteMetaData()
{
   WriteCKAttitudeMetaData();
}

//------------------------------------------------------------------------------
// void WriteDataComments(const std::string &comments, bool isErrorMsg,
//                    bool ignoreBlankComments = true, bool writeKeyword = true)
//------------------------------------------------------------------------------
/**
 * Writes comments to specific file.
 */
 //------------------------------------------------------------------------------
void AttitudeWriterCK::WriteDataComments(const std::string& comments, bool isErrorMsg,
   bool ignoreBlankComments, bool writeKeyword)
{
#ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage
   ("WriteDataComments() entered, comments='%s', ignoreBlankComments=%d\n",
      comments.c_str(), ignoreBlankComments);
#endif

   if (comments == "" && ignoreBlankComments)
   {
#ifdef DEBUG_EPHEMFILE_COMMENTS
      MessageInterface::ShowMessage("WriteDataComments() just leaving\n");
#endif
      return;
   }

   WriteCKComments(comments);

#ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage("WriteDataComments() wrote comment and leaving\n");
#endif
}


//------------------------------------------------------------------------------
// void WriteCKAttitudeDataSegment()
//------------------------------------------------------------------------------
/**
 * Writes orbit data segment to CK file and deletes data array
 */
 //------------------------------------------------------------------------------
void AttitudeWriterCK::WriteCKAttitudeDataSegment()
{
#ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
   ("=====> WriteCKAttitudeDataSegment() entered, a1MjdArray.size()=%d, "
      "stateArray.size()=%d\n", a1MjdArray.size(), stateArray.size());
#endif

#ifdef __USE_SPICE__
   if (a1MjdArray.size() > 0)
   {
      if (ckWriter == NULL)
         throw SubscriberException
         ("*** INTERNAL ERROR *** CK Writer is NULL in "
            "AttitudeWriterCK::WriteCKAttitudeDataSegment()\n");

      A1Mjd* start = a1MjdArray.front();
      A1Mjd* end = a1MjdArray.back();

#ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage
      ("   Writing start=%.15f, end=%.15f\n", start->GetReal(), end->GetReal());
      MessageInterface::ShowMessage("Here are epochs and states:\n");
      for (unsigned int ii = 0; ii < a1MjdArray.size(); ii++)
      {
         A1Mjd* t = a1MjdArray[ii];
         Real time = t->GetReal();
         Rvector6* st = stateArray[ii];
         MessageInterface::ShowMessage
         ("[%3d] %12.10f  %s  %s\n", ii, time, ToUtcGregorian(time, true).c_str(),
            (st->ToString()).c_str());
      }
#endif

#ifdef DEBUG_EPHEMFILE_TEXT
      WriteString("\n");
      for (unsigned int ii = 0; ii < a1MjdArray.size(); ii++)
         DebugWriteOrbit("In WriteCKAttitudeDataSegment:", a1MjdArray[ii], stateArray[ii]);
#endif

      std::vector<Rvector> quaternionArray;
      for (Integer i = 0; i < quatArray.size(); ++i)
         quaternionArray.push_back(*quatArray.at(i));

      ckWriteFailed = false;
      try
      {
         ckWriter->StoreSegment(*start, *end, quaternionArray, a1MjdArray);
         ClearOrbitData();
         numCKSegmentsWritten++;
         insufficientDataPoints = false;
      }
      catch (BaseException& e)
      {
         ClearOrbitData();
         ckWriteFailed = true;
         dstream.flush();
         dstream.close();
#ifdef DEBUG_EPHEMFILE_SPICE
         MessageInterface::ShowMessage("**** ERROR **** " + e.GetFullMessage());
#endif
         e.SetFatal(true);
         throw;
      }
   }
#endif

#ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
   ("=====> WriteCKAttitudeDataSegment() leaving\n");
#endif
}

//------------------------------------------------------------------------------
// void WriteCKAttitudeMetaData()
//------------------------------------------------------------------------------
/**
 * Writes meta data for CK file
 */
//------------------------------------------------------------------------------
void AttitudeWriterCK::WriteCKAttitudeMetaData()
{
   #ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage("=====> WriteCKOrbitMetaData() entered\n");
   #endif

   std::string objId = spacecraft->GetStringParameter("Id");
   std::string origin = outCoordSystem->GetOriginName();

   std::stringstream ss("");
   ss << std::endl;
   ss << "META_START" << std::endl;
   ss << "OBJECT_NAME = " << spacecraftName << std::endl;
   ss << "OBJECT_ID = " << objId << std::endl;
   ss << "CENTER_NAME = " << origin << std::endl;
   ss << "REF_FRAME = J2000" << std::endl; // Currently only J2000 coordinates available
   ss << "TIME_SYSTEM = " << epochFormat << std::endl;
   ss << "START_TIME = " << "@TODO_START" << std::endl;
   ss << "USEABLE_START_TIME = " << "@TODO_USTART" << std::endl;
   ss << "USEABLE_STOP_TIME = " << "@TODO_USTOP" << std::endl;
   ss << "STOP_TIME = " << "@TODO_STOP" << std::endl;
   ss << "INTERPOLATION = " << interpolatorName << std::endl;
   ss << "INTERPOLATION_DEGREE = " << interpolationOrder << std::endl;
   ss << "META_STOP" << std::endl << std::endl;

   #ifdef DEBUG_EPHEMFILE_TEXT
      WriteString(ss.str());
   #endif

   WriteCKComments(ss.str());
}

//------------------------------------------------------------------------------
// void WriteCKComments(const std::string &comments)
//------------------------------------------------------------------------------
void AttitudeWriterCK::WriteCKComments(const std::string& comments)
{
#ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> WriteCKComments() entered\n");
#endif

#ifdef DEBUG_EPHEMFILE_TEXT
   WriteString("\nCOMMENT  " + comments + "\n");
#endif

#ifdef __USE_SPICE__
   if (a1MjdArray.empty() && !writeCommentAfterData)
   {
      ckWriteFailed = true;
      MessageInterface::ShowMessage
      ("**** TODO **** AttitudeWriterCK::WriteCKComments() There must be at "
         "least one segment before this comment \"" + comments + "\" is written\n");
      return;
   }

   try
   {
      ckWriter->AddMetaData(comments);
   }
   catch (BaseException& e)
   {
      // Keep from setting a warning
      e.GetMessageType();

      ckWriteFailed = true;
#ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage("ckWriter->AddMetaData() failed\n");
      MessageInterface::ShowMessage(e.GetFullMessage());
#endif
      throw;
   }
#endif

#ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> WriteCKComments() leaving\n");
#endif
}


//------------------------------------------------------------------------------
// void FinalizeCKFile(bool done = true, writeMetaData = true)
//------------------------------------------------------------------------------
/**
 * Finalize the CK, write any remaining data, and prepare to close the file and
 * clear the remaining data after it has been written
 * 
 * @param done             If we are ready for file closure
 * @param writeMetaData    Whether to write the current meta data to the file
 */
 //------------------------------------------------------------------------------
void AttitudeWriterCK::FinalizeCKFile(bool done, bool writeMetaData)
{
#ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> FinalizeCKFile() entered\n");
   MessageInterface::ShowMessage("   and size of data is %d\n",
      (Integer)a1MjdArray.size());
#endif

#ifdef __USE_SPICE__
   try
   {
      if (!a1MjdArray.empty())
      {
         Integer mnSz = ckWriter->GetMinNumberOfAttitudes();
         Integer numPts = (Integer)a1MjdArray.size();
         // if we are generating CK files in the background and there
         // are not enough states for the interpolation, we DO NOT
         // want to try to write and trigger the SPICE error;
         // for user-specified CK files, we DO want to present
         // errors to the user.
         if (!generateInBackground || (numPts >= mnSz))
         {
#ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("   about to write CK orbit data segment\n");
#endif
            // Save last data to become first data of next segment - since we may start up
            // a new CK file after this one
            A1Mjd* a1mjd = new A1Mjd(*a1MjdArray.back());
            Rvector* rv = new Rvector(*quatArray.back());

            // Write a segment and delete data array pointers
            WriteCKAttitudeDataSegment();

            // Add saved data to arrays if we are not done yet
            if (!done)
            {
               a1MjdArray.push_back(a1mjd);
               quatArray.push_back(rv);
            }
            insufficientDataPoints = false;
#ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("   DONE writing CK orbit data segment\n");
#endif
         }
         // background CKs need to know if there was data unwritten
         // will have 1 point from the last segment in the beginning of this
         // set of data
         else if (generateInBackground && (numPts > 1))
         {
#ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("NOT WRITING CK data - only %d points available!!!\n",
               numPts);
#endif
            insufficientDataPoints = true; // data is available, but has not been written yet
         }
      }

#ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage("   about to call FinalizeKernel!!!\n");
#endif
      ckWriter->FinalizeKernel(done, writeMetaData);
      // so we recreate next time - for background CKs only
      if (!done) isEphemFileOpened = false;
   }
   catch (BaseException& e)
   {
      // Keep from setting a warning
      e.GetMessageType();

      ClearOrbitData();
      ckWriteFailed = true;
#ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage("ckWriter->FinalizeCKFile() failed\n");
      MessageInterface::ShowMessage(e.GetFullMessage());
#endif
      throw;
   }
#endif

#ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> FinalizeCKFile() leaving\n");
#endif
}

//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

