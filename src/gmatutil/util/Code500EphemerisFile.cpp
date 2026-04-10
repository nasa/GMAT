//$Id$
//------------------------------------------------------------------------------
//                                  Code500EphemerisFile
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2013.05.01
//
// This class reads and writes ephemeris file in Code-500 format in either
// big-endian (UNIX) or little-endian (PC) byte order, and can automatically
// detect the endianness of the file for reading.
//
// The file format is based on the section 4.1.1 of Flight Dynamics Division
// Generic Data Products Formats Interface Control Document & Appendix A.3
// of Swingby User's Guide: Swingby User 552-FDD-93-046R3UD0 document.
//
// Notes on endianness:
// Endianness refers to how bytes are ordered within a data item. A big-endian
// machine stores the most significant byte first at the lowest byte address
// while a little-endian machine stores the least significant byte first.
//
//
//------------------------------------------------------------------------------

#include "Code500EphemerisFile.hpp"
#include "TimeSystemConverter.hpp"   // For ConvertGregorianToMjd()
#include "StateConversionUtil.hpp"   // For Convert()
#include "Rvector6.hpp"
#include "DateUtil.hpp"              // For friend function UnpackDate(), ToHMSFromSecondsOfDay
#include "RealUtilities.hpp"         // For IsEqual()
#include "GregorianDate.hpp"         // For GregorianDate()
#include "GmatConstants.hpp"         // For GmatTimeConstants::A1_TAI_OFFSET
#include "UtilityException.hpp"      // for UtilityException
#include "MessageInterface.hpp"

#include "A1Mjd.hpp"
#include "DateUtil.hpp"

//#define DEBUG_OPEN
//#define DEBUG_INIT
//#define DEBUG_SET
//#define DEBUG_READ_DATA
//#define DEBUG_WRITE_HEADERS
//#define DEBUG_READ_HEADERS
//#define DEBUG_WRITE_DATA_SEGMENT
//#define DEBUG_WRITE_DATA_SEGMENT_MORE
//#define DEBUG_WRITE_DATA
//#define DEBUG_DATA_SEGMENT_TIME
//#define DEBUG_WRITE_SENTINELS
//#define DEBUG_UNPACK_HEADERS
//#define DEBUG_UNPACK_DATA
//#define DEBUG_TIME_CONVERSION
//#define DEBUG_INITIAL_FINAL

//---------------------------------
// static data
//---------------------------------
// For unit conversion
const double Code500EphemerisFile::DUL_TO_KM         = 10000.0;
const double Code500EphemerisFile::DUL_DUT_TO_KM_SEC = 10000.0 / 864.0;
const double Code500EphemerisFile::KM_TO_DUL         = 1.0 / 10000.0;
const double Code500EphemerisFile::KM_SEC_TO_DUL_DUT = 864.0 / 10000.0;
const double Code500EphemerisFile::SEC_TO_DUT        = 1.0 / 864.0;
const double Code500EphemerisFile::DAY_TO_DUT        = 86400.0 / 864.0;
const double Code500EphemerisFile::DUT_TO_DAY        = 864.0 / 86400.0;
const double Code500EphemerisFile::DUT_TO_SEC        = 864.0;


//------------------------------------------------------------------------------
// Code500EphemerisFile(double satId, const std::string &productId, ...)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param  fileName     Filename to be opened
 * @param  satId        Spacecraft ID [123.0]
 * @param  timeSystem   Time system used "UTC" or "A1" ["UTC"]
 * @param  sourceId     Source ID ["GMAT"]
 * @param  centralBody  Central body name of output ephem used ["Earth"]
 * @param  coordSystem  3 = True of date, 4 = J2000, 5 = Earth-fixed/Body-fixed [4]
 * @param  fileMode     1 = input, 2 = output [2]
 * @param  fileFormat   1 = Little-endian, 2 = Big-endian [1]
 * @param  yearFormat   1 = YYY, 2 = YYYY
 */
//------------------------------------------------------------------------------
Code500EphemerisFile::Code500EphemerisFile(const std::string &fileName, double satId,
                                           const std::string &timeSystem,
                                           const std::string &sourceId,
                                           const std::string &centralBody,
                                           int coordSystemType,
                                           int fileMode, int fileFormat,
                                           int yearFormat)
{
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("Code500EphemerisFile() entered\n   fileName='%s', satId=%f, timeSystem='%s', sourceId='%s', "
       "centralBody='%s', coordSystemType='%d'\n", fileName.c_str(), satId, timeSystem.c_str(), sourceId.c_str(),
       centralBody.c_str(), coordSystemType);
   MessageInterface::ShowMessage
      ("   fileMode=%d, fileFormat=%d, yearFormat=%d\n", fileMode, fileFormat, yearFormat);
   #endif
   
   mSatId       = satId;
   mTimeSystem  = timeSystem;
   mSourceId    = sourceId;
   mOutputCentralBody = centralBody;
   mCoordSystemIndicator = coordSystemType;
   mFileMode    = fileMode;
   mCentralBodyOfIntegration = -99.99;
   mCentralBodyOfOutputEphem = -99.99;
   if (mFileMode == 1)
   {
      mInputFileName = fileName;
      mInputFileFormat = fileFormat;
      mInputYearFormat = yearFormat;
   }
   else
   {
      mOutputFileName = fileName;
      mOutputFileFormat = fileFormat;
      mOutputYearFormat = yearFormat;
      
      // Just for testing
      // mInputYearFormat = 2;
      // mOutputYearFormat = 2;
   }
   
   mPrecNutIndicator = 1.0; // hardcoded
   a1StartEpoch = 0.0;
   a1EndEpoch   = 0.0;

   theTimeConverter = TimeSystemConverter::Instance();

   Initialize();
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("%s ephemeris file in %s format\n", fileMode == 1 ? "Reading" : "Writing", fileFormat == 1 ? "little-endian" : "big-endian");
   MessageInterface::ShowMessage
      ("mOutputFileName='%s', mOutputFileFormat=%d, mOutputYearFormat=%d\n", mOutputFileName.c_str(),
       mOutputFileFormat, mOutputYearFormat);
   MessageInterface::ShowMessage
      ("Code500EphemerisFile() leaving\n   size of header1 = %d\n   "
       "size of header2 = %d\n   size of data    = %d\n   mSentinelData   = % 1.15e\n",
       sizeof(mEphemHeader1), sizeof(mEphemHeader2), sizeof(mEphemData),
       mSentinelData);
   #endif
}


//------------------------------------------------------------------------------
// ~Code500EphemerisFile()
//------------------------------------------------------------------------------
Code500EphemerisFile::~Code500EphemerisFile()
{
   if (mEphemFileIn.is_open())
      mEphemFileIn.close();
   
   if (mEphemFileOut.is_open())
   {
      mEphemFileOut.flush();
      mEphemFileOut.close();
   }
}


//------------------------------------------------------------------------------
// Code500EphemerisFile(const Code500EphemerisFile &ef) 
//------------------------------------------------------------------------------
Code500EphemerisFile::Code500EphemerisFile(const Code500EphemerisFile &ef) 
{
   mSatId = ef.mSatId;
   mTimeSystem = ef.mTimeSystem;
   mSourceId = ef.mSourceId;
   mOutputCentralBody = ef.mOutputCentralBody;
   mCoordSystemIndicator = ef.mCoordSystemIndicator;
   mCentralBodyOfIntegration = ef.mCentralBodyOfIntegration;
   mCentralBodyOfOutputEphem = ef.mCentralBodyOfOutputEphem;
   mFileMode = ef.mFileMode;
   mInputFileFormat = ef.mInputFileFormat;
   mOutputFileFormat = ef.mOutputFileFormat;
   mInputFileName = ef.mInputFileName;
   mOutputFileName = ef.mOutputFileName;
   mInputYearFormat = ef.mInputYearFormat;
   mOutputYearFormat = ef.mOutputYearFormat;
   a1StartEpoch = ef.a1StartEpoch;
   a1EndEpoch = ef.a1EndEpoch;
   
   Initialize();
}


//------------------------------------------------------------------------------
// Code500EphemerisFile& operator=(const Code500EphemerisFile& ef)
//------------------------------------------------------------------------------
Code500EphemerisFile& Code500EphemerisFile::operator=(const Code500EphemerisFile& ef)
{
   if (this == &ef)
   {
      return *this;
   }
   else
   {
      mSatId = ef.mSatId;
      mTimeSystem = ef.mTimeSystem;
      mSourceId = ef.mSourceId;
      mOutputCentralBody = ef.mOutputCentralBody;
      mCoordSystemIndicator = ef.mCoordSystemIndicator;
      mCentralBodyOfIntegration = ef.mCentralBodyOfIntegration;
      mCentralBodyOfOutputEphem = ef.mCentralBodyOfOutputEphem;
      mFileMode = ef.mFileMode;
      mInputFileFormat = ef.mInputFileFormat;
      mOutputFileFormat = ef.mOutputFileFormat;
      mInputFileName = ef.mInputFileName;
      mOutputFileName = ef.mOutputFileName;
      mInputYearFormat = ef.mInputYearFormat;
      mOutputYearFormat = ef.mOutputYearFormat;
      a1StartEpoch = ef.a1StartEpoch;
      a1EndEpoch = ef.a1EndEpoch;
      
      Initialize();
      return *this;
   }
}


//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void Code500EphemerisFile::Initialize()
{
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::Initialize() entered, mOutputCentralBody='%s', mSwapInputEndian=%d, "
       "mSwapOutputEndian=%d\n", mOutputCentralBody.c_str(), mSwapInputEndian, mSwapOutputEndian);
   #endif
   
   mProductId = "EPHEM   ";
   mTapeId = "STANDARD";
   
   // Set central body of output ephem
   mCentralBodyOfOutputEphem = GetBodyIndicator(mOutputCentralBody, 2);
   
   // Set central body of integration indicator same body as output for now
   if (mCentralBodyOfOutputEphem == -99.99)
      mCentralBodyOfIntegration = -99.99;
   else
      mCentralBodyOfIntegration = mCentralBodyOfOutputEphem + 1;
   
   // Time System Indicator: 0.0 = A.1, Atomic Time, 1.0 = UTC, Universal Time Coordinated
   mInputTimeSystem = 0.0;
   mOutputTimeSystem = 0.0;
   if (mTimeSystem == "A1")
      mOutputTimeSystem = 1.0;
   else if (mTimeSystem == "UTC")
      mOutputTimeSystem = 2.0;
   
   mCoordSystem = "2000";
   if (mCoordSystemIndicator == 3)
      mCoordSystem = "INER";
   else if (mCoordSystemIndicator == 5)
      mCoordSystem = "EFI ";

   mDataRecWriteCounter = 2; // data record starts at 3
   mLastDataRecRead = 2;
   mLastStateIndexRead = -1;
   mNumberOfRecordsInFile = -1;
   
   mSentinelData = 9.999999999999999e15;
   mSentinelsFound = false;
   mGregorianOfDUTRef = "18 Sep 1957 00:00:00.000";
   mRefTimeForDUT_YYMMDD = 570918.0;
   mMjdOfDUTRef = theTimeConverter->ConvertGregorianToMjd(mGregorianOfDUTRef);
   mTimeIntervalBetweenPointsSecs = 0.0;
   mLeapSecsStartOutput = 0.0;
   mLeapSecsEndOutput = 0.0;
   mStartUtcMjd = 0.0;
   mEndUtcMjd = 0.0;
   mLeapSecsInput = 0.0;
   
   mOutputCentralBodyMu = 0.0;
   
   mSwapInputEndian = false;
   mSwapOutputEndian = false;
   
   // Open file
   if (mFileMode == 1)
      OpenForRead(mInputFileName, mInputFileFormat);
   else if (mFileMode == 2)
      OpenForWrite(mOutputFileName, mOutputFileFormat);
   
   // Fill in some header and data record with initial values if writing
   if (mFileMode == 2)
   {
      InitializeHeaderRecord1();
      InitializeHeaderRecord2();
      InitializeDataRecord();
   }
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::Initialize() leaving, mOutputCentralBody='%s', "
       "mCentralBodyOfIntegration=%f, mCentralBodyOfOutputEphem=%f, "
       "mCoordSystem='%s' ,mCoordSystemIndicator='%d'\n", mOutputCentralBody.c_str(),
       mCentralBodyOfIntegration, mCentralBodyOfOutputEphem, mCoordSystem.c_str(), mCoordSystemIndicator);
   #endif
}


