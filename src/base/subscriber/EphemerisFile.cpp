//$Id$
//------------------------------------------------------------------------------
//                                  EphemerisFile
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
// Created: 2009.09.02
// Modified: 2015.11.18 Refactored EphemerisFile
//           2016.02.05 Added STK EphemerisTimePosVel format
//
/**
 * Base class for writing spacecraft orbit states to an ephemeris file.
 */
//------------------------------------------------------------------------------

#include "EphemerisFile.hpp"
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
#include "AttitudeWriterCK.hpp"
#include "EphemWriterCCSDS.hpp"
#include "EphemWriterSPK.hpp"
#include "EphemWriterCode500.hpp"
#include "EphemWriterSTK.hpp"
#include "Propagate.hpp"
#include <sstream>                   // for <<, std::endl


//#define DEBUG_EPHEMFILE_INSTANCE
//#define DEBUG_EPHEMFILE_ACTION
//#define DEBUG_EPHEMFILE_SET
//#define DEBUG_EPHEMFILE_INIT
//#define DEBUG_EPHEMFILE_FINISH
//#define DEBUG_EPHEMFILE_RESTART
//#define DEBUG_EPHEMFILE_EVENTS
//#define DEBUG_EPHEMFILE_SOLVER_DATA
//#define DEBUG_EPHEMFILE_CONVERT_STATE
//#define DEBUG_FILE_PATH
//#define DEBUG_EPHEMFILE_DATA
//#define DEBUG_EPHEMFILE_DATA_MORE
//#define DEBUG_EPHEMFILE_DATA_LABELS
//#define DEBUG_MANEUVER
//#define DEBUG_EPHEMFILE_MANEUVER
//#define DEBUG_EPHEMFILE_MANEUVER_MORE
//#define DEBUG_EPHEMFILE_PROPAGATOR_CHANGE
//#define DEBUG_EPHEMFILE_SC_PROPERTY_CHANGE
//#define DEBUG_EPHEMFILE_REF_OBJ
//#define DEBUG_EPHEMFILE_EXTENSION
//#define DEBUG_EPHEMFILE_ORBIT

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
StringArray EphemerisFile::fileFormatList;
StringArray EphemerisFile::epochFormatList;
StringArray EphemerisFile::initialEpochList;
StringArray EphemerisFile::finalEpochList;
StringArray EphemerisFile::stepSizeList;
StringArray EphemerisFile::stateTypeList;
StringArray EphemerisFile::writeEphemerisList;
StringArray EphemerisFile::interpolatorTypeList;
StringArray EphemerisFile::outputFormatList;
StringArray EphemerisFile::covFormatList;
StringArray EphemerisFile::distanceUnitList;
StringArray EphemerisFile::eventBoundariesList;

const std::string
EphemerisFile::PARAMETER_TEXT[EphemerisFileParamCount - SubscriberParamCount] =
{
   "Spacecraft",            // SPACECRAFT
   "Filename",              // FILENAME
   "FullPathFileName",      // FULLPATH_FILENAME
   "FileFormat",            // FILE_FORMAT

   "FileFormatVersion",     // FILE_FORMAT_VERSION
   "AddingAccelerationOption",  // ADDING_ACCELERATION_OPTION
   "AddingCovarianceOption",    // ADDING_ACCELERATION_OPTION
   
   "EpochFormat",           // EPOCH_FORMAT
   "InitialEpoch",          // INITIAL_EPOCH
   "FinalEpoch",            // FINAL_EPOCH
   "StepSize",              // STEP_SIZE
   "Interpolator",          // INTERPOLATOR
   "InterpolationOrder",    // INTERPOLATION_ORDER
   "StateType",             // STATE_TYPE
   "CoordinateSystem",      // COORDINATE_SYSTEM
   "OutputFormat",          // OUTPUT_FORMAT
   "IncludeCovariance",     // INCLUDE_COVARIANCE
   "WriteEphemeris",        // WRITE_EPHEMERIS
   "FileName",              // FILE_NAME - deprecated
   "DistanceUnit",          // DISTANCE_UNIT
   "IncludeEventBoundaries" // INCLUDE_EVENT_BOUNDARIES
};

const Gmat::ParameterType
EphemerisFile::PARAMETER_TYPE[EphemerisFileParamCount - SubscriberParamCount] =
{
   Gmat::OBJECT_TYPE,       // SPACECRAFT
   Gmat::FILENAME_TYPE,     // FILENAME
   Gmat::FILENAME_TYPE,     // FULLPATH_FILENAME
   Gmat::ENUMERATION_TYPE,  // FILE_FORMAT
   
   Gmat::STRING_TYPE,       // FILE_FORMAT_VERSION
   Gmat::BOOLEAN_TYPE,      // ADDING_ACCELERATION_OPTION
   Gmat::BOOLEAN_TYPE,      // ADDING_COVRAIANCE_OPTION

   Gmat::ENUMERATION_TYPE,  // EPOCH_FORMAT
   Gmat::ENUMERATION_TYPE,  // INITIAL_EPOCH
   Gmat::ENUMERATION_TYPE,  // FINAL_EPOCH
   Gmat::ENUMERATION_TYPE,  // STEP_SIZE
   Gmat::OBJECT_TYPE,       // INTERPOLATOR
   Gmat::INTEGER_TYPE,      // INTERPOLATION_ORDER
   Gmat::ENUMERATION_TYPE,  // STATE_TYPE
   Gmat::OBJECT_TYPE,       // COORDINATE_SYSTEM
   Gmat::ENUMERATION_TYPE,  // OUTPUT_FORMAT
   Gmat::ENUMERATION_TYPE,  // INCLUDE_COVARIANCE
   Gmat::BOOLEAN_TYPE,      // WRITE_EPHEMERIS
   Gmat::STRING_TYPE,       // FILE_NAME - deprecated
   Gmat::ENUMERATION_TYPE,  // DISTANCE_UNIT
   Gmat::BOOLEAN_TYPE,      // INCLUDE_EVENT_BOUNDARIES
};


//------------------------------------------------------------------------------
// EphemerisFile(const std::string &name, const std::string &type = "EphemerisFile")
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EphemerisFile::EphemerisFile(const std::string &name, const std::string &type) :
   Subscriber              (type, name),
   spacecraft              (NULL),
   outCoordSystem          (NULL),
   ephemWriter             (NULL),
   fullPathFileName        (""),
   spacecraftName          (""),
   spacecraftId            (""),
   prevFileName            (""),
   fileName                (""),
   fileFormat              ("CCSDS-OEM"),
   
   fileFormatVersion       ("1.0"),
   addingAccelerationOption(true),
   addingCovarianceOption  (true),

   epochFormat             ("UTCGregorian"),
   initialEpochStr         ("InitialSpacecraftEpoch"),
   finalEpochStr           ("FinalSpacecraftEpoch"),
   stepSize                ("IntegratorSteps"),
   interpolatorName        ("Lagrange"),
   stateType               ("Cartesian"),
   outCoordSystemName      ("EarthMJ2000Eq"),
   outputFormat            ("LittleEndian"),
   covFormat               ("None"),
   writeEphemeris          (true),
   usingDefaultFileName    (true),
   generateInBackground    (false),
   allowMultipleSegments   (true),
   prevPropName            (""),
   currPropName            (""),
   interpolationOrder      (7),
   toggleStatus            (0),
   propIndicator           (0),
   prevPropDirection       (0.0),
   currPropDirection       (0.0),
   initialEpochA1Mjd       (-999.999),
   finalEpochA1Mjd         (-999.999),
   blockBeginA1Mjd         (-999.999),
   currEpochInDays         (-999.999),
   currEpochInSecs         (-999.999),
   prevEpochInSecs         (-999.999),
   maneuverEpochInDays     (-999.999),
   eventEpochInSecs        (-999.999),
   stepSizeInSecs          (-999.999),
   firstTimeWriting        (true),
   useFixedStepSize        (false),
   checkForLargeTimeGap    (false),
   isEphemFileOpened       (false),
   canFinalize             (false),
   fileType                (UNKNOWN_FILE_TYPE),
   distanceUnit            ("Kilometers"),
   includeEventBoundaries  (true)
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemerisFile::EphemerisFile() default constructor <%p>'%s' entered\n",
       this, GetName().c_str());
   #endif
   
   objectTypes.push_back(Gmat::EPHEMERIS_FILE);
   objectTypeNames.push_back("EphemerisFile");
   objectTypeNames.push_back("FileOutput");
   parameterCount = EphemerisFileParamCount;
      
   // Get full path file name (LOJ: 2014.06.24)
   // The new GmatBase static method GetFullPathFileName() now handles empty file name
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage
      ("EphemerisFile::EphemerisFile() '%s' calling GmatBase::GetFullPathFileName()\n",
       GetName().c_str());
   #endif
   
   fullPathFileName =
      GmatBase::GetFullPathFileName(fileName, GetName(), fileName, "EPHEM_OUTPUT_FILE", false, ".oem");
   prevFileName = fileName;
   
   // Available enumeration type list, since it is static data, clear it first
   fileFormatList.clear();
   fileFormatList.push_back("CCSDS-OEM");
   // CCSDS-AEM not allowed in 2010 release (bug 2219)
   //fileFormatList.push_back("CCSDS-AEM");
   fileFormatList.push_back("SPK");
   fileFormatList.push_back("CK");
   fileFormatList.push_back("Code-500");
   fileFormatList.push_back("STK-TimePosVel");
   
   epochFormatList.clear();
   epochFormatList.push_back("UTCGregorian");
   epochFormatList.push_back("UTCModJulian");
   epochFormatList.push_back("TAIGregorian");
   epochFormatList.push_back("TAIModJulian");
   epochFormatList.push_back("TTGregorian");
   epochFormatList.push_back("TTModJulian");
   epochFormatList.push_back("A1Gregorian");
   epochFormatList.push_back("A1ModJulian");
   
   initialEpochList.clear();
   initialEpochList.push_back("InitialSpacecraftEpoch");
   
   finalEpochList.clear();
   finalEpochList.push_back("FinalSpacecraftEpoch");
   
   stepSizeList.clear();
   stepSizeList.push_back("IntegratorSteps");

   // Cartesian is the only allowed state type for the 2010 release (bug 2219)
   stateTypeList.clear();
   stateTypeList.push_back("Cartesian");
   stateTypeList.push_back("Quaternion");
   
   writeEphemerisList.clear();
   writeEphemerisList.push_back("Yes");
   writeEphemerisList.push_back("No");
   
   interpolatorTypeList.clear();
   interpolatorTypeList.push_back("Lagrange");
   interpolatorTypeList.push_back("Hermite");
   
   // SLERP not allowed in 2010 release (Bug 2219)
   //interpolatorTypeList.push_back("SLERP");
   
   outputFormatList.clear();
   outputFormatList.push_back("LittleEndian");
   outputFormatList.push_back("BigEndian");
   
   covFormatList.clear();
   covFormatList.push_back("None");
   covFormatList.push_back("Position");
   covFormatList.push_back("PositionAndVelocity");
   
   distanceUnitList.clear();
   distanceUnitList.push_back("Kilometers");
   distanceUnitList.push_back("Meters");

   eventBoundariesList.clear();
   eventBoundariesList.push_back("Yes");
   eventBoundariesList.push_back("No");

   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("fileName='%s', fullPathFileName='%s'\n", fileName.c_str(), fullPathFileName.c_str());
   MessageInterface::ShowMessage
      ("EphemerisFile::EphemerisFile() default constructor <%p>'%s' leaving\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// ~EphemerisFile()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EphemerisFile::~EphemerisFile()
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemerisFile::~EphemerisFile() destructor <%p>'%s' entered, ephemWriter=<%p>\n",
       this, GetName().c_str(), ephemWriter);
   #endif
   
   if (ephemWriter)
      delete ephemWriter;
}


