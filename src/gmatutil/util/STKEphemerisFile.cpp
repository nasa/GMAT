//------------------------------------------------------------------------------
//                                  STKEphemerisFile
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
// Created: 2016.01.13
//
/**
 * Writes a spacecraft orbit ephemeris to a file in STK format.
 */
//------------------------------------------------------------------------------

#include "STKEphemerisFile.hpp"
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

//#define DEBUG_STK_FILE
//#define DEBUG_INITIALIZE
//#define DEBUG_WRITE_DATA_SEGMENT
//#define DEBUG_FINALIZE
//#define DEBUG_INITIAL_FINAL
//#define DEBUG_WRITE_POSVEL
//#define DEBUG_WRITE_COVPOSVEL
//#define DEBUG_DISTANCEUNIT

//----------------------------
// static data
//----------------------------

//----------------------------
// public methods
//----------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
STKEphemerisFile::STKEphemerisFile() :
   stkFileNameForRead  (""),
   stkFileNameForWrite (""),
   writeFinalized      (false)
{
   theTimeConverter = TimeSystemConverter::Instance();
   InitializeData();
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
STKEphemerisFile::STKEphemerisFile(const STKEphemerisFile &copy) :
   stkFileNameForRead  (copy.stkFileNameForRead),
   stkFileNameForWrite (copy.stkFileNameForWrite),
   writeFinalized      (false)
{
   theTimeConverter = TimeSystemConverter::Instance();
   InitializeData();
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
STKEphemerisFile& STKEphemerisFile::operator=(const STKEphemerisFile &copy)
{
   if (&copy == this)
      return *this;
   
   stkFileNameForRead = copy.stkFileNameForRead;
   stkFileNameForWrite = copy.stkFileNameForWrite;
   writeFinalized = false;
   InitializeData();

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
STKEphemerisFile::~STKEphemerisFile()
{
   if (stkInStream.is_open())
      stkInStream.close();
   
   if (stkOutStream.is_open())
   {
      stkOutStream.flush();
      stkOutStream.close();
   }
}

//------------------------------------------------------------------------------
// void InitializeData()
//------------------------------------------------------------------------------
/**
 * Clears header information.
 */
//------------------------------------------------------------------------------
void STKEphemerisFile::InitializeData()
{
   #ifdef DEBUG_STK_EPHEMFILE_INIT
   MessageInterface::ShowMessage("STKEphemerisFile::InitializeData() entered\n");
   #endif
   
   firstTimeWriting = true;
   openForTempOutput = true;
   
   scenarioEpochA1Mjd  = 0.0;
   coordinateSystemEpochA1Mjd = 0.0;
   beginSegmentTime = 0.0;
   lastEpochWritten = -999.999;

   beginSegmentArray.clear();
   numberOfSegmentPoints = 0;
   numberOfEphemPoints = 0;
   numberOfCovPoints = 0;
   interpolationOrder = 0;
   
   scenarioEpochUtcGreg = "";
   interpolationMethod = "";
   centralBody = "";
   coordinateSystem = "";
   coordinateSystemEpochStr = "";
   // STK ephems default to meters
   distanceUnit = "Meters";
   ephemTypeForRead = "";
   ephemTypeForWrite = "";
   ephemCovTypeForWrite = "";
   writeCov = false;
   stkTempFileName = "";
   stkTempCovFileName = "";
   numEphemPointsBegPos = 0;
   
   initialSecsFromEpoch = 0.0;
   finalSecsFromEpoch   = 0.0;
   writeFinalized       = false;

   warnInterpolationDegradation = true;
}

//------------------------------------------------------------------------------
// bool OpenForRead(const std::string &filename, const std::string &ephemType,
//                  const std::string &ephemCovType)
//------------------------------------------------------------------------------
/**
 * Opens STK ephemeris (.e) file for reading.
 *
 * @filename File name to open
 * @ephemType Ephemeris type to read, at this time only "TimePos" or "TimePosVel"
 *               is allowed
 * @ephemCovType Ephemeris covariance type to read, at this time only "", "TimePos" or "TimePosVel"
 *               is allowed
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::OpenForRead(const std::string &filename,
                                   const std::string &ephemType,
                                   const std::string &ephemCovType)
{
   #ifdef DEBUG_STK_FILE
   MessageInterface::ShowMessage
      ("STKEphemerisFile::OpenForRead() entered, filename='%s'\n", filename.c_str());
   #endif
   
   bool retval = false;
   
   // Check ephem type
   // Currently only TimePos and TimePosVel are allowed
   /// @todo Do we allow TimePos?  ReadDataRecords seems to say no
   if (ephemType != "TimePos" && ephemType != "TimePosVel")
   {
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::OpenForRead() *** INTERNAL ERROR *** "
                    "Only TimePos or TimePosVel is valid for read on STK "
                    "ephemeris file '%s'.", stkFileNameForRead.c_str());
      throw ue;
   }

   // Check covariance ephem type
   // Currently only "", TimePos, and TimePosVel are allowed
   if (ephemCovType != "" && ephemCovType != "TimePos" && ephemCovType != "TimePosVel")
   {
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::OpenForRead() *** INTERNAL ERROR *** "
         "Only "", TimePos, or TimePosVel is valid to read covariance on STK "
         "ephemeris file '%s'.", stkFileNameForRead.c_str());
      throw ue;
   }
   
   if (stkInStream.is_open())
      stkInStream.close();
   
   stkFileNameForRead = filename;
   ephemTypeForRead = ephemType;
   stkInStream.open(stkFileNameForRead.c_str());
   
   // Base class setting
   ephemerisFileName = filename;

   if (stkInStream.is_open())
   {
      retval = true;
      #ifdef DEBUG_STK_FILE
      MessageInterface::ShowMessage
         ("   Successfully opened '%s' for reading\n", stkFileNameForRead.c_str());
      #endif
   }
   else
   {
      retval = false;
      #ifdef DEBUG_STK_FILE
      MessageInterface::ShowMessage
         ("   Failed open '%s' for reading\n", stkFileNameForRead.c_str());
      #endif
   }

   if (openForTempOutput && writeCov)
   {
      if (stkCovInStream.is_open())
         stkCovInStream.close();

      std::string stkCovFileNameForRead = filename + ".cov";
      stkCovInStream.open(stkCovFileNameForRead.c_str());

      if (stkCovInStream.is_open())
      {
         #ifdef DEBUG_STK_FILE
         MessageInterface::ShowMessage
            ("   Successfully opened '%s' for reading\n", stkCovFileNameForRead.c_str());
         #endif
      }
      else
      {
         retval = false;
         #ifdef DEBUG_STK_FILE
         MessageInterface::ShowMessage
            ("   Failed open '%s' for reading\n", stkCovFileNameForRead.c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_STK_FILE
   MessageInterface::ShowMessage
      ("STKEphemerisFile::OpenForRead() returning %d\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool OpenForWrite(const std::string &filename, const std::string &ephemType,
//                   const std::string &ephemCovType)
//------------------------------------------------------------------------------
/**
 * Opens STK ephemeris (.e) file for writing.
 *
 * @filename File name to open
 * @ephemType Ephemeris type to write, at this time only "TimePos" or "TimePosVel"
 *               is allowed
 * @ephemCovType Ephemeris covariance type to read, at this time only "", "TimePos" or "TimePosVel"
 *               is allowed
  */
//------------------------------------------------------------------------------
bool STKEphemerisFile::OpenForWrite(const std::string &filename,
                                    const std::string &ephemType,
                                    const std::string &ephemCovType)
{
   #ifdef DEBUG_STK_FILE
   MessageInterface::ShowMessage
      ("STKEphemerisFile::OpenForWrite() entered, filename='%s', openForTempOutput=%d\n",
       filename.c_str(), openForTempOutput);
   #endif
   
   bool retval = false;
   
   // Check ephem type
   // Currently only TimePos and TimePosVel are allowed
   if (ephemType != "TimePos" && ephemType != "TimePosVel")
   {
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::OpenForWrite() *** INTERNAL ERROR *** "
                    "Only TimePos or TimePosVel is valid for writing to STK "
                    "ephemeris file '%s'.", stkFileNameForWrite.c_str());
      throw ue;
   }

   // Check covariance ephem type
   // Currently only "", TimePos, and TimePosVel are allowed
   if (ephemCovType != "" && ephemCovType != "TimePos" && ephemCovType != "TimePosVel")
   {
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::OpenForWrite() *** INTERNAL ERROR *** "
         "Only "", TimePos, or TimePosVel is valid for writing covariance to STK "
         "ephemeris file '%s'.", stkFileNameForWrite.c_str());
      throw ue;
   }
   
   if (stkOutStream.is_open())
      stkOutStream.close();
   if (stkCovOutStream.is_open())
      stkCovOutStream.close();
   
   stkFileNameForWrite = filename;
   ephemTypeForWrite = ephemType;
   ephemCovTypeForWrite = ephemCovType;
   writeCov = ephemCovTypeForWrite != "";
   
   stkOutStream.open(stkFileNameForWrite.c_str());
   
   // Base class setting
   ephemerisFileName = filename;

   if (stkOutStream.is_open())
   {
      retval = true;
      #ifdef DEBUG_STK_FILE
      MessageInterface::ShowMessage
         ("   Successfully opened '%s' for writing\n", stkFileNameForWrite.c_str());
      #endif
   }
   else
   {
      retval = false;
      #ifdef DEBUG_STK_FILE
      MessageInterface::ShowMessage
         ("   Failed open '%s' for writing\n", stkFileNameForWrite.c_str());
      #endif
   }
   
   // Open temporary file to write STK ephemeris since header data needs to be
   // updated after final data
   if (openForTempOutput)
   {
      stkOutStream.close();
      std::string tempPath = GmatFileUtil::GetTemporaryDirectory();
      std::string fileNameNoPath = GmatFileUtil::ParseFileName(filename);
      stkTempFileName = tempPath + fileNameNoPath;
      #ifdef DEBUG_STK_FILE
      MessageInterface::ShowMessage
         ("          tempPath='%s'\n   stkTempFileName='%s'\n", tempPath.c_str(),
          stkTempFileName.c_str());
      #endif
      stkOutStream.open(stkTempFileName.c_str());

      if (ephemCovType != "")
      {
         stkTempCovFileName = tempPath + fileNameNoPath + ".cov";
         #ifdef DEBUG_STK_FILE
            MessageInterface::ShowMessage
            ("          tempPath='%s'\n   stkTempCovFileName='%s'\n", tempPath.c_str(),
             stkTempCovFileName.c_str());
         #endif
         stkCovOutStream.open(stkTempCovFileName.c_str());
      }
   }
   
   #ifdef DEBUG_STK_FILE
   MessageInterface::ShowMessage
      ("STKEphemerisFile::OpenForWrite() returning %d\n", retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// void CloseForRead()
//------------------------------------------------------------------------------
void STKEphemerisFile::CloseForRead()
{
   if (stkInStream.is_open())
      stkInStream.close();
}

//------------------------------------------------------------------------------
// void CloseForWrite()
//------------------------------------------------------------------------------
void STKEphemerisFile::CloseForWrite()
{
   if (stkOutStream.is_open())
      stkOutStream.close();

   if (stkCovOutStream.is_open())
      stkCovOutStream.close();
}

//------------------------------------------------------------------------------
// bool GetInitialAndFinalStates(Real &initialA1Mjd, Real &finalA1Mjd,
//                               Rvector6 &initialState, Rvector6 &finalState,
//                               std::string &cbName, std::string &csName)
//------------------------------------------------------------------------------
/**
 * Retrieves initial and final state from STK ephem file. Assumes ephem file
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
bool STKEphemerisFile::GetInitialAndFinalStates(Real &initialA1Mjd, Real &finalA1Mjd,
                                                Rvector6 &initialState, Rvector6 &finalState,
                                                std::string &cbName, std::string &csName)
{
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("STKEphemerisFile::GetInitialAndFinalState() entered\n");
   #endif
   
   // Set defaults for output
   cbName = "Earth";
   csName = "J2000";
   
   bool lastStateFound = false;
   
   if (ReadDataRecords())
   {
      int numRecords = ephemRecords.size();
      initialA1Mjd = scenarioEpochA1Mjd + ephemRecords[0].timeFromEpoch / 86400.0;
      finalA1Mjd = scenarioEpochA1Mjd + ephemRecords[numRecords - 1].timeFromEpoch / 86400.0;
      initialState = ephemRecords[0].theState;
      finalState = ephemRecords[numRecords - 1].theState;
      
      cbName = centralBody;
      csName = coordinateSystem;
      lastStateFound = true;
   }
   
   return lastStateFound;
}

//------------------------------------------------------------------------------
// void SetVersion(const std::string &version)
//------------------------------------------------------------------------------
void STKEphemerisFile::SetVersion(const std::string &version)
{
   #ifdef DEBUG_VERSION
   MessageInterface::ShowMessage
      ("STKEphemerisFile::SetVersion() entered, stkVersion='%s'\n",
       stkVersion.c_str());
   #endif
   
   stkVersion = version;
}

//------------------------------------------------------------------------------
// void SetInterpolationOrder(Integer order)
//------------------------------------------------------------------------------
void STKEphemerisFile::SetInterpolationOrder(Integer order)
{
   interpolationOrder = order;
}

//------------------------------------------------------------------------------
// bool SetHeaderForWriting(const std::string &fieldName,
//                          const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets STK header data for writing. It does not validate the input value.
 *
 * @param fieldName Header field name to be set
 * @param value Value to set to the field
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::SetHeaderForWriting(const std::string &fieldName,
                                           const std::string &value)
{
   if (fieldName == "Version")
      stkVersion = value;
   else if (fieldName == "InterpolationMethod")
      interpolationMethod = value;
   else if (fieldName == "CentralBody")
      centralBody = value;
   else if (fieldName == "CoordinateSystem")
      coordinateSystem = value;
   else if (fieldName == "CoordinateSystemEpoch")
      coordinateSystemEpochStr = value;
   else if (fieldName == "DistanceUnit")
      distanceUnit = value;
   else
      throw UtilityException
         ("The field \"" + fieldName + "\" is not valid STK header field.\n"
          "Valid fields are: TBS");
   
   return true;
}

//------------------------------------------------------------------------------
// bool WriteHeader()
//------------------------------------------------------------------------------
/**
 * Formats and writes header to a file. If output stream is not opened,
 * it just returns false.
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::WriteHeader()
{
   #ifdef DEBUG_WRITE_HEADER
   MessageInterface::ShowMessage
      ("STKEphemerisFile::WriteHeader() entered, stkVersion='%s'\n", stkVersion.c_str());
   #endif
   
   if (!stkOutStream.is_open())
   {
      #ifdef DEBUG_WRITE_HEADER
      MessageInterface::ShowMessage
         ("STKEphemerisFile::WriteHeader() returning false, stkOutStream is not opened\n");
      #endif
      return false;
   }
   
   std::string ephemFormat = "Ephemeris" + ephemTypeForWrite;
   scenarioEpochUtcGreg = A1ModJulianToUtcGregorian(scenarioEpochA1Mjd, 1);
   
   std::stringstream ss("");
   ss << stkVersion << std::endl;
   ss << "# WrittenBy    GMAT " << GmatGlobal::Instance()->GetGmatVersion() << std::endl;
   ss << "BEGIN Ephemeris" << std::endl;
   stkOutStream << ss.str();
   
   numEphemPointsBegPos = stkOutStream.tellp();
   #ifdef DEBUG_WRITE_HEADER
   MessageInterface::ShowMessage("numEphemPointsBegPos=%ld\n", (long)numEphemPointsBegPos);
   #endif
   
   // Clear stream contents
   ss.str("");
   
   ss << "NumberOfEphemerisPoints " << numberOfEphemPoints << std::endl;
   if (writeCov)
   {
      ss << "NumberOfCovariancePoints " << numberOfCovPoints << std::endl;
      ss << "CovarianceFormat LowerTriangular" << std::endl;
   }
   ss << "ScenarioEpoch           " << scenarioEpochUtcGreg << std::endl;
   
   // Write interpolation info if not blank
   if (interpolationMethod != "")
   {
      // Figure out actual interpolation order
      Integer actualInterpOrder = interpolationOrder;
      if (numberOfEphemPoints <= interpolationOrder)
         actualInterpOrder = numberOfEphemPoints - 1;
      if (numberOfEphemPoints == 1)
         actualInterpOrder = 1;
      ss << "InterpolationMethod     " << interpolationMethod << std::endl;
      ss << "InterpolationOrder      " << actualInterpOrder << std::endl;
   }
   
   ss << "CentralBody             " << centralBody << std::endl;
   ss << "CoordinateSystem        " << coordinateSystem << std::endl;
   
   // GMAT writes in km only for now
   ss << "DistanceUnit            " << distanceUnit << std::endl;

   // Write begin segment times if not empty
   if (includeEventBoundaries){
       if (!beginSegmentArray.empty())
       {
           char strBuff[200];
           ss << "BEGIN SegmentBoundaryTimes" << std::endl;
           Integer numSegments = beginSegmentArray.size();
           if (numberOfSegmentPoints == 0)
              numSegments--;

           for (unsigned int i = 0; i < numSegments; i++)
           {
               sprintf(strBuff, "   %1.15e\n", beginSegmentArray[i]);
               ss << strBuff;
           }
           ss << "END SegmentBoundaryTimes" << std::endl;
       }
   }
   
   ss << std::endl;
   ss << ephemFormat << std::endl;
   ss << std::endl;
   
   stkOutStream << ss.str();
   stkOutStream.flush();
   
   #ifdef DEBUG_WRITE_HEADER
   MessageInterface::ShowMessage
      ("STKEphemerisFile::WriteHeader() returning true\n");
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// bool WriteBlankLine()
//------------------------------------------------------------------------------
/**
 * Writes blank line to a file.
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::WriteBlankLine()
{
   if (!stkOutStream.is_open())
      return false;

   if (writeCov)
      if (!stkCovOutStream.is_open())
         return false;
   
   stkOutStream << std::endl;
   stkOutStream.flush();

   if (writeCov)
   {
      stkCovOutStream << std::endl;
      stkCovOutStream.flush();
   }

   return true;
}

//------------------------------------------------------------------------------
// bool WriteString(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Writes input string to a file.
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::WriteString(const std::string &str)
{
   #ifdef DEBUG_WRITE_STRING
   MessageInterface::ShowMessage
      ("STKEphemerisFile::WriteString() entered, str='%s'\n", str.c_str());
   #endif
   
   if (!stkOutStream.is_open())
      return false;
   
   stkOutStream << str << std::endl;
   stkOutStream.flush();
   
   #ifdef DEBUG_WRITE_STRING
   MessageInterface::ShowMessage
      ("STKEphemerisFile::WriteString() returning true\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool WriteDataSegment(const EpochArray &epochArray, const StateArray &stateArray, 
//                       const std::vector<Rvector*> &covArray, bool canFinalize = false);
//------------------------------------------------------------------------------
bool STKEphemerisFile::WriteDataSegment(const EpochArray &epochArray,
                                        const StateArray &stateArray, 
                                        const std::vector<Rvector*> &covArray,
                                        bool canFinalize)
{
   Integer numPoints = stateArray.size();
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("============================================================\n"
       "STKEphemerisFile::WriteDataSegment() entered, numPoints=%d, canFinalize=%d, "
       "firstTimeWriting=%d\n", numPoints, canFinalize, firstTimeWriting);
   #endif
   
   if (numPoints == 0)
   {
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      MessageInterface::ShowMessage
         ("STKEphemerisFile::WriteDataSegment() just returning true, data size is zero\n");
      #endif
      return true;
   }
   else if (numPoints != stateArray.size())
   {
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::WriteDataSegment() *** INTERNAL ERROR *** "
                    "Received different number of timeS and stateS. size of time "
                    "array: %d, size of state array: %d\n", epochArray.size(),
                    stateArray.size());
      throw ue;
   }
   
   // If first time writing, save scenario epoch
   if (firstTimeWriting)
   {
      scenarioEpochA1Mjd = (epochArray[0])->GetReal();
      openForTempOutput = true;
   }
   
   if (ephemTypeForWrite == "TimePosVel")
      WriteTimePosVel(epochArray, stateArray);
   else if (ephemTypeForWrite == "TimePos")
      WriteTimePos(epochArray, stateArray);
   else
   {
      // ephemType has already validated in OpenFile but check anyway
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::WriteDataSegment() *** INTERNAL ERROR *** "
                    "Only TimePos or TimePosVel is valid for read on STK "
                    "ephemeris file '%s'.", stkFileNameForWrite.c_str());
      throw ue;
   }

   if (ephemCovTypeForWrite == "TimePosVel")
      WriteCovTimePosVel(epochArray, covArray);
   else if (ephemCovTypeForWrite == "TimePos")
      WriteCovTimePos(epochArray, covArray);
   else if (ephemCovTypeForWrite != "")
   {
      // ephemType has already validated in OpenFile but check anyway
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::WriteDataSegment() *** INTERNAL ERROR *** "
         "Only TimePos or TimePosVel is valid for read on STK "
         "ephemeris file '%s'.", stkFileNameForWrite.c_str());
      throw ue;
   }
   
   // If final data segment received, write end ephemeris
   if (canFinalize)
   {
      FinalizeEphemeris();
   }
   else
   {
      // Indicate new segment starting by writing blank line and the last data
      if (includeEventBoundaries)
      {
          WriteBlankLine();
      }

      #ifdef DEBUG_WRITE_DATA_SEGMENT
      WriteString("# ============================== new segment");
      #endif

      Integer last = numPoints - 1;
      Real lastEpoch = (epochArray[last])->GetReal();
      
      // Don't write last line of code from previous segment
//      WriteTimePosVel(lastEpoch, stateArray[last]);
      if (firstTimeWriting)
         beginSegmentArray.push_back(0.0);
      beginSegmentTime = (lastEpoch - scenarioEpochA1Mjd) * 86400.0;
      beginSegmentArray.push_back(beginSegmentTime);
      numberOfSegmentPoints = 0;
   }
   
   firstTimeWriting = false;
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("STKEphemerisFile::WriteDataSegment() leavng\n");
   #endif
   return true;
}

//----------------------------
// protected methods
//----------------------------

//------------------------------------------------------------------------------
// bool GetEpochAndState(const std::string &line, Real &epoch, Rvector6 &state)
//------------------------------------------------------------------------------
bool STKEphemerisFile::GetEpochAndState(const std::string &line, Real &epoch, Rvector6 &state)
{
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("STKEphemerisFile::GetEpochAndState() entered\n   line =\n   '%s'\n", line.c_str());
   #endif
   
   // Parse line
   bool retval = false;
   StringArray items = GmatStringUtil::SeparateBy(line, " ", false, false, false);
   if (items.size() == 7)
   {
      retval = true;
      Real rval;
      #ifdef DEBUG_INITIAL_FINAL
      for (unsigned int i = 0; i < items.size(); i++)
         MessageInterface::ShowMessage("   items[%d] = '%s'\n", i, items[i].c_str());
      #endif
      if (GmatStringUtil::ToReal(items[0], rval))
         epoch = rval;
      else
      {
         retval = false;
         MessageInterface::ShowMessage("*** ERROR *** '%s' is not a valid real number\n", items[0].c_str());
      }
      for (unsigned int i = 1; i < items.size(); i++)
      {
         if (GmatStringUtil::ToReal(items[i], rval))
            state[i-1] = rval;
         else
         {
            retval = false;
            MessageInterface::ShowMessage("*** ERROR *** '%s' is not a valid real number\n", items[i].c_str());
         }
      }
   }
   
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("STKEphemerisFile::GetEpochAndState() returing %d\n", retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// std::string GetLastLine()
//------------------------------------------------------------------------------
std::string STKEphemerisFile::GetLastLine()
{
   std::ifstream::pos_type pos = stkInStream.tellg();
   std::ifstream::pos_type lastPos;
   while (stkInStream >> std::ws && IgnoreLine(lastPos))
      pos = lastPos;
   
   stkInStream.clear();
   stkInStream.seekg(pos);
   
   std::string line;
   GmatFileUtil::GetLine(&stkInStream, line);
   return line;
}

//------------------------------------------------------------------------------
// std::istream& IgnoreLine(std::ifstream::pos_type& pos)
//------------------------------------------------------------------------------
std::istream& STKEphemerisFile::IgnoreLine(std::ifstream::pos_type& pos)
{
   pos = stkInStream.tellg();
   return stkInStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//------------------------------------------------------------------------------
// void WriteTimePosVel(const EpochArray &epochArray, const StateArray &stateArray)
//------------------------------------------------------------------------------
/**
 * Writes STK ephemeris in EphemerisTimePosVel format
 */
//------------------------------------------------------------------------------
void STKEphemerisFile::WriteTimePosVel(const EpochArray &epochArray,
                                       const StateArray &stateArray)
{
   #ifdef DEBUG_WRITE_POSVEL
   MessageInterface::ShowMessage
      ("STKEphemerisFile::WriteTimePosVel() entered, epochArray.size()=%d, "
       "stateArray.size()=%d\n", epochArray.size(), stateArray.size());
   #endif
   
   Integer numPoints = stateArray.size();
   
   // Write out to stream
   for (int i = 0; i < numPoints; i++)
   {
      // For multiple segments, end epoch of previous segment may be the same as
      // beginning epoch of new segment, so check for duplicate epoch and use the
      // state of new epoch since any maneuver or some spacecraft property update
      // may happened.
      Real epoch = (epochArray[i])->GetReal();
      if (!includeEventBoundaries && (epoch == lastEpochWritten && i > 1))
      {
          continue;
      }
      
      WriteTimePosVel(epoch, stateArray[i]);
   }
   
   #ifdef DEBUG_WRITE_POSVEL
   MessageInterface::ShowMessage
      ("STKEphemerisFile::WriteTimePosVel() leaving, epochArray.size()=%d, "
       "stateArray.size()=%d\n", epochArray.size(), stateArray.size());
   #endif
}

//------------------------------------------------------------------------------
// void WriteTimePosVel(Real epoch, const Rvector6 *state)
//------------------------------------------------------------------------------
void STKEphemerisFile::WriteTimePosVel(Real epoch, const Rvector6 *state)
{
   // Format output using scientific notation
   const Real *outState = state->GetDataVector();
   Real timeIntervalInSecs = (epoch - scenarioEpochA1Mjd) * 86400.0;
   char strBuff[200];

   #ifdef DEBUG_DISTANCEUNIT
   MessageInterface::ShowMessage
      ("WriteTimePosVel DistanceUnit: %s\n", distanceUnit.c_str());
   #endif

   if (distanceUnit == "Meters")
   {
      sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
               timeIntervalInSecs, outState[0]*1000.0, outState[1]*1000.0,
               outState[2]*1000.0, outState[3]*1000.0, outState[4]*1000.0,
               outState[5]*1000.0);

   }
   else
   {
      sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
               timeIntervalInSecs, outState[0], outState[1], outState[2], outState[3],
               outState[4], outState[5]);
   }
   
   if (includeEventBoundaries || (!includeEventBoundaries && (epoch != lastEpochWritten)))
   {
      stkOutStream << strBuff;
      lastEpochWritten = epoch;
      numberOfEphemPoints++;
      numberOfSegmentPoints++;
   }

   #ifdef DEBUG_WRITE_POSVEL
   std::string epochStr = A1ModJulianToUtcGregorian(epoch, 2);
   MessageInterface::ShowMessage("   %s  % 1.15e\n", epochStr.c_str(), outState[0]);
   #endif
}

//------------------------------------------------------------------------------
// void WriteTimePos(const EpochArray &epochArray, const StateArray &stateArray)
//------------------------------------------------------------------------------
/**
 * Writes STK ephemeris in EphemerisTimePos format
 */
//------------------------------------------------------------------------------
void STKEphemerisFile::WriteTimePos(const EpochArray &epochArray,
                                    const StateArray &stateArray)
{
   Integer numPoints = stateArray.size();
   
   // Write out to stream
   for (int i = 0; i < numPoints; i++)
   {
      // For multiple segments, end epoch of previous segment may be the same as
      // beginning epoch of new segment, so check for duplicate epoch and use the
      // state of new epoch since any maneuver or some spacecraft property update
      // may happened.
      Real epoch = (epochArray[i])->GetReal();
      if (!includeEventBoundaries && (epoch == lastEpochWritten))
         continue;
      
      WriteTimePos(epoch, stateArray[i]);
      
      lastEpochWritten = epoch;
      numberOfEphemPoints++;
      numberOfSegmentPoints++;
   }
}


//------------------------------------------------------------------------------
// void WriteTimePos(Real epoch, const Rvector6 *state)
//------------------------------------------------------------------------------
void STKEphemerisFile::WriteTimePos(Real epoch, const Rvector6 *state)
{
   // Format output using scientific notation
   const Real *outState = state->GetDataVector();
   Real timeIntervalInSecs = (epoch - scenarioEpochA1Mjd) * 86400.0;
   char strBuff[200];
   sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e\n",
           timeIntervalInSecs, outState[0], outState[1], outState[2]);
   stkOutStream << strBuff;
   
   #ifdef DEBUG_WRITE_POSVEL
   std::string epochStr = A1ModJulianToUtcGregorian(epoch, 2);
   MessageInterface::ShowMessage("   %s  % 1.15e\n", epochStr.c_str(), outState[0]);
   #endif
}