//------------------------------------------------------------------------------
// void Validate()
//------------------------------------------------------------------------------
/**
 * Validates header data. This method is usually called after Initialize.
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::Validate()
{
   // If file mode is writing, check for some header value
   if (mFileMode == 1)
   {
      // Check for uninitialized central body indicator
      if (mCentralBodyOfOutputEphem == -99.99)
         throw UtilityException
            ("Code 500 ephemeris header field error: central body of output ephem "
             "indicator is uninitialized for the file \"" + mOutputFileName + "\"");
   }
}


//------------------------------------------------------------------------------
// bool OpenForRead(const std::string &fileName, int fileFormat = 0, logOption = 0)
//------------------------------------------------------------------------------
/**
 * Opens ephemeris file for reading.
 *
 * @param  fileName  File name to be open for reading
 * @param  fileFormat  Input file format (0 = Auto-Detect, 1 = Little-Endian, 2 = Big-Endian)
 * @param  logOption  0 = no debug, 1 = write file size in bytes
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::OpenForRead(const std::string &fileName, int fileFormat,
                                       int logOption)
{
   #ifdef DEBUG_OPEN
   MessageInterface::ShowMessage
      ("OpenForRead() entered, fileName='%s', fileFormat=%d, logOption=%d\n",
       fileName.c_str(), fileFormat, logOption);
   #endif
   
   if (fileName == "")
   {
      #ifdef DEBUG_OPEN
      MessageInterface::ShowMessage
         ("OpenForRead() returning false, file name is blank\n");
      #endif
      return false;
   }
   
   if (mInputFileName != fileName)
   {
      // Close it first
      CloseForRead();      
      mEphemFileIn.open(fileName.c_str(), std::ios_base::binary);
      
      if (!mEphemFileIn.is_open())
         throw UtilityException("Cannot open code 500 ephemeris file \"" + fileName + "\" for reading");
      
      mInputFileName = fileName;
   }
   
   std::streampos fsize = mEphemFileIn.tellg();
   mEphemFileIn.seekg(0, std::ios::end);
   fsize = mEphemFileIn.tellg() - fsize;
   mEphemFileIn.seekg(std::ios_base::beg);
   mNumberOfRecordsInFile = (unsigned long)fsize / RECORD_SIZE;

   mSwapInputEndian = false;
   if (fileFormat == 0)
      mSwapInputEndian = IsFileEndianSwapped();
   else if (fileFormat == 1)
      mSwapInputEndian = !IsProcessorLittleEndian();
   else if (fileFormat == 2)
      mSwapInputEndian = IsProcessorLittleEndian();
   
   if (logOption == 1)
   {
      // Write out size of the file
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::OpenForRead() \n   ephem file: '%s'\n"
          "   size in bytes = %lu, number of records = %d\n", fileName.c_str(),
          (unsigned long)fsize, mNumberOfRecordsInFile);
   }
   
   #ifdef DEBUG_OPEN
   MessageInterface::ShowMessage
      ("OpenForRead() returning true, ephemeris file successfully opened for reading\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool OpenForWrite(const std::string &fileName, int fileFormat = 1)
//------------------------------------------------------------------------------
/**
 * Opens ephemeris file for writing.
 *
 * @param  fileName  File name to be open for writing
 * @param  fileFormat  Output file format (1 = Little-Endian, 2 = Big-Endian)
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::OpenForWrite(const std::string &fileName, int fileFormat)
{
   #ifdef DEBUG_OPEN
   MessageInterface::ShowMessage
      ("OpenForWrite() entered, fileName='%s', fileFormat=%d\n", fileName.c_str(), fileFormat);
   #endif
   
   if (fileName == "")
   {
      #ifdef DEBUG_OPEN
      MessageInterface::ShowMessage
         ("OpenForWrite() returning false, file name is blank\n");
      #endif
      return false;
   }
   
   if (mInputFileName != fileName)
   {
      // Close it firat
      CloseForWrite();
      mEphemFileOut.open(fileName.c_str(), std::ios_base::binary);
      
      if (!mEphemFileOut.is_open())
         throw UtilityException("Cannot open code 500 ephemeris file \"" + fileName + "\" for writing");
      
      mSwapOutputEndian = false;
      if (fileFormat == 1)
         mSwapOutputEndian = !IsProcessorLittleEndian();
      else if (fileFormat == 2)
         mSwapOutputEndian = IsProcessorLittleEndian();
   }
   
   #ifdef DEBUG_OPEN
   MessageInterface::ShowMessage
      ("OpenForWrite() returning true, ephemeris file successfully opened for writing\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// void CloseForRead()
//------------------------------------------------------------------------------
void Code500EphemerisFile::CloseForRead()
{
   if (mEphemFileIn.is_open())
      mEphemFileIn.close();
   
   mSentinelsFound = false;
}


//------------------------------------------------------------------------------
// void CloseForWrite()
//------------------------------------------------------------------------------
void Code500EphemerisFile::CloseForWrite()
{
   if (mEphemFileOut.is_open())
      mEphemFileOut.close();
}


//------------------------------------------------------------------------------
// bool GetInitialAndFinalStates(Real &initialEpoch, Real &finalEpoch, 
//         Rvector6 &initialState, Rvector6 &finalState, std::string &centralBody,
//         std::string &coordSystem, Integer &coordSysIndicator)
//------------------------------------------------------------------------------
/**
 * Retrieves inital and final epochs and states from the input Code500 ephemeris file.
 * It also retrieves central body and coordinate system on the file. It assumes
 * that input file is already opened successfully.
 *
 * @param initialEpoch  Initial epoch on the file
 * @param finalEpoch    Final epoch on the file
 * @param initialState  Initial state on the file
 * @param finalState    Final state on the file
 * @param centralBody   Central body of output on the file
 * @param coordSystem   Coordinate system on the file
 *                      ("2000" for J2000, "INER" for true of reference, "MEAN" for B1950)
 * @param coordSysIndicator  Coordinate system indicator
 *                      (2 = Mean of 1950, 3 = True of reference, 4 = J2000)
 * 
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::GetInitialAndFinalStates(Real &initialEpoch, Real &finalEpoch,
                                                    Rvector6 &initialState, Rvector6 &finalState,
                                                    std::string &centralBody, std::string &coordSystem,
                                                    Integer &coordSysIndicator)
{
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::GetInitialAndFinalState() entered\n");
   MessageInterface::ShowMessage("   mNumberOfRecordsInFile = %d\n", mNumberOfRecordsInFile);
   #endif
   
   // For initial and final epoch/state
   #ifdef DEBUG_INITIAL_FINAL
   ReadHeader1(1);
   #else
   ReadHeader1();
   #endif

   // For initial state
   ReadDataAt(1);
   int i = 0;
   double posDult, velDult;
   for (int j = 0; j < 3; j++)
   {
      posDult = ReadDoubleField(&mEphemData.firstStateVector_DULT[j]);
      mInitialState[j] = posDult * DUL_TO_KM;
   }
   for (int j = 3; j < 6; j++)
   {
      velDult = ReadDoubleField(&mEphemData.firstStateVector_DULT[j]);
      mInitialState[j] = velDult * DUL_DUT_TO_KM_SEC;
   }
   
   // For final state
   ReadDataAt(mNumberOfRecordsInFile-2);
   i = mLastStateIndexRead;
   for (int j = 0; j < 3; j++)
   {
      posDult = ReadDoubleField(&mEphemData.stateVector2Thru50_DULT[i][j]);
      mFinalState[j] = posDult * DUL_TO_KM;
   }
   for (int j = 3; j < 6; j++)
   {
      velDult = ReadDoubleField(&mEphemData.stateVector2Thru50_DULT[i][j]);
      mFinalState[j] = velDult * DUL_DUT_TO_KM_SEC;
   }
   
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("   a1StartEpoch = %.12f\n   a1EndEpoch   = %.12f\n", a1StartEpoch, a1EndEpoch);
   MessageInterface::ShowMessage
      ("   initialState = %s\n   finalState   = %s\n", mInitialState.ToString().c_str(),
       mFinalState.ToString().c_str());
   #endif
   
   initialEpoch = a1StartEpoch;
   finalEpoch   = a1EndEpoch;
   initialState = mInitialState;
   finalState   = mFinalState;
   
   // Set central body name of output ephem
   centralBody = GetBodyName(mCentralBodyOfOutputEphem, 2);
   
   // Set coordiante system
   // "INER" for True of date, "2000" for J2000, "EFI " for Earth-fixed
   coordSystem = mCoordSystem;
   // 3 = True of date, 4 = J2000, 5 = Earth-fixed
   coordSysIndicator = mCoordSystemIndicator;
   
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("   centralBody = '%s', coordSystem = '%s', coordSysIndicator = %d\n",
       centralBody.c_str(), coordSystem.c_str(), coordSysIndicator);
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::GetInitialAndFinalState() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool IsProcessorLittleEndian()
//------------------------------------------------------------------------------
/**
* Checks if the processor is little-endian
*
* @return true if the processor is little-endian,
*         false if the processor is big-endian
*/
//------------------------------------------------------------------------------
bool Code500EphemerisFile::IsProcessorLittleEndian()
{
   UnsignedInt endianInt = 1U;
   bool isLittleEndian = ((char*)&endianInt)[0] == 0x01;
   return isLittleEndian;
}


//------------------------------------------------------------------------------
// bool IsFileEndianSwapped()
//------------------------------------------------------------------------------
/**
* Checks the value of mEphemHeader1.timeSystemIndicator to determine if the
* endianness of the file needs to be swapped in order to be read.
* 
* @return true if the endianness of the file is swapped,
*         false if the endianness of the file is the same
*/
//------------------------------------------------------------------------------
bool Code500EphemerisFile::IsFileEndianSwapped()
{

   if (!mEphemFileIn.is_open())
      throw UtilityException("Cannot open code 500 ephemeris file \"" + mInputFileName + "\" for reading");

   // Read the first header
   mEphemFileIn.seekg(std::ios_base::beg);
   mEphemFileIn.read((char*)(&mEphemHeader1), RECORD_SIZE);

   // Save infile time system used
   //mInputTimeSystem = mEphemHeader1.timeSystemIndicator;
   Real timeSystemIndicator = mEphemHeader1.timeSystemIndicator;

   if (timeSystemIndicator == 1 || timeSystemIndicator == 2)
      return false; // Endianness matches

   return true;
}