//------------------------------------------------------------------------------
// EphemerisFile(const EphemerisFile &ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EphemerisFile::EphemerisFile(const EphemerisFile &ef) :
   Subscriber              (ef),
   spacecraft              (ef.spacecraft),
   outCoordSystem          (ef.outCoordSystem),
   ephemWriter             (NULL),
   fullPathFileName        (ef.fullPathFileName),
   spacecraftName          (ef.spacecraftName),
   spacecraftId            (ef.spacecraftId),
   prevFileName            (ef.prevFileName),
   fileName                (ef.fileName),
   fileFormat              (ef.fileFormat),
   
   fileFormatVersion       (ef.fileFormatVersion),
   addingAccelerationOption(ef.addingAccelerationOption),
   addingCovarianceOption  (ef.addingCovarianceOption),

   epochFormat             (ef.epochFormat),
   initialEpochStr         (ef.initialEpochStr),
   finalEpochStr           (ef.finalEpochStr),
   stepSize                (ef.stepSize),
   interpolatorName        (ef.interpolatorName),
   stateType               (ef.stateType),
   outCoordSystemName      (ef.outCoordSystemName),
   outputFormat            (ef.outputFormat),
   covFormat               (ef.covFormat),
   writeEphemeris          (ef.writeEphemeris),
   usingDefaultFileName    (ef.usingDefaultFileName),
   generateInBackground    (ef.generateInBackground),
   allowMultipleSegments   (ef.allowMultipleSegments),
   prevPropName            (ef.prevPropName),
   currPropName            (ef.currPropName),
   interpolationOrder      (ef.interpolationOrder),
   toggleStatus            (ef.toggleStatus),
   propIndicator           (ef.propIndicator),
   prevPropDirection       (ef.prevPropDirection),
   currPropDirection       (ef.currPropDirection),
   initialEpochA1Mjd       (ef.initialEpochA1Mjd),
   finalEpochA1Mjd         (ef.finalEpochA1Mjd),
   blockBeginA1Mjd         (ef.blockBeginA1Mjd),
   currEpochInDays         (ef.currEpochInDays),
   currEpochInSecs         (ef.currEpochInSecs),
   prevEpochInSecs         (ef.prevEpochInSecs),
   maneuverEpochInDays     (ef.maneuverEpochInDays),
   eventEpochInSecs        (ef.eventEpochInSecs),
   stepSizeInSecs          (ef.stepSizeInSecs),
   firstTimeWriting        (ef.firstTimeWriting),
   useFixedStepSize        (ef.useFixedStepSize),
   checkForLargeTimeGap    (ef.checkForLargeTimeGap),
   isEphemFileOpened       (ef.isEphemFileOpened),
   canFinalize             (ef.canFinalize),
   distanceUnit            (ef.distanceUnit),
   includeEventBoundaries  (ef.includeEventBoundaries)
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemerisFile::EphemerisFile() copy constructor <%p>'%s' entered\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// EphemerisFile& EphemerisFile::operator=(const EphemerisFile& ef)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
EphemerisFile& EphemerisFile::operator=(const EphemerisFile& ef)
{
   #ifdef DEBUG_EPHEMFILE_INSTANCE
   MessageInterface::ShowMessage
      ("EphemerisFile::operator=() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   if (this == &ef)
      return *this;
   
   Subscriber::operator=(ef);
   
   spacecraft           = ef.spacecraft;
   outCoordSystem       = ef.outCoordSystem;
   ephemWriter          = NULL;
   fullPathFileName     = ef.fullPathFileName;
   spacecraftName       = ef.spacecraftName;
   spacecraftId         = ef.spacecraftId;
   prevFileName         = ef.prevFileName;
   fileName             = ef.fileName;
   fileFormat           = ef.fileFormat;
   
   fileFormatVersion    = ef.fileFormatVersion;
   addingAccelerationOption = ef.addingAccelerationOption;
   addingCovarianceOption   = ef.addingCovarianceOption;
   
   epochFormat          = ef.epochFormat;
   initialEpochStr      = ef.initialEpochStr;
   finalEpochStr        = ef.finalEpochStr;
   stepSize             = ef.stepSize;
   interpolatorName     = ef.interpolatorName;
   stateType            = ef.stateType;
   outCoordSystemName   = ef.outCoordSystemName;
   outputFormat         = ef.outputFormat;
   covFormat            = ef.covFormat;
   writeEphemeris       = ef.writeEphemeris;
   usingDefaultFileName = ef.usingDefaultFileName;
   generateInBackground = ef.generateInBackground;
   allowMultipleSegments= ef.allowMultipleSegments;
   prevPropName         = ef.prevPropName;
   currPropName         = ef.currPropName;
   interpolationOrder   = ef.interpolationOrder;
   toggleStatus         = ef.toggleStatus;
   propIndicator        = ef.propIndicator;
   prevPropDirection    = ef.prevPropDirection;
   currPropDirection    = ef.currPropDirection;
   initialEpochA1Mjd    = ef.initialEpochA1Mjd;
   finalEpochA1Mjd      = ef.finalEpochA1Mjd;
   blockBeginA1Mjd      = ef.blockBeginA1Mjd;
   currEpochInDays      = ef.currEpochInDays;
   currEpochInSecs      = ef.currEpochInSecs;
   prevEpochInSecs      = ef.prevEpochInSecs;
   maneuverEpochInDays  = ef.maneuverEpochInDays;
   eventEpochInSecs     = ef.eventEpochInSecs;
   stepSizeInSecs       = ef.stepSizeInSecs;
   firstTimeWriting     = ef.firstTimeWriting;
   useFixedStepSize     = ef.useFixedStepSize;
   checkForLargeTimeGap = ef.checkForLargeTimeGap;
   isEphemFileOpened    = ef.isEphemFileOpened;
   canFinalize          = ef.canFinalize;
   distanceUnit         = ef.distanceUnit;
   includeEventBoundaries = ef.includeEventBoundaries;
   return *this;
}

//---------------------------------
// methods for this class
//---------------------------------

//------------------------------------------------------------------------------
// std::string GetProperFileName(const std::string &fName, const std::string &fType)
//------------------------------------------------------------------------------
std::string EphemerisFile::GetProperFileName(const std::string &fName,
                                             const std::string &fType,
                                             bool saveFileName)
{
   #ifdef DEBUG_EPHEMFILE_EXTENSION
   MessageInterface::ShowMessage
      ("EphemerisFile::GetProperFileName() entered, fName='%s', fType='%s', "
       "saveFileName=%d\n", fName.c_str(), fType.c_str(), saveFileName);
   #endif
   
   std::string expFileName = fName;
   std::string defaultExt = ".eph";
   if (fType == "SPK")
      defaultExt = ".bsp";
   else if (fType == "CCSDS-OEM")
      defaultExt = ".oem";
   else if (fType == "CCSDS-AEM")
      defaultExt = ".aem";
   else if (fType == "STK-TimePosVel")
      defaultExt = ".e";
   
   std::string parsedExt = GmatFileUtil::ParseFileExtension(fName, true);
   if (parsedExt != "" && parsedExt != defaultExt)
   {
      // Replace file extension only for SPK ephem type
      if (fType == "SPK")
      {
         std::string ofname = expFileName;
         expFileName = GmatStringUtil::Replace(expFileName, parsedExt, defaultExt);
         if (saveFileName)
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** %s file extension should be \"%s\", so "
                "file name '%s' changed to '%s'\n", fType.c_str(), defaultExt.c_str(),
                ofname.c_str(), expFileName.c_str());
            
            // Set filename and full path filename
            fileName = expFileName;
            fullPathFileName = GmatStringUtil::Replace(fullPathFileName, parsedExt, defaultExt);
         }
      }
      else
      {
         // We don't want to change the extension for other format such as Code-500
         // but we want to change it if creating new from the GUI. If saveFileName
         // is set to false, this method is called from the GUI.
         if (!saveFileName)
         {
            expFileName = GmatStringUtil::Replace(expFileName, parsedExt, defaultExt);
         }
      }
   }
   else if (parsedExt == "")
   {
      std::string ofname = expFileName;
      expFileName = expFileName + defaultExt;
      if (saveFileName)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** %s file extension should be \"%s\", so "
             "file name '%s' changed to '%s'\n", fType.c_str(), defaultExt.c_str(),
             ofname.c_str(), expFileName.c_str());
         
         // Set filename and full path filename
         fileName = expFileName;
         fullPathFileName = fullPathFileName + defaultExt;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_EXTENSION
   MessageInterface::ShowMessage
      ("EphemerisFile::GetProperFileName() returning '%s'\n", expFileName.c_str());
   #endif
   return expFileName;
}


//------------------------------------------------------------------------------
// void SetBackgroundGeneration(bool inBackground)
//------------------------------------------------------------------------------
void EphemerisFile::SetBackgroundGeneration(bool inBackground)
{
   #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
      ("EphemerisFile::SetBackgroundGeneration() entered, inBackground='%s', ephemWriter=<%p>\n",
       (inBackground? "true" : "false"), ephemWriter);
   #endif
   if (ephemWriter)
      ephemWriter->SetBackgroundGeneration(inBackground);
}


//----------------------------------
// methods inherited from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual void SetProvider(GmatBase *provider, Real epochInMjd = -999.999)
//------------------------------------------------------------------------------
void EphemerisFile::SetProvider(GmatBase *provider, Real epochInMjd)
{
   Subscriber::SetProvider(provider, epochInMjd);
   HandlePropagatorChange(provider, epochInMjd);
}