//------------------------------------------------------------------------------
// void WriteCovTimePosVel(const EpochArray &epochArray, const std::vector<Rvector*> &covArray)
//------------------------------------------------------------------------------
/**
 * Writes STK ephemeris in EphemerisTimePosVel format
 */
 //------------------------------------------------------------------------------
void STKEphemerisFile::WriteCovTimePosVel(const EpochArray &epochArray,
                                          const std::vector<Rvector*> &covArray)
{
#ifdef DEBUG_WRITE_COVPOSVEL
   MessageInterface::ShowMessage
   ("STKEphemerisFile::WriteTimePosVel() entered, epochArray.size()=%d, "
      "covArray.size()=%d\n", epochArray.size(), covArray.size());
#endif

   Integer numPoints = covArray.size();

   // Write out to stream
   for (int i = 0; i < numPoints; i++)
   {
      // For multiple segments, end epoch of previous segment may be the same as
      // beginning epoch of new segment, so check for duplicate epoch and use the
      // state of new epoch since any maneuver or some spacecraft property update
      // may happened.
      Real epoch = (epochArray[i])->GetReal();
      if (!includeEventBoundaries && (epoch == lastEpochWritten && i > 1))
      {
         continue;
      }

      WriteCovTimePosVel(epoch, covArray[i]);
   }

#ifdef DEBUG_WRITE_COVPOSVEL
   MessageInterface::ShowMessage
   ("STKEphemerisFile::WriteTimePosVel() leaving, epochArray.size()=%d, "
      "covArray.size()=%d\n", epochArray.size(), covArray.size());
#endif
}

