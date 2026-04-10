//$Id$
//------------------------------------------------------------------------------
//                                  EphemerisWriter
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
// Created: 2015.11.17
//
/**
 * Base class for writing spacecraft orbit states to an ephemeris file.
 */
//------------------------------------------------------------------------------

#include "EphemerisWriter.hpp"
#include "CelestialBody.hpp"
#include "Publisher.hpp"             // for Instance()
//#include "FileManager.hpp"           // for GetPathname()
#include "SubscriberException.hpp"   // for exception
#include "StringUtil.hpp"            // for ToString()
#include "FileUtil.hpp"              // for ParseFileExtension()
#include "TimeSystemConverter.hpp"   // for ValidateTimeFormat()
#include "RealUtilities.hpp"         // for IsEven()
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "AttitudeConversionUtility.hpp"
#include <sstream>                   // for <<, std::endl


//#define DEBUG_EPHEMFILE_INSTANCE
//#define DEBUG_EPHEMFILE_SET
//#define DEBUG_EPHEMFILE_INIT
//#define DEBUG_EPHEMFILE_CREATE
//#define DEBUG_EPHEMFILE_OPEN
//#define DEBUG_EPHEMFILE_CLEAR
//#define DEBUG_EPHEMFILE_DATA
//#define DEBUG_EPHEMFILE_WRITE
//#define DEBUG_EPHEMFILE_FINISH
//#define DEBUG_EPHEMFILE_RESTART
//#define DEBUG_EPHEMFILE_RUNFLAG
//#define DEBUG_EPHEMFILE_CONVERT_STATE
//#define DEBUG_EPHEMFILE_BACKGROUND

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
// EphemerisWriter(const std::string &name, const std::string &type = "EphemerisWriter")
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EphemerisWriter::EphemerisWriter(const std::string &name, const std::string &type) :
   ephemName            (name),
   ephemType            (type),
   
   ephemVersion         (""),
   writeAccelerationOption (false),
   writeCovarianceOption   (false),

   fileType             (UNKNOWN_FILE_TYPE),
   maxSegmentSize       (1000),
   spacecraft           (NULL),
   dataCoordSystem      (NULL),
   outCoordSystem       (NULL),
   fullPathFileName     (""),
   spacecraftName       (""),
   spacecraftId         (""),
   prevFileName         (""),
   fileName             (""),
   epochFormat          ("UTCGregorian"),
   initialEpochStr      ("InitialSpacecraftEpoch"),
   finalEpochStr        ("FinalSpacecraftEpoch"),
   stepSize             ("IntegratorSteps"),
   interpolatorName     ("Lagrange"),
   stateType            ("Cartesian"),
   outCoordSystemName   ("EarthMJ2000Eq"),
   outputFormat         ("LittleEndian"),
   covarianceFormat     ("None"),
   currComments         (""),
   interpolationOrder   (7),
   toggleStatus         (0),
   propIndicator        (0),
   numSegments          (1),
   prevPropDirection    (0.0),
   currPropDirection    (0.0),
   stepSizeInA1Mjd      (-999.999),
   stepSizeInSecs       (-999.999),
   initialEpochA1Mjd    (-999.999),
   finalEpochA1Mjd      (-999.999),
   blockBeginA1Mjd      (-999.999),
   nextOutEpochInSecs   (-999.999),
   nextReqEpochInSecs   (-999.999),
   currEpochInDays      (-999.999),
   currEpochInSecs      (-999.999),
   prevEpochInSecs      (-999.999),
   prevProcTime         (-999.999),
   lastEpochWrote       (-999.999),
   attEpoch             (-999.999),
   maneuverEpochInDays  (-999.999),
   eventEpochInSecs     (-999.999),
   canFinalize          (false),
   isEndOfRun           (false),
   isInitialized        (false),
   isFinalized          (false),
   isEphemGlobal        (false),
   isEphemLocal         (false),
   usingDefaultFileName (true),
   generateInBackground (false),
   isEphemFileOpened    (false),
   firstTimeWriting     (true),
   writingNewSegment    (true),
   useFixedStepSize     (false),
   interpolateInitialState(false),
   interpolateFinalState(false),
   createInterpolator   (false),
   writeOrbit           (false),
   writeAttitude        (false),
   finalEpochReached    (false),
   finalEpochProcessed  (false),
   writeDataInDataCS    (true),
   writeCommentAfterData (true),
   insufficientDataPoints (false)
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemerisWriter() default constructor <%p> enterd, name='%s', type='%s', "
       "writingNewSegment=%d\n", this, name.c_str(), type.c_str(), writingNewSegment);
   #endif
}