//----------------------------------
// methods inherited from GmatBase
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
bool EphemerisFile::Validate()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::Validate() <%p>'%s' entered, fileFormat='%s', spacecraft=<%p>'%s', "
       "outCoordSystem=<%p>'%s'\n", this, GetName().c_str(), fileFormat.c_str(), spacecraft,
       spacecraft ? spacecraft->GetName().c_str() : "NULL", outCoordSystem,
       outCoordSystem ? outCoordSystem->GetName().c_str() : "NULL");
   #endif
   
   ValidateParameters(false);

   // Traps set here are caught on script parsing
   if (outCoordSystem &&
       outCoordSystem->GetAxisSystem()->IsOfType("ObjectReferencedAxes"))
      if (stepSize != "IntegratorSteps")
         throw SubscriberException("Ephemerides written using an "
               "ObjectReferenced coordinate system require writing with the "
               "StepSize parameter set to IntegratorSteps.");

   if (outCoordSystem &&
       outCoordSystem->GetOrigin()->IsOfType("Spacecraft"))
      if (stepSize != "IntegratorSteps")
         throw SubscriberException("Ephemerides written using a "
               "Spacecraft origin based coordinate system require writing with "
               "the StepSize parameter set to IntegratorSteps.");

   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::Validate() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool EphemerisFile::Initialize()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::Initialize() <%p>'%s' entered, spacecraftName='%s', active=%d, "
       "writeEphemeris=%d, isInitialized=%d\n   fileFormat='%s', stateType='%s', "
       "outputFormat='%s', ephemWriter=<%p>\n   fileName='%s'\n   fullPathFileName='%s'\n",
       this, GetName().c_str(), spacecraftName.c_str(), active, writeEphemeris,
       isInitialized, fileFormat.c_str(), stateType.c_str(), outputFormat.c_str(),
       ephemWriter, fileName.c_str(), fullPathFileName.c_str());
   #endif
   
   if (isInitialized)
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("EphemerisFile::Initialize() <%p>'%s' is already initialized so just returning true\n",
          this, GetName().c_str());
      #endif
      return true;
   }
   
   Subscriber::Initialize();

   // Set file type
   if (fileFormat == "CCSDS-OEM")
      fileType = CCSDS_OEM;
   else if (fileFormat == "CCSDS-AEM")
      fileType = CCSDS_AEM;
   else if (fileFormat == "SPK" && stateType == "Cartesian")
      fileType = SPK_ORBIT;
   else if (fileFormat == "SPK" && stateType == "Quaternion")
      fileType = SPK_ATTITUDE;
   else if (fileFormat == "CK" && stateType == "Quaternion")
      fileType = CK_ATTITUDE;
   else if (fileFormat == "Code-500")
      fileType = CODE500_EPHEM;
   else if (fileFormat == "STK-TimePosVel")
      fileType = STK_TIMEPOSVEL;
   else
      throw SubscriberException
         ("FileFormat \"" + fileFormat + "\" is not valid");
   
   // Find out if multiple segments allowed
   allowMultipleSegments = true;
   if (fileType == CODE500_EPHEM)
      allowMultipleSegments = false;
   
   // Do some validation
   ValidateParameters(true);
   
   // Traps set here are caught on script run, so that if set via the GUI we still catch the issue
   if (outCoordSystem &&
       outCoordSystem->GetAxisSystem()->IsOfType("ObjectReferencedAxes"))
      if (stepSize != "IntegratorSteps")
         throw SubscriberException("Ephemerides written using an "
               "ObjectReferenced coordinate system require writing with the "
               "StepSize parameter set to IntegratorSteps.");

   if (outCoordSystem &&
       outCoordSystem->GetOrigin()->IsOfType("Spacecraft"))
      if (stepSize != "IntegratorSteps")
         throw SubscriberException("Ephemerides written using a "
               "Spacecraft origin based coordinate system require writing with "
               "the StepSize parameter set to IntegratorSteps.");

   // If active and not initialized already, open ephemeris file
   if (active && !isInitialized)
      isInitialized = true;
   
   // Initialize data
   InitializeData();
   maneuversHandled.clear();
     
   firstTimeWriting  = true;
   prevPropName = "";
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage("   useFixedStepSize=%d\n", useFixedStepSize);
   #endif
   
   // Set solver iteration option to none. We only writes solutions to a file
   mSolverIterOption = SI_NONE;
   
   // Delete old file to avoid showing old file contents in the GUI output tree
   // when it is toggled off or not writing ephemeris file in the current run.
   if (GmatFileUtil::DoesFileExist(fullPathFileName))
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("   Removing the existing \"%s\" file\n", fullPathFileName.c_str());
      #endif
      remove(fullPathFileName.c_str());
   }
      
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::Initialize() <%p>'%s' returning true, "
       "useFixedStepSize=%d,\n   initialEpochA1Mjd=%.9f, "
       "finalEpochA1Mjd=%.9f, stepSizeInSecs=%.9f\n", this, GetName().c_str(),
       useFixedStepSize, initialEpochA1Mjd, finalEpochA1Mjd,
       stepSizeInSecs);
   #endif

   spacecraft->SetEphemerisFileTypes(fileFormat);
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EphemerisFile.
 *
 * @return clone of the EphemerisFile.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EphemerisFile::Clone(void) const
{
   return (new EphemerisFile(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void EphemerisFile::Copy(const GmatBase* orig)
{
   operator=(*((EphemerisFile *)(orig)));
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool EphemerisFile::TakeAction(const std::string &action,
                               const std::string &actionData)
{
   #ifdef DEBUG_EPHEMFILE_ACTION
   MessageInterface::ShowMessage
      ("EphemerisFile::TakeAction() this=<%p>'%s' entered, action='%s', actionData='%s', "
       "isInitialized=%d, firstTimeWriting=%d\n", this, GetName().c_str(), action.c_str(),
       actionData.c_str(), isInitialized, firstTimeWriting);
   #endif
   
   bool retval = false;
   if (action == "Clear")
   {
      retval = true;
   }
   else if (action == "Finalize")
   {
      // Finish up writing if action is finalize. Finalize action is usually
      // set when function run completes (LOJ: 2015.07.02)
      if (writeEphemeris && isInitialized)
      {
         isEndOfRun = true;
         FinishUpWriting();
      }
      retval = true;
   }
   else if (action == "ToggleOn")
   {
      // Create ephemeris file in case it was initially turned off and toggled on
      if (ephemWriter == NULL)
         CreateEphemerisWriter();
      
      // If it was last toggled off
      if (fileType == CODE500_EPHEM && toggleStatus == 2 && !firstTimeWriting)
         checkForLargeTimeGap = true;
      else
         checkForLargeTimeGap = false;
      
      toggleStatus = 1;
      writeEphemeris = true;
      retval = true;
   }
   else if (action == "ToggleOff")
   {
      // Check if it needs to check for large time gap for Code500
      if (fileType == CODE500_EPHEM && !firstTimeWriting)
         checkForLargeTimeGap = true;
      else
         checkForLargeTimeGap = false;
      
      toggleStatus = 2;
      writeEphemeris = false;
      // If toggle off, finish writing ephemeris and restart interpolation
      //LOJ: Write continuous ephemeris if CODE500_EPHEM or STK_TIMEPOSVEL
      //if (fileType != CODE500_EPHEM && fileType != STK_TIMEPOSVEL)
      if (allowMultipleSegments)
      {
         StartNewSegment("", false, true, true);
      }
      retval = true;
   }
   else if (action == "ChangeTypeName")
   {
      typeName = actionData;
      retval = true;
   }
   
   #ifdef DEBUG_EPHEMFILE_ACTION
   MessageInterface::ShowMessage
      ("EphemerisFile::TakeAction() this=<%p>'%s', action='%s', actionData='%s' "
       "returning %d\n", this, GetName().c_str(), action.c_str(), actionData.c_str(), retval);
   #endif
   return retval;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool EphemerisFile::RenameRefObject(const UnsignedInt type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   if (type != Gmat::SPACECRAFT && type != Gmat::COORDINATE_SYSTEM)
      return true;
   
   if (type == Gmat::SPACECRAFT)
   {
      if (spacecraftName == oldName)
         spacecraftName = newName;
   }
   else if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (outCoordSystemName == oldName)
         outCoordSystemName = newName;
   }
   
   return Subscriber::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string EphemerisFile::GetParameterText(const Integer id) const
{
    if (id >= SubscriberParamCount && id < EphemerisFileParamCount)
        return PARAMETER_TEXT[id - SubscriberParamCount];
    else
        return Subscriber::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer EphemerisFile::GetParameterID(const std::string &str) const
{
   for (Integer i = SubscriberParamCount; i < EphemerisFileParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
      {
         if (i == FILE_NAME)
            WriteDeprecatedMessage(i);
         return i;
      }
   }
   return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType EphemerisFile::GetParameterType(const Integer id) const
{
    if (id >= SubscriberParamCount && id < EphemerisFileParamCount)
        return PARAMETER_TYPE[id - SubscriberParamCount];
    else
        return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string EphemerisFile::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < EphemerisFileParamCount)
      return EphemerisFile::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Subscriber::GetParameterTypeString(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool EphemerisFile::IsParameterReadOnly(const Integer id) const
{
   if (id == SOLVER_ITERATIONS)
      return true;
   if (id == FILE_NAME)
      return true;
   // Disable state type until it is selectable -- currently must be Cartesian
   if (id == STATE_TYPE)
      return true;
   if (id == FULLPATH_FILENAME)
      return true;
   if (id == DISTANCE_UNIT)
      if (fileFormat != "STK-TimePosVel")
         return true;
   if (id == INCLUDE_EVENT_BOUNDARIES)
      if (fileFormat != "STK-TimePosVel")
         return true;

   //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)          // made changes by TUAN NGUYEN
   if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING)     // made changes by TUAN NGUYEN
   {
      // Allow reading and writing when it runs in TESTING mode
      if (id == FILE_FORMAT_VERSION)
         return false;
      if (id == ADDING_ACCELERATION_OPTION)
         return false;
      if (id == ADDING_COVARIANCE_OPTION)
         return false;
   }
   else
   {
      // Allow reading only when it runs in NORMAL mode
      if (id == FILE_FORMAT_VERSION)
         return true;
      if (id == ADDING_ACCELERATION_OPTION)
         return true;
      if (id == ADDING_COVARIANCE_OPTION)
         return true;
   }

   return Subscriber::IsParameterReadOnly(id);
}



//---------------------------------------------------------------------------
//  bool IsParameterCloaked(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is cloaked.
 *
 * @param <id> ID for the parameter.
 *
 * @return true if the parameter is cloaked, false if not
 */
 //---------------------------------------------------------------------------
bool EphemerisFile::IsParameterCloaked(const Integer id) const
{
   //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)          // made changes by TUAN NGUYEN
   if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING)     // made changes by TUAN NGUYEN
   {
      // Allow to show, write, or the script for these parameters in TESTING mode
      if (id == FILE_FORMAT_VERSION ||
         id == ADDING_ACCELERATION_OPTION ||
         id == ADDING_COVARIANCE_OPTION)
         return false;
   }
   else
   {
      // Disable to show, write, or the script for these parameters in NORMAL mode
      if (id == FILE_FORMAT_VERSION ||
         id == ADDING_ACCELERATION_OPTION ||
         id == ADDING_COVARIANCE_OPTION)
         return true;
   }

   return Subscriber::IsParameterCloaked(id);
}


//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool EphemerisFile::IsParameterCommandModeSettable(const Integer id) const
{
   // Override this one from the parent class
   // STEP_SIZE turned off for GMT-8111
   if (id == FILENAME || id == WRITE_EPHEMERIS)
      return true;

   if (id == OUTPUT_FORMAT)
      return false;
   if (id == INCLUDE_COVARIANCE)
      return false;

   // And let the parent class handle its own
   return Subscriber::IsParameterCommandModeSettable(id);
}

//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
UnsignedInt EphemerisFile::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case SPACECRAFT:
      return Gmat::SPACECRAFT;
   case INTERPOLATOR:
      return Gmat::INTERPOLATOR;
   case COORDINATE_SYSTEM:
      return Gmat::COORDINATE_SYSTEM;
   default:
      return Subscriber::GetPropertyObjectType(id);
   }
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves eumeration symbols of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return list of enumeration symbols
 */
//---------------------------------------------------------------------------
const StringArray& EphemerisFile::GetPropertyEnumStrings(const Integer id) const
{
   switch (id)
   {
   case FILE_FORMAT:
      return fileFormatList;
   case EPOCH_FORMAT:
      return epochFormatList;
   case INITIAL_EPOCH:
      return initialEpochList;
   case FINAL_EPOCH:
      return finalEpochList;
   case STEP_SIZE:
      return stepSizeList;
   case STATE_TYPE:
      return stateTypeList;
   case INTERPOLATOR:
      return interpolatorTypeList;
   case OUTPUT_FORMAT:
      return outputFormatList;
   case INCLUDE_COVARIANCE:
      return covFormatList;
   case DISTANCE_UNIT:
      return distanceUnitList;
   default:
      return Subscriber::GetPropertyEnumStrings(id);
   }
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool EphemerisFile::GetBooleanParameter(const Integer id) const
{
   switch (id)
   {
   case WRITE_EPHEMERIS:
      return writeEphemeris;
   case INCLUDE_EVENT_BOUNDARIES:
      return includeEventBoundaries;

   case ADDING_ACCELERATION_OPTION:
      return addingAccelerationOption;
   case ADDING_COVARIANCE_OPTION:
      return addingCovarianceOption;
   
   default:
      return Subscriber::GetBooleanParameter(id);
   }
}


//------------------------------------------------------------------------------
//  bool  SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> bool value for the requested parameter.
 *
 * @return  the boolean set.
 */
 //------------------------------------------------------------------------------
bool EphemerisFile::SetBooleanParameter(const Integer id, const bool value)
{
   switch (id)
   {
   case WRITE_EPHEMERIS:
      writeEphemeris = value;
      active = value;
      return writeEphemeris;
   case INCLUDE_EVENT_BOUNDARIES:
      includeEventBoundaries = value;
      return includeEventBoundaries;

   case ADDING_ACCELERATION_OPTION:
   {
      //MessageInterface::ShowMessage("********   ADDING_ACCELERATION_OPTION:  value = %s   GetRunMode() = %d\n", (value?"true":"false"), GmatGlobal::Instance()->GetRunMode());
      //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)       // made changes by TUAN NGUYEN
      if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING)  // made changes by TUAN NGUYEN
      {
         addingAccelerationOption = value;
         return addingAccelerationOption;
      }
   }
   case ADDING_COVARIANCE_OPTION:
   {
      //MessageInterface::ShowMessage("********   ADDING_COVARIANCE_OPTION:  value = %s   GetRunMode() = %d\n", (value ? "true" : "false"), GmatGlobal::Instance()->GetRunMode());
      //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)            // made changes by TUAN NGUYEN
      if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING)       // made changes by TUAN NGUYEN
      {
         addingCovarianceOption = value;
         return addingCovarianceOption;
      }
   }
   default:
      return Subscriber::SetBooleanParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer EphemerisFile::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case INTERPOLATION_ORDER:
      return interpolationOrder;
   default:
      return Subscriber::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer EphemerisFile::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
   case INTERPOLATION_ORDER:
   {
      bool violatesHermiteOddness = false;
      if (interpolatorName == "Hermite")
      {
         // Make sure the number is odd
         Integer roundTrip = (Integer)((value / 2) * 2);
         if (roundTrip == value)  // Number is even
            violatesHermiteOddness = true;
      }

      if ((value >= 1 && value <= 10) && !violatesHermiteOddness)
      {
         interpolationOrder = value;
         return interpolationOrder;
      }
      else
      {
         SubscriberException se;
         if (interpolatorName == "Hermite")
         {
            se.SetDetails(errorMessageFormat.c_str(),
                          GmatStringUtil::ToString(value, 1).c_str(),
                          GetParameterText(INTERPOLATION_ORDER).c_str(),
                          "1 <= Odd Integer Number <= 10");
         }
         else
         {
            se.SetDetails(errorMessageFormat.c_str(),
                          GmatStringUtil::ToString(value, 1).c_str(),
                          GetParameterText(INTERPOLATION_ORDER).c_str(),
                          "1 <= Integer Number <= 10");
         }
         throw se;
      }
   }
   default:
      return Subscriber::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string EphemerisFile::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case SPACECRAFT:
      return spacecraftName;
   case FILENAME:
      return fileName;
   case FULLPATH_FILENAME:
      return fullPathFileName;
   case FILE_FORMAT:
      return fileFormat;
   case FILE_FORMAT_VERSION:
      return fileFormatVersion;
   case EPOCH_FORMAT:
      return epochFormat;
   case INITIAL_EPOCH:
      return initialEpochStr;
   case FINAL_EPOCH:
      return finalEpochStr;
   case STEP_SIZE:
      return stepSize;
   case INTERPOLATOR:
      return interpolatorName;
   case STATE_TYPE:
      return stateType;
   case COORDINATE_SYSTEM:
      return outCoordSystemName;
   case OUTPUT_FORMAT:
      return outputFormat;
   case INCLUDE_COVARIANCE:
      return covFormat;
   case FILE_NAME:
      WriteDeprecatedMessage(id);
      return fileName;
   case DISTANCE_UNIT:
      return distanceUnit;
   default:
      return Subscriber::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string EphemerisFile::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool EphemerisFile::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisFile::SetStringParameter() this=<%p>'%s' entered, id=%d, "
       "value='%s'\n   isInitialized=%d, firstTimeWriting=%d, fileFormat='%s'\n",
       this, GetName().c_str(), id, value.c_str(), isInitialized, firstTimeWriting,
       fileFormat.c_str());
   #endif

   // Write deprecated message for each once per GMAT session
   static bool pcFormatFirstWarning = true;
   static bool unixFormatFirstWarning = true;
   
   switch (id)
   {
   case SPACECRAFT:
      spacecraftName = value;
      return true;
   case FILENAME:
   {
      #ifdef DEBUG_EPHEMFILE_SET
      MessageInterface::ShowMessage
         ("EphemerisFile::SetStringParameter() Setting Filename '%s' to "
          "EphemerisFile '%s', isInitialized=%d\n", value.c_str(), instanceName.c_str(),
          isInitialized);
      #endif
      
      // Validate filename
      if (!GmatFileUtil::IsValidFileName(value, true))
      {
         std::string msg = GmatFileUtil::GetInvalidFileNameMessage(1);
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(), value.c_str(), "Filename", msg.c_str());
         throw se;
      }
      
      // If already initialized, close ephemeris file first
      if (isInitialized)
      {
         #ifdef DEBUG_EPHEMFILE_INIT
         MessageInterface::ShowMessage
            ("EphemerisFile::SetStringParameter() <%p>'%s' is already initialized "
             "so finish it first, if ephemeris file is opened, ephemWriter=<%p>\n", this,
             GetName().c_str(), ephemWriter);
         #endif
         // if (isEphemFileOpened)
         if (ephemWriter)
         {
            isEphemFileOpened = ephemWriter->IsEphemFileOpened();
            #ifdef DEBUG_EPHEMFILE_INIT
            MessageInterface::ShowMessage
               ("ephemWriter->IsEphemFileOpened()=%d\n", isEphemFileOpened);
            #endif
            if (isEphemFileOpened)
            {
               #ifdef DEBUG_EPHEMFILE_INIT
               MessageInterface::ShowMessage
                  ("EphemerisFile::SetStringParameter() calling FinishUpWriting()\n");
               #endif
               FinishUpWriting();
            }
         }
      }
      
      prevFileName = fileName;
      fileName = value;
      fullPathFileName =
         GmatBase::GetFullPathFileName(fileName, GetName(), fileName, "EPHEM_OUTPUT_FILE",
                                       false, ".eph", false, true);
      
      // Check for directory name - MUST check for full path directory
      std::string dirName = GmatFileUtil::ParsePathName(fullPathFileName);
      #ifdef DEBUG_EPHEMFILE_SET
      MessageInterface::ShowMessage("   dirName = '%s'\n", dirName.c_str());
      #endif
      
      if (dirName != "")
      {
         if (!GmatFileUtil::DoesDirectoryExist(dirName))
         {
            SubscriberException se;
            se.SetDetails("Cannot create an EphemerisFile with non-existing directory '%s'",
                          dirName.c_str());
            throw se;
         }
      }
      
      // If filename is set in resource mode, set prevFileName
      if (!isInitialized)
         prevFileName = fileName;
      
      // Set new name to EphemerisWriter
      SetFileName();
      
      usingDefaultFileName = false;
      #ifdef DEBUG_EPHEMFILE_SET
      MessageInterface::ShowMessage
         ("       prevFileName='%s'\n           fileName='%s'\n   fullPathFileName='%s'\n",
          prevFileName.c_str(), fileName.c_str(), fullPathFileName.c_str());
      MessageInterface::ShowMessage
         ("EphemerisFile::SetStringParameter() this=<%p>'%s' returning true\n",
          this, GetName().c_str());
      #endif
      return true;
   }
   // Interpolator is now set along with file format (bug 2219)
   case FILE_FORMAT:
      if (find(fileFormatList.begin(), fileFormatList.end(), value) !=
          fileFormatList.end())
      {
         fileFormat = value;
         
         // Code to link interpolator selection to file type
         if (fileFormat == "CCSDS-OEM" || fileFormat == "STK-TimePosVel")
            interpolatorName = "Lagrange";
         else if (fileFormat == "SPK")
            interpolatorName = "Hermite";
         else if (fileFormat == "CK")
            interpolatorName = "Lagrange";
         else if (fileFormat == "Code-500")
         {
            interpolatorName = "Lagrange";
            // Also set default fixed step size to 60.0
            stepSize = "60.0";
            useFixedStepSize = true;
         }
         return true;
      }
      else
      {
         HandleError(FILE_FORMAT, value, fileFormatList);
      }
   case FILE_FORMAT_VERSION:
      ///@todo: check for testing mode will be removed when all CCSDS OEM v2 test cases are passed.
      //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)            // made changes by TUAN NGUYEN
      if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING)       // made changes by TUAN NGUYEN
      {
         if (value != "1.0" && value != "2.0")
         {
            SubscriberException se;
            se.SetDetails("The Only Permitted CCSDS-File Format Values are \"1.0\" and \"2.0\"");
            throw se;
         }
         fileFormatVersion = value;
         return true;
      }
      else
         return false;
      break;
   case EPOCH_FORMAT:
      if (find(epochFormatList.begin(), epochFormatList.end(), value) !=
          epochFormatList.end())
      {
         epochFormat = value;
         return true;
      }
      else
      {
         HandleError(EPOCH_FORMAT, value, epochFormatList);
      }
   case INITIAL_EPOCH:
      if (find(initialEpochList.begin(), initialEpochList.end(), value) !=
          initialEpochList.end())
      {
         initialEpochStr = value;
         return true;
      }
      else
      {
         return SetEpoch(INITIAL_EPOCH, value, initialEpochList);
      }
   case FINAL_EPOCH:
      if (find(finalEpochList.begin(), finalEpochList.end(), value) !=
          finalEpochList.end())
      {
         finalEpochStr = value;
         return true;
      }
      else
      {
         return SetEpoch(FINAL_EPOCH, value, finalEpochList);
      }
   case STEP_SIZE:
      if (find(stepSizeList.begin(), stepSizeList.end(), value) !=
          stepSizeList.end())
      {
         stepSize = value;
         useFixedStepSize = false;
         return true;
      }
      else
      {
         return SetStepSize(STEP_SIZE, value, stepSizeList);
      }
   // Interpolator is now set along with file format (bug 2219); if the parm is
   // passed in, just ensure compatibility
   case INTERPOLATOR:
      if (fileFormat == "CCSDS-OEM" || fileFormat == "Code-500" ||
          fileFormat == "STK-TimePosVel" || fileFormat == "CK")
      {
         //if (value != "Lagrange")                                                    // made changes by TUAN NGUYEN 
         if ((value != "Lagrange") && (value != "Linear") && (value != "Hermite"))     // made changes by TUAN NGUYEN
            throw SubscriberException("Cannot set interpolator \"" + value +
                  "\" on the EphemerisFile named \"" + instanceName + "\"; " +
                  fileFormat + " ephemerides require \"Lagrange\" interpolators");

         interpolatorName = value;                                                     // made changes by TUAN NGUYEN
      }
      else if (fileFormat == "SPK")
      {
         if (value != "Hermite")
            throw SubscriberException("Cannot set interpolator \"" + value +
                  "\" on the EphemerisFile named \"" + instanceName +
                  "\"; SPK ephemerides require \"Hermite\" interpolators");

         interpolatorName = value;                                                     // made changes by TUAN NGUYEN
      }
      else
      {
         SubscriberException se;
         se.SetDetails("The interpolator \"%s\" on the EphemerisFile named \"%s\" "
                       "cannot be set; File format \"%s\" is unknown format\n",
                       value.c_str(), instanceName.c_str(), fileFormat.c_str());
         throw se;
         // throw SubscriberException("The interpolator \"" + value +
         //       "\" on the EphemerisFile named \"" + instanceName +
         //       "\" cannot be set; set the file format to set the interpolator");
      }
      return true;
   case STATE_TYPE:
      if (find(stateTypeList.begin(), stateTypeList.end(), value) !=
          stateTypeList.end())
      {
         stateType = value;
         return true;
      }
      else
      {
         HandleError(STATE_TYPE, value, stateTypeList);
      }
   case COORDINATE_SYSTEM:
      outCoordSystemName = value;
      return true;
   case OUTPUT_FORMAT:
      if (find(outputFormatList.begin(), outputFormatList.end(), value) !=
          outputFormatList.end())
      {
         outputFormat = value;
         return true;
      }
	   else if (value == "PC")
	   {
         if (pcFormatFirstWarning)
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** \"PC\" option for OutputFormat in EphemerisFile "
                  "is deprecated and will be removed from a future build; "
                  "please use \"LittleEndian\" instead.\n");
            pcFormatFirstWarning = false;
         }
         
         outputFormat = "LittleEndian";
         return true;
	   }
	   else if (value == "UNIX")
	   {
         if (unixFormatFirstWarning)
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** \"UNIX\" option for OutputFormat in EphemerisFile "
                  "is deprecated and will be removed from a future build; "
                  "please use \"BigEndian\" instead.\n");
            unixFormatFirstWarning = false;
         }

         outputFormat = "BigEndian";
         return true;
	   }
      else
      {
         HandleError(OUTPUT_FORMAT, value, outputFormatList);
      }
   case INCLUDE_COVARIANCE:
      if (find(covFormatList.begin(), covFormatList.end(), value) !=
         covFormatList.end())
      {
         covFormat = value;
         return true;
      }
      else
      {
         HandleError(INCLUDE_COVARIANCE, value, covFormatList);
      }
   case FILE_NAME:
      WriteDeprecatedMessage(id);
      return SetStringParameter(FILENAME, value);
   case DISTANCE_UNIT:
      if (find(distanceUnitList.begin(), distanceUnitList.end(), value) !=
          distanceUnitList.end())
      {
         distanceUnit = value;
         return true;
      }
      else
      {
         HandleError(DISTANCE_UNIT, value, distanceUnitList);
      }
   default:
      return Subscriber::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool EphemerisFile::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* EphemerisFile::GetRefObject(const UnsignedInt type,
                                      const std::string &name)
{
   if (type == Gmat::SPACECRAFT)
      return spacecraft;
   
   if (type == Gmat::COORDINATE_SYSTEM)
      return outCoordSystem;
   
   return Subscriber::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
bool EphemerisFile::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                 const std::string &name)
{
   #ifdef DEBUG_EPHEMFILE_REF_OBJ
   MessageInterface::ShowMessage
      ("EphemerisFile::SetRefObject() <%p>'%s' entered, obj=%p, name=%s, objtype=%s, "
       "objname=%s\n", this, GetName().c_str(), obj, name.c_str(), obj->GetTypeName().c_str(),
       obj->GetName().c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   if (type == Gmat::SPACECRAFT && name == spacecraftName)
   {
      spacecraft = (Spacecraft*)obj;
      if (spacecraft)
      {
         spacecraftId = spacecraft->GetStringParameter("Id");
         if (ephemWriter)
            ephemWriter->SetSpacecraft(spacecraft);
      }
      return true;
   }
   else if (type == Gmat::COORDINATE_SYSTEM && name == outCoordSystemName)
   {
      outCoordSystem = (CoordinateSystem*)obj;
      if (outCoordSystem)
         if (ephemWriter)
            ephemWriter->SetOutCoordSystem(outCoordSystem);
      
      return true;
   }
   
   return Subscriber::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& EphemerisFile::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();
   
   if (type == Gmat::SPACECRAFT || type == Gmat::UNKNOWN_OBJECT)
      refObjectNames.push_back(spacecraftName);
   
   if (type == Gmat::COORDINATE_SYSTEM || type == Gmat::UNKNOWN_OBJECT)
      refObjectNames.push_back(outCoordSystemName);
   
   return refObjectNames;
}


//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
// Real ConvertInitialAndFinalEpoch()
//------------------------------------------------------------------------------
/**
 * Converts initial and final epoch to A1ModJulian time format since spacecraft
 * uses this time format
 *
 * @return Spacecraft initial epoch
 * @throws SubscriberException if initial epoch is before spacecraft initial epoch
 */
//------------------------------------------------------------------------------
Real EphemerisFile::ConvertInitialAndFinalEpoch()
{
   #ifdef DEBUG_CONVERT_INITIAL_FINAL_EPOCH
   MessageInterface::ShowMessage
      ("EphemerisFile::ConvertInitialAndFinalEpoch() <%p>'%s' entered, epochFormat='%s'\n"
       "     initialEpochStr = '%s'\n       finalEpochStr = '%s'\n", this, GetName().c_str(),
       epochFormat.c_str(), initialEpochStr.c_str(), finalEpochStr.c_str());
   #endif
   
   // Convert initial and final epoch to A1ModJulian format if needed.
   // Currently spacecraft uses A1ModJulian as output epoch
   Real dummyA1Mjd = -999.999;
   std::string epochStr;
   
   // Convert initial epoch to A1Mjd
   if (initialEpochStr != "InitialSpacecraftEpoch")
      TimeSystemConverter::Instance()->Convert(epochFormat, dummyA1Mjd, initialEpochStr,
                                 "A1ModJulian", initialEpochA1Mjd, epochStr);
   
   // Convert final epoch to A1Mjd
   if (finalEpochStr != "FinalSpacecraftEpoch")
      TimeSystemConverter::Instance()->Convert(epochFormat, dummyA1Mjd, finalEpochStr,
                                 "A1ModJulian", finalEpochA1Mjd, epochStr);
   
   // Check if ephemeris initial epoch is before the spacecraft initial epoch
   Real satInitialEpoch = spacecraft->GetEpoch();
   if ((initialEpochStr != "InitialSpacecraftEpoch") && initialEpochA1Mjd < satInitialEpoch)
   {
      SubscriberException se;
      se.SetDetails("Initial epoch (%f) of ephemeris file \"%s\" cannot be before "
                    "initial epoch (%f) of spacecraft \"%s\"\n", initialEpochA1Mjd,
                    GetName().c_str(), satInitialEpoch, spacecraft->GetName().c_str());
      throw se;
   }
   
   // Check if ephemeris final epoch is greater than initial epoch
   if ((initialEpochStr != "InitialSpacecraftEpoch") && (finalEpochStr != "FinalSpacecraftEpoch"))
   {
      if (initialEpochA1Mjd > finalEpochA1Mjd)
      {
         SubscriberException se;
         se.SetDetails("Initial epoch (%f) of ephemeris file \"%s\" cannot be greater than "
                       "final epoch (%f)\n", initialEpochA1Mjd, GetName().c_str(),
                       finalEpochA1Mjd);
         throw se;
      }
   }
   
   #ifdef DEBUG_CONVERT_INITIAL_FINAL_EPOCH
   MessageInterface::ShowMessage
      ("   initialEpochA1Mjd = %.15f\n     finalEpochA1Mjd = %.15f\n     satInitialEpoch = %.15f\n",
       initialEpochA1Mjd, finalEpochA1Mjd, satInitialEpoch);
   MessageInterface::ShowMessage
      ("EphemerisFile::ConvertInitialAndFinalEpoch() <%p>'%s' returning\n", this,
       GetName().c_str());
   #endif
   return satInitialEpoch;
}


//------------------------------------------------------------------------------
// void ValidateParameters(bool forInitialization)
//------------------------------------------------------------------------------
void EphemerisFile::ValidateParameters(bool forInitialization)
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::ValidateParameters() <%p>'%s' entered, forInitialization=%d\n",
       this, GetName().c_str(), forInitialization);
   #endif
   
   if (fileFormat == "SPK")
   {
      if (stateType == "Quaternion")
         throw SubscriberException
            ("Currently GMAT only supports writing orbit states in SPK format");
      
      if (interpolatorName == "Hermite" && GmatMathUtil::IsEven(interpolationOrder))
         throw SubscriberException
            ("The SPK file interpolation order must be an odd number when using "
             "Hermite interpolator");
   }
   else
   {
      // check for FileFormat and StateType
      if ((fileFormat == "CCSDS-OEM" && stateType == "Quaternion") ||
          (fileFormat == "CCSDS-AEM" && stateType == "Cartesian") ||
          (fileFormat == "Code-500" && stateType == "Quaternion"))
         throw SubscriberException
            ("FileFormat \"" + fileFormat + "\" and StateType " + "\"" + stateType +
             "\" does not match for the EphemerisFile \"" + GetName() + "\"");
      
      // check interpolator type
      if (stepSize != "IntegratorSteps")
      {
         // check for StateType Cartesian and Interpolator
         if (stateType == "Cartesian" && interpolatorName != "Lagrange")
            throw SubscriberException
               ("The Interpolator must be \"Lagrange\" for StateType of \"Cartesian\" for "
                "the EphemerisFile \"" + GetName() + "\"");
         
         // check for StateType Quaternion and Interpolator
         if (stateType == "Quaternion" && interpolatorName != "SLERP")
            throw SubscriberException
               ("The Interpolator must be \"SLERP\" for StateType of \"Quaternion\" for "
                "the EphemerisFile \"" + GetName() + "\"");
      }
      
      if (fileFormat == "Code-500")
      {
         if (!useFixedStepSize)
            throw SubscriberException
               ("Code-500 ephemeris file \"" + GetName() + "\" requires fixed step size");
         
         // Give default step size for code-500
         if (stepSize == "IntegratorSteps")
         {
            stepSize = "60";
            stepSizeInSecs = 60.0;
         }
      }
   }
   
   // By this time, coordinate system should not be NULL, so check it
   if (outCoordSystem == NULL)
      throw SubscriberException
         ("The CoordinateSystem \"" + outCoordSystemName + "\" has not been set for "
          "the EphemerisFile \"" + GetName() + "\"");
   
   // Do some validation
   if (fileFormat == "SPK")
   {
      if (!outCoordSystem->AreAxesOfType("MJ2000EqAxes"))
      {
         SubscriberException se;
         se.SetDetails("%s ephemeris file \"%s\" only allows coordinate system "
            "with MJ2000Eq Axis", fileFormat.c_str(), GetName().c_str());
         throw se;
      }
   }

   if (fileFormat == "Code-500")
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("   outCoordSystem=<%p>'%s'\n", outCoordSystem, outCoordSystem->GetName().c_str());
      #endif
      if (!outCoordSystem->AreAxesOfType("MJ2000EqAxes") && 
          !outCoordSystem->AreAxesOfType("BodyFixedAxes") && 
          !outCoordSystem->AreAxesOfType("TrueOfDateAxes"))
      {
         SubscriberException se;
         se.SetDetails("%s ephemeris file \"%s\" only allows coordinate system "
                       "with MJ2000Eq Axis", fileFormat.c_str(), GetName().c_str());
         throw se;
      }
      
      // Check for valid central body for Code500 ephem
      // 1=Earth, 2=Luna(Earth Moon), 3=Sun, 4=Mars, 5=Jupiter, 6=Saturn, 7=Uranus,
      // 8=Neptune, 9=Pluto, 10=Mercury, 11=Venus
      if (fileFormat == "Code-500")
      {
         std::string origin = outCoordSystem->GetOriginName();
         if (origin != "Earth" && origin != "Luna" && origin != "Sun" &&
             origin != "Mars" && origin != "Jupiter" && origin != "Saturn" &&
             origin != "Uranus" && origin != "Neptune" && origin != "Pluto" &&
             origin != "Mercury" && origin != "Venus")
         {
            SubscriberException se;
            se.SetDetails("%s ephemeris file \"%s\" only allows coordinate system "
                          "with Sun, Planet or Luna(Earth Moon) origin", fileFormat.c_str(),
                          GetName().c_str());
            throw se;
         }
      }
   }
   
   // Catch invalid initial epoch early if spacecraft is not NULL
   if (spacecraft != NULL)
      ConvertInitialAndFinalEpoch();
   
   // If for initialization, all other pointers should have been set, so check it
   if (forInitialization)
   {
      // check for NULL pointers
      if (spacecraft == NULL)
         throw SubscriberException
            ("The Spacecraft \"" + spacecraftName + "\" has not been set for "
             "the EphemerisFile \"" + GetName() + "\"");
      
      if (theDataCoordSystem == NULL)
         throw SubscriberException
            ("The internal CoordinateSystem which orbit data represents has not been set for "
             "the EphemerisFile \"" + GetName() + "\"");
   }
   
   if (forInitialization)
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("   spacecraft=<%p>'%s', outCoordSystem=<%p>'%s', theDataCoordSystem=<%p>'%s'\n",
          spacecraft, spacecraft->GetName().c_str(), outCoordSystem,
          outCoordSystem->GetName().c_str(), theDataCoordSystem,
          theDataCoordSystem->GetName().c_str());
      #endif
   }
   
   // if (ephemWriter)
   //    ephemWriter->ValidateParameters(forInitialization);
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::ValidateParameters() <%p>'%s' leaving\n", this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// void InitializeData(bool saveEpochInfo = true)
//------------------------------------------------------------------------------
void EphemerisFile::InitializeData(bool saveEpochInfo)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisFile::InitializeData() <%p>'%s' entered, saveEpochInfo=%d, "
       "ephemWriter=<%p>\n", this, GetName().c_str(), saveEpochInfo, ephemWriter);
   #endif
   
   if (!saveEpochInfo)
   {
      currEpochInDays = -999.999;
      currEpochInSecs = -999.999;
   }
   
   blockBeginA1Mjd  = -999.999;
   prevEpochInSecs  = -999.999;
   eventEpochInSecs = -999.999;
   
   if (ephemWriter)
      ephemWriter->InitializeData(saveEpochInfo);
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisFile::InitializeData() <%p>'%s' leaving, currEpochInSecs=%.15f, "
       "prevEpochInSecs=%.15f\n", this, GetName().c_str(), currEpochInSecs, prevEpochInSecs);
   #endif
}