//------------------------------------------------------------------------------
// void WriteCovTimePosVel(Real epoch, const Rvector *cov)
//------------------------------------------------------------------------------
void STKEphemerisFile::WriteCovTimePosVel(Real epoch, const Rvector *cov)
{
   // Format output using scientific notation
   const Real *outCov = cov->GetDataVector();
   Real timeIntervalInSecs = (epoch - scenarioEpochA1Mjd) * 86400.0;
   char strBuff[200];
   std::stringstream ss("");

   if (distanceUnit == "Meters")
   {
      sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
         timeIntervalInSecs, outCov[0]*1e6, outCov[1]*1e6, outCov[2]*1e6,
         outCov[3]*1e6, outCov[4]*1e6, outCov[5]*1e6, outCov[6]*1e6);
      ss << strBuff;

      sprintf(strBuff, "%21s  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
         " ", outCov[7]*1e6, outCov[8]*1e6, outCov[9]*1e6, outCov[10]*1e6,
         outCov[11]*1e6, outCov[12]*1e6, outCov[13]*1e6);
      ss << strBuff;

      sprintf(strBuff, "%21s  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
         " ", outCov[14]*1e6, outCov[15]*1e6, outCov[16]*1e6, outCov[17]*1e6,
         outCov[18]*1e6, outCov[19]*1e6, outCov[20]*1e6);
      ss << strBuff;
   }
   else
   {
      sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
         timeIntervalInSecs, outCov[0], outCov[1], outCov[2],
         outCov[3], outCov[4], outCov[5], outCov[6]);
      ss << strBuff;

      sprintf(strBuff, "%21s  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
         " ", outCov[7], outCov[8], outCov[9], outCov[10],
         outCov[11], outCov[12], outCov[13]);
      ss << strBuff;

      sprintf(strBuff, "%21s  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
         " ", outCov[14], outCov[15], outCov[16], outCov[17],
         outCov[18], outCov[19], outCov[20]);
      ss << strBuff;
   }

   if (includeEventBoundaries || (!includeEventBoundaries && (epoch != lastEpochWritten)))
   {
      stkCovOutStream << ss.str();
      //lastEpochWritten = epoch;
      numberOfCovPoints++;
   }

