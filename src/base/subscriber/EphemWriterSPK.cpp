//$Id$
//------------------------------------------------------------------------------
//                                  EphemWriterSPK
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
 * SPK format.
 */
//------------------------------------------------------------------------------

#include "EphemWriterSPK.hpp"
#include "CelestialBody.hpp"
#include "SubscriberException.hpp"   // for exception
#include "RealUtilities.hpp"         // for IsEven()
#include "MessageInterface.hpp"
#include <sstream>                   // for <<, std::endl

#ifdef __USE_SPICE__
#include "SpiceOrbitKernelWriter.hpp"
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
// EphemWriterSPK(const std::string &name, const std::string &type = "EphemWriterSPK")
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EphemWriterSPK::EphemWriterSPK(const std::string &name, const std::string &type) :
   EphemerisWriter       (type, name),
   spkWriter             (NULL),
   spkWriteFailed        (false),
   numSPKSegmentsWritten (0)
{
   fileType = SPK_ORBIT;
   
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::EphemWriterSPK() <%p>'%s' entered\n", this, ephemName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// ~EphemWriterSPK()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EphemWriterSPK::~EphemWriterSPK()
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::~EphemWriterSPK() <%p>'%s' entered\n", this, ephemName.c_str());
   #endif
   
   #ifdef __USE_SPICE__
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("   spkWriter=<%p>, spkWriteFailed=%s\n", spkWriter, (spkWriteFailed? "true":"false"));
   #endif
   if (spkWriter != NULL)
   {
      if (!spkWriteFailed)
         FinalizeSpkFile();
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (spkWriter, "SPK writer", "EphemWriterSPK::~EphemWriterSPK()()",
          "deleting local SPK writer");
      #endif
      delete spkWriter;
   }
   #endif
   
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::~EphemWriterSPK() <%p>'%s' leaving\n", this, ephemName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// EphemWriterSPK(const EphemWriterSPK &ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EphemWriterSPK::EphemWriterSPK(const EphemWriterSPK &ef) :
   EphemerisWriter       (ef),
   spkWriter             (NULL),
   spkWriteFailed        (ef.spkWriteFailed),
   numSPKSegmentsWritten (ef.numSPKSegmentsWritten)
{
}


//------------------------------------------------------------------------------
// EphemWriterSPK& EphemWriterSPK::operator=(const EphemWriterSPK& ef)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
EphemWriterSPK& EphemWriterSPK::operator=(const EphemWriterSPK& ef)
{
   if (this == &ef)
      return *this;
   
   EphemerisWriter::operator=(ef);
   
   spkWriter            = NULL;
   spkWriteFailed       = ef.spkWriteFailed;
   numSPKSegmentsWritten = ef.numSPKSegmentsWritten;
   
   return *this;
}

//---------------------------------
// methods for this class
//---------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool EphemWriterSPK::Initialize()
{
   bool retval = EphemerisWriter::Initialize();
   
   // Set file format
   fileType = UNKNOWN_FILE_TYPE;
   if (ephemType == "SPK" && stateType == "Cartesian")
      fileType = SPK_ORBIT;
   else if (ephemType == "SPK" && stateType == "Quaternion")
      fileType = SPK_ATTITUDE;
   else
      return false;
   
   // Set maximum segment size
   maxSegmentSize = 1000;
   
   // // Determine output coordinate system, set to boolean to avoid string comparison.
   // // We don't need conversion for SPK_ORBIT. SpiceOrbitKernelWriter assumes it is in
   // // J2000Eq frame for now
   // if (dataCoordSystem->GetName() != outCoordSystemName)
   //    writeDataInDataCS = false;
   
   return retval;
}


//------------------------------------------------------------------------------
//  EphemerisWriter* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EphemWriterSPK.
 *
 * @return clone of the EphemWriterSPK.
 *
 */
//------------------------------------------------------------------------------
EphemerisWriter* EphemWriterSPK::Clone(void) const
{
   return (new EphemWriterSPK(*this));
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
void EphemWriterSPK::Copy(const EphemerisWriter* orig)
{
   operator=(*((EphemWriterSPK *)(orig)));
}


//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
// void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21], const Real accel[3])
//------------------------------------------------------------------------------
void EphemWriterSPK::BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21],
                                     const Real accel[3], const Real quat[4])
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
      A1Mjd *a1mjd  = new A1Mjd(*a1MjdArray.back());
      Rvector6 *rv6 = new Rvector6(*stateArray.back());
      
      // Write a segment and delete data array pointers
      WriteSpkOrbitDataSegment();
      
      // Add saved data to arrays
      a1MjdArray.push_back(a1mjd);
      stateArray.push_back(rv6);
   }
   
   // Add new data point
   A1Mjd *a1mjd = new A1Mjd(epochInDays);
   Rvector6 *rv6 = new Rvector6(state);
   a1MjdArray.push_back(a1mjd);
   stateArray.push_back(rv6);
   
   #ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
      ("BufferOrbitData() leaving, there is(are) %d data point(s)\n", a1MjdArray.size());
   #endif
} // BufferOrbitData()