//------------------------------------------------------------------------------
// bool ReadHeader1(int logOption = 0)
//------------------------------------------------------------------------------
/**
 * Unpacks header 1 record and log values to log file on option.
 *
 * @param  logOption  0 = no log, 1 = log all header data
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::ReadHeader1(int logOption)
{
   #ifdef DEBUG_READ_HEADERS
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::ReadHeader1() entered, logOption=%d, mInputYearFormat=%d\n",
       logOption, mInputYearFormat);
   #endif
   
   if (!mEphemFileIn.is_open())
      throw UtilityException("Cannot open code 500 ephemeris file \"" + mInputFileName + "\" for reading");
   
   // Read the first header
   mEphemFileIn.seekg(std::ios_base::beg);
   mEphemFileIn.read((char*)(&mEphemHeader1), RECORD_SIZE);
   
   // Save infile time system used
   //mInputTimeSystem = mEphemHeader1.timeSystemIndicator;
   mInputTimeSystem = ReadDoubleField(&mEphemHeader1.timeSystemIndicator);
   
   if (logOption == 1)
      UnpackHeader1();
   
   // Save the data used by the Code500 propagator in GMAT compatible formats
   double yyymmddStart = ReadDoubleField(&mEphemHeader1.startDateOfEphem_YYYMMDD);
   if (mInputYearFormat == 2)
      yyymmddStart = yyymmddStart - 19000000;
   double secsOfDayStart = ReadDoubleField(&mEphemHeader1.startSecondsOfDay);

   Real startDate = ReadDoubleField(&mEphemHeader1.startDateOfEphem_YYYMMDD);
   Real startSecs = ReadDoubleField(&mEphemHeader1.startSecondsOfDay);
   Real endDate   = ReadDoubleField(&mEphemHeader1.endDateOfEphem_YYYMMDD);
   Real endSecs   = ReadDoubleField(&mEphemHeader1.endSecondsOfDay);

   Integer year   = (Integer)(startDate / 10000);
   Integer month  = (Integer)((startDate - year * 10000) / 100);
   Integer day    = (Integer)(startDate - year * 10000 - month * 100);
   year += 1900;
   Integer hour   = (Integer)(startSecs / 3600);
   Integer minute = (Integer)((startSecs - hour * 3600) / 60);
   Real second    = startSecs - hour * 3600 - minute * 60;
   GmatEpoch epoch = ModifiedJulianDate(year, month, day, hour, minute, second);

   year   = (Integer)(endDate / 10000);
   month  = (Integer)((endDate - year * 10000) / 100);
   day    = (Integer)(endDate - year * 10000 - month * 100);
   year += 1900;
   hour   = (Integer)(endSecs / 3600);
   minute = (Integer)((endSecs - hour * 3600) / 60);
   second = endSecs - hour * 3600 - minute * 60;
   GmatEpoch epochend = ModifiedJulianDate(year, month, day, hour, minute, second);

   if (mInputTimeSystem == 1.0)
   {
      a1StartEpoch = epoch;
      a1EndEpoch   = epochend;
   }
   else if (mInputTimeSystem == 2.0) // UTC
   {
      a1StartEpoch = theTimeConverter->Convert(epoch, TimeSystemConverter::UTCMJD,
            TimeSystemConverter::A1MJD);
      a1EndEpoch = theTimeConverter->Convert(epochend, TimeSystemConverter::UTCMJD,
            TimeSystemConverter::A1MJD);
   }
   
   // Save central body of integration indicator
   mCentralBodyOfIntegration = ReadDoubleField(&mEphemHeader1.centralBodyIndicator);
   
   // Save central body and coordinate system needed by GetInitialAndFinalState()
   mCentralBodyOfOutputEphem = ReadDoubleField(&mEphemHeader1.coordinateCenterIndicator);
   std::string coordSystemStr = mEphemHeader1.coordSystemIndicator1;
   mCoordSystem = coordSystemStr.substr(0,4);
   mCoordSystemIndicator = ReadIntegerField(&mEphemHeader1.coordSystemIndicator2);
   
   // Save initial state needed by GetInitialAndFinalState()
   for (int i = 0; i < 3; i++)
   {
      double posDult = ReadDoubleField(&mEphemHeader1.cartesianElementsAtEpoch_DULT[i]);
      // Save initial position
      mInitialState[i] = posDult * DUL_TO_KM;
   }
   for (int i = 3; i < 6; i++)
   {
      double velDult = ReadDoubleField(&mEphemHeader1.cartesianElementsAtEpoch_DULT[i]);
      // Save initial velocity
      mInitialState[i] = velDult * DUL_DUT_TO_KM_SEC;
   }
   
   
   #ifdef DEBUG_READ_HEADERS
   MessageInterface::ShowMessage("A.1 Start Epoch: %.12lf\n", a1StartEpoch);
   MessageInterface::ShowMessage("A.1 End Epoch:   %.12lf\n", a1EndEpoch);
   MessageInterface::ShowMessage("A.1 Step Size:   %.12lf\n", ReadDoubleField(&mEphemHeader1.stepSize_SEC));

   MessageInterface::ShowMessage
      ("Code500EphemerisFile::ReadHeader1() returning true, mInputTimeSystem=%d\n",
       mInputTimeSystem);
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool ReadHeader2(int logOption = 0)
//------------------------------------------------------------------------------
/**
 * Unpacks header 2 record and log values to log file on option.
 *
 * @param  logOption  0 = no log, 1 = log all header data
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::ReadHeader2(int logOption)
{
   if (!mEphemFileIn.is_open())
      throw UtilityException("Cannot open code 500 ephemeris file \"" + mInputFileName + "\" for reading");
   
   // Read the second header
   mEphemFileIn.seekg(RECORD_SIZE, std::ios_base::beg);
   mEphemFileIn.read((char*)&mEphemHeader2, RECORD_SIZE);
   
   if (logOption == 1)
      UnpackHeader2();
   
   return true;
}

//------------------------------------------------------------------------------
// void GetStartAndEndEpochs(GmatEpoch &startEpoch, GmatEpoch &endEpoch)
//------------------------------------------------------------------------------
/**
 * Accesses the start and end epochs as contained in the header data
 *
 * @param startEpoch The start epoch
 * @param endEpoch The end epoch
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::GetStartAndEndEpochs(GmatEpoch &startEpoch,
      GmatEpoch &endEpoch, std::vector<EphemData> **records)
{
   startEpoch = a1StartEpoch;
   endEpoch   = a1EndEpoch;
   *records = &ephemRecords;
}

//------------------------------------------------------------------------------
// bool ReadDataAt(int dataRecNumber, int logOption = 0)
//------------------------------------------------------------------------------
/**
 * Reads data record at requested data record number. It will add the size of
 * 2 header records before positioning the file.
 *
 * @param  dataRecNumber  Data record number
 * @param  logOption      = 0, no log
 *                        = 1, log first state vector of only first and last record
 *                        = 2, log first and last state vector of all records
 *                        = 3, log all state vectors of all records
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::ReadDataAt(int dataRecNumber, int logOption)
{
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage
      ("ReadDataAt() entered, dataRecNumber = %d, logOption = %d\n", dataRecNumber,
       logOption);
   #endif
   
   if (!mEphemFileIn.is_open())
      throw UtilityException("Cannot open code 500 ephemeris file \"" + mInputFileName + "\" for reading");
   
   int filePos = (dataRecNumber + 1) * RECORD_SIZE;
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage("   filePos = %d\n", filePos);
   #endif
   // Position file at requested data record number
   mEphemFileIn.seekg(filePos, std::ios_base::beg);
   
   // Check for eof
   if (mEphemFileIn.eof())
   {
      #ifdef DEBUG_READ_DATA
      MessageInterface::ShowMessage
         ("ReadDataAt() returning false, input ephem file reached eof\n");
      #endif
      return false;
   }
   
   mEphemFileIn.read((char*)&mEphemData, RECORD_SIZE);
   
   // Always unpack data to find sentinel data for end of state vector
   UnpackDataRecord(dataRecNumber, logOption);
   
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage
      ("ReadDataAt() returning true, dataRecNumber = %d, mLastStateIndexRead = %d\n",
       dataRecNumber, mLastStateIndexRead);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool ReadDataRecords(int numRecordsToRead = -1, int logOption = 0)
//------------------------------------------------------------------------------
/**
 * Reads data record at requested data record number. It will add the size of
 * 2 header records before positioning the file.
 *
 * @param  numRecordsToRead = -999, Read entire file
 *                          > 0, Reads number of records
 * @param  logOption        = 0, no log
 *                          = 1, log first state vector of only first and last record
 *                          = 2, log first state vector of all specified number of records
 *                          = 3, log all state vectors of all specified number of records
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::ReadDataRecords(int numRecordsToRead, int logOption)
{
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage
      ("ReadDataRecords() entered, numRecordsToRead = %d, logOption = %d, "
       "mSentinelsFound=%d\n", numRecordsToRead, logOption, mSentinelsFound);
   #endif
   
   if (!mEphemFileIn.is_open())
      throw UtilityException("Cannot open code 500 ephemeris file \"" + mInputFileName + "\" for reading");
   
   int recCount = 1;
   ephemRecords.clear();
   
   // Position file to first data record
   int filePos = RECORD_SIZE * 2;
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage("   filePos = %d\n", filePos);
   #endif
   mEphemFileIn.seekg(filePos, std::ios_base::beg);
   
   // Read the data until eof or 10 sentinels found, or numRecordsToRead reached
   bool continueRead = true;
   //while (continueRead && !mSentinelsFound)
   while (continueRead)
   {
      if (mEphemFileIn.eof())
      {
         #ifdef DEBUG_READ_DATA
         MessageInterface::ShowMessage("End of file reached\n");
         #endif
         break;
      }
      else
      {
         ReadDataAt(recCount, logOption);
         
         if ((numRecordsToRead != -999) && (recCount >= numRecordsToRead))
         {
            #ifdef DEBUG_READ_DATA
            MessageInterface::ShowMessage("Read requested %d records\n", numRecordsToRead);
            #endif
            break;
         }
         else if (mSentinelsFound)
         {
            #ifdef DEBUG_READ_DATA
            MessageInterface::ShowMessage
               ("Sentinels found at record # %d, mLastStateIndexRead = %d\n", recCount, mLastStateIndexRead);
            #endif
            
            // Log last data record
            if (logOption > 0)
               UnpackDataRecord(recCount, logOption);
            break;
         }
         recCount++;
      }
   }
   
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage("ReadDataRecords() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool FinalizeHeaders()
//------------------------------------------------------------------------------
bool Code500EphemerisFile::FinalizeHeaders()
{
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::FinalizeHeaders() entered\n");
   #endif
   
   // Anything to finalize header 1?
   
   // Write final header 1 record
   WriteHeader1();
   mEphemFileOut.flush();
   
   #ifdef DEBUG_WRITE_HEADERS
   DebugDouble("mEphemHeader1.endDateOfEphem_YYYMMDD = %f\n", mEphemHeader1.endDateOfEphem_YYYMMDD, mSwapOutputEndian);
   DebugDouble("mEphemHeader1.endTimeOfEphemeris_DUT = %f\n", mEphemHeader1.endTimeOfEphemeris_DUT, mSwapOutputEndian);
   #endif
   
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::FinalizeHeaders() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool WriteHeader1()
//------------------------------------------------------------------------------
bool Code500EphemerisFile::WriteHeader1()
{
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteHeader1() entered\n");
   #endif
   
   if (!mEphemFileOut.is_open())
   {
      #ifdef DEBUG_WRITE_HEADERS
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteHeader1() returning false, output ephem file is not opened\n");
      #endif
      return false;
   }
   
   mEphemFileOut.seekp(std::ios_base::beg);
   mEphemFileOut.write((char*)&mEphemHeader1, RECORD_SIZE);
   
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteHeader1() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool WriteHeader2()
//------------------------------------------------------------------------------
bool Code500EphemerisFile::WriteHeader2()
{
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteHeader2() entered\n");
   #endif
   
   if (!mEphemFileOut.is_open())
   {
      #ifdef DEBUG_WRITE_HEADERS
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteHeader2() returning false, output ephem file is not opened\n");
      #endif
      return false;
   }
   
   mEphemFileOut.seekp(RECORD_SIZE, std::ios_base::beg);
   mEphemFileOut.write((char*)&mEphemHeader2, RECORD_SIZE);
   
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteHeader2() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool WriteDataAt(int recNumber)
//------------------------------------------------------------------------------
bool Code500EphemerisFile::WriteDataAt(int recNumber)
{
   #ifdef DEBUG_WRITE_DATA
   MessageInterface::ShowMessage
      ("============================= Code500EphemerisFile::WriteDataAt() entered, recNumber=%d\n", recNumber);
   #endif
   
   if (!mEphemFileOut.is_open())
   {
      #ifdef DEBUG_WRITE_DATA
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteDataAt() returning false, output ephem file is not opened\n");
      #endif
      
      //@note Should exception thron here?
      return false;
   }
   
   if (recNumber <=2)
   {
      #ifdef DEBUG_WRITE_DATA
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteDataAt() returning false, data records starts at 3, but requsted at %d\n",
          recNumber);
      #endif
      return false;
   }
   
   #ifdef DEBUG_WRITE_DATA
   MessageInterface::ShowMessage
      ("Writing data record at %d\n", RECORD_SIZE * (recNumber-1));
   #endif
   
   // Set to no thrust
   WriteDoubleField(&mEphemData.thrustIndicator, 2.0); // 1 = thrust, 2 = free flight
   
   mEphemFileOut.seekp(RECORD_SIZE * (recNumber-1), std::ios_base::beg);
   mEphemFileOut.write((char*)&mEphemData, RECORD_SIZE);
   
   #ifdef DEBUG_WRITE_DATA
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteDataAt() leaving\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool WriteDataSegment(const EpochArray &epochArray, const StateArray &stateArray, 
//                       bool canFinalize = false)
//------------------------------------------------------------------------------
/**
 * Handles writing state vector to data record.
 *
 * @param  epochArray  Array of epoch pointer
 * @param  stateArray  Array of state vector pointer
 *
 * @note It assumes the maximum number of state vector receiving is NUM_STATES_PER_RECORD (50).
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::WriteDataSegment(const EpochArray &epochArray,
                                            const StateArray &stateArray, 
                                            bool canFinalize)
{
   Integer numPoints = stateArray.size();
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("============================================================\n"
       "Code500EphemerisFile::WriteDataSegment() entered, numPoints=%d, canFinalize=%d\n",
       numPoints, canFinalize);
   #endif
   
   if (numPoints == 0)
   {
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteDataSegment() just returning true, data size is zero\n");
      #endif
      return true;
   }
   else if (numPoints > NUM_STATES_PER_RECORD)
   {
      UtilityException ue;
      ue.SetDetails("Code500EphemerisFile::WriteDataSegment() received too many state vector: %d, "
                    "The maximum it can handle is 50\n", numPoints);
      throw ue;
   }
   
   // For multiple segments, end epoch of previous segment may be the same as
   // beginning epoch of new segmnet, so check for duplicate epoch and use the
   // state of new epoch since any maneuver or some spacecraft property update
   // may happened.
   if (!mA1MjdArray.empty())
   {
      A1Mjd *end = mA1MjdArray.back();
      A1Mjd *newStart = epochArray.front();
      if (end->GetReal() == newStart->GetReal())
      {
         #ifdef DEBUG_WRITE_DATA_SEGMENT
         MessageInterface::ShowMessage
            ("Duplicate epoch found, so removing end dadta, end=%f, newStart=%f\n",
             end->GetReal(), newStart->GetReal());
         #endif
         
         // Remove the last dadta
         mA1MjdArray.pop_back();
         mStateArray.pop_back();
      }
   }
   
   // Fill the buffer
   for (int i = 0; i < numPoints; i++)
   {
      // Push cloned epoch and state since epoch and state POINTER array is cleared in
      // the EphemerisFile
      mA1MjdArray.push_back(epochArray[i]->Clone());
      mStateArray.push_back(stateArray[i]->Clone());
      
      #ifdef DEBUG_WRITE_DATA_SEGMENT_INDEX
      MessageInterface::ShowMessage("i=%2d, mA1MjdArray.size()=%2d\n", i, mA1MjdArray.size());
      #endif
      // If buffer is full, write data
      if ((int)mA1MjdArray.size() == NUM_STATES_PER_RECORD)
      {
         // Write data record and clear buffer
         bool isEndOfData = false;
         if (canFinalize && i == numPoints - 1)
            isEndOfData = true;
         WriteDataRecord(isEndOfData);
         ClearBuffer();
      }
   }
   
   // If final data segment received, write data
   if (canFinalize)
   {
      WriteDataRecord(canFinalize);
      ClearBuffer();
   }
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::WriteDataSegment() leavng, mA1MjdArray.size()=%d\n", mA1MjdArray.size());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// void SetSwapEndian(bool swapEndian, int fileMode)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetSwapEndian(bool swapEndian, int fileMode)
{
   #ifdef DEBUG_SWAP_ENDIAN
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::SetSwapEndian() entered, swapEndian=%d, fileMode=%d\n",
       swapEndian, fileMode);
   #endif
   
   if (fileMode == 1)
      mSwapInputEndian = swapEndian;
   else
      mSwapOutputEndian = swapEndian;
   
   #ifdef DEBUG_SWAP_ENDIAN
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::SetSwapEndian() leaving, mSwapInputEndian=%d, mSwapOutputEndian=%d\n",
       mSwapInputEndian, mSwapOutputEndian);
   #endif
}


//------------------------------------------------------------------------------
// bool GetSwapEndian(int fileMode)
//------------------------------------------------------------------------------
bool Code500EphemerisFile::GetSwapEndian(int fileMode)
{
   if (fileMode == 1)
      return mSwapInputEndian;
   else
      return mSwapOutputEndian;
}


//------------------------------------------------------------------------------
// double SwapDoubleEndian(double value)
//------------------------------------------------------------------------------
/**
 * Swaps endiness for double value
 */
//------------------------------------------------------------------------------
double Code500EphemerisFile::SwapDoubleEndian(double value)
{
   mDoubleOriginBytes.doubleValue = value;
   double swapped = SwapDouble();
   return swapped;
}


//------------------------------------------------------------------------------
// int SwapIntegerEndian(ing value)
//------------------------------------------------------------------------------
/**
 * Swaps endiness for int value
 */
//------------------------------------------------------------------------------
int Code500EphemerisFile::SwapIntegerEndian(int value)
{
   mIntOriginBytes.intValue = value;
   int swapped = SwapInteger();
   return swapped;
}

//---------------------------------
// protected methods
//---------------------------------