#ifdef DEBUG_WRITE_COVPOSVEL
   std::string epochStr = A1ModJulianToUtcGregorian(epoch, 2);
   MessageInterface::ShowMessage("   %s  % 1.15e\n", epochStr.c_str(), outCov[0]);
#endif
}

//------------------------------------------------------------------------------
// void WriteCovTimePos(const EpochArray &epochArray, const std::vector<Rvector*> &covArray)
//------------------------------------------------------------------------------
/**
 * Writes STK ephemeris in EphemerisTimePos format
 */
 //------------------------------------------------------------------------------
void STKEphemerisFile::WriteCovTimePos(const EpochArray &epochArray,
                                       const std::vector<Rvector*> &covArray)
{
   Integer numPoints = covArray.size();

   // Write out to stream
   for (int i = 0; i < numPoints; i++)
   {
      // For multiple segments, end epoch of previous segment may be the same as
      // beginning epoch of new segment, so check for duplicate epoch and use the
      // state of new epoch since any maneuver or some spacecraft property update
      // may happened.
      Real epoch = (epochArray[i])->GetReal();
      if (!includeEventBoundaries && (epoch == lastEpochWritten))
         continue;

      WriteCovTimePos(epoch, covArray[i]);

      //lastEpochWritten = epoch;
      numberOfCovPoints++;
   }
}