//------------------------------------------------------------------------------
// ~EphemerisWriter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EphemerisWriter::~EphemerisWriter()
{
   #ifdef DEBUG_EPHEMFILE
   MessageInterface::ShowMessage
      ("EphemerisWriter::~EphemerisWriter() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   // Close text ephemeris for CCSDS or debug
   dstream.flush();
   dstream.close();
   
   #ifdef DEBUG_EPHEMFILE
   MessageInterface::ShowMessage
      ("EphemerisWriter::~EphemerisWriter() <%p>'%s' leaving\n", this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// EphemerisWriter(const EphemerisWriter &ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EphemerisWriter::EphemerisWriter(const EphemerisWriter &ef) :
   ephemName            (ef.ephemName),
   ephemType            (ef.ephemType),

   ephemVersion         (ef.ephemVersion),
   writeAccelerationOption (ef.writeAccelerationOption),
   writeCovarianceOption   (ef.writeCovarianceOption),

   fileType             (ef.fileType),
   maxSegmentSize       (ef.maxSegmentSize),
   spacecraft           (ef.spacecraft),
   outCoordSystem       (ef.outCoordSystem),
   dataCoordSystem      (ef.outCoordSystem),
   fullPathFileName     (ef.fullPathFileName),
   spacecraftName       (ef.spacecraftName),
   spacecraftId         (ef.spacecraftId),
   prevFileName         (ef.prevFileName),
   fileName             (ef.fileName),
   epochFormat          (ef.epochFormat),
   initialEpochStr      (ef.initialEpochStr),
   finalEpochStr        (ef.finalEpochStr),
   stepSize             (ef.stepSize),
   interpolatorName     (ef.interpolatorName),
   stateType            (ef.stateType),
   outCoordSystemName   (ef.outCoordSystemName),
   outputFormat         (ef.outputFormat),
   covarianceFormat     (ef.covarianceFormat),
   currComments         (ef.currComments),
   interpolationOrder   (ef.interpolationOrder),
   toggleStatus         (ef.toggleStatus),
   propIndicator        (ef.propIndicator),
   numSegments          (ef.numSegments),
   prevPropDirection    (ef.prevPropDirection),
   currPropDirection    (ef.currPropDirection),
   stepSizeInA1Mjd      (ef.stepSizeInA1Mjd),
   stepSizeInSecs       (ef.stepSizeInSecs),
   initialEpochA1Mjd    (ef.initialEpochA1Mjd),
   finalEpochA1Mjd      (ef.finalEpochA1Mjd),
   blockBeginA1Mjd      (ef.blockBeginA1Mjd),
   nextOutEpochInSecs   (ef.nextOutEpochInSecs),
   nextReqEpochInSecs   (ef.nextReqEpochInSecs),
   currEpochInDays      (ef.currEpochInDays),
   currEpochInSecs      (ef.currEpochInSecs),
   prevEpochInSecs      (ef.prevEpochInSecs),
   prevProcTime         (ef.prevProcTime),
   lastEpochWrote       (ef.lastEpochWrote),
   attEpoch             (ef.attEpoch),
   maneuverEpochInDays  (ef.maneuverEpochInDays),
   eventEpochInSecs     (ef.eventEpochInSecs),
   canFinalize          (ef.canFinalize),
   isEndOfRun           (ef.isEndOfRun),
   isInitialized        (ef.isInitialized),
   isFinalized          (ef.isFinalized),
   isEphemGlobal        (ef.isEphemGlobal),
   isEphemLocal         (ef.isEphemLocal),
   usingDefaultFileName (ef.usingDefaultFileName),
   generateInBackground (ef.generateInBackground),
   isEphemFileOpened    (ef.isEphemFileOpened),
   firstTimeWriting     (ef.firstTimeWriting),
   writingNewSegment    (ef.writingNewSegment),
   useFixedStepSize     (ef.useFixedStepSize),
   interpolateInitialState(ef.interpolateInitialState),
   interpolateFinalState(ef.interpolateFinalState),
   createInterpolator   (ef.createInterpolator),
   writeOrbit           (ef.writeOrbit),
   writeAttitude        (ef.writeAttitude),
   finalEpochReached    (ef.finalEpochReached),
   finalEpochProcessed  (ef.finalEpochProcessed),
   writeDataInDataCS    (ef.writeDataInDataCS),
   writeCommentAfterData (ef.writeCommentAfterData),
   insufficientDataPoints (ef.insufficientDataPoints)
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemerisWriter() copy constructor <%p> enterd, ephemName='%s', ephemType='%s', "
       "writingNewSegment=%d\n", this, ephemName.c_str(), ephemType.c_str(), writingNewSegment);
   coordConverter = ef.coordConverter;
   #endif
}


//------------------------------------------------------------------------------
// EphemerisWriter& EphemerisWriter::operator=(const EphemerisWriter& ef)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
EphemerisWriter& EphemerisWriter::operator=(const EphemerisWriter& ef)
{
   if (this == &ef)
      return *this;
   
   ephemName            = ef.ephemName;
   ephemType            = ef.ephemType;
   
   ephemVersion         = ef.ephemVersion;
   writeAccelerationOption = ef.writeAccelerationOption;
   writeCovarianceOption   = ef.writeCovarianceOption;

   fileType             = ef.fileType;
   maxSegmentSize       = ef.maxSegmentSize;
   spacecraft           = ef.spacecraft;
   outCoordSystem       = ef.outCoordSystem;
   dataCoordSystem      = ef.dataCoordSystem;
   fullPathFileName     = ef.fullPathFileName;
   spacecraftName       = ef.spacecraftName;
   spacecraftId         = ef.spacecraftId;
   prevFileName         = ef.prevFileName;
   fileName             = ef.fileName;
   epochFormat          = ef.epochFormat;
   initialEpochStr      = ef.initialEpochStr;
   finalEpochStr        = ef.finalEpochStr;
   stepSize             = ef.stepSize;
   interpolatorName     = ef.interpolatorName;
   stateType            = ef.stateType;
   outCoordSystemName   = ef.outCoordSystemName;
   outputFormat         = ef.outputFormat;
   covarianceFormat     = ef.covarianceFormat;
   currComments         = ef.currComments;
   interpolationOrder   = ef.interpolationOrder;
   toggleStatus         = ef.toggleStatus;
   propIndicator        = ef.propIndicator;
   numSegments          = ef.numSegments;
   prevPropDirection    = ef.prevPropDirection;
   currPropDirection    = ef.currPropDirection;
   stepSizeInA1Mjd      = ef.stepSizeInA1Mjd;
   stepSizeInSecs       = ef.stepSizeInSecs;
   initialEpochA1Mjd    = ef.initialEpochA1Mjd;
   finalEpochA1Mjd      = ef.finalEpochA1Mjd;
   blockBeginA1Mjd      = ef.blockBeginA1Mjd;
   nextOutEpochInSecs   = ef.nextOutEpochInSecs;
   nextReqEpochInSecs   = ef.nextReqEpochInSecs;
   currEpochInDays      = ef.currEpochInDays;
   currEpochInSecs      = ef.currEpochInSecs;
   prevEpochInSecs      = ef.prevEpochInSecs;
   prevProcTime         = ef.prevProcTime;
   lastEpochWrote       = ef.lastEpochWrote;
   attEpoch             = ef.attEpoch;
   maneuverEpochInDays  = ef.maneuverEpochInDays;
   eventEpochInSecs     = ef.eventEpochInSecs;
   canFinalize          = ef.canFinalize;
   isEndOfRun           = ef.isEndOfRun;
   isInitialized        = ef.isInitialized;
   isFinalized          = ef.isFinalized;
   isEphemGlobal        = ef.isEphemGlobal;
   isEphemLocal         = ef.isEphemLocal;
   usingDefaultFileName = ef.usingDefaultFileName;
   generateInBackground = ef.generateInBackground;
   isEphemFileOpened    = ef.isEphemFileOpened;
   firstTimeWriting     = ef.firstTimeWriting;
   writingNewSegment    = ef.writingNewSegment;
   useFixedStepSize     = ef.useFixedStepSize;
   interpolateInitialState= ef.interpolateInitialState;
   interpolateFinalState= ef.interpolateFinalState;
   createInterpolator   = ef.createInterpolator;
   writeOrbit           = ef.writeOrbit;
   writeAttitude        = ef.writeAttitude;
   finalEpochReached    = ef.finalEpochReached;
   finalEpochProcessed  = ef.finalEpochProcessed;
   writeDataInDataCS    = ef.writeDataInDataCS;
   writeCommentAfterData = ef.writeCommentAfterData;
   coordConverter       = ef.coordConverter;
   insufficientDataPoints = ef.insufficientDataPoints;
   
   return *this;
}

//---------------------------------
// Methods for this class
//---------------------------------

//------------------------------------------------------------------------------
// void SetFileName(const std::string &currFileName, const std::string &currFullName,
//                  const std::string &prevFullName)
//------------------------------------------------------------------------------
void EphemerisWriter::SetFileName(const std::string &currFileName,
                                  const std::string &currFullName,
                                  const std::string &prevFullName)
{
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisWriter::SetFileName() <%p> entered\n   currFileName='%s'\n   "
       "currFullName='%s'\n   prevFullName='%s'\n   writingNewSegment=%d\n", this,
       currFileName.c_str(), currFullName.c_str(), prevFullName.c_str(),
       writingNewSegment);
   #endif
   fileName = currFileName;
   fullPathFileName = currFullName;
   prevFileName = prevFullName;
}



//------------------------------------------------------------------------------
// void SetEphemerisFormatVersion(const std::string &formatVersion)
//------------------------------------------------------------------------------
/**
* This function is used to set ephemeris format (type) version
* 
* @param formatVersion      format version of the ephemeris file
*/
void EphemerisWriter::SetEphemerisFormatVersion(const std::string &formatVersion)
{
   ephemVersion = formatVersion;
}


//------------------------------------------------------------------------------
// void SetSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
void EphemerisWriter::SetSpacecraft(Spacecraft *sc)
{
   spacecraft = sc;
   spacecraftName = sc->GetName();
   spacecraftId = sc->GetStringParameter("Id");
}

//------------------------------------------------------------------------------
// void SetDataCoordSystem(CoordinateSystem *dataCS)
//------------------------------------------------------------------------------
void EphemerisWriter::SetDataCoordSystem(CoordinateSystem *dataCS)
{
   dataCoordSystem = dataCS;
}

//------------------------------------------------------------------------------
// void SetOutCoordSystem(CoordinateSystem *outCS)
//------------------------------------------------------------------------------
void EphemerisWriter::SetOutCoordSystem(CoordinateSystem *outCS)
{
   outCoordSystem = outCS;
}

