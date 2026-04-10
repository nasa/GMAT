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
// Created: 2016.01.29
//
/**
 * Writes a spacecraft orbit ephemeris to a file in STK format.
 */
//------------------------------------------------------------------------------
#ifndef STKEphemerisFile_hpp
#define STKEphemerisFile_hpp

#include "utildefs.hpp"
#include "Ephemeris.hpp"
#include "Rvector6.hpp"
#include "TimeSystemConverter.hpp"   // for TimeSystemConverter
#include <fstream>

class GMATUTIL_API STKEphemerisFile : public Ephemeris
{
public:
   /// class methods
   STKEphemerisFile();
   STKEphemerisFile(const STKEphemerisFile &copy);
   STKEphemerisFile& operator=(const STKEphemerisFile &copy);
   virtual ~STKEphemerisFile();

   
   struct GMATUTIL_API EphemData
   {
      Real timeFromEpoch;
      Real theState[6];
   };

   void InitializeData(); // Move to protected?
   
   /// Open the ephemeris file for reading/writing
   bool OpenForRead(const std::string &filename, const std::string &ephemType,
                    const std::string &ephemCovType = "");
   bool OpenForWrite(const std::string &filename, const std::string &ephemType,
                     const std::string &ephemCovType = "");
   void CloseForRead();
   void CloseForWrite();

   bool ReadDataRecords(int logOption = 0);
   void GetStartAndEndEpochs(GmatEpoch &startEpoch, GmatEpoch &endEpoch,
         std::vector<EphemData> **records);

   // For ephemeris file reading
   bool GetInitialAndFinalStates(Real &initialA1Mjd, Real &finalA1Mjd,
                                 Rvector6 &initialState, Rvector6 &finalState,
                                 std::string &cbName, std::string &csName);
   
   // For ephemeris file writing
   void SetVersion(const std::string &version);
   void SetInterpolationOrder(Integer order);
   bool SetHeaderForWriting(const std::string &fieldName,
                            const std::string &value);
   
   bool WriteHeader();
   bool WriteBlankLine();
   bool WriteString(const std::string &str);
   bool WriteDataSegment(const EpochArray &epochArray, const StateArray &stateArray,
                         const std::vector<Rvector*> &covArray, bool canFinalize = false);
   void FinalizeEphemeris();
   
   std::string GetDistanceUnit();
   void SetDistanceUnit(const std::string &dU);

   bool GetIncludeEventBoundaries();
   void SetIncludeEventBoundaries(bool iEB);

   // MOVE TO Ephemeris base class!!!
   std::string GetCentralBody();
   std::string GetEphemCoordinates();

protected:

   bool        firstTimeWriting;
   bool        openForTempOutput;
   
   bool        includeEventBoundaries;
   bool        writeFinalized;

   Real        scenarioEpochA1Mjd;
   Real        coordinateSystemEpochA1Mjd;
   Real        beginSegmentTime;
   Real        lastEpochWritten;
   // If we need to compare full states, this field is a start
//   Rvector6  lastDataWritten;
   
   RealArray   beginSegmentArray;
   Integer     numberOfSegmentPoints;
   Integer     numberOfEphemPoints;
   Integer     numberOfCovPoints;
   Integer     interpolationOrder;
   
   // Header fields
   std::string stkVersion;           // Required
   std::string scenarioEpochUtcGreg; // Required
   std::string interpolationMethod;
   std::string centralBody;
   std::string coordinateSystem;
   std::string coordinateSystemEpochStr;
   std::string distanceUnit;
   
   // The file name for read/write
   std::string stkFileNameForRead;
   std::string stkFileNameForWrite;
   std::string stkTempFileName;
   std::string stkTempCovFileName;
   
   // Ephemeris type for read/write
   std::string ephemTypeForRead;
   std::string ephemTypeForWrite;
   std::string ephemCovTypeForWrite;
   bool writeCov;
   
   // File position for updating number of ephem points
   std::ofstream::pos_type numEphemPointsBegPos;
   
    // File input/output streams
   std::ifstream stkInStream;
   std::ifstream stkCovInStream;
   std::ofstream stkOutStream;
   std::ofstream stkCovOutStream;
   
   // Epoch and state buffer for read/write
   std::vector<EphemData> ephemRecords;
   
   // Initial/Final epochs and states read from file
   Real          initialSecsFromEpoch;
   Real          finalSecsFromEpoch;
   Rvector6      initialState;
   Rvector6      finalState;
   
   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;

   // For ephemeris reading
   bool          GetEpochAndState(const std::string &line, Real &epoch, Rvector6 &state);
   std::string   GetLastLine();
   std::istream& IgnoreLine(std::ifstream::pos_type& pos);
   
   // For ephemeris writing
   void WriteTimePosVel(const EpochArray &epochArray, const StateArray &stateArray);
   void WriteTimePosVel(Real time, const Rvector6 *state);
   void WriteTimePos(const EpochArray &epochArray, const StateArray &stateArray);
   void WriteTimePos(Real time, const Rvector6 *state);

   void WriteCovTimePosVel(const EpochArray &epochArray, const std::vector<Rvector*> &covArray);
   void WriteCovTimePosVel(Real time, const Rvector *cov);
   void WriteCovTimePos(const EpochArray &epochArray, const std::vector<Rvector*> &covArray);
   void WriteCovTimePos(Real time, const Rvector *cov);
   
   // Time conversion
   std::string A1ModJulianToUtcGregorian(Real epochInDays, Integer format);
   bool        UTCGregorianToA1ModJulian(const std::string &utcGreg, Real &a1Mjd);
};

#endif // STKEphemerisFile_hpp