//------------------------------------------------------------------------------
// void WriteCovTimePos(Real epoch, const Rvector *cov)
//------------------------------------------------------------------------------
void STKEphemerisFile::WriteCovTimePos(Real epoch, const Rvector *cov)
{
   // Format output using scientific notation
   const Real *outCov = cov->GetDataVector();
   Real timeIntervalInSecs = (epoch - scenarioEpochA1Mjd) * 86400.0;
   char strBuff[200];

   if (distanceUnit == "Meters")
   {
      sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
         timeIntervalInSecs, outCov[0]*1e6, outCov[1]*1e6, outCov[2]*1e6,
         outCov[3]*1e6, outCov[4]*1e6, outCov[5]*1e6);
   }
   else
   {
      sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
         timeIntervalInSecs, outCov[0], outCov[1], outCov[2],
         outCov[3], outCov[4], outCov[5]);
   }

   stkCovOutStream << strBuff;

#ifdef DEBUG_WRITE_COVPOSVEL
   std::string epochStr = A1ModJulianToUtcGregorian(epoch, 2);
   MessageInterface::ShowMessage("   %s  % 1.15e\n", epochStr.c_str(), outCov[0]);
#endif
}


//------------------------------------------------------------------------------
// void FinalizeEphemeris()
//------------------------------------------------------------------------------
/**
 * Finalizes the ephemeris file.
 *
 * Write number of data points and end ephemeris keyword.
 */
//------------------------------------------------------------------------------
void STKEphemerisFile::FinalizeEphemeris()
{
   #ifdef DEBUG_FINALIZE
   MessageInterface::ShowMessage
      ("STKEphemerisFile::FinalizeEphemeris() entered, numberOfEphemPoints=%d, "
       "numEphemPointsBegPos=%ld\n   stkTempFileName='%s'", numberOfEphemPoints,
       (long)numEphemPointsBegPos, stkTempFileName.c_str());
   #endif
   
   // Close temp file and copy content to actual STK ephemeris file
   // after writing header data
   stkOutStream.close();   
   
   if (OpenForRead(stkTempFileName, "TimePosVel", ephemCovTypeForWrite))
   {
      openForTempOutput = false;
      OpenForWrite(stkFileNameForWrite, "TimePosVel", ephemCovTypeForWrite);
      WriteHeader();   
      
      // Read lines
      std::string line;
      while (!stkInStream.eof())
      {
         // Use cross-platform GetLine
         //GmatFileUtil::GetLine(&stkInStream, line);
         GmatFileUtil::GetLine(&stkInStream, line);
         stkOutStream << line << std::endl;
      }

      if (writeCov)
      {
         stkOutStream << "Covariance" << ephemCovTypeForWrite << std::endl << std::endl;

         while (!stkCovInStream.eof())
         {
            // Use cross-platform GetLine
            //GmatFileUtil::GetLine(&stkInStream, line);
            GmatFileUtil::GetLine(&stkCovInStream, line);
            stkOutStream << line << std::endl;
         }
      }

      // Write end ephemeris keyword
      stkOutStream << "END Ephemeris" << std::endl << std::endl;
      stkOutStream.flush();
      
      stkInStream.close();
      stkOutStream.close();
      
      // Delete temp file
      remove(stkTempFileName.c_str());
      remove(stkTempCovFileName.c_str());
      writeFinalized = true;
   }
   else
   {
      if (!writeFinalized)
         MessageInterface::ShowMessage("Failed to open temp file %s\n",
               stkTempFileName.c_str());
   }
         
   #ifdef DEBUG_FINALIZE
   MessageInterface::ShowMessage("There are %d segments\n", beginSegmentArray);
   for (unsigned int i = 0; i < beginSegmentArray.size(); i++)
      MessageInterface::ShowMessage("   %.12e\n", beginSegmentArray[i]);
   MessageInterface::ShowMessage("STKEphemerisFile::FinalizeEphemeris() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// std::string A1ModJulianToUtcGregorian(Real epochInDays, Integer format = 1)
//------------------------------------------------------------------------------
/**
 * Formats epoch in either in days or seconds to desired format.
 *
 * @param  epoch   Epoch in A1 days
 * @param  format  Desired output format [1]
 *                 1 = "01 Jan 2000 11:59:28.000"
 *                 2 = "2000-01-01T11:59:28.000"
 */
//------------------------------------------------------------------------------
std::string STKEphemerisFile::A1ModJulianToUtcGregorian(Real epochInDays, Integer format)
{
   if (epochInDays == -999.999)
      return "-999.999";
   
   Real toMjd;
   std::string epochStr;
   std::string fromType = "A1ModJulian";
   std::string toType = "UTCGregorian";
   
   // Convert current epoch to specified format
   theTimeConverter->Convert(fromType, epochInDays, "", toType, toMjd, epochStr, format);
   
   if (epochStr == "")
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** EphemerisWriter::ToUtcGregorian() Cannot convert epoch %.10f %s "
          "to UTCGregorian\n", epochInDays, "days");
      
      epochStr = "EpochError";
   }
   
   return epochStr;
}