//------------------------------------------------------------------------------
// void SetFileName()
//------------------------------------------------------------------------------
void EphemerisFile::SetFileName()
{
   if (ephemWriter)
      ephemWriter->SetFileName(fileName, fullPathFileName, prevFileName);
}

//------------------------------------------------------------------------------
// void CreateEphemerisFile()
//------------------------------------------------------------------------------
void EphemerisFile::CreateEphemerisFile()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateEphemerisFile() <%p>'%s' entered, usingDefaultFileName=%d\n"
       "   stateType='%s', outputFormat='%s', fileFormat='%s'\n", this, GetName().c_str(),
       usingDefaultFileName, stateType.c_str(), outputFormat.c_str(), fileFormat.c_str());
   #endif
   
   if (ephemWriter)
   {
      ephemWriter->CreateEphemerisFile(usingDefaultFileName, stateType, outputFormat, covFormat);
      isEphemFileOpened = ephemWriter->IsEphemFileOpened();
   }
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateEphemerisFile() <%p>'%s' leaving\n", this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void CreateEphemerisWriter()
//------------------------------------------------------------------------------
void EphemerisFile::CreateEphemerisWriter()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateEphemerisWriter() <%p>'%s' entered, ephemWriter=<%p>, "
       "fileFormat='%s',\n   fileName='%s'\n   fullPathFileName='%s'\n", this,
       GetName().c_str(), ephemWriter, fileFormat.c_str(), fileName.c_str(),
       fullPathFileName.c_str());
   #endif
   
   if (ephemWriter)
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("EphemerisFile::CreateEphemerisWriter() '%s' just leaving, ephemWriter=<%p>, "
          "fileFormat='%s'\n", GetName().c_str(), ephemWriter, fileFormat.c_str());
      #endif
      return;
   }
   
   // Create appropriate EphemerisFile writer
   if (fileFormat == "CCSDS-OEM")
      ephemWriter = new EphemWriterCCSDS(GetName(), fileFormat);
   else if (fileFormat == "SPK")
      ephemWriter = new EphemWriterSPK(GetName(), fileFormat);
   else if (fileFormat == "CK")
      ephemWriter = new AttitudeWriterCK(GetName(), fileFormat);
   else if (fileFormat == "Code-500")
      ephemWriter = new EphemWriterCode500(GetName(), fileFormat);
   else if (fileFormat == "STK-TimePosVel")
   {
      ephemWriter = new EphemWriterSTK(GetName(), fileFormat);
      ((EphemWriterSTK*)ephemWriter)->SetDistanceUnit(distanceUnit);
      ((EphemWriterSTK*)ephemWriter)->SetIncludeEventBoundaries(includeEventBoundaries);
   }
   else
   {
      SubscriberException se;
      se.SetDetails("Cannot create an EphemerisFile with unknown file format of '%s'\n",
                    fileFormat.c_str());
      throw se;
   }

   ephemWriter->SetEphemerisFormatVersion(fileFormatVersion);
   ephemWriter->SetAddingAccelerationOption(addingAccelerationOption);
   ephemWriter->SetAddingCovarianceOption(addingCovarianceOption);

   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage("   ephemWriter<%p> created\n", ephemWriter);
   MessageInterface::ShowMessage
      ("   Calling ephemWriter to set objects, create EphemerisFile, and to initialize\n");
   #endif
   GetProperFileName(fileName, fileFormat, true);
   SetFileName();
   ephemWriter->SetSpacecraft(spacecraft);
   ephemWriter->SetDataCoordSystem(theDataCoordSystem);
   ephemWriter->SetOutCoordSystem(outCoordSystem);
   ephemWriter->SetInitialData(initialEpochStr, finalEpochStr, stepSize, stepSizeInSecs,
                               useFixedStepSize, interpolatorName, interpolationOrder);
   ephemWriter->SetInitialTime(initialEpochA1Mjd, finalEpochA1Mjd);
   ephemWriter->SetIsEphemGlobal(IsGlobal());
   ephemWriter->Initialize();
   CreateEphemerisFile();
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateEphemerisWriter() <%p>'%s' leaving, ephemWriter=<%p>, "
       "fileFormat='%s',\n   fileName='%s'\n", this, GetName().c_str(), ephemWriter,
       fileFormat.c_str(), fileName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void CloseEphemerisFile(bool done = true, writeMetaData = true)
