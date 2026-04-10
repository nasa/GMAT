//$Id$
//------------------------------------------------------------------------------
//                                  EphemWriterSTK
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
// Created: 2016.02.04
//
/**
 * Writes a spacecraft orbit states or attitude to an ephemeris file in
 * STK EphemerisTimePosVel format.
 */
//------------------------------------------------------------------------------
#ifndef EphemWriterSTK_hpp
#define EphemWriterSTK_hpp

#include "EphemWriterWithInterpolator.hpp"
#include "STKEphemerisFile.hpp"

class GMAT_API EphemWriterSTK : public EphemWriterWithInterpolator
{
public:
   EphemWriterSTK(const std::string &name, const std::string &type = "EphemWriterSTK");
   virtual ~EphemWriterSTK();
   EphemWriterSTK(const EphemWriterSTK &);
   EphemWriterSTK& operator=(const EphemWriterSTK&);
   
   virtual bool             Initialize();
   virtual EphemerisWriter* Clone(void) const;
   virtual void             Copy(const EphemerisWriter* orig);
   
   void         SetDistanceUnit(const std::string &dU);
   void         SetIncludeEventBoundaries(bool iEB);

protected:
   
   STKEphemerisFile *stkEphemFile; // owned object
   std::string      stkVersion;
   bool             stkWriteFailed;
   std::string      distanceUnit;
   bool             includeEventBoundaries;
   
   // Abstract methods required by all subclasses
   // virtual void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21]);
   virtual void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21],
                                const Real accel[3], const Real quat[4]);
   
   // Initialization
   virtual void CreateEphemerisFile(bool useDefaultFileName,
                                    const std::string &stType,
                                    const std::string &outFormat,
                                    const std::string &covFormat);
   void         CreateSTKEphemerisFile();
   
   // Data
   virtual bool NeedToHandleBackwardProp();
   
   virtual void HandleOrbitData();
   virtual void StartNewSegment(const std::string &comments,
                                bool saveEpochInfo,
                                bool writeAfterData,
                                bool ignoreBlankComments);
   virtual void FinishUpWriting();
   virtual void CloseEphemerisFile(bool done = true, bool writeMetaData = true);
   
   void         HandleSTKOrbitData(bool writeDatda, bool timeToWrite);
   void         FinishUpWritingSTK();
   
   // STK file writing
   void         WriteSTKOrbitDataSegment(bool canFinish);
   void         FinalizeSTKEphemeris();
};

#endif // EphemWriterSTK_hpp
