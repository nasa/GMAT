//------------------------------------------------------------------------------
//                                  CCSDSEphemerisFile
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
// Author: Tuan D. Nguyen / GSFC-NASA, Code 583
// Created: 2021.07.19
//
/**
 * Writes a spacecraft orbit ephemeris to a file in CCSDS format.
 */
//------------------------------------------------------------------------------

#include "CCSDSEphemerisFile.hpp"
#include "CCSDSOEMReader.hpp"
#include "CCSDSAEMReader.hpp"
#include "FileUtil.hpp"
#include "FileTypes.hpp"
#include "StringUtil.hpp"
#include "TimeTypes.hpp"             // for FormatCurrentTime()
#include "UtilityException.hpp"
#include "MessageInterface.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "GmatGlobal.hpp"
#include <sstream>
#include <limits>
#include <algorithm>

// We want to use std::numeric_limits<std::streamsize>::max()
#ifdef _MSC_VER  // if Microsoft Visual C++
#undef max
#endif

//#define DEBUG_CCSDS_FILE
//#define DEBUG_CCSDS_EPHEMFILE_INIT
//#define DEBUG_INITIALIZE
//#define DEBUG_WRITE_DATA_SEGMENT
//#define DEBUG_FINALIZE
//#define DEBUG_INITIAL_FINAL
//#define DEBUG_WRITE_POSVEL
//#define DEBUG_WRITE_COVPOSVEL
//#define DEBUG_DISTANCEUNIT
//#define DEBUG_VERSION
//#define DEBUG_WRITE_HEADER
//#define DEBUG_WRITE_STRING
//#define DEBUG_SEGMENTING
//#define DUMP_SEGMENT_DATA

//----------------------------
// static data
//----------------------------

//----------------------------
// public methods
//----------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
CCSDSEphemerisFile::CCSDSEphemerisFile() :
   ccsdsFileNameForRead  (""),
   stateType             ("OEM"),
   fileReader            (NULL)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