//------------------------------------------------------------------------------
// void InitializeHeaderRecord1()
//------------------------------------------------------------------------------
void Code500EphemerisFile::InitializeHeaderRecord1()
{
   #ifdef DEBUG_HEADERS
   MessageInterface::ShowMessage
      ("InitializeHeaderRecord1() entered, mSwapOutputEndian=%d\n", mSwapOutputEndian);
   #endif
   
   // Initialize all character arrays
   BlankOut(mEphemHeader1.productId, 8);
   BlankOut(mEphemHeader1.tapeId, 8);
   BlankOut(mEphemHeader1.sourceId, 8);
   BlankOut(mEphemHeader1.headerTitle, 56);
   BlankOut(mEphemHeader1.coordSystemIndicator1, 4);
   BlankOut(mEphemHeader1.orbitTheory, 8);
   BlankOut(mEphemHeader1.spares1, 16);
   BlankOut(mEphemHeader1.atmosphericDensityModel, 8);
   BlankOut(mEphemHeader1.spares2, 8);
   BlankOut(mEphemHeader1.spares3, 48);
   BlankOut(mEphemHeader1.spares4, 40);
   BlankOut(mEphemHeader1.spares5, 480);
   BlankOut(mEphemHeader1.spares6, 660);
   BlankOut(mEphemHeader1.harmonicsWithTitles1, 456);
   
   CopyString(mEphemHeader1.productId, mProductId, 8);
   WriteDoubleField(&mEphemHeader1.satId, mSatId);
   CopyString(mEphemHeader1.tapeId, mTapeId, 8);
   CopyString(mEphemHeader1.sourceId, mSourceId, 8);
   WriteDoubleField(&mEphemHeader1.timeSystemIndicator, mOutputTimeSystem);
   WriteDoubleField(&mEphemHeader1.refTimeForDUT_YYMMDD, mRefTimeForDUT_YYMMDD);
   WriteDoubleField(&mEphemHeader1.centralBodyIndicator, mCentralBodyOfIntegration);
   WriteDoubleField(&mEphemHeader1.coordinateCenterIndicator, mCentralBodyOfOutputEphem);
   CopyString(mEphemHeader1.coordSystemIndicator1, mCoordSystem, 4); // "2000" for J2000
   WriteIntegerField(&mEphemHeader1.coordSystemIndicator2, mCoordSystemIndicator); // 2 = Mean of 1950, 3 = True of reference, 4 = J2000
   
   std::string str;
   #ifdef DEBUG_HEADERS
   str = mEphemHeader1.tapeId;
   MessageInterface::ShowMessage("tapeId = '%s'\n", str.substr(0,8).c_str());
   str = mEphemHeader1.sourceId;
   MessageInterface::ShowMessage("sourceId = '%s'\n", str.substr(0,8).c_str());
   #endif
   
   // Set Orbit Theory to COWELL
   str = "COWELL  ";
   CopyString(mEphemHeader1.orbitTheory, str, 8);
   
   // Set leap second info
   WriteIntegerField(&mEphemHeader1.leapSecondIndicator, 1); // 1 = no leap second occurs, 2 = leap second occurs
   WriteDoubleField(&mEphemHeader1.dateOfLeapSeconds_YYYMMDD, 0.0);
   WriteDoubleField(&mEphemHeader1.timeOfLeapSeconds_HHMMSS,  0.0);
   WriteDoubleField(&mEphemHeader1.utcTimeAdjustment_SEC,     0.0);
   
   // Write precession-nutation info
   WriteDoubleField(&mEphemHeader1.precessionNutationIndicator, mPrecNutIndicator);

   // Write other indicators
   WriteDoubleField(&mEphemHeader1.zonalTesseralHarmonicsIndicator, 1.0);
   WriteDoubleField(&mEphemHeader1.lunarGravPerturbIndicator,       1.0);
   WriteDoubleField(&mEphemHeader1.solarRadiationPerturbIndicator,  1.0);
   WriteDoubleField(&mEphemHeader1.solarGravPerturbIndicator,       1.0);
   WriteDoubleField(&mEphemHeader1.atmosphericDragPerturbIndicator, 1.0);

   #ifdef DEBUG_HEADERS
   MessageInterface::ShowMessage("InitializeHeaderRecord1() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void InitializeHeaderRecord2()
//------------------------------------------------------------------------------
void Code500EphemerisFile::InitializeHeaderRecord2()
{
   #ifdef DEBUG_HEADERS
   MessageInterface::ShowMessage("InitializeHeaderRecord2() entered\n");
   #endif
   
   BlankOut(mEphemHeader2.harmonicsWithTitles2, RECORD_SIZE);
   
   #ifdef DEBUG_HEADERS
   MessageInterface::ShowMessage("InitializeHeaderRecord2() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void InitializeDataRecord()
//------------------------------------------------------------------------------
void Code500EphemerisFile::InitializeDataRecord()
{
   // Set to no thrust
   WriteDoubleField(&mEphemData.thrustIndicator, 0.0);
   
   // Set all states to sentinels
   WriteDoubleField(&mEphemData.firstStateVector_DULT[0], mSentinelData);
   WriteDoubleField(&mEphemData.firstStateVector_DULT[1], mSentinelData);
   WriteDoubleField(&mEphemData.firstStateVector_DULT[2], mSentinelData);
   WriteDoubleField(&mEphemData.firstStateVector_DULT[3], mSentinelData);
   WriteDoubleField(&mEphemData.firstStateVector_DULT[4], mSentinelData);
   WriteDoubleField(&mEphemData.firstStateVector_DULT[5], mSentinelData);
   
   for (int i = 0; i < NUM_STATES_PER_RECORD - 1; i++)
   {
      for (int j = 0; j < 6; j++)
         WriteDoubleField(&mEphemData.stateVector2Thru50_DULT[i][j], mSentinelData);
   }
   
   // Blank out spares
   BlankOut(mEphemData.spares1, 344);
}


//------------------------------------------------------------------------------
// void SetEphemerisStartTime(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
/**
 * Sets start time of ephemeris in the header record 1.
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetEphemerisStartTime(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::SetEphemerisStartTime() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   
   double startMjd = 0.0;
   if (mOutputTimeSystem == 1) // A1 time system
   {
      startMjd = a1Mjd.GetReal();
      mStartUtcMjd = ToUtcModJulian(a1Mjd);
   }
   else  // UTC time system
   {
      startMjd = ToUtcModJulian(a1Mjd);
      mStartUtcMjd = startMjd;
   }
   
   mLeapSecsStartOutput = theTimeConverter->NumberOfLeapSecondsFrom(startMjd);
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("mLeapSecsStartOutput = %f\n", mLeapSecsStartOutput);
   #endif
   
   double yyymmdd, hhmmss;
   ToYYYMMDDHHMMSS(startMjd, yyymmdd, hhmmss);
   hhmmss = 200000;   // hard-code this
   
   double doy = ToDayOfYear(startMjd);
   double secsOfDay = ToSecondsOfDay(startMjd);
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("yyymmdd=%f, hhmmss=%f, doy=%f, secsOfDay=%f\n", yyymmdd, hhmmss, doy, secsOfDay);
   #endif

   if (mOutputYearFormat == 2)
      yyymmdd = yyymmdd + 19000000;
   WriteDoubleField(&mEphemHeader1.startDateOfEphem_YYYMMDD, yyymmdd);
   WriteDoubleField(&mEphemHeader1.startDayCountOfYear, doy);
   WriteDoubleField(&mEphemHeader1.startSecondsOfDay, secsOfDay);
   double startDut = ToDUT(startMjd);
   WriteDoubleField(&mEphemHeader1.startTimeOfEphemeris_DUT, startDut);
   
   // Should I set initiation time to start time of ephemeris?
   WriteDoubleField(&mEphemHeader1.dateOfInitiationOfEphemComp_YYYMMDD, yyymmdd);
   WriteDoubleField(&mEphemHeader1.timeOfInitiationOfEphemComp_HHMMSS, hhmmss);
   
   #ifdef DEBUG_SET
   if (mOutputYearFormat == 2)
      yyymmdd = yyymmdd - 19000000;
   std::string ymdhms = ToYearMonthDayHourMinSec(yyymmdd, secsOfDay);
   MessageInterface::ShowMessage("startYYYYMMDDHHMMSSsss = %s\n", ymdhms.c_str());
   DebugDouble("mEphemHeader1.startTimeOfEphemeris_DUT = %f\n", startDut, mSwapOutputEndian);
   MessageInterface::ShowMessage("Code500EphemerisFile::SetEphemerisStartTime() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetEphemerisEndTime(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
/**
 * Sets end time of ephemeris in the header record 1.
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetEphemerisEndTime(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::SetEphemerisEndTime() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   
   double endMjd = 0.0;
   if (mOutputTimeSystem == 1) // A1 time system
   {
      endMjd = a1Mjd.GetReal();
      mEndUtcMjd = ToUtcModJulian(a1Mjd);
   }
   else  // UTC time system
   {
      endMjd = ToUtcModJulian(a1Mjd);
      mEndUtcMjd = endMjd;
   }
   
   // Leap seconds info
   mLeapSecsEndOutput = theTimeConverter->NumberOfLeapSecondsFrom(endMjd);
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("mLeapSecsEndOutput = %f\n", mLeapSecsEndOutput);
   #endif
   
   if ((mLeapSecsEndOutput - mLeapSecsStartOutput) > 0)
   {
      // Find out UTC date and time of the first leap second occurred between start and end time
      double firstUtcMjd = theTimeConverter->GetFirstLeapSecondMJD(mStartUtcMjd, mEndUtcMjd);
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage("firstUtcMjd       = %.12f\n", firstUtcMjd);
      #endif
      if (firstUtcMjd != -1.0)
      {
         double ymd, hms;
         ToYYYMMDDHHMMSS(firstUtcMjd, ymd, hms);
         // Add a couple of seconds to avoid noise at the leap second date boundary
         Real firstUtcMjd2 = firstUtcMjd + (2.0/86400);
         Real firstA1Mjd = theTimeConverter->Convert(firstUtcMjd2, TimeSystemConverter::UTCMJD, TimeSystemConverter::A1MJD);
         double a1UtcOffsetInSecs = (firstA1Mjd - firstUtcMjd2) * 86400.0;
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage("firstUtcMjd+2sec  = %.12f\n", firstUtcMjd2);
         MessageInterface::ShowMessage("firstA1Mjd        = %.12f\n", firstA1Mjd);
         MessageInterface::ShowMessage("a1UtcOffsetInSecs = %.12f\n", a1UtcOffsetInSecs);
         #endif
         WriteIntegerField(&mEphemHeader1.leapSecondIndicator, 2); // 1 = no leap second occurs, 2 = leap second occurs
         WriteDoubleField(&mEphemHeader1.dateOfLeapSeconds_YYYMMDD, ymd);
         WriteDoubleField(&mEphemHeader1.timeOfLeapSeconds_HHMMSS,  hms);
         WriteDoubleField(&mEphemHeader1.utcTimeAdjustment_SEC,     a1UtcOffsetInSecs);
      }
   }
   
   double yyymmdd = ToYYYMMDD(endMjd);
   double doy = ToDayOfYear(endMjd);
   double secsOfDay = ToSecondsOfDay(endMjd);
   
   if (mOutputYearFormat == 2)
      yyymmdd = yyymmdd + 19000000;
   WriteDoubleField(&mEphemHeader1.endDateOfEphem_YYYMMDD, yyymmdd);
   WriteDoubleField(&mEphemHeader1.endDayCountOfYear, doy);
   WriteDoubleField(&mEphemHeader1.endSecondsOfDay, secsOfDay);
   double endDut = ToDUT(endMjd);
   WriteDoubleField(&mEphemHeader1.endTimeOfEphemeris_DUT, endDut);
   
   #ifdef DEBUG_SET
   if (mOutputYearFormat == 2)
      yyymmdd = yyymmdd - 19000000;
   std::string ymdhms = ToYearMonthDayHourMinSec(yyymmdd, secsOfDay);
   MessageInterface::ShowMessage("endYYYYMMDDHHMMSSsss. = %s\n", ymdhms.c_str());
   DebugDouble("mEphemHeader1.endDateOfEphem_YYYMMDD = %f\n", mEphemHeader1.endDateOfEphem_YYYMMDD, mSwapOutputEndian);
   DebugDouble("mEphemHeader1.endTimeOfEphemeris_DUT = %f\n", mEphemHeader1.endTimeOfEphemeris_DUT, mSwapOutputEndian);
   MessageInterface::ShowMessage("Code500EphemerisFile::SetEphemerisEndTime() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetTimeIntervalBetweenPoints(double secs)
//------------------------------------------------------------------------------
/**
 * Sets time interval between ephemeris points.
 *
 * @param secs  Time interval in secconds (-999.999 indicates variable interval)
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetTimeIntervalBetweenPoints(double secs)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("SetTimeIntervalBetweenPoints() entered, secs=%f\n", secs);
   #endif
   
   if (secs == -999.999)
   {
      mTimeIntervalBetweenPointsSecs = 0.0;//@note What should be the default value?
      WriteIntegerField(&mEphemHeader1.outputIntervalIndicator, 2); // 1 = fixed step, 2 = variable step
   }
   else
   {
      mTimeIntervalBetweenPointsSecs = secs;
      WriteIntegerField(&mEphemHeader1.outputIntervalIndicator, 1); // 1 = fixed step, 2 = variable step
   }
   
   WriteDoubleField(&mEphemHeader1.stepSize_SEC, mTimeIntervalBetweenPointsSecs);
   WriteDoubleField(&mEphemHeader1.timeIntervalBetweenPoints_DUT, mTimeIntervalBetweenPointsSecs * SEC_TO_DUT);
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetTimeIntervalBetweenPoints() leaving, mTimeIntervalBetweenPointsSecs=%f\n",
       mTimeIntervalBetweenPointsSecs);
   #endif
}


//------------------------------------------------------------------------------
// void SetCentralBodyMu(double mu)
//------------------------------------------------------------------------------
/**
 * Sets central body mu for ephem output cartesian to keplerian conversion
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetCentralBodyMu(double mu)
{
   #ifdef DEBUG_CENTRALBODY_MU
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::SetCentralBodyMu() entered, mu=%.15f\n", mu);
   #endif
   mOutputCentralBodyMu = mu;
}


//------------------------------------------------------------------------------
// void SetInitialEpoch(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetInitialEpoch(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetInitialEpoch() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   
   std::string initialGreg;
   double initialMjd;
   
   if (mOutputTimeSystem == 1) // A1 time system
   {
      initialGreg = ToA1Gregorian(a1Mjd);
      initialMjd = a1Mjd.GetReal();
   }
   else  // UTC time system
   {
      initialGreg = ToUtcGregorian(a1Mjd);
      initialMjd = ToUtcModJulian(a1Mjd);
   }
   
   A1Mjd tempA1Mjd = initialMjd;
   double epochDUT = ToDUT(initialMjd);
   Real year, month, day, hour, min, sec;
   A1Date a1Date = tempA1Mjd.ToA1Date();
   a1Date.ToYearMonthDayHourMinSec(year, month, day, hour, min, sec);
   
   WriteDoubleField(&mEphemHeader1.epochTimeOfElements_DUT, epochDUT);
   WriteDoubleField(&mEphemHeader1.yearOfEpoch_YYY, year - 1900);
   WriteDoubleField(&mEphemHeader1.monthOfEpoch_MM, month);
   WriteDoubleField(&mEphemHeader1.dayOfEpoch_DD, day);
   WriteDoubleField(&mEphemHeader1.hourOfEpoch_HH, hour);
   WriteDoubleField(&mEphemHeader1.minuteOfEpoch_MM, min);
   WriteDoubleField(&mEphemHeader1.secondsOfEpoch_MILSEC, sec * 1000.0);
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetInitialEpoch() leaving, epochDUT=%f\n", epochDUT);
   #endif
}


//------------------------------------------------------------------------------
// void SetInitialState(Rvector6 *kmsec)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetInitialState(Rvector6 *kmsec)
{
   Rvector6 cartState = *kmsec;
   SetInitialCartesianState(cartState);
   
   Rvector6 kepState =
      StateConversionUtil::Convert(cartState, "Cartesian", "Keplerian", mOutputCentralBodyMu);  // flat, radius, "TA");
   SetInitialKeplerianState(kepState);
}


//------------------------------------------------------------------------------
// void SetInitialCartesianState(const Rvector6 &cartState)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetInitialCartesianState(const Rvector6 &cartState)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetInitialCartesianState() entered, cartState[0]=%f\n", cartState[0]);
   #endif
   
   Rvector6 cartStateDULT = cartState;
   for (int i = 0; i < 3; i++)
      cartStateDULT[i] = cartState[i] * KM_TO_DUL;
   for (int i = 3; i < 6; i++)
      cartStateDULT[i] = cartState[i] * KM_SEC_TO_DUL_DUT;
   
   for (int i = 0; i < 6; i++)
   {
      WriteDoubleField(&mEphemHeader1.cartesianElementsAtEpoch_DULT[i], cartStateDULT[i]);
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetKeplerianElements() leaving, cartStateDULT[0]=%f\n", cartStateDULT[0]);
   #endif
}


//------------------------------------------------------------------------------
// void SetInitialKeplerianState(const Rvector6 &kepState)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetInitialKeplerianState(const Rvector6 &kepState)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetInitialKeplerianState() entered, kepState[2]=%f\n", kepState[2]);
   #endif
   
   Rvector6 kepStateRad = kepState;

   // Keplerian elements:
   // [SMA, ECC, INC, RAAN, AOP, TA]
   for (int i = 2; i < 6; i++)
      kepStateRad[i] = kepState[i] * GmatMathConstants::RAD_PER_DEG;

   // It should write MA instead of TA
   Real ma = StateConversionUtil::TrueToMeanAnomaly(kepStateRad[5], kepState[1], true);
   kepStateRad[5] = ma;

   WriteDoubleField(&mEphemHeader1.keplerianElementsAtEpoch_RAD[0], kepStateRad[0]);
   WriteDoubleField(&mEphemHeader1.keplerianElementsAtEpoch_RAD[1], kepStateRad[1]);
   WriteDoubleField(&mEphemHeader1.keplerianElementsAtEpoch_RAD[2], kepStateRad[2]);
   // file stores RAAN and AOP in reverse order
   WriteDoubleField(&mEphemHeader1.keplerianElementsAtEpoch_RAD[3], kepStateRad[4]);
   WriteDoubleField(&mEphemHeader1.keplerianElementsAtEpoch_RAD[4], kepStateRad[3]);
   WriteDoubleField(&mEphemHeader1.keplerianElementsAtEpoch_RAD[5], kepStateRad[5]);

   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetKeplerianElements() leaving, kepStateRad[2]=%f\n", kepStateRad[2]);
   #endif
}


//------------------------------------------------------------------------------
// void WriteDataRecord(bool canFinalize)
//------------------------------------------------------------------------------
void Code500EphemerisFile::WriteDataRecord(bool canFinalize)
{
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("==================================================\n"
       "Code500EphemerisFile::WriteDataRecord() entered, canFinalize=%d, "
       "mA1MjdArray.size()=%d, mStateArray.size()=%d\n", canFinalize,
       mA1MjdArray.size(), mStateArray.size());
   #endif
   
   int numPoints = mA1MjdArray.size();
   A1Mjd *start = mA1MjdArray.front();
   A1Mjd *end = mA1MjdArray.back();
   
   if (numPoints == 0)
   {
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteDataRecord() leaving, there are no data to write\n");
      #endif
      return;
   }
   
   double startMjd = 0.0;
   if (mOutputTimeSystem == 1) // A1 time system
   {
      mLastDataRecStartGreg = ToA1Gregorian(*start);
      mLastDataRecEndGreg = ToA1Gregorian(*end);
      startMjd = start->GetReal();
   }
   else  // UTC time system
   {
      mLastDataRecStartGreg = ToUtcGregorian(*start);
      mLastDataRecEndGreg = ToUtcGregorian(*end);
      startMjd = ToUtcModJulian(*start);
   }
   
   // Write data record
   mDataRecWriteCounter++;
   
   #ifdef DEBUG_DATA_SEGMENT_TIME
   // Note: Subtracted 2 from mDataRecWriteCounter for actual data record counter
   // since it started at 2
   MessageInterface::ShowMessage
      ("===> mDataRecWriteCounter=%2d, start=%f '%s', end=%f '%s'\n", mDataRecWriteCounter-2,
       start->GetReal(), mLastDataRecStartGreg.c_str(), end->GetReal(), mLastDataRecEndGreg.c_str());
   #endif
   
   // Set start date of ephemeris if first data record
   // Header1 = record1,  Header2 = record2,  Data1 = record3
   if (mDataRecWriteCounter == 3)
   {
      SetInitialEpoch(*start);
      SetInitialState(mStateArray[0]);
      SetEphemerisStartTime(*start);
      // Write initial headers
      WriteHeader1();
      WriteHeader2();
   }
   
   if (canFinalize)
      SetEphemerisEndTime(*end);
   
   WriteDoubleField(&mEphemData.dateOfFirstEphemPoint_YYYMMDD, ToYYYMMDD(startMjd));
   WriteDoubleField(&mEphemData.dayOfYearForFirstEphemPoint, ToDayOfYear(startMjd));
   WriteDoubleField(&mEphemData.secsOfDayForFirstEphemPoint, ToSecondsOfDay(startMjd));
   WriteDoubleField(&mEphemData.timeIntervalBetweenPoints_SEC, mTimeIntervalBetweenPointsSecs);
   
   // Write first state vector
   double stateDULT[6];
   ConvertStateKmSecToDULT(mStateArray[0], stateDULT);
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("----- State vector 1\n");
   DebugWriteState(mStateArray[0], stateDULT, 2);
   #endif
   WriteDoubleField(&mEphemData.firstStateVector_DULT[0], stateDULT[0]);
   WriteDoubleField(&mEphemData.firstStateVector_DULT[1], stateDULT[1]);
   WriteDoubleField(&mEphemData.firstStateVector_DULT[2], stateDULT[2]);
   WriteDoubleField(&mEphemData.firstStateVector_DULT[3], stateDULT[3]);
   WriteDoubleField(&mEphemData.firstStateVector_DULT[4], stateDULT[4]);
   WriteDoubleField(&mEphemData.firstStateVector_DULT[5], stateDULT[5]);
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   DebugDouble("mEphemData.firstStateVector_DULT[0]  =% 1.15e\n",
               mEphemData.firstStateVector_DULT[0], mSwapOutputEndian);
   #endif
   
   // Write state 2 through numPoints,
   // data record state vector index starts from 0 and ends at numPoints - 1
   for (int i = 1; i < numPoints; i++)
   {
      ConvertStateKmSecToDULT(mStateArray[i], stateDULT);      
      for (int j = 0; j < 6; j++)
         WriteDoubleField(&mEphemData.stateVector2Thru50_DULT[i-1][j], stateDULT[j]);
      
      #ifdef DEBUG_WRITE_DATA_SEGMENT_MORE
      MessageInterface::ShowMessage("----- State vector %d\n", i+1);
      DebugWriteState(mStateArray[i], stateDULT, 2);
      DebugWriteStateVector(mEphemData.stateVector2Thru50_DULT[i-1], i-1, 1);
      #endif
   }
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("Check if sentinel data needs to be written out\n");
   #endif
   // If data points received is less than NUM_STATES_PER_RECORD, write sentinel flag
   if ((numPoints < NUM_STATES_PER_RECORD) && canFinalize)
   {
      #ifdef DEBUG_WRITE_SENTINELS
      MessageInterface::ShowMessage
         ("===> Writing %d sentinel data\n", NUM_STATES_PER_RECORD-numPoints);
      #endif
      
      for (int i = numPoints - 1; i < NUM_STATES_PER_RECORD - 1; i++)
      {
         for (int j = 0; j < 6; j++)
            WriteDoubleField(&mEphemData.stateVector2Thru50_DULT[i][j], mSentinelData);
         
         #ifdef DEBUG_WRITE_DATA_SEGMENT_MORE
         DebugWriteStateVector(mEphemData.stateVector2Thru50_DULT[i], i, 1);
         #endif
      }
   }
   
   // Write time of first data point and time interval in DUT
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("Writing time of first data point and time interval\n");
   #endif
   double timeInDUT = ToDUT(startMjd);
   WriteDoubleField(&mEphemData.timeOfFirstDataPoint_DUT, timeInDUT);
   WriteDoubleField(&mEphemData.timeIntervalBetweenPoints_SEC, mTimeIntervalBetweenPointsSecs);
   WriteDoubleField(&mEphemData.timeIntervalBetweenPoints_DUT, mTimeIntervalBetweenPointsSecs * SEC_TO_DUT);
   
   // Write data record
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("Calling WriteDataAt(%d)\n", mDataRecWriteCounter);
   #endif
   WriteDataAt(mDataRecWriteCounter);
   
   // Flush data to a file
   mEphemFileOut.flush();
   
   // If last data record contains NUM_STATES_PER_RECORD valid states and writing
   // final data, write next record with 10 sentinels.
   if (numPoints == NUM_STATES_PER_RECORD && canFinalize)
   {
      #ifdef DEBUG_WRITE_SENTINELS
      MessageInterface::ShowMessage
         ("===> Writing last data record with first 10 sentinel data\n");
      #endif
      
      // Initialize state vectors with sentinel value
      InitializeDataRecord();
      
      // Write 4 fields with sentinels
      WriteDoubleField(&mEphemData.dateOfFirstEphemPoint_YYYMMDD, mSentinelData);
      WriteDoubleField(&mEphemData.dayOfYearForFirstEphemPoint, mSentinelData);
      WriteDoubleField(&mEphemData.secsOfDayForFirstEphemPoint, mSentinelData);
      WriteDoubleField(&mEphemData.timeIntervalBetweenPoints_SEC, mSentinelData);
      
      mDataRecWriteCounter++;
      WriteDataAt(mDataRecWriteCounter);
   }
   
   // Flush data to a file
   mEphemFileOut.flush();
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteDataRecord() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// double ReadDoubleField(double *field)
//------------------------------------------------------------------------------
double Code500EphemerisFile::ReadDoubleField(double *field)
{
   double value = *field;
   if (mSwapInputEndian)
   {
      mDoubleOriginBytes.doubleValue = value;
      double swapped = SwapDouble();
      value = swapped;
   }
   return value;
}


//------------------------------------------------------------------------------
// int ReadIntegerField(int *field)
//------------------------------------------------------------------------------
int Code500EphemerisFile::ReadIntegerField(int *field)
{
   int value = *field;
   if (mSwapInputEndian)
   {
      mIntOriginBytes.intValue = value;
      int swapped = SwapInteger();
      value = swapped;
   }
   return value;
}


//------------------------------------------------------------------------------
// void WriteDoubleField(double *field, double value)
//------------------------------------------------------------------------------
void Code500EphemerisFile::WriteDoubleField(double *field, double value)
{
   if (mSwapOutputEndian)
   {
      mDoubleOriginBytes.doubleValue = value;
      double swapped = SwapDouble();
      *field = swapped;
   }
   else
   {
      *field = value;
   }
}


//------------------------------------------------------------------------------
// void WriteIntegerField(int *field, int value)
//------------------------------------------------------------------------------
void Code500EphemerisFile::WriteIntegerField(int *field, int value)
{
   if (mSwapOutputEndian)
   {
      mIntOriginBytes.intValue = value;
      int swapped = SwapInteger();
      *field = swapped;
   }
   else
   {
      *field = value;
   }
}


//------------------------------------------------------------------------------
// void ClearBuffer()
//------------------------------------------------------------------------------
void Code500EphemerisFile::ClearBuffer()
{
   #ifdef DEBUG_CLEAR_BUFFER
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::ClearBuffer() entered, mA1MjdArray.size()=%d, "
       "mStateArray.size()=%d\n", mA1MjdArray.size(), mStateArray.size());
   #endif
   
   EpochArray::iterator ei;
   for (ei = mA1MjdArray.begin(); ei != mA1MjdArray.end(); ++ei)
      delete (*ei);
   
   StateArray::iterator si;
   for (si = mStateArray.begin(); si != mStateArray.end(); ++si)
      delete (*si);
   
   mA1MjdArray.clear();
   mStateArray.clear();
}


//------------------------------------------------------------------------------
// void UnpackHeader1()
//------------------------------------------------------------------------------
void Code500EphemerisFile::UnpackHeader1()
{
   MessageInterface::ShowMessage("======================================== Begin of Header1\n");
   
   bool swap = mSwapInputEndian;
   MessageInterface::ShowMessage("mSwapInputEndian = %d\n", mSwapInputEndian);
   
   std::string str(mEphemHeader1.productId);
   MessageInterface::ShowMessage("productId                           = '%s'\n", str.substr(0,8).c_str());
   DebugDouble("satId                               = % f\n", mEphemHeader1.satId, swap);
   DebugDouble("timeSystemIndicator                 = % f\n", mEphemHeader1.timeSystemIndicator, swap);
   DebugDouble("StartDateOfEphem_YYYMMDD            = % f\n", mEphemHeader1.startDateOfEphem_YYYMMDD, swap);
   DebugDouble("startDayCountOfYear                 = % f\n", mEphemHeader1.startDayCountOfYear, swap);
   DebugDouble("startSecondsOfDay                   = % f\n", mEphemHeader1.startSecondsOfDay, swap);
   DebugDouble("endDateOfEphem_YYYMMDD              = % f\n", mEphemHeader1.endDateOfEphem_YYYMMDD, swap);
   DebugDouble("endDayCountOfYear                   = % f\n", mEphemHeader1.endDayCountOfYear, swap);
   DebugDouble("endSecondsOfDay                     = % f\n", mEphemHeader1.endSecondsOfDay, swap);
   DebugDouble("stepSize_SEC                        = % f\n", mEphemHeader1.stepSize_SEC, swap);
   
   double yyymmddStart = ReadDoubleField(&mEphemHeader1.startDateOfEphem_YYYMMDD);
   //MessageInterface::ShowMessage("==> yyymmddStart = %f\n", yyymmddStart);
   if (mInputYearFormat == 2) yyymmddStart = yyymmddStart - 19000000;
   //MessageInterface::ShowMessage("==> yyymmddStart = %f\n", yyymmddStart);
   double secsOfDayStart = ReadDoubleField(&mEphemHeader1.startSecondsOfDay);
   std::string ymdhmsStart = ToYearMonthDayHourMinSec(yyymmddStart, secsOfDayStart);
   MessageInterface::ShowMessage("startYYYYMMDDHHMMSSsss.             = %s\n", ymdhmsStart.c_str());
   
   double yyymmddEnd = ReadDoubleField(&mEphemHeader1.endDateOfEphem_YYYMMDD);
   if (mInputYearFormat == 2) yyymmddEnd = yyymmddEnd - 19000000;
   double secsOfDayEnd = ReadDoubleField(&mEphemHeader1.endSecondsOfDay);
   std::string ymdhmsEnd = ToYearMonthDayHourMinSec(yyymmddEnd, secsOfDayEnd);
   MessageInterface::ShowMessage("endYYYYMMDDHHMMSSsss.               = %s\n", ymdhmsEnd.c_str());
   
   std::string tapeIdStr = mEphemHeader1.tapeId;
   std::string sourceIdStr = mEphemHeader1.sourceId;
   std::string headerTitleStr = mEphemHeader1.headerTitle;
   std::string coordSystemStr = mEphemHeader1.coordSystemIndicator1;
   std::string orbitTheoryStr = mEphemHeader1.orbitTheory;
   MessageInterface::ShowMessage("tapeId                              = '%s'\n", tapeIdStr.substr(0,8).c_str());
   MessageInterface::ShowMessage("sourceId                            = '%s'\n", sourceIdStr.substr(0,8).c_str());
   MessageInterface::ShowMessage("headerTitle                         = '%s'\n", headerTitleStr.substr(0,56).c_str());
   DebugDouble("centralBodyIndicator                = % f\n", mEphemHeader1.centralBodyIndicator, swap);
   DebugDouble("coordinateCenterIndicator           = % f\n", mEphemHeader1.coordinateCenterIndicator, swap);
   DebugDouble("refTimeForDUT_YYMMDD                = % f\n", mEphemHeader1.refTimeForDUT_YYMMDD, swap);
   MessageInterface::ShowMessage("coordSystemIndicator1               = '%s'\n", coordSystemStr.substr(0,4).c_str());
   DebugInteger("coordSystemIndicator2               = %d\n", mEphemHeader1.coordSystemIndicator2, swap);
   DebugInteger("coordSystemIndicator2               = %d\n", mCoordSystemIndicator, swap);
   MessageInterface::ShowMessage("orbitTheory                         = '%s'\n", orbitTheoryStr.substr(0,8).c_str());
   double timeIntervalDUT = ReadDoubleField(&mEphemHeader1.timeIntervalBetweenPoints_DUT);
   DebugDouble("timeIntervalBetweenPoints_DUT       = % f\n", timeIntervalDUT);
   DebugDouble("timeIntervalBetweenPoints_SEC.      = % f\n", timeIntervalDUT * DUT_TO_SEC, false);
   DebugInteger("outputIntervalIndicator             = %d\n", mEphemHeader1.outputIntervalIndicator, swap);
   double epochTimeDUT = ReadDoubleField(&mEphemHeader1.epochTimeOfElements_DUT);
   DebugDouble("epochTimeOfElements_DUT             = % f\n", epochTimeDUT);
   DebugDouble("epochTimeOfElements_DAY.            = % f\n", epochTimeDUT * DUT_TO_DAY, false);
   
   double dutTime = ReadDoubleField(&mEphemHeader1.epochTimeOfElements_DUT);
   std::string epochA1Greg = ToA1Gregorian(dutTime, false);
   MessageInterface::ShowMessage("epochA1Greg.                        = '%s'\n", epochA1Greg.c_str());
   std::string epochUtcGreg = ToUtcGregorian(dutTime, false);
   MessageInterface::ShowMessage("epochUtcGreg.                       = '%s'\n", epochUtcGreg.c_str());
   
   DebugDouble("yearOfEpoch_YYY                     = % f\n", mEphemHeader1.yearOfEpoch_YYY, swap);
   DebugDouble("monthOfEpoch_MM                     = % f\n", mEphemHeader1.monthOfEpoch_MM, swap);
   DebugDouble("dayOfEpoch_DD                       = % f\n", mEphemHeader1.dayOfEpoch_DD, swap);
   DebugDouble("hourOfEpoch_HH                      = % f\n", mEphemHeader1.hourOfEpoch_HH, swap);
   DebugDouble("minuteOfEpoch_MM                    = % f\n", mEphemHeader1.minuteOfEpoch_MM, swap);
   DebugDouble("secondsOfEpoch_MILSEC               = % f\n", mEphemHeader1.secondsOfEpoch_MILSEC, swap);
   
   // SMA is not angular
   DebugDouble("keplerianElementsAtEpoch_RAD[0]     = % 1.15e\n",
               mEphemHeader1.keplerianElementsAtEpoch_RAD[0], swap);
   for (int i = 1; i < 6; i++)
   {
      double elemRad = ReadDoubleField(&mEphemHeader1.keplerianElementsAtEpoch_RAD[i]);
      DebugDouble("keplerianElementsAtEpoch_RAD[%d]     = % 1.15e\n", i, elemRad, false);
      DebugDouble("keplerianElementsAtEpoch_DEG[%d].    = % 1.15e\n", i, elemRad * GmatMathConstants::DEG_PER_RAD, false);
   }
   for (int i = 0; i < 3; i++)
   {
      double posDult = ReadDoubleField(&mEphemHeader1.cartesianElementsAtEpoch_DULT[i]);
      DebugDouble("cartesianElementsAtEpoch_DULT[%d]    = % 1.15e\n", i, posDult, false);
      DebugDouble("cartesianElementsAtEpoch_KMSE[%d].   = % 1.15e\n", i, posDult * DUL_TO_KM, false);
   }
   for (int i = 3; i < 6; i++)
   {
      double velDult = ReadDoubleField(&mEphemHeader1.cartesianElementsAtEpoch_DULT[i]);
      DebugDouble("cartesianElementsAtEpoch_DULT[%d]    = % 1.15e\n", i, velDult, false);
      DebugDouble("cartesianElementsAtEpoch_KMSE[%d].   = % 1.15e\n", i, velDult * DUL_DUT_TO_KM_SEC, false);
   }
   
   double rval = ReadDoubleField(&mEphemHeader1.startTimeOfEphemeris_DUT);
   DebugDouble("startTimeOfEphemeris_DUT            = % f\n", rval, false);
   DebugDouble("startTimeOfEphemeris_DAY.           = % f\n", rval * DUT_TO_DAY, false);
   rval = ReadDoubleField(&mEphemHeader1.endTimeOfEphemeris_DUT);
   DebugDouble("endTimeOfEphemeris_DUT              = % f\n", rval, false);
   DebugDouble("endTimeOfEphemeris_DAY.             = % f\n", rval * DUT_TO_DAY, false);
   rval = ReadDoubleField(&mEphemHeader1.timeIntervalBetweenPoints_DUT);
   DebugDouble("timeIntervalBetweenPoints_DUT       = % f\n", rval, false);
   DebugDouble("timeIntervalBetweenPoints_SEC.      = % f\n", rval * DUT_TO_SEC, false);
   DebugDouble("precessionNutationIndicator         = % f\n", mEphemHeader1.precessionNutationIndicator, swap);
   DebugDouble("zonalTesseralHarmonicsIndicator     = % f\n", mEphemHeader1.zonalTesseralHarmonicsIndicator, swap);
   DebugDouble("lunarGravPerturbIndicator           = % f\n", mEphemHeader1.lunarGravPerturbIndicator, swap);
   DebugDouble("solarRadiationPerturbIndicator      = % f\n", mEphemHeader1.solarRadiationPerturbIndicator, swap);
   DebugDouble("solarGravPerturbIndicator           = % f\n", mEphemHeader1.solarGravPerturbIndicator, swap);
   DebugDouble("atmosphericDragPerturbIndicator     = % f\n", mEphemHeader1.atmosphericDragPerturbIndicator, swap);
   DebugDouble("dateOfInitiationOfEphemComp_YYYMMDD = % f\n", mEphemHeader1.dateOfInitiationOfEphemComp_YYYMMDD, swap);
   DebugDouble("timeOfInitiationOfEphemComp_HHMMSS  = % f\n", mEphemHeader1.timeOfInitiationOfEphemComp_HHMMSS, swap);
   DebugInteger("leapSecondIndicator                 = % d\n", mEphemHeader1.leapSecondIndicator, swap);
   DebugDouble("dateOfLeapSeconds_YYYMMDD           = % f\n", mEphemHeader1.dateOfLeapSeconds_YYYMMDD, swap);
   DebugDouble("dateOfLeapSeconds_HHMMSS            = % f\n", mEphemHeader1.timeOfLeapSeconds_HHMMSS, swap);
   DebugDouble("utcTimeAdjustment_SEC               = % .9f\n", mEphemHeader1.utcTimeAdjustment_SEC, swap);
   
   MessageInterface::ShowMessage("======================================== End of Header1\n");
}


//------------------------------------------------------------------------------
// void UnpackHeader2()
//------------------------------------------------------------------------------
void Code500EphemerisFile::UnpackHeader2()
{
   #ifdef DEBUG_UNPACK_HEADERS
   MessageInterface::ShowMessage
      ("======================================== Begin of Header2\n  ... todo ...");
   #endif
}


//------------------------------------------------------------------------------
// void UnpackDataRecord(int recNum, int logOption)
//------------------------------------------------------------------------------
/**
 * Unpacks data fields in the data record.
 *
 * @param  recNum         Data record number (must start from 3)
 * @param  logOption      = 0, no log
 *                        = 1, log first state vector of only first and last record
 *                        = 2, log first and last state vector of all records
 *                        = 3, log all state vectors of all records
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::UnpackDataRecord(int recNum, int logOption)
{
   if ((logOption > 1) || (logOption == 1 && recNum == 1) || (logOption == 1 && mSentinelsFound))
      MessageInterface::ShowMessage
         ("======================================== Begin of data record %d\nmSentinelsFound = %s\n",
          recNum, mSentinelsFound ? "true" : "false");
   
   bool swap = mSwapInputEndian;
   double sentinels[50];
   sentinels[0] = ReadDoubleField(&mEphemData.dateOfFirstEphemPoint_YYYMMDD);
   sentinels[1] = ReadDoubleField(&mEphemData.dayOfYearForFirstEphemPoint);
   sentinels[2] = ReadDoubleField(&mEphemData.secsOfDayForFirstEphemPoint);
   sentinels[3] = ReadDoubleField(&mEphemData.timeIntervalBetweenPoints_SEC);
   
   mLastDataRecRead = recNum;
   
   if ((logOption > 1) || (logOption == 1 && recNum == 1) || (logOption == 1 && mSentinelsFound))
   {
      double firstDate = ReadDoubleField(&mEphemData.dateOfFirstEphemPoint_YYYMMDD);
      double firstSecs = ReadDoubleField(&mEphemData.secsOfDayForFirstEphemPoint);
      std::string ymdhmsStr = "sentinel";
      // If data and secs are not sentinels, convert to string
      if ((!GmatMathUtil::IsEqual(firstDate, mSentinelData, 10.0) &&
           !GmatMathUtil::IsEqual(firstSecs, mSentinelData, 10.0)))
         ymdhmsStr = ToYearMonthDayHourMinSec(firstDate, firstSecs);
      MessageInterface::ShowMessage("timeOfFirstEphemPoint.          =  %s\n", ymdhmsStr.c_str());
      DebugDouble("dateOfFirstEphemPoint_YYYMMDD   = % f\n", mEphemData.dateOfFirstEphemPoint_YYYMMDD, swap);
      DebugDouble("dayOfYearForFirstEphemPoint     = % f\n", mEphemData.dayOfYearForFirstEphemPoint, swap);
      DebugDouble("secsOfDayForFirstEphemPoint     = % f\n", mEphemData.secsOfDayForFirstEphemPoint, swap);
      DebugDouble("timeIntervalBetweenPoints_SEC   = % f\n", mEphemData.timeIntervalBetweenPoints_SEC, swap);
   }
   
   double posDult, velDult;
   
   // First position vector
   for (int j = 0; j < 3; j++)
   {
      sentinels[4+j] = ReadDoubleField(&mEphemData.firstStateVector_DULT[j]);
      posDult = ReadDoubleField(&mEphemData.firstStateVector_DULT[j]);
      if ((logOption > 1) || (logOption == 1 && recNum == 1) || (logOption == 1 && mSentinelsFound))
      {
         DebugDouble("firstStateVector_DULT[%d]        = % 1.15e\n", j, posDult, false);
         DebugDouble("firstStateVector_KMSE[%d].       = % 1.15e\n", j, posDult * DUL_TO_KM, false);
      }
   }
   
   // First velocity vector
   for (int j = 3; j < 6; j++)
   {
      sentinels[4+j] = ReadDoubleField(&mEphemData.firstStateVector_DULT[j]);
      velDult = ReadDoubleField(&mEphemData.firstStateVector_DULT[j]);
      if ((logOption > 1) || (logOption == 1 && recNum == 1) || (logOption == 1 && mSentinelsFound))
      {
         DebugDouble("firstStateVector_DULT[%d]        = % 1.15e\n", j, velDult, false);
         DebugDouble("firstStateVector_KMSE[%d].       = % 1.15e\n", j, velDult * DUL_DUT_TO_KM_SEC, false);
      }
   }
   
   
   // If sentinels already found, log data and return
   if (mSentinelsFound)
   {
      int i = mLastStateIndexRead;
      if (logOption >= 2)
         MessageInterface::ShowMessage("timeOfLastEphemPoint.           =  %s\n", mLastDataRecEndGreg.c_str());
      
      for (int j = 0; j < 3; j++)
      {
         posDult = ReadDoubleField(&mEphemData.stateVector2Thru50_DULT[i][j]);
         if (logOption >= 2)
         {           
            DebugDouble("stateVector2Thru50_DULT[%2d][%2d] = % 1.15e\n", i, j, posDult, false);
            DebugDouble("stateVector2Thru50_KMSE[%2d][%2d].= % 1.15e\n", i, j, posDult * DUL_TO_KM, false);
         }
      }
      
      for (int j = 3; j < 6; j++)
      {
         velDult = ReadDoubleField(&mEphemData.stateVector2Thru50_DULT[i][j]);
         if (logOption >= 2)
         {
            DebugDouble("stateVector2Thru50_DULT[%2d][%2d] = % 1.15e\n", i, j, velDult, false);
            DebugDouble("stateVector2Thru50_KMSE[%2d][%2d].= % 1.15e\n", i, j, velDult * DUL_DUT_TO_KM_SEC, false);
         }
      }
      return;
   }
   
   
   // Check for sentinels
   int sentinelCount = 0;
   for (int k = 0; k < 10; k++)
   {
      #ifdef DEBUG_UNPACK_DATA
      MessageInterface::ShowMessage("   sentinels[%d] = % 1.15e\n", k, sentinels[k]);
      #endif
      if (GmatMathUtil::IsEqual(sentinels[k], mSentinelData, 10.0))
         sentinelCount++;
   }
   
   #ifdef DEBUG_UNPACK_DATA
   MessageInterface::ShowMessage("   sentinelCount = %d\n", sentinelCount);
   #endif
   
   if (sentinelCount == 10)
   {
      if (logOption > 0)
         MessageInterface::ShowMessage("=====> First 10 sentinels found\n");
      mSentinelsFound = true;
      mLastStateIndexRead = -1;
      return;
   }
   
   // Initially set last state index to 48, index starts from 0 to 48
   mLastStateIndexRead = 48;
   
   // State vector 2 through NUM_STATES_PER_RECORD
   for (int i = 0; i < NUM_STATES_PER_RECORD - 1; i++)
   {
      sentinelCount = 0;
      for (int j = 0; j < 3; j++)
         sentinels[j] = ReadDoubleField(&mEphemData.stateVector2Thru50_DULT[i][j]);
      
      for (int j = 3; j < 6; j++)
         sentinels[j] = ReadDoubleField(&mEphemData.stateVector2Thru50_DULT[i][j]);
      
      if (logOption > 2 || (logOption == 2 && i == mLastStateIndexRead))
         DebugWriteStateVector(mEphemData.stateVector2Thru50_DULT[i], i, 6, swap);
      
      #ifdef DEBUG_UNPACK_DATA
      MessageInterface::ShowMessage("Checking for sentinels\n");
      #endif
      
      // Check for sentinels
      for (int k = 0; k < 6; k++)
      {
         if (GmatMathUtil::IsEqual(sentinels[k], mSentinelData, 10.0))
            sentinelCount++;
      }
      
      #ifdef DEBUG_UNPACK_DATA
      MessageInterface::ShowMessage("sentinelCount = %d\n", sentinelCount);
      #endif
      
      if (sentinelCount > 5)
      {
         if (logOption > 0)
            MessageInterface::ShowMessage("=====> State sentinels found\n");
         mSentinelsFound = true;
         mLastStateIndexRead = i - 1;
         break;
      }
      
      #ifdef DEBUG_UNPACK_DATA
      MessageInterface::ShowMessage("Checking for zero state vector\n");
      #endif
      
      // Check for zero vector
      sentinelCount = 0;
      for (int k = 0; k < 6; k++)
      {
         #ifdef DEBUG_UNPACK_DATA
         MessageInterface::ShowMessage("sentinels[%d] = % 1.15e\n", k, sentinels[k]);
         #endif
         // Check for zero state vector
         if (GmatMathUtil::IsZero(sentinels[k], 1e-12))
            sentinelCount++;
      }
      
      #ifdef DEBUG_UNPACK_DATA
      MessageInterface::ShowMessage("sentinelCount = %d\n", sentinelCount);
      #endif
      
      if (sentinelCount > 5)
      {
         if (logOption > 0)
            MessageInterface::ShowMessage("=====> State zero vector found\n");
         mSentinelsFound = true;
         mLastStateIndexRead = i - 1;
         break;
      }
   }
   
   #ifdef DEBUG_UNPACK_DATA
   MessageInterface::ShowMessage("mLastStateIndexRead = %d\n", mLastStateIndexRead);
   #endif
   
   if ((logOption > 1) || (logOption == 1 && recNum == 1) || (logOption == 1 && mSentinelsFound))
   {
      DebugDouble("timeOfFirstDataPoint_DUT        = % f\n", mEphemData.timeOfFirstDataPoint_DUT, swap);
      DebugDouble("timeIntervalBetweenPoints_DUT   = % f\n", mEphemData.timeIntervalBetweenPoints_DUT, swap);
      DebugDouble("thrustIndicator                 = % f\n", mEphemData.thrustIndicator, swap);
      MessageInterface::ShowMessage
         ("======================================== End of data record %d\n", recNum);
   }

   ephemRecords.push_back(mEphemData);
}


//------------------------------------------------------------------------------
// void ConvertStateKmSecToDULT(Rvector6 *kmsec, double *dult)
//------------------------------------------------------------------------------
void Code500EphemerisFile::ConvertStateKmSecToDULT(Rvector6 *kmsec, double *dult)
{
   // DUT = 864 seconds
   // DUL = 10000 km

   dult[0] = kmsec->Get(0) * KM_TO_DUL;
   dult[1] = kmsec->Get(1) * KM_TO_DUL;
   dult[2] = kmsec->Get(2) * KM_TO_DUL;
   dult[3] = kmsec->Get(3) * KM_SEC_TO_DUL_DUT;
   dult[4] = kmsec->Get(4) * KM_SEC_TO_DUL_DUT;
   dult[5] = kmsec->Get(5) * KM_SEC_TO_DUL_DUT;
}


//------------------------------------------------------------------------------
// void ConvertAsciiToEbcdic(char *ascii, char *ebcdic, int numChars)
//------------------------------------------------------------------------------
void Code500EphemerisFile::ConvertAsciiToEbcdic(char *ascii, char *ebcdic, int numChars)
{
   unsigned char asc;
   unsigned char ebc;
   
   for (int i = 0; i < numChars; i++)
   {
      asc = (unsigned char)ascii[i];
      ebc = AsciiToEbcdic(asc);
      ebcdic[i] = (char)ebc;
   }
}


//------------------------------------------------------------------------------
// void ConvertEbcdicToAscii(char *ebcdic, char *ascii, int numChars)
//------------------------------------------------------------------------------
void Code500EphemerisFile::ConvertEbcdicToAscii(char *ebcdic, char *ascii, int numChars)
{
   unsigned char asc;
   unsigned char ebc;
   
   for (int i = 0; i < numChars; i++)
   {
      ebc = (unsigned char)ebcdic[i];
      asc = EbcdicToAscii(ebc);
      ascii[i] = (char)asc;
   }
}


//------------------------------------------------------------------------------
// Real Code500EphemerisFile::GetTimeSystem()
//------------------------------------------------------------------------------
/**
 * Returns the time system as set mOutputTimeSystem
 *
 * @return The real number value for the time system setting
 */
//------------------------------------------------------------------------------
Real Code500EphemerisFile::GetTimeSystem()
{
   return mOutputTimeSystem;
}


//------------------------------------------------------------------------------
// std::string GetCentralBody()
//------------------------------------------------------------------------------
/**
 * Retrieves the name of the central body of output ephem
 *
 * @return The central body name
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::GetCentralBody()
{
   mOutputCentralBody = GetBodyName(mCentralBodyOfOutputEphem, 2);
   return mOutputCentralBody;
}


//------------------------------------------------------------------------------
// Integer GetCoordSystemIndicator()
//------------------------------------------------------------------------------
/**
* Retrieves the coordinate system indicator
*
* @return The coordinate system indicator
*/
//------------------------------------------------------------------------------
Integer Code500EphemerisFile::GetCoordSystemIndicator()
{
   return mCoordSystemIndicator;
}


//------------------------------------------------------------------------------
// void ToYearMonthDayHourMinSec(double yyymmdd, double secsOfDay, ...
//------------------------------------------------------------------------------
void Code500EphemerisFile::ToYearMonthDayHourMinSec(double yyymmdd, double secsOfDay,
                              int &year, int &month, int &day,
                              int &hour, int &min, double &sec)
{
   ToYearMonthDay(yyymmdd, year, month, day);
   ToHMSFromSecondsOfDay(secsOfDay, hour, min, sec);
}


//------------------------------------------------------------------------------
// std::string ToYearMonthDayHourMinSec(double yyymmdd, double secsOfDay)
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToYearMonthDayHourMinSec(double yyymmdd, double secsOfDay)
{
   int year, month, day, hour, min;
   double sec;
   ToYearMonthDayHourMinSec(yyymmdd, secsOfDay, year, month, day, hour, min, sec);
   char buffer[30];
   sprintf(buffer, "%d-%02d-%02d %02d:%02d:%09.6f", year, month, day, hour, min, sec);
   return std::string(buffer);
}


//------------------------------------------------------------------------------
// void ToYearMonthDay(double yyymmdd, int &year, int &month, int &day)
//------------------------------------------------------------------------------
/**
 * Converts time in yyymmdd to yyyy, mm, day.
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::ToYearMonthDay(double yyymmdd, int &year, int &month, int &day)
{
   double yyyymmdd = yyymmdd + 19000000;
   UnpackDate(yyyymmdd, year, month, day);
}


//------------------------------------------------------------------------------
// void ToYYYMMDDHHMMSS(double mjd, double &ymd, double &hms)
//------------------------------------------------------------------------------
void Code500EphemerisFile::ToYYYMMDDHHMMSS(double mjd, double &ymd, double &hms)
{
   A1Mjd tempA1Mjd(mjd);
   A1Date a1Date = tempA1Mjd.ToA1Date();
   a1Date.ToYearMonthDayHourMinSec(ymd, hms);

   // Since ymd is in yyyymmdd, subtract 19000000
   ymd = ymd - 19000000.0;
}


//------------------------------------------------------------------------------
// double ToDUT(double mjd)
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToDUT(double mjd)
{
   double dut = (mjd - mMjdOfDUTRef) * DAY_TO_DUT;
   return dut;
}


//------------------------------------------------------------------------------
// double ToUtcModJulian(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToUtcModJulian(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToUtcModJulian() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   
   Real a1MjdReal = a1Mjd.GetReal();
   
   //std::string epochStr;
   //Integer format = 1;
   // Need to apply leap seconds, so use TimeSystemConverter
   // Convert current epoch to specified format
   //theTimeConverter->Convert("A1ModJulian", mjd, "", "UTCModJulian",
   //                           utcMjd, epochStr, format);
   
   Real utcMjd = theTimeConverter->Convert(a1MjdReal, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD);
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToUtcModJulian() returning utcMjd=%f\n", utcMjd);
   #endif
   return utcMjd;
}


//------------------------------------------------------------------------------
// double ToYYYMMDD(double mjd)
//------------------------------------------------------------------------------
/**
 * Converts mod julian days to YYYMMDD format. Assuming input mjd has
 * leap seconds already applied for UTC time system.
 *
 * @param  mjd  Modified julian days in A1 or UTC, or other time system
 */
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToYYYMMDD(double mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToYYYMMDD() entered, mjd=% f\n", mjd);
   #endif
   
   A1Mjd tempA1Mjd(mjd);
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double yyymmdd = a1Date.ToPackedYYYMMDD();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToYYYMMDD() returning % f\n", yyymmdd);
   #endif
   
   return yyymmdd;
}


//------------------------------------------------------------------------------
// double ToHHMMSS(double mjd)
//------------------------------------------------------------------------------
/**
 * Converts mod julian days to HHMMSS.mmm format. Assuming input mjd has
 * leap seconds already applied for UTC time system.
 *
 * @param  mjd  Modified julian days in A1 or UTC, or other time system
 */
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToHHMMSS(double mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToHHMMSS() entered, mjd=f\n", mjd);
   #endif
   
   A1Mjd tempA1Mjd(mjd);
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double hhmmss = a1Date.ToPackedHHMMSS();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToHHMMSS() returning % f\n", hhmmss);
   #endif
   
   return hhmmss;
}


//------------------------------------------------------------------------------
// double ToDayOfYear(double mjd)
//------------------------------------------------------------------------------
/**
 * Converts mod julian days to day of year. Assuming input mjd has
 * leap seconds already applied for UTC time system.
 *
 * @param  mjd  Modified julian days in A1 or UTC, or other time system
 */
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToDayOfYear(double mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToDayOfYear() entered, mjd=%f\n", mjd);
   #endif
   
   A1Mjd tempA1Mjd(mjd);
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double doy = a1Date.ToDayOfYear();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToDayOfYear() returning % f\n", doy);
   #endif
   
   return doy;
}


//------------------------------------------------------------------------------
// double ToSecondsOfDay(double mjd)
//------------------------------------------------------------------------------
/**
 * Converts mod julian days to seconds of day part. Assuming input mjd has
 * leap seconds already applied for UTC time system.
 *
 * @param  mjd  Modified julian days in A1 or UTC, or other time system
 */
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToSecondsOfDay(double mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToSecondsOfDay() entered, mjd=%f\n", mjd);
   #endif
   
   A1Mjd tempA1Mjd(mjd);
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double secs = a1Date.GetSecondsOfDay();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToSecondsOfDay() returning % f\n", secs);
   #endif
   
   return secs;
}


//------------------------------------------------------------------------------
// A1Mjd  ToA1Mjd(double dutTime, bool forOutput = true)
//------------------------------------------------------------------------------
/**
 *  Converts DUT time in given time system to A1Mjd.
 *
 * @param  dutTime     Input time in DUT unit(centiday)
 * @param  forOutput   true if converting for writing
 */
//------------------------------------------------------------------------------
A1Mjd Code500EphemerisFile::ToA1Mjd(double dutTime, bool forOutput)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToA1Mjd() entered, dutTime=%f, forOutput=%d, mInputTimeSystem=%f\n",
       dutTime, forOutput, mInputTimeSystem);
   #endif
   
   Real timeOffset = 0.0;
   double a1MjdReal = (dutTime * DUT_TO_DAY) + mMjdOfDUTRef + timeOffset;
   A1Mjd a1Mjd = A1Mjd(a1MjdReal);
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToA1Mjd() returning a1Mjd=%f for dutTime=%f\n", a1MjdReal, dutTime);
   #endif
   
   return a1Mjd;
}


//------------------------------------------------------------------------------
// std::string ToA1Gregorian(double dutTime, bool forOutput = true)
//------------------------------------------------------------------------------
/**
 * Converts DUT time to A1 Gregorian time system (no leap seconds)
 *
 * @param  dutTime  time in DUT unit (centiday)
 * @param  forOutput  true if converting for writing 
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToA1Gregorian(double dutTime, bool forOutput)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToA1Gregorian() entered, dutTime=%f, forOutput=%d\n", dutTime, forOutput);
   #endif
   
   A1Mjd tempA1Mjd = ToA1Mjd(dutTime, forOutput);
   std::string epochStr = ToA1Gregorian(tempA1Mjd);
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToA1Gregorian() returning '%s' for dutTime=%f\n", epochStr.c_str(), dutTime);
   #endif
   
   return epochStr;
}


//------------------------------------------------------------------------------
// std::string ToA1Gregorian(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
/**
 * Converts A1 mod julian time to A1 Gregorian time system (no leap seconds)
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToA1Gregorian(const A1Mjd &a1Mjd)
{
   // format 1 = "01 Jan 2000 11:59:28.000"
   //        2 = "2000-01-01T11:59:28.000"
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToA1Gregorian() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   Integer format = 1;
   A1Mjd tempA1Mjd = a1Mjd;
   A1Date a1Date = tempA1Mjd.ToA1Date();
   GregorianDate gregorianDate(&a1Date, format);
   std::string epochStr = gregorianDate.GetDate();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToA1Gregorian() returning '%s' for a1Mjd=%f\n", epochStr.c_str(), a1Mjd.GetReal());
   #endif
   
   return epochStr;
}


//------------------------------------------------------------------------------
// std::string ToUtcGregorian(double dutTime, bool forOutput = true)
//------------------------------------------------------------------------------
/**
 * Converts DUT time to UTC Gregorian time system.
 *
 * @param  dutTime  time in DUT unit (centiday)
 * @param  forOutput  true if converting for writing
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToUtcGregorian(double dutTime, bool forOutput)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToUtcGregorian() entered, dutTime=%f, forOutput=%d\n", dutTime, forOutput);
   #endif
   
   A1Mjd tempA1Mjd = ToA1Mjd(dutTime, forOutput);
   std::string epochStr = ToUtcGregorian(tempA1Mjd, forOutput);
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToUtcGregorian() returning '%s' for dutTime=%f\n", epochStr.c_str(), dutTime);
   #endif
   
   return epochStr;
}


//------------------------------------------------------------------------------
// std::string ToUtcGregorian(const A1Mjd &a1Mjd, bool forOutput = true)
//------------------------------------------------------------------------------
/**
 * Converts A1 mod julian time to UTC Gregorian time system (with leap seconds)
 *
 * @param  a1Mjd  time in A1 mod julian
 * @param  forOutput  true if converting for writing
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToUtcGregorian(const A1Mjd &a1Mjd, bool forOutput)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToUtcGregorian() entered, a1Mjd=%f, forOutput=%d\n", a1Mjd.GetReal(), forOutput);
   #endif
   
   // format 1 = "01 Jan 2000 11:59:28.000"
   //        2 = "2000-01-01T11:59:28.000"
   Integer format = 1;
   std::string epochStr;
   if (forOutput)
   {
      Real utcMjd;
      Real epochInDays = a1Mjd.GetReal();
      
      // Need to apply leap seconds, so use TimeSystemConverter
      // Convert current epoch to specified format
      theTimeConverter->Convert("A1ModJulian", epochInDays, "", "UTCGregorian",
                                 utcMjd, epochStr, format);
      
      if (epochStr == "")
      {
         MessageInterface::ShowMessage
            ("**** ERROR **** Code500EphemerisFile::ToUtcGregorian() Cannot convert epoch %.10f "
             "to UTCGregorian\n", a1Mjd.GetReal());
         
         epochStr = "EpochError";
      }
   }
   else
   {
      // No extra leap seconds applied
      epochStr = ToA1Gregorian(a1Mjd);
   }
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToUtcGregorian() returning '%s' for a1Mjd=%f\n", epochStr.c_str(), a1Mjd.GetReal());
   #endif
   return epochStr;
}

//------------------------------------------------------------------------------
// double GetBodyIndicator(const std::string &bodyName, int forWhich)
//------------------------------------------------------------------------------
/**
 * Returns body indicator for integration or ephemeris output
 *
 * @param forWhich body indicator for integration = 1
 *                 body indicator for ephemeris output = 2
 */
//------------------------------------------------------------------------------
double Code500EphemerisFile::GetBodyIndicator(const std::string &bodyName, int forWhich)
{
   // Central body name from central body of integration
   // 1.0 = Earth, 2.0 = Luna(Earth Moon), 3.0 = Sun, 4.0 = Mars, 5.0 = Jupiter,
   // 6.0 = Saturn, 7.0 = Uranus, 8.0 = Neptune, 9.0 = Pluto, 10.0 = Mercury,
   // 11.0 = Venus
   
   // Central body name from central body of ephem output
   // 0.0 = Earth, 1.0 = Luna(Earth Moon), 2.0 = Sun, 3.0 = Mars, 4.0 = Jupiter,
   // 5.0 = Saturn, 6.0 = Uranus, 7.0 = Neptune, 8.0 = Pluto, 9.0 = Mercury,
   // 10.0 = Venus
   
   double bodyIndicator = -99.99;
   if (bodyName == "Earth")
      bodyIndicator = 1.0;
   else if (bodyName == "Luna")
      bodyIndicator = 2.0;
   else if (bodyName == "Sun")
      bodyIndicator = 3.0;
   else if (bodyName == "Mars")
      bodyIndicator = 4.0;
   else if (bodyName == "Jupiter")
      bodyIndicator = 5.0;
   else if (bodyName == "Saturn")
      bodyIndicator = 6.0;
   else if (bodyName == "Uranus")
      bodyIndicator = 7.0;
   else if (bodyName == "Neptune")
      bodyIndicator = 8.0;
   else if (bodyName == "Pluto")
      bodyIndicator = 9.0;
   else if (bodyName == "Mercury")
      bodyIndicator = 10.0;
   else if (bodyName == "Venus")
      bodyIndicator = 11.0;
   else
      bodyIndicator = -99.99;
   
   if (forWhich == 2)
      if (bodyIndicator != -99.99)
         bodyIndicator = bodyIndicator - 1;
   
   return bodyIndicator;
}

//------------------------------------------------------------------------------
// std::string GetBodyName(double bodyInd, int forWhich)
//------------------------------------------------------------------------------
/**
 * Returns body name for integration or ephemeris output
 *
 * @param bodyInd  body index
 * @param forWhich body string for integration = 1
 *                 body string for ephemeris output = 2
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::GetBodyName(double bodyInd, int forWhich)
{
   #ifdef DEBUG_BODY_NAME
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::GetBodyName() entered, bodyInd=%f, forWhich=%d\n",
       bodyInd, forWhich);
   #endif
   
   std::string bodyName = "Unknown";
   double bodyIndicator = bodyInd;
   
   // Central body name from central body of integration
   // 1.0 = Earth, 2.0 = Luna(Earth Moon), 3.0 = Sun, 4.0 = Mars, 5.0 = Jupiter,
   // 6.0 = Saturn, 7.0 = Uranus, 8.0 = Neptune, 9.0 = Pluto, 10.0 = Mercury,
   // 11.0 = Venus

   // Central body name from central body of ephem output
   // 0.0 = Earth, 1.0 = Luna(Earth Moon), 2.0 = Sun, 3.0 = Mars, 4.0 = Jupiter,
   // 5.0 = Saturn, 6.0 = Uranus, 7.0 = Neptune, 8.0 = Pluto, 9.0 = Mercury,
   // 10.0 = Venus
   
   if (forWhich == 2)
      bodyIndicator = bodyInd + 1;
   
   #ifdef DEBUG_BODY_NAME
   MessageInterface::ShowMessage("   bodyIndicator=%d\n", bodyIndicator);
   #endif
   
   if (bodyIndicator == 1.0)
      bodyName = "Earth";
   else if (bodyIndicator == 2.0)
      bodyName = "Luna";
   else if (bodyIndicator == 3.0)
      bodyName = "Sun";
   else if (bodyIndicator == 4.0)
      bodyName = "Mars";
   else if (bodyIndicator == 5.0)
      bodyName = "Jupiter";
   else if (bodyIndicator == 6.0)
      bodyName = "Saturn";
   else if (bodyIndicator == 7.0)
      bodyName = "Uranus";
   else if (bodyIndicator == 8.0)
      bodyName = "Neptune";
   else if (bodyIndicator == 9.0)
      bodyName = "Pluto";
   else if (bodyIndicator == 10.0)
      bodyName = "Mercury";
   else if (bodyIndicator == 11.0)
      bodyName = "Venus";
   
   #ifdef DEBUG_BODY_NAME
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::GetBodyName() returning '%s'\n", bodyName.c_str());
   #endif
   
   return bodyName;
}

//------------------------------------------------------------------------------
// void CopyString(char *to, const std::string &from, int numChars)
//------------------------------------------------------------------------------
void Code500EphemerisFile::CopyString(char *to, const std::string &from, int numChars)
{
   #ifdef DEBUG_COPY_STRING
   MessageInterface::ShowMessage
      ("CopyString() entered, from='%s', numChars=%d\n", from.c_str(), numChars);
   #endif
   
   int fromSize = from.size();
   
   for (int i = 0; i < fromSize; i++)
      to[i] = from[i];
   
   if (fromSize < numChars)
      for (int i = fromSize; i < numChars; i++)
         to[i] = ' ';

   std::string toStr(to);

   #ifdef DEBUG_COPY_STRING
   MessageInterface::ShowMessagememcopoy
      ("   from = '%s'\n   to   = '%s'\n", from.c_str(), toStr.substr(0,numChars).c_str());
   MessageInterface::ShowMessage("CopyString() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void CopyString(char *to, char *from, int numChars)
//------------------------------------------------------------------------------
void Code500EphemerisFile::CopyString(char *to, char *from, int numChars)
{
   memcpy(to, from, numChars);
}


//------------------------------------------------------------------------------
// void BlankOut(char *str, int numChars)
//------------------------------------------------------------------------------
void Code500EphemerisFile::BlankOut(char *str, int numChars)
{
   for (int i = 0; i < numChars; i++)
      str[i] = ' ';
}


//------------------------------------------------------------------------------
// unsigned char AsciiToEbcdic(unsigned char ascii)
//------------------------------------------------------------------------------
unsigned char Code500EphemerisFile::AsciiToEbcdic(unsigned char ascii)
{
    unsigned char ebcd;
    static unsigned char ascToEbcTable[256]=
    {
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x4F,0x7F,0x7B,0x5B,0x6C,0x50,0x7D,    /*  !"#$%&' */
       0x4D,0x5D,0x5C,0x4E,0x6B,0x60,0x4B,0x61,    /* ()*+,-./ */
       0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,    /* 01234567 */
       0xF8,0xF9,0x7A,0x5E,0x4C,0x7E,0x6E,0x6F,    /* 89:;<=>? */
       0x7C,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,    /* @ABCDEFG */
       0xC8,0xC9,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,    /* HIJKLMNO */
       0xD7,0xD8,0xD9,0xE2,0xE3,0xE4,0xE5,0xE6,    /* PQRSTUVW */
       0xE7,0xE8,0xE9,0x4A,0xE0,0x5A,0x5F,0x6D,    /* XYZ[\]^_ */
       0x79,0x81,0x82,0x83,0x84,0x85,0x86,0x87,    /* `abcdefg */
       0x88,0x89,0x91,0x92,0x93,0x94,0x95,0x96,    /* hijklmno */
       0x97,0x98,0x99,0xA2,0xA3,0xA4,0xA5,0xA6,    /* pqrstuvw */
       0xA7,0xA8,0xA9,0xC0,0x6A,0xD0,0xA1,0x40,    /* xyz{|}~  */
       0xB9,0xBA,0xED,0xBF,0xBC,0xBD,0xEC,0xFA,    /*          */
       0xCB,0xCC,0xCD,0xCE,0xCF,0xDA,0xDB,0xDC,    /*          */
       0xDE,0xDF,0xEA,0xEB,0xBE,0xCA,0xBB,0xFE,    /*          */
       0xFB,0xFD,0x7d,0xEF,0xEE,0xFC,0xB8,0xDD,    /*          */
       0x77,0x78,0xAF,0x8D,0x8A,0x8B,0xAE,0xB2,    /*          */
       0x8F,0x90,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0xAA,0xAB,0xAC,0xAD,0x8C,0x8E,0x80,0xB6,    /*          */
       0xB3,0xB5,0xB7,0xB1,0xB0,0xB4,0x76,0xA0,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40     /*          */
    };
    
    ebcd = ascToEbcTable[ascii];
    return (ebcd);
}


