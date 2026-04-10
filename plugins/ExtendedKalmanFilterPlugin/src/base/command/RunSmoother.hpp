//$Id$
//------------------------------------------------------------------------------
//                         RunSmoother
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2019/07/25
//
/**
 * Definition of the MissionControlSequence command that drives smoothing
 */
//------------------------------------------------------------------------------


#ifndef RunSmoother_hpp
#define RunSmoother_hpp

#include "kalman_defs.hpp"
#include "RunEstimator.hpp"
//#include "Smoother.hpp"


/**
 * Mission Control Sequence Command that runs smoothing
 *
 * This command interacts with a smoother to perform the smoothing process.
 * Each GMAT smoother performs smoothing by running a finite state machine
 * that implements a smoothing algorithm.  The RunSmoother command performs
 * command side actions required by the state machine during this process.
 */
class KALMAN_API RunSmoother : public RunEstimator
{
public:
   RunSmoother();
   virtual ~RunSmoother();
   RunSmoother(const RunSmoother& rs);
   RunSmoother& operator=(const RunSmoother& rs);

   virtual GmatBase* Clone() const;

   virtual bool Initialize();

protected:
   virtual void PrepareToEstimate();
   virtual void Propagate();
   virtual void Calculate();

   virtual bool IsSmootherPredicting();

   std::vector<GmatState> measStates;

private:
   void LoadSolveForsToFilterESM();
};

#endif /* RunSmoother_hpp */
