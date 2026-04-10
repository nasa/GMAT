//$Id$
//------------------------------------------------------------------------------
//                                  EphemWriterCode500
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
 * Code500 format.
 */
//------------------------------------------------------------------------------
#ifndef EphemWriterCode500_hpp
#define EphemWriterCode500_hpp

#include "EphemWriterWithInterpolator.hpp"
#include "Code500EphemerisFile.hpp"

class GMAT_API EphemWriterCode500 : public EphemWriterWithInterpolator
{
public:
   EphemWriterCode500(const std::string &name, const std::string &type = "EphemWriterCode500");
   virtual ~EphemWriterCode500();
   EphemWriterCode500(const EphemWriterCode500 &);
   EphemWriterCode500& operator=(const EphemWriterCode500&);
   
   virtual bool             Initialize();
   virtual EphemerisWriter* Clone(void) const;
   virtual void             Copy(const EphemerisWriter* orig);
   
protected:

   Code500EphemerisFile   *code500EphemFile; // owned object
   bool        code500WriteFailed;
   
   // Abstract methods required by all subclasses
   // virtual void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21]); 
   virtual void BufferOrbitData(Real epochInDays, const Real state[6], const Real cov[21],
                                const Real accel[3], const Real quat[4]);

   // Initialization
   virtual void CreateEphemerisFile(bool useDefaultFileName,
                                    const std::string &stType,
                                    const std::string &outFormat,
                                    const std::string &covFormat);
   void         CreateCode500EphemerisFile();
   
   // Data
   virtual bool NeedToHandleBackwardProp();
   
   virtual void HandleOrbitData();
   virtual void StartNewSegment(const std::string &comments,
                                bool saveEpochInfo,
                                bool writeAfterData,
                                bool ignoreBlankComments);
   virtual void FinishUpWriting();
   
   void         HandleCode500OrbitData(bool writeDatda, bool timeToWrite);
   void         FinishUpWritingCode500();
   
   // Code500 file writing
   void         SetCode500HeaderData();
   void         WriteCode500OrbitDataSegment(bool canFinish);
   void         FinalizeCode500Ephemeris();
   
};

#endif // EphemWriterCode500_hpp