//------------------------------------------------------------------------------
// unsigned char EbcdicToAscii(unsigned char ebcd)
//------------------------------------------------------------------------------
unsigned char Code500EphemerisFile::EbcdicToAscii(unsigned char ebcd)
{
    unsigned char asc;
    
    static int ebcToAscTable[256]=
    {
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 91, 46, 60, 40, 43, 33,
            38,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 93, 36, 42, 41, 59, 94,
            45,  47, 32, 32, 32, 32, 32, 32,
            32,  32,124, 44, 37, 95, 62, 63,
            32,  32, 32, 32, 32, 32,238,160,
            161, 96, 58, 35, 64, 39, 61, 34,
            230, 97, 98, 99,100,101,102,103,
            104,105,164,165,228,163,229,168,
            169,106,107,108,109,110,111,112,
            113,114,170,171,172,173,174,175,
            239,126,115,116,117,118,119,120,
            121,122,224,225,226,227,166,162,
            236,235,167,232,237,233,231,234,
            158,128,129,150,132,133,148,131,
            123, 65, 66, 67, 68, 69, 70, 71,
            72,  73,149,136,137,138,139,140,
            125, 74, 75, 76, 77, 78, 79, 80,
            81,  82,141,142,143,159,144,145,
            92,  32, 83, 84, 85, 86, 87, 88,
            89,  90,146,147,134,130,156,155,
            48,  49, 50, 51, 52, 53, 54, 55,
            56,  57,135,152,157,153,151, 32
    };
    
    asc = ebcToAscTable[ebcd];
    return (asc);
}