//------------------------------------------------------------------------------
// bool UtcGregorianToA1ModJulian(const std::string &utcGreg, Real &a1Mjd)
//------------------------------------------------------------------------------
/**
 * Converts epoch in UTCGregorian to A1ModJulian format. It uses UTCGregorian
 * format of "01 Jan 2000 11:59:28.000."
 *
 * @param  utcGreg  Epoch in UTCGregorian
 * @param  a1Mjd    output epoch in A1ModJulian
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::UTCGregorianToA1ModJulian(const std::string &utcGreg, Real &a1Mjd)
{
   bool retval = true;
   Real fromMjd = -999.999;
   Real toMjd = -999.999;
   std::string epochStr;
   std::string fromType = "UTCGregorian";
   std::string toType = "A1ModJulian";
   std::string timetemp = utcGreg;
  
   // Check that the day of the month in the input epoch is given with two 
   // digits.  If only one was given, add a leading zero.
   if (utcGreg[1] == ' ')
   {
      timetemp = "0" + timetemp;
   }
   

   // Time converter currently only handles millisecond precision in Gregorian epochs.
   // So, split out part of epoch more precise than millisecond, perform time conversion
   // using time converter, then add the precision beyond a millisecond back in.  
   // NOTE, this will work when going from A1 to UTC, as is done here, because they 
   // only differ by a step function and the extra precision cannot cause a jump from 
   // one step to the next.  (i.e. don't do this for TDB)
   std::string dateToMilliSecond;
   Real submillisec = 0.0;

   if (timetemp.size() > 24)
   {
      dateToMilliSecond = timetemp.substr(0, 24);

      std::string precBeyondMilliSecond = "0.000";
      precBeyondMilliSecond += timetemp.substr(24);

      submillisec = atof(precBeyondMilliSecond.c_str()) /
            GmatTimeConstants::SECS_PER_DAY;
   }
   else
       dateToMilliSecond = timetemp;

   // Build the base epoch.  Note that the return from this call may differ from
   // the returned a1Mjd value by the submillisecond value, so the epochStr
   // returned here is not necessarily the final ephem epoch.
   theTimeConverter->Convert(fromType, fromMjd, dateToMilliSecond, toType,
         toMjd, epochStr, 1);
   
   if (epochStr == "")
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** EphemerisWriter::ToUtcGregorian() Cannot convert epoch %s "
          "to A1ModJulian\n", utcGreg.c_str());
      retval = false;
   }

   //  Add in any precision beyond millisecond in the epoch for the return value
   a1Mjd = toMjd + submillisec;

   #ifdef DEBUG_PRECISION_STARTTIME
      MessageInterface::ShowMessage("Epoch %.13lf = %.13lf + %.13lf\n", a1Mjd,
            toMjd, submillisec);
   #endif

   return retval;
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
bool STKEphemerisFile::ReadDataRecords(int logOption)
{

   //Map of tested coordinate systems to be updated when new systems have been tested and verified to work.
   //Add the central body as the key and then any coordinate systems verified for STK files and that central body can be added in the value vector.
   std::map<std::string, std::vector<std::string>> testedCoordinateSystems =
   {
      {"Earth",  {"J2000", "TrueOfDate", "ICRF", "Fixed"}},
      {"Luna",   {"J2000", "ICRF", "Fixed"}},
      {"Venus",  {"J2000", "Fixed"}},
      {"Sun",    {"J2000", "ICRF", "J2000_Ecliptic", "Fixed"}}
   };

   //These are frames allowed for all Central Bodies
   std::vector<std::string> universallySupportedFrames = {"J2000", "ICRF", "Fixed"};
   
   bool retval = false;

   // Read headers
   std::string line;
   int headerCount = 0;

   /// @todo Replace this list with a StringArray so it can easily grow?
   std::string stkVersionKeyword    = "stk.v.";
   std::string beginEphemKeyword    = "BEGIN Ephemeris";
   std::string numPointsKeyword     = "NumberOfEphemerisPoints";
   std::string epochKeyword         = "ScenarioEpoch";
   std::string centralBodyKeyword   = "CentralBody";
   std::string coordSysKeyword      = "CoordinateSystem";
   std::string timePosVelKeyword    = "EphemerisTimePosVel";
   std::string timePosVelKeywordV4  = "EphemerisEciTimePosVel";    // for STK 4.0
   std::string distanceUnitKeyword  = "DistanceUnit";
   std::string segmentStartKeyword  = "BEGIN SegmentBoundaryTimes";
   std::string segmentEndKeyword    = "END SegmentBoundaryTimes";
   std::string interpolationKeyword = "InterpolationMethod";


   std::string item;
   std::string::size_type index1;

   // Define flags for required keywords 
   bool stkVersionFound       = false;
   bool beginEphemFound       = false;
   bool numEphemPointsFound   = false;
   bool scenarioEpochFound    = false;
   bool timePosVelFound       = false;
   bool readingSegmentTimes   = false;
   // The following flags are not required by STK but GMAT requires them anyway
   bool coordinateSystemFound = false;
   bool centralBodyFound      = false;
   segmentStartTimes.clear();

   // Flag used to toggle time/pos/vel processing off and on
   bool readingTPV = false;

   // Parse the file header
   while (!stkInStream.eof())
   {
	  GmatFileUtil::GetLine(&stkInStream, line);

      if (line.find(numPointsKeyword) != line.npos)
      {
         index1 = line.find(numPointsKeyword);
         item = line.substr(index1 + numPointsKeyword.size());
         item = GmatStringUtil::Strip(item);
         numEphemPointsFound = true;
         headerCount++;
      }
      else if (line.find(epochKeyword) != line.npos)
      {
         index1 = line.find(epochKeyword);
         item = line.substr(index1 + epochKeyword.size());
         item = GmatStringUtil::Strip(item);
         scenarioEpochUtcGreg = item;
         // Convert epoch from UTCGregorian to A1Mjd
         if (UTCGregorianToA1ModJulian(item, scenarioEpochA1Mjd))
         {
            // Default start epoch to match scenario
            a1StartEpoch = scenarioEpochA1Mjd;
            scenarioEpochFound = true;
            headerCount++;
         }
         else
         {
            MessageInterface::ShowMessage
               ("*** ERROR *** Cannot convert ScenarioEpoch '%s' to "
                "A1ModJulian read from ephemeris file '%s'\n", item.c_str(),
                stkFileNameForRead.c_str());
            retval = false;
            break;
         }
      }
      else if (line.find(centralBodyKeyword) != line.npos)
      {
         index1 = line.find(centralBodyKeyword);
         item = line.substr(index1 + centralBodyKeyword.size());
         item = GmatStringUtil::Strip(item);
         centralBody = item;
         centralBodyFound = true;
         headerCount++;
      }
      else if (line.find(coordSysKeyword) != line.npos)
      {
         index1 = line.find(coordSysKeyword);
         item = line.substr(index1 + coordSysKeyword.size());
         item = GmatStringUtil::Strip(item);
         coordinateSystem = item;
         coordinateSystemFound = true;
         headerCount++;
      }
      else if (line.find(distanceUnitKeyword) != line.npos)
      {
         index1 = line.find(distanceUnitKeyword);
         item = line.substr(index1 + distanceUnitKeyword.size());
         item = GmatStringUtil::Strip(item);
         distanceUnit = item;
         headerCount++;
      }
      else if (line.find(interpolationKeyword) != line.npos)
      {
         index1 = line.find(interpolationKeyword);
         item = line.substr(index1 + interpolationKeyword.size());
         item = GmatStringUtil::Strip(item);
         interpolationMethod = item;
         headerCount++;

         if (interpolationMethod == "Hermite")
         {
            //Uncommenting this line and removing the warning will allow for use of Hermite interpolation with STK.e files.
            //useHermite = true;
            MessageInterface::ShowMessage("STK ephemeris file " + stkFileNameForRead + " has \"Hermite\" as the specified Interpolation method. Lagrange is the only method currently supported by GMAT for use with STK propagators and will be used instead.\n");
         }
         else if (interpolationMethod != "Lagrange")
         {
            throw UtilityException("STK ephemeris file " + stkFileNameForRead + " contains an unsupported InterpolationMethod. Lagrange and Hermite are the only methods currently supported by GMAT for use with STK propagators.\n");
         }
      }
      else if (line.find(beginEphemKeyword) != line.npos)
      {
          headerCount++;
          beginEphemFound = true;
      }
      else if (line.find(stkVersionKeyword) != line.npos)
      {
          headerCount++;
          stkVersionFound = true;
      }
      else if (line.find(segmentStartKeyword) != line.npos)
      {
          readingSegmentTimes = true;
      }
      else if (line.find(segmentEndKeyword) != line.npos)
      {
          readingSegmentTimes = false;
      }
      else if ((line.find(timePosVelKeyword) != line.npos) ||
               (line.find(timePosVelKeywordV4) != line.npos))
      {
         index1 = line.find(timePosVelKeyword);
         item = line.substr(index1 + timePosVelKeyword.size());
         item = GmatStringUtil::Strip(item);
         headerCount++;
         timePosVelFound = true;
         readingTPV = true;
         break;
      }
      else if (readingSegmentTimes)
      {
         std::stringstream segTime;
         if (line != "")
         {
            segTime << line;
            Real dTime;
            segTime >> dTime;
            
            // Set first segment start time as the start epoch
            if (segmentStartTimes.size() == 0)
            {
               a1StartEpoch += dTime / GmatTimeConstants::SECS_PER_DAY;
               segmentStartTimes.push_back(a1StartEpoch);
            }
            else
               segmentStartTimes.push_back(scenarioEpochA1Mjd +
                     dTime / GmatTimeConstants::SECS_PER_DAY);
         }
      }
   }

   // Check that all required keywords in the file header were found
   bool foundAllRequiredHeaderElements = true;
   std::stringstream missingElements;

   if (!numEphemPointsFound)
   {
      missingElements << "   The required keyword "
            "\"NumberOfEphemerisPoints\" was not found\n";
      foundAllRequiredHeaderElements = false;
   }
   if(!scenarioEpochFound)
   {
      missingElements << "   The required keyword \"ScenarioEpoch\" "
            "was not found\n";
      foundAllRequiredHeaderElements = false;
   }
   if (!stkVersionFound)
   {
      missingElements << "   The required keyword \"stk.v.\" was "
            "not found\n";
       foundAllRequiredHeaderElements = false;
   }
   if (!timePosVelFound)
   {
      missingElements << "   The required keyword "
            "\"EphemerisTimePosVel\" was not found\n";
       foundAllRequiredHeaderElements = false;
   }
   if (!beginEphemFound)
   {
      missingElements << "   The required string \"BEGIN "
            "Ephemeris\" was not found\n";
       foundAllRequiredHeaderElements = false;
   }
   if (!coordinateSystemFound)
   {
      missingElements << "   The required keyword \"CoordinateSystem\" was not found. This keyword is required by GMAT.\n";
      foundAllRequiredHeaderElements = false;
   }
   if (!centralBodyFound)
   {
      missingElements << "   The required keyword \"CentralBody"
         "\" was not found. This keyword is required by GMAT.\n";
      foundAllRequiredHeaderElements = false;
   }

   // If all required elements were found then parse data, otherwise raise exception
   if (!foundAllRequiredHeaderElements) 
   {
       throw UtilityException("*** ERROR *** Error reading the STK ephemeris "
             "file " + stkFileNameForRead + ":\n" + missingElements.str());
   }
   else
   {
      // Read initial TimePosVel
      StringArray items;
      ephemRecords.clear();

      while (!stkInStream.eof())
      {
         // Use cross-platform GetLine
         GmatFileUtil::GetLine(&stkInStream, line);

         if (line.find("END Ephemeris") != line.npos)
            break;
		 if (line.find("CovarianceTimePosVel") != line.npos || line.find("CovarianceTimePos") != line.npos)
			 readingTPV = false;
         if (line.find(timePosVelKeyword) != line.npos)
            readingTPV = true;

         if (readingTPV)
         {
            if (line != "")
            {
               #ifdef DEBUG_INITIAL_FINAL
                  MessageInterface::ShowMessage("   data line =\n   '%s'\n",
                        line.c_str());
               #endif
               // Check if line has 7 items
               items = GmatStringUtil::SeparateBy(line, " ");
               if (items.size() != 7)
               {
                  MessageInterface::ShowMessage
                     ("*** ERROR *** Did not find correct number of elements in the STK file " + stkFileNameForRead + 
                           " the data may be incomplete\n");
                  break;
               }
               else
               {
                  Real time;
                  Rvector6 posvel;
                  if (!GetEpochAndState(line, time, posvel))
                  {
                     throw UtilityException("Error reading the STK ephemeris file " +
                           stkFileNameForRead);
                  }

                  EphemData ed;
                  ed.timeFromEpoch = time;

                  #ifdef DEBUG_DISTANCEUNIT
                     MessageInterface::ShowMessage("distanceUnit in ReadDataRecords: %s\n",s
                           distanceUnit.c_str());
                  #endif

                  if(distanceUnit == "Meters")
                  {
                     ed.theState[0] = posvel[0]/1000.0;
                     ed.theState[1] = posvel[1]/1000.0;
                     ed.theState[2] = posvel[2]/1000.0;
                     ed.theState[3] = posvel[3]/1000.0;
                     ed.theState[4] = posvel[4]/1000.0;
                     ed.theState[5] = posvel[5]/1000.0;
                  }
                  else
                  {
                     ed.theState[0] = posvel[0];
                     ed.theState[1] = posvel[1];
                     ed.theState[2] = posvel[2];
                     ed.theState[3] = posvel[3];
                     ed.theState[4] = posvel[4];
                     ed.theState[5] = posvel[5];
                  }

                  if (ephemRecords.size() == 0)
                  {
                     if (segmentStartTimes.size() == 0)
                     {
                        a1StartEpoch += time / GmatTimeConstants::SECS_PER_DAY;
                        segmentStartTimes.push_back(a1StartEpoch);
                     }
                     if (segmentStartTimes[0] != a1StartEpoch)
                        MessageInterface::ShowMessage("Warning!  The first "
                              "ephemeris segment start time, %.12lf, does not "
                              "match the start of the ephemeris file, %.12lf.\n",
                              segmentStartTimes[0], a1StartEpoch);
                  }

                  ephemRecords.push_back(ed);
                  retval = true;
               }
            }
         }

         if (logOption == 1)
         {
            MessageInterface::ShowMessage("Ephemeris Epoch:  %s\nData Size: %d\n"
                  "\nData:\n", scenarioEpochUtcGreg.c_str(), ephemRecords.size());
            for (UnsignedInt i = 0; i < ephemRecords.size(); ++i)
               MessageInterface::ShowMessage("   %lf  [%lf %lf %lf %.12lf %.12lf "
                     "%.12lf]\n", ephemRecords[i].timeFromEpoch,
                     ephemRecords[i].theState[0], ephemRecords[i].theState[1],
                     ephemRecords[i].theState[2], ephemRecords[i].theState[3],
                     ephemRecords[i].theState[4], ephemRecords[i].theState[5]);
         }
      }
   }

   if (ephemRecords.size() == 0)
   {
      MessageInterface::ShowMessage("*** ERROR *** There are no ephemeris data "
            "points\n");
   }

   // Now fill the base class data structure
   theEphem.clear();
   // Prepare the segment data structures
   for (UnsignedInt i = 0; i < segmentStartTimes.size(); ++i)
   {
      Segment seg;
      seg.segStart = segmentStartTimes[i];
      theEphem.push_back(seg);
   }
   Integer segNum = 0;
   GmatEpoch nextSegEpoch = (theEphem.size() > segNum+1 ?
         theEphem[segNum+1].segStart : 999999.0);

   GmatEpoch currentEpoch;
   EphemPoint currentPoint;
   currentPoint.theEpoch = 0.0;
   for (UnsignedInt i = 0; i < ephemRecords.size(); ++i)
   {
      currentEpoch = scenarioEpochA1Mjd + ephemRecords[i].timeFromEpoch /
            GmatTimeConstants::SECS_PER_DAY;

      currentPoint.theEpoch = currentEpoch;
      for (UnsignedInt j = 0; j < 6; ++j)
         currentPoint.posvel[j] = ephemRecords[i].theState[j];
      if (theEphem[segNum].points.size() > 1)
      {
         if (theEphem[segNum].points[theEphem[segNum].points.size() - 1].theEpoch == currentPoint.theEpoch)
         {

            if (theEphem[segNum].points[theEphem[segNum].points.size() - 1].posvel != currentPoint.posvel)
            {
               TimeSystemConverter *theTimeConverter;
               theTimeConverter = TimeSystemConverter::Instance();
               std::string fn = stkFileNameForRead.substr(stkFileNameForRead.find_last_of("/\\") + 1);
               std::string time = ""; 
               Real temp;
               theTimeConverter->Convert("A1ModJulian", currentPoint.theEpoch, "", "UTCGregorian", temp, time, 1);
               
               std::string errmsg = "";
               errmsg += "ERROR reading data in ephemeris file: \"";
               errmsg += fn;
               errmsg += "\". Duplicate epochs with different position/velocity data were found within a segment at ";
               errmsg += time;
               errmsg += ". Ephemeris data can only be discontinuous at segment boundaries.";
               throw UtilityException(errmsg);
            }
         }
      }
      theEphem[segNum].points.push_back(currentPoint);
      theEphem[segNum].segEnd = currentEpoch;

      // At a segment boundary, write the end epoch and update the segment index
      if (currentEpoch >= nextSegEpoch)
      {
         ++segNum;
         nextSegEpoch = (theEphem.size() > segNum+1 ?
                  theEphem[segNum+1].segStart : 999999.0);
      }
   }
   a1EndEpoch = currentEpoch;

   if (centralBody == "Moon")
      centralBody = "Luna";

   std::vector<std::string> unsupportedNonEarthFrames = {"TrueOfDate"};
   if(std::find(unsupportedNonEarthFrames.begin(), unsupportedNonEarthFrames.end(), coordinateSystem) != unsupportedNonEarthFrames.end() && centralBody != "Earth")
      throw UtilityException("Use of the " + coordinateSystem + " axes in STK ephemeris files is not permitted for central bodies other than Earth. Check file " + stkFileNameForRead + ". See the User's Guide for more details.\n");


   
   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
   
   if (runmode != GmatGlobal::TESTING) {
      bool validCoordinateSystem = false;
      
      if (std::find(universallySupportedFrames.begin(), universallySupportedFrames.end(), coordinateSystem) != universallySupportedFrames.end())
         validCoordinateSystem = true;
      else if (testedCoordinateSystems.find(centralBody) != testedCoordinateSystems.end())
         if ((std::count(testedCoordinateSystems[centralBody].begin(), testedCoordinateSystems[centralBody].end(), coordinateSystem)))
            validCoordinateSystem = true;
      if (!validCoordinateSystem)
      { 
         if (centralBody == "Luna")
            centralBody = "Moon";
         //did not find coordinates in central body list
         std::string errmsg = "The coordinate system \"" + coordinateSystem + "\" in use in ephemeris file \"" + stkFileNameForRead + "\" is not currently supported by GMAT for use with the central body \"" + centralBody + "\" in a STK ephemeris file.\n";
         throw UtilityException(errmsg);
      }
   }
   #ifdef DEBUG_SEGMENTING
      MessageInterface::ShowMessage("Segment Start Times:\n");
      for (UnsignedInt i = 0; i < segmentStartTimes.size(); ++i)
         MessageInterface::ShowMessage("   %.12lf\n", segmentStartTimes[i]);

      MessageInterface::ShowMessage("Segment data:\n");
      for (UnsignedInt i = 0; i < theEphem.size(); ++i)
      {
         MessageInterface::ShowMessage("   Segment %d, Spanning %.12lf to "
               "%.12lf, has %d points\n", i, theEphem[i].segStart,
               theEphem[i].segEnd, theEphem[i].points.size());
         #ifdef DUMP_SEGMENT_DATA
            for (UnsignedInt j = 0; j < theEphem[i].points.size(); ++j)
            {
               MessageInterface::ShowMessage("      %4d:  %.12lf [%s]\n", j,
                     theEphem[i].points[j].theEpoch,
                     theEphem[i].points[j].posvel.ToString(15).c_str());
            }
         #endif
      }
   #endif

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
void STKEphemerisFile::GetStartAndEndEpochs(GmatEpoch& startEpoch,
      GmatEpoch& endEpoch, std::vector<EphemData>** records)
{
   if (ephemRecords.size() > 0)
   {
      startEpoch = scenarioEpochA1Mjd + ephemRecords[0].timeFromEpoch /
            GmatTimeConstants::SECS_PER_DAY;
      endEpoch = scenarioEpochA1Mjd + ephemRecords[ephemRecords.size()-1].timeFromEpoch /
            GmatTimeConstants::SECS_PER_DAY;;
   }
   else
      MessageInterface::ShowMessage("Warning: STK Ephemeris file %s contains "
            "zero records: has the file been read?\n",
            stkFileNameForRead.c_str());
   *records   = &ephemRecords;
}

//------------------------------------------------------------------------------
// std::string GetDistanceUnit()
//------------------------------------------------------------------------------
/**
 * Returns the distance units used in the ephem
 *
 * @note Supported units are Meters and Kilometers
 *
 * @return The units in use
 */