//------------------------------------------------------------------------------
// void SetInitialData(bool useFixedStep)
//------------------------------------------------------------------------------
void EphemerisWriter::SetInitialData(const std::string &iniEpoch,
                                     const std::string &finEpoch,
                                     const std::string &stpSize,
                                     Real  stepInSecs, bool useFixedStep,
                                     const std::string &interpName,
                                     Integer interpOrder)
{
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisWriter::SetInitialData() <%p>'%s' enterd, ephemType='%s'\n",
       this, ephemName.c_str(), ephemType.c_str());
   #endif
   
   initialEpochStr = iniEpoch;
   finalEpochStr = finEpoch;
   stepSize = stpSize;
   stepSizeInSecs = stepInSecs;
   useFixedStepSize = useFixedStep;
   interpolatorName = interpName;
   interpolationOrder = interpOrder;
   
   if (stepSizeInSecs != -999.999)
      stepSizeInA1Mjd = stepSizeInSecs / GmatTimeConstants::SECS_PER_DAY;
   
   if (useFixedStepSize)
      createInterpolator = true;
   else
      createInterpolator = false;
   
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("   initialEpochStr='%s', finalEpochStr='%s',\n   stepSizeInSecs=%.9f, "
       "stepSizeInA1Mjd=%.9f, useFixedStepSize=%d\n   interpolatorName='%s', "
       "interpolationOrder=%d\n", initialEpochStr.c_str(), finalEpochStr.c_str(),
       stepSizeInSecs, stepSizeInA1Mjd, useFixedStepSize,
       interpolatorName.c_str(), interpolationOrder);
   MessageInterface::ShowMessage
      ("EphemerisWriter::SetInitialData() <%p>'%s' leaving, ephemType='%s'\n",
       this, ephemName.c_str(), ephemType.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void SetInitialTime(Real iniEpochA1Mjd, Real finEpochA1Mjd)
//------------------------------------------------------------------------------
void EphemerisWriter::SetInitialTime(Real iniEpochA1Mjd, Real finEpochA1Mjd)
{
   initialEpochA1Mjd = iniEpochA1Mjd;
   finalEpochA1Mjd = finEpochA1Mjd;
}

//------------------------------------------------------------------------------
// void SetIsEphemGlobal(bool isGlobal)
//------------------------------------------------------------------------------
void EphemerisWriter::SetIsEphemGlobal(bool isGlobal)
{
   isEphemGlobal = isGlobal;
}

//------------------------------------------------------------------------------
// void SetIsEphemLocal(bool isLocal)
//------------------------------------------------------------------------------
void EphemerisWriter::SetIsEphemLocal(bool isLocal)
{
   isEphemLocal = isLocal;
}

//------------------------------------------------------------------------------
// void SetBackgroundGeneration(bool inBackground)
//------------------------------------------------------------------------------
void EphemerisWriter::SetBackgroundGeneration(bool inBackground)
{
#ifdef DEBUG_EPHEMFILE_BACKGROUND
   MessageInterface::ShowMessage("Setting generateInBackground to %s\n",
                                 (inBackground? "true" : "false"));
#endif
   generateInBackground = inBackground;
}

//------------------------------------------------------------------------------
// void SetRunFlags(bool finalize, bool endOfRun, bool finalized)
//------------------------------------------------------------------------------
void EphemerisWriter::SetRunFlags(bool finalize, bool endOfRun, bool finalized)
{
   #ifdef DEBUG_EPHEMFILE_RUNFLAG
   MessageInterface::ShowMessage
      ("EphemerisWriter::SetRunFlags() <%p> entered, finalize=%d, endOfRun=%d, "
       "finalized=%d\n", this, finalize, endOfRun, finalized);
   #endif
   
   canFinalize = finalize;
   isEndOfRun = endOfRun;
   isFinalized = finalized;
   
   #ifdef DEBUG_EPHEMFILE_FLAG
   MessageInterface::ShowMessage
      ("EphemerisWriter::SetRunFlags() <%p> leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void SetOrbitData(Real epochInDays, Real state[6], Real cov[21], Real accel[3])
//------------------------------------------------------------------------------
void EphemerisWriter::SetOrbitData(Real epochInDays, Real state[6], Real cov[21], Real accel[3], Real quat[4])
{
   #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage("epoch = %.12lf state = [%.15lf %.15lf %.15lf %.15lf %.15lf %.15lf]\n",
            epochInDays, state[0], state[1], state[2], state[3], state[4],
            state[5]);
      MessageInterface::ShowMessage("accel = [%.15lf %.15lf %.15lf]\n", accel[0], accel[1], accel[3]);
   #endif

   currEpochInDays = epochInDays;
   for (int i = 0; i < 6; i++)
      currState[i] = state[i];
   for (int i = 0; i < 21; i++)
      currCov[i] = cov[i];

   for (int i = 0; i < 3; i++)
      currAccel[i] = accel[i];
   for (int i = 0; i < 4; i++)
      currQuat[i] = quat[i];

}


//------------------------------------------------------------------------------
// void  SetEpochAndDirection(Real prvEpochInSecs, Real curEpochInSecs,
//                            Real prvPropDir, Real curPropDir)
//------------------------------------------------------------------------------
void EphemerisWriter::SetEpochAndDirection(Real prvEpochInSecs, Real curEpochInSecs,
                                           Real prvPropDir, Real curPropDir)
{
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisWriter::SetEpochAndDirection() <%p>'%s' entered, prvPropDir=%f, "
       "curPropDir=%f, a1MjdArray.size()=%d\n", this, ephemName.c_str(), prvPropDir,
       curPropDir, a1MjdArray.size());
   #endif
   
   prevEpochInSecs = prvEpochInSecs;
   currEpochInSecs = curEpochInSecs;
   prevPropDirection = prvPropDir;
   currPropDirection = curPropDir;
   
   #ifdef DEBUG_EPHEMFILE_DATA
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   DebugWriteTime("   prevEpochInSecs = ", prevEpochInSecs);
   MessageInterface::ShowMessage
      ("EphemerisWriter::SetEpochAndDirection() <%p>'%s' leaving\n", this, ephemName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void SetPropIndicator(Integer propInd)
//------------------------------------------------------------------------------
void EphemerisWriter::SetPropIndicator(Integer propInd)
{
   propIndicator = propInd;
}

//------------------------------------------------------------------------------
// void SetEventEpoch(Real epochInSecs)
//------------------------------------------------------------------------------
void EphemerisWriter::SetEventEpoch(Real epochInSecs)
{
   eventEpochInSecs = epochInSecs;
}


//------------------------------------------------------------------------------
// void SetWriteNewSegment(bool newSeg)
//------------------------------------------------------------------------------
void EphemerisWriter::SetWriteNewSegment(bool newSeg)
{
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisWriter::SetWriteNewSegment() '%s' entered, newSeg=%d\n",
       ephemName.c_str(), newSeg);
   #endif
   writingNewSegment = newSeg;
}


//------------------------------------------------------------------------------
// void ProcessData(Real currEpochInDays, Real currState[6]);
//------------------------------------------------------------------------------
void ProcessData(Real currEpochInDays, Real currState[6])
{
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool EphemerisWriter::Initialize()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisWriter::Initialize() <%p>'%s' entered, spacecraftName='%s', "
       "isInitialized=%d\n   ephemType='%s', stateType='%s', "
       "outputFormat='%s'\n", this, ephemName.c_str(), spacecraftName.c_str(), 
       isInitialized, ephemType.c_str(), stateType.c_str(),
       outputFormat.c_str());
   MessageInterface::ShowMessage
      ("   interpolateInitialState=%d, interpolateFinalState=%d\n", interpolateInitialState,
       interpolateFinalState);
   #endif
   
   if (isInitialized)
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("EphemerisWriter::Initialize() <%p>'%s' is already initialized so just returning true\n",
          this, ephemName.c_str());
      #endif
      return true;
   }
   
   // Initialize data
   isInitialized = true;
   isFinalized = false;
   canFinalize = false;
   isEndOfRun = false;
   firstTimeWriting  = true;
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("   spacecraft=<%p>'%s', outCoordSystem=<%p>'%s'\n", 
       spacecraft, spacecraft->GetName().c_str(), outCoordSystem,
       outCoordSystem->GetName().c_str());
   #endif
   
   // Determine orbit or attitude, set to boolean to avoid string comparison
   if (stateType == "Cartesian")
      writeOrbit = true;
   else
      writeAttitude = true;
   
   // Determine output coordinate system, set to boolean to avoid string comparison.
   if (dataCoordSystem && (dataCoordSystem->GetName() != outCoordSystemName))
      writeDataInDataCS = false;
   
   // Set initial interpolation flag for first and final state
   if (initialEpochStr != "InitialSpacecraftEpoch")
      interpolateInitialState = true;
   
   if (finalEpochStr != "FinalSpacecraftEpoch")
      interpolateFinalState = true;
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("   After checking, interpolateInitialState=%d, interpolateFinalState=%d\n",
       interpolateInitialState, interpolateFinalState);
   #endif
   
   // Set spacecraft ID
   spacecraftId = spacecraft->GetStringParameter("Id");
   
   // Get spacecraft initial epoch
   Real satInitialEpoch = spacecraft->GetEpoch();
   
   // Check if initial state really needs to be interpolated
   if (!useFixedStepSize)
   {      
      // If user defined initial epoch is the same or less than spacecraft
      // initial epoch, no interpolation is needed for the first state
      if (initialEpochA1Mjd == satInitialEpoch)
      {
         interpolateInitialState = false;
         initialEpochA1Mjd = -999.999;
         #ifdef DEBUG_EPHEMFILE_INIT
         MessageInterface::ShowMessage
            ("   ==> User defined initial epoch is equal to SC epoch, "
             "so turning off interpolateInitialState\n");
         #endif
      }
      else
      {
         // If using user defined initial epoch, make integrator step size to
         // difference between initialEpoch and spacecraft initial epoch
         if (initialEpochStr != "InitialSpacecraftEpoch")
            stepSizeInSecs = (initialEpochA1Mjd - satInitialEpoch) * GmatTimeConstants::SECS_PER_DAY;
         
         #ifdef DEBUG_EPHEMFILE_INIT
         MessageInterface::ShowMessage
            ("   ==> User defined initial epoch is greater than SC epoch\n   "
             "initialEpochA1Mjd=%.12f, satInitialEpoch=%.12f, stepSizeInSecs=%.12f\n",
             initialEpochA1Mjd, satInitialEpoch, stepSizeInSecs);
         #endif
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("   useFixedStepSize=%d, interpolateInitialState=%d, interpolateFinalState=%d\n",
       useFixedStepSize, interpolateInitialState, interpolateFinalState);
   MessageInterface::ShowMessage
      ("EphemerisWriter::Initialize() <%p>'%s' returning true, writeOrbit=%d, writeAttitude=%d, "
       "useFixedStepSize=%d,\n   writeDataInDataCS=%d, initialEpochA1Mjd=%.9f, "
       "finalEpochA1Mjd=%.9f, stepSizeInSecs=%.9f\n", this, ephemName.c_str(), writeOrbit,
       writeAttitude, useFixedStepSize, writeDataInDataCS, initialEpochA1Mjd, finalEpochA1Mjd,
       stepSizeInSecs);
   #endif
   
   return true;
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
void EphemerisWriter::Copy(const EphemerisWriter* orig)
{
   operator=(*((EphemerisWriter *)(orig)));
}

//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
// void InitializeData(bool saveEpochInfo = true)
//------------------------------------------------------------------------------
void EphemerisWriter::InitializeData(bool saveEpochInfo)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisWriter::InitializeData() <%p>'%s' entered, saveEpochInfo=%d\n",
       this, ephemName.c_str(), saveEpochInfo);
   #endif
   
   if (!saveEpochInfo)
   {
      currEpochInDays = -999.999;
      currEpochInSecs = -999.999;
   }
   
   blockBeginA1Mjd    = -999.999;
   nextOutEpochInSecs = -999.999;
   nextReqEpochInSecs = -999.999;
   prevEpochInSecs    = -999.999;
   prevProcTime       = -999.999;
   lastEpochWrote     = -999.999;
   eventEpochInSecs   = -999.999;
   writingNewSegment  = true;
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisWriter::InitializeData() <%p>'%s' leaving, currEpochInSecs=%.15f, "
       "prevEpochInSecs=%.15f\n", this, ephemName.c_str(), currEpochInSecs, prevEpochInSecs);
   #endif
}