#if 0
//------------------------------------------------------------------------------
// void SwapEndian(char *input, int numBytes)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SwapEndian(char *input, int numBytes)
{
   char *temp = NULL;
   
   temp = new char[numBytes];
   memcpy( temp, input, numBytes);
   for (int i = 0; i < numBytes; i++)
      input[(numBytes - 1) - i] = temp[i];
   
   delete [] temp;
}
#endif


//------------------------------------------------------------------------------
// double SwapDouble()
//------------------------------------------------------------------------------
double Code500EphemerisFile::SwapDouble()
{
   return SwapDoubleBytes(mDoubleOriginBytes.doubleBytes.byte1, mDoubleOriginBytes.doubleBytes.byte2,
                          mDoubleOriginBytes.doubleBytes.byte3, mDoubleOriginBytes.doubleBytes.byte4,
                          mDoubleOriginBytes.doubleBytes.byte5, mDoubleOriginBytes.doubleBytes.byte6,
                          mDoubleOriginBytes.doubleBytes.byte7, mDoubleOriginBytes.doubleBytes.byte8);
}

//------------------------------------------------------------------------------
// double SwapDoubleBytes(const Byte byte1, const Byte byte2, const Byte byte3, ...)
//------------------------------------------------------------------------------
double Code500EphemerisFile::SwapDoubleBytes(const Byte byte1, const Byte byte2, const Byte byte3,
                                             const Byte byte4, const Byte byte5, const Byte byte6,
                                             const Byte byte7, const Byte byte8)
{
   mDoubleSwappedBytes.doubleBytes.byte1 = byte8;
   mDoubleSwappedBytes.doubleBytes.byte2 = byte7;
   mDoubleSwappedBytes.doubleBytes.byte3 = byte6;
   mDoubleSwappedBytes.doubleBytes.byte4 = byte5;
   mDoubleSwappedBytes.doubleBytes.byte5 = byte4;
   mDoubleSwappedBytes.doubleBytes.byte6 = byte3;
   mDoubleSwappedBytes.doubleBytes.byte7 = byte2;
   mDoubleSwappedBytes.doubleBytes.byte8 = byte1;
   return mDoubleSwappedBytes.doubleValue;
}