//------------------------------------------------------------------------------
// void CreateEphemerisFile(bool useDefaultFileName, const std::string &stType,
//                          const std::string &outFormat, const std::string &covFormat)
//------------------------------------------------------------------------------
void EphemWriterSPK::CreateEphemerisFile(bool useDefaultFileName,
                                         const std::string &stType,
                                         const std::string &outFormat,
                                         const std::string &covFormat)
{
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::CreateEphemerisFile() <%p> entered, useDefaultFileName=%d, "
       "outFormat='%s'\n   fullPathFileName='%s'\n", this, useDefaultFileName,
       outFormat.c_str(), fullPathFileName.c_str());
   #endif
   
   EphemerisWriter::CreateEphemerisFile(useDefaultFileName, stType, outFormat, covFormat);
   
   CreateSpiceKernelWriter();
   isEphemFileOpened = true;
   
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::CreateEphemerisFile() <%p> leaving, writingNewSegment=%d\n   "
       "fullPathFileName='%s'\n", this, writingNewSegment, fullPathFileName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void CreateSpiceKernelWriter()
//------------------------------------------------------------------------------
void EphemWriterSPK::CreateSpiceKernelWriter()
{
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::CreateSpiceKernelWriter() entered, spkWriter=<%p>, "
       "isInitialized=%d, firstTimeWriting=%d\n   prevFileName='%s'\n   "
       "    fileName='%s', spacecraft=<%p>\n", spkWriter, isInitialized, firstTimeWriting,
       prevFileName.c_str(), fileName.c_str(), spacecraft);
   #endif
   
   if (spacecraft == NULL)
   {
      #ifdef DEBUG_EPHEMFILE_CREATE
      MessageInterface::ShowMessage
         ("EphemWriterSPK::CreateSpiceKernelWriter() just leaving, spacecraft is not set\n");
      #endif
      return;
   }
   
   //=======================================================
   #ifdef __USE_SPICE__
   //=======================================================
   // If spkWriter is not NULL, delete it first
   // Do we really want to delete here? This causes to only write
   // segments after toggle on. So all previous data are wiped out.
   // So check for file name change in the mission sequence
   // before creating new spkWriter (LOJ: 2015.10.14)
   if (spkWriter != NULL)
   {
      if (prevFileName != fileName)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (spkWriter, "spkWriter", "EphemWriterSPK::CreateSpiceKernelWriter()",
             "deleting local spkWriter");
         #endif
         delete spkWriter;
         spkWriter = NULL;
      }
   }
   
   // Create spkWrite if it is NULL (LOJ: 2015.10.14)
   if (spkWriter == NULL)
   {
      std::string name = spacecraft->GetName();
      std::string centerName = outCoordSystem->GetOriginName();
      Integer objNAIFId = spacecraft->GetIntegerParameter("NAIFId");
      Integer centerNAIFId = (outCoordSystem->GetOrigin())->GetIntegerParameter("NAIFId");
      
      #ifdef DEBUG_EPHEMFILE_CREATE
      MessageInterface::ShowMessage
         ("   Creating SpiceOrbitKernelWriter with name='%s', centerName='%s', "
          "objNAIFId=%d, centerNAIFId=%d\n   fileName='%s', interpolationOrder=%d\n",
          name.c_str(), centerName.c_str(), objNAIFId, centerNAIFId,
          fileName.c_str(), interpolationOrder);
      #endif
      
      try
      {
         spkWriter =
            new SpiceOrbitKernelWriter(name, centerName, objNAIFId, centerNAIFId,
                                       fullPathFileName, interpolationOrder, "J2000");
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (spkWriter, "spkWriter", "EphemWriterSPK::CreateSpiceKernelWriter()",
             "spkWriter = new SpiceOrbitKernelWriter()");
         #endif
      }
      catch (BaseException &e)
      {
         // Keep from setting a warning
         e.GetMessageType();
         
         #ifdef DEBUG_EPHEMFILE_CREATE
         MessageInterface::ShowMessage(
            "  Error creating SpiceOrbitKernelWriter: %s", (e.GetFullMessage()).c_str());
         #endif
         throw;
      }
   }
   
   
   //=======================================================
   #else
   //=======================================================
   MessageInterface::ShowMessage
      ("*** WARNING *** Use of SpiceOrbitKernelWriter is turned off\n");
   //=======================================================
   #endif
   //=======================================================
   
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::CreateSpiceKernelWriter() leaving, spkWriter=<%p>\n",
       spkWriter);
   #endif
}