//------------------------------------------------------------------------------
// virtual void CreateEphemerisFile(bool useDefaultFileName, const std::string &stType,
//                                  const std::string &outFormat, const std::string &covType)
//------------------------------------------------------------------------------
void EphemerisWriter::CreateEphemerisFile(bool useDefaultFileName,
                                          const std::string &stType,
                                          const std::string &outFormat,
                                          const std::string &covFormat)
{
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("===== EphemerisWriter::CreateEphemerisFile() <%p>'%s' entered, useDefaultFileName=%d, "
       "stType='%s', outFormat='%s'\n", this, ephemName.c_str(), useDefaultFileName,
       stType.c_str(), outFormat.c_str());
   #endif
   
   usingDefaultFileName = useDefaultFileName;
   stateType = stType;
   outputFormat = outFormat;
   covarianceFormat = covFormat;
   
   #ifdef DEBUG_EPHEMFILE_CREATE
   MessageInterface::ShowMessage
      ("===== EphemerisWriter::CreateEphemerisFile() <%p>'%s' leaving\n", this,
       ephemName.c_str(), useDefaultFileName);
   #endif
}


//------------------------------------------------------------------------------
// bool OpenTextEphemerisFile(const std::string &fn)
//------------------------------------------------------------------------------
bool EphemerisWriter::OpenTextEphemerisFile(const std::string &fname)
{
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemerisWriter::OpenTextEphemerisFile() <%p>'%s' entered, fileName = %s\n",
       this, ephemName.c_str(), fileName.c_str());
   #endif
   
   bool retval = false;
   
   // Close the stream if it is open
   if (dstream.is_open())
      dstream.close();
   
   std::string fileNameToOpen = fname;
   
   dstream.open(fileNameToOpen.c_str());
   if (dstream.is_open())
   {
      retval = true;
      #ifdef DEBUG_EPHEMFILE_OPEN
      MessageInterface::ShowMessage
         ("   '%s' is opened for %s\n", fileNameToOpen.c_str(), ephemType.c_str());
      #endif
   }
   else
   {
      retval = false;
      #ifdef DEBUG_EPHEMFILE_OPEN
      MessageInterface::ShowMessage
         ("   '%s' is failed open for %s\n", ephemType.c_str(), fileNameToOpen.c_str());
      #endif
   }
   
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemerisWriter::OpenTextEphemerisFile() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void CloseEphemerisFile(bool done = true, writeMetaData = true)
//------------------------------------------------------------------------------
void EphemerisWriter::CloseEphemerisFile(bool done, bool writeMetaData)
{
   // Do nothing here?
}


//------------------------------------------------------------------------------
// bool InsufficientDataPoints()
//------------------------------------------------------------------------------
bool EphemerisWriter::InsufficientDataPoints()
{
   return insufficientDataPoints;
}

//------------------------------------------------------------------------------
// bool IsEphemFileOpened()
//------------------------------------------------------------------------------
bool EphemerisWriter::IsEphemFileOpened()
{
   return isEphemFileOpened;
}

//------------------------------------------------------------------------------
// bool IsFirstTimeWriting()
//------------------------------------------------------------------------------
bool EphemerisWriter::IsFirstTimeWriting()
{
   return firstTimeWriting;
}

