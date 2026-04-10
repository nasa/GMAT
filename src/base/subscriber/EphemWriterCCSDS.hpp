//$Id$
//------------------------------------------------------------------------------
//                                  EphemWriterCCSDS
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
 * CCSDS format.
 */
//------------------------------------------------------------------------------
#ifndef EphemWriterCCSDS_hpp
#define EphemWriterCCSDS_hpp

#include "EphemWriterWithInterpolator.hpp"
#include "CCSDSOEMWriter.hpp"
#include <iostream>
#include <fstream>


class GMAT_API EphemWriterCCSDS : public EphemWriterWithInterpolator
{
public:
   EphemWriterCCSDS(const std::string &name, const std::string &type = "EphemWriterCCSDS");
   virtual ~EphemWriterCCSDS();
   EphemWriterCCSDS(const EphemWriterCCSDS &);
   EphemWriterCCSDS& operator=(const EphemWriterCCSDS&);
   
   // Methods can be overridden by subclasses
   virtual void CreateEphemerisFile(bool useDefaultFileName,
                                    const std::string &stType,
                                    const std::string &outFormat,
                                    const std::string &covFormat);
   virtual void InitializeData(bool saveEpochInfo);
   virtual bool IsBackwardPropAllowed(Real propDirection);
   virtual void HandleOrbitData();
   virtual void StartNewSegment(const std::string &comments,
                                bool saveEpochInfo,
                                bool writeAfterData,
                                bool ignoreBlankComments);
   virtual void FinishUpWriting();
   virtual void CloseEphemerisFile(bool done = true, bool writeMetaData = true);
   
   virtual bool             Initialize();
   virtual EphemerisWriter* Clone(void) const;
   virtual void             Copy(const EphemerisWriter* orig);
   
   
protected:

   CCSDSOEMWriter *ccsdsOemWriter;
   
   std::string ccsdsEpochFormat;
   
   /// for meta data and block
   Real        metaDataStart;
   Real        metaDataStop;
   std::string metaDataStartStr;
   std::string metaDataStopStr;
   Integer     metaDataWriteOption;
   std::ofstream::pos_type metaDataBegPosition;
   std::ofstream::pos_type metaDataEndPosition;
   
   bool        continuousSegment;
   bool        firstTimeMetaData;
   bool        saveMetaDataStart;
   
   // Abstract methods required by all subclasses
   // virtual void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21]);
   virtual void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21],
                                const Real accel[3], const Real quat[4]);
   
   
   // Data
   bool         OpenCcsdsEphemerisFile();
   void         HandleCcsdsOrbitData(bool writeData, bool &timeToWrite);      
   void         FinishUpWritingOrbitData();
   void         FinishUpWritingAttitudeData();
   
   // General writing
   void         WriteHeader();
   void         WriteMetaData();
   void         WriteDataComments(const std::string &comments, bool writeCmtsNow,
                              bool ignoreBlanks = true, bool writeKeyword = true);
   
   // CCSDS file writing for debug and actual
   void         WriteCcsdsHeader();
   void         WriteCcsdsDataComments(const std::string &comments, bool writeCmtsNow,
                                       bool ignoreBlanks = true, bool writeKeyword = true);
   
   // OEM (Orbit Ephemeris Message)
   void         WriteCcsdsOrbitDataSegment();
   void         WriteCcsdsOemMetaData();
   void         WriteCcsdsOemData();
   void         ClearLastCcsdsOemMetaData(const std::string &comments = "");
   
   // AEM (Attitude Ephemeris Message)
   void         WriteCcsdsAemMetaData();
   void         WriteCcsdsAemData(Real reqEpochInSecs, const Real quat[4]);
};

#endif // EphemWriterCCSDS_hpp