//------------------------------------------------------------------------------
// void CloseEphemerisFile(bool done = true, writeMetaData = true)
//------------------------------------------------------------------------------
void EphemWriterSPK::CloseEphemerisFile(bool done, bool writeMetaData)
{
   // Close SPK file
   #ifdef __USE_SPICE__
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::CloseEphemWriterSPK() spkWriter=<%p>, spkWriteFailed=%s\n",
       spkWriter, (spkWriteFailed? "true":"false"));
   #endif
   if (spkWriter != NULL)
   {
      if (!spkWriteFailed)
         FinalizeSpkFile(done, writeMetaData);
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (spkWriter, "SPK writer", "EphemWriterSPK::~EphemWriterSPK()()",
          "deleting local SPK writer");
      #endif
      if (done)
      {
         delete spkWriter;
         spkWriter = NULL;
      }
   }
   #endif
}


//------------------------------------------------------------------------------
// void HandleOrbitData(bool isEndOfData)
//------------------------------------------------------------------------------
void EphemWriterSPK::HandleOrbitData()
{
   // Check user defined initial and final epoch
   bool processData = CheckInitialAndFinalEpoch();
   
   #if DBGLVL_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemWriterSPK::HandleOrbitData() checked initial and final epoch\n");
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
   HandleSpkOrbitData(processData, timeToWrite);
}