//------------------------------------------------------------------------------
// bool IsDataEmpty()
//------------------------------------------------------------------------------
bool EphemerisWriter::IsDataEmpty()
{
   if (firstTimeWriting && currEpochInSecs == -999.999 && a1MjdArray.empty())
      return true;
   else
      return false;
}

//------------------------------------------------------------------------------
// bool HasFinalEpochProcessed()
//------------------------------------------------------------------------------
bool EphemerisWriter::HasFinalEpochProcessed()
{
   return finalEpochProcessed;
}

//------------------------------------------------------------------------------
// bool IsFinalized()
//------------------------------------------------------------------------------
bool EphemerisWriter::IsFinalized()
{
   return isFinalized;
}

//------------------------------------------------------------------------------
// bool CheckInitialAndFinalEpoch()
//------------------------------------------------------------------------------
bool EphemerisWriter::CheckInitialAndFinalEpoch()
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("\nEphemerisWriter::CheckInitialAndFinalEpoch() <%p>'%s' entered, currEpochInDays=%.15f\n   "
       "initialEpochA1Mjd=%.15f, finalEpochA1Mjd=%.15f, finalEpochReached=%d, finalEpochProcessed=%d\n",
       this, ephemName.c_str(), currEpochInDays, initialEpochA1Mjd, finalEpochA1Mjd,
       finalEpochReached, finalEpochProcessed);
   #endif
   
   // Check initial and final epoch for writing, dat[0] is epoch
   bool writeData = false;
   bool initialEpochReached = true;
   finalEpochReached = false;
   
   // From InitialSpacecraftEpoch to FinalSpacecraftEpoch
   if (initialEpochA1Mjd == -999.999 && finalEpochA1Mjd == -999.999)
   {
      writeData = true;
   }
   // From InitialSpacecraftEpoch to user specified final epoch
   else if (initialEpochA1Mjd == -999.999 && finalEpochA1Mjd != -999.999)
   {
      if (currEpochInDays <= finalEpochA1Mjd)
         writeData = true;
      
      if (currEpochInDays > finalEpochA1Mjd)
         finalEpochReached = true;
   }
   // From user specified initial epoch to FinalSpacecraftEpoch
   else if (initialEpochA1Mjd != -999.999 && finalEpochA1Mjd == -999.999)
   {
      if (currEpochInDays >= initialEpochA1Mjd)
         writeData = true;
      else
         initialEpochReached = false;
   }
   // From user specified initial epoch to user specified final epoch
   else
   {
      #ifdef DEBUG_EPHEMFILE_WRITE
      MessageInterface::ShowMessage
         ("   ===> Using user specified initial and final epoch, so checking...\n");
      #endif
      if (currEpochInDays < initialEpochA1Mjd)
         initialEpochReached = false;
      
      // Use tolerance of -1.0e-11 when checking for time to write (GMT-4079 fix)
      //if ((currEpochInDays >= initialEpochA1Mjd) && (currEpochInDays <= finalEpochA1Mjd))
      if (((currEpochInDays - initialEpochA1Mjd) >= -1.0e-11) &&
          (finalEpochA1Mjd - currEpochInDays) >= -1.0e-11)
         writeData = true;
      
      if (currEpochInDays > finalEpochA1Mjd)
         finalEpochReached = true;
   }
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemerisWriter::CheckInitialAndFinalEpoch() returning writeData=%d, "
       "finalEpochReached=%d, initial epoch %s\n\n", writeData,
       finalEpochReached, initialEpochReached ? "reached" : "not reached");
   #endif
   
   return writeData;
}


//------------------------------------------------------------------------------
// void HandleWriteOrbit()
//------------------------------------------------------------------------------
void EphemerisWriter::HandleWriteOrbit()
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemerisWriter::HandleWriteOrbit() entered, useFixedStepSize=%d, "
       "interpolateInitialState=%d, interpolateFinalState=%d\n", useFixedStepSize,
       interpolateInitialState, interpolateFinalState);
   DebugWriteTime("   nextReqEpochInSecs = ", nextReqEpochInSecs);
   #endif
   
   // Check interpolateInitialState first
   if (interpolateInitialState)
   {
      #ifdef DEBUG_EPHEMFILE_WRITE
      MessageInterface::ShowMessage
         ("===> Need to interpolateInitialState, so calling WriteOrbitAt()\n");
      #endif
      WriteOrbitAt(nextReqEpochInSecs, currState, currCov, currAccel, currQuat);
      Real tdiff = nextReqEpochInSecs - (initialEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY);
      if (GmatMathUtil::Abs(tdiff) <= 1.0-6)
      {
         #ifdef DEBUG_EPHEMFILE_WRITE
         MessageInterface::ShowMessage
            ("===> State at initial epoch has been written, so turning off "
             "interpolateInitialState flag\n");
         #endif
         interpolateInitialState = false;
         // Reset to write integrator steps
         initialEpochA1Mjd = -999.999;
         nextOutEpochInSecs = -999.999;
      }
   }
   
   if (useFixedStepSize)
   {
      WriteOrbitAt(nextReqEpochInSecs, currState, currCov, currAccel, currQuat);
   }
   else if (interpolateFinalState)
   {
      if (currEpochInDays < finalEpochA1Mjd)
         WriteOrbit(currEpochInSecs, currState, currCov, currAccel, currQuat);
      else
         WriteOrbitAt(nextReqEpochInSecs, currState, currCov, currAccel, currQuat);
   }
   else
   {
      WriteOrbit(currEpochInSecs, currState, currCov, currAccel, currQuat);
   }
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemerisWriter::HandleWriteOrbit() <%p> leaving\n", this);
   #endif
}


//------------------------------------------------------------------------------
// void HandleWriteAttitude()
//------------------------------------------------------------------------------
void EphemerisWriter::HandleWriteAttitude()
{
   GetAttitude();
   WriteAttitudeToFile();
}