//------------------------------------------------------------------------------
std::string STKEphemerisFile::GetDistanceUnit()
{
   return distanceUnit;
}

//------------------------------------------------------------------------------
// void SetDistanceUnit(const std::string &dU)
//------------------------------------------------------------------------------
/**
 * Returns the distance units used in the ephem
 *
 * @note Supported units are Meters and Kilometers
 *
 * @param Sets the distance units that are used
 */
//------------------------------------------------------------------------------
void STKEphemerisFile::SetDistanceUnit(const std::string &dU)
{
   distanceUnit = dU;
}

//------------------------------------------------------------------------------
// bool STKEphemerisFile::GetIncludeEventBoundaries()
//------------------------------------------------------------------------------
/**
 * Method used to access the event boundary setting for ephem writing
 *
 * @return The setting for the flag
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::GetIncludeEventBoundaries()
{
   return includeEventBoundaries;
}

//------------------------------------------------------------------------------
// void SetIncludeEventBoundaries(bool iEB)
//------------------------------------------------------------------------------
/**
 * Method used to toggle event boundary writing to the ephem
 *
 * @param iEB The new flag setting
 */
//------------------------------------------------------------------------------
void STKEphemerisFile::SetIncludeEventBoundaries(bool iEB)
{
   includeEventBoundaries = iEB;
}


//------------------------------------------------------------------------------
// std::string GetCentralBody()
//------------------------------------------------------------------------------
/**
 * Returns centralBody 
 */
 //------------------------------------------------------------------------------
std::string STKEphemerisFile::GetCentralBody()
{
   return centralBody;
}

//------------------------------------------------------------------------------
// std::string GetEphemCoordinates()
//------------------------------------------------------------------------------
/**
 * Returns coordinateSystem
 */
 //------------------------------------------------------------------------------
std::string STKEphemerisFile::GetEphemCoordinates()
{
   return coordinateSystem;
}