//------------------------------------------------------------------------------
// void HandleWriteOrbit()
//------------------------------------------------------------------------------
void EphemWriterSPK::HandleWriteOrbit()
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::HandleWriteOrbit() entered\n");
   #endif
   
   WriteOrbit(currEpochInSecs, currState, currCov, currAccel, currQuat);
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage("EphemWriterSPK::HandleWriteOrbit() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void HandleSpkOrbitData(bool writeData, bool timeToWrite)
//------------------------------------------------------------------------------
void EphemWriterSPK::HandleSpkOrbitData(bool writeData, bool timeToWrite)
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::HandleSpkOrbitData() entered, writeData=%d, timeToWrite=%d, "
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
         DebugWriteOrbit("In HandleSpkOrbitData:", currEpochInDays, currState, true, true);
         #endif
      }
      
      // Set flags (GMT-3745 SPK writing fix)
      if (firstTimeWriting)
         firstTimeWriting = false;
      
      // if (writingNewSegment)
      //    writingNewSegment = false;
   }
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("EphemWriterSPK::HandleSpkOrbitData() leaving, firstTimeWriting=%d\n",
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
void EphemWriterSPK::StartNewSegment(const std::string &comments,
                                     bool saveEpochInfo, bool writeAfterData,
                                     bool ignoreBlankComments)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterSPK::StartNewSegment() entered\n   comments='%s'\n   "
       "saveEpochInfo=%d, writeAfterData=%d, ignoreBlankComments=%d, canFinalize=%d, firstTimeWriting=%d\n",
       comments.c_str(), saveEpochInfo, writeAfterData, ignoreBlankComments, canFinalize, firstTimeWriting);
   #endif
   
   // If no first data has written out yet, just return
   if (firstTimeWriting)
   {
      #ifdef DEBUG_EPHEMFILE_RESTART
      MessageInterface::ShowMessage
         ("EphemWriterSPK::StartNewSegment() returning, no first data written out yet\n");
      #endif
      return;
   }
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("EphemWriterSPK::StartNewSegment() Calling FinishUpWriting(), canFinalize=%d\n",
       canFinalize);
   #endif
   
   // Finish up writing data
   FinishUpWriting();
   
   // Set comments
   writeCommentAfterData = writeAfterData;
   currComments = comments;
   
   if (spkWriter != NULL)
   {
      Integer mnSz   = spkWriter->GetMinNumberOfStates();
      Integer numPts = (Integer) a1MjdArray.size();
      if (!generateInBackground || (numPts >= mnSz))
      {
         if (!writeAfterData)
            WriteDataComments(comments, false, ignoreBlankComments);
         
         WriteSpkOrbitDataSegment();
         
         if (writeAfterData)
            WriteDataComments(comments, false, ignoreBlankComments);

         insufficientDataPoints = false; // there was enough data
         currComments = "";
      }
      else if (generateInBackground && (numPts > 1))
      {
         #ifdef DEBUG_EPHEMFILE_SPICE
         MessageInterface::ShowMessage("NOT WRITING SPK data - only %d points available!!!\n",
               numPts);
         #endif
         insufficientDataPoints = true; // data is available, but has not been written yet
      }
   }
   
   // Initialize data
   InitializeData(saveEpochInfo);
   
   ++numSegments;

   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemWriterSPK::StartNewSegment() leaving\n");
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
void EphemWriterSPK::FinishUpWriting()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterSPK::FinishUpWriting() '%s' entered, canFinalize=%d, isFinalized=%d, "
       "firstTimeWriting=%d, isEndOfRun=%d\n",
       ephemName.c_str(), canFinalize, isFinalized, firstTimeWriting, isEndOfRun);
   DebugWriteTime("    lastEpochWrote = ", lastEpochWrote);
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   MessageInterface::ShowMessage
      ("   There is(are) %d data point(s) in the buffer, spkWriter=<%p>\n", a1MjdArray.size(),
       spkWriter);
   #endif
   
   if (!isFinalized)
   {
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage
         ("   It is not finalized yet, so trying to write the remainder of data\n");
      #endif
      
      FinishUpWritingSPK();
      
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
      ("EphemWriterSPK::FinishUpWriting() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void FinishUpWritingSPK()
//------------------------------------------------------------------------------
void EphemWriterSPK::FinishUpWritingSPK()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemWriterSPK::FinishUpWritingSPK) entered, canFinalize=%d\n", canFinalize);
   #endif
   try 
   {
      if (spkWriter != NULL)
      {
         Integer mnSz   = spkWriter->GetMinNumberOfStates();
         Integer numPts = (Integer) a1MjdArray.size();
         #ifdef DEBUG_EPHEMFILE_SPICE
         MessageInterface::ShowMessage("FinishUpWritingSPK::mnSz = %d, numPts = %d\n",
                                       mnSz, numPts);
         MessageInterface::ShowMessage("FinishUpWritingSPK::generateInBackground = %s\n",
                                       (generateInBackground? "true" : "false"));
         #endif
         // if we are generating SPK files in the background and there
         // are not enough states for the interpolation, we DO NOT
         // want to try to write and trigger the SPICE error;
         // for user-specified SPK files, we DO want to present
         // errors to the user.
         if (!generateInBackground || (numPts >= mnSz))
         {
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("FinishUpWritingSPK::about to write SPK orbit data segment\n");
            #endif

            // Write a segment and delete data array pointers
            WriteSpkOrbitDataSegment();

            insufficientDataPoints = false;
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("   DONE writing SPK orbit data segment\n");
            #endif
         }
         // background SPKs need to know if there was data unwritten
         // will have 1 point from the last segment in the beginning of this
         // set of data
         else if (generateInBackground && (numPts > 1))
         {
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("NOT WRITING SPK data - only %d points available!!!\n",
                  numPts);
            #endif
            insufficientDataPoints = true; // data is available, but has not been written yet
         }
      }
      else
      {
         #ifdef __USE_SPICE__
         if (a1MjdArray.size() > 0)
         {
            throw SubscriberException
               ("*** INTERNAL ERROR *** SPK Writer is NULL in "
                "EphemWriterSPK::FinishUpWritingSPK()\n");
         }
         #endif
      }
   }
   catch (BaseException &ex)
   {
      // Catch and ignore exceptions thrown from util, since we manage them later
      #ifdef DEBUG_EPHEMFILE_WRITE
      MessageInterface::ShowMessage
         ("Caught the exception %s\n", ex.GetFullMessage().c_str());
      #endif
   }
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemWriterSPK::FinishUpWritingSPK() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteHeader()
//------------------------------------------------------------------------------
void EphemWriterSPK::WriteHeader()
{
   WriteSpkHeader();
}


//------------------------------------------------------------------------------
// void WriteMetaData()
//------------------------------------------------------------------------------
void EphemWriterSPK::WriteMetaData()
{
   WriteSpkOrbitMetaData();
}


//------------------------------------------------------------------------------
// void WriteDataComments(const std::string &comments, bool isErrorMsg,
//                    bool ignoreBlankComments = true, bool writeKeyword = true)
//------------------------------------------------------------------------------
/**
 * Writes comments to specific file.
 */
//------------------------------------------------------------------------------
void EphemWriterSPK::WriteDataComments(const std::string &comments, bool isErrorMsg,
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
   
   WriteSpkComments(comments);
   
   #ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage("WriteDataComments() wrote comment and leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteSpkHeader()
//------------------------------------------------------------------------------
void EphemWriterSPK::WriteSpkHeader()
{
   #ifdef DEBUG_EPHEMFILE_TEXT
   std::string creationTime = GmatTimeUtil::FormatCurrentTime(2);
   std::stringstream ss("");
   
   ss << "SPK ORBIT DATA" << std::endl;
   ss << "CREATION_DATE  = " << creationTime << std::endl;
   ss << "ORIGINATOR     = GMAT USER" << std::endl;
   
   WriteString(ss.str());
   #endif
}


//------------------------------------------------------------------------------
// void WriteSpkOrbitDataSegment()
//------------------------------------------------------------------------------
/**
 * Writes orbit data segment to SPK file and deletes data array
 */
//------------------------------------------------------------------------------
void EphemWriterSPK::WriteSpkOrbitDataSegment()
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("=====> WriteSpkOrbitDataSegment() entered, a1MjdArray.size()=%d, "
       "stateArray.size()=%d\n", a1MjdArray.size(), stateArray.size());
   #endif
   
   #ifdef __USE_SPICE__
   if (a1MjdArray.size() > 0)
   {
      if (spkWriter == NULL)
         throw SubscriberException
            ("*** INTERNAL ERROR *** SPK Writer is NULL in "
             "EphemWriterSPK::WriteSpkOrbitDataSegment()\n");

      A1Mjd *start = a1MjdArray.front();
      A1Mjd *end   = a1MjdArray.back();

      #ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage
         ("   Writing start=%.15f, end=%.15f\n", start->GetReal(), end->GetReal());
      MessageInterface::ShowMessage("Here are epochs and states:\n");
      for (unsigned int ii = 0; ii < a1MjdArray.size(); ii++)
      {
         A1Mjd *t = a1MjdArray[ii];
         Real time = t->GetReal();
         Rvector6 *st = stateArray[ii];
         MessageInterface::ShowMessage
            ("[%3d] %12.10f  %s  %s\n", ii, time, ToUtcGregorian(time, true).c_str(),
             (st->ToString()).c_str());
      }
      #endif

      #ifdef DEBUG_EPHEMFILE_TEXT
      WriteString("\n");
      for (unsigned int ii = 0; ii < a1MjdArray.size(); ii++)
         DebugWriteOrbit("In WriteSpkOrbitDataSegment:", a1MjdArray[ii], stateArray[ii]);
      #endif

      spkWriteFailed = false;
      try
      {
         spkWriter->WriteSegment(*start, *end, stateArray, a1MjdArray);
         ClearOrbitData();
         numSPKSegmentsWritten++;
         insufficientDataPoints = false;
      }
      catch (BaseException &e)
      {
         ClearOrbitData();
         spkWriteFailed = true;
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
      ("=====> WriteSpkOrbitDataSegment() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteSpkOrbitMetaData()
//------------------------------------------------------------------------------
void EphemWriterSPK::WriteSpkOrbitMetaData()
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> WriteSpkOrbitMetaData() entered\n");
   #endif
   
   std::string objId  = spacecraft->GetStringParameter("Id");
   //std::string origin = spacecraft->GetOriginName();
   std::string origin = outCoordSystem->GetOriginName();
   std::string csType = "UNKNOWN";
   GmatBase *cs = (GmatBase*)(spacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
   if (cs)
      csType = (cs->GetRefObject(Gmat::AXIS_SYSTEM, ""))->GetTypeName();
   
   std::stringstream ss("");
   ss << std::endl;
   ss << "META_START" << std::endl;
   ss << "OBJECT_NAME = " << spacecraftName << std::endl;
   ss << "OBJECT_ID = " << objId << std::endl;
   ss << "CENTER_NAME = " << origin << std::endl;
   ss << "REF_FRAME = " << csType << std::endl;
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
   
   WriteSpkComments(ss.str());
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> WriteSpkOrbitMetaData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteSpkComments(const std::string &comments)
//------------------------------------------------------------------------------
void EphemWriterSPK::WriteSpkComments(const std::string &comments)
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> WriteSpkComments() entered\n");
   #endif
   
   #ifdef DEBUG_EPHEMFILE_TEXT
   WriteString("\nCOMMENT  " + comments + "\n");
   #endif
   
   #ifdef __USE_SPICE__
   if (a1MjdArray.empty() && !writeCommentAfterData)
   {
      spkWriteFailed = true;
      MessageInterface::ShowMessage
         ("**** TODO **** EphemWriterSPK::WriteSpkComments() There must be at "
          "least one segment before this comment \"" + comments + "\" is written\n");
      return;
   }
   
   try
   {
      spkWriter->AddMetaData(comments);
   }
   catch (BaseException &e)
   {
      // Keep from setting a warning
      e.GetMessageType();

      spkWriteFailed = true;
      #ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage("spkWriter->AddMetaData() failed\n");
      MessageInterface::ShowMessage(e.GetFullMessage());
      #endif
      throw;
   }
   #endif
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> WriteSpkComments() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void FinalizeSpkFile(bool done = true, writeMetaData = true)
//------------------------------------------------------------------------------
void EphemWriterSPK::FinalizeSpkFile(bool done, bool writeMetaData)
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> FinalizeSpkFile() entered\n");
   MessageInterface::ShowMessage("   and size of data is %d\n",
         (Integer) a1MjdArray.size());
   #endif
   
   #ifdef __USE_SPICE__
   try
   {
      if (!a1MjdArray.empty())
      {
         Integer mnSz   = spkWriter->GetMinNumberOfStates();
         Integer numPts = (Integer) a1MjdArray.size();
         // if we are generating SPK files in the background and there
         // are not enough states for the interpolation, we DO NOT
         // want to try to write and trigger the SPICE error;
         // for user-specified SPK files, we DO want to present
         // errors to the user.
         if (!generateInBackground || (numPts >= mnSz))
         {
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("   about to write SPK orbit data segment\n");
            #endif
            // Save last data to become first data of next segment - since we may start up
            // a new SPK file after this one
            A1Mjd *a1mjd  = new A1Mjd(*a1MjdArray.back());
            Rvector6 *rv6 = new Rvector6(*stateArray.back());

            // Write a segment and delete data array pointers
            WriteSpkOrbitDataSegment();

            // Add saved data to arrays if we are not done yet
            if (!done)
            {
               a1MjdArray.push_back(a1mjd);
               stateArray.push_back(rv6);
            }
            insufficientDataPoints = false;
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("   DONE writing SPK orbit data segment\n");
            #endif
         }
         // background SPKs need to know if there was data unwritten
         // will have 1 point from the last segment in the beginning of this
         // set of data
         else if (generateInBackground && (numPts > 1))
         {
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("NOT WRITING SPK data - only %d points available!!!\n",
                  numPts);
            #endif
            insufficientDataPoints = true; // data is available, but has not been written yet
         }
      }

      #ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage("   about to call FinalizeKernel!!!\n");
      #endif
      spkWriter->FinalizeKernel(done, writeMetaData);
      // so we recreate next time - for background SPKs only
      if (!done) isEphemFileOpened = false;
   }
   catch (BaseException &e)
   {
      // Keep from setting a warning
      e.GetMessageType();

      ClearOrbitData();
      spkWriteFailed = true;
      #ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage("spkWriter->FinalizeSpkFile() failed\n");
      MessageInterface::ShowMessage(e.GetFullMessage());
      #endif
      throw;
   }
   #endif
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> FinalizeSpkFile() leaving\n");
   #endif
}

//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