//------------------------------------------------------------------------------
// void HandleEndOfRun()
//------------------------------------------------------------------------------
void EphemerisWriter::HandleEndOfRun()
{
   // End of run, blank out the last comment if needed
   #ifdef DEBUG_EPHEMFILE_FINISH
   std::string lastEpochWroteStr = ToUtcGregorian(lastEpochWrote);
   std::string currEpochStr = ToUtcGregorian(currEpochInSecs);
   std::string prevEpochStr = ToUtcGregorian(prevEpochInSecs);
   MessageInterface::ShowMessage
      ("EphemerisWriter::HandleEndOfRun() <%p>'%s' entered\n   isEndOfRun=%d, "
       "firstTimeWriting=%d\n   lastEpochWrote=%s, currEpochInSecs=%s, "
       "prevEpochInSecs=%s\n   a1MjdArray.size()=%d\n", this, ephemName.c_str(),
       isEndOfRun, firstTimeWriting, lastEpochWroteStr.c_str(), currEpochStr.c_str(),
       prevEpochStr.c_str(), a1MjdArray.size());
   #endif
   
   // If not first time and there is data to process, finish up writing
   //if (!firstTimeWriting && !a1MjdArray.empty())
   // If not first time writing, finish up writing. For STK array can be empty at the end of
   // event so do not check for empty array here. (Fix for GMT-5929 LOJ: 2016.12.29)
   if (!firstTimeWriting)
   {
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage("EphemerisWriter::HandleEndOfRun() Calling FinishUpWriting()\n");
      #endif
      
      try
      {
         FinishUpWriting();
      }
      catch (BaseException &be)
      {
         #ifdef DEBUG_EPHEMFILE_FINISH
         MessageInterface::ShowMessage
            ("EphemerisWriter::HandleEndOfRun() returning true after writing error "
             "message\n   FinishUpWriting() threw an exception\n  message = %s\n",
             be.GetFullMessage().c_str());
         #endif
         MessageInterface::ShowMessage("%s\n", be.GetFullMessage().c_str());
         return;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage("EphemerisWriter::HandleEndOfRun() <%p> leaving\n", this);
   #endif
}


//------------------------------------------------------------------------------
// bool SkipFunctionData()
//------------------------------------------------------------------------------
bool EphemerisWriter::SkipFunctionData()
{
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisWriter::SkipFunctionData() entered, Data is published from the function, "
       "'%s' IsGlobal:%s, IsLocal:%s\n   spacecraft=<%p>\n", ephemName.c_str(),
       isEphemGlobal ? "Yes" : "No", isEphemLocal ? "Yes" : "No");
   #endif
   
   bool skipData = false;
   if (spacecraft)
   {
      #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("   spacecraft = <%p>[%s]'%s', IsGlobal=%d, IsLocal=%d\n",
          spacecraft, spacecraft->GetTypeName().c_str(), spacecraft->GetName().c_str(),
          spacecraft->IsGlobal(), spacecraft->IsLocal());
      #endif
      
      // Skip data if EphemerisWriter is global and spacecraft is local
      if (isEphemGlobal && spacecraft->IsLocal())
      {
         #ifdef DEBUG_EPHEMFILE_DATA
         MessageInterface::ShowMessage
            ("   Skip data since '%s' is global and spacecraft is local\n",
             ephemName.c_str());
         #endif
         skipData = true;
      }
      // Skip data if spacecraft is not a global nor a local object
      else if (!(spacecraft->IsGlobal()) && !(spacecraft->IsLocal()))
      {
         #ifdef DEBUG_EPHEMFILE_DATA
         MessageInterface::ShowMessage
            ("   Skip data since spacecraft is not a global nor a local object\n");
         #endif
         skipData = true;
      }
   }
   
   if (skipData)
   {
      #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisWriter::SkipFunctionData() this=<%p>'%s' just returning true\n   data is "
          "from a function and spacecraft is not a global nor a local object\n",
          this, ephemName.c_str());
      #endif
   }
   return skipData;
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
bool EphemerisWriter::IsBackwardPropAllowed(Real propDirection)
{
   return false;
}


//------------------------------------------------------------------------------
// void WriteOrbit(Real reqEpochInSecs, const Real state[6], const Real cov[21], const Real accel[3])
//------------------------------------------------------------------------------
/**
 * Writes spacecraft orbit data to a ephemeris file.
 *
 * @param reqEpochInSecs Requested epoch to write in seconds 
 * @param state State to write 
 * @param cov Covariance to write
 * @param accel  acceleartion to write
 */
//------------------------------------------------------------------------------
void EphemerisWriter::WriteOrbit(Real reqEpochInSecs, const Real state[6], 
   const Real cov[21], const Real accel[3], const Real quat[4])
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("\nEphemerisWriter::WriteOrbit() <%p>'%s' entered\n", this, ephemName.c_str());
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   DebugWriteTime("    reqEpochInSecs = ", reqEpochInSecs);
   MessageInterface::ShowMessage
      ("   currState[0:2]=%.15f, %.15f, %.15f\n       state[0:2]=%.15f, %.15f, %.15f\n",
       currState[0], currState[1], currState[2], state[0], state[1], state[2]);
   MessageInterface::ShowMessage
      ("   currAccel[0:2]= [%.15f, %.15f, %.15f]\n       accel[0:2] = [%.15f, %.15f, %.15f]\n", 
         currAccel[0], currAccel[1], currAccel[2], accel[0], accel[1], accel[2]);
   #endif
   
   Real stateToWrite[6], covToWrite[21], accelToWrite[3], quatToWrite[4];
   for (int i=0; i<6; i++)
      stateToWrite[i] = state[i];
   for (int i = 0; i < 21; i++)
      covToWrite[i] = cov[i];
   for (int i = 0; i < 3; i++)
      accelToWrite[i] = accel[i];
   for (int i = 0; i < 4; i++)
      quatToWrite[i] = quat[i];

   Real outEpochInSecs = reqEpochInSecs;
   
   if (useFixedStepSize)
      FindNextOutputEpoch(reqEpochInSecs, outEpochInSecs, stateToWrite, covToWrite, accelToWrite);
   
   WriteOrbitData(outEpochInSecs, stateToWrite, covToWrite, accelToWrite, quatToWrite);
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   DebugWriteTime("   Setting lastEpochWrote to outEpochInsecs ", outEpochInSecs);
   #endif
   
   lastEpochWrote = outEpochInSecs;
   
   // If user defined final epoch has been written out, finalize
   if (finalEpochStr != "FinalSpacecraftEpoch" &&
       (lastEpochWrote >= (finalEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY)))
   {
      #ifdef DEBUG_EPHEMFILE_WRITE
      MessageInterface::ShowMessage("==========> Completed writing the final epoch\n");
      #endif
      finalEpochProcessed = true;
   }
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemerisWriter::WriteOrbit() leaving, lastEpochWrote=%.15f, %s\n\n",
       lastEpochWrote, ToUtcGregorian(lastEpochWrote).c_str());
   #endif
}


//------------------------------------------------------------------------------
// void WriteOrbitAt(Real reqEpochInSecs, const Real state[6], const Real cov[21], const Real accel[3])
//------------------------------------------------------------------------------
/**
 * Writes spacecraft orbit data to a ephemeris file.
 *
 * @param reqEpochInSecs Requested epoch to write in seconds 
 * @param state State to write 
 * @param cov Covariance to write
 * @param accel   Acceleration to write
 */
//------------------------------------------------------------------------------
void EphemerisWriter::WriteOrbitAt(Real reqEpochInSecs, const Real state[6], const Real cov[21], const Real accel[3], const Real quat[4])
{
}


//------------------------------------------------------------------------------
// void GetAttitude()
//------------------------------------------------------------------------------
void EphemerisWriter::GetAttitude()
{
   // Get spacecraft attitude in direction cosine matrix
   attEpoch = spacecraft->GetEpoch();
   Rmatrix33 dcm = spacecraft->GetAttitude(attEpoch);
   Rvector quat = AttitudeConversionUtility::ToQuaternion(dcm);
   for (int i = 0; i < 4; i++)
      currQuat[i] = quat[i];
}


//------------------------------------------------------------------------------
// void WriteAttitudeToFile()
//------------------------------------------------------------------------------
/**
 * Writes attitude to text output file
 */
//------------------------------------------------------------------------------
void EphemerisWriter::WriteAttitudeToFile()
{
   char strBuff[200];
   sprintf(strBuff, "%16.10f  %19.15f  %19.15f  %19.15f  %19.15f\n",
           attEpoch, currQuat[0], currQuat[1], currQuat[2], currQuat[3]);
   dstream << strBuff;
}


//------------------------------------------------------------------------------
// bool SetStepSize(const std::string &value)
//------------------------------------------------------------------------------
/*
 * Sets real value step size.
 *
 * @param value step size value string
 *
 * @exception SubscriberException is thrown if value not converted to real number
 *            or value is negative
 */
//------------------------------------------------------------------------------
bool EphemerisWriter::SetStepSize(Real stepSzInSecs)
{
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisWriter::SetStepSize() entered, stepSzInSecs='%f'\n",
       stepSzInSecs);
   #endif
   
   // If step size is not undefined, set it
   if (stepSzInSecs != -999.999)
   {
      stepSizeInSecs = stepSzInSecs;
      stepSizeInA1Mjd = stepSizeInSecs / GmatTimeConstants::SECS_PER_DAY;
      
      useFixedStepSize = true;
      createInterpolator = true;
   }
   
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisWriter::SetStepSize() leaving, stepSize='%s', stepSizeInA1Mjd=%.15f, "
       "stepSizeInSecs=%.15f\n", stepSize.c_str(), stepSizeInA1Mjd, stepSizeInSecs);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// std::string ToString(const StringArray &strList)