//------------------------------------------------------------------------------
void EphemerisFile::CloseEphemerisFile(bool done, bool writeMetaData)
{
   if (ephemWriter)
      ephemWriter->CloseEphemerisFile(done, writeMetaData);
}


//------------------------------------------------------------------------------
// bool InsufficientDataPoints()
//------------------------------------------------------------------------------
bool EphemerisFile::InsufficientDataPoints()
{
   if (ephemWriter)
      return ephemWriter->InsufficientDataPoints();
   else
      return false;
}


//------------------------------------------------------------------------------
// virtual void HandleOrbitData()
//------------------------------------------------------------------------------
void EphemerisFile::HandleOrbitData()
{
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleOrbitData() <%p> entered\n", this);
   #endif

   canFinalize = false;
   if (ephemWriter)
   {
      ephemWriter->SetRunFlags(canFinalize, isEndOfRun, isFinalized);
      ephemWriter->HandleOrbitData();
      if (firstTimeWriting)
         firstTimeWriting = false;
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_ORBIT
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleOrbitData() ephemWriter is NULL\n");
      #endif
   }
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleOrbitData() <%p> leaving\n", this);
   #endif
}

//------------------------------------------------------------------------------
// virtual void StartNewSegment(const std::string &comments,
//                              bool saveEpochInfo,
//                              bool writeAfterData,
//                              bool ignoreBlankComments)
//------------------------------------------------------------------------------
void EphemerisFile::StartNewSegment(const std::string &comments,
                                    bool saveEpochInfo, bool writeAfterData,
                                    bool ignoreBlankComments)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("\nEphemerisFile::StartNewSegment() <%p>'%s' entered, ephemWriter=<%p>, "
       "comments='%s'\n", this, GetName().c_str(), ephemWriter, comments.c_str());
   #endif
   
   canFinalize = false;
   if (ephemWriter)
   {
      ephemWriter->SetRunFlags(canFinalize, isEndOfRun, isFinalized);
      ephemWriter->StartNewSegment(comments, saveEpochInfo, writeAfterData,
                                   ignoreBlankComments);
   }
   // Initialize data
   InitializeData(saveEpochInfo);
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("EphemerisFile::StartNewSegment() <%p>'%s' leaving, ephemWriter=<%p>, "
       "saveEpochInfo=%d\n   comments='%s'\n\n", this, GetName().c_str(),
       ephemWriter, saveEpochInfo, comments.c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void FinishUpWriting()
//------------------------------------------------------------------------------
void EphemerisFile::FinishUpWriting()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemerisFile::FinishUpWriting() <%p>'%s' entered, ephemWriter=<%p>\n",
       this, GetName().c_str(), ephemWriter);
   #endif
   
   canFinalize = true;
   if (ephemWriter)
   {
      ephemWriter->SetRunFlags(canFinalize, isEndOfRun, isFinalized);
      ephemWriter->FinishUpWriting();
      isFinalized = ephemWriter->IsFinalized();
   }
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemerisFile::FinishUpWriting() <%p>'%s' leaving, ephemWriter=<%p>\n",
       this, GetName().c_str(), ephemWriter);
   #endif
}

//------------------------------------------------------------------------------
// bool HandleEndOfRun()
//------------------------------------------------------------------------------
bool EphemerisFile::HandleEndOfRun()
{
   // End of run, blank out the last comment if needed
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("=====> End of run\n");
   std::string currEpochStr = ToUtcGregorian(currEpochInSecs);
   std::string prevEpochStr = ToUtcGregorian(prevEpochInSecs);
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleEndOfRun() this=<%p>'%s' entered, isEndOfReceive=%d, "
       "isEndOfDataBlock=%d, isEndOfRun=%d, firstTimeWriting=%d\n"
       "   currEpochInSecs=%s, prevEpochInSecs=%s, ephemWriter=<%p>\n",
       this, GetName().c_str(), isEndOfReceive, isEndOfDataBlock, isEndOfRun,
       firstTimeWriting, currEpochStr.c_str(), prevEpochStr.c_str(), ephemWriter);
   #endif
   
   canFinalize = true;
   if (ephemWriter)
   {
      ephemWriter->SetRunFlags(canFinalize, isEndOfRun, isFinalized);
      ephemWriter->HandleEndOfRun();
      isFinalized = ephemWriter->IsFinalized();
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleEndOfRun() this=<%p>'%s' returning true, "
          "ephemWriter is NULL\n", this, GetName().c_str());
      #endif
      return true;
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleEndOfRun() this=<%p>'%s' returning false, "
          "ephemWriter is NULL\n", this, GetName().c_str());
      #endif
      return false;
   }
}


//------------------------------------------------------------------------------
// bool SkipFunctionData()
//------------------------------------------------------------------------------
bool EphemerisFile::SkipFunctionData()
{
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::SkipFunctionData() <%p>'%s' entered, Data is published from the function, "
       "IsGlobal:%s, IsLocal:%s\n", this, GetName().c_str(), IsGlobal() ? "Yes" : "No",
       IsLocal() ? "Yes" : "No");
   #endif
   
   bool skipData = false;
   
   if (ephemWriter)
      skipData = ephemWriter->SkipFunctionData();
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::SkipFunctionData() returning %d\n", skipData);
   #endif
   
   return skipData;
}


