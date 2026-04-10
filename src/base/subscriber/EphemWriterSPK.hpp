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
// Created: 2009/09/02
//
/**
 * Writes a spacecraft orbit states or attitude to an ephemeris file in SPK format.
 */
//------------------------------------------------------------------------------
#ifndef EphemWriterSPK_hpp
#define EphemWriterSPK_hpp

#include "EphemerisWriter.hpp"

class SpiceOrbitKernelWriter;

class GMAT_API EphemWriterSPK : public EphemerisWriter
{
public:
   EphemWriterSPK(const std::string &name, const std::string &type = "EphemWriterSPK");
   virtual ~EphemWriterSPK();
   EphemWriterSPK(const EphemWriterSPK &);
   EphemWriterSPK& operator=(const EphemWriterSPK&);
   
   // methods for this class
   // Need to be able to close background SPKs and leave ready for appending
   // Finalization
   virtual void         CloseEphemerisFile(bool done = true, bool writeMetaData = true);
   
   virtual bool             Initialize();
   virtual EphemerisWriter* Clone(void) const;
   virtual void             Copy(const EphemerisWriter* orig);
   
protected:
   
   SpiceOrbitKernelWriter *spkWriter;        // owned object
   
   bool        spkWriteFailed;
   
   /// number of SPK segments that have been written
   Integer     numSPKSegmentsWritten;
   
   // Abstract methods required by all subclasses
   // virtual void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21]);
   virtual void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21],
                                const Real accel[3], const Real quat[4]);

   // Initialization
   virtual void CreateEphemerisFile(bool useDefaultFileName,
                                    const std::string &stType,
                                    const std::string &outFormat,
                                    const std::string &covFormat);
   void         CreateSpiceKernelWriter();
   
   // Data   
   virtual void HandleOrbitData();
   virtual void StartNewSegment(const std::string &comments,
                                bool saveEpochInfo,
                                bool writeAfterData,
                                bool ignoreBlankComments);
   virtual void FinishUpWriting();
   
   
   void         HandleWriteOrbit();
   void         HandleSpkOrbitData(bool writeData, bool timeToWrite);
   void         FinishUpWritingSPK();
   
   // General writing
   virtual void WriteHeader();
   virtual void WriteMetaData();
   virtual void WriteDataComments(const std::string &comments, bool isErrorMsg,
                              bool ignoreBlank = true, bool writeKeyword = true);
   
   // SPK file writing
   void         WriteSpkHeader(); // This is for debug
   void         WriteSpkOrbitDataSegment();
   void         WriteSpkOrbitMetaData();
   void         WriteSpkComments(const std::string &comments);
   void         FinalizeSpkFile(bool done = true, bool writeMetaData = true);
   
   // Event checking
   bool         IsEventFeasible(bool checkForNoData = true);
   
};

#endif // EphemWriterSPK_hpp