//------------------------------------------------------------------------------
/**
 * Converts std::string array to std::string separated by comma.
 */
//------------------------------------------------------------------------------
std::string EphemerisWriter::ToString(const StringArray &strList)
{
   std::string str = "";
   std::string delimiter = ", ";
   if (strList.size() > 0)
   {
      str = strList[0];
      
      for (unsigned int i=1; i<strList.size(); i++)
         str = str + delimiter + strList[i];
   }
   
   return str;
}


//------------------------------------------------------------------------------
// std::string GetBackwardPropWarning()
//------------------------------------------------------------------------------
/**
 * Formats and returns backward propagation warning.
 *
 * @return  Formatted backward prop warning
 */
//------------------------------------------------------------------------------
std::string EphemerisWriter::GetBackwardPropWarning()
{
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisWriter::GetBackwardPropWarning() entered\n");
   #endif
   
   std::string currTimeStr = ToUtcGregorian(currEpochInSecs);
   std::string prevTimeStr = ToUtcGregorian(prevEpochInSecs);
   std::string msg = "*** WARNING *** The user has generated non-monotonic "
      "invalid ephemeris file \"" + ephemName + "\" starting at " + currTimeStr +
      "; previous time is " + prevTimeStr + ".";
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("   current time (%s> < previous time (%s)\n", currTimeStr.c_str(),
       prevTimeStr.c_str());
   MessageInterface::ShowMessage
      ("EphemerisWriter::GetBackwardPropWarning() returning '%s'\n", msg.c_str());
   #endif
   
   return msg;
}


//------------------------------------------------------------------------------
// void WriteHeader()
//------------------------------------------------------------------------------
void EphemerisWriter::WriteHeader()
{
}


//------------------------------------------------------------------------------
// void WriteMetaData()
//------------------------------------------------------------------------------
void EphemerisWriter::WriteMetaData()
{
}


//------------------------------------------------------------------------------
// void WriteDataComments(const std::string &comments, bool isErrorMsg,
//                    bool ignoreBlankComments = true, bool writeKeyword = true)
//------------------------------------------------------------------------------
/**
 * Writes comments to specific file.
 */
//------------------------------------------------------------------------------
void EphemerisWriter::WriteDataComments(const std::string &comments, bool isErrorMsg,
                                    bool ignoreBlankComments, bool writeKeyword)
{
}


//------------------------------------------------------------------------------
// void WriteStringToFile(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Writes string to text file for CCSDS or as debug output
 */
//------------------------------------------------------------------------------
void EphemerisWriter::WriteStringToFile(const std::string &str)
{
   // For now write it text file
   dstream << str;
   dstream.flush();
}


//------------------------------------------------------------------------------
// void ClearOrbitData()
//------------------------------------------------------------------------------
void EphemerisWriter::ClearOrbitData()
{
   #ifdef DEBUG_EPHEMFILE_CLEAR
   MessageInterface::ShowMessage
      ("ClearOrbitData() entered, there is(are) %d data point(s) to clear\n", a1MjdArray.size());
   #endif
   EpochArray::iterator ei;
   for (ei = a1MjdArray.begin(); ei != a1MjdArray.end(); ++ei)
      delete (*ei);
   
   StateArray::iterator si;
   for (si = stateArray.begin(); si != stateArray.end(); ++si)
      delete (*si);

   std::vector<Rvector*>::iterator ci;
   for (ci = covArray.begin(); ci != covArray.end(); ++ci)
      delete (*ci);
   
   std::vector<Rvector3*>::iterator ai;
   for (ai = accelArray.begin(); ai != accelArray.end(); ++ai)
      delete (*ai);

   std::vector<Rvector*>::iterator rvacovi;
   for (rvacovi = rvacovArray.begin(); rvacovi != rvacovArray.end(); ++rvacovi)
      delete (*rvacovi);

   std::vector<Rvector*>::iterator qi;
   for (qi = quatArray.begin(); qi != quatArray.end(); ++qi)
      delete (*qi);

   a1MjdArray.clear();
   stateArray.clear();
   covArray.clear();
   accelArray.clear();
   rvacovArray.clear();
   quatArray.clear();
}


//------------------------------------------------------------------------------
// void FindNextOutputEpoch(Real reqEpochInSecs, Real &outEpochInSecs,
//                          Real stateToWrite[6], Real covToWrite[21],
//                          Real accelToWrite[3])
//------------------------------------------------------------------------------
void EphemerisWriter::FindNextOutputEpoch(Real reqEpochInSecs, Real &outEpochInSecs,
                                          Real stateToWrite[6], Real covToWrite[21], 
                                          Real accelToWrite[3])
{
   // Do nothing here
}


//------------------------------------------------------------------------------
// void WriteOrbitData(Real reqEpochInSecs, const Real state[6], const Real cov[21], const Real accel[3])
//------------------------------------------------------------------------------
void EphemerisWriter::WriteOrbitData(Real reqEpochInSecs, const Real state[6], const Real cov[21], const Real accel[3], const Real quat[4])
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   DebugWriteTime
      ("EphemerisWriter::WriteOrbitData() entered, reqEpochInSecs = ", reqEpochInSecs);
   MessageInterface::ShowMessage
      ("state[0:2]=%.15f, %.15f, %.15f\n", state[0], state[1], state[2]);
   #endif
   
   Real outState[6], outCov[21], outAccel[3], outQuat[4];
   for (int i = 0; i < 6; i++)
      outState[i] = state[i];
   for (int i = 0; i < 21; i++)
      outCov[i] = cov[i];
   for (int i = 0; i < 3; i++)
      outAccel[i] = accel[i];
   for (int i = 0; i < 3; i++)
      outQuat[i] = quat[i];

   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage("EphemerisWriter::WriteOrbitData() Calling ConvertState()\n");
   #endif
   // Convert state if needed
   if (!writeDataInDataCS)
   {
      ConvertState(reqEpochInSecs/GmatTimeConstants::SECS_PER_DAY, state, outState, cov, outCov, accel, outAccel);
   }
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage("EphemerisWriter::WriteOrbitData() Calling BufferOrbitData()\n");
   #endif
   // BufferOrbitData(reqEpochInSecs / GmatTimeConstants::SECS_PER_DAY, outState, outCov);
   BufferOrbitData(reqEpochInSecs / GmatTimeConstants::SECS_PER_DAY, outState, outCov, outAccel, outQuat);

   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage("EphemerisWriter::WriteOrbitData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void ConvertState(Real epochInDays, const Real inState[6], Real outState[6],
//                   const Real inCov[21], Real outCov[21], const Real inAccel[3], 
//                   Real outAccel[3])
//------------------------------------------------------------------------------
void EphemerisWriter::ConvertState(Real epochInDays, const Real inState[6],
                                 Real outState[6], const Real inCov[21],
                                 Real outCov[21], const Real inAccel[3], 
                                 Real outAccel[3])
{
   #ifdef DEBUG_EPHEMFILE_CONVERT_STATE   
   DebugWriteOrbit("In ConvertState(in):", epochInDays, inState, true, true);
   #endif
   
   coordConverter.Convert(A1Mjd(epochInDays), inState, dataCoordSystem,
                          outState, outCoordSystem, true);

   Rmatrix33 r = coordConverter.GetLastRotationMatrix();
   Rmatrix33 rdot = coordConverter.GetLastRotationDotMatrix();

   Rmatrix cov(6,6);
   for (UnsignedInt ii = 0U; ii < 6; ii++)
   {
      for (UnsignedInt jj = 0U; jj < 6; jj++)
      {
         UnsignedInt idx = (ii)*(ii + 1) / 2 + jj;
         cov(ii, jj) = inCov[idx];
      }
   }

   Rmatrix covRot(6,6);
   for (UnsignedInt ii = 0U; ii < 3; ii++)
   {
      for (UnsignedInt jj = 0U; jj < 3; jj++)
      {
         covRot(ii, jj) = r(ii, jj);
         covRot(ii, jj + 3) = 0;
         covRot(ii + 3, jj) = rdot(ii, jj);
         covRot(ii + 3, jj + 3) = r(ii, jj);
      }
   }

   Rmatrix outCovMatrix(6, 6);
   outCovMatrix = covRot * cov * covRot.Transpose();

   UnsignedInt idx = 0U;
   for (UnsignedInt ii = 0; ii < 6; ii++)
   {
      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         outCov[idx] = outCovMatrix(ii, jj);
         idx++;
      }
   }
   
   Rvector3 pos, vel, accel, outAcceleration;
   for (Integer i = 0; i < 3; ++i)
   {
      pos[i] = inState[i];
      vel[i] = inState[i + 3];
      accel[i] = inAccel[i];
   }

   // Convert spacecraft position from Frame1 to Frame2. It needs to translation and then a rotation.
   // This is equation represtion for conversion between 2 frames:
   //             posNew = posO2 - posO1 + [Rot]*pos
   // Where: posO1 and posO2 are position of the origins of Frame1 and Frame2 respectively.
   //        [Rot] is rotaion matrix from Frame1 to Frame2
   //        newPos and pos are position of spacecarft represents in Frame2 and Frame1 respectively 
   // Take both size of equation w.r.t. time tiwce. We obatins
   //        newAcceleration = accelO2 - accelO1 + accel + 2*[(d/dt)[Rot]]*vel + [(dd/dtdt)[Rot]]*pos 
   // Where: pos, vel, and accel are position, velocity, and acceleration in Frame1
   //        [(d/dt)[Rot]] and [(dd/dtdt)[Rot]] are first and second derivative of [Rot] matrix w.r.t. time 
   //        accelO1 and accelO2 are accelration of origins of Frame1 and Frame2 respectively.
   //        newAcceleartion is the acceleration of spacecraft as it is presented in Frame2.

   Rvector3 accelO1 = dataCoordSystem->GetOrigin()->GetMJ2000Acceleration(A1Mjd(epochInDays));
   Rvector3 accelO2 = outCoordSystem->GetOrigin()->GetMJ2000Acceleration(A1Mjd(epochInDays));

   // Currently, all build in GMAT coordinate system does not calculate the second derivative of rotation matrix.
   // To simplify, we assume it is a zero matrix.
   Rmatrix33 r2dot;
   r2dot.Set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

   outAcceleration = accelO2 - accelO1 + accel + 2 * (rdot*vel) + r2dot * pos;
   for (Integer i = 0; i < 3; ++i)
      outAccel[i] = outAcceleration[i];

   #ifdef DEBUG_EPHEMFILE_CONVERT_STATE   
   DebugWriteOrbit("In ConvertState(out):", epochInDays, outState, true, true);
   #endif
}