//------------------------------------------------------------------------------
// bool RetrieveData(const Real *dat, Integer len)
//------------------------------------------------------------------------------
bool EphemerisFile::RetrieveData(const Real *dat, Integer len)
{
   StringArray dataLabels = theDataLabels[0];
   
   #ifdef DEBUG_EPHEMFILE_DATA_LABELS
   MessageInterface::ShowMessage
      ("EphemerisFile::RetrieveData() entered, Data labels for %s =\n   ",
       GetName().c_str());
   MessageInterface::ShowMessage("dataLabels.size() = %d\n", dataLabels.size());
   for (int j=0; j<(int)dataLabels.size(); j++)
      MessageInterface::ShowMessage("%s ", dataLabels[j].c_str());
   MessageInterface::ShowMessage("\n");
   #endif

   std::string names[6] = { "X", "Y", "Z", "Vx", "Vy", "Vz" };
   
   Integer idX, idY, idZ;
   Integer idVx, idVy, idVz;
   Integer idAx, idAy, idAz;
   Integer idQ1, idQ2, idQ3, idQ4;
   
   idX  = FindIndexOfElement(dataLabels, spacecraftName + ".X");
   idY  = FindIndexOfElement(dataLabels, spacecraftName + ".Y");
   idZ  = FindIndexOfElement(dataLabels, spacecraftName + ".Z");
   idVx = FindIndexOfElement(dataLabels, spacecraftName + ".Vx");
   idVy = FindIndexOfElement(dataLabels, spacecraftName + ".Vy");
   idVz = FindIndexOfElement(dataLabels, spacecraftName + ".Vz");

   idAx = FindIndexOfElement(dataLabels, spacecraftName + ".AccelerationX");
   idAy = FindIndexOfElement(dataLabels, spacecraftName + ".AccelerationY");
   idAz = FindIndexOfElement(dataLabels, spacecraftName + ".AccelerationZ");
   //MessageInterface::ShowMessage("&&&&&    idAx = %d,   idAy = %d,   idAz = %d\n", idAx, idAy, idAz);

   Integer idCov[21];
   UnsignedInt idx = 0;

   for (UnsignedInt ii = 0; ii < 6; ii++)
   {
      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         std::string elementName = spacecraftName + ".C" + names[ii] + names[jj];
         idCov[idx] = FindIndexOfElement(dataLabels, elementName);
         idx++;
      }
   }

   idQ1 = FindIndexOfElement(dataLabels, spacecraftName + ".Q1");
   idQ2 = FindIndexOfElement(dataLabels, spacecraftName + ".Q2");
   idQ3 = FindIndexOfElement(dataLabels, spacecraftName + ".Q3");
   idQ4 = FindIndexOfElement(dataLabels, spacecraftName + ".Q4");
   
   #ifdef DEBUG_EPHEMFILE_DATA_LABELS
   MessageInterface::ShowMessage
      ("   spacecraft='%s', idX=%d, idY=%d, idZ=%d, idVx=%d, idVy=%d, idVz=%d, idAx=%d, idAy=%d, idAz=%d\n",
       spacecraftName.c_str(), idX, idY, idZ, idVx, idVy, idVz, idAx, idAy, idAz);
   MessageInterface::ShowMessage(" idCov = [");
   for (Integer i = 0; i < 21; ++i)
      MessageInterface::ShowMessage("idCov[%d] = %d, ", i, idCov[i]);
   MessageInterface::ShowMessage("]\n"); 
   MessageInterface::ShowMessage
   ("idQ1=%d, idQ2=%d, idQ3=%d, idQ4=%d\n",
      idQ1, idQ2, idQ3, idQ4, idVy, idVz, idAx, idAy, idAz);
   #endif
   
   // if any of index not found, just return true
   if (idX  == -1 || idY  == -1 || idZ  == -1 ||
       idVx == -1 || idVy == -1 || idVz == -1 ||
       idQ1 == -1 || idQ2 == -1 || idQ3 == -1 ||
       idQ4 == -1)
   {
      #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::RetrieveData() '%s' Just returning false, data index "
          "not found for the spacecraft '%s'\n", GetName().c_str(), spacecraftName.c_str());
      #endif
      return false;
   }
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("   %s, epoch = %.15f, state = [%.15f, %.15f, %.15f, %.15f, %.15f, %.15f]\n",
       GetName().c_str(), dat[0], dat[idX], dat[idY], dat[idZ], dat[idVx], dat[idVy], dat[idVz]);
   MessageInterface::ShowMessage("   acceleration = [%.15le, %.15le, %.15le]\n",
      dat[idAx], dat[idAy], dat[idAz]);
   MessageInterface::ShowMessage("   covariance = [");
   Integer index = 0;
   for (Integer row = 0; row < 6; ++row)
   {
      for (Integer col = 0; col < 6; ++col)
      {
         if (row < col)
            continue;

         MessageInterface::ShowMessage("%.15le  ", dat[idCov[index]]);
         ++index;
      }
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("]\n");
   MessageInterface::ShowMessage("   attitude = [%.15le, %.15le, %.15le, %.15le]\n",
      dat[idQ1], dat[idQ2], dat[idQ3], dat[idQ4]);
   #endif
   
   // Now copy distributed data to data member
   currEpochInDays = dat[0];
   currState[0] = dat[idX];
   currState[1] = dat[idY];
   currState[2] = dat[idZ];
   currState[3] = dat[idVx];
   currState[4] = dat[idVy];
   currState[5] = dat[idVz];

   currAccel[0] = dat[idAx];
   currAccel[1] = dat[idAy];
   currAccel[2] = dat[idAz];

   for (Integer ii = 0; ii < 21; ii++)
   {
      if (idCov[ii] != -1 && idCov[ii] < len)
         currCov[ii] = dat[idCov[ii]];
      else
         currCov[ii] = 0.0;
   }

   currQuat[0] = dat[idQ1];
   currQuat[1] = dat[idQ2];
   currQuat[2] = dat[idQ3];
   currQuat[3] = dat[idQ4];

   #ifdef DEBUG_EPHEMFILE_DATA_LABELS
   MessageInterface::ShowMessage
      ("EphemerisFile::RetrieveData() returning true for '%s'\n", GetName().c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void HandlePropDirectionChange()
//------------------------------------------------------------------------------
void EphemerisFile::HandlePropDirectionChange()
{
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::HandlePropDirectionChange() <%p>'%s' entered, ephemWriter=<%p>\n",
       this, GetName().c_str(), ephemWriter);
   #endif
   
   if (ephemWriter == NULL)
   {
      #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::HandlePropDirectionChange() just leaving, "
          "ephemWriter is not created\n");
      #endif
      return;
   }
   
   propIndicator = 0;
   
   if (prevPropDirection == 0.0 && currPropDirection == 1.0)
      propIndicator = 1; // Initial forward prop
   else if (prevPropDirection == 0.0 && currPropDirection == -1.0)
      propIndicator = 2; // Initial backward prop
   else if (prevPropDirection == 1.0 && currPropDirection == -1.0)
      propIndicator = 3; // Changed from forward to backward prop
   else if (prevPropDirection == -1.0 && currPropDirection == 1.0)
      propIndicator = 4; // Changed from backward to forward prop
   
   #ifdef DEBUG_EPHEMFILE_DATA
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   DebugWriteTime("   prevEpochInSecs = ", prevEpochInSecs);
   MessageInterface::ShowMessage
      ("   currPropDirection=%f, prevPropDirection=%f, propIndicator=%d\n",
       currPropDirection, prevPropDirection, propIndicator);
   if (propIndicator == 1)
      MessageInterface::ShowMessage("   ==> It is initial forward propagation\n");
   else if (propIndicator == 2)
      MessageInterface::ShowMessage("   ==> It is initial backward propagation\n");
   else if (propIndicator == 3)
      MessageInterface::ShowMessage("   ==> It changed to backward propagation\n");
   else if (propIndicator == 4)
      MessageInterface::ShowMessage("   ==> It changed to forward propagation\n");
   #endif
   
   ephemWriter->SetPropIndicator(propIndicator);
   
   // If propagation direction changed, finishup writing current segment
   if (propIndicator > 2)
   {
      //if (!firstTimeWriting && !a1MjdArray.empty())
      if (!firstTimeWriting && !ephemWriter->IsDataEmpty())
      {
         // Start new segment if CCSDS format
         if ((fileFormat == "CCSDS-OEM" || fileFormat == "CCSDS-AEM"))
         {
            #ifdef DEBUG_EPHEMFILE_FINISH
            MessageInterface::ShowMessage
               ("   ===> prop direction changed for CCSDS, so calling StartNewSegment()\n");
            #endif
            std::string comment = "This block begins after propagation direction change";
            StartNewSegment(comment, true, true, true);
         }
         else
         {
            #ifdef DEBUG_EPHEMFILE_FINISH
            MessageInterface::ShowMessage
               ("   ===> prop direction changed for non-CCSDS, so calling FinishUpWriting()\n");
            #endif
            FinishUpWriting();
         }
      }
   }
      
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::HandlePropDirectionChange() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// bool IsBackwardPropAllowed()
//------------------------------------------------------------------------------
/**
 * Checks if backward prop is allowed or don't need special handling.
 *
 * @return  false  if no special handling is needed
 *          true   if need to procced to next step
 */
//------------------------------------------------------------------------------
bool EphemerisFile::IsBackwardPropAllowed()
{
   if (ephemWriter)
      return ephemWriter->IsBackwardPropAllowed(propDirection);
   else
      return false;
}


//------------------------------------------------------------------------------
// bool ProcessOrbitData()
//------------------------------------------------------------------------------
bool EphemerisFile::ProcessOrbitData()
{
   #ifdef DEBUG_EPHEMFILE_SOLVER_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::ProcessOrbitData() <%p> entered\nWriting out state with solver's final "
       "solution, runstate=%d, maneuverEpochInDays=%.15f\n", this, runstate,
       maneuverEpochInDays);
   DebugWriteOrbit("In ProcessOrbitData:", currEpochInDays, currState, true);
   MessageInterface::ShowMessage("acceleration = [%.15le  %.15le  %.15le]\n", currAccel[0], currAccel[1], currAccel[2]);
   #endif
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::ProcessOrbitData() state is RUNNING or SOLVEDPASS\n");
   #endif
   
   // Check for epoch before maneuver epoch
   // Propagate publishes data with epoch before maneuver epoch
   if (runstate == Gmat::SOLVEDPASS && currEpochInDays < maneuverEpochInDays)
   {
      #ifdef DEBUG_EPHEMFILE_SOLVER_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::ProcessOrbitData() Just returning; epoch (%.15f) < maneuver "
          "epoch (%.15f)solver is running\n", currEpochInDays, maneuverEpochInDays);
      #endif
      return false;
   }
   
   //if (finalEpochProcessed)
   if (ephemWriter->HasFinalEpochProcessed())
   {
      #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("=====> User defined final epoch has been processed, so just return\n");
      #endif
      return false;
   }
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::ProcessOrbitData() calling HandleOrbitData()\n");
   #endif
   
   // Call method to handle orbit data
   HandleOrbitData();
   
   #ifdef DEBUG_EPHEMFILE_SOLVER_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::ProcessOrbitData() <%p> returning true\n", this);
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool SetEpoch(const std::string &value)
//------------------------------------------------------------------------------
bool EphemerisFile::SetEpoch(Integer id, const std::string &value,
                             const StringArray &allowedValues)
{
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisFile::SetEpoch() entered, id=%d, value='%s', "
       "epochFormat='%s'\n", id, value.c_str(), epochFormat.c_str());
   #endif
   
   try
   {
      TimeSystemConverter::Instance()->ValidateTimeFormat(epochFormat, value);
   }
   catch (BaseException &)
   {
      if (epochFormat.find("Gregorian") != epochFormat.npos)
      {
         std::string rangeMsg = epochFormat;
         rangeMsg += " [" + DateUtil::EARLIEST_VALID_GREGORIAN;
         rangeMsg += " to " + DateUtil::LATEST_VALID_GREGORIAN;
         rangeMsg += "]";
         HandleError(id, value, allowedValues, " or value in " + rangeMsg);
      }
      else
      {
         std::string rangeMsg = "[" + DateUtil::EARLIEST_VALID_MJD;
         rangeMsg += " <= " + epochFormat;
         rangeMsg += " <= " + DateUtil::LATEST_VALID_MJD + "]";
         HandleError(id, value, allowedValues, " or value in " + rangeMsg);
      }
   }
   
   if (id == INITIAL_EPOCH)
      initialEpochStr = value;
   else if (id == FINAL_EPOCH)
      finalEpochStr = value;
   
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisFile::SetEpoch() returning true, initialEpochStr='%s', finalEpochStr='%s'\n",
       initialEpochStr.c_str(), finalEpochStr.c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetStepSize(Integer id, const std::string &value, const StringArray &allowedValues)
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
bool EphemerisFile::SetStepSize(Integer id, const std::string &value,
                                const StringArray &allowedValues)
{
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisFile::SetStepSize() entered, id=%d, value='%s'\n",
       id, value.c_str());
   #endif

   Real rval = -999.999;
   bool isReal = GmatStringUtil::ToReal(value, rval);
   if (!isReal || rval <= 0.0)
   {
      HandleError(id, value, allowedValues, " or Real Number > 0.0");
   }
   
   if (ephemWriter)
      ephemWriter->SetStepSize(rval);
   
   stepSize = value;
   stepSizeInSecs = rval;
   useFixedStepSize = true;
   
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisFile::SetStepSize() leaving, stepSize='%s'\n", stepSize.c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void HandleError(Integer id, const std::string &value,
//                  const StringArray &allowedValues, const std::string &additionalMsg)
//------------------------------------------------------------------------------
void EphemerisFile::HandleError(Integer id, const std::string &value,
                                const StringArray &allowedValues,
                                const std::string &additionalMsg)
{
   std::string allowedList = ToString(allowedValues);
   SubscriberException se;
   se.SetDetails(errorMessageFormat.c_str(), value.c_str(),
                 GetParameterText(id).c_str(),
                 (allowedList + additionalMsg).c_str());
   throw se;
}


//------------------------------------------------------------------------------
// std::string ToString(const StringArray &strList)
//------------------------------------------------------------------------------
/**
 * Converts std::string array to std::string separated by comma.
 */
//------------------------------------------------------------------------------
std::string EphemerisFile::ToString(const StringArray &strList)
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
std::string EphemerisFile::GetBackwardPropWarning()
{
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::GetBackwardPropWarning() entered\n");
   #endif
   
   std::string currTimeStr = ToUtcGregorian(currEpochInSecs);
   std::string prevTimeStr = ToUtcGregorian(prevEpochInSecs);
   std::string msg = "*** WARNING *** The user has generated non-monotonic "
      "invalid ephemeris file \"" + GetName() + "\" starting at " + currTimeStr +
      "; previous time is " + prevTimeStr + ".";
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("   current time (%s> < previous time (%s)\n", currTimeStr.c_str(),
       prevTimeStr.c_str());
   MessageInterface::ShowMessage
      ("EphemerisFile::GetBackwardPropWarning() returning '%s'\n", msg.c_str());
   #endif
   
   return msg;
}