CCSDSEphemerisFile::CCSDSEphemerisFile(const CCSDSEphemerisFile &copy) :
   ccsdsFileNameForRead  (copy.ccsdsFileNameForRead),
   stateType             (copy.stateType),
   //fileReader            (copy.fileReader)
   fileReader            (NULL)
{
   if (copy.fileReader)
      fileReader = copy.fileReader->Clone();
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
CCSDSEphemerisFile& CCSDSEphemerisFile::operator=(const CCSDSEphemerisFile &copy)
{
   if (&copy == this)
      return *this;
   
   ccsdsFileNameForRead  = copy.ccsdsFileNameForRead;
   stateType             = copy.stateType;

   //fileReader            = copy.fileReader;
   if (fileReader)
      delete fileReader;
   fileReader = NULL;
   if (copy.fileReader)
      fileReader = copy.fileReader->Clone();

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CCSDSEphemerisFile::~CCSDSEphemerisFile()
{
   if (fileReader != NULL)
      delete fileReader;
}

//------------------------------------------------------------------------------
// void InitializeData()
//------------------------------------------------------------------------------
/**
 * Clears header information.
 */
//------------------------------------------------------------------------------
void CCSDSEphemerisFile::InitializeData()
{
   #ifdef DEBUG_CCSDS_EPHEMFILE_INIT
   MessageInterface::ShowMessage("CCSDSEphemerisFile::InitializeData() entered\n");
   #endif
   
   scenarioEpochA1Mjd  = 0.0;

   warnInterpolationDegradation = true;
}

//------------------------------------------------------------------------------
// bool OpenForRead(const std::string &filename, const std::string &ephemType,
//                  const std::string &ephemCovType)
//------------------------------------------------------------------------------
/**
 * Opens CCSDS ephemeris (.e) file for reading.
 *
 * @filename File name to open
 * @ephemType Ephemeris type to read, at this time only "TimePos" or "TimePosVel"
 *               is allowed
 * @ephemCovType Ephemeris covariance type to read, at this time only "", "TimePos" or "TimePosVel"
 *               is allowed
 */
//------------------------------------------------------------------------------
bool CCSDSEphemerisFile::OpenForRead(const std::string &filename,
   const std::string &ephemType,
   const std::string &ephemCovType)
{
#ifdef DEBUG_CCSDS_FILE
   MessageInterface::ShowMessage
   ("CCSDSEphemerisFile::OpenForRead() entered, ephemType = <%s>  ephemCovType = <%s>  filename='%s'\n",
      ephemType.c_str(), ephemCovType.c_str(), filename.c_str());
#endif
   
   ccsdsFileNameForRead = filename;

   bool retval = false;

   // Check ephem type
   // Currently only TimePos and TimePosVel are allowed
   /// @todo Do we allow TimePos?  ReadDataRecords seems to say no
   if (ephemType != "TimePos" && ephemType != "TimePosVel")
   {
      UtilityException ue;
      ue.SetDetails("CCSDSEphemerisFile::OpenForRead() *** INTERNAL ERROR *** "
         "Only TimePos or TimePosVel is valid for read on CCSDS "
         "ephemeris file '%s'.", ccsdsFileNameForRead.c_str());
      throw ue;
   }

   if ((ephemType == "TimePos") || (ephemType == "TimePosVel"))
      stateType = "OEM";
   else if ((ephemType == "TimeAngle") || (ephemType == "TimeAngleVel"))
      stateType = "AEM";

   // Check covariance ephem type
   // Currently only "", TimePos, and TimePosVel are allowed
   if (ephemCovType != "" && ephemCovType != "TimePos" && ephemCovType != "TimePosVel")
   {
      UtilityException ue;
      ue.SetDetails("CCSDSEphemerisFile::OpenForRead() *** INTERNAL ERROR *** "
         "Only "", TimePos, or TimePosVel is valid to read covariance on CCSDS "
         "ephemeris file '%s'.", ccsdsFileNameForRead.c_str());
      throw ue;
   }
   
#ifdef DEBUG_CCSDS_FILE
   MessageInterface::ShowMessage
   ("CCSDSEphemerisFile::OpenForRead() fileReader = <%p>\n", fileReader);
#endif
   if (fileReader == NULL)
   {
      if (stateType == "OEM")
         fileReader = new CCSDSOEMReader();
      else if (stateType == "AEM")
         fileReader = new CCSDSAEMReader();

      fileReader->SetFile(ccsdsFileNameForRead);
      Integer numOfSegs = fileReader->GetNumberOfSegments();
      Real startEp = -1.0;

      theEphem.clear();
      ephemRecords.clear();

      for (Integer i = 0; i < numOfSegs; ++i)
      {
         CCSDSEMSegment* ccsdsSegment = fileReader->GetSegment(i);
         Integer numOfDataPoints = ccsdsSegment->GetNumberOfDataPoints();
         
         Segment newsegment;
         for (Integer index = 0; index < numOfDataPoints; ++index)
         {
            // Get epoch and state from CCSDS segment
            Real epoch;
            Rvector state;
            ccsdsSegment->GetEpochAndData(index, epoch, state);
            if (startEp == -1.0)
               startEp = epoch;

            // store epoch and state to Ephemeris' Segment
            EphemPoint point;
            point.theEpoch = epoch;
            point.posvel = Rvector6(state[0], state[1], state[2], state[3], state[4], state[5]);
            newsegment.points.push_back(point);

            // Store time from start epoch and state to ephemRecords
            EphemData eData;
            eData.timeFromEpoch = (point.theEpoch - startEp) * GmatTimeConstants::SECS_PER_DAY;
            for(Integer k = 0; k < 6; ++k)
              eData.theState[k] = point.posvel(k);
            ephemRecords.push_back(eData);
         }
         
         newsegment.segStart = newsegment.points[0].theEpoch;
         newsegment.segEnd   = newsegment.points[newsegment.points.size()-1].theEpoch;
         theEphem.push_back(newsegment);
      }
      
      a1StartEpoch = theEphem[0].points[0].theEpoch;
      a1EndEpoch   = theEphem[theEphem.size()-1].points[theEphem[theEphem.size() - 1].points.size()-1].theEpoch;
      scenarioEpochA1Mjd = a1StartEpoch;
      
      interpolatorName = fileReader->GetSegment(0)->GetStringMetaData("INTERPOLATION");
      if (interpolatorName == "Lagrange")
         useHermite = false;
      
      order = fileReader->GetSegment(0)->GetIntegerMetaData("INTERPOLATION_DEGREE");
   }

#ifdef DEBUG_CCSDS_FILE
   MessageInterface::ShowMessage
   ("CCSDSEphemerisFile::OpenForRead() end, filename='%s'\n", filename.c_str());
#endif
   return true;
}


//------------------------------------------------------------------------------
// void CloseForRead()
//------------------------------------------------------------------------------
void CCSDSEphemerisFile::CloseForRead()
{
}

//------------------------------------------------------------------------------
// bool GetInitialAndFinalStates(Real &initialA1Mjd, Real &finalA1Mjd,
//                               Rvector6 &initialState, Rvector6 &finalState,
//                               std::string &cbName, std::string &csName)
//------------------------------------------------------------------------------
/**
 * Retrieves initial and final state from CCSDS ephem file. Assumes ephem file
 * is successfully opened via OpenForRead()
 *
 * @param initialA1Mjd  output initial epoch in a1mjd
 * @param finalA1Mjd    output final epoch in a1mjd
 * @param initialState  output initial cartesian state in MJ2000Eq
 * @param finalState    output final cartesian state in MJ2000Eq
 * @param cbName        output central body name
 * @param csName        output coordinate system name
 *
 * @return true if all output parameter values are assigned, false otherwise
 */
//------------------------------------------------------------------------------
bool CCSDSEphemerisFile::GetInitialAndFinalStates(Real &initialA1Mjd, Real &finalA1Mjd,
                                                Rvector6 &initialState, Rvector6 &finalState,
                                                std::string &cbName, std::string &csName)
{
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("CCSDSEphemerisFile::GetInitialAndFinalState() entered\n");
   #endif
   
   // Set defaults for output
   cbName = "Earth";
   csName = "J2000";

   bool lastStateFound = false;
   
   if (ReadDataRecords())
   {
      int numRecords = ephemRecords.size();
      
      initialA1Mjd = scenarioEpochA1Mjd + ephemRecords[0].timeFromEpoch / GmatTimeConstants::SECS_PER_DAY;
      finalA1Mjd = scenarioEpochA1Mjd + ephemRecords[numRecords - 1].timeFromEpoch / GmatTimeConstants::SECS_PER_DAY;
      initialState = ephemRecords[0].theState;
      finalState = ephemRecords[numRecords - 1].theState;

      lastStateFound = true;

      cbName = GetCentralBody(0);
      csName = GetReferenceFrame(0);
      if (csName == "EME2000")
         csName = "J2000";
   }   
   return lastStateFound;
}


//------------------------------------------------------------------------------
// bool ReadDataRecords(int numRecordsToRead, int logOption)
//------------------------------------------------------------------------------
/**
 * Reads the header and checks for required elements, and loads data records
 *
 * @param logOption Flag used to trigger writing the data to the log and message
 *                  window (not used)
 *
 * @return true if state data was read, false if not
 */
//------------------------------------------------------------------------------
bool CCSDSEphemerisFile::ReadDataRecords(int logOption)
{
   //Map of tested coordinate systems to be updated when new systems have been tested and verified to work.
  //Add the central body as the key and then any coordinate systems verified for CCSDS-OEM files and that central body can be added in the value vector.
   std::map<std::string, std::vector<std::string>> testedCoordinateSystems =
   {
      {"Earth",  {"EME2000", "GRC", "TDR", "TOD"}},
      {"Luna",   {"EME2000"}},
      {"Mars",   {"EME2000"}},
      {"Sun",    {"EME2000"}}
   };

   std::vector<std::string> universallySupportedFrames = { "EME2000"};

   bool retval = false;
   retval = fileReader->SetFile(ccsdsFileNameForRead);

   Integer numOfSegs = fileReader->GetNumberOfSegments();
   Real startEp = -1.0;
   
   theEphem.clear();
   ephemRecords.clear();

   for (Integer i = 0; i < numOfSegs; ++i)
   {
      CCSDSEMSegment* ccsdsSegment = fileReader->GetSegment(i);
      Integer numOfDataPoints = ccsdsSegment->GetNumberOfDataPoints();
      
      Segment newsegment;
      for (Integer index = 0; index < numOfDataPoints; ++index)
      {
         // Get epoch and state from CCSDS segment
         Real epoch;
         Rvector state;
         ccsdsSegment->GetEpochAndData(index, epoch, state);
         if (startEp == -1.0)
            startEp = epoch;

         // store epoch and state to Ephemeris' Segment
         EphemPoint point;
         point.theEpoch = epoch;
         point.posvel = Rvector6(state[0], state[1], state[2], state[3], state[4], state[5]);
         newsegment.points.push_back(point);

         // Store time from start peoch and state to ephemRecords
         EphemData eData;
         eData.timeFromEpoch = (point.theEpoch - startEp) * GmatTimeConstants::SECS_PER_DAY;
         for (Integer k = 0; k < 6; ++k)
            eData.theState[k] = point.posvel(k);
         ephemRecords.push_back(eData);
      }
      
      newsegment.segStart = newsegment.points[0].theEpoch;
      newsegment.segEnd = newsegment.points[newsegment.points.size() - 1].theEpoch;
      theEphem.push_back(newsegment);
   }

   a1StartEpoch = theEphem[0].points[0].theEpoch;
   a1EndEpoch = theEphem[theEphem.size() - 1].points[theEphem[theEphem.size() - 1].points.size() - 1].theEpoch;
   scenarioEpochA1Mjd = a1StartEpoch;

   interpolatorName = fileReader->GetSegment(0)->GetStringMetaData("INTERPOLATION");
   if (interpolatorName == "Lagrange")
      useHermite = false;

   order = fileReader->GetSegment(0)->GetIntegerMetaData("INTERPOLATION_DEGREE");
   
   std::string cb = GetCentralBody();
   if (cb == "Moon")
      cb = "Luna";
   std::string coordinates = GetReferenceFrame();
   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
   if (runmode != GmatGlobal::TESTING) {
      bool validCoordinateSystem = false;
      if(std::find(universallySupportedFrames.begin(), universallySupportedFrames.end(), coordinates) != universallySupportedFrames.end())
         validCoordinateSystem = true;
      else if (testedCoordinateSystems.find(cb) != testedCoordinateSystems.end())
         if ((std::count(testedCoordinateSystems[cb].begin(), testedCoordinateSystems[cb].end(), coordinates)))
            validCoordinateSystem = true;
      
      if (!validCoordinateSystem)
      {
         if (cb == "Luna")
            cb = "Moon";
         //did not find coordinates in central body list
         std::string errmsg = "The coordinate system \"" + coordinates + "\" in use in ephemeris file \"" + ccsdsFileNameForRead + "\" is not supported for the central body \"" + cb + "\" for use by GMAT in a CCSDS-OEM ephemeris file.\n";
         throw UtilityException(errmsg);
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void GetStartAndEndEpochs(GmatEpoch& startEpoch, GmatEpoch& endEpoch,
//       std::vector<EphemData>** records)
//------------------------------------------------------------------------------
/**
 * Accesses the start and end epochs as contained in the header data
 *
 * @param startEpoch The start epoch
 * @param endEpoch The end epoch
 * @param records The ephem data
 */
//------------------------------------------------------------------------------
void CCSDSEphemerisFile::GetStartAndEndEpochs(GmatEpoch& startEpoch,
      GmatEpoch& endEpoch, std::vector<EphemData>** records)
{
   startEpoch = a1StartEpoch;
   endEpoch   = a1EndEpoch;

   *records   = &ephemRecords;
}

//------------------------------------------------------------------------------
// std::string GetCentralBody(Integer segmentNo)
//------------------------------------------------------------------------------
/**
 * Returns the central body from the file
 *
 * @param segmentNo - segment number you want the central body of
 */
 //------------------------------------------------------------------------------
std::string CCSDSEphemerisFile::GetCentralBody(Integer segmentNo)
{
   // Get segment with index given by segmentNo
   CCSDSEMSegment* segment;
   if (segmentNo == -1)
      segment = fileReader->GetSegment();
   else
      segment = fileReader->GetSegment(segmentNo);

   // Get central body name associate with this data segment
   std::string cbName = segment->GetStringMetaData("CENTER_NAME");
   cbName = GmatStringUtil::ToUpper(cbName.substr(0, 1)) + GmatStringUtil::ToLower(cbName.substr(1));   // convert "LUNA", "EARTH", etc to "Luna", "Earth", etc

   return cbName;
}

//------------------------------------------------------------------------------
// std::string GetReferenceFrame(Integer segmentNo)
//------------------------------------------------------------------------------
/**
 * Returns the refrence frame from the file
 *
 * @param segmentNo - segment number you want the central body of
 */
 //------------------------------------------------------------------------------
std::string CCSDSEphemerisFile::GetReferenceFrame(Integer segmentNo)
{
   // Get segment with index given by segmentNo
   CCSDSEMSegment* segment;
   if (segmentNo == -1)
      segment = fileReader->GetSegment();
   else
      segment = fileReader->GetSegment(segmentNo);

   // Get name of the reference frame associated with this data segment
   std::string frameName = segment->GetStringMetaData("REF_FRAME");

   return frameName;
}

void CCSDSEphemerisFile::GetInterpolationInfoFromSegment(Integer segmentNo)
{
   // Get segment with index given by segmentNo
   CCSDSEMSegment* segment;
   if (segmentNo == -1)
      segment = fileReader->GetSegment();
   else
      segment = fileReader->GetSegment(segmentNo);

   // Get central body name associate with this data segment
   order = segment->GetIntegerMetaData("INTERPOLATION_DEGREE");
   std::string interpolationMethod = segment->GetStringMetaData("INTERPOLATION");
   if (GmatStringUtil::ToLower(interpolationMethod) == "hermite")
      useHermite = true;
   else 
      useHermite = false;
}