//------------------------------------------------------------------------------
// std::string ToUtcGregorian(Real epoch, bool inDays = false, Integer format = 1)
//------------------------------------------------------------------------------
/**
 * Formats epoch in either in days or seconds to desired format.
 *
 * @param  epoch  Epoch in days or seconds
 * @param  inDays  Flag indicating epoch is in days
 * @param  format  Desired output format [1]
 *                 1 = "01 Jan 2000 11:59:28.000"
 *                 2 = "2000-01-01T11:59:28.000"
 */
//------------------------------------------------------------------------------
std::string EphemerisWriter::ToUtcGregorian(Real epoch, bool inDays, Integer format)
{
   if (epoch == -999.999)
      return "-999.999";

   
   Real toMjd;
   std::string epochStr;
   
   Real epochInDays = epoch;
   if (!inDays)
      epochInDays = epoch / GmatTimeConstants::SECS_PER_DAY;
   
   std::string outFormat = epochFormat;
   if (format == 2)
      outFormat = "UTCGregorian";
   
   // Convert current epoch to specified format
   TimeSystemConverter::Instance()->Convert("A1ModJulian", epochInDays, "", outFormat,
                              toMjd, epochStr, format);
   
   if (epochStr == "")
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** EphemerisWriter::ToUtcGregorian() Cannot convert epoch %.10f %s "
          "to UTCGregorian\n", epoch, inDays ? "days" : "secs");
      
      epochStr = "EpochError";
   }
   
   return epochStr;
}


//------------------------------------------------------------------------------
// void DebugWriteTime(const std::string &msg, Real epoch, bool inDays = false,
//                     Integer format = 1)
//------------------------------------------------------------------------------
/**
 * Writes debug output of time
 *
 * @param  msg  Message to write
 * @param  epoch  Epoch in days or seconds
 * @param  inDays  Flag indicating epoch is in days
 * @param  format  Format of the time to be written out
 *                 1 = "01 Jan 2000 11:59:28.000"
 *                 2 = "2000-01-01T11:59:28.000"
 */
//------------------------------------------------------------------------------
void EphemerisWriter::DebugWriteTime(const std::string &msg, Real epoch, bool inDays,
                                   Integer format)
{
   if (epoch == -999.999)
   {
      MessageInterface::ShowMessage("%s%f\n", msg.c_str(), epoch);
      return;
   }
   
   Real epochInDays = epoch;
   if (!inDays)
      epochInDays = epoch / GmatTimeConstants::SECS_PER_DAY;
   
   std::string epochStr = ToUtcGregorian(epochInDays, true, format);
   
   MessageInterface::ShowMessage
      ("%s%.15f, %.15f, '%s'\n", msg.c_str(), epoch, epochInDays, epochStr.c_str());
}


//------------------------------------------------------------------------------
// void DebugWriteOrbit(const std::string &msg, Real epoch, const Real state[6],
//                      bool inDays = false, bool logOnly = false)
//------------------------------------------------------------------------------
void EphemerisWriter::DebugWriteOrbit(const std::string &msg, Real epoch,
                                      const Real state[6], bool inDays, bool logOnly)
{
   Real reqEpochInDays = epoch;
   if (!inDays)
      reqEpochInDays = epoch / GmatTimeConstants::SECS_PER_DAY;
   
   std::string epochStr = ToUtcGregorian(reqEpochInDays, true, 2);
   
   if (logOnly)
   {
      MessageInterface::ShowMessage
         ("%s\n%s\n% 1.15e  % 1.15e  % 1.15e\n% 1.15e  % 1.15e  % 1.15e\n",
          msg.c_str(), epochStr.c_str(), state[0], state[1], state[2],
          state[3], state[4], state[5]);
   }
   else
   {
      char strBuff[200];
      sprintf(strBuff, "%s  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
              epochStr.c_str(), state[0], state[1], state[2], state[3],
              state[4], state[5]);
      dstream << strBuff;
      dstream.flush();
   }
}


//------------------------------------------------------------------------------
// void DebugWriteOrbit(const std::string &msg, A1Mjd *epochInDays, Rvector6 *state,
//                      bool logOnly = false)
//------------------------------------------------------------------------------
void EphemerisWriter::DebugWriteOrbit(const std::string &msg, A1Mjd *epochInDays,
                                    Rvector6 *state, bool logOnly)
{
   DebugWriteOrbit(msg, epochInDays->GetReal(), state->GetDataVector(), true, logOnly);
}

//------------------------------------------------------------------------------
// void DebugWriteState(const std::string &msg, const Real state[6])
//------------------------------------------------------------------------------
void EphemerisWriter::DebugWriteState(const std::string &msg, const Real state[6])
{   
   MessageInterface::ShowMessage
      ("%s\n% 1.15e  % 1.15e  % 1.15e\n% 1.15e  % 1.15e  % 1.15e\n",
       msg.c_str(), state[0], state[1], state[2], state[3], state[4], state[5]);
}

