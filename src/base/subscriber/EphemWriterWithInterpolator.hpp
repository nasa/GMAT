//$Id$
//------------------------------------------------------------------------------
//                               EphemWriterWithInterpolator
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
// Created: 2015.10.28
//
/**
 * Intermediate class to handle ephemeris file interpolation.
 */
//------------------------------------------------------------------------------
#ifndef EphemWriterWithInterpolator_hpp
#define EphemWriterWithInterpolator_hpp

#include "EphemerisWriter.hpp"
#include "Interpolator.hpp"


class GMAT_API EphemWriterWithInterpolator : public EphemerisWriter
{
public:
   EphemWriterWithInterpolator(const std::string &name, const std::string &type);
   virtual ~EphemWriterWithInterpolator();
   EphemWriterWithInterpolator(const EphemWriterWithInterpolator &);
   EphemWriterWithInterpolator& operator=(const EphemWriterWithInterpolator&);
   
   // // methods inherited from GmatBase
   // virtual GmatBase*    Clone(void) const;
   virtual void         Copy(const GmatBase* orig);
   
   
protected:

   Interpolator *interpolator;     // owned object
   
   Integer      interpolatorStatus;
   Integer      initialCount;
   Integer      waitCount;
   Integer      afterFinalEpochCount;
   RealArray    epochsOnWaiting;

   bool         isNextOutputEpochInLeapSecond;
   bool         handleFinalEpoch;
   bool         processingLargeStep;
   bool         checkForLargeTimeGap;
   bool         allowLowerOrder;
   /// Time tolerance used for checking time difference
   Real         timeTolerance;
   
   // Initialization
   void         InitializeData(bool saveEpochInfo = false);
   void         CreateInterpolator();
   
   // General data handling
   //virtual void FindNextOutputEpoch(Real reqEpochInSecs, Real &outEpochInSecs, 
   //                                 Real stateToWrite[6]);
   virtual void FindNextOutputEpoch(Real reqEpochInSecs, Real &outEpochInSecs, 
      Real stateToWrite[6], Real covToWrite[21], Real accelToWrite[3]);

   
   // Interpolation
   void         RestartInterpolation(const std::string &comments,
                                     bool saveEpochInfo,
                                     bool writeAfterData,
                                     bool ignoreBlankComments);
   // bool         IsTimeToWrite(Real epochInSecs, const Real state[6], const Real cov[21]);
   bool         IsTimeToWrite(Real epochInSecs, const Real state[6], const Real cov[21], const Real accel[3]);
   // void         WriteOrbitAt(Real reqEpochInSecs, const Real state[6], const Real cov[21]); 
   virtual void WriteOrbitAt(Real reqEpochInSecs, const Real state[6], const Real cov[21], const Real accel[3], const Real quat[4]);
   
   void         ProcessFinalDataOnWaiting(bool canFinish = true);
   void         ProcessEpochsOnWaiting(bool checkFinalEpoch,
                                       bool checkEventEpoch);
   
   // Epoch handling
   RealArray::iterator
                FindEpochOnWaiting(Real epochInSecs, const std::string &msg);
   void         RemoveEpochAlreadyWritten(Real epochInSecs, const std::string &msg);
   void         AddNextEpochToWrite(Real epochInSecs, const std::string &msg);
   
   // Error message formatting
   void         FormatErrorMessage(std::string &ephemMsg, std::string &errMsg);

   // Debug output
   void         DebugWriteEpochsOnWaiting(const std::string &msg = "");

};

#endif // EphemWriterWithInterpolator_hpp
