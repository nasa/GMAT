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
// Author: Tuan D. Nguyen / GSFC-NASA Code 583
// Created: 2021.07.19
//
/**
 * Writes a spacecraft orbit ephemeris to a file in CCSDS format.
 */
//------------------------------------------------------------------------------
#ifndef CCSDSEphemerisFile_hpp
#define CCSDSEphemerisFile_hpp

#include "utildefs.hpp"
#include "Ephemeris.hpp"
#include "Rvector6.hpp"
#include "CCSDSEMReader.hpp"
#include <fstream>

class GMATUTIL_API CCSDSEphemerisFile : public Ephemeris
{
public:
   /// class methods
   CCSDSEphemerisFile();
   CCSDSEphemerisFile(const CCSDSEphemerisFile &copy);
   CCSDSEphemerisFile& operator=(const CCSDSEphemerisFile &copy);
   virtual ~CCSDSEphemerisFile();


   struct GMATUTIL_API EphemData
   {
      Real timeFromEpoch;
      Real theState[6];
   };

   void InitializeData(); // Move to protected?

   /// Open the ephemeris file for reading/writing
   bool OpenForRead(const std::string &filename, const std::string &ephemType,
      const std::string &ephemCovType = "");

   void CloseForRead();

   bool ReadDataRecords(int logOption = 0);
   void GetStartAndEndEpochs(GmatEpoch &startEpoch, GmatEpoch &endEpoch,
      std::vector<EphemData> **records);

   // For ephemeris file reading
   bool GetInitialAndFinalStates(Real &initialA1Mjd, Real &finalA1Mjd,
      Rvector6 &initialState, Rvector6 &finalState,
      std::string &cbName, std::string &csName);


   // MOVE TO Ephemeris base class!!!
   std::string GetCentralBody(Integer segmentNum = -1);   // when segmentNum = -1, it means the current segment
   std::string GetReferenceFrame(Integer segmentNo = -1);     // when segmentNum = -1, it means the current segment

   virtual void GetInterpolationInfoFromSegment(Integer segmentNo);

protected:
   /// state type would be: "OEM", "AEM", or "". 
   /// ""   : unspecified state
   /// "OEM": (pos,vel) state
   /// "AEM": (atitute, atituteDot) state
   std::string    stateType;

   /// fileReader is either CCSDSOEMReader object or CCSDSAEMReader object
   CCSDSEMReader* fileReader;
   
   std::string ccsdsFileNameForRead;
   std::ifstream ccsdsInStream;

   // Epoch and state buffer for read/write
   std::vector<EphemData> ephemRecords;
   Real        scenarioEpochA1Mjd;

   std::string interpolatorName;
};

#endif // CCSDSEphemerisFile_hpp