//------------------------------------------------------------------------------
// int SwapInteger()
//------------------------------------------------------------------------------
int Code500EphemerisFile::SwapInteger()
{
   return SwapIntegerBytes(mIntOriginBytes.intBytes.byte1, mIntOriginBytes.intBytes.byte2,
                           mIntOriginBytes.intBytes.byte3, mIntOriginBytes.intBytes.byte4);
}


//------------------------------------------------------------------------------
// int SwapIntegerBytes(const Byte byte1, const Byte byte2, const Byte byte3, const Byte byte4)
//------------------------------------------------------------------------------
int Code500EphemerisFile::SwapIntegerBytes(const Byte byte1, const Byte byte2,
                                           const Byte byte3, const Byte byte4)
{
   mIntSwappedBytes.intBytes.byte1 = byte4;
   mIntSwappedBytes.intBytes.byte2 = byte3;
   mIntSwappedBytes.intBytes.byte3 = byte2;
   mIntSwappedBytes.intBytes.byte4 = byte1;
   return mIntSwappedBytes.intValue;
}


//------------------------------------------------------------------------------
// void DebugDouble(const std::string &fieldAndFormat, double value, bool swapEndian = false)
//------------------------------------------------------------------------------
/**
 * Writes debug out of double field value.
 *
 * @param  fieldAndFormat  Filed name and format
 * @param  value  Real value
 * @param  swapEndian  true if swapping endian; false otherwise [false]
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::DebugDouble(const std::string &fieldAndFormat, double value,
                                       bool swapEndian)
{
   if (swapEndian)
      MessageInterface::ShowMessage(fieldAndFormat.c_str(), SwapDoubleEndian(value));
   else
      MessageInterface::ShowMessage(fieldAndFormat.c_str(), value);
}


//------------------------------------------------------------------------------
// void DebugDouble(const std::string &fieldAndFormat, double value, int index,
//                  bool swapEndian = false)
//------------------------------------------------------------------------------
/**
 * Writes debug out of double field value.
 *
 * @param  fieldAndFormat  Filed name and format
 * @param  index  Index of input field
 * @param  value  Real value
 * @param  swapEndian  true if swapping endian; false otherwise
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::DebugDouble(const std::string &fieldAndFormat, int index,
                                       double value, bool swapEndian)
{
   if (swapEndian)
      MessageInterface::ShowMessage(fieldAndFormat.c_str(), index, SwapDoubleEndian(value));
   else
      MessageInterface::ShowMessage(fieldAndFormat.c_str(), index, value);
}


//------------------------------------------------------------------------------
// void DebugDouble(const std::string &fieldAndFormat, double value, int index1,
//                  int index2, bool swapEndian = false)
//------------------------------------------------------------------------------
/**
 * Writes debug out of double field value.
 *
 * @param  fieldAndFormat  Filed name and format
 * @param  index1  Row index of input field
 * @param  index2  Column index of input field
 * @param  value  Real value
 * @param  swapEndian  true if swapping endian; false otherwise
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::DebugDouble(const std::string &fieldAndFormat, int index1,
                                       int index2, double value, bool swapEndian)
{
   if (swapEndian)
      MessageInterface::ShowMessage(fieldAndFormat.c_str(), index1, index2, SwapDoubleEndian(value));
   else
      MessageInterface::ShowMessage(fieldAndFormat.c_str(), index1, index2, value);
}


//------------------------------------------------------------------------------
// void DebugInteger(const std::string &fieldAndFormat, int value, bool swapEndian = false)
//------------------------------------------------------------------------------
/**
 * Writes debug out of integer field value.
 *
 * @param  fieldAndFormat  Filed name and format
 * @param  value  Integer value
 * @param  swapEndian  Set to true if swapping endian; false otherwise
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::DebugInteger(const std::string &fieldAndFormat, int value,
                                       bool swapEndian)
{
   if (swapEndian)
      MessageInterface::ShowMessage(fieldAndFormat.c_str(), SwapIntegerEndian(value));
   else
      MessageInterface::ShowMessage(fieldAndFormat.c_str(), value);
}


//------------------------------------------------------------------------------
// void DebugWriteStateVector(double *stateDULT, int i, int numElem = 1,
//                            bool swapEndian = false)
//------------------------------------------------------------------------------
void Code500EphemerisFile::DebugWriteStateVector(double *stateDULT, int i, int numElem,
                                                 bool swapEndian)
{
   double posDult, velDult;
   
   for (int j = 0; j < 3; j++)
   {
      if (numElem < j)
         break;
      
      if (swapEndian)
         posDult = ReadDoubleField(&mEphemData.stateVector2Thru50_DULT[i][j]);
      else
         posDult = mEphemData.stateVector2Thru50_DULT[i][j];
      
      MessageInterface::ShowMessage
         ("stateVector2Thru50_DULT[%2d][%2d] = % 1.15e\n", i, j, posDult);
      MessageInterface::ShowMessage
         ("stateVector2Thru50_KMSE[%2d][%2d].= % 1.15e\n", i, j, posDult * DUL_TO_KM);
   }
   
   for (int j = 3; j < 6; j++)
   {
      if (numElem < j)
         break;
      
      if (swapEndian)
         velDult = ReadDoubleField(&mEphemData.stateVector2Thru50_DULT[i][j]);
      else
         velDult = mEphemData.stateVector2Thru50_DULT[i][j];
      
      MessageInterface::ShowMessage
         ("stateVector2Thru50_DULT[%2d][%2d] = % 1.15e\n", i, j, velDult);
      MessageInterface::ShowMessage
         ("stateVector2Thru50_KMSE[%2d][%2d].= % 1.15e\n", i, j, velDult * DUL_DUT_TO_KM_SEC);
   }
}


//------------------------------------------------------------------------------
// void DebugWriteState(Rvector6 *stateKmSec, double *stateDULT, int option = 1)
//------------------------------------------------------------------------------
/**
 * Write state in km/sec or in DULT.
 *
 * @param option 1 = Write only in km/sec
 *               2 = Write in km/sec and in DUL/DUT
 *               other = Write only in DUL/DUT
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::DebugWriteState(Rvector6 *stateKmSec,
                                           double *stateDULT, int option)
{
   if (option == 1 || option == 2)
   {
      MessageInterface::ShowMessage
         ("stateKmSec = % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e\n", 
          stateKmSec->Get(0), stateKmSec->Get(1), stateKmSec->Get(2),
          stateKmSec->Get(3), stateKmSec->Get(4), stateKmSec->Get(5));
      if (option == 2)
         MessageInterface::ShowMessage
            ("stateDULT  = % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e\n",
             stateDULT[0], stateDULT[1], stateDULT[2], stateDULT[3], stateDULT[4], stateDULT[5]);
   }
   else
      MessageInterface::ShowMessage
         ("stateDULT  = % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e\n", 
          stateDULT[0], stateDULT[1], stateDULT[2], stateDULT[3], stateDULT[4], stateDULT[5]);
}