//------------------------------------------------------------------------------
//bool IsEventFeasible(bool checkForNoData = true)
//------------------------------------------------------------------------------
/**
 * Checks if events can occur. Events other than maneuver can only occur after
 * valid data is received, so checkForNoData should be set appropriately.
 */
//------------------------------------------------------------------------------
bool EphemerisFile::IsEventFeasible(bool checkForNoData)
{
   #ifdef DEBUG_EPHEMFILE_EVENTS
   MessageInterface::ShowMessage
      ("EphemerisFile::IsEventFeasible() '%s' entered, checkForNoData=%d, active=%d, "
       "firstTimeWriting=%d, currEpochInSecs=%f\n", GetName().c_str(),
       checkForNoData, active, firstTimeWriting, currEpochInSecs);
   #endif
   
   if (!active)
   {
      #ifdef DEBUG_EPHEMFILE_EVENTS
      MessageInterface::ShowMessage
         ("EphemerisFile::IsEventFeasible() '%s' returning false, it is not active\n",
          GetName().c_str());
      #endif
      return false;
   }
   
   bool finalEpochProcessed = false;
   if (ephemWriter)
      finalEpochProcessed = ephemWriter->HasFinalEpochProcessed();
   
   if (finalEpochProcessed)
   {
      #ifdef DEBUG_EPHEMFILE_EVENTS
      MessageInterface::ShowMessage
         ("EphemerisFile::IsEventFeasible() '%s' returning false, user defined final "
          "epoch has been processed\n", GetName().c_str());
      #endif
      return false;
   }
   
   // Check if any valid data received if not maneuver event
   if (checkForNoData)
   {
      if (ephemWriter)
      {
         if (ephemWriter->IsDataEmpty())
            return false;
      }
      else
         return false;
   }
   
   #ifdef DEBUG_EPHEMFILE_EVENTS
   MessageInterface::ShowMessage
      ("EphemerisFile::IsEventFeasible() '%s' returning true\n", GetName().c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void SetEventEpoch();
//------------------------------------------------------------------------------
void EphemerisFile::SetEventEpoch()
{
   if (ephemWriter)
      ephemWriter->SetEventEpoch(eventEpochInSecs);
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
std::string EphemerisFile::ToUtcGregorian(Real epoch, bool inDays, Integer format)
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
         ("**** ERROR **** EphemerisFile::ToUtcGregorian() Cannot convert epoch %.10f %s "
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
void EphemerisFile::DebugWriteTime(const std::string &msg, Real epoch, bool inDays,
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
//                      bool inDays = false)
//------------------------------------------------------------------------------
void EphemerisFile::DebugWriteOrbit(const std::string &msg, Real epoch,
                                    const Real state[6], bool inDays)
{
   Real reqEpochInDays = epoch;
   if (!inDays)
      reqEpochInDays = epoch / GmatTimeConstants::SECS_PER_DAY;
   
   Rvector6 inState(state);
   Rvector6 outState(state);
   
   std::string epochStr = ToUtcGregorian(reqEpochInDays, true, 2);
   
   MessageInterface::ShowMessage
      ("%s\n%s\n% 1.15e  % 1.15e  % 1.15e\n% 1.15e  % 1.15e  % 1.15e\n",
       msg.c_str(), epochStr.c_str(), outState[0], outState[1], outState[2],
       outState[3], outState[4], outState[5]);
}


//------------------------------------------------------------------------------
// void DebugWriteOrbit(const std::string &msg, A1Mjd *epochInDays, Rvector6 *state)
//------------------------------------------------------------------------------
void EphemerisFile::DebugWriteOrbit(const std::string &msg, A1Mjd *epochInDays,
                                    Rvector6 *state)
{
   DebugWriteOrbit(msg, epochInDays->GetReal(), state->GetDataVector(), true);
}


//------------------------------------------------------------------------------
// void WriteDeprecatedMessage(Integer id) const
//------------------------------------------------------------------------------
/**
 * Writes deprecated field message per GMAT session
 */
//------------------------------------------------------------------------------
void EphemerisFile::WriteDeprecatedMessage(Integer id) const
{
   // Write only one message per session
   static bool writeFileNameMsg = true;
   
   switch (id)
   {
   case FILE_NAME:
      if (writeFileNameMsg)
      {
         MessageInterface::ShowMessage
            (deprecatedMessageFormat.c_str(), "FileName", GetName().c_str(),
             "Filename");
         writeFileNameMsg = false;
      }
      break;
   default:
      break;
   }
}

//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

//------------------------------------------------------------------------------
// bool Distribute(int len)
//------------------------------------------------------------------------------
bool EphemerisFile::Distribute(int len)
{
   return true;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real *dat, Integer len)
//------------------------------------------------------------------------------
/*
 * Handles distributed data from Subscriber::ReceiveData() through
 * Publisher::Publish(). Asssumes first data dat[0] is data epoch in A1Mjd.
 *
 * @param dat Data received 
 */
//------------------------------------------------------------------------------
bool EphemerisFile::Distribute(const Real *dat, Integer len)
{
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("======================================================================\n"
       "EphemerisFile::Distribute() this=<%p>'%s' entered, ephemWriter=<%p> for '%s'\n",
       this, GetName().c_str(), ephemWriter, spacecraftName.c_str());
   MessageInterface::ShowMessage
      ("   len=%d, active=%d, writeEphemeris=%d, isEndOfReceive=%d, isEndOfDataBlock=%d, "
       "isEndOfRun=%d\n   runstate=%d, prevRunState=%d, isManeuvering=%d, firstTimeWriting=%d, "
       "propDirection=%f\n", len, active, writeEphemeris, isEndOfReceive, isEndOfDataBlock,
       isEndOfRun, runstate, prevRunState, isManeuvering, firstTimeWriting, propDirection);
   if (len > 0)
   {
      DebugWriteTime("   dat[0] = ", dat[0], true);
      MessageInterface::ShowMessage("   dat[*] = ");
      for (Integer i = 1; i < len; ++i)
      {
         MessageInterface::ShowMessage("%1.15e", dat[i]);
         if (i == len-1)
            MessageInterface::ShowMessage("\n");
         else
            MessageInterface::ShowMessage(", ");
      }
   }
   #endif
   #ifdef DEBUG_EPHEMFILE_DATA_MORE
   MessageInterface::ShowMessage("   fileName='%s'\n", fileName.c_str());
   #endif

   // If end of run received, finishup writing
   if (isEndOfRun)
      return HandleEndOfRun();
   
   // If EphemerisFile was toggled off, start new segment (LOJ: 2010.09.30)
   if (!active)
   {
      if (ephemWriter)
         ephemWriter->SetWriteNewSegment(true);
      
      #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("=====> EphemerisFile::Distribute() '%s' returning true, it is toggled off\n",
          GetName().c_str());
      #endif
      return true;
   }
   
   // Create EphemerisWriter if not created already
   // CreateEphmerisWriter will throw an exception if failed to create
   if (ephemWriter == NULL)
      CreateEphemerisWriter();
   
   if (!isEphemFileOpened)
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("===> EphemerisFile::Distribute() <%p>'%s' creating EphemerisFile of type '%s'\n",
          this, GetName().c_str(), fileFormat.c_str());
      #endif
      CreateEphemerisFile();
   }
   
   if (len == 0)
   {
      #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Just returning true, data length is zero\n");
      #endif
      return true;
   }
   
   isFinalized = false;
   
   //------------------------------------------------------------
   // if solver is running, just return
   //------------------------------------------------------------
   if (runstate == Gmat::SOLVING)
   {
      #ifdef DEBUG_EPHEMFILE_SOLVER_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Just returning true; solver is running\n");
      #endif
      
      return true;
   }
   
   // Skip data if data publishing command such as Propagate is inside a function
   // and this EphemerisFile is not a global nor a local object (i.e declared in the main script)
   // (LOJ: 2015.08.13)
//   if (currentProvider && currentProvider->TakeAction("IsInFunction"))
//   {
//      if (SkipFunctionData())
//         return true;
//   }
   
   // Get proper id with data label
   if (theDataLabels.empty())
   {
      #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Just returning true, data labels are empty\n");
      #endif
      return true;
   }

   if ((currentProvider->IsOfType("RunEstimator") || currentProvider->IsOfType("RunSmoother")) && useFixedStepSize)
   {
      std::string estName = currentProvider->GetRefObjectName(Gmat::SOLVER);
      GmatBase* obj = currentProvider->GetRefObject(Gmat::SOLVER, estName);

      if (obj->IsOfType("SeqEstimator") || obj->IsOfType("SmootherBase"))
         throw SubscriberException("Error writing to ephemeris file \"" + fileName + "\". "
            "Writing to an ephemeris file with a fixed StepSize is not allowed inside "
            "an Estimator of type " + obj->GetTypeName() + ". To correct this problem, "
            "set the ephemeris StepSize parameter to 'IntegratorSteps' or choose an "
            "ephemeris type that supports the setting StepSize = 'IntegratorSteps'.");
   }
   
   // Retrieve data
   if (!RetrieveData(dat, len))
      return true;
   
   // Set data to EphemerisWriter
   ephemWriter->SetOrbitData(currEpochInDays, currState, currCov, currAccel, currQuat);
   
   // To compute block time span for use in the error message (LOJ: 2014.04.30)
   // Save block begin time
   if (blockBeginA1Mjd == -999.999)
      blockBeginA1Mjd = currEpochInDays;
   
   #ifdef DEBUG_MANEUVER
   MessageInterface::ShowMessage
      ("   Distribute dat[0]=%.15f, dat[1]=%.15f, dat[4]=%.15f\n", dat[0], dat[1], dat[4]);
   #endif
   
   // Internally all epochs are in seconds to avoid epoch drifting.
   // For long run epochs to process drifts behind the actual.
   prevEpochInSecs = currEpochInSecs;
   currEpochInSecs = currEpochInDays * GmatTimeConstants::SECS_PER_DAY;
   prevPropDirection = currPropDirection;
   currPropDirection = propDirection;
   
   // Set epoch and prop direction to EphemerisWriter
   ephemWriter->SetEpochAndDirection(prevEpochInSecs, currEpochInSecs,
                                     prevPropDirection, currPropDirection);
   
   // Write a new segment if propation direction changes
   HandlePropDirectionChange();
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage("   ===> Check for time going backward\n");
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   DebugWriteTime("   prevEpochInSecs = ", prevEpochInSecs);
   #endif
   // Check for time going backward (GMT-4066 FIX)
   if (currEpochInSecs < prevEpochInSecs)
   {
      #ifdef DEBUG_EPHEMFILE_DATA
      std::string currTimeStr = ToUtcGregorian(currEpochInSecs);
      std::string prevTimeStr = ToUtcGregorian(prevEpochInSecs);
      MessageInterface::ShowMessage
         ("   current time (%s> < previous time (%s)\n", currTimeStr.c_str(),
          prevTimeStr.c_str());
      #endif
      
      if (!IsBackwardPropAllowed())
      {
         std::string msg = GetBackwardPropWarning();
         throw SubscriberException(msg);
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage("   ===> Check for duplicate data\n");
   #endif
   // Ignore duplicate data
   if (currEpochInSecs == prevEpochInSecs)
   {
      #ifdef DEBUG_MANEUVER
      MessageInterface::ShowMessage("Ignoring duplicate\n");
      #endif
      #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Just returning true, currEpochInSecs (%.15f) "
          "= prevEpochInSecs (%.15f)\n", currEpochInSecs, prevEpochInSecs);
      #endif
      //if (currentProvider->GetTypeName() == "Propagate" && ((Propagate *)currentProvider)->DoPropsHaveFiniteBurn() && fileFormat == "CCSDS-OEM")
      //{
      //   //Do Nothing
      //}
      //else
         return true;
   }
   
   
   //=================================================================
   //Hold this for now to complete GMT-4066(LOJ: 2013.07.19)
   //=================================================================
   #if 0
   // Check for large time gap if needed
   if (checkForLargeTimeGap)
   {
      #ifdef DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage("   Checking for large time gap for Code500 ephemeris\n");
      #endif
      
      // Assuming time gap is greater than 0.1 day
      if (currEpochInSecs > ( prevEpochInSecs + 8640.0))
      {
         std::string currTimeStr = ToUtcGregorian(currEpochInSecs);
         std::string prevTimeStr = ToUtcGregorian(prevEpochInSecs);
         SubscriberException se;
         se.SetDetails("Cannot continue ephemeris generation for the EphemerisFile \"%s\".\n   "
                       "It encounterd large time gap between points, so stopping.\n   "
                       "current epoch: %s, previous epoch: %s\n", GetName().c_str(),
                       currTimeStr.c_str(), prevTimeStr.c_str());
         throw se;
      }
   }
   #endif
   //=================================================================
   
   
   bool processData = false;
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::Distribute() ********* state is %d\n", runstate);
   #endif
   
   //------------------------------------------------------------
   // if solver is not running or solver has finished, write data
   //------------------------------------------------------------
   if (runstate == Gmat::RUNNING || runstate == Gmat::SOLVEDPASS)
   {
      processData = ProcessOrbitData();
      if (!processData)
      {
         #ifdef DEBUG_EPHEMFILE_DATA
         MessageInterface::ShowMessage
            ("EphemerisFile::Distribute() this=<%p>'%s' returning true, data %s processed\n",
             this, GetName().c_str(), processData ? "has" : "has not");
         #endif
         return true;
      }
   }
   
   // Set previous prop direction
   prevPropDirection = currPropDirection;
   
   #ifdef DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFile::Distribute() this=<%p>'%s' returning true, data %s processed\n",
       this, GetName().c_str(), processData ? "has" : "has not");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// virtual void HandleManeuvering(GmatBase *originator, bool maneuvering,
//                                Real epoch, const StringArray &satNames,
//                                const std::string &desc)
//------------------------------------------------------------------------------
/*
 * @see Subscriber
 */
//------------------------------------------------------------------------------
void EphemerisFile::HandleManeuvering(GmatBase *originator, bool maneuvering,
                                      Real epoch, const StringArray &satNames,
                                      const std::string &desc)
{
   #ifdef DEBUG_EPHEMFILE_MANEUVER
   MessageInterface::ShowMessage
      ("\n==================================================\n"
       "EphemerisFile::HandleManeuvering() '%s' entered, active=%d, originator=<%p><%s>, "
       "maneuvering=%d, maneuver %s, epoch=%.15f, %s\n   satNames.size()=%d, satNames[0]='%s', "
       "desc='%s'\n   prevRunState=%d, runstate=%d, maneuversHandled.size()=%d\n", GetName().c_str(),
       active, originator, originator ? originator->GetTypeName().c_str() : "NULL",
       maneuvering, maneuvering ? "started" : "stopped", epoch, ToUtcGregorian(epoch, true).c_str(),
       satNames.size(), satNames.empty() ? "NULL" : satNames[0].c_str(), desc.c_str(),
       prevRunState, runstate, maneuversHandled.size());
   #endif
   
   if (!IsEventFeasible(false))
   {
      #ifdef DEBUG_EPHEMFILE_MANEUVER
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleManeuvering() '%s' leaving, the maneuver "
          "event is not feasible at this time\n", GetName().c_str());
      #endif
      return;
   }
   
   if (originator == NULL)
      throw SubscriberException
         ("Cannot continue with ephemeris file writing, the maneuvering burn object is NULL");
   
   // Check spacecraft name first   
   if (find(satNames.begin(), satNames.end(), spacecraftName) == satNames.end())
   {
      #ifdef DEBUG_EPHEMFILE_MANEUVER_MORE
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleManeuvering() '%s' leaving, the spacecraft '%s' is not "
          "writing Ephmeris File\n", GetName().c_str(), spacecraftName.c_str());
      #endif
      return;
   }
   
   bool restart = false;
   // Check if finite maneuver started
   if (runstate == Gmat::RUNNING && prevRunState == Gmat::IDLE && maneuvering)
   {
      #ifdef DEBUG_EPHEMFILE_MANEUVER_MORE
      MessageInterface::ShowMessage("=====> 1 setting to restart\n");
      #endif
      restart = true;
   }
   // Check if finite maneuver ended
   else if (runstate == Gmat::RUNNING && prevRunState == Gmat::RUNNING && !maneuvering)
   {
      #ifdef DEBUG_EPHEMFILE_MANEUVER_MORE
      MessageInterface::ShowMessage("=====> 2 setting to restart\n");
      #endif
      restart = true;
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_MANEUVER_MORE
      MessageInterface::ShowMessage
         ("   ==> Checking if runstate is SOLVEDPASS and Maneuver already handled\n");
      #endif
      bool doNext = true;
      if (prevRunState == runstate && runstate == Gmat::SOLVEDPASS)
      {
         // Check if the originator already handled
         if (find(maneuversHandled.begin(), maneuversHandled.end(), originator) !=
             maneuversHandled.end())
         {
            #ifdef DEBUG_EPHEMFILE_MANEUVER
            MessageInterface::ShowMessage
               ("EphemerisFile::HandleManeuvering() prevRunState is "
                "SOLVEDPASS for the same originator\n");
            #endif
            doNext = false;
         }
      }
      
      #ifdef DEBUG_EPHEMFILE_MANEUVER_MORE
      MessageInterface::ShowMessage("   doNext=%d\n", doNext);
      #endif
      if (doNext && (runstate == Gmat::RUNNING || runstate == Gmat::SOLVEDPASS))
      {
         #ifdef DEBUG_EPHEMFILE_MANEUVER
         MessageInterface::ShowMessage
            ("EphemerisFile::HandleManeuvering() GMAT is not solving or solver has "
             "finished; prevRunState=%d, runstate=%d\n", prevRunState, runstate);
         #endif
         
         if (prevRunState != Gmat::IDLE)
         {            
            // Added to maneuvers handled
            maneuversHandled.push_back(originator);
            #ifdef DEBUG_EPHEMFILE_MANEUVER_MORE
            MessageInterface::ShowMessage("=====> 3 setting to restart\n");
            #endif
            restart = true;
         }
         else
         {
            #ifdef DEBUG_EPHEMFILE_MANEUVER_MORE
            MessageInterface::ShowMessage
               ("EphemerisFile::HandleManeuvering() GMAT is running or solving\n");
            #endif
         }
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_MANEUVER_MORE
   MessageInterface::ShowMessage("   restart=%d\n", restart);
   #endif
   
   // Finish up writing and restart interpolation if restart is needed
   if (restart)
   {
      #ifdef DEBUG_EPHEMFILE_RESTART
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleManeuvering() Calling FinishUpWriting()\n");
      #endif
      
      //LOJ: Write continuous ephemeris if CODE500_EPHEM or STK_TIMEPOSVEL
      //if (fileType != CODE500_EPHEM && fileType != STK_TIMEPOSVEL)
      if (allowMultipleSegments)
      {
         FinishUpWriting();
         
         maneuverEpochInDays = epoch;
         
         // Convert current epoch to gregorian format
         std::string epochStr = ToUtcGregorian(epoch, true, 2);
         
         #ifdef DEBUG_EPHEMFILE_RESTART
         MessageInterface::ShowMessage
            ("=====> Burn event, Restarting the interpolation at %s\n", epochStr.c_str());
         #endif
         
         // Restart interpolation
         std::string comment;
         bool writeComment = false;
         if (maneuvering)
            writeComment = true;
         else if (originator->IsOfType("EndFiniteBurn"))
            writeComment = true;
         
         if (writeComment)
            comment = "This block begins after " + desc + " at " + epochStr;
         
         #ifdef DEBUG_EPHEMFILE_MANEUVER
         MessageInterface::ShowMessage
            ("   writeComment=%d, comment='%s'\n", writeComment, comment.c_str());
         #endif
         
         StartNewSegment(comment, false, true, true);
      }
   }
   
   // It is set in the Subscriber. Do we need it here? (LOJ: 2014.04.08)
   //prevRunState = runstate;
   
   #ifdef DEBUG_EPHEMFILE_MANEUVER
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleManeuvering() '%s' leaving\n\n", GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void HandlePropagatorChange(GmatBase *provider, Real epochInMjd = -999.999)
//------------------------------------------------------------------------------
void EphemerisFile::HandlePropagatorChange(GmatBase *provider, Real epochInMjd)
{
   #ifdef DEBUG_EPHEMFILE_PROPAGATOR_CHANGE
   MessageInterface::ShowMessage
      ("\n==================================================\n"
       "EphemerisFile::HandlePropagatorChange() '%s' entered, provider=<%p><%s>, "
       "epochInMjd=%f, active=%d, isEndOfRun=%d\n", GetName().c_str(), provider,
       provider->GetTypeName().c_str(), epochInMjd, active, isEndOfRun);
   #endif
   
   if (!IsEventFeasible())
   {
      #ifdef DEBUG_EPHEMFILE_PROPAGATOR_CHANGE
      MessageInterface::ShowMessage
         ("EphemerisFile::HandlePropagatorChange() '%s' leaving, the propagator "
          "change is not feasible at this time\n", GetName().c_str());
      #endif
      return;
   }
   
   if (provider == NULL)
      throw SubscriberException
         ("Cannot continue with ephemeris file writing, the provider command is NULL");
   
   if (runstate == Gmat::RUNNING || runstate == Gmat::SOLVEDPASS)
   {
      #ifdef DEBUG_EPHEMFILE_PROPAGATOR_CHANGE_MORE
      MessageInterface::ShowMessage
         ("EphemerisFile::HandlePropagatorChange() GMAT is not solving or solver has finished\n");
      #endif
      
      // Check if propagator name changed on ephemeris file spacecraft
      if (provider->GetTypeName() == "Propagate")
      {
         // Go through propagator list and check if spacecraft found
         StringArray propNames = provider->GetRefObjectNameArray(Gmat::PROP_SETUP);
         Integer scId = provider->GetParameterID("Spacecraft");
         for (UnsignedInt prop = 0; prop < propNames.size(); prop++)
         {
            StringArray satNames = provider->GetStringArrayParameter(scId, prop);
            for (UnsignedInt sat = 0; sat < satNames.size(); sat++)
            {
               if (spacecraftName == satNames[sat])
               {
                  if (currPropName != propNames[prop])
                  {
                     currPropName = propNames[prop];
                     
                     #ifdef DEBUG_EPHEMFILE_PROPAGATOR_CHANGE
                     MessageInterface::ShowMessage
                        ("The propagator changed from '%s' to '%s'\n", prevPropName.c_str(),
                         currPropName.c_str());
                     #endif
                     
                     if (prevPropName != "")
                     {
                        #ifdef DEBUG_EPHEMFILE_RESTART
                        MessageInterface::ShowMessage
                           ("EphemerisFile::HandlePropagatorChange() Calling FinishUpWriting()\n   "
                            "The propagator changed from '%s' to '%s'\n", prevPropName.c_str(),
                            currPropName.c_str());
                        #endif
                        
                        // Write any data in the buffer (fixes missing lines for GMT-3745)
                        //LOJ: Write continuous ephemeris if CODE500_EPHEM or STK_TIMEPOSVEL
                        //if (fileType != CODE500_EPHEM && fileType != STK_TIMEPOSVEL)
                        if (allowMultipleSegments)
                        {
                           FinishUpWriting();
                           
                           #ifdef DEBUG_EPHEMFILE_PROPAGATOR_CHANGE
                           MessageInterface::ShowMessage
                              ("=====> Propagator change, Restarting the interpolation\n");
                           #endif
                           
                           // Convert current epoch to gregorian format
                           std::string epochStr;
                           if (epochInMjd != -999.999)
                           {
                              epochStr = ToUtcGregorian(epochInMjd, true, 2);
                              epochStr = " at " + epochStr;
                           }
                           
                           // Restart interpolation
                           std::string comment = "This block begins after propagator change from " +
                              prevPropName + " to " + currPropName + epochStr;
                           
                           StartNewSegment(comment, false, true, true);
                        }
                     }
                     else
                     {
                        #ifdef DEBUG_EPHEMFILE_PROPAGATOR_CHANGE
                        MessageInterface::ShowMessage
                           ("The previous propagator name is blank, so nothing needs to be done\n");
                        #endif
                     }
                     
                     prevPropName = currPropName;
                     
                  }
                  else
                  {
                     #ifdef DEBUG_EPHEMFILE_PROPAGATOR_CHANGE_MORE
                     MessageInterface::ShowMessage
                        ("The propagator is the same as '%s'\n", currPropName.c_str());
                     #endif
                  }
               }
            }
         }
      }
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_PROPAGATOR_CHANGE_MORE
      MessageInterface::ShowMessage
         ("EphemerisFile::HandlePropagatorChange() GMAT is solving\n");
      #endif
   }
   
   #ifdef DEBUG_EPHEMFILE_PROPAGATOR_CHANGE
   MessageInterface::ShowMessage
      ("EphemerisFile::HandlePropagatorChange() '%s' leaving, provider=<%p><%s>\n",
       GetName().c_str(), provider, provider->GetTypeName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void HandleSpacecraftPropertyChange(GmatBase *originator, Real epoch,
//                 const std::string &satName, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * @see Subscriber
 */
//------------------------------------------------------------------------------
void EphemerisFile::HandleSpacecraftPropertyChange(GmatBase *originator, Real epoch,
                                                   const std::string &satName,
                                                   const std::string &desc)
{
   #ifdef DEBUG_EPHEMFILE_SC_PROPERTY_CHANGE
   MessageInterface::ShowMessage
      ("\n==================================================\n"
       "EphemerisFile::HandleSpacecraftPropertyChange() '%s' entered, originator=<%p>\n   "
       "epoch=%.15f, satName='%s', desc='%s', active=%d\n   firstTimeWriting=%d, "
       "currEpochInDays=%f, runstate=%d\n", GetName().c_str(),
       originator, epoch, satName.c_str(), desc.c_str(), active, firstTimeWriting,
       currEpochInDays, runstate);
   DebugWriteTime("   event epoch ", epoch, true);
   #endif
      
   if (originator == NULL)
      throw SubscriberException
         ("Cannot continue with ephemeris file writing, the spacecraft of which "
          "property changed is NULL");
   
   bool checkForEmptyData = true;
   if (originator->IsOfType("Vary"))
      checkForEmptyData = false;
   
   if (!IsEventFeasible(checkForEmptyData))
   {
      #ifdef DEBUG_EPHEMFILE_SC_PROPERTY_CHANGE
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleSpacecraftPropertyChange() '%s' leaving, the spacecraft "
          "property change is not feasible at this time\n", GetName().c_str());
      #endif
      return;
   }
   
   eventEpochInSecs = epoch * GmatTimeConstants::SECS_PER_DAY;

   // Set event epoch to EphemerisWriter
   SetEventEpoch();
   
   std::string epochStr = ToUtcGregorian(epoch, true, 2);
   
   if (spacecraftName == satName)
   {
      if (originator->IsOfType("Vary"))
      {
         if (runstate == Gmat::SOLVING)
         {
            #ifdef DEBUG_EPHEMFILE_SC_PROPERTY_CHANGE
            MessageInterface::ShowMessage
               ("EphemerisFile::HandleSpacecraftPropertyChange() '%s' leaving, "
                "the spacecraft is set from the Vary command and status is SOLVING\n",
                GetName().c_str());
            #endif
            return;
         }
      }
      
      #ifdef DEBUG_EPHEMFILE_RESTART
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleSpacecraftPropertyChange() Calling FinishUpWriting()\n");
      #endif
      
      // Write any data in the buffer
      //LOJ: Write continuous ephemeris if CODE500_EPHEM or STK_TIMEPOSVEL
      //if (fileType != CODE500_EPHEM && fileType != STK_TIMEPOSVEL)
      if (allowMultipleSegments)
      {
         FinishUpWriting();
         
         // Restart interpolation
         std::string comment = "This block begins after spacecraft setting " +
            desc + " at " + epochStr;
         
         StartNewSegment(comment, false, true, true);
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_SC_PROPERTY_CHANGE
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleSpacecraftPropertyChange() '%s' leaving\n", GetName().c_str());
   #endif
}
